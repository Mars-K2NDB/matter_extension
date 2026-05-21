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

    /** Overcurrent trip: PWM duty 0, block SetOn(true) until fault clears. */
    static void ForceOffForFault();

private:
    static void ApplyOutput();
    static uint8_t LevelToBrightnessPercent(uint8_t level);
    /** Map Matter level to PWM brightness; handles on-at-min-level (CurrentLevel==1). */
    static uint8_t ResolveLevelForPwm(chip::EndpointId endpoint, bool on, uint8_t clusterLevel);
    /** Like ResolveLevelForPwm; caller must hold the chip stack lock. */
    static uint8_t ResolveLevelForPwmLocked(chip::EndpointId endpoint, bool on, uint8_t clusterLevel);

    static bool sOn;
    static uint8_t sLevel;
    static uint16_t sCtMireds;
    static bool sPwmStarted;
};
