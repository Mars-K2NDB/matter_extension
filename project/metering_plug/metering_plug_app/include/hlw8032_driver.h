/*
 * HLW8032 电能计量芯片驱动（从 connectedhomeip zephyr_hlw8032 移植）
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define HLW8032_FRAME_LEN   24
#define HLW8032_RX_BUF_SIZE 64

typedef struct
{
    float    voltage;
    float    current;
    float    active_power;
    float    power_factor;
    float    kwh;
    uint32_t timestamp;
    bool     valid;
} hlw8032_energy_data_t;

typedef struct
{
    bool     initialized;
    uint8_t  rx_ring[HLW8032_RX_BUF_SIZE];
    uint32_t rx_head;
    uint32_t rx_tail;
    uint8_t  frame[HLW8032_FRAME_LEN];
    uint32_t vol_par;
    uint32_t vol_data;
    uint32_t current_par;
    uint32_t current_data;
    uint32_t power_par;
    uint32_t power_data;
    uint16_t pf;
    uint32_t pf_data;
    float    vf;
    float    cf;
    uint32_t vol_r1;
    uint32_t vol_r2;
    float    current_rf;
    bool     serial_read;
} hlw8032_driver_t;

extern hlw8032_driver_t g_hlw8032;

bool hlw8032_init(hlw8032_driver_t * hlw8032);
bool hlw8032_set_vf(hlw8032_driver_t * hlw8032, float vf);
bool hlw8032_set_cf(hlw8032_driver_t * hlw8032, float cf);
bool hlw8032_read_energy_data(hlw8032_driver_t * hlw8032, hlw8032_energy_data_t * data);
bool hlw8032_is_initialized(hlw8032_driver_t * hlw8032);
float hlw8032_get_voltage(hlw8032_driver_t * hlw8032);
float hlw8032_get_current(hlw8032_driver_t * hlw8032);
float hlw8032_get_active_power(hlw8032_driver_t * hlw8032);
float hlw8032_get_power_factor(hlw8032_driver_t * hlw8032);
float hlw8032_get_kwh(hlw8032_driver_t * hlw8032);

#ifdef __cplusplus
}
#endif
