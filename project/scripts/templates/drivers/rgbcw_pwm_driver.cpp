/*
 * Five-channel RGBCW PWM: R/G/B = pwm0/1/2 (PC0-2), C/W = pwm3/4 (PB4-5).
 */

#include "rgbcw_pwm_driver.h"
#include "overcurrent_protector.h"

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

RgbcwPwmDriver::RuntimeState RgbcwPwmDriver::runtime_{};
RgbcwPwmDriver::FaultSnapshot RgbcwPwmDriver::fault_{};
RgbcwPwmDriver::LastOnSnapshot RgbcwPwmDriver::last_on_{};
RgbcwPwmDriver::OutputFrame RgbcwPwmDriver::display_{};
RgbcwPwmDriver::FadeState RgbcwPwmDriver::fade_{};

namespace {
constexpr uint32_t kFadeTickMs          = 10;
constexpr uint16_t kFadeDurationOnMs    = 500;
constexpr uint16_t kFadeDurationOffMs   = 350;
constexpr uint16_t kFadeDurationLevelMs = 320;
constexpr uint16_t kFadeDurationColorMs = 650;
constexpr uint16_t kFadeDurationCtMs    = 600;
constexpr uint32_t kFadeFpOne = 65535U;

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
    sl_gpio_t gpio = { .port = pwm.port, .pin = pwm.pin };
    (void) sl_gpio_set_pin_mode(&gpio, SL_GPIO_MODE_PUSH_PULL, activeHigh);
}

void GpioForceOff(const sl_pwm_instance_t & pwm, bool activeHigh)
{
    sl_gpio_t gpio = { .port = pwm.port, .pin = pwm.pin };
    (void) sl_gpio_set_pin_mode(&gpio, SL_GPIO_MODE_PUSH_PULL, activeHigh ? 0 : 1);
}

void KillPwmInstance(sl_pwm_instance_t * pwm)
{
    PwmCompareZero(*pwm);
    PwmRouteDisable(*pwm);
    GpioForceOff(*pwm, ActiveHighFor(pwm));
}

void RestorePwmInstance(sl_pwm_instance_t * pwm)
{
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

    const uint32_t t  = (static_cast<uint32_t>(step) * kFadeFpOne) / total;
    const uint32_t t2 = (t * t) / kFadeFpOne;
    return (t2 * (3U * kFadeFpOne - 2U * t)) / kFadeFpOne;
}

template <typename T>
T InterpolateScalar(T from, T to, uint16_t step, uint16_t total)
{
    if (total == 0 || step >= total)
    {
        return to;
    }

    const uint32_t t    = SmoothstepT(step, total);
    const int32_t delta = static_cast<int32_t>(to) - static_cast<int32_t>(from);
    return static_cast<T>(static_cast<int32_t>(from) +
                          (delta * static_cast<int32_t>(t)) / static_cast<int32_t>(kFadeFpOne));
}

uint16_t WarmRatioFpFromMireds(uint16_t mireds)
{
    const uint16_t ct = std::clamp(mireds, RgbcwPwmDriver::kCtMinMireds, RgbcwPwmDriver::kCtMaxMireds);
    const uint32_t warmWeight = ct - RgbcwPwmDriver::kCtMinMireds;
    const uint32_t weightSum  = (RgbcwPwmDriver::kCtMaxMireds - RgbcwPwmDriver::kCtMinMireds);
    if (weightSum == 0)
    {
        return kFadeFpOne / 2U;
    }
    return static_cast<uint16_t>((warmWeight * kFadeFpOne) / weightSum);
}

uint16_t WarmRatioFpFromDuties(uint8_t coolDuty, uint8_t warmDuty)
{
    const uint32_t sum = static_cast<uint32_t>(coolDuty) + static_cast<uint32_t>(warmDuty);
    if (sum == 0)
    {
        return 0;
    }
    return static_cast<uint16_t>((static_cast<uint32_t>(warmDuty) * kFadeFpOne) / sum);
}

