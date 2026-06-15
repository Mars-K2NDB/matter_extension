#ifndef SL_SIMPLE_BUTTON_CONFIG_H
#define SL_SIMPLE_BUTTON_CONFIG_H

#include "sl_gpio.h"

// 低电平有效按键需内部上拉，避免 PC0-PC3 浮空误触发中断
// 3 次采样消抖：在 5~10ms 轮询下约 15~30ms，兼顾手感与抗抖
#define SL_SIMPLE_BUTTON_DEBOUNCE_BITS           3U
#define SL_SIMPLE_BUTTON_GPIO_MODE               SL_GPIO_MODE_INPUT_PULL
#define SL_SIMPLE_BUTTON_GPIO_DOUT               1U
#define SL_SIMPLE_BUTTON_POLARITY                0U
#define SL_SIMPLE_BUTTON_ALLOW_LED_CONFLICT      0U

#endif // SL_SIMPLE_BUTTON_CONFIG_H
