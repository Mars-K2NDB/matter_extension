#pragma once

#include "rgbcw_pwm_driver.h"

namespace light_output {

inline void Init() { RgbcwPwmDriver::Init(); }
inline void SetOn(bool on) { RgbcwPwmDriver::SetOn(on); }
inline void SetLevel(uint8_t level) { RgbcwPwmDriver::SetLevel(level); }
inline void SetHueSat(uint8_t h, uint8_t s) { RgbcwPwmDriver::SetHueSat(h, s); }
inline void SetColorTemperatureMireds(uint16_t m) { RgbcwPwmDriver::SetColorTemperatureMireds(m); }
inline void ApplyClusterLevel(chip::EndpointId ep, uint8_t level) { RgbcwPwmDriver::ApplyClusterLevel(ep, level); }
inline void SyncFromMatterEndpoint(chip::EndpointId ep) { RgbcwPwmDriver::SyncFromMatterEndpoint(ep); }
inline void ForceOffForFault() { RgbcwPwmDriver::ForceOffForFault(); }
inline void ForceOffForFaultFromIsr() { RgbcwPwmDriver::ForceOffForFaultFromIsr(); }
inline void RestoreToPreFault(bool on, uint8_t level, uint16_t ct_mireds)
{
    RgbcwPwmDriver::RestoreToPreFault(on, level, ct_mireds);
}
inline bool GetPreFaultState(bool& on, uint8_t& level, uint16_t& ct_mireds)
{
    return RgbcwPwmDriver::GetPreFaultState(on, level, ct_mireds);
}
inline uint8_t ResolveLevelForCluster(chip::EndpointId ep, bool on, uint8_t lv)
{
    return RgbcwPwmDriver::ResolveLevelForCluster(ep, on, lv);
}
inline void ApplyProvisionReminderOutput(bool on) { RgbcwPwmDriver::ApplyProvisionReminderOutput(on); }

static constexpr uint16_t kCtMinMireds     = RgbcwPwmDriver::kCtMinMireds;
static constexpr uint16_t kCtMaxMireds     = RgbcwPwmDriver::kCtMaxMireds;
static constexpr uint16_t kDefaultCtMireds = RgbcwPwmDriver::kDefaultCtMireds;

} // namespace light_output
