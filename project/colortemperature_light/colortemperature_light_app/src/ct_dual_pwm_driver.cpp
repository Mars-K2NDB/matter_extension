/*
 * Dual-channel PWM driver for Matter color-temperature lighting.
 * Pin Tool: TIMER0 CC0 -> PB4 (cool), TIMER1 CC0 -> PB5 (warm).
 * Cool/warm PWM instances are chosen from SL_PWM_PWMx_PERIPHERAL (TIMER0/TIMER1).
 */

#include "ct_dual_pwm_driver.h"
#include "ct_pwm_version.h"
#include "overcurrent_protector.h"

#include "sl_pwm_init_pwm0_config.h"
#include "sl_pwm_init_pwm1_config.h"
#include "sl_pwm_instances.h"

#include <em_gpio.h>
#include <em_timer.h>

#include "sl_gpio.h"

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/clusters/on-off-server/on-off-server.h>
#include <app/data-model/Nullable.h>
#include <clusters/ColorControl/AttributeIds.h>
#include <clusters/LevelControl/AttributeIds.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>
#include <silabs_utils.h>
#include <system/SystemClock.h>
#include <system/SystemLayer.h>

#include <algorithm>

bool CtDualPwmDriver::on_         = false;
uint8_t CtDualPwmDriver::level_     = 254;
uint16_t CtDualPwmDriver::ct_mireds_ = kDefaultCtMireds;
bool CtDualPwmDriver::pwm_started_        = false;
bool CtDualPwmDriver::route_disabled_     = false;
bool CtDualPwmDriver::pre_fault_saved_     = false;
bool CtDualPwmDriver::pre_fault_on_        = false;
uint8_t CtDualPwmDriver::pre_fault_level_    = 254;
uint16_t CtDualPwmDriver::pre_fault_ct_mireds_ = kDefaultCtMireds;
bool CtDualPwmDriver::last_on_valid_       = false;
uint8_t CtDualPwmDriver::last_on_level_     = 254;
uint16_t CtDualPwmDriver::last_on_ct_mireds_ = kDefaultCtMireds;

uint8_t CtDualPwmDriver::display_cool_duty_     = 0;
uint8_t CtDualPwmDriver::display_warm_duty_     = 0;
uint8_t CtDualPwmDriver::fade_start_brightness_  = 0;
uint8_t CtDualPwmDriver::fade_target_brightness_ = 0;
uint16_t CtDualPwmDriver::fade_start_warm_ratio_fp_  = 0;
uint16_t CtDualPwmDriver::fade_target_warm_ratio_fp_ = 0;
CtDualPwmDriver::FadeKind CtDualPwmDriver::fade_kind_ = FadeKind::kOnOff;
uint16_t CtDualPwmDriver::fade_step_           = 0;
uint16_t CtDualPwmDriver::fade_steps_total_     = 0;
bool CtDualPwmDriver::fade_active_             = false;

