/*
 * Single-channel PWM driver for Matter dimmable light (PWM0 -> PB4).
 */

#include "SinglePwmDriver.h"
#include "OvercurrentProtector.h"

#include "sl_pwm_init_pwm0_config.h"
#include "sl_pwm_instances.h"

#include <em_gpio.h>
#include <em_timer.h>
#include <sl_gpio.h>

#include <app/clusters/on-off-server/on-off-server.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <clusters/LevelControl/AttributeIds.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>
#include <system/SystemClock.h>
#include <system/SystemLayer.h>

#include <algorithm>

bool SinglePwmDriver::sOn              = false;
uint8_t SinglePwmDriver::sLevel        = 254;
bool SinglePwmDriver::sPwmStarted      = false;
bool SinglePwmDriver::sRouteDisabled   = false;
bool SinglePwmDriver::sPreFaultSaved   = false;
bool SinglePwmDriver::sPreFaultOn      = false;
uint8_t SinglePwmDriver::sPreFaultLevel = 254;
uint8_t SinglePwmDriver::sDisplayDuty   = 0;
uint8_t SinglePwmDriver::sFadeStartDuty = 0;
uint8_t SinglePwmDriver::sFadeTargetDuty = 0;
uint16_t SinglePwmDriver::sFadeStep      = 0;
uint16_t SinglePwmDriver::sFadeStepsTotal = 0;
bool SinglePwmDriver::sFadeActive        = false;

namespace {

constexpr uint32_t kFadeTickMs         = 10;
constexpr uint16_t kFadeDurationOnMs   = 450;
constexpr uint16_t kFadeDurationOffMs  = 350;
constexpr uint16_t kFadeDurationLevelMs = 300;
constexpr uint32_t kFadeFpOne = 65535U;

uint32_t SmoothstepT(uint16_t step, uint16_t total)
{
    if (total == 0 || step >= total)
    {
        return kFadeFpOne;
    }

    const uint32_t t = (static_cast<uint32_t>(step) * kFadeFpOne) / total;
    const uint32_t t2 = (t * t) / kFadeFpOne;
    return (t2 * (3U * kFadeFpOne - 2U * t)) / kFadeFpOne;
}

uint8_t InterpolateDuty(uint8_t from, uint8_t to, uint16_t step, uint16_t total)
{
    if (total == 0 || step >= total)
    {
        return to;
    }

    const uint32_t t = SmoothstepT(step, total);
    const int32_t delta = static_cast<int32_t>(to) - static_cast<int32_t>(from);
    return static_cast<uint8_t>(static_cast<int32_t>(from) + (delta * static_cast<int32_t>(t)) / static_cast<int32_t>(kFadeFpOne));
}

void PwmRouteDisable(const sl_pwm_instance_t & pwm)
{
#if defined(_GPIO_TIMER_ROUTEEN_MASK)
    GPIO->TIMERROUTE_CLR[TIMER_NUM(pwm.timer)].ROUTEEN = 1U << (pwm.channel + _GPIO_TIMER_ROUTEEN_CC0PEN_SHIFT);
#else
    switch (TIMER_NUM(pwm.timer))
    {
    case 0:
        GPIO->TIMER0ROUTE_CLR[0].ROUTEEN = 1U << (pwm.channel + _GPIO_TIMER0_ROUTEEN_CC0PEN_SHIFT);
        break;
    case 4:
        GPIO->TIMER4ROUTE_CLR[0].ROUTEEN = 1U << (pwm.channel + _GPIO_TIMER4_ROUTEEN_CC0PEN_SHIFT);
        break;
    default:
        break;
    }
#endif
}

void PwmCompareZero(const sl_pwm_instance_t & pwm)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
    TIMER_CompareSet(pwm.timer, pwm.channel, 0U);
    TIMER_CompareBufSet(pwm.timer, pwm.channel, 0U);
#else
    sl_pwm_set_duty_cycle(const_cast<sl_pwm_instance_t *>(&pwm), 0);
#endif
}

void RestorePwmPinMode(const sl_pwm_instance_t & pwm, bool activeHigh)
{
    sl_gpio_t gpio = {
        .port = pwm.port,
        .pin  = pwm.pin,
    };
    (void) sl_gpio_set_pin_mode(&gpio, SL_GPIO_MODE_PUSH_PULL, activeHigh);
}

