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
    static void OnAdcSample(uint32_t millivolts, bool sample_valid);

    /** PB00 short-circuit ISR: PWM off immediately; Matter off deferred to task context. */
    static void TripFromIsr();

    static bool IsFaultActive() { return fault_active_; }
    static bool BlocksTurnOn() { return fault_active_; }
    static uint32_t AverageMillivolts() { return avg_millivolts_; }

private:
    static void UpdateAverage(uint32_t millivolts);
    static bool IsGpioOk();
    static void Trip();
    static void Recover();
    static void ProcessDeferredTrip();
    static void SavePreFaultSnapshot();

#if !DIMMABLE_LIGHT_ADC_PROTECT_ENABLE
    static void OnProtectionPoll();
    static void OnProtectionPollTimer(chip::System::Layer* layer, void* app_state);
    static void StartProtectionPoll();
    static bool poll_active_;
#endif

    static bool fault_active_;
    static bool snap_valid_;
    static bool snap_on_;
    static uint8_t snap_level_;
    static uint16_t snap_ct_mireds_;
    static bool matter_off_pending_;
    static uint32_t avg_millivolts_;
    static uint32_t recovery_ms_;
    static uint32_t sample_ring_[OVERCURRENT_AVG_SAMPLES];
    static uint8_t sample_count_;
    static uint8_t sample_index_;
};
