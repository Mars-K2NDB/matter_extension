/*
 * Dual-channel PWM driver for Matter color-temperature lighting.
 * Pin Tool: TIMER0 CC0 -> PB4 (cool), TIMER1 CC0 -> PB5 (warm).
 * Cool/warm PWM instances are chosen from SL_PWM_PWMx_PERIPHERAL (TIMER0/TIMER1).
 */

#include "CtDualPwmDriver.h"
#include "CtPwmVersion.h"
#include "OvercurrentProtector.h"

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

bool CtDualPwmDriver::sOn         = false;
uint8_t CtDualPwmDriver::sLevel     = 254;
uint16_t CtDualPwmDriver::sCtMireds = kDefaultCtMireds;
bool CtDualPwmDriver::sPwmStarted        = false;
bool CtDualPwmDriver::sRouteDisabled     = false;
bool CtDualPwmDriver::sPreFaultSaved     = false;
bool CtDualPwmDriver::sPreFaultOn        = false;
uint8_t CtDualPwmDriver::sPreFaultLevel    = 254;
uint16_t CtDualPwmDriver::sPreFaultCtMireds = kDefaultCtMireds;
bool CtDualPwmDriver::sLastOnValid       = false;
uint8_t CtDualPwmDriver::sLastOnLevel     = 254;
uint16_t CtDualPwmDriver::sLastOnCtMireds = kDefaultCtMireds;

uint8_t CtDualPwmDriver::sDisplayCoolDuty     = 0;
uint8_t CtDualPwmDriver::sDisplayWarmDuty     = 0;
uint8_t CtDualPwmDriver::sFadeStartBrightness  = 0;
uint8_t CtDualPwmDriver::sFadeTargetBrightness = 0;
uint16_t CtDualPwmDriver::sFadeStartWarmRatioFp  = 0;
uint16_t CtDualPwmDriver::sFadeTargetWarmRatioFp = 0;
CtDualPwmDriver::FadeKind CtDualPwmDriver::sFadeKind = FadeKind::kOnOff;
uint16_t CtDualPwmDriver::sFadeStep           = 0;
uint16_t CtDualPwmDriver::sFadeStepsTotal     = 0;
bool CtDualPwmDriver::sFadeActive             = false;

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

void DutiesFromBrightnessAndRatio(uint8_t brightness, uint16_t warmRatioFp, uint8_t & coolDuty, uint8_t & warmDuty)
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
    sPwmStarted    = true;
    sRouteDisabled = false;
}

void CtDualPwmDriver::SaveStateBeforeFault()
{
    if (sPreFaultSaved)
    {
        return;
    }

    sPreFaultOn       = sOn;
    sPreFaultLevel    = sLevel;
    sPreFaultCtMireds = sCtMireds;
    sPreFaultSaved    = true;
}

bool CtDualPwmDriver::GetPreFaultState(bool & on, uint8_t & level, uint16_t & ctMireds)
{
    if (!sPreFaultSaved)
    {
        return false;
    }

    on       = sPreFaultOn;
    level    = sPreFaultLevel;
    ctMireds = sPreFaultCtMireds;
    return true;
}

void CtDualPwmDriver::CancelFadeTimer()
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

void CtDualPwmDriver::ComputeFadeTargets(uint8_t & brightness, uint16_t & warmRatioFp)
{
    brightness  = sOn ? LevelToBrightnessPercent(sLevel) : 0;
    warmRatioFp = WarmRatioFpFromMireds(sCtMireds);
}

void CtDualPwmDriver::CaptureFadeStartFromDisplay()
{
    const uint8_t sum = static_cast<uint8_t>(sDisplayCoolDuty + sDisplayWarmDuty);
    sFadeStartBrightness = sum;
    sFadeStartWarmRatioFp =
        (sum > 0) ? WarmRatioFpFromDuties(sDisplayCoolDuty, sDisplayWarmDuty) : WarmRatioFpFromMireds(sCtMireds);
}

void CtDualPwmDriver::ApplyFadeFrame(uint16_t step)
{
    const uint8_t brightness = InterpolateBrightness(sFadeStartBrightness, sFadeTargetBrightness, step, sFadeStepsTotal);
    const uint16_t warmRatioFp =
        InterpolateWarmRatioFp(sFadeStartWarmRatioFp, sFadeTargetWarmRatioFp, step, sFadeStepsTotal);

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
        sRouteDisabled = true;
        return;
    }

    if (sRouteDisabled || !sPwmStarted)
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

    sDisplayCoolDuty = coolDuty;
    sDisplayWarmDuty = warmDuty;

    ChipLogDetail(Zcl, "CtPwm duty: cool=%u warm=%u (on=%u level=%u ct=%u)", coolDuty, warmDuty, sOn, sLevel, sCtMireds);
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

