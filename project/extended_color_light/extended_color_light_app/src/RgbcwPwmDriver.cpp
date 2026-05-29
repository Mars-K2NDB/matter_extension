/*
 * Five-channel RGBCW PWM: R/G/B = pwm0/1/2 (PC0-2), C/W = pwm3/4 (PB4-5).
 */

#include "RgbcwPwmDriver.h"
#include "OvercurrentProtector.h"

#include "sl_pwm_init_pwm0_config.h"
#include "sl_pwm_init_pwm1_config.h"
#include "sl_pwm_init_pwm2_config.h"
#include "sl_pwm_init_pwm3_config.h"
#include "sl_pwm_init_pwm4_config.h"
#include "sl_pwm_instances.h"

#include "sl_gpio.h"

#include <em_gpio.h>
#include <em_timer.h>

#include <app-common/zap-generated/attributes/Accessors.h>
#include <clusters/ColorControl/AttributeIds.h>
#include <clusters/ColorControl/Enums.h>
#include <clusters/LevelControl/AttributeIds.h>
#include <clusters/OnOff/AttributeIds.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>
#include <system/SystemClock.h>
#include <system/SystemLayer.h>

#include <algorithm>

RgbcwPwmDriver::RuntimeState RgbcwPwmDriver::sRuntime{};
RgbcwPwmDriver::FaultSnapshot RgbcwPwmDriver::sFault{};
RgbcwPwmDriver::LastOnSnapshot RgbcwPwmDriver::sLastOn{};
RgbcwPwmDriver::OutputFrame RgbcwPwmDriver::sDisplay{};
RgbcwPwmDriver::FadeState RgbcwPwmDriver::sFade{};

