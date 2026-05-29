/***************************************************************************//**
 * @file
 * @brief ADC overcurrent protection feature switch (PA00 / IADC).
 *
 * Set DIMMABLE_LIGHT_ADC_PROTECT_ENABLE to 0 to disable ADC sampling and
 * overcurrent trip/recovery. PB00 short-circuit interrupt protection is unchanged.
 ******************************************************************************/
#ifndef ADC_PROTECT_CONFIG_H
#define ADC_PROTECT_CONFIG_H

/** 1 = enable ADC overcurrent protection; 0 = disable. */
#ifndef DIMMABLE_LIGHT_ADC_PROTECT_ENABLE
#define DIMMABLE_LIGHT_ADC_PROTECT_ENABLE 0
#endif

#endif // ADC_PROTECT_CONFIG_H
