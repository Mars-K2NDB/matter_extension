/*
 * RGBCW SPI 幻彩灯带：四混五 + WS2814 SPI（移植 Telink lighting-app/light + zephyr_ws2814）。
 */

#include "rgbcw_strip_driver.h"

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

bool RgbcwStripDriver::on_                  = false;
uint8_t RgbcwStripDriver::level_              = 254;
uint8_t RgbcwStripDriver::hue_                = 0;
uint8_t RgbcwStripDriver::sat_                = 0;
uint16_t RgbcwStripDriver::ct_mireds_          = kDefaultCtMireds;
uint16_t RgbcwStripDriver::current_x_          = 0;
uint16_t RgbcwStripDriver::current_y_          = 0;
RgbcwStripDriver::ColorPath RgbcwStripDriver::color_path_ = ColorPath::kCt;
bool RgbcwStripDriver::pre_fault_saved_           = false;
bool RgbcwStripDriver::pre_fault_on_              = false;
uint8_t RgbcwStripDriver::pre_fault_level_        = 254;
uint16_t RgbcwStripDriver::pre_fault_ct_mireds_    = kDefaultCtMireds;
bool RgbcwStripDriver::last_on_valid_             = false;
uint8_t RgbcwStripDriver::last_on_level_          = 254;
uint16_t RgbcwStripDriver::last_on_ct_mireds_      = kDefaultCtMireds;
bool RgbcwStripDriver::last_on_use_ct_             = false;
RgbcwStripDriver::ColorPath RgbcwStripDriver::last_on_color_path_ = ColorPath::kCt;
uint8_t RgbcwStripDriver::last_on_hue_            = 0;
uint8_t RgbcwStripDriver::last_on_sat_            = 0;
RgbcwStripDriver::Rgbw1024 RgbcwStripDriver::display_{};
RgbcwStripDriver::Rgbw1024 RgbcwStripDriver::fade_start_{};
RgbcwStripDriver::Rgbw1024 RgbcwStripDriver::fade_target_{};
RgbcwStripDriver::FadeKind RgbcwStripDriver::fade_kind_ = FadeKind::kOnOff;
uint16_t RgbcwStripDriver::fade_step_            = 0;
uint16_t RgbcwStripDriver::fade_steps_total_      = 0;
bool RgbcwStripDriver::fade_active_              = false;

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

    ColorModeEnum color_mode = ColorModeEnum::kColorTemperatureMireds;
    if (ColorControl::Attributes::ColorMode::Get(endpoint, &color_mode) ==
        chip::Protocols::InteractionModel::Status::Success)
    {
        if (color_mode == ColorModeEnum::kCurrentHueAndCurrentSaturation)
        {
            return ColorPath::kHsv;
        }
        if (color_mode == ColorModeEnum::kCurrentXAndCurrentY)
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

void RgbcwStripDriver::ComputeTargetOutput(Rgbw1024& out)
{
    out = {};

    if (!on_)
    {
        return;
    }

#if RGBW_4CH_MIX_5CH
    if (color_path_ == ColorPath::kCt)
    {
        color_format::MixRgbwFromMireds(ct_mireds_, level_, &out.r, &out.g, &out.b, &out.w);
        return;
    }

    if (color_path_ == ColorPath::kXy)
    {
        const color_format::Rgb8 rgb = color_format::XyToRgb(level_, current_x_, current_y_);
        out.r                        = color_format::ScaleTo1024(rgb.r);
        out.g                        = color_format::ScaleTo1024(rgb.g);
        out.b                        = color_format::ScaleTo1024(rgb.b);
        out.w          = 0;
        return;
    }

    const color_format::Hsv8 hsv = { hue_, sat_, level_ };
    const color_format::Rgb8 rgb = color_format::HsvToRgb(hsv);
    out.r                        = color_format::ScaleTo1024(rgb.r);
    out.g                        = color_format::ScaleTo1024(rgb.g);
    out.b                        = color_format::ScaleTo1024(rgb.b);
    out.w          = 0;
#else
    const uint8_t br = static_cast<uint8_t>((static_cast<uint32_t>(level_) * 100U) / 254U);
    if (color_path_ == ColorPath::kCt)
    {
        const uint16_t ct = std::clamp(ct_mireds_, kCtMinMireds, kCtMaxMireds);
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
        const color_format::Hsv8 hsv = { hue_, sat_, static_cast<uint8_t>(br) };
        const color_format::Rgb8 rgb = color_format::HsvToRgb(hsv);
        out.r          = static_cast<uint16_t>((static_cast<uint32_t>(rgb.r) * STRIP_LEVEL_MAX) / 255U);
        out.g          = static_cast<uint16_t>((static_cast<uint32_t>(rgb.g) * STRIP_LEVEL_MAX) / 255U);
        out.b          = static_cast<uint16_t>((static_cast<uint32_t>(rgb.b) * STRIP_LEVEL_MAX) / 255U);
    }
#endif
}

void RgbcwStripDriver::ApplyRgbwLumaCompensation(Rgbw1024& current, const Rgbw1024& start, const Rgbw1024& target,
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

void RgbcwStripDriver::ApplyDisplayOutput(const Rgbw1024& rgbw)
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
    display_ = rgbw;
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
    if (!fade_active_)
    {
        return;
    }

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    chip::DeviceLayer::SystemLayer().CancelTimer(OnFadeTimer, nullptr);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    fade_active_ = false;
}

void RgbcwStripDriver::ApplyFadeFrame(uint16_t step)
{
    Rgbw1024 current{
        Interpolate1024(fade_start_.r, fade_target_.r, step, fade_steps_total_),
        Interpolate1024(fade_start_.g, fade_target_.g, step, fade_steps_total_),
        Interpolate1024(fade_start_.b, fade_target_.b, step, fade_steps_total_),
        Interpolate1024(fade_start_.w, fade_target_.w, step, fade_steps_total_),
    };

#if RGBW_4CH_MIX_5CH
    if (fade_steps_total_ > 0)
    {
        const float progress = static_cast<float>(step) / static_cast<float>(fade_steps_total_);
        ApplyRgbwLumaCompensation(current, fade_start_, fade_target_, progress);
    }
#endif

    ApplyDisplayOutput(current);
}

void RgbcwStripDriver::OnFadeTimer(chip::System::Layer* layer, void* app_state)
{
    (void) layer;
    (void) app_state;

    if (!fade_active_)
    {
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

void RgbcwStripDriver::ScheduleFade(FadeKind kind, bool restart_fade)
{
#if !STRIP_FADE_ENABLED
    (void) kind;
    (void) restart_fade;
    CancelFadeTimer();
    ApplyOutputImmediate();
    return;
#else
    Rgbw1024 target{};
    ComputeTargetOutput(target);
    fade_target_ = target;

    if (kind == FadeKind::kColor && fade_active_ && fade_kind_ == FadeKind::kColor && !restart_fade)
    {
        if (display_.r == fade_target_.r && display_.g == fade_target_.g && display_.b == fade_target_.b &&
            display_.w == fade_target_.w)
        {
            CancelFadeTimer();
        }
        else if (fade_steps_total_ > 0)
        {
            /* 渐变过程中收到新目标色：立即按当前步刷新 SPI，避免只更新 fade_target_ 却不发帧 */
            ApplyFadeFrame(fade_step_);
        }
        return;
    }

    if (!fade_active_ || restart_fade || fade_kind_ != kind)
    {
        fade_start_ = display_;
    }
    fade_kind_ = kind;

    if (fade_start_.r == fade_target_.r && fade_start_.g == fade_target_.g && fade_start_.b == fade_target_.b &&
        fade_start_.w == fade_target_.w)
    {
        CancelFadeTimer();
        ApplyFadeFrame(fade_steps_total_);
        return;
    }

    uint16_t duration_ms = kFadeDurationLevelMs;
    if (kind == FadeKind::kOnOff)
    {
        duration_ms = on_ ? kFadeDurationOnMs : kFadeDurationOffMs;
    }
    else if (kind == FadeKind::kColor)
    {
        duration_ms = kFadeDurationColorMs;
    }

    fade_step_        = 0;
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
#endif // STRIP_FADE_ENABLED
}

void RgbcwStripDriver::SetOn(bool on)
{
    if (!on && on_)
    {
        last_on_valid_    = true;
        last_on_level_    = level_;
        last_on_ct_mireds_ = ct_mireds_;
        last_on_use_ct_      = (color_path_ == ColorPath::kCt);
        last_on_color_path_  = color_path_;
        last_on_hue_        = hue_;
        last_on_sat_        = sat_;
    }

    if (on && !on_ && last_on_valid_)
    {
        level_       = last_on_level_;
        ct_mireds_    = last_on_ct_mireds_;
        color_path_   = last_on_color_path_;
        last_on_use_ct_ = (color_path_ == ColorPath::kCt);
        hue_         = last_on_hue_;
        sat_         = last_on_sat_;
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
}

void RgbcwStripDriver::SetLevel(uint8_t level)
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

void RgbcwStripDriver::SetHueSat(uint8_t hue, uint8_t saturation)
{
    hue_        = hue;
    sat_        = saturation;
    color_path_  = ColorPath::kHsv;
    PublishMatterColorMode(ColorPath::kHsv);
    if (on_)
    {
        last_on_valid_ = true;
        last_on_level_ = level_;
        last_on_use_ct_ = false;
        last_on_hue_   = hue_;
        last_on_sat_   = sat_;
    }
    if (!on_)
    {
        return;
    }
    ScheduleFade(FadeKind::kColor, false);
}

void RgbcwStripDriver::SetXy(uint16_t current_x, uint16_t current_y)
{
    current_x_  = current_x;
    current_y_  = current_y;
    color_path_ = ColorPath::kXy;
    PublishMatterColorMode(ColorPath::kXy);
    if (on_)
    {
        last_on_valid_ = true;
        last_on_level_ = level_;
        last_on_use_ct_ = false;
    }
    if (!on_)
    {
        return;
    }
    ScheduleFade(FadeKind::kColor, false);
}

void RgbcwStripDriver::SetColorTemperatureMireds(uint16_t mireds)
{
    ct_mireds_  = std::clamp(mireds, kCtMinMireds, kCtMaxMireds);
    color_path_ = ColorPath::kCt;
    PublishMatterColorMode(ColorPath::kCt);
    if (on_)
    {
        last_on_valid_    = true;
        last_on_level_    = level_;
        last_on_use_ct_    = true;
        last_on_ct_mireds_ = ct_mireds_;
    }
    if (!on_)
    {
        return;
    }
    ScheduleFade(FadeKind::kColor, false);
}

void RgbcwStripDriver::ApplyClusterLevel(chip::EndpointId endpoint, uint8_t cluster_level)
{
    chip::DeviceLayer::PlatformMgr().LockChipStack();
    const uint8_t level = ResolveLevelForCluster(endpoint, on_, cluster_level);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    SetLevel(level);
}

uint8_t RgbcwStripDriver::ResolveLevelForCluster(chip::EndpointId endpoint, bool on, uint8_t cluster_level)
{
    (void) endpoint;
    if (!on)
    {
        return level_;
    }
    if (cluster_level <= 1)
    {
        return cluster_level;
    }
    return cluster_level;
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
    uint16_t current_x = 0;
    uint16_t current_y = 0;

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
    ColorControl::Attributes::CurrentX::Get(endpoint, &current_x);
    ColorControl::Attributes::CurrentY::Get(endpoint, &current_y);
    const ColorPath color_path = ResolveColorPathFromMatter(endpoint);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

    on_       = on;
    level_    = level;
    hue_      = hue;
    sat_      = sat;
    ct_mireds_ = ct;
    current_x_ = current_x;
    current_y_ = current_y;
    color_path_ = color_path;
    if (on_)
    {
        last_on_valid_    = true;
        last_on_level_    = level_;
        last_on_ct_mireds_ = ct_mireds_;
        last_on_use_ct_    = (color_path_ == ColorPath::kCt);
        last_on_hue_      = hue_;
        last_on_sat_      = sat_;
    }
    ApplyOutputImmediate();
}

void RgbcwStripDriver::ForceOffForFault()
{
    CancelFadeTimer();
    on_ = false;
    display_ = {};
    ApplyDisplayOutput(display_);
}

void RgbcwStripDriver::ForceOffForFaultFromIsr()
{
    on_ = false;
    fade_active_ = false;
    display_ = {};
    for (uint8_t i = 0; i < WS2814_OFF_FRAME_REPEAT; ++i)
    {
        (void) ws2814_strip::ControlRgbw(0, 0, 0, 0);
    }
}

void RgbcwStripDriver::RecoverFromFault()
{
    ApplyOutputImmediate();
}

void RgbcwStripDriver::RestoreToPreFault(bool on, uint8_t level, uint16_t ct_mireds)
{
    on_       = on;
    level_    = level;
    ct_mireds_ = ct_mireds;
    color_path_ = ColorPath::kCt;
    ApplyOutputImmediate();
}

bool RgbcwStripDriver::GetPreFaultState(bool& on, uint8_t& level, uint16_t& ct_mireds)
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
