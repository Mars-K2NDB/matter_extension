/***************************************************************************//**
 * @file
 * @brief Overcurrent protection thresholds (current sense via PA00 / IADC).
 *
 * Sense voltage is proportional to load current. Tune TRIP/CLEAR for your shunt.
 *
 * Optional GPIO_OK: when defined, pin must read OVERCURRENT_GPIO_OK_LEVEL for recovery.
 * When undefined, "GPIO OK" means ADC initialized and last conversion succeeded.
 ******************************************************************************/
#ifndef OVERCURRENT_PROTECT_CONFIG_H
#define OVERCURRENT_PROTECT_CONFIG_H

/** Trip when rolling average exceeds this (mV). */
#ifndef OVERCURRENT_TRIP_MV
#define OVERCURRENT_TRIP_MV 145
#endif

/** Recovery when average stays at or below this (mV). */
#ifndef OVERCURRENT_CLEAR_MV
#define OVERCURRENT_CLEAR_MV 130
#endif

/** Continuous time below CLEAR_MV required before auto-recovery (ms). */
#ifndef OVERCURRENT_RECOVERY_MS
#define OVERCURRENT_RECOVERY_MS 1000
#endif

/** ADC sample period for OCP (ms). */
#ifndef OVERCURRENT_SAMPLE_MS
#define OVERCURRENT_SAMPLE_MS 100
#endif

/** Rolling-average window length (samples). */
#ifndef OVERCURRENT_AVG_SAMPLES
#define OVERCURRENT_AVG_SAMPLES 8
#endif

/* Uncomment to require a GPIO high before recovery:
#define OVERCURRENT_GPIO_OK_PORT SL_GPIO_PORT_x
#define OVERCURRENT_GPIO_OK_PIN  n
#define OVERCURRENT_GPIO_OK_LEVEL 1
*/

#endif // OVERCURRENT_PROTECT_CONFIG_H