void GpioForceOff(const sl_pwm_instance_t & pwm, bool activeHigh)
{
    sl_gpio_t gpio = {
        .port = pwm.port,
        .pin  = pwm.pin,
    };
    (void) sl_gpio_set_pin_mode(&gpio, SL_GPIO_MODE_PUSH_PULL, activeHigh ? 0 : 1);
}

} // namespace

void SinglePwmDriver::PwmOutputKillRegisters()
{
    PwmCompareZero(sl_pwm_pwm0);
    PwmRouteDisable(sl_pwm_pwm0);
    GpioForceOff(sl_pwm_pwm0, SL_PWM_PWM0_POLARITY == PWM_ACTIVE_HIGH);
}

void SinglePwmDriver::PwmOutputRestoreRegisters()
{
    RestorePwmPinMode(sl_pwm_pwm0, SL_PWM_PWM0_POLARITY == PWM_ACTIVE_HIGH);
    sl_pwm_start(&sl_pwm_pwm0);
    sPwmStarted    = true;
    sRouteDisabled = false;
}

void SinglePwmDriver::SaveStateBeforeFault()
{
    if (sPreFaultSaved)
    {
        return;
    }

    sPreFaultOn    = sOn;
    sPreFaultLevel = sLevel;
    sPreFaultSaved = true;
}

void SinglePwmDriver::Init()
{
    sl_pwm_start(&sl_pwm_pwm0);
    sPwmStarted    = true;
    sRouteDisabled = false;
    SetOn(false);
}

uint8_t SinglePwmDriver::LevelToBrightnessPercent(uint8_t level)
{
    if (level == 0)
    {
        return 0;
    }
    return static_cast<uint8_t>((static_cast<uint32_t>(level) * 100U) / 254U);
}

void SinglePwmDriver::ApplyOutputImmediate()
{
    CancelFadeTimer();

    if (OvercurrentProtector::IsFaultActive())
    {
        PwmOutputKillRegisters();
        sRouteDisabled = true;
        return;
    }

    const uint8_t duty = sOn ? LevelToBrightnessPercent(sLevel) : 0;

    if (sRouteDisabled || !sPwmStarted)
    {
        PwmOutputRestoreRegisters();
    }

    sl_pwm_set_duty_cycle(&sl_pwm_pwm0, duty);
    sDisplayDuty = duty;
}

void SinglePwmDriver::CancelFadeTimer()
{
    if (!sFadeActive)
    {
        return;
    }

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    chip::DeviceLayer::SystemLayer().CancelTimer(OnFadeTimer, nullptr);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    sFadeActive = false;
}

void SinglePwmDriver::ApplyFadeFrame(uint16_t step)
{
    const uint8_t duty = InterpolateDuty(sFadeStartDuty, sFadeTargetDuty, step, sFadeStepsTotal);

    if (OvercurrentProtector::IsFaultActive())
    {
        PwmOutputKillRegisters();
        sRouteDisabled = true;
        return;
    }

    if (sRouteDisabled || !sPwmStarted)
    {
        PwmOutputRestoreRegisters();
    }

    sl_pwm_set_duty_cycle(&sl_pwm_pwm0, duty);
    sDisplayDuty = duty;
}

void SinglePwmDriver::OnFadeTimer(chip::System::Layer * layer, void * appState)
{
    (void) layer;
    (void) appState;

    if (!sFadeActive || OvercurrentProtector::IsFaultActive())
    {
        sFadeActive = false;
        return;
    }

    sFadeStep++;
    if (sFadeStep >= sFadeStepsTotal)
    {
        sFadeActive = false;
        ApplyFadeFrame(sFadeStepsTotal);
        return;
    }

    ApplyFadeFrame(sFadeStep);
    (void) chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::Milliseconds32(kFadeTickMs), OnFadeTimer, nullptr);
}

