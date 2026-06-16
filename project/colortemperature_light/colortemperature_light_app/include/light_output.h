#pragma once

#include "ct_dual_pwm_driver.h"

namespace light_output {

inline void Init() { CtDualPwmDriver::Init(); }
inline void SetOn(bool on) { CtDualPwmDriver::SetOn(on); }
inline void SetLevel(uint8_t level) { CtDualPwmDriver::SetLevel(level); }
inline void SetColorTemperatureMireds(uint16_t mireds) { CtDualPwmDriver::SetColorTemperatureMireds(mireds); }
inline void ApplyClusterLevel(chip::EndpointId ep, uint8_t level) { CtDualPwmDriver::ApplyClusterLevel(ep, level); }
inline void SyncFromMatterEndpoint(chip::EndpointId ep) { CtDualPwmDriver::SyncFromMatterEndpoint(ep); }
inline void ForceOffForFault() { CtDualPwmDriver::ForceOffForFault(); }
inline void ForceOffForFaultFromIsr() { CtDualPwmDriver::ForceOffForFaultFromIsr(); }
inline void RestoreToPreFault(bool on, uint8_t level, uint16_t ct_mireds)
{
    CtDualPwmDriver::RestoreToPreFault(on, level, ct_mireds);
}
inline bool GetPreFaultState(bool& on, uint8_t& level, uint16_t& ct_mireds)
{
    return CtDualPwmDriver::GetPreFaultState(on, level, ct_mireds);
}
inline uint8_t ResolveLevelForCluster(chip::EndpointId ep, bool on, uint8_t lv)
{
    return CtDualPwmDriver::ResolveLevelForCluster(ep, on, lv);
}
inline void ApplyProvisionReminderOutput(bool on) { CtDualPwmDriver::ApplyProvisionReminderOutput(on); }

static constexpr uint16_t kCtMinMireds     = CtDualPwmDriver::kCtMinMireds;
static constexpr uint16_t kCtMaxMireds     = CtDualPwmDriver::kCtMaxMireds;
static constexpr uint16_t kDefaultCtMireds = CtDualPwmDriver::kDefaultCtMireds;

} // namespace light_output
