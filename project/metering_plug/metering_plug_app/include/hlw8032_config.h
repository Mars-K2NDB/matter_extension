/*
 * HLW8032 计量芯片 UART 配置（EUSART1 @ PB4/PB5）
 */

#pragma once

#include "pin_config.h"

/* HLW8032 使用 EUSART1，TX=PB4，RX=PB5 */
#define HLW8032_EUSART            EUSART1
#define HLW8032_EUSART_NUM        1
#define HLW8032_EUSART_CLK        cmuClock_EUSART1

#define HLW8032_UART_BAUDRATE     4800

#ifndef HLW8032_TX_PORT
#define HLW8032_TX_PORT           EUSART1_TX_PORT
#endif
#ifndef HLW8032_TX_PIN
#define HLW8032_TX_PIN            EUSART1_TX_PIN
#endif
#ifndef HLW8032_RX_PORT
#define HLW8032_RX_PORT           EUSART1_RX_PORT
#endif
#ifndef HLW8032_RX_PIN
#define HLW8032_RX_PIN            EUSART1_RX_PIN
#endif

/* 轮询间隔（ms），与 connectedhomeip 计量插座一致 */
#define HLW8032_ENERGY_POLL_MS    1000

/* 测试模式默认运行功率（W），插座开启且尚无采样数据时使用 */
#define HLW8032_TEST_DEFAULT_POWER_W  1000.0f

/* 设为 1 时不依赖硬件，使用模拟数据 */
#ifndef CONFIG_HLW8032_ENERGY_READING_TEST_MODE
#define CONFIG_HLW8032_ENERGY_READING_TEST_MODE 0
#endif

/* 测试模式下累计电量上报间隔（秒），便于 App 验证 */
#if CONFIG_HLW8032_ENERGY_READING_TEST_MODE
#define HLW8032_CUMULATIVE_REPORT_INTERVAL_SECONDS 5
#else
#define HLW8032_CUMULATIVE_REPORT_INTERVAL_SECONDS CUMULATIVE_REPORT_INTERVAL_SECONDS
#endif