namespace {
constexpr uint32_t kFadeTickMs          = 10;
constexpr uint16_t kFadeDurationOnMs    = 500;
constexpr uint16_t kFadeDurationOffMs   = 350;
constexpr uint16_t kFadeDurationLevelMs = 320;
constexpr uint16_t kFadeDurationColorMs = 650;
constexpr uint32_t kFadeFpOne = 65535U;

sl_pwm_instance_t * const kAllPwms[] = { &sl_pwm_pwm0, &sl_pwm_pwm1, &sl_pwm_pwm2, &sl_pwm_pwm3, &sl_pwm_pwm4 };

// SS6 PWM instance mapping (按实例号对应颜色通道):
//   pwm0/pwm1/pwm2 -> R/G/B
//   pwm3/pwm4     -> Cool/Warm
//
// 这里不依赖 SL_PWM_PWMx_OUTPUT_PORT/PIN（部分配置可能不生成），直接按 SS6 实例号取通道。
static sl_pwm_instance_t * const kPwmR    = &sl_pwm_pwm0;
static sl_pwm_instance_t * const kPwmG    = &sl_pwm_pwm1;
static sl_pwm_instance_t * const kPwmB    = &sl_pwm_pwm2;
static sl_pwm_instance_t * const kPwmCool = &sl_pwm_pwm3;
static sl_pwm_instance_t * const kPwmWarm = &sl_pwm_pwm4;

bool ActiveHighFor(const sl_pwm_instance_t * pwm)
{
    if (pwm == &sl_pwm_pwm0)
    {
        return SL_PWM_PWM0_POLARITY == PWM_ACTIVE_HIGH;
    }
    if (pwm == &sl_pwm_pwm1)
    {
        return SL_PWM_PWM1_POLARITY == PWM_ACTIVE_HIGH;
    }
    if (pwm == &sl_pwm_pwm2)
    {
        return SL_PWM_PWM2_POLARITY == PWM_ACTIVE_HIGH;
    }
    if (pwm == &sl_pwm_pwm3)
    {
        return SL_PWM_PWM3_POLARITY == PWM_ACTIVE_HIGH;
    }
    if (pwm == &sl_pwm_pwm4)
    {
        return SL_PWM_PWM4_POLARITY == PWM_ACTIVE_HIGH;
    }
    return true;
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
    case 2:
        GPIO->TIMER2ROUTE_CLR[0].ROUTEEN = 1U << (pwm.channel + _GPIO_TIMER2_ROUTEEN_CC0PEN_SHIFT);
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

void KillPwmInstance(sl_pwm_instance_t * pwm)
{
    if (pwm == nullptr)
    {
        return;
    }
    PwmCompareZero(*pwm);
    PwmRouteDisable(*pwm);
    GpioForceOff(*pwm, ActiveHighFor(pwm));
}

void RestorePwmInstance(sl_pwm_instance_t * pwm)
{
    if (pwm == nullptr)
    {
        return;
    }
    RestorePwmPinMode(*pwm, ActiveHighFor(pwm));
    sl_pwm_start(pwm);
}

void ForEachChannel(void (*fn)(sl_pwm_instance_t *))
{
    fn(kPwmR);
    fn(kPwmG);
    fn(kPwmB);
    fn(kPwmCool);
    fn(kPwmWarm);
}

void SetAllDuty(uint8_t r, uint8_t g, uint8_t b, uint8_t cool, uint8_t warm)
{
    sl_pwm_set_duty_cycle(kPwmR, r);
    sl_pwm_set_duty_cycle(kPwmG, g);
    sl_pwm_set_duty_cycle(kPwmB, b);
    sl_pwm_set_duty_cycle(kPwmCool, cool);
    sl_pwm_set_duty_cycle(kPwmWarm, warm);
}

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

void HsvToRgb(uint8_t hue, uint8_t sat, uint8_t brightness, uint8_t & r, uint8_t & g, uint8_t & b)
{
    if (brightness == 0 || sat == 0)
    {
        r = g = b = 0;
        return;
    }
    const float h = static_cast<float>(hue) * 360.0f / 254.0f;
    const float s = static_cast<float>(sat) / 254.0f;
    const float v = static_cast<float>(brightness) / 100.0f;
    const int hi = static_cast<int>(h / 60.0f) % 6;
    const float f  = h / 60.0f - static_cast<float>(hi);
    const float p  = v * (1.0f - s);
    const float q  = v * (1.0f - f * s);
    const float t  = v * (1.0f - (1.0f - f) * s);
    float rf = 0, gf = 0, bf = 0;
    switch (hi)
    {
    case 0:
        rf = v;
        gf = t;
        bf = p;
        break;
    case 1:
        rf = q;
        gf = v;
        bf = p;
        break;
    case 2:
        rf = p;
        gf = v;
        bf = t;
        break;
    case 3:
        rf = p;
        gf = q;
        bf = v;
        break;
    case 4:
        rf = t;
        gf = p;
        bf = v;
        break;
    default:
        rf = v;
        gf = p;
        bf = q;
        break;
    }
    r = static_cast<uint8_t>(rf * 100.0f);
    g = static_cast<uint8_t>(gf * 100.0f);
    b = static_cast<uint8_t>(bf * 100.0f);
}

void CtToCoolWarm(uint16_t mireds, uint8_t brightness, uint8_t & cool, uint8_t & warm)
{
    const uint16_t ct = std::clamp(mireds, RgbcwPwmDriver::kCtMinMireds, RgbcwPwmDriver::kCtMaxMireds);
    const uint32_t warmWeight = ct - RgbcwPwmDriver::kCtMinMireds;
    const uint32_t sum        = (RgbcwPwmDriver::kCtMaxMireds - RgbcwPwmDriver::kCtMinMireds);
    if (sum == 0)
    {
        cool = brightness / 2;
        warm = brightness - cool;
        return;
    }
    warm = static_cast<uint8_t>((brightness * warmWeight) / sum);
    cool = brightness - warm;
}

} // namespace

void RgbcwPwmDriver::SaveStateBeforeFault()
{
    if (sFault.saved)
    {
        return;
    }

    sFault.light.on       = sRuntime.light.on;
    sFault.light.level    = sRuntime.light.level;
    sFault.light.ctMireds = sRuntime.light.ctMireds;
    sFault.light.useCt    = sRuntime.light.useCt;
    sFault.light.hue      = sRuntime.light.hue;
    sFault.light.saturation      = sRuntime.light.saturation;
    sFault.saved    = true;
}

void RgbcwPwmDriver::PwmOutputKillRegisters()
{
    ForEachChannel(KillPwmInstance);
}

void RgbcwPwmDriver::PwmOutputRestoreRegisters()
{
    ForEachChannel(RestorePwmInstance);
    sRuntime.pwmStarted    = true;
    sRuntime.routeDisabled = false;
}

void RgbcwPwmDriver::Init()
{
    for (auto * pwm : kAllPwms)
    {
        sl_pwm_start(pwm);
    }
    sRuntime.pwmStarted    = true;
    sRuntime.routeDisabled = false;
    SetOn(false);
}

uint8_t RgbcwPwmDriver::LevelToBrightnessPercent(uint8_t level)
{
    if (level == 0)
    {
        return 0;
    }
    return static_cast<uint8_t>((static_cast<uint32_t>(level) * 100U) / 254U);
}

void RgbcwPwmDriver::ApplyOutputImmediate()
{
    CancelFadeTimer();

    if (OvercurrentProtector::IsFaultActive())
    {
        PwmOutputKillRegisters();
        sRuntime.routeDisabled = true;
        return;
    }

    if (sRuntime.routeDisabled || !sRuntime.pwmStarted)
    {
        PwmOutputRestoreRegisters();
    }

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t cool = 0;
    uint8_t warm = 0;
    ComputeTargetDuties(r, g, b, cool, warm);
    ApplyDisplayDuties(r, g, b, cool, warm);
}

void RgbcwPwmDriver::ComputeTargetDuties(uint8_t & r, uint8_t & g, uint8_t & b, uint8_t & cool, uint8_t & warm)
{
    r = 0;
    g = 0;
    b = 0;
    cool = 0;
    warm = 0;

    if (!sRuntime.light.on)
    {
        return;
    }

    const uint8_t br = LevelToBrightnessPercent(sRuntime.light.level);
    if (sRuntime.light.useCt)
    {
        CtToCoolWarm(sRuntime.light.ctMireds, br, cool, warm);
        return;
    }

    HsvToRgb(sRuntime.light.hue, sRuntime.light.saturation, br, r, g, b);
}

void RgbcwPwmDriver::ApplyDisplayDuties(uint8_t r, uint8_t g, uint8_t b, uint8_t cool, uint8_t warm)
{
    SetAllDuty(r, g, b, cool, warm);
    sDisplay.r    = r;
    sDisplay.g    = g;
    sDisplay.b    = b;
    sDisplay.cool = cool;
    sDisplay.warm = warm;
}

void RgbcwPwmDriver::CaptureLastOnState()
{
    sLastOn.valid = true;
    sLastOn.light = sRuntime.light;
}

void RgbcwPwmDriver::RestoreLastOnStateIfNeeded()
{
    if (sRuntime.light.on || !sLastOn.valid)
    {
        return;
    }
    sRuntime.light = sLastOn.light;
}

void RgbcwPwmDriver::SyncLastOnStateIfOn()
{
    if (!sRuntime.light.on)
    {
        return;
    }
    CaptureLastOnState();
}

void RgbcwPwmDriver::CancelFadeTimer()
{
    if (!sFade.active)
    {
        return;
    }

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    chip::DeviceLayer::SystemLayer().CancelTimer(OnFadeTimer, nullptr);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    sFade.active = false;
}

void RgbcwPwmDriver::ApplyFadeFrame(uint16_t step)
{
    if (OvercurrentProtector::IsFaultActive())
    {
        PwmOutputKillRegisters();
        sRuntime.routeDisabled = true;
        return;
    }

    if (sRuntime.routeDisabled || !sRuntime.pwmStarted)
    {
        PwmOutputRestoreRegisters();
    }

    ApplyDisplayDuties(InterpolateDuty(sFade.start.r, sFade.target.r, step, sFade.stepsTotal),
                       InterpolateDuty(sFade.start.g, sFade.target.g, step, sFade.stepsTotal),
                       InterpolateDuty(sFade.start.b, sFade.target.b, step, sFade.stepsTotal),
                       InterpolateDuty(sFade.start.cool, sFade.target.cool, step, sFade.stepsTotal),
                       InterpolateDuty(sFade.start.warm, sFade.target.warm, step, sFade.stepsTotal));
}

void RgbcwPwmDriver::OnFadeTimer(chip::System::Layer * layer, void * appState)
{
    (void) layer;
    (void) appState;

    if (!sFade.active || OvercurrentProtector::IsFaultActive())
    {
        sFade.active = false;
        return;
    }

    sFade.step++;
    if (sFade.step >= sFade.stepsTotal)
    {
        sFade.active = false;
        ApplyFadeFrame(sFade.stepsTotal);
        return;
    }

    ApplyFadeFrame(sFade.step);
    (void) chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::Milliseconds32(kFadeTickMs), OnFadeTimer, nullptr);
}

