/*
 * RGBCW SPI 幻彩灯带：四混五 + WS2814 SPI（移植 Telink lighting-app/light + zephyr_ws2814）。
 */

#include "RgbcwStripDriver.h"

#include "AppConfig.h"
#include "color_format_mix.h"
#include "ws2814_strip_hal.h"
#include "ws2814_strip_config.h"

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

bool RgbcwStripDriver::sOn                  = false;
uint8_t RgbcwStripDriver::sLevel              = 254;
uint8_t RgbcwStripDriver::sHue                = 0;
uint8_t RgbcwStripDriver::sSat                = 0;
uint16_t RgbcwStripDriver::sCtMireds          = kDefaultCtMireds;
uint16_t RgbcwStripDriver::sCurrentX          = 0;
uint16_t RgbcwStripDriver::sCurrentY          = 0;
RgbcwStripDriver::ColorPath RgbcwStripDriver::sColorPath = ColorPath::kCt;
bool RgbcwStripDriver::sPreFaultSaved           = false;
bool RgbcwStripDriver::sPreFaultOn              = false;
uint8_t RgbcwStripDriver::sPreFaultLevel        = 254;
uint16_t RgbcwStripDriver::sPreFaultCtMireds    = kDefaultCtMireds;
bool RgbcwStripDriver::sLastOnValid             = false;
uint8_t RgbcwStripDriver::sLastOnLevel          = 254;
uint16_t RgbcwStripDriver::sLastOnCtMireds      = kDefaultCtMireds;
bool RgbcwStripDriver::sLastOnUseCt             = false;
RgbcwStripDriver::ColorPath RgbcwStripDriver::sLastOnColorPath = ColorPath::kCt;
uint8_t RgbcwStripDriver::sLastOnHue            = 0;
uint8_t RgbcwStripDriver::sLastOnSat            = 0;
RgbcwStripDriver::Rgbw1024 RgbcwStripDriver::sDisplay{};
RgbcwStripDriver::Rgbw1024 RgbcwStripDriver::sFadeStart{};
RgbcwStripDriver::Rgbw1024 RgbcwStripDriver::sFadeTarget{};
RgbcwStripDriver::FadeKind RgbcwStripDriver::sFadeKind = FadeKind::kOnOff;
uint16_t RgbcwStripDriver::sFadeStep            = 0;
uint16_t RgbcwStripDriver::sFadeStepsTotal      = 0;
bool RgbcwStripDriver::sFadeActive              = false;

namespace {
constexpr uint32_t kFadeTickMs          = 10;
constexpr uint16_t kFadeDurationOnMs    = 500;
constexpr uint16_t kFadeDurationOffMs   = 350;
constexpr uint16_t kFadeDurationLevelMs = 320;
constexpr uint16_t kFadeDurationColorMs = 650;
constexpr uint32_t kFadeFpOne = 65535U;

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

uint16_t Interpolate1024(uint16_t from, uint16_t to, uint16_t step, uint16_t total)
{
    if (total == 0 || step >= total)
    {
        return to;
    }

    const uint32_t t      = SmoothstepT(step, total);
    const int32_t delta   = static_cast<int32_t>(to) - static_cast<int32_t>(from);
    const int32_t result  = static_cast<int32_t>(from) + (delta * static_cast<int32_t>(t)) / static_cast<int32_t>(kFadeFpOne);
    const int32_t lo = 0;
    const int32_t hi = static_cast<int32_t>(STRIP_LEVEL_MAX);
    const int32_t clamped = std::clamp(result, lo, hi);
    return static_cast<uint16_t>(clamped);
}

bool IsMixedRgbw(uint16_t r, uint16_t g, uint16_t b, uint16_t w)
{
    const bool hasRgb = (r > 0 || g > 0 || b > 0);
    return hasRgb && (w > 0);
}

} // namespace

