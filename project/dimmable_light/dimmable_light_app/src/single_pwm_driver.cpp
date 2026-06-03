/*
 * Single-channel PWM driver for Matter dimmable light (PWM0 -> PB4).
 */

#include "single_pwm_driver.h"
#include "overcurrent_protector.h"

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

bool SinglePwmDriver::on_              = false;
uint8_t SinglePwmDriver::level_        = 254;
bool SinglePwmDriver::pwm_started_      = false;
bool SinglePwmDriver::route_disabled_   = false;
bool SinglePwmDriver::pre_fault_saved_   = false;
bool SinglePwmDriver::pre_fault_on_      = false;
uint8_t SinglePwmDriver::pre_fault_level_ = 254;
uint8_t SinglePwmDriver::display_duty_   = 0;
uint8_t SinglePwmDriver::fade_start_duty_ = 0;
uint8_t SinglePwmDriver::fade_target_duty_ = 0;
uint16_t SinglePwmDriver::fade_step_      = 0;
uint16_t SinglePwmDriver::fade_step_sTotal = 0;
bool SinglePwmDriver::fade_active_        = false;

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
    pwm_started_    = true;
    route_disabled_ = false;
}

void SinglePwmDriver::SaveStateBeforeFault()
{
    if (pre_fault_saved_)
    {
        return;
    }

    pre_fault_on_    = on_;
    pre_fault_level_ = level_;
    pre_fault_saved_ = true;
}

void SinglePwmDriver::Init()
{
    sl_pwm_start(&sl_pwm_pwm0);
    pwm_started_    = true;
    route_disabled_ = false;
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
        route_disabled_ = true;
        return;
    }

    const uint8_t duty = on_ ? LevelToBrightnessPercent(level_) : 0;

    if (route_disabled_ || !pwm_started_)
    {
        PwmOutputRestoreRegisters();
    }

    sl_pwm_set_duty_cycle(&sl_pwm_pwm0, duty);
    display_duty_ = duty;
}

void SinglePwmDriver::CancelFadeTimer()
{
    if (!fade_active_)
    {
        return;
    }

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    chip::DeviceLayer::SystemLayer().CancelTimer(OnFadeTimer, nullptr);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    fade_active_ = false;
}

void SinglePwmDriver::ApplyFadeFrame(uint16_t step)
{
    const uint8_t duty = InterpolateDuty(fade_start_duty_, fade_target_duty_, step, fade_steps_total_);

    if (OvercurrentProtector::IsFaultActive())
    {
        PwmOutputKillRegisters();
        route_disabled_ = true;
        return;
    }

    if (route_disabled_ || !pwm_started_)
    {
        PwmOutputRestoreRegisters();
    }

    sl_pwm_set_duty_cycle(&sl_pwm_pwm0, duty);
    display_duty_ = duty;
}

void SinglePwmDriver::OnFadeTimer(chip::System::Layer* layer, void* app_state)
{
    (void) layer;
    (void) app_state;

    if (!fade_active_ || OvercurrentProtector::IsFaultActive())
    {
        fade_active_ = false;
        return;
    }

    fade_step_++;
    if (fade_step_ >= fade_steps_total_)
    {
        fade_active_ = false;
        ApplyFadeFrame(fade_steps_total_);
        return;
    }

    ApplyFadeFrame(fade_step_);
    (void) chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::Milliseconds32(kFadeTickMs), OnFadeTimer, nullptr);
}

void SinglePwmDriver::ScheduleFade(bool restart_fade)
{
    if (OvercurrentProtector::IsFaultActive())
    {
        ApplyOutputImmediate();
        return;
    }

    const uint8_t targetDuty = on_ ? LevelToBrightnessPercent(level_) : 0;
    fade_target_duty_ = targetDuty;

    if (!fade_active_ || restart_fade)
    {
        fade_start_duty_ = display_duty_;
    }

    if (fade_start_duty_ == fade_target_duty_)
    {
        CancelFadeTimer();
        ApplyFadeFrame(fade_steps_total_);
        return;
    }

    const uint16_t duration_ms = on_ ? (restart_fade ? kFadeDurationOnMs : kFadeDurationLevelMs) : kFadeDurationOffMs;
    fade_step_       = 0;
    fade_steps_total_ = std::max<uint16_t>(duration_ms / static_cast<uint16_t>(kFadeTickMs), 1U);

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
    fade_active_ = true;
}

void SinglePwmDriver::SetOn(bool on)
{
    if (on && OvercurrentProtector::BlocksTurnOn())
    {
        ChipLogProgress(AppServer, "SinglePwm: on rejected (protection fault)");
        return;
    }

    on_ = on;
    if (on && level_ <= 1)
    {
        level_ = 254;
    }
    ScheduleFade(true);
}

void SinglePwmDriver::SetLevel(uint8_t level)
{
    level_ = level;
    if (!on_)
    {
        return;
    }
    ScheduleFade(false);
}

void SinglePwmDriver::ApplyClusterLevel(chip::EndpointId endpoint, uint8_t cluster_level)
{
    chip::DeviceLayer::PlatformMgr().LockChipStack();
    const uint8_t level = ResolveLevelForCluster(endpoint, on_, cluster_level);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    SetLevel(level);
}

uint8_t SinglePwmDriver::ResolveLevelForCluster(chip::EndpointId endpoint, bool on, uint8_t cluster_level)
{
    (void) endpoint;
    if (!on)
    {
        return level_;
    }
    if (cluster_level == 0)
    {
        return 0;
    }
    if (cluster_level == 1)
    {
        return 1;
    }
    return cluster_level;
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
    on_    = on;
    level_ = level;
    ApplyOutputImmediate();
}

void SinglePwmDriver::ForceOffForFaultFromIsr()
{
    SaveStateBeforeFault();
    on_ = false;
    fade_active_ = false;
    display_duty_ = 0;
    PwmOutputKillRegisters();
    route_disabled_ = true;
}

void SinglePwmDriver::ForceOffForFault()
{
    SaveStateBeforeFault();
    CancelFadeTimer();
    on_ = false;
    display_duty_ = 0;
    PwmOutputKillRegisters();
    route_disabled_ = true;
}

void SinglePwmDriver::RecoverFromFault()
{
    if (pre_fault_saved_)
    {
        RestoreToPreFault(pre_fault_on_, pre_fault_level_);
        return;
    }

    PwmOutputRestoreRegisters();
    ApplyOutputImmediate();
}

void SinglePwmDriver::RestoreToPreFault(bool on, uint8_t level)
{
    on_            = on;
    level_         = level;
    pre_fault_saved_ = false;

    PwmOutputRestoreRegisters();
    ApplyOutputImmediate();

    ChipLogProgress(AppServer, "SinglePwm RECOVER: on=%u level=%u", on_, level_);
}

bool SinglePwmDriver::GetPreFaultState(bool& on, uint8_t& level)
{
    if (!pre_fault_saved_)
    {
        return false;
    }
    on    = pre_fault_on_;
    level = pre_fault_level_;
    return true;
}
