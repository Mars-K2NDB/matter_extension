/*
 * Dual-channel PWM driver for Matter color-temperature lighting.
 * PWM0 = cool (cold), PWM1 = warm.
 */

#pragma once

#include <cstdint>
#include <lib/core/DataModelTypes.h>

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
    static void ApplyClusterLevel(chip::EndpointId endpoint, uint8_t clusterLevel);

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
    static void RestoreToPreFault(bool on, uint8_t level, uint16_t ctMireds);

    /** Driver-only snapshot on trip; false if none. */
    static bool GetPreFaultState(bool & on, uint8_t & level, uint16_t & ctMireds);

    /** Caller must hold the chip stack lock. Maps min Matter level (1) to PWM brightness level. */
    static uint8_t ResolveLevelForPwmLocked(chip::EndpointId endpoint, bool on, uint8_t clusterLevel);

private:
    static void SaveStateBeforeFault();
    static void PwmOutputRestoreRegisters();
    static void ApplyOutput();
    static uint8_t LevelToBrightnessPercent(uint8_t level);
    static uint8_t ResolveLevelForPwm(chip::EndpointId endpoint, bool on, uint8_t clusterLevel);

    static bool sOn;
    static uint8_t sLevel;
    static uint16_t sCtMireds;
    static bool sPwmStarted;
    static bool sRouteDisabled;
    static bool sPreFaultSaved;
    static bool sPreFaultOn;
    static uint8_t sPreFaultLevel;
    static uint16_t sPreFaultCtMireds;
};