void DutiesFromBrightnessAndRatio(uint8_t brightness, uint16_t warmRatioFp, uint8_t& coolDuty, uint8_t& warmDuty)
{
    if (brightness == 0)
    {
        coolDuty = 0;
        warmDuty = 0;
        return;
    }

    const uint32_t warmProduct = static_cast<uint32_t>(brightness) * warmRatioFp;
    const uint32_t coolProduct = static_cast<uint32_t>(brightness) * (kFadeFpOne - warmRatioFp);
    warmDuty                   = static_cast<uint8_t>(warmProduct / kFadeFpOne);
    coolDuty                   = static_cast<uint8_t>(coolProduct / kFadeFpOne);

    const uint8_t assigned = static_cast<uint8_t>(warmDuty + coolDuty);
    if (assigned < brightness)
    {
        const uint8_t remainder = static_cast<uint8_t>(brightness - assigned);
        if ((warmProduct % kFadeFpOne) >= (coolProduct % kFadeFpOne))
        {
            warmDuty = static_cast<uint8_t>(warmDuty + remainder);
        }
        else
        {
            coolDuty = static_cast<uint8_t>(coolDuty + remainder);
        }
    }
}

bool IsCtColorMode(chip::app::Clusters::ColorControl::ColorModeEnum color_mode,
                   chip::app::Clusters::ColorControl::EnhancedColorModeEnum enhanced_mode)
{
    using chip::app::Clusters::ColorControl::ColorModeEnum;
    using chip::app::Clusters::ColorControl::EnhancedColorModeEnum;
    return (color_mode == ColorModeEnum::kColorTemperatureMireds) ||
           (enhanced_mode == EnhancedColorModeEnum::kColorTemperatureMireds);
}

void HsvToRgb(uint8_t hue, uint8_t sat, uint8_t brightness, uint8_t& r, uint8_t& g, uint8_t& b)
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

} // namespace

void RgbcwPwmDriver::SaveStateBeforeFault()
{
    if (fault_.saved)
    {
        return;
    }

    fault_.light = runtime_.light;
    fault_.saved = true;
}

void RgbcwPwmDriver::PwmOutputKillRegisters()
{
    ForEachChannel(KillPwmInstance);
}

void RgbcwPwmDriver::PwmOutputRestoreRegisters()
{
    ForEachChannel(RestorePwmInstance);
    runtime_.pwm_started    = true;
    runtime_.route_disabled = false;
}

void RgbcwPwmDriver::Init()
{
    runtime_.light.on = false;
    PwmOutputRestoreRegisters();
    ApplyOutputImmediate();
}

uint8_t RgbcwPwmDriver::LevelToBrightnessPercent(uint8_t level)
{
    if (level == 0)
    {
        return 0;
    }
    return static_cast<uint8_t>(1U + (static_cast<uint32_t>(level - 1U) * 99U) / 253U);
}

void RgbcwPwmDriver::ApplyOutputImmediate()
{
    CancelFadeTimer();
    if (!EnsurePwmReadyForOutput())
    {
        return;
    }

    OutputFrame frame{};
    ComputeTargetDuties(frame.r, frame.g, frame.b, frame.cool, frame.warm);
    ApplyDisplayDuties(frame.r, frame.g, frame.b, frame.cool, frame.warm);
}

void RgbcwPwmDriver::ApplyProvisionReminderOutput(bool on)
{
    if (OvercurrentProtector::IsFaultActive())
    {
        return;
    }

    CancelFadeTimer();
    if (!EnsurePwmReadyForOutput())
    {
        return;
    }

    uint8_t cool = 0;
    uint8_t warm = 0;
    if (on)
    {
        DutiesFromBrightnessAndRatio(100, 0, cool, warm);
    }
    ApplyDisplayDuties(0, 0, 0, cool, warm);
}

void RgbcwPwmDriver::ComputeTargetDuties(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& cool, uint8_t& warm)
{
    r = 0;
    g = 0;
    b = 0;
    cool = 0;
    warm = 0;

    if (!runtime_.light.on)
    {
        return;
    }

    const uint8_t br = LevelToBrightnessPercent(runtime_.light.level);
    if (runtime_.light.use_ct)
    {
        DutiesFromBrightnessAndRatio(br, WarmRatioFpFromMireds(runtime_.light.ct_mireds), cool, warm);
        return;
    }

    HsvToRgb(runtime_.light.hue, runtime_.light.saturation, br, r, g, b);
}

void RgbcwPwmDriver::ApplyDisplayDuties(uint8_t r, uint8_t g, uint8_t b, uint8_t cool, uint8_t warm)
{
    if (runtime_.light.use_ct)
    {
        r = 0;
        g = 0;
        b = 0;
    }

    SetAllDuty(r, g, b, cool, warm);
    display_ = { r, g, b, cool, warm };
}

void RgbcwPwmDriver::RestoreLastOnStateIfNeeded()
{
    if (runtime_.light.on || !last_on_.valid)
    {
        return;
    }
    runtime_.light = last_on_.light;
}

