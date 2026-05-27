/*
 * Single-channel PWM driver for Matter dimmable light (PB4).
 */

#pragma once

#include <cstdint>
#include <lib/core/DataModelTypes.h>

class SinglePwmDriver
{
public:
    static void Init();
    static void SetOn(bool on);
    static void SetLevel(uint8_t level);
    static void ApplyClusterLevel(chip::EndpointId endpoint, uint8_t clusterLevel);
    static void SyncFromMatterEndpoint(chip::EndpointId endpoint);
    static void ForceOffForFault();
    static void ForceOffForFaultFromIsr();
    static void RecoverFromFault();
    static void RestoreToPreFault(bool on, uint8_t level);
    static bool GetPreFaultState(bool & on, uint8_t & level);
    static uint8_t ResolveLevelForPwmLocked(chip::EndpointId endpoint, bool on, uint8_t clusterLevel);

private:
    static void SaveStateBeforeFault();
    static void PwmOutputKillRegisters();
    static void PwmOutputRestoreRegisters();
    static void ApplyOutputImmediate();
    static uint8_t LevelToBrightnessPercent(uint8_t level);

    static bool sOn;
    static uint8_t sLevel;
    static bool sPwmStarted;
    static bool sRouteDisabled;
    static bool sPreFaultSaved;
    static bool sPreFaultOn;
    static uint8_t sPreFaultLevel;
};
