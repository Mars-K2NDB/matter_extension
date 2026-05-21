/*
 * Overcurrent protection from IADC sense voltage (shunt / current mirror on PA00).
 */

#pragma once

#include "overcurrent_protect_config.h"

#include <cstdint>

class OvercurrentProtector
{
public:
    static void Init();
    static void OnAdcSample(uint32_t millivolts, bool sampleValid);

    static bool IsFaultActive() { return sFaultActive; }
    static bool BlocksTurnOn() { return sFaultActive; }
    static uint32_t AverageMillivolts() { return sAvgMillivolts; }

private:
    static void UpdateAverage(uint32_t millivolts);
    static bool IsGpioOk();
    static void Trip();
    static void Recover();

    static bool sFaultActive;
    static uint32_t sAvgMillivolts;
    static uint32_t sRecoveryMs;
    static uint32_t sSampleRing[OVERCURRENT_AVG_SAMPLES];
    static uint8_t sSampleCount;
    static uint8_t sSampleIndex;
};
