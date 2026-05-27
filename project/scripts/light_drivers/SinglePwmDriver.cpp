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

bool SinglePwmDriver::sOn              = false;
uint8_t SinglePwmDriver::sLevel        = 254;
bool SinglePwmDriver::sPwmStarted      = false;
bool SinglePwmDriver::sRouteDisabled   = false;
bool SinglePwmDriver::sPreFaultSaved   = false;
bool SinglePwmDriver::sPreFaultOn      = false;
uint8_t SinglePwmDriver::sPreFaultLevel = 254;

namespace {

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
    ApplyOutputImmediate();
}

void SinglePwmDriver::SetLevel(uint8_t level)
{
    sLevel = level;
    ApplyOutputImmediate();
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
    PwmOutputKillRegisters();
    sRouteDisabled = true;
}

void SinglePwmDriver::ForceOffForFault()
{
    SaveStateBeforeFault();
    sOn = false;
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