void RgbcwPwmDriver::ScheduleFade(FadeKind kind, bool restartFade)
{
    if (OvercurrentProtector::IsFaultActive())
    {
        ApplyOutputImmediate();
        return;
    }

    uint8_t targetR = 0;
    uint8_t targetG = 0;
    uint8_t targetB = 0;
    uint8_t targetCool = 0;
    uint8_t targetWarm = 0;
    ComputeTargetDuties(targetR, targetG, targetB, targetCool, targetWarm);

    sFade.target.r    = targetR;
    sFade.target.g    = targetG;
    sFade.target.b    = targetB;
    sFade.target.cool = targetCool;
    sFade.target.warm = targetWarm;

    if (kind == FadeKind::kColor && sFade.active && sFade.kind == FadeKind::kColor && !restartFade)
    {
        if (sDisplay.r == sFade.target.r && sDisplay.g == sFade.target.g && sDisplay.b == sFade.target.b &&
            sDisplay.cool == sFade.target.cool && sDisplay.warm == sFade.target.warm)
        {
            CancelFadeTimer();
        }
        return;
    }

    if (!sFade.active || restartFade || sFade.kind != kind)
    {
        sFade.start.r    = sDisplay.r;
        sFade.start.g    = sDisplay.g;
        sFade.start.b    = sDisplay.b;
        sFade.start.cool = sDisplay.cool;
        sFade.start.warm = sDisplay.warm;
    }

    sFade.kind = kind;

    if (sFade.start.r == sFade.target.r && sFade.start.g == sFade.target.g && sFade.start.b == sFade.target.b &&
        sFade.start.cool == sFade.target.cool && sFade.start.warm == sFade.target.warm)
    {
        CancelFadeTimer();
        ApplyFadeFrame(sFade.stepsTotal);
        return;
    }

    uint16_t durationMs = kFadeDurationLevelMs;
    if (kind == FadeKind::kOnOff)
    {
        durationMs = sRuntime.light.on ? kFadeDurationOnMs : kFadeDurationOffMs;
    }
    else if (kind == FadeKind::kColor)
    {
        durationMs = kFadeDurationColorMs;
    }

    sFade.step       = 0;
    sFade.stepsTotal = std::max<uint16_t>(durationMs / static_cast<uint16_t>(kFadeTickMs), 1U);

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
    sFade.active = true;
}