void CtDualPwmDriver::ScheduleFade(FadeKind kind, bool restartFade)
{
    if (OvercurrentProtector::IsFaultActive())
    {
        ApplyOutputImmediate();
        return;
    }

    uint8_t targetBrightness  = 0;
    uint16_t targetWarmRatioFp = 0;
    ComputeFadeTargets(targetBrightness, targetWarmRatioFp);

    if (kind == FadeKind::kCt && sFadeActive && sFadeKind == FadeKind::kCt && !restartFade)
    {
        sFadeTargetWarmRatioFp = targetWarmRatioFp;
        sFadeTargetBrightness  = targetBrightness;

        const uint8_t sum = static_cast<uint8_t>(sDisplayCoolDuty + sDisplayWarmDuty);
        if (sum == targetBrightness &&
            WarmRatioFpFromDuties(sDisplayCoolDuty, sDisplayWarmDuty) == targetWarmRatioFp)
        {
            CancelFadeTimer();
        }
        return;
    }

    CaptureFadeStartFromDisplay();
    sFadeTargetBrightness  = targetBrightness;
    sFadeTargetWarmRatioFp = targetWarmRatioFp;
    sFadeKind              = kind;

    if (sFadeStartBrightness == sFadeTargetBrightness && sFadeStartWarmRatioFp == sFadeTargetWarmRatioFp)
    {
        CancelFadeTimer();
        ApplyFadeFrame(sFadeStepsTotal);
        return;
    }

    uint16_t durationMs = kFadeDurationLevelMs;
    switch (kind)
    {
    case FadeKind::kOnOff:
        durationMs = sOn ? kFadeDurationOnMs : kFadeDurationOffMs;
        break;
    case FadeKind::kLevel:
        durationMs = kFadeDurationLevelMs;
        break;
    case FadeKind::kCt:
        durationMs = kFadeDurationCtMs;
        break;
    }

    const bool mustRestart = restartFade || !sFadeActive || sFadeKind != kind;

    if (mustRestart)
    {
        sFadeStep       = 0;
        sFadeStepsTotal = std::max<uint16_t>(durationMs / static_cast<uint16_t>(kFadeTickMs), 1U);
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

        sFadeActive = true;
    }
}

void CtDualPwmDriver::OnFadeTimer(chip::System::Layer * layer, void * appState)
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

    (void) chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::Milliseconds32(kFadeTickMs), OnFadeTimer,
                                                       nullptr);
}

void CtDualPwmDriver::RecoverFromFault()
{
    if (sPreFaultSaved)
    {
        RestoreToPreFault(sPreFaultOn, sPreFaultLevel, sPreFaultCtMireds);
        return;
    }

    PwmOutputRestoreRegisters();
    ApplyOutputImmediate();
}

void CtDualPwmDriver::RestoreToPreFault(bool on, uint8_t level, uint16_t ctMireds)
{
    sOn            = on;
    sLevel         = level;
    sCtMireds      = std::clamp(ctMireds, kCtMinMireds, kCtMaxMireds);
    sPreFaultSaved = false;

    PwmOutputRestoreRegisters();
    ApplyOutputImmediate();

    ChipLogProgress(AppServer, "CtPwm RECOVER: on=%u level=%u ct=%u", sOn, sLevel, sCtMireds);
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

    if (!sPwmStarted)
    {
        sl_pwm_start(&sl_pwm_pwm0);
        sl_pwm_start(&sl_pwm_pwm1);
        sPwmStarted = true;
    }
    ApplyOutputImmediate();
}

void CtDualPwmDriver::ForceOffForFaultFromIsr()
{
    SaveStateBeforeFault();
    sOn = false;
    sDisplayCoolDuty = 0;
    sDisplayWarmDuty = 0;
    sFadeActive        = false;
    PwmOutputKillRegisters();
    sRouteDisabled = true;
}

void CtDualPwmDriver::ForceOffForFault()
{
    SaveStateBeforeFault();
    sOn = false;
    CancelFadeTimer();
    sDisplayCoolDuty = 0;
    sDisplayWarmDuty = 0;
    PwmOutputKillRegisters();
    sRouteDisabled = true;
}