void RgbcwStripDriver::PublishMatterColorMode(ColorPath path)
{
    using namespace chip::app::Clusters;
    using ColorControl::ColorModeEnum;
    using ColorControl::EnhancedColorModeEnum;

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    switch (path)
    {
    case ColorPath::kHsv:
        ColorControl::Attributes::ColorMode::Set(LIGHT_ENDPOINT, ColorModeEnum::kCurrentHueAndCurrentSaturation);
        ColorControl::Attributes::EnhancedColorMode::Set(LIGHT_ENDPOINT,
                                                         EnhancedColorModeEnum::kCurrentHueAndCurrentSaturation);
        break;
    case ColorPath::kXy:
        ColorControl::Attributes::ColorMode::Set(LIGHT_ENDPOINT, ColorModeEnum::kCurrentXAndCurrentY);
        ColorControl::Attributes::EnhancedColorMode::Set(LIGHT_ENDPOINT, EnhancedColorModeEnum::kCurrentXAndCurrentY);
        break;
    case ColorPath::kCt:
    default:
        ColorControl::Attributes::ColorMode::Set(LIGHT_ENDPOINT, ColorModeEnum::kColorTemperatureMireds);
        ColorControl::Attributes::EnhancedColorMode::Set(LIGHT_ENDPOINT, EnhancedColorModeEnum::kColorTemperatureMireds);
        break;
    }
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
}

RgbcwStripDriver::ColorPath RgbcwStripDriver::ResolveColorPathFromMatter(chip::EndpointId endpoint)
{
    using namespace chip::app::Clusters;
    using ColorControl::ColorModeEnum;
    using ColorControl::EnhancedColorModeEnum;

    EnhancedColorModeEnum enhanced = EnhancedColorModeEnum::kColorTemperatureMireds;
    if (ColorControl::Attributes::EnhancedColorMode::Get(endpoint, &enhanced) == chip::Protocols::InteractionModel::Status::Success)
    {
        switch (enhanced)
        {
        case EnhancedColorModeEnum::kCurrentHueAndCurrentSaturation:
            return ColorPath::kHsv;
        case EnhancedColorModeEnum::kCurrentXAndCurrentY:
            return ColorPath::kXy;
        case EnhancedColorModeEnum::kColorTemperatureMireds:
        default:
            return ColorPath::kCt;
        }
    }

    ColorModeEnum colorMode = ColorModeEnum::kColorTemperatureMireds;
    if (ColorControl::Attributes::ColorMode::Get(endpoint, &colorMode) == chip::Protocols::InteractionModel::Status::Success)
    {
        if (colorMode == ColorModeEnum::kCurrentHueAndCurrentSaturation)
        {
            return ColorPath::kHsv;
        }
        if (colorMode == ColorModeEnum::kCurrentXAndCurrentY)
        {
            return ColorPath::kXy;
        }
    }

    return ColorPath::kCt;
}

void RgbcwStripDriver::Init()
{
    (void) ws2814_strip::Init();
    SetOn(false);
}

void RgbcwStripDriver::ComputeTargetOutput(Rgbw1024 & out)
{
    out = {};

    if (!sOn)
    {
        return;
    }

#if RGBW_4CH_MIX_5CH
    if (sColorPath == ColorPath::kCt)
    {
        color_format::MixRgbwFromMireds(sCtMireds, sLevel, &out.r, &out.g, &out.b, &out.w);
        return;
    }

    if (sColorPath == ColorPath::kXy)
    {
        const color_format::Rgb8 rgb = color_format::XyToRgb(sLevel, sCurrentX, sCurrentY);
        out.r                        = color_format::ScaleTo1024(rgb.r);
        out.g                        = color_format::ScaleTo1024(rgb.g);
        out.b                        = color_format::ScaleTo1024(rgb.b);
        out.w          = 0;
        return;
    }

    const color_format::Hsv8 hsv = { sHue, sSat, sLevel };
    const color_format::Rgb8 rgb = color_format::HsvToRgb(hsv);
    out.r                        = color_format::ScaleTo1024(rgb.r);
    out.g                        = color_format::ScaleTo1024(rgb.g);
    out.b                        = color_format::ScaleTo1024(rgb.b);
    out.w          = 0;
#else
    const uint8_t br = static_cast<uint8_t>((static_cast<uint32_t>(sLevel) * 100U) / 254U);
    if (sColorPath == ColorPath::kCt)
    {
        const uint16_t ct = std::clamp(sCtMireds, kCtMinMireds, kCtMaxMireds);
        const uint32_t warmWeight = ct - kCtMinMireds;
        const uint32_t sum        = (kCtMaxMireds - kCtMinMireds);
        uint8_t warm              = 0;
        uint8_t cool              = 0;
        if (sum == 0)
        {
            cool = br / 2;
            warm = br - cool;
        }
        else
        {
            warm = static_cast<uint8_t>((br * warmWeight) / sum);
            cool = br - warm;
        }
        out.w = static_cast<uint16_t>((static_cast<uint32_t>(warm + cool) * STRIP_LEVEL_MAX) / 100U);
    }
    else
    {
        const color_format::Hsv8 hsv = { sHue, sSat, static_cast<uint8_t>(br) };
        const color_format::Rgb8 rgb = color_format::HsvToRgb(hsv);
        out.r          = static_cast<uint16_t>((static_cast<uint32_t>(rgb.r) * STRIP_LEVEL_MAX) / 255U);
        out.g          = static_cast<uint16_t>((static_cast<uint32_t>(rgb.g) * STRIP_LEVEL_MAX) / 255U);
        out.b          = static_cast<uint16_t>((static_cast<uint32_t>(rgb.b) * STRIP_LEVEL_MAX) / 255U);
    }
#endif
}

