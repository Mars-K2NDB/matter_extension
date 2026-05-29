/*
 * RGBCW SPI 幻彩灯带驱动（无 PWM，SPI 发送待实现）。
 */

#include "RgbcwStripDriver.h"

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

bool RgbcwStripDriver::sOn     = false;
uint8_t RgbcwStripDriver::sLevel = 254;
uint8_t RgbcwStripDriver::sHue   = 0;
uint8_t RgbcwStripDriver::sSat   = 0;
uint16_t RgbcwStripDriver::sCtMireds = kDefaultCtMireds;
bool RgbcwStripDriver::sUseCt  = false;
bool RgbcwStripDriver::sPreFaultSaved = false;
bool RgbcwStripDriver::sPreFaultOn    = false;
uint8_t RgbcwStripDriver::sPreFaultLevel = 254;
uint16_t RgbcwStripDriver::sPreFaultCtMireds = kDefaultCtMireds;
bool RgbcwStripDriver::sLastOnValid      = false;
uint8_t RgbcwStripDriver::sLastOnLevel   = 254;
uint16_t RgbcwStripDriver::sLastOnCtMireds = kDefaultCtMireds;
bool RgbcwStripDriver::sLastOnUseCt      = false;
uint8_t RgbcwStripDriver::sLastOnHue     = 0;
uint8_t RgbcwStripDriver::sLastOnSat     = 0;
uint8_t RgbcwStripDriver::sDisplayR    = 0;
uint8_t RgbcwStripDriver::sDisplayG    = 0;
uint8_t RgbcwStripDriver::sDisplayB    = 0;
uint8_t RgbcwStripDriver::sDisplayC    = 0;
uint8_t RgbcwStripDriver::sDisplayW    = 0;
uint8_t RgbcwStripDriver::sFadeStartR  = 0;
uint8_t RgbcwStripDriver::sFadeStartG  = 0;
uint8_t RgbcwStripDriver::sFadeStartB  = 0;
uint8_t RgbcwStripDriver::sFadeStartC  = 0;
uint8_t RgbcwStripDriver::sFadeStartW  = 0;
uint8_t RgbcwStripDriver::sFadeTargetR = 0;
uint8_t RgbcwStripDriver::sFadeTargetG = 0;
uint8_t RgbcwStripDriver::sFadeTargetB = 0;
uint8_t RgbcwStripDriver::sFadeTargetC = 0;
uint8_t RgbcwStripDriver::sFadeTargetW = 0;
RgbcwStripDriver::FadeKind RgbcwStripDriver::sFadeKind = FadeKind::kOnOff;
uint16_t RgbcwStripDriver::sFadeStep      = 0;
uint16_t RgbcwStripDriver::sFadeStepsTotal = 0;
bool RgbcwStripDriver::sFadeActive        = false;

namespace {
constexpr uint32_t kFadeTickMs          = 10;
constexpr uint16_t kFadeDurationOnMs    = 500;
constexpr uint16_t kFadeDurationOffMs   = 350;
constexpr uint16_t kFadeDurationLevelMs = 320;
constexpr uint16_t kFadeDurationColorMs = 650;
constexpr uint32_t kFadeFpOne = 65535U;

// TODO: 配置 SPI 外设与数据线引脚，实现 GRBW/RGBCW 帧发送。
void SpiStripWrite(uint8_t r, uint8_t g, uint8_t b, uint8_t c, uint8_t w)
{
    (void) r;
    (void) g;
    (void) b;
    (void) c;
    (void) w;
    ChipLogDetail(AppServer, "RgbcwStrip SPI TODO: r=%u g=%u b=%u c=%u w=%u", r, g, b, c, w);
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
    const uint16_t ct = std::clamp(mireds, RgbcwStripDriver::kCtMinMireds, RgbcwStripDriver::kCtMaxMireds);
    const uint32_t warmWeight = ct - RgbcwStripDriver::kCtMinMireds;
    const uint32_t sum        = (RgbcwStripDriver::kCtMaxMireds - RgbcwStripDriver::kCtMinMireds);
    if (sum == 0)
    {
        cool = brightness / 2;
        warm = brightness - cool;
        return;
    }
    warm = static_cast<uint8_t>((brightness * warmWeight) / sum);
    cool = brightness - warm;
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

} // namespace

void RgbcwStripDriver::Init()
{
    // TODO: sl_spidrv_init / DMA 等
    SetOn(false);
}

uint8_t RgbcwStripDriver::LevelToBrightnessPercent(uint8_t level)
{
    if (level == 0)
    {
        return 0;
    }
    return static_cast<uint8_t>((static_cast<uint32_t>(level) * 100U) / 254U);
}

void RgbcwStripDriver::ApplyOutputImmediate()
{
    CancelFadeTimer();

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t c = 0;
    uint8_t w = 0;
    ComputeTargetOutput(r, g, b, c, w);
    ApplyDisplayOutput(r, g, b, c, w);
}

void RgbcwStripDriver::ComputeTargetOutput(uint8_t & r, uint8_t & g, uint8_t & b, uint8_t & c, uint8_t & w)
{
    r = 0;
    g = 0;
    b = 0;
    c = 0;
    w = 0;

    if (!sOn)
    {
        return;
    }

    const uint8_t br = LevelToBrightnessPercent(sLevel);
    if (sUseCt)
    {
        CtToCoolWarm(sCtMireds, br, c, w);
    }
    else
    {
        HsvToRgb(sHue, sSat, br, r, g, b);
    }
}

void RgbcwStripDriver::ApplyDisplayOutput(uint8_t r, uint8_t g, uint8_t b, uint8_t c, uint8_t w)
{
    SpiStripWrite(r, g, b, c, w);
    sDisplayR = r;
    sDisplayG = g;
    sDisplayB = b;
    sDisplayC = c;
    sDisplayW = w;
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
    ApplyDisplayOutput(InterpolateDuty(sFadeStartR, sFadeTargetR, step, sFadeStepsTotal),
                       InterpolateDuty(sFadeStartG, sFadeTargetG, step, sFadeStepsTotal),
                       InterpolateDuty(sFadeStartB, sFadeTargetB, step, sFadeStepsTotal),
                       InterpolateDuty(sFadeStartC, sFadeTargetC, step, sFadeStepsTotal),
                       InterpolateDuty(sFadeStartW, sFadeTargetW, step, sFadeStepsTotal));
}

void RgbcwStripDriver::OnFadeTimer(chip::System::Layer * layer, void * appState)
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
    uint8_t targetR = 0;
    uint8_t targetG = 0;
    uint8_t targetB = 0;
    uint8_t targetC = 0;
    uint8_t targetW = 0;
    ComputeTargetOutput(targetR, targetG, targetB, targetC, targetW);