namespace {

constexpr uint32_t kFadeTickMs          = 10;
constexpr uint16_t kFadeDurationOnMs    = 500;
constexpr uint16_t kFadeDurationOffMs   = 400;
constexpr uint16_t kFadeDurationLevelMs = 350;
constexpr uint16_t kFadeDurationCtMs    = 600;

constexpr uint16_t kRatioFpOne = 65535U;

bool PolarityActiveHigh(const sl_pwm_instance_t * pwm)
{
    if (pwm == &sl_pwm_pwm0)
    {
        return SL_PWM_PWM0_POLARITY == PWM_ACTIVE_HIGH;
    }
    return SL_PWM_PWM1_POLARITY == PWM_ACTIVE_HIGH;
}

// Pin Tool maps SL_PWM_PWM0 -> TIMER0 (PB4), SL_PWM_PWM1 -> TIMER1 (PB5).
static sl_pwm_instance_t * const kPwmCool =
#if defined(SL_PWM_PWM0_PERIPHERAL) && defined(SL_PWM_PWM1_PERIPHERAL)
    (SL_PWM_PWM0_PERIPHERAL == TIMER0) ? &sl_pwm_pwm0 :
    (SL_PWM_PWM1_PERIPHERAL == TIMER0) ? &sl_pwm_pwm1 :
#endif
    &sl_pwm_pwm0;

static sl_pwm_instance_t * const kPwmWarm =
#if defined(SL_PWM_PWM0_PERIPHERAL) && defined(SL_PWM_PWM1_PERIPHERAL)
    (SL_PWM_PWM0_PERIPHERAL == TIMER1) ? &sl_pwm_pwm0 :
    (SL_PWM_PWM1_PERIPHERAL == TIMER1) ? &sl_pwm_pwm1 :
#endif
    &sl_pwm_pwm1;

uint16_t WarmRatioFpFromMireds(uint16_t mireds)
{
    const uint16_t ct = std::clamp(mireds, CtDualPwmDriver::kCtMinMireds, CtDualPwmDriver::kCtMaxMireds);
    const uint32_t warmWeight = ct - CtDualPwmDriver::kCtMinMireds;
    const uint32_t weightSum  = (CtDualPwmDriver::kCtMaxMireds - CtDualPwmDriver::kCtMinMireds);
    if (weightSum == 0)
    {
        return kRatioFpOne / 2U;
    }
    return static_cast<uint16_t>((warmWeight * kRatioFpOne) / weightSum);
}

uint16_t WarmRatioFpFromDuties(uint8_t coolDuty, uint8_t warmDuty)
{
    const uint32_t sum = static_cast<uint32_t>(coolDuty) + static_cast<uint32_t>(warmDuty);
    if (sum == 0)
    {
        return 0;
    }
    return static_cast<uint16_t>((static_cast<uint32_t>(warmDuty) * kRatioFpOne) / sum);
}

void DutiesFromBrightnessAndRatio(uint8_t brightness, uint16_t warmRatioFp, uint8_t& coolDuty, uint8_t& warmDuty)
{
    if (brightness == 0)
    {
        coolDuty = 0;
        warmDuty = 0;
        return;
    }

    warmDuty = static_cast<uint8_t>((static_cast<uint32_t>(brightness) * warmRatioFp) / kRatioFpOne);
    coolDuty = brightness - warmDuty;
}

uint32_t SmoothstepT(uint16_t step, uint16_t total)
{
    if (total == 0 || step >= total)
    {
        return kRatioFpOne;
    }

    const uint32_t t = (static_cast<uint32_t>(step) * kRatioFpOne) / total;
    const uint32_t t2 = (t * t) / kRatioFpOne;
    return (t2 * (3U * kRatioFpOne - 2U * t)) / kRatioFpOne;
}

uint8_t InterpolateBrightness(uint8_t from, uint8_t to, uint16_t step, uint16_t total)
{
    if (total == 0 || step >= total)
    {
        return to;
    }

    const uint32_t t = SmoothstepT(step, total);
    const int32_t delta = static_cast<int32_t>(to) - static_cast<int32_t>(from);
    return static_cast<uint8_t>(static_cast<int32_t>(from) + (delta * static_cast<int32_t>(t)) / static_cast<int32_t>(kRatioFpOne));
}

uint16_t InterpolateWarmRatioFp(uint16_t from, uint16_t to, uint16_t step, uint16_t total)
{
    if (total == 0 || step >= total)
    {
        return to;
    }

    const uint32_t t = SmoothstepT(step, total);
    const int32_t delta = static_cast<int32_t>(to) - static_cast<int32_t>(from);
    return static_cast<uint16_t>(static_cast<int32_t>(from) + (delta * static_cast<int32_t>(t)) / static_cast<int32_t>(kRatioFpOne));
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
    case 1:
        GPIO->TIMER1ROUTE_CLR[0].ROUTEEN = 1U << (pwm.channel + _GPIO_TIMER1_ROUTEEN_CC0PEN_SHIFT);
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

/** Glitch-free duty update: buffer only, loaded at next PWM period. */
void PwmSetDutyBuffered(const sl_pwm_instance_t & pwm, uint8_t percent)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
    const uint32_t top = TIMER_TopGet(pwm.timer);
    const uint32_t cmp = (top * static_cast<uint32_t>(percent)) / 100U;
    TIMER_CompareBufSet(pwm.timer, pwm.channel, cmp);
#else
    sl_pwm_set_duty_cycle(const_cast<sl_pwm_instance_t *>(&pwm), percent);
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

/** After route disable, hold PWM pin in off state (active-high -> GPIO low). */
void GpioForceOff(const sl_pwm_instance_t & pwm, bool activeHigh)
{
    sl_gpio_t gpio = {
        .port = pwm.port,
        .pin  = pwm.pin,
    };
    (void) sl_gpio_set_pin_mode(&gpio, SL_GPIO_MODE_PUSH_PULL, activeHigh ? 0 : 1);
}

void KillPwmInstance(sl_pwm_instance_t & pwm)
{
    PwmCompareZero(pwm);
    PwmRouteDisable(pwm);
    GpioForceOff(pwm, PolarityActiveHigh(&pwm));
}

/** Register-level PWM kill: compare 0, disconnect route, GPIO force off. */
void PwmOutputKillRegisters()
{
    KillPwmInstance(*kPwmCool);
    if (kPwmWarm != kPwmCool)
    {
        KillPwmInstance(*kPwmWarm);
    }
}

void SeedPwmCompareFromDuty(const sl_pwm_instance_t & pwm, uint8_t percent)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
    const uint32_t top = TIMER_TopGet(pwm.timer);
    const uint32_t cmp = (top * static_cast<uint32_t>(percent)) / 100U;
    TIMER_CompareSet(pwm.timer, pwm.channel, cmp);
    TIMER_CompareBufSet(pwm.timer, pwm.channel, cmp);
#else
    sl_pwm_set_duty_cycle(const_cast<sl_pwm_instance_t *>(&pwm), percent);
#endif
}

} // namespace