void RgbcwStripDriver::ApplyRgbwLumaCompensation(Rgbw1024 & current, const Rgbw1024 & start, const Rgbw1024 & target,
                                               float progress)
{
    const bool startMixed  = IsMixedRgbw(start.r, start.g, start.b, start.w);
    const bool targetMixed = IsMixedRgbw(target.r, target.g, target.b, target.w);
    if (!startMixed && !targetMixed)
    {
        return;
    }

    const uint32_t startTotal =
        static_cast<uint32_t>(start.r) + start.g + start.b + start.w;
    const uint32_t targetTotal =
        static_cast<uint32_t>(target.r) + target.g + target.b + target.w;
    const uint32_t currentTotal =
        static_cast<uint32_t>(current.r) + current.g + current.b + current.w;

    if (currentTotal == 0)
    {
        return;
    }

    const float desiredTotalF =
        (static_cast<float>(startTotal) * (1.0f - progress)) + (static_cast<float>(targetTotal) * progress);
    if (desiredTotalF <= 0.0f)
    {
        return;
    }

    const float scale = desiredTotalF / static_cast<float>(currentTotal);
    auto clamp1024    = [](float v) -> uint16_t {
        if (v <= 0.0f)
        {
            return 0;
        }
        if (v >= static_cast<float>(STRIP_LEVEL_MAX))
        {
            return STRIP_LEVEL_MAX;
        }
        return static_cast<uint16_t>(v);
    };

    current.r = clamp1024(static_cast<float>(current.r) * scale);
    current.g = clamp1024(static_cast<float>(current.g) * scale);
    current.b = clamp1024(static_cast<float>(current.b) * scale);
    current.w = clamp1024(static_cast<float>(current.w) * scale);
}

void RgbcwStripDriver::ApplyDisplayOutput(const Rgbw1024 & rgbw)
{
    const bool allOff = (rgbw.r == 0 && rgbw.g == 0 && rgbw.b == 0 && rgbw.w == 0);
    ws2814_strip::ControlRgbw(rgbw.r, rgbw.g, rgbw.b, rgbw.w);
#if WS2814_OFF_FRAME_REPEAT > 1
    if (allOff)
    {
        for (uint8_t i = 1; i < WS2814_OFF_FRAME_REPEAT; ++i)
        {
            (void) ws2814_strip::ControlRgbw(0, 0, 0, 0);
        }
    }
#endif
    sDisplay = rgbw;
}

void RgbcwStripDriver::ApplyOutputImmediate()
{
    CancelFadeTimer();

    Rgbw1024 target{};
    ComputeTargetOutput(target);
    ApplyDisplayOutput(target);
}

void RgbcwStripDriver::CancelFadeTimer()
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