void RgbcwPwmDriver::SyncLastOnStateIfOn()
{
    if (runtime_.light.on)
    {
        last_on_ = { true, runtime_.light };
    }
}

void RgbcwPwmDriver::CancelFadeTimer()
{
    if (!fade_.active)
    {
        return;
    }

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    chip::DeviceLayer::SystemLayer().CancelTimer(OnFadeTimer, nullptr);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    fade_.active = false;
}

bool RgbcwPwmDriver::EnsurePwmReadyForOutput()
{
    if (OvercurrentProtector::IsFaultActive())
    {
        PwmOutputKillRegisters();
        runtime_.route_disabled = true;
        return false;
    }

    if (runtime_.route_disabled || !runtime_.pwm_started)
    {
        PwmOutputRestoreRegisters();
    }
    return true;
}

void RgbcwPwmDriver::CompleteFadeNow()
{
    CancelFadeTimer();
    ApplyFadeFrame(fade_.steps_total > 0 ? fade_.steps_total : 1);
}

void RgbcwPwmDriver::StartFadeTimer(uint16_t duration_ms)
{
    fade_.step        = 0;
    fade_.steps_total = std::max<uint16_t>(duration_ms / static_cast<uint16_t>(kFadeTickMs), 1U);

    CancelFadeTimer();
    ApplyFadeFrame(0);

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    const CHIP_ERROR err = chip::DeviceLayer::SystemLayer().StartTimer(
        chip::System::Clock::Milliseconds32(kFadeTickMs), OnFadeTimer, nullptr);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    if (err != CHIP_NO_ERROR)
    {
        ApplyOutputImmediate();
        return;
    }
    fade_.active = true;
}

void RgbcwPwmDriver::ApplyFadeFrame(uint16_t step)
{
    if (!EnsurePwmReadyForOutput())
    {
        return;
    }

    if (runtime_.light.use_ct)
    {
        const uint8_t brightness =
            InterpolateScalar(fade_.start_brightness, fade_.target_brightness, step, fade_.steps_total);
        uint16_t warmRatioFp = fade_.target_warm_ratio_fp;
        if (fade_.kind != FadeKind::kLevel)
        {
            warmRatioFp = InterpolateScalar(fade_.start_warm_ratio_fp, fade_.target_warm_ratio_fp, step, fade_.steps_total);
        }

        uint8_t cool = 0;
        uint8_t warm = 0;
        DutiesFromBrightnessAndRatio(brightness, warmRatioFp, cool, warm);
        ApplyDisplayDuties(0, 0, 0, cool, warm);
        return;
    }

    ApplyDisplayDuties(InterpolateScalar(fade_.start.r, fade_.target.r, step, fade_.steps_total),
                       InterpolateScalar(fade_.start.g, fade_.target.g, step, fade_.steps_total),
                       InterpolateScalar(fade_.start.b, fade_.target.b, step, fade_.steps_total),
                       InterpolateScalar(fade_.start.cool, fade_.target.cool, step, fade_.steps_total),
                       InterpolateScalar(fade_.start.warm, fade_.target.warm, step, fade_.steps_total));
}

void RgbcwPwmDriver::OnFadeTimer(chip::System::Layer* layer, void* app_state)
{
    (void) layer;
    (void) app_state;

    if (!fade_.active || OvercurrentProtector::IsFaultActive())
    {
        fade_.active = false;
        return;
    }

    fade_.step++;
    if (fade_.step >= fade_.steps_total)
    {
        fade_.active = false;
        ApplyFadeFrame(fade_.steps_total);
        return;
    }

    ApplyFadeFrame(fade_.step);
    (void) chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::Milliseconds32(kFadeTickMs), OnFadeTimer, nullptr);
}