void RestorePwmInstance(sl_pwm_instance_t & pwm)
{
    RestorePwmPinMode(pwm, PolarityActiveHigh(&pwm));
    sl_pwm_start(&pwm);
}

void CtDualPwmDriver::PwmOutputRestoreRegisters()
{
    RestorePwmInstance(*kPwmCool);
    if (kPwmWarm != kPwmCool)
    {
        RestorePwmInstance(*kPwmWarm);
    }
    pwm_started_    = true;
    route_disabled_ = false;
}

void CtDualPwmDriver::SaveStateBeforeFault()
{
    if (pre_fault_saved_)
    {
        return;
    }

    pre_fault_on_       = on_;
    pre_fault_level_    = level_;
    pre_fault_ct_mireds_ = ct_mireds_;
    pre_fault_saved_    = true;
}

bool CtDualPwmDriver::GetPreFaultState(bool& on, uint8_t& level, uint16_t& ct_mireds)
{
    if (!pre_fault_saved_)
    {
        return false;
    }

    on       = pre_fault_on_;
    level    = pre_fault_level_;
    ct_mireds = pre_fault_ct_mireds_;
    return true;
}

void CtDualPwmDriver::CancelFadeTimer()
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

void CtDualPwmDriver::ComputeFadeTargets(uint8_t& brightness, uint16_t& warmRatioFp)
{
    brightness  = on_ ? LevelToBrightnessPercent(level_) : 0;
    warmRatioFp = WarmRatioFpFromMireds(ct_mireds_);
}

void CtDualPwmDriver::CaptureFadeStartFromDisplay()
{
    const uint8_t sum = static_cast<uint8_t>(display_cool_duty_ + display_warm_duty_);
    fade_start_brightness_ = sum;
    fade_start_warm_ratio_fp_ =
        (sum > 0) ? WarmRatioFpFromDuties(display_cool_duty_, display_warm_duty_) : WarmRatioFpFromMireds(ct_mireds_);
}

void CtDualPwmDriver::ApplyFadeFrame(uint16_t step)
{
    const uint8_t brightness = InterpolateBrightness(fade_start_brightness_, fade_target_brightness_, step, fade_steps_total_);
    const uint16_t warmRatioFp =
        InterpolateWarmRatioFp(fade_start_warm_ratio_fp_, fade_target_warm_ratio_fp_, step, fade_steps_total_);

    uint8_t coolDuty = 0;
    uint8_t warmDuty = 0;
    DutiesFromBrightnessAndRatio(brightness, warmRatioFp, coolDuty, warmDuty);
    ApplyDisplayDuties(coolDuty, warmDuty);
}

