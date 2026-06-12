#ifndef SL_SIMPLE_BUTTON_BTN0_CONFIG_H
#define SL_SIMPLE_BUTTON_BTN0_CONFIG_H

#include "sl_gpio.h"
#include "sl_simple_button.h"

#define SL_SIMPLE_BUTTON_BTN0_MODE SL_SIMPLE_BUTTON_MODE_POLL_AND_DEBOUNCE

// <<< sl:start pin_tool >>>
#ifndef SL_SIMPLE_BUTTON_BTN0_PORT
#define SL_SIMPLE_BUTTON_BTN0_PORT SL_GPIO_PORT_B
#endif
#ifndef SL_SIMPLE_BUTTON_BTN0_PIN
#define SL_SIMPLE_BUTTON_BTN0_PIN 2
#endif
// <<< sl:end pin_tool >>>

#endif // SL_SIMPLE_BUTTON_BTN0_CONFIG_H
