/***************************************************************************//**
 * @file
 * @brief Voltage ADC pin and scaling (user-maintained; align with Pin Tool IADC0).
 *
 * Pin routing: IADC0 SCAN0POS on PA00 (scan table entry 0).
 * Installing Software Component "IADC" does NOT add IADC0 to Pin Configurator UI
 * (no instantiable IADC driver). Use pintool PORTIO or edit voltage_adc_config.h here.
 * After slc Generate, pin_config.h should define IADC0_SCAN0POS_PORT/PIN.
 *
 * Reference (pick one use case):
 *   VOLTAGE_ADC_REFERENCE_VDDX=0: internal 1.2V ref; pin must stay <= ~1.2V (use divider for 3.3V).
 *   VOLTAGE_ADC_REFERENCE_VDDX=1: VDDA ref; pin may equal supply (e.g. 3.3V on PA00).
 *
 * External divider scaling (after adcMv):
 *   Vin_mV = adcMv * VOLTAGE_ADC_INPUT_SCALE_NUM / VOLTAGE_ADC_INPUT_SCALE_DEN
 ******************************************************************************/
#ifndef VOLTAGE_ADC_CONFIG_H
#define VOLTAGE_ADC_CONFIG_H

#include "sl_gpio.h"

#ifndef VOLTAGE_ADC_GPIO_PORT
#define VOLTAGE_ADC_GPIO_PORT SL_GPIO_PORT_A
#endif
#ifndef VOLTAGE_ADC_GPIO_PIN
#define VOLTAGE_ADC_GPIO_PIN 0
#endif

/** Scan table index matching IADC0 SCAN0POS (entry 0). */
#ifndef VOLTAGE_ADC_SCAN_ENTRY_ID
#define VOLTAGE_ADC_SCAN_ENTRY_ID 0
#endif

/** 0 = 1.2V internal ref; 1 = VDDA (direct 0..3.3V on sense pin). */
#ifndef VOLTAGE_ADC_REFERENCE_VDDX
#define VOLTAGE_ADC_REFERENCE_VDDX 1
#endif
#ifndef VOLTAGE_ADC_VREF_MV
#define VOLTAGE_ADC_VREF_MV 3300
#endif

#ifndef VOLTAGE_ADC_INPUT_SCALE_NUM
#define VOLTAGE_ADC_INPUT_SCALE_NUM 1
#endif
#ifndef VOLTAGE_ADC_INPUT_SCALE_DEN
#define VOLTAGE_ADC_INPUT_SCALE_DEN 1
#endif

/** Periodic sample/log interval (ms). */
#ifndef VOLTAGE_ADC_SAMPLE_PERIOD_MS
#define VOLTAGE_ADC_SAMPLE_PERIOD_MS 1000
#endif

#endif // VOLTAGE_ADC_CONFIG_H