void CtDualPwmDriver::ApplyDisplayDuties(uint8_t coolDuty, uint8_t warmDuty)
{
    if (OvercurrentProtector::IsFaultActive())
    {
        PwmOutputKillRegisters();
        route_disabled_ = true;
        return;
    }

    if (route_disabled_ || !pwm_started_)
    {
        PwmOutputRestoreRegisters();
        SeedPwmCompareFromDuty(*kPwmCool, coolDuty);
        SeedPwmCompareFromDuty(*kPwmWarm, warmDuty);
    }
    else
    {
        PwmSetDutyBuffered(*kPwmCool, coolDuty);
        PwmSetDutyBuffered(*kPwmWarm, warmDuty);
    }

    display_cool_duty_ = coolDuty;
    display_warm_duty_ = warmDuty;

    ChipLogDetail(Zcl, "CtPwm duty: cool=%u warm=%u (on=%u level=%u ct=%u)", coolDuty, warmDuty, on_, level_, ct_mireds_);
}

void CtDualPwmDriver::ApplyOutputImmediate()
{
    CancelFadeTimer();

    uint8_t brightness  = 0;
    uint16_t warmRatioFp = 0;
    ComputeFadeTargets(brightness, warmRatioFp);

    uint8_t coolDuty = 0;
    uint8_t warmDuty = 0;
    DutiesFromBrightnessAndRatio(brightness, warmRatioFp, coolDuty, warmDuty);
    ApplyDisplayDuties(coolDuty, warmDuty);
}

void CtDualPwmDriver::ApplyProvisionReminderOutput(bool on)
{
    if (OvercurrentProtector::IsFaultActive())
    {
        return;
    }

    CancelFadeTimer();

    uint8_t coolDuty = 0;
    uint8_t warmDuty = 0;
    if (on)
    {
        DutiesFromBrightnessAndRatio(100, 0, coolDuty, warmDuty);
    }
    ApplyDisplayDuties(coolDuty, warmDuty);
}

void CtDualPwmDriver::ScheduleFade(FadeKind kind, bool restart_fade)
{
    if (OvercurrentProtector::IsFaultActive())
    {
        ApplyOutputImmediate();
        return;
    }

    uint8_t targetBrightness  = 0;
    uint16_t targetWarmRatioFp = 0;
    ComputeFadeTargets(targetBrightness, targetWarmRatioFp);

    if (kind == FadeKind::kCt && fade_active_ && fade_kind_ == FadeKind::kCt && !restart_fade)
    {
        fade_target_warm_ratio_fp_ = targetWarmRatioFp;
        fade_target_brightness_  = targetBrightness;

        const uint8_t sum = static_cast<uint8_t>(display_cool_duty_ + display_warm_duty_);
        if (sum == targetBrightness &&
            WarmRatioFpFromDuties(display_cool_duty_, display_warm_duty_) == targetWarmRatioFp)
        {
            CancelFadeTimer();
        }
        return;
    }

    CaptureFadeStartFromDisplay();
    fade_target_brightness_  = targetBrightness;
    fade_target_warm_ratio_fp_ = targetWarmRatioFp;
    fade_kind_              = kind;

    if (fade_start_brightness_ == fade_target_brightness_ && fade_start_warm_ratio_fp_ == fade_target_warm_ratio_fp_)
    {
        CancelFadeTimer();
        ApplyFadeFrame(fade_steps_total_);
        return;
    }

    uint16_t duration_ms = kFadeDurationLevelMs;
    switch (kind)
    {
    case FadeKind::kOnOff:
        duration_ms = on_ ? kFadeDurationOnMs : kFadeDurationOffMs;
        break;
    case FadeKind::kLevel:
        duration_ms = kFadeDurationLevelMs;
        break;
    case FadeKind::kCt:
        duration_ms = kFadeDurationCtMs;
        break;
    }

    const bool mustRestart = restart_fade || !fade_active_ || fade_kind_ != kind;

    if (mustRestart)
    {
        fade_step_       = 0;
        fade_steps_total_ = std::max<uint16_t>(duration_ms / static_cast<uint16_t>(kFadeTickMs), 1U);
        CancelFadeTimer();

        ApplyFadeFrame(0);

        chip::DeviceLayer::PlatformMgr().LockChipStack();
        const CHIP_ERROR err = chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::Milliseconds32(kFadeTickMs),
                                                                           OnFadeTimer, nullptr);
        chip::DeviceLayer::PlatformMgr().UnlockChipStack();

        if (err != CHIP_NO_ERROR)
        {
            ApplyOutputImmediate();
            return;
        }

        fade_active_ = true;
    }
}