void SinglePwmDriver::ScheduleFade(bool restartFade)
{
    if (OvercurrentProtector::IsFaultActive())
    {
        ApplyOutputImmediate();
        return;
    }

    const uint8_t targetDuty = sOn ? LevelToBrightnessPercent(sLevel) : 0;
    sFadeTargetDuty = targetDuty;

    if (!sFadeActive || restartFade)
    {
        sFadeStartDuty = sDisplayDuty;
    }

    if (sFadeStartDuty == sFadeTargetDuty)
    {
        CancelFadeTimer();
        ApplyFadeFrame(sFadeStepsTotal);
        return;
    }

    const uint16_t durationMs = sOn ? (restartFade ? kFadeDurationOnMs : kFadeDurationLevelMs) : kFadeDurationOffMs;
    sFadeStep       = 0;
    sFadeStepsTotal = std::max<uint16_t>(durationMs / static_cast<uint16_t>(kFadeTickMs), 1U);

    CancelFadeTimer();
    ApplyFadeFrame(0);

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    const CHIP_ERROR err =
        chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::Milliseconds32(kFadeTickMs), OnFadeTimer, nullptr);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    if (err != CHIP_NO_ERROR)
    {
        ApplyOutputImmediate();
        return;
    }
    sFadeActive = true;
}

void SinglePwmDriver::SetOn(bool on)
{
    if (on && OvercurrentProtector::BlocksTurnOn())
    {
        ChipLogProgress(AppServer, "SinglePwm: on rejected (protection fault)");
        return;
    }

    sOn = on;
    if (on && sLevel <= 1)
    {
        sLevel = 254;
    }
    ScheduleFade(true);
}

void SinglePwmDriver::SetLevel(uint8_t level)
{
    sLevel = level;
    if (!sOn)
    {
        return;
    }
    ScheduleFade(false);
}

void SinglePwmDriver::ApplyClusterLevel(chip::EndpointId endpoint, uint8_t clusterLevel)
{
    chip::DeviceLayer::PlatformMgr().LockChipStack();
    const uint8_t level = ResolveLevelForPwmLocked(endpoint, sOn, clusterLevel);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    SetLevel(level);
}

uint8_t SinglePwmDriver::ResolveLevelForPwmLocked(chip::EndpointId endpoint, bool on, uint8_t clusterLevel)
{
    (void) endpoint;
    if (!on)
    {
        return sLevel;
    }
    if (clusterLevel == 0)
    {
        return 0;
    }
    if (clusterLevel == 1)
    {
        return 1;
    }
    return clusterLevel;
}

void SinglePwmDriver::SyncFromMatterEndpoint(chip::EndpointId endpoint)
{
    using namespace chip::app::Clusters;
    using namespace chip::app::DataModel;

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    bool on = false;
    uint8_t level = 254;
    OnOff::Attributes::OnOff::Get(endpoint, &on);
    Nullable<uint8_t> currentLevel;
    if (LevelControl::Attributes::CurrentLevel::Get(endpoint, currentLevel) == chip::Protocols::InteractionModel::Status::Success &&
        !currentLevel.IsNull())
    {
        level = currentLevel.Value();
    }
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    sOn    = on;
    sLevel = level;
    ApplyOutputImmediate();
}

void SinglePwmDriver::ForceOffForFaultFromIsr()
{
    SaveStateBeforeFault();
    sOn = false;
    sFadeActive = false;
    sDisplayDuty = 0;
    PwmOutputKillRegisters();
    sRouteDisabled = true;
}

void SinglePwmDriver::ForceOffForFault()
{
    SaveStateBeforeFault();
    CancelFadeTimer();
    sOn = false;
    sDisplayDuty = 0;
    PwmOutputKillRegisters();
    sRouteDisabled = true;
}

void SinglePwmDriver::RecoverFromFault()
{
    if (sPreFaultSaved)
    {
        RestoreToPreFault(sPreFaultOn, sPreFaultLevel);
        return;
    }

    PwmOutputRestoreRegisters();
    ApplyOutputImmediate();
}

void SinglePwmDriver::RestoreToPreFault(bool on, uint8_t level)
{
    sOn            = on;
    sLevel         = level;
    sPreFaultSaved = false;

    PwmOutputRestoreRegisters();
    ApplyOutputImmediate();

    ChipLogProgress(AppServer, "SinglePwm RECOVER: on=%u level=%u", sOn, sLevel);
}

bool SinglePwmDriver::GetPreFaultState(bool & on, uint8_t & level)
{
    if (!sPreFaultSaved)
    {
        return false;
    }
    on    = sPreFaultOn;
    level = sPreFaultLevel;
    return true;
}
