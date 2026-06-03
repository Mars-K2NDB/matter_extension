/*
 * Dual-channel PWM driver for Matter color-temperature lighting.
 * PWM0 = cool (cold), PWM1 = warm.
 */

#pragma once

#include <cstdint>
#include <lib/core/DataModelTypes.h>

namespace chip {
namespace System {
class Layer;
} // namespace System
} // namespace chip

class CtDualPwmDriver
{
public:
    // Match lighting-thread-app.zap ColorTempPhysicalMin/MaxMireds.
    static constexpr uint16_t kCtMinMireds      = 111;
    static constexpr uint16_t kCtMaxMireds      = 500;
    static constexpr uint16_t kDefaultCtMireds  = 250;

    static void LogVersion();
    static void Init();
    static void SetOn(bool on);
    static void SetLevel(uint8_t level);
    static void SetColorTemperatureMireds(uint16_t mireds);

    /** Apply CurrentLevel from cluster; ignores min-level (1) reports while lamp is on. */
    static void ApplyClusterLevel(chip::EndpointId endpoint, uint8_t cluster_level);

    /**
     * Read OnOff / Level / CT and refresh PWM.
     * Caller must already hold the chip stack lock (e.g. Ember/ZCL callback).
     */
    static void RefreshFromMatterEndpoint(chip::EndpointId endpoint);

    /** Same as RefreshFromMatterEndpoint but acquires/releases chip stack lock (AppTask init). */
    static void SyncFromMatterEndpoint(chip::EndpointId endpoint);

    /** Overcurrent/short trip (task context): PWM off via fast register path. */
    static void ForceOffForFault();

    /** ISR-safe: TIMER compare 0 + route disable + GPIO forced off (fastest). */
    static void ForceOffForFaultFromIsr();

    /**
     * After fault clears: re-enable TIMER routes and restore on/level/CT to PWM.
     */
    static void RecoverFromFault();

    /** Restore PWM from explicit pre-fault light state (used on protection exit). */
    static void RestoreToPreFault(bool on, uint8_t level, uint16_t ct_mireds);

    /** Driver-only snapshot on trip; false if none. */
    static bool GetPreFaultState(bool& on, uint8_t& level, uint16_t& ct_mireds);

    /** Caller must hold the chip stack lock. Maps min Matter level (1) to PWM brightness level. */
    static uint8_t ResolveLevelForCluster(chip::EndpointId endpoint, bool on, uint8_t cluster_level);

private:
    enum class FadeKind : uint8_t
    {
        kOnOff = 0,
        kLevel = 1,
        kCt    = 2,
    };

    static void SaveStateBeforeFault();
    static void PwmOutputRestoreRegisters();
    static void ComputeFadeTargets(uint8_t& brightness, uint16_t& warmRatioFp);
    static void CaptureFadeStartFromDisplay();
    static void ApplyFadeFrame(uint16_t step);
    static void ApplyDisplayDuties(uint8_t coolDuty, uint8_t warmDuty);
    static void ApplyOutputImmediate();
    static void ScheduleFade(FadeKind kind, bool restart_fade);
    static void CancelFadeTimer();
    static void OnFadeTimer(chip::System::Layer* layer, void* app_state);
    static uint8_t LevelToBrightnessPercent(uint8_t level);
    static uint8_t ResolveLevelForCluster(chip::EndpointId endpoint, bool on, uint8_t cluster_level);

    static bool on_;
    static uint8_t level_;
    static uint16_t ct_mireds_;
    static bool pwm_started_;
    static bool route_disabled_;
    static bool pre_fault_saved_;
    static bool pre_fault_on_;
    static uint8_t pre_fault_level_;
    static uint16_t pre_fault_ct_mireds_;
    static bool last_on_valid_;
    static uint8_t last_on_level_;
    static uint16_t last_on_ct_mireds_;

    static uint8_t display_cool_duty_;
    static uint8_t display_warm_duty_;
    static uint8_t fade_start_brightness_;
    static uint8_t fade_target_brightness_;
    static uint16_t fade_start_warm_ratio_fp_;
    static uint16_t fade_target_warm_ratio_fp_;
    static FadeKind fade_kind_;
    static uint16_t fade_step_;
    static uint16_t fade_steps_total_;
    static bool fade_active_;
};
