/*
 * Five-channel RGBCW PWM driver (R/G/B on PC0-2, C/W on PB4-5).
 */

#pragma once

#include <cstdint>
#include <lib/core/DataModelTypes.h>

class RgbcwPwmDriver
{
public:
    static constexpr uint16_t kCtMinMireds     = 111;
    static constexpr uint16_t kCtMaxMireds     = 500;
    static constexpr uint16_t kDefaultCtMireds = 250;

    static void Init();
    static void SetOn(bool on);
    static void SetLevel(uint8_t level);
    static void SetHueSat(uint8_t hue, uint8_t saturation);
    static void SetColorTemperatureMireds(uint16_t mireds);
    static void ApplyClusterLevel(chip::EndpointId endpoint, uint8_t clusterLevel);
    static void SyncFromMatterEndpoint(chip::EndpointId endpoint);
    static void ForceOffForFault();
    static void ForceOffForFaultFromIsr();
    static void RecoverFromFault();
    static void RestoreToPreFault(bool on, uint8_t level, uint16_t ctMireds);
    static bool GetPreFaultState(bool & on, uint8_t & level, uint16_t & ctMireds);
    static uint8_t ResolveLevelForPwmLocked(chip::EndpointId endpoint, bool on, uint8_t clusterLevel);

private:
    static void SaveStateBeforeFault();
    static void PwmOutputKillRegisters();
    static void PwmOutputRestoreRegisters();
    static void ApplyOutputImmediate();
    static uint8_t LevelToBrightnessPercent(uint8_t level);

    static bool sOn;
    static uint8_t sLevel;
    static uint8_t sHue;
    static uint8_t sSat;
    static uint16_t sCtMireds;
    static bool sUseCt;
    static bool sPwmStarted;
    static bool sRouteDisabled;
    static bool sPreFaultSaved;
    static bool sPreFaultOn;
    static uint8_t sPreFaultLevel;
    static uint16_t sPreFaultCtMireds;
    static bool sPreFaultUseCt;
    static uint8_t sPreFaultHue;
    static uint8_t sPreFaultSat;
};
