/*
 * IADC scan-channel driver for voltage sense (PA00 / IADC0 SCAN0POS by default).
 */

#pragma once

#include <cstdint>

namespace chip {
namespace System {
class Layer;
} // namespace System
} // namespace chip

class VoltageAdcDriver
{
public:
    static void Init();
    static uint32_t ReadMillivolts();
    /** Returns false on conversion timeout (sample not used for OCP). */
    static bool TryReadMillivolts(uint32_t & millivoltsOut);
    static bool IsInitialized() { return sInitialized; }

    /** Start periodic sampling for overcurrent protection. No-op if Init failed. */
    static void StartPeriodicSampling();
    static void StopPeriodicSampling();

private:
    static void OnPeriodicSampleTimer(chip::System::Layer* layer, void* app_state);

    static bool sInitialized;
    static bool sPeriodicActive;
};