void CtDualPwmDriver::SetOn(bool on)
{
    if (on && OvercurrentProtector::BlocksTurnOn())
    {
        ChipLogProgress(AppServer, "CtPwm: on rejected (overcurrent fault)");
        return;
    }

    if (!on && sOn)
    {
        sLastOnValid    = true;
        sLastOnLevel    = sLevel;
        sLastOnCtMireds = sCtMireds;
    }

    if (on && !sOn && sLastOnValid)
    {
        sLevel    = sLastOnLevel;
        sCtMireds = sLastOnCtMireds;
    }

    sOn = on;
    if (on && sLevel <= 1)
    {
        sLevel = 254;
        if (sLastOnValid)
        {
            sLastOnLevel = sLevel;
        }
    }
    ScheduleFade(FadeKind::kOnOff, true);
    ChipLogProgress(Zcl, "CtPwm On -> %u (ct=%u level=%u)", on, sCtMireds, sLevel);
}

void CtDualPwmDriver::SetLevel(uint8_t level)
{
    sLevel = level;
    if (sOn)
    {
        sLastOnValid = true;
        sLastOnLevel = sLevel;
    }
    if (!sOn)
    {
        return;
    }
    ScheduleFade(FadeKind::kLevel, true);
}

void CtDualPwmDriver::ApplyClusterLevel(chip::EndpointId endpoint, uint8_t clusterLevel)
{
    if (sOn && clusterLevel <= 1)
    {
        return;
    }

    if (!sOn)
    {
        sLevel = clusterLevel;
        return;
    }

    sLevel = ResolveLevelForPwm(endpoint, true, clusterLevel);
    ScheduleFade(FadeKind::kLevel, true);
}

void CtDualPwmDriver::SetColorTemperatureMireds(uint16_t mireds)
{
    sCtMireds = std::clamp(mireds, kCtMinMireds, kCtMaxMireds);
    if (sOn)
    {
        sLastOnValid    = true;
        sLastOnLevel    = sLevel;
        sLastOnCtMireds = sCtMireds;
    }
    if (!sOn)
    {
        return;
    }

    // Matter MoveToColorTemperature streams attribute updates; chase target without restarting fade.
    ScheduleFade(FadeKind::kCt, false);
    ChipLogProgress(Zcl, "CtPwm CT -> %u mireds (on=%u level=%u)", mireds, sOn, sLevel);
}

uint8_t CtDualPwmDriver::ResolveLevelForPwmLocked(chip::EndpointId endpoint, bool on, uint8_t clusterLevel)
{
    if (!on)
    {
        return 0;
    }

    uint8_t level = clusterLevel;
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

    if (sLevel > 1)
    {
        return sLevel;
    }

    return 254;
}

uint8_t CtDualPwmDriver::ResolveLevelForPwm(chip::EndpointId endpoint, bool on, uint8_t clusterLevel)
{
    chip::DeviceLayer::PlatformMgr().LockChipStack();
    const uint8_t level = ResolveLevelForPwmLocked(endpoint, on, clusterLevel);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    return level;
}

void CtDualPwmDriver::RefreshFromMatterEndpoint(chip::EndpointId endpoint)
{
    if (OvercurrentProtector::IsFaultActive())
    {
        sOn = false;
        ApplyOutputImmediate();
        return;
    }

    using namespace chip;
    using namespace chip::app;
    using namespace chip::app::Clusters;
    using namespace chip::Protocols::InteractionModel;

    bool on           = false;
    uint8_t level     = sLevel;
    uint16_t ctMireds = sCtMireds;

    OnOffServer::Instance().getOnOffValue(endpoint, &on);

    DataModel::Nullable<uint8_t> currentLevel;
    if (LevelControl::Attributes::CurrentLevel::Get(endpoint, currentLevel) == Status::Success && !currentLevel.IsNull())
    {
        level = ResolveLevelForPwmLocked(endpoint, on, currentLevel.Value());
    }

    if (ColorControl::Attributes::ColorTemperatureMireds::Get(endpoint, &ctMireds) != Status::Success)
    {
        ctMireds = kDefaultCtMireds;
    }

    sOn       = on;
    sLevel    = level;
    sCtMireds = ctMireds;
    if (sOn)
    {
        sLastOnValid    = true;
        sLastOnLevel    = sLevel;
        sLastOnCtMireds = sCtMireds;
    }

    ApplyOutputImmediate();

    ChipLogProgress(Zcl, "CtPwm sync: on=%u level=%u ct=%u", on, level, ctMireds);
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
