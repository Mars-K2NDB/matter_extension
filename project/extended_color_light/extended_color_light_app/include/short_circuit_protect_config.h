/***************************************************************************//**
 * @file
 * @brief Short-circuit detect on PB00 (falling edge -> latch shared fault flag).
 ******************************************************************************/
#ifndef SHORT_CIRCUIT_PROTECT_CONFIG_H
#define SHORT_CIRCUIT_PROTECT_CONFIG_H

#include "sl_gpio.h"

#ifndef SHORT_CIRCUIT_GPIO_PORT
#define SHORT_CIRCUIT_GPIO_PORT SL_GPIO_PORT_B
#endif
#ifndef SHORT_CIRCUIT_GPIO_PIN
#define SHORT_CIRCUIT_GPIO_PIN 0
#endif

/** Pin high = not shorted; required for auto-recovery with ADC OCP. */
#ifndef SHORT_CIRCUIT_GPIO_OK_LEVEL
#define SHORT_CIRCUIT_GPIO_OK_LEVEL 1
#endif

#endif // SHORT_CIRCUIT_PROTECT_CONFIG_H
