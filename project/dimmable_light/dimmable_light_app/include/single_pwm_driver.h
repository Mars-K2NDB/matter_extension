/*
 * Single-channel PWM driver for Matter dimmable light (PB4).
 */

#pragma once

#include <cstdint>
#include <lib/core/DataModelTypes.h>

namespace chip {
namespace System {
class Layer;
} // namespace System
} // namespace chip

class SinglePwmDriver
{
public:
    static void Init();
    static void SetOn(bool on);
    static void SetLevel(uint8_t level);
    static void ApplyClusterLevel(chip::EndpointId endpoint, uint8_t cluster_level);
    static void SyncFromMatterEndpoint(chip::EndpointId endpoint);
    static void ForceOffForFault();
    static void ForceOffForFaultFromIsr();
    static void RecoverFromFault();
    static void RestoreToPreFault(bool on, uint8_t level);
    static bool GetPreFaultState(bool& on, uint8_t& level);
    static uint8_t ResolveLevelForCluster(chip::EndpointId endpoint, bool on, uint8_t cluster_level);

    /** Provisioning reminder overlay; does not change Matter on/level state. */
    static void ApplyProvisionReminderOutput(bool on);

private:
    static void SaveStateBeforeFault();
    static void PwmOutputKillRegisters();
    static void PwmOutputRestoreRegisters();
    static void CancelFadeTimer();
    static void ScheduleFade(bool restart_fade);
    static void ApplyFadeFrame(uint16_t step);
    static void OnFadeTimer(chip::System::Layer* layer, void* app_state);
    static void ApplyOutputImmediate();
    static uint8_t LevelToBrightnessPercent(uint8_t level);

    static bool on_;
    static uint8_t level_;
    static bool pwm_started_;
    static bool route_disabled_;
    static bool pre_fault_saved_;
    static bool pre_fault_on_;
    static uint8_t pre_fault_level_;
    static uint8_t display_duty_;
    static uint8_t fade_start_duty_;
    static uint8_t fade_target_duty_;
    static uint16_t fade_step_;
    static uint16_t fade_steps_total_;
    static bool fade_active_;
};
