#pragma once

#include "single_pwm_driver.h"

namespace light_output {

inline void Init() { SinglePwmDriver::Init(); }
inline void SetOn(bool on) { SinglePwmDriver::SetOn(on); }
inline void SetLevel(uint8_t level) { SinglePwmDriver::SetLevel(level); }
inline void ApplyClusterLevel(chip::EndpointId ep, uint8_t level) { SinglePwmDriver::ApplyClusterLevel(ep, level); }
inline void SyncFromMatterEndpoint(chip::EndpointId ep) { SinglePwmDriver::SyncFromMatterEndpoint(ep); }
inline void ForceOffForFault() { SinglePwmDriver::ForceOffForFault(); }
inline void ForceOffForFaultFromIsr() { SinglePwmDriver::ForceOffForFaultFromIsr(); }
inline void RestoreToPreFault(bool on, uint8_t level, uint16_t /*ct*/) { SinglePwmDriver::RestoreToPreFault(on, level); }
inline bool GetPreFaultState(bool& on, uint8_t& level, uint16_t& /*ct*/)
{
    return SinglePwmDriver::GetPreFaultState(on, level);
}
inline uint8_t ResolveLevelForCluster(chip::EndpointId ep, bool on, uint8_t lv)
{
    return SinglePwmDriver::ResolveLevelForCluster(ep, on, lv);
}

static constexpr uint16_t kCtMinMireds     = 111;
static constexpr uint16_t kCtMaxMireds     = 500;
static constexpr uint16_t kDefaultCtMireds = 250;

} // namespace light_output
