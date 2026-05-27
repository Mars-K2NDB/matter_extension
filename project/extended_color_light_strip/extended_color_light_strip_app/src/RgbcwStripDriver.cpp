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

namespace {

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
    if (!sOn)
    {
        SpiStripWrite(0, 0, 0, 0, 0);
        return;
    }

    const uint8_t br = LevelToBrightnessPercent(sLevel);
    if (sUseCt)
    {
        uint8_t cool = 0;
        uint8_t warm = 0;
        CtToCoolWarm(sCtMireds, br, cool, warm);
        SpiStripWrite(0, 0, 0, cool, warm);
    }
    else
    {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        HsvToRgb(sHue, sSat, br, r, g, b);
        SpiStripWrite(r, g, b, 0, 0);
    }
}

void RgbcwStripDriver::SetOn(bool on)
{
    sOn = on;
    ApplyOutputImmediate();
}

void RgbcwStripDriver::SetLevel(uint8_t level)
{
    sLevel = level;
    ApplyOutputImmediate();
}

void RgbcwStripDriver::SetHueSat(uint8_t hue, uint8_t saturation)
{
    sHue   = hue;
    sSat   = saturation;
    sUseCt = false;
    ApplyOutputImmediate();
}

void RgbcwStripDriver::SetColorTemperatureMireds(uint16_t mireds)
{
    sCtMireds = std::clamp(mireds, kCtMinMireds, kCtMaxMireds);
    sUseCt    = true;
    ApplyOutputImmediate();
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
    ApplyOutputImmediate();
}

void RgbcwStripDriver::ForceOffForFault()
{
    SpiStripWrite(0, 0, 0, 0, 0);
}

void RgbcwStripDriver::ForceOffForFaultFromIsr()
{
    ForceOffForFault();
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