void RgbcwPwmDriver::SetOn(bool on)
{
    if (on && OvercurrentProtector::BlocksTurnOn())
    {
        ChipLogProgress(AppServer, "RgbcwPwm: on rejected (protection fault)");
        return;
    }

    if (!on && sRuntime.light.on)
    {
        CaptureLastOnState();
    }

    if (on)
    {
        RestoreLastOnStateIfNeeded();
    }

    sRuntime.light.on = on;
    if (on && sRuntime.light.level <= 1)
    {
        sRuntime.light.level = 254;
        if (sLastOn.valid)
        {
            sLastOn.light.level = sRuntime.light.level;
        }
    }
    ScheduleFade(FadeKind::kOnOff, true);
}

void RgbcwPwmDriver::SetLevel(uint8_t level)
{
    sRuntime.light.level = level;
    SyncLastOnStateIfOn();
    if (!sRuntime.light.on)
    {
        return;
    }
    ScheduleFade(FadeKind::kLevel, true);
}

void RgbcwPwmDriver::SetHueSat(uint8_t hue, uint8_t saturation)
{
    sRuntime.light.hue   = hue;
    sRuntime.light.saturation   = saturation;
    sRuntime.light.useCt = false;
    SyncLastOnStateIfOn();
    if (!sRuntime.light.on)
    {
        return;
    }
    ScheduleFade(FadeKind::kColor, false);
}

void RgbcwPwmDriver::SetColorTemperatureMireds(uint16_t mireds)
{
    sRuntime.light.ctMireds = std::clamp(mireds, kCtMinMireds, kCtMaxMireds);
    sRuntime.light.useCt    = true;
    SyncLastOnStateIfOn();
    if (!sRuntime.light.on)
    {
        return;
    }
    ScheduleFade(FadeKind::kColor, false);
}

void RgbcwPwmDriver::ApplyClusterLevel(chip::EndpointId endpoint, uint8_t clusterLevel)
{
    chip::DeviceLayer::PlatformMgr().LockChipStack();
    const uint8_t level = ResolveLevelForPwmLocked(endpoint, sRuntime.light.on, clusterLevel);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    SetLevel(level);
}

