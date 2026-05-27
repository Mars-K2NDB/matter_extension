#pragma once

#include "CtDualPwmDriver.h"

namespace LightOutput {

inline void Init() { CtDualPwmDriver::Init(); }
inline void SetOn(bool on) { CtDualPwmDriver::SetOn(on); }
inline void SetLevel(uint8_t level) { CtDualPwmDriver::SetLevel(level); }
inline void SetColorTemperatureMireds(uint16_t mireds) { CtDualPwmDriver::SetColorTemperatureMireds(mireds); }
inline void ApplyClusterLevel(chip::EndpointId ep, uint8_t level) { CtDualPwmDriver::ApplyClusterLevel(ep, level); }
inline void SyncFromMatterEndpoint(chip::EndpointId ep) { CtDualPwmDriver::SyncFromMatterEndpoint(ep); }
inline void ForceOffForFault() { CtDualPwmDriver::ForceOffForFault(); }
inline void ForceOffForFaultFromIsr() { CtDualPwmDriver::ForceOffForFaultFromIsr(); }
inline void RestoreToPreFault(bool on, uint8_t level, uint16_t ct) { CtDualPwmDriver::RestoreToPreFault(on, level, ct); }
inline bool GetPreFaultState(bool & on, uint8_t & level, uint16_t & ct) { return CtDualPwmDriver::GetPreFaultState(on, level, ct); }
inline uint8_t ResolveLevelForPwmLocked(chip::EndpointId ep, bool on, uint8_t lv)
{
    return CtDualPwmDriver::ResolveLevelForPwmLocked(ep, on, lv);
}

static constexpr uint16_t kCtMinMireds     = CtDualPwmDriver::kCtMinMireds;
static constexpr uint16_t kCtMaxMireds     = CtDualPwmDriver::kCtMaxMireds;
static constexpr uint16_t kDefaultCtMireds = CtDualPwmDriver::kDefaultCtMireds;

} // namespace LightOutput
