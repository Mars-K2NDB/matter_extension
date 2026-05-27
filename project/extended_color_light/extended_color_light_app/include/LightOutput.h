#pragma once

#include "RgbcwPwmDriver.h"

namespace LightOutput {

inline void Init() { RgbcwPwmDriver::Init(); }
inline void SetOn(bool on) { RgbcwPwmDriver::SetOn(on); }
inline void SetLevel(uint8_t level) { RgbcwPwmDriver::SetLevel(level); }
inline void SetHueSat(uint8_t h, uint8_t s) { RgbcwPwmDriver::SetHueSat(h, s); }
inline void SetColorTemperatureMireds(uint16_t m) { RgbcwPwmDriver::SetColorTemperatureMireds(m); }
inline void ApplyClusterLevel(chip::EndpointId ep, uint8_t level) { RgbcwPwmDriver::ApplyClusterLevel(ep, level); }
inline void SyncFromMatterEndpoint(chip::EndpointId ep) { RgbcwPwmDriver::SyncFromMatterEndpoint(ep); }
inline void ForceOffForFault() { RgbcwPwmDriver::ForceOffForFault(); }
inline void ForceOffForFaultFromIsr() { RgbcwPwmDriver::ForceOffForFaultFromIsr(); }
inline void RestoreToPreFault(bool on, uint8_t level, uint16_t ct) { RgbcwPwmDriver::RestoreToPreFault(on, level, ct); }
inline bool GetPreFaultState(bool & on, uint8_t & level, uint16_t & ct) { return RgbcwPwmDriver::GetPreFaultState(on, level, ct); }
inline uint8_t ResolveLevelForPwmLocked(chip::EndpointId ep, bool on, uint8_t lv)
{
    return RgbcwPwmDriver::ResolveLevelForPwmLocked(ep, on, lv);
}

static constexpr uint16_t kCtMinMireds     = RgbcwPwmDriver::kCtMinMireds;
static constexpr uint16_t kCtMaxMireds     = RgbcwPwmDriver::kCtMaxMireds;
static constexpr uint16_t kDefaultCtMireds = RgbcwPwmDriver::kDefaultCtMireds;

} // namespace LightOutput
