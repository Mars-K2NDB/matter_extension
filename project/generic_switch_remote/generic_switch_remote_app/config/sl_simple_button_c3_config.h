#ifndef SL_SIMPLE_BUTTON_C3_CONFIG_H
#define SL_SIMPLE_BUTTON_C3_CONFIG_H

#include "sl_gpio.h"
#include "sl_simple_button.h"

#define SL_SIMPLE_BUTTON_C3_MODE SL_SIMPLE_BUTTON_MODE_POLL_AND_DEBOUNCE

// <<< sl:start pin_tool >>>
// PC03: Generic Switch 按键 c3 (低电平有效)
#ifndef SL_SIMPLE_BUTTON_C3_PORT
#define SL_SIMPLE_BUTTON_C3_PORT SL_GPIO_PORT_C
#endif
#ifndef SL_SIMPLE_BUTTON_C3_PIN
#define SL_SIMPLE_BUTTON_C3_PIN 3
#endif
// <<< sl:end pin_tool >>>

#endif // SL_SIMPLE_BUTTON_C3_CONFIG_H
