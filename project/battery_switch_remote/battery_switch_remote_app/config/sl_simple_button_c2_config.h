#ifndef SL_SIMPLE_BUTTON_C2_CONFIG_H
#define SL_SIMPLE_BUTTON_C2_CONFIG_H

#include "sl_gpio.h"
#include "sl_simple_button.h"

#define SL_SIMPLE_BUTTON_C2_MODE SL_SIMPLE_BUTTON_MODE_POLL_AND_DEBOUNCE

// <<< sl:start pin_tool >>>
// PC02: Battery Switch 按键 c2 (低电平有效)
#ifndef SL_SIMPLE_BUTTON_C2_PORT
#define SL_SIMPLE_BUTTON_C2_PORT SL_GPIO_PORT_C
#endif
#ifndef SL_SIMPLE_BUTTON_C2_PIN
#define SL_SIMPLE_BUTTON_C2_PIN 2
#endif
// <<< sl:end pin_tool >>>

#endif // SL_SIMPLE_BUTTON_C2_CONFIG_H