void RgbcwPwmDriver::ScheduleFade(FadeKind kind, bool restart_fade)
{
    if (OvercurrentProtector::IsFaultActive())
    {
        ApplyOutputImmediate();
        return;
    }

    const FadeKind previousKind = fade_.kind;
    uint16_t duration_ms        = kFadeDurationLevelMs;
    if (kind == FadeKind::kOnOff)
    {
        duration_ms = runtime_.light.on ? kFadeDurationOnMs : kFadeDurationOffMs;
    }
    else if (kind == FadeKind::kColor)
    {
        duration_ms = runtime_.light.use_ct ? kFadeDurationCtMs : kFadeDurationColorMs;
    }

    if (runtime_.light.use_ct)
    {
        const uint8_t targetBrightness   = runtime_.light.on ? LevelToBrightnessPercent(runtime_.light.level) : 0;
        const uint16_t targetWarmRatioFp = WarmRatioFpFromMireds(runtime_.light.ct_mireds);
        const uint8_t displayBrightness  = static_cast<uint8_t>(display_.cool + display_.warm);

        if (!restart_fade && fade_.active && fade_.kind == kind)
        {
            if (kind == FadeKind::kColor)
            {
                fade_.target_brightness      = targetBrightness;
                fade_.target_warm_ratio_fp = targetWarmRatioFp;
                if (displayBrightness == targetBrightness &&
                    WarmRatioFpFromDuties(display_.cool, display_.warm) == targetWarmRatioFp)
                {
                    CancelFadeTimer();
                }
                return;
            }
            if (kind == FadeKind::kLevel)
            {
                fade_.target_brightness = targetBrightness;
                return;
            }
        }

        fade_.start_brightness     = displayBrightness;
        fade_.start_warm_ratio_fp = (kind == FadeKind::kLevel || displayBrightness == 0)
                                        ? targetWarmRatioFp
                                        : WarmRatioFpFromDuties(display_.cool, display_.warm);
        fade_.target_brightness     = targetBrightness;
        fade_.target_warm_ratio_fp = targetWarmRatioFp;
        fade_.kind                  = kind;

        if (fade_.start_brightness == fade_.target_brightness &&
            (kind == FadeKind::kLevel || fade_.start_warm_ratio_fp == fade_.target_warm_ratio_fp))
        {
            CompleteFadeNow();
            return;
        }

        if (restart_fade || !fade_.active || previousKind != kind)
        {
            StartFadeTimer(duration_ms);
        }
        return;
    }

    OutputFrame target{};
    ComputeTargetDuties(target.r, target.g, target.b, target.cool, target.warm);
    fade_.target = target;

    if (!restart_fade && fade_.active && fade_.kind == FadeKind::kColor && kind == FadeKind::kColor)
    {
        if (display_ == fade_.target)
        {
            CancelFadeTimer();
        }
        return;
    }

    if (!fade_.active || restart_fade || fade_.kind != kind)
    {
        fade_.start = display_;
    }

    fade_.kind = kind;

    if (fade_.start == fade_.target)
    {
        CompleteFadeNow();
        return;
    }

    StartFadeTimer(duration_ms);
}

void RgbcwPwmDriver::ScheduleLightFade(FadeKind kind, bool restart_fade)
{
    SyncLastOnStateIfOn();
    if (runtime_.light.on)
    {
        ScheduleFade(kind, restart_fade);
    }
}

void RgbcwPwmDriver::RefreshColorModeFromMatter(chip::EndpointId endpoint)
{
    using namespace chip::app::Clusters;

    ColorControl::ColorModeEnum color_mode = ColorControl::ColorModeEnum::kCurrentHueAndCurrentSaturation;
    ColorControl::EnhancedColorModeEnum enhanced_mode =
        ColorControl::EnhancedColorModeEnum::kCurrentHueAndCurrentSaturation;

    ColorControl::Attributes::ColorMode::Get(endpoint, &color_mode);
    ColorControl::Attributes::EnhancedColorMode::Get(endpoint, &enhanced_mode);

    if (IsCtColorMode(color_mode, enhanced_mode))
    {
        runtime_.light.use_ct = true;
    }
}

void RgbcwPwmDriver::SetOn(chip::EndpointId endpoint, bool on)
{
    if (on && OvercurrentProtector::BlocksTurnOn())
    {
        ChipLogProgress(AppServer, "RgbcwPwm: on rejected (protection fault)");
        return;
    }

    if (!on && runtime_.light.on)
    {
        last_on_ = { true, runtime_.light };
    }

    if (on)
    {
        RestoreLastOnStateIfNeeded();

        using namespace chip::app::Clusters;
        using namespace chip::app::DataModel;
        using namespace chip::Protocols::InteractionModel;

        chip::DeviceLayer::PlatformMgr().LockChipStack();
        RefreshColorModeFromMatter(endpoint);
        Nullable<uint8_t> currentLevel;
        if (LevelControl::Attributes::CurrentLevel::Get(endpoint, currentLevel) == Status::Success && !currentLevel.IsNull())
        {
            runtime_.light.level = ResolveLevelForCluster(endpoint, true, currentLevel.Value());
        }
        chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    }

    runtime_.light.on = on;
    ScheduleFade(FadeKind::kOnOff, true);
}