    sFadeTargetR = targetR;
    sFadeTargetG = targetG;
    sFadeTargetB = targetB;
    sFadeTargetC = targetC;
    sFadeTargetW = targetW;

    if (kind == FadeKind::kColor && sFadeActive && sFadeKind == FadeKind::kColor && !restartFade)
    {
        if (sDisplayR == sFadeTargetR && sDisplayG == sFadeTargetG && sDisplayB == sFadeTargetB &&
            sDisplayC == sFadeTargetC && sDisplayW == sFadeTargetW)
        {
            CancelFadeTimer();
        }
        return;
    }

    if (!sFadeActive || restartFade || sFadeKind != kind)
    {
        sFadeStartR = sDisplayR;
        sFadeStartG = sDisplayG;
        sFadeStartB = sDisplayB;
        sFadeStartC = sDisplayC;
        sFadeStartW = sDisplayW;
    }
    sFadeKind = kind;

    if (sFadeStartR == sFadeTargetR && sFadeStartG == sFadeTargetG && sFadeStartB == sFadeTargetB &&
        sFadeStartC == sFadeTargetC && sFadeStartW == sFadeTargetW)
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
}

void RgbcwStripDriver::SetOn(bool on)
{
    if (!on && sOn)
    {
        sLastOnValid    = true;
        sLastOnLevel    = sLevel;
        sLastOnCtMireds = sCtMireds;
        sLastOnUseCt    = sUseCt;
        sLastOnHue      = sHue;
        sLastOnSat      = sSat;
    }

    if (on && !sOn && sLastOnValid)
    {
        sLevel    = sLastOnLevel;
        sCtMireds = sLastOnCtMireds;
        sUseCt    = sLastOnUseCt;
        sHue      = sLastOnHue;
        sSat      = sLastOnSat;
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
    sHue   = hue;
    sSat   = saturation;
    sUseCt = false;
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

void RgbcwStripDriver::SetColorTemperatureMireds(uint16_t mireds)
{
    sCtMireds = std::clamp(mireds, kCtMinMireds, kCtMaxMireds);
    sUseCt    = true;
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

    sOn    = on;
    sLevel = level;
    sHue   = hue;
    sSat   = sat;
    sCtMireds = ct;
    sUseCt = (colorMode == ColorControl::ColorModeEnum::kColorTemperatureMireds);
    if (sOn)
    {
        sLastOnValid    = true;
        sLastOnLevel    = sLevel;
        sLastOnCtMireds = sCtMireds;
        sLastOnUseCt    = sUseCt;
        sLastOnHue      = sHue;
        sLastOnSat      = sSat;
    }
    ApplyOutputImmediate();
}

void RgbcwStripDriver::ForceOffForFault()
{
    CancelFadeTimer();
    sOn = false;
    sDisplayR = 0;
    sDisplayG = 0;
    sDisplayB = 0;
    sDisplayC = 0;
    sDisplayW = 0;
    SpiStripWrite(0, 0, 0, 0, 0);
}

void RgbcwStripDriver::ForceOffForFaultFromIsr()
{
    sOn = false;
    sFadeActive = false;
    sDisplayR = 0;
    sDisplayG = 0;
    sDisplayB = 0;
    sDisplayC = 0;
    sDisplayW = 0;
    SpiStripWrite(0, 0, 0, 0, 0);
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
    sUseCt    = true;
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