void CtDualPwmDriver::OnFadeTimer(chip::System::Layer* layer, void* app_state)
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

    (void) chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::Milliseconds32(kFadeTickMs), OnFadeTimer,
                                                       nullptr);
}

void CtDualPwmDriver::RecoverFromFault()
{
    if (pre_fault_saved_)
    {
        RestoreToPreFault(pre_fault_on_, pre_fault_level_, pre_fault_ct_mireds_);
        return;
    }

    PwmOutputRestoreRegisters();
    ApplyOutputImmediate();
}

void CtDualPwmDriver::RestoreToPreFault(bool on, uint8_t level, uint16_t ct_mireds)
{
    on_            = on;
    level_         = level;
    ct_mireds_      = std::clamp(ct_mireds, kCtMinMireds, kCtMaxMireds);
    pre_fault_saved_ = false;

    PwmOutputRestoreRegisters();
    ApplyOutputImmediate();

    ChipLogProgress(AppServer, "CtPwm RECOVER: on=%u level=%u ct=%u", on_, level_, ct_mireds_);
}

void CtDualPwmDriver::LogVersion()
{
    SILABS_LOG("CtDualPwm driver v%s (TIMER0/PB4=cool, TIMER1/PB5=warm, mired %u-%u, fade)", CT_DUAL_PWM_DRIVER_VERSION,
               static_cast<unsigned>(kCtMinMireds), static_cast<unsigned>(kCtMaxMireds));
    ChipLogProgress(AppServer, "CtDualPwm driver v%s (TIMER0/PB4=cool, TIMER1/PB5=warm, fade)", CT_DUAL_PWM_DRIVER_VERSION);
}

void CtDualPwmDriver::Init()
{
    LogVersion();

    if (!pwm_started_)
    {
        sl_pwm_start(&sl_pwm_pwm0);
        sl_pwm_start(&sl_pwm_pwm1);
        pwm_started_ = true;
    }
    ApplyOutputImmediate();
}

void CtDualPwmDriver::ForceOffForFaultFromIsr()
{
    SaveStateBeforeFault();
    on_ = false;
    display_cool_duty_ = 0;
    display_warm_duty_ = 0;
    fade_active_        = false;
    PwmOutputKillRegisters();
    route_disabled_ = true;
}

void CtDualPwmDriver::ForceOffForFault()
{
    SaveStateBeforeFault();
    on_ = false;
    CancelFadeTimer();
    display_cool_duty_ = 0;
    display_warm_duty_ = 0;
    PwmOutputKillRegisters();
    route_disabled_ = true;
}

void CtDualPwmDriver::SetOn(bool on)
{
    if (on && OvercurrentProtector::BlocksTurnOn())
    {
        ChipLogProgress(AppServer, "CtPwm: on rejected (overcurrent fault)");
        return;
    }

    if (!on && on_)
    {
        last_on_valid_    = true;
        last_on_level_    = level_;
        last_on_ct_mireds_ = ct_mireds_;
    }

    if (on && !on_ && last_on_valid_)
    {
        level_    = last_on_level_;
        ct_mireds_ = last_on_ct_mireds_;
    }

    on_ = on;
    if (on && level_ <= 1)
    {
        level_ = 254;
        if (last_on_valid_)
        {
            last_on_level_ = level_;
        }
    }
    ScheduleFade(FadeKind::kOnOff, true);
    ChipLogProgress(Zcl, "CtPwm On -> %u (ct=%u level=%u)", on, ct_mireds_, level_);
}

