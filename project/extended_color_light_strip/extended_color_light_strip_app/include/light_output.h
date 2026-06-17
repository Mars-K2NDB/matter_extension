#pragma once

#include "rgbcw_strip_driver.h"
#include "ws2814_strip_effects.h"

namespace light_output {

inline void Init()
{
    ws2814_effects::Init();
    RgbcwStripDriver::Init();
}

inline bool IsStripEffectRunning() { return ws2814_effects::IsRunning(); }
inline bool StartStripEffect(ws2814_effects::Mode mode, uint8_t speed = 128, uint8_t intensity = 128)
{
    return ws2814_effects::Start(mode, speed, intensity);
}
inline bool StopStripEffect() { return ws2814_effects::Stop(); }
inline bool NextStripEffect() { return ws2814_effects::NextMode(); }

inline void SetOn(chip::EndpointId ep, bool on)
{
    if (!on)
    {
        (void) ws2814_effects::Stop();
    }
    RgbcwStripDriver::SetOn(ep, on);
}
inline void SetLevel(uint8_t level)
{
    (void) ws2814_effects::Stop();
    RgbcwStripDriver::SetLevel(level);
}
inline void SetHueSat(uint8_t hue, uint8_t saturation)
{
    (void) ws2814_effects::Stop();
    RgbcwStripDriver::SetHueSat(hue, saturation);
}
inline void SetXy(uint16_t x, uint16_t y)
{
    (void) ws2814_effects::Stop();
    RgbcwStripDriver::SetXy(x, y);
}
inline void SetColorTemperatureMireds(uint16_t mireds)
{
    (void) ws2814_effects::Stop();
    RgbcwStripDriver::SetColorTemperatureMireds(mireds);
}
inline void ApplyClusterLevel(chip::EndpointId endpoint, uint8_t level)
{
    (void) ws2814_effects::Stop();
    RgbcwStripDriver::ApplyClusterLevel(endpoint, level);
}
inline void SyncFromMatterEndpoint(chip::EndpointId endpoint)
{
    (void) ws2814_effects::Stop();
    RgbcwStripDriver::SyncFromMatterEndpoint(endpoint);
}
inline void ForceOffForFault() { RgbcwStripDriver::ForceOffForFault(); }
inline void ForceOffForFaultFromIsr() { RgbcwStripDriver::ForceOffForFaultFromIsr(); }
inline void RestoreToPreFault(bool on, uint8_t level, uint16_t ct_mireds)
{
    RgbcwStripDriver::RestoreToPreFault(on, level, ct_mireds);
}
inline bool GetPreFaultState(bool& on, uint8_t& level, uint16_t& ct_mireds)
{
    return RgbcwStripDriver::GetPreFaultState(on, level, ct_mireds);
}
inline uint8_t ResolveLevelForCluster(chip::EndpointId endpoint, bool on, uint8_t cluster_level)
{
    return RgbcwStripDriver::ResolveLevelForCluster(endpoint, on, cluster_level);
}

static constexpr uint16_t kCtMinMireds     = RgbcwStripDriver::kCtMinMireds;
static constexpr uint16_t kCtMaxMireds     = RgbcwStripDriver::kCtMaxMireds;
static constexpr uint16_t kDefaultCtMireds = RgbcwStripDriver::kDefaultCtMireds;

} // namespace light_output
