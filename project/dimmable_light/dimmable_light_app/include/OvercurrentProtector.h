/*
 * Overcurrent protection from IADC sense voltage (shunt / current mirror on PA00).
 */

#pragma once

#include "adc_protect_config.h"
#include "overcurrent_protect_config.h"

#include <cstdint>

namespace chip {
namespace System {
class Layer;
} // namespace System
} // namespace chip

class OvercurrentProtector
{
public:
    static void Init();
    static void OnAdcSample(uint32_t millivolts, bool sampleValid);

    /** PB00 short-circuit ISR: PWM off immediately; Matter off deferred to task context. */
    static void TripFromIsr();

    static bool IsFaultActive() { return sFaultActive; }
    static bool BlocksTurnOn() { return sFaultActive; }
    static uint32_t AverageMillivolts() { return sAvgMillivolts; }

private:
    static void UpdateAverage(uint32_t millivolts);
    static bool IsGpioOk();
    static void Trip();
    static void Recover();
    static void ProcessDeferredTrip();
    static void SavePreFaultSnapshot();

#if !DIMMABLE_LIGHT_ADC_PROTECT_ENABLE
    static void OnProtectionPoll();
    static void OnProtectionPollTimer(chip::System::Layer * layer, void * appState);
    static void StartProtectionPoll();
    static bool sPollActive;
#endif

    static bool sFaultActive;
    static bool sSnapValid;
    static bool sSnapOn;
    static uint8_t sSnapLevel;
    static uint16_t sSnapCtMireds;
    static bool sMatterOffPending;
    static uint32_t sAvgMillivolts;
    static uint32_t sRecoveryMs;
    static uint32_t sSampleRing[OVERCURRENT_AVG_SAMPLES];
    static uint8_t sSampleCount;
    static uint8_t sSampleIndex;
};