void RgbcwStripDriver::ApplyFadeFrame(uint16_t step)
{
    Rgbw1024 current{
        Interpolate1024(sFadeStart.r, sFadeTarget.r, step, sFadeStepsTotal),
        Interpolate1024(sFadeStart.g, sFadeTarget.g, step, sFadeStepsTotal),
        Interpolate1024(sFadeStart.b, sFadeTarget.b, step, sFadeStepsTotal),
        Interpolate1024(sFadeStart.w, sFadeTarget.w, step, sFadeStepsTotal),
    };

#if RGBW_4CH_MIX_5CH
    if (sFadeStepsTotal > 0)
    {
        const float progress = static_cast<float>(step) / static_cast<float>(sFadeStepsTotal);
        ApplyRgbwLumaCompensation(current, sFadeStart, sFadeTarget, progress);
    }
#endif

    ApplyDisplayOutput(current);
}

void RgbcwStripDriver::OnFadeTimer(chip::System::Layer* layer, void* appState)
{
    (void) layer;
    (void) appState;

    if (!sFadeActive)
    {
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

void RgbcwStripDriver::ScheduleFade(FadeKind kind, bool restartFade)
{
#if !STRIP_FADE_ENABLED
    (void) kind;
    (void) restartFade;
    CancelFadeTimer();
    ApplyOutputImmediate();
    return;
#else
    Rgbw1024 target{};
    ComputeTargetOutput(target);
    sFadeTarget = target;

    if (kind == FadeKind::kColor && sFadeActive && sFadeKind == FadeKind::kColor && !restartFade)
    {
        if (sDisplay.r == sFadeTarget.r && sDisplay.g == sFadeTarget.g && sDisplay.b == sFadeTarget.b &&
            sDisplay.w == sFadeTarget.w)
        {
            CancelFadeTimer();
        }
        else if (sFadeStepsTotal > 0)
        {
            /* 渐变过程中收到新目标色：立即按当前步刷新 SPI，避免只更新 sFadeTarget 却不发帧 */
            ApplyFadeFrame(sFadeStep);
        }
        return;
    }

    if (!sFadeActive || restartFade || sFadeKind != kind)
    {
        sFadeStart = sDisplay;
    }
    sFadeKind = kind;

    if (sFadeStart.r == sFadeTarget.r && sFadeStart.g == sFadeTarget.g && sFadeStart.b == sFadeTarget.b &&
        sFadeStart.w == sFadeTarget.w)
    {
        CancelFadeTimer();
        ApplyFadeFrame(sFadeStepsTotal);
        return;
    }

    uint16_t durationMs = kFadeDurationLevelMs;
    if (kind == FadeKind::kOnOff)
    {
        durationMs = sOn ? kFadeDurationOnMs : kFadeDurationOffMs;
    }
    else if (kind == FadeKind::kColor)
    {
        durationMs = kFadeDurationColorMs;
    }

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
#endif // STRIP_FADE_ENABLED
}

void RgbcwStripDriver::SetOn(bool on)
{
    if (!on && sOn)
    {
        sLastOnValid    = true;
        sLastOnLevel    = sLevel;
        sLastOnCtMireds = sCtMireds;
        sLastOnUseCt      = (sColorPath == ColorPath::kCt);
        sLastOnColorPath  = sColorPath;
        sLastOnHue        = sHue;
        sLastOnSat        = sSat;
    }

    if (on && !sOn && sLastOnValid)
    {
        sLevel       = sLastOnLevel;
        sCtMireds    = sLastOnCtMireds;
        sColorPath   = sLastOnColorPath;
        sLastOnUseCt = (sColorPath == ColorPath::kCt);
        sHue         = sLastOnHue;
        sSat         = sLastOnSat;
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
}

void RgbcwStripDriver::SetLevel(uint8_t level)
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

void RgbcwStripDriver::SetHueSat(uint8_t hue, uint8_t saturation)
{
    sHue        = hue;
    sSat        = saturation;
    sColorPath  = ColorPath::kHsv;
    PublishMatterColorMode(ColorPath::kHsv);
    if (sOn)
    {
        sLastOnValid = true;
        sLastOnLevel = sLevel;
        sLastOnUseCt = false;
        sLastOnHue   = sHue;
        sLastOnSat   = sSat;
    }
    if (!sOn)
    {
        return;
    }
    ScheduleFade(FadeKind::kColor, false);
}

void RgbcwStripDriver::SetXy(uint16_t currentX, uint16_t currentY)
{
    sCurrentX  = currentX;
    sCurrentY  = currentY;
    sColorPath = ColorPath::kXy;
    PublishMatterColorMode(ColorPath::kXy);
    if (sOn)
    {
        sLastOnValid = true;
        sLastOnLevel = sLevel;
        sLastOnUseCt = false;
    }
    if (!sOn)
    {
        return;
    }
    ScheduleFade(FadeKind::kColor, false);
}

void RgbcwStripDriver::SetColorTemperatureMireds(uint16_t mireds)
{
    sCtMireds  = std::clamp(mireds, kCtMinMireds, kCtMaxMireds);
    sColorPath = ColorPath::kCt;
    PublishMatterColorMode(ColorPath::kCt);
    if (sOn)
    {
        sLastOnValid    = true;
        sLastOnLevel    = sLevel;
        sLastOnUseCt    = true;
        sLastOnCtMireds = sCtMireds;
    }
    if (!sOn)
    {
        return;
    }
    ScheduleFade(FadeKind::kColor, false);
}

void RgbcwStripDriver::ApplyClusterLevel(chip::EndpointId endpoint, uint8_t clusterLevel)
{
    chip::DeviceLayer::PlatformMgr().LockChipStack();
    const uint8_t level = ResolveLevelForPwmLocked(endpoint, sOn, clusterLevel);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    SetLevel(level);
}

uint8_t RgbcwStripDriver::ResolveLevelForPwmLocked(chip::EndpointId endpoint, bool on, uint8_t clusterLevel)
{
    (void) endpoint;
    if (!on)
    {
        return sLevel;
    }
    if (clusterLevel <= 1)
    {
        return clusterLevel;
    }
    return clusterLevel;
}

void RgbcwStripDriver::SyncFromMatterEndpoint(chip::EndpointId endpoint)
{
    using namespace chip::app::Clusters;

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    bool on = false;
    uint8_t level = 254;
    uint8_t hue = 0;
    uint8_t sat = 0;
    uint16_t ct = kDefaultCtMireds;
    uint16_t currentX = 0;
    uint16_t currentY = 0;

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
    ColorControl::Attributes::CurrentX::Get(endpoint, &currentX);
    ColorControl::Attributes::CurrentY::Get(endpoint, &currentY);
    const ColorPath colorPath = ResolveColorPathFromMatter(endpoint);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

    sOn       = on;
    sLevel    = level;
    sHue      = hue;
    sSat      = sat;
    sCtMireds = ct;
    sCurrentX = currentX;
    sCurrentY = currentY;
    sColorPath = colorPath;
    if (sOn)
    {
        sLastOnValid    = true;
        sLastOnLevel    = sLevel;
        sLastOnCtMireds = sCtMireds;
        sLastOnUseCt    = (sColorPath == ColorPath::kCt);
        sLastOnHue      = sHue;
        sLastOnSat      = sSat;
    }
    ApplyOutputImmediate();
}

void RgbcwStripDriver::ForceOffForFault()
{
    CancelFadeTimer();
    sOn = false;
    sDisplay = {};
    ApplyDisplayOutput(sDisplay);
}

void RgbcwStripDriver::ForceOffForFaultFromIsr()
{
    sOn = false;
    sFadeActive = false;
    sDisplay = {};
    for (uint8_t i = 0; i < WS2814_OFF_FRAME_REPEAT; ++i)
    {
        (void) ws2814_strip::ControlRgbw(0, 0, 0, 0);
    }
}

void RgbcwStripDriver::RecoverFromFault()
{
    ApplyOutputImmediate();
}

void RgbcwStripDriver::RestoreToPreFault(bool on, uint8_t level, uint16_t ctMireds)
{
    sOn       = on;
    sLevel    = level;
    sCtMireds = ctMireds;
    sColorPath = ColorPath::kCt;
    ApplyOutputImmediate();
}

bool RgbcwStripDriver::GetPreFaultState(bool & on, uint8_t & level, uint16_t & ctMireds)
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