void RgbcwPwmDriver::SetLevel(uint8_t level)
{
    runtime_.light.level = level;
    ScheduleLightFade(FadeKind::kLevel, false);
}

void RgbcwPwmDriver::SetHueSat(uint8_t hue, uint8_t saturation)
{
    runtime_.light.hue        = hue;
    runtime_.light.saturation = saturation;
    runtime_.light.use_ct     = false;
    ScheduleLightFade(FadeKind::kColor, false);
}

void RgbcwPwmDriver::SetColorTemperatureMireds(uint16_t mireds)
{
    runtime_.light.ct_mireds = std::clamp(mireds, kCtMinMireds, kCtMaxMireds);
    runtime_.light.use_ct    = true;
    ScheduleLightFade(FadeKind::kColor, false);
}

void RgbcwPwmDriver::ApplyClusterLevel(chip::EndpointId endpoint, uint8_t cluster_level)
{
    if (runtime_.light.on && cluster_level <= 1)
    {
        return;
    }

    if (!runtime_.light.on)
    {
        runtime_.light.level = cluster_level;
        return;
    }

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    RefreshColorModeFromMatter(endpoint);
    const uint8_t level = ResolveLevelForCluster(endpoint, true, cluster_level);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    SetLevel(level);
}

uint8_t RgbcwPwmDriver::ResolveLevelForCluster(chip::EndpointId endpoint, bool on, uint8_t cluster_level)
{
    (void) endpoint;
    if (!on)
    {
        return runtime_.light.level;
    }
    if (cluster_level > 1)
    {
        return cluster_level;
    }
    if (runtime_.light.level > 1)
    {
        return runtime_.light.level;
    }
    return cluster_level;
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
    ColorControl::ColorModeEnum color_mode = ColorControl::ColorModeEnum::kCurrentHueAndCurrentSaturation;
    ColorControl::EnhancedColorModeEnum enhanced_mode =
        ColorControl::EnhancedColorModeEnum::kCurrentHueAndCurrentSaturation;

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
    ColorControl::Attributes::ColorMode::Get(endpoint, &color_mode);
    ColorControl::Attributes::EnhancedColorMode::Get(endpoint, &enhanced_mode);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

    runtime_.light.on    = on;
    runtime_.light.level = level;
    runtime_.light.hue   = hue;
    runtime_.light.saturation   = sat;
    runtime_.light.ct_mireds = std::clamp(ct, kCtMinMireds, kCtMaxMireds);
    runtime_.light.use_ct = IsCtColorMode(color_mode, enhanced_mode);
    SyncLastOnStateIfOn();
    ApplyOutputImmediate();
}

void RgbcwPwmDriver::ForceOffForFaultFromIsr()
{
    SaveStateBeforeFault();
    runtime_.light.on       = false;
    fade_.active            = false;
    display_                = {};
    runtime_.route_disabled = true;
    PwmOutputKillRegisters();
}

void RgbcwPwmDriver::ForceOffForFault()
{
    SaveStateBeforeFault();
    CancelFadeTimer();
    runtime_.light.on       = false;
    display_                = {};
    runtime_.route_disabled = true;
    PwmOutputKillRegisters();
}

void RgbcwPwmDriver::RecoverFromFault()
{
    if (fault_.saved)
    {
        RestoreToPreFault(fault_.light.on, fault_.light.level, fault_.light.ct_mireds);
        return;
    }

    PwmOutputRestoreRegisters();
    ApplyOutputImmediate();
}

void RgbcwPwmDriver::RestoreToPreFault(bool on, uint8_t level, uint16_t ct_mireds)
{
    runtime_.light            = fault_.light;
    runtime_.light.on         = on;
    runtime_.light.level      = level;
    runtime_.light.ct_mireds  = std::clamp(ct_mireds, kCtMinMireds, kCtMaxMireds);
    fault_.saved              = false;

    PwmOutputRestoreRegisters();
    ApplyOutputImmediate();

    ChipLogProgress(AppServer, "RgbcwPwm RECOVER: on=%u level=%u ct=%u use_ct=%u", runtime_.light.on, runtime_.light.level,
                    runtime_.light.ct_mireds, runtime_.light.use_ct);
}

bool RgbcwPwmDriver::GetPreFaultState(bool& on, uint8_t& level, uint16_t& ct_mireds)
{
    if (!fault_.saved)
    {
        return false;
    }

    on        = fault_.light.on;
    level     = fault_.light.level;
    ct_mireds = fault_.light.ct_mireds;
    return true;
}
