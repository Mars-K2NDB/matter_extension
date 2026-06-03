#pragma once

#include "rgbcw_strip_driver.h"

namespace light_output {

inline void Init() { RgbcwStripDriver::Init(); }
inline void SetOn(bool on) { RgbcwStripDriver::SetOn(on); }
inline void SetLevel(uint8_t level) { RgbcwStripDriver::SetLevel(level); }
inline void SetHueSat(uint8_t hue, uint8_t saturation) { RgbcwStripDriver::SetHueSat(hue, saturation); }
inline void SetXy(uint16_t x, uint16_t y) { RgbcwStripDriver::SetXy(x, y); }
inline void SetColorTemperatureMireds(uint16_t mireds) { RgbcwStripDriver::SetColorTemperatureMireds(mireds); }
inline void ApplyClusterLevel(chip::EndpointId endpoint, uint8_t level)
{
    RgbcwStripDriver::ApplyClusterLevel(endpoint, level);
}
inline void SyncFromMatterEndpoint(chip::EndpointId endpoint) { RgbcwStripDriver::SyncFromMatterEndpoint(endpoint); }
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
