#pragma once

#include "RgbcwStripDriver.h"

namespace LightOutput {

inline void Init() { RgbcwStripDriver::Init(); }
inline void SetOn(bool on) { RgbcwStripDriver::SetOn(on); }
inline void SetLevel(uint8_t level) { RgbcwStripDriver::SetLevel(level); }
inline void SetHueSat(uint8_t h, uint8_t s) { RgbcwStripDriver::SetHueSat(h, s); }
inline void SetColorTemperatureMireds(uint16_t m) { RgbcwStripDriver::SetColorTemperatureMireds(m); }
inline void ApplyClusterLevel(chip::EndpointId ep, uint8_t level) { RgbcwStripDriver::ApplyClusterLevel(ep, level); }
inline void SyncFromMatterEndpoint(chip::EndpointId ep) { RgbcwStripDriver::SyncFromMatterEndpoint(ep); }
inline void ForceOffForFault() { RgbcwStripDriver::ForceOffForFault(); }
inline void ForceOffForFaultFromIsr() { RgbcwStripDriver::ForceOffForFaultFromIsr(); }
inline void RestoreToPreFault(bool on, uint8_t level, uint16_t ct) { RgbcwStripDriver::RestoreToPreFault(on, level, ct); }
inline bool GetPreFaultState(bool & on, uint8_t & level, uint16_t & ct) { return RgbcwStripDriver::GetPreFaultState(on, level, ct); }
inline uint8_t ResolveLevelForPwmLocked(chip::EndpointId ep, bool on, uint8_t lv)
{
    return RgbcwStripDriver::ResolveLevelForPwmLocked(ep, on, lv);
}

static constexpr uint16_t kCtMinMireds     = RgbcwStripDriver::kCtMinMireds;
static constexpr uint16_t kCtMaxMireds     = RgbcwStripDriver::kCtMaxMireds;
static constexpr uint16_t kDefaultCtMireds = RgbcwStripDriver::kDefaultCtMireds;

} // namespace LightOutput