uint8_t RgbcwPwmDriver::ResolveLevelForPwmLocked(chip::EndpointId endpoint, bool on, uint8_t clusterLevel)
{
    (void) endpoint;
    if (!on)
    {
        return sRuntime.light.level;
    }
    if (clusterLevel <= 1)
    {
        return clusterLevel;
    }
    return clusterLevel;
}

void RgbcwPwmDriver::SyncFromMatterEndpoint(chip::EndpointId endpoint)
{
    using namespace chip::app::Clusters;

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    bool on = false;
    uint8_t level = 254;
    uint8_t hue = 0;
    uint8_t sat = 0;
    uint16_t ct = kDefaultCtMireds;
    ColorControl::ColorModeEnum colorMode = ColorControl::ColorModeEnum::kCurrentHueAndCurrentSaturation;

    OnOff::Attributes::OnOff::Get(endpoint, &on);
    chip::app::DataModel::Nullable<uint8_t> currentLevel;
    if (LevelControl::Attributes::CurrentLevel::Get(endpoint, currentLevel) == chip::Protocols::InteractionModel::Status::Success &&
        !currentLevel.IsNull())
    {
        level = currentLevel.Value();
    }
    ColorControl::Attributes::CurrentHue::Get(endpoint, &hue);
    ColorControl::Attributes::CurrentSaturation::Get(endpoint, &sat);
    ColorControl::Attributes::ColorTemperatureMireds::Get(endpoint, &ct);
    ColorControl::Attributes::ColorMode::Get(endpoint, &colorMode);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

    sRuntime.light.on    = on;
    sRuntime.light.level = level;
    sRuntime.light.hue   = hue;
    sRuntime.light.saturation   = sat;
    sRuntime.light.ctMireds = ct;
    sRuntime.light.useCt = (colorMode == ColorControl::ColorModeEnum::kColorTemperatureMireds);
    SyncLastOnStateIfOn();
    ApplyOutputImmediate();
}

void RgbcwPwmDriver::ForceOffForFaultFromIsr()
{
    SaveStateBeforeFault();
    sRuntime.light.on = false;
    sFade.active = false;
    sDisplay.r = 0;
    sDisplay.g = 0;
    sDisplay.b = 0;
    sDisplay.cool = 0;
    sDisplay.warm = 0;
    PwmOutputKillRegisters();
    sRuntime.routeDisabled = true;
}

void RgbcwPwmDriver::ForceOffForFault()
{
    SaveStateBeforeFault();
    CancelFadeTimer();
    sRuntime.light.on = false;
    sDisplay.r = 0;
    sDisplay.g = 0;
    sDisplay.b = 0;
    sDisplay.cool = 0;
    sDisplay.warm = 0;
    PwmOutputKillRegisters();
    sRuntime.routeDisabled = true;
}

void RgbcwPwmDriver::RecoverFromFault()
{
    if (sFault.saved)
    {
        RestoreToPreFault(sFault.light.on, sFault.light.level, sFault.light.ctMireds);
        return;
    }

    PwmOutputRestoreRegisters();
    ApplyOutputImmediate();
}

void RgbcwPwmDriver::RestoreToPreFault(bool on, uint8_t level, uint16_t ctMireds)
{
    sRuntime.light.on            = on;
    sRuntime.light.level         = level;
    sRuntime.light.ctMireds      = std::clamp(ctMireds, kCtMinMireds, kCtMaxMireds);
    sRuntime.light.useCt         = sFault.light.useCt;
    sRuntime.light.hue           = sFault.light.hue;
    sRuntime.light.saturation           = sFault.light.saturation;
    sFault.saved = false;

    PwmOutputRestoreRegisters();
    ApplyOutputImmediate();

    ChipLogProgress(AppServer, "RgbcwPwm RECOVER: on=%u level=%u ct=%u useCt=%u", sRuntime.light.on, sRuntime.light.level, sRuntime.light.ctMireds, sRuntime.light.useCt);
}

bool RgbcwPwmDriver::GetPreFaultState(bool & on, uint8_t & level, uint16_t & ctMireds)
{
    if (!sFault.saved)
    {
        return false;
    }
    on       = sFault.light.on;
    level    = sFault.light.level;
    ctMireds = sFault.light.ctMireds;
    return true;
}