void CtDualPwmDriver::SetLevel(uint8_t level)
{
    level_ = level;
    if (on_)
    {
        last_on_valid_ = true;
        last_on_level_ = level_;
    }
    if (!on_)
    {
        return;
    }
    ScheduleFade(FadeKind::kLevel, true);
}

void CtDualPwmDriver::ApplyClusterLevel(chip::EndpointId endpoint, uint8_t cluster_level)
{
    if (on_ && cluster_level <= 1)
    {
        return;
    }

    if (!on_)
    {
        level_ = cluster_level;
        return;
    }

    level_ = ResolveLevelForCluster(endpoint, true, cluster_level);
    ScheduleFade(FadeKind::kLevel, true);
}

void CtDualPwmDriver::SetColorTemperatureMireds(uint16_t mireds)
{
    ct_mireds_ = std::clamp(mireds, kCtMinMireds, kCtMaxMireds);
    if (on_)
    {
        last_on_valid_    = true;
        last_on_level_    = level_;
        last_on_ct_mireds_ = ct_mireds_;
    }
    if (!on_)
    {
        return;
    }

    // Matter MoveToColorTemperature streams attribute updates; chase target without restarting fade.
    ScheduleFade(FadeKind::kCt, false);
    ChipLogProgress(Zcl, "CtPwm CT -> %u mireds (on=%u level=%u)", mireds, on_, level_);
}

uint8_t CtDualPwmDriver::ResolveLevelForCluster(chip::EndpointId endpoint, bool on, uint8_t cluster_level)
{
    if (!on)
    {
        return 0;
    }

    uint8_t level = cluster_level;
    if (level > 1)
    {
        return level;
    }

    using namespace chip;
    using namespace chip::app;
    using namespace chip::app::Clusters;
    using namespace chip::Protocols::InteractionModel;

    DataModel::Nullable<uint8_t> onLevel;
    if (LevelControl::Attributes::OnLevel::Get(endpoint, onLevel) == Status::Success && !onLevel.IsNull() &&
        onLevel.Value() > level)
    {
        return onLevel.Value();
    }

    if (level_ > 1)
    {
        return level_;
    }

    return 254;
}

void CtDualPwmDriver::RefreshFromMatterEndpoint(chip::EndpointId endpoint)
{
    if (OvercurrentProtector::IsFaultActive())
    {
        on_ = false;
        ApplyOutputImmediate();
        return;
    }

    using namespace chip;
    using namespace chip::app;
    using namespace chip::app::Clusters;
    using namespace chip::Protocols::InteractionModel;

    bool on           = false;
    uint8_t level     = level_;
    uint16_t ct_mireds = ct_mireds_;

    OnOffServer::Instance().getOnOffValue(endpoint, &on);

    DataModel::Nullable<uint8_t> currentLevel;
    if (LevelControl::Attributes::CurrentLevel::Get(endpoint, currentLevel) == Status::Success && !currentLevel.IsNull())
    {
        level = ResolveLevelForCluster(endpoint, on, currentLevel.Value());
    }

    if (ColorControl::Attributes::ColorTemperatureMireds::Get(endpoint, &ct_mireds) != Status::Success)
    {
        ct_mireds = kDefaultCtMireds;
    }

    on_       = on;
    level_    = level;
    ct_mireds_ = ct_mireds;
    if (on_)
    {
        last_on_valid_    = true;
        last_on_level_    = level_;
        last_on_ct_mireds_ = ct_mireds_;
    }

    ApplyOutputImmediate();

    ChipLogProgress(Zcl, "CtPwm sync: on=%u level=%u ct=%u", on, level, ct_mireds);
}

void CtDualPwmDriver::SyncFromMatterEndpoint(chip::EndpointId endpoint)
{
    chip::DeviceLayer::PlatformMgr().LockChipStack();
    RefreshFromMatterEndpoint(endpoint);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
}

uint8_t CtDualPwmDriver::LevelToBrightnessPercent(uint8_t level)
{
    if (level == 0)
    {
        return 0;
    }
    return static_cast<uint8_t>(1U + (static_cast<uint32_t>(level - 1U) * 99U) / 253U);
}
