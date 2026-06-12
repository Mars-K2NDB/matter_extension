#ifndef SL_SIMPLE_BUTTON_C1_CONFIG_H
#define SL_SIMPLE_BUTTON_C1_CONFIG_H

#include "sl_gpio.h"
#include "sl_simple_button.h"

#define SL_SIMPLE_BUTTON_C1_MODE SL_SIMPLE_BUTTON_MODE_POLL_AND_DEBOUNCE

// <<< sl:start pin_tool >>>
// PC01: Generic Switch 按键 c1 (低电平有效)
#ifndef SL_SIMPLE_BUTTON_C1_PORT
#define SL_SIMPLE_BUTTON_C1_PORT SL_GPIO_PORT_C
#endif
#ifndef SL_SIMPLE_BUTTON_C1_PIN
#define SL_SIMPLE_BUTTON_C1_PIN 1
#endif
// <<< sl:end pin_tool >>>

#endif // SL_SIMPLE_BUTTON_C1_CONFIG_H
