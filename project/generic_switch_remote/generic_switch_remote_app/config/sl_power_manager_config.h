/***************************************************************************//**
 * @file
 * @brief Power Manager configuration file.
 *******************************************************************************
 * SPDX-License-Identifier: Zlib
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

#ifndef SL_POWER_MANAGER_CONFIG_H
#define SL_POWER_MANAGER_CONFIG_H

// <h>Power Manager Configuration

#ifndef SL_POWER_MANAGER_CUSTOM_HF_OSCILLATOR_IRQ_HANDLER
#define SL_POWER_MANAGER_CUSTOM_HF_OSCILLATOR_IRQ_HANDLER  0
#endif

#ifndef SL_POWER_MANAGER_CONFIG_VOLTAGE_SCALING_FAST_WAKEUP
#define SL_POWER_MANAGER_CONFIG_VOLTAGE_SCALING_FAST_WAKEUP   0
#endif

#ifndef SL_POWER_MANAGER_ENABLE_EM01_VOLTAGE_SCALING
#define SL_POWER_MANAGER_ENABLE_EM01_VOLTAGE_SCALING   0
#endif

#ifndef SL_POWER_MANAGER_DEBUG
#define SL_POWER_MANAGER_DEBUG  0
#endif

#ifndef SL_POWER_MANAGER_DEBUG_POOL_SIZE
#define SL_POWER_MANAGER_DEBUG_POOL_SIZE  10
#endif

#ifndef SL_POWER_MANAGER_INIT_EMU_EM4_PIN_RETENTION_MODE
#define SL_POWER_MANAGER_INIT_EMU_EM4_PIN_RETENTION_MODE EMU_EM4CTRL_EM4IORETMODE_DISABLE
#endif

// 量产低功耗：关闭 EM2 调试保持，避免调试器连接时额外耗电
#ifndef SL_POWER_MANAGER_INIT_EMU_EM2_DEBUG_ENABLE
#define SL_POWER_MANAGER_INIT_EMU_EM2_DEBUG_ENABLE 0
#endif

// </h>

#endif /* SL_POWER_MANAGER_CONFIG_H */

// <<< end of configuration section >>>
