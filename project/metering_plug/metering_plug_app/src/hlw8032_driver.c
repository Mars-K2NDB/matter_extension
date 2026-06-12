/*
 * HLW8032 电能计量芯片驱动（Silicon Labs EUSART1，4800 8E1）
 */

#include "hlw8032_driver.h"
#include "hlw8032_config.h"

#include <em_cmu.h>
#include <em_eusart.h>
#include <em_gpio.h>

#include "sl_clock_manager.h"
#include "sl_device_peripheral.h"

#include <silabs_utils.h>
#include <string.h>

hlw8032_driver_t g_hlw8032 = {
    .initialized  = false,
    .rx_head      = 0,
    .rx_tail      = 0,
    .vol_par      = 0,
    .vol_data     = 0,
    .current_par  = 0,
    .current_data = 0,
    .power_par    = 0,
    .power_data   = 0,
    .pf           = 0,
    .pf_data      = 1,
    .vf           = 0.0f,
    .cf           = 0.0f,
    .vol_r1       = 2040000,
    .vol_r2       = 1000,
    .current_rf   = 1.0f,
    .serial_read  = false,
};

static hlw8032_driver_t * s_irq_driver = NULL;

static inline uint32_t ring_available(const hlw8032_driver_t * hlw8032)
{
    return (hlw8032->rx_head - hlw8032->rx_tail) & (HLW8032_RX_BUF_SIZE - 1);
}

static inline uint8_t ring_read_byte(hlw8032_driver_t * hlw8032)
{
    uint8_t b = hlw8032->rx_ring[hlw8032->rx_tail & (HLW8032_RX_BUF_SIZE - 1)];
    hlw8032->rx_tail = (hlw8032->rx_tail + 1) & (HLW8032_RX_BUF_SIZE - 1);
    return b;
}

static void ring_write_byte_isr(hlw8032_driver_t * hlw8032, uint8_t byte)
{
    uint32_t next_head = (hlw8032->rx_head + 1) & (HLW8032_RX_BUF_SIZE - 1);
    if (next_head != hlw8032->rx_tail)
    {
        hlw8032->rx_ring[hlw8032->rx_head] = byte;
        hlw8032->rx_head                   = next_head;
    }
}

static bool frame_checksum_ok(const uint8_t * frame)
{
    uint8_t sum = 0;
    for (int i = 2; i <= 22; i++)
    {
        sum += frame[i];
    }
    return (sum == frame[23]);
}

static void parse_frame(hlw8032_driver_t * hlw8032)
{
    const uint8_t * f = hlw8032->frame;

    hlw8032->vol_par = ((uint32_t) f[2] << 16) | ((uint32_t) f[3] << 8) | f[4];

    if (f[20] & (1u << 6))
    {
        hlw8032->vol_data = ((uint32_t) f[5] << 16) | ((uint32_t) f[6] << 8) | f[7];
    }

    hlw8032->current_par = ((uint32_t) f[8] << 16) | ((uint32_t) f[9] << 8) | f[10];

    if (f[20] & (1u << 5))
    {
        hlw8032->current_data = ((uint32_t) f[11] << 16) | ((uint32_t) f[12] << 8) | f[13];
    }

    hlw8032->power_par = ((uint32_t) f[14] << 16) | ((uint32_t) f[15] << 8) | f[16];

    if (f[20] & (1u << 4))
    {
        hlw8032->power_data = ((uint32_t) f[17] << 16) | ((uint32_t) f[18] << 8) | f[19];
    }

    hlw8032->pf = ((uint16_t) f[21] << 8) | f[22];

    if (f[20] & (1u << 7))
    {
        hlw8032->pf_data++;
    }

    hlw8032->serial_read = true;
}

void EUSART1_RX_IRQHandler(void)
{
    if (s_irq_driver == NULL)
    {
        return;
    }

    uint32_t flags = HLW8032_EUSART->IF;
    if (flags & EUSART_IF_RXFL)
    {
        while (HLW8032_EUSART->STATUS & EUSART_STATUS_RXFL)
        {
            uint8_t c = (uint8_t) HLW8032_EUSART->RXDATA;
            ring_write_byte_isr(s_irq_driver, c);
        }
        EUSART_IntClear(HLW8032_EUSART, EUSART_IF_RXFL);
    }
    if (flags & EUSART_IF_RXOF)
    {
        EUSART_IntClear(HLW8032_EUSART, EUSART_IF_RXOF);
    }
}

static void hlw8032_uart_hw_init(void)
{
    sl_bus_clock_t bus_clock;

    bus_clock = sl_device_peripheral_get_bus_clock(SL_PERIPHERAL_EUSART1);
    sl_clock_manager_enable_bus_clock(bus_clock);
    sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_GPIO);

    GPIO_PinModeSet((GPIO_Port_TypeDef) HLW8032_TX_PORT, HLW8032_TX_PIN, gpioModePushPull, 1);
    GPIO_PinModeSet((GPIO_Port_TypeDef) HLW8032_RX_PORT, HLW8032_RX_PIN, gpioModeInputPull, 1);

    GPIO->EUSARTROUTE[HLW8032_EUSART_NUM].ROUTEEN = GPIO_EUSART_ROUTEEN_TXPEN | GPIO_EUSART_ROUTEEN_RXPEN;
    GPIO->EUSARTROUTE[HLW8032_EUSART_NUM].TXROUTE =
        ((uint32_t) HLW8032_TX_PORT << _GPIO_EUSART_TXROUTE_PORT_SHIFT) |
        ((uint32_t) HLW8032_TX_PIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
    GPIO->EUSARTROUTE[HLW8032_EUSART_NUM].RXROUTE =
        ((uint32_t) HLW8032_RX_PORT << _GPIO_EUSART_RXROUTE_PORT_SHIFT) |
        ((uint32_t) HLW8032_RX_PIN << _GPIO_EUSART_RXROUTE_PIN_SHIFT);

    EUSART_UartInit_TypeDef uart_cfg = EUSART_UART_INIT_DEFAULT_HF;
    uart_cfg.enable       = eusartDisable;
    uart_cfg.baudrate     = HLW8032_UART_BAUDRATE;
    uart_cfg.parity       = eusartEvenParity;
    uart_cfg.stopbits     = eusartStopbits1;
    uart_cfg.databits     = eusartDataBits8;
    uart_cfg.oversampling = eusartOVS16;

    EUSART_UartInitHf(HLW8032_EUSART, &uart_cfg);

    EUSART_IntClear(HLW8032_EUSART, EUSART_IF_RXFL | EUSART_IF_RXOF);
    EUSART_IntEnable(HLW8032_EUSART, EUSART_IEN_RXFL);

    NVIC_ClearPendingIRQ(EUSART1_RX_IRQn);
    NVIC_EnableIRQ(EUSART1_RX_IRQn);

    EUSART_Enable(HLW8032_EUSART, eusartEnable);
}

bool hlw8032_init(hlw8032_driver_t * hlw8032)
{
    if (hlw8032 == NULL)
    {
        return false;
    }

    memset(hlw8032, 0, sizeof(*hlw8032));
    hlw8032->pf_data     = 1;
    hlw8032->vol_r1      = 2040000;
    hlw8032->vol_r2      = 1000;
    hlw8032->current_rf  = 1.0f;
    hlw8032->vf          = (float) hlw8032->vol_r1 / (float) hlw8032->vol_r2;
    hlw8032->cf          = 1.0f / (hlw8032->current_rf * 1000.0f);
    hlw8032->serial_read = false;

    s_irq_driver = hlw8032;
    hlw8032_uart_hw_init();

    hlw8032->initialized = true;
    SILABS_LOG("HLW8032: EUSART1 PB%d/PB%d @ %d 8E1", HLW8032_TX_PIN, HLW8032_RX_PIN, HLW8032_UART_BAUDRATE);
    return true;
}

bool hlw8032_set_vf(hlw8032_driver_t * hlw8032, float vf)
{
    if ((hlw8032 == NULL) || !hlw8032->initialized)
    {
        return false;
    }
    hlw8032->vf = vf;
    return true;
}

bool hlw8032_set_cf(hlw8032_driver_t * hlw8032, float cf)
{
    if ((hlw8032 == NULL) || !hlw8032->initialized)
    {
        return false;
    }
    hlw8032->cf = cf;
    return true;
}

bool hlw8032_is_initialized(hlw8032_driver_t * hlw8032)
{
    return (hlw8032 != NULL) && hlw8032->initialized;
}

bool hlw8032_read_energy_data(hlw8032_driver_t * hlw8032, hlw8032_energy_data_t * data)
{
    if ((hlw8032 == NULL) || !hlw8032->initialized || (data == NULL))
    {
        return false;
    }

    const uint32_t max_frames = 3u;
    const uint32_t max_iter   = 32u;
    uint32_t iter             = 0;
    uint32_t frames_parsed    = 0;

    while ((ring_available(hlw8032) >= HLW8032_FRAME_LEN) && (iter < max_iter) && (frames_parsed < max_frames))
    {
        iter++;
        uint32_t peek_idx = (hlw8032->rx_tail + 1) & (HLW8032_RX_BUF_SIZE - 1);
        if (hlw8032->rx_ring[peek_idx] == 0x5A)
        {
            for (int i = 0; i < HLW8032_FRAME_LEN; i++)
            {
                hlw8032->frame[i] = ring_read_byte(hlw8032);
            }
            if (!frame_checksum_ok(hlw8032->frame))
            {
                continue;
            }
            parse_frame(hlw8032);
            frames_parsed++;
        }
        else
        {
            ring_read_byte(hlw8032);
        }
    }

    if (!hlw8032->serial_read)
    {
        return false;
    }

    data->voltage      = hlw8032_get_voltage(hlw8032);
    data->current      = hlw8032_get_current(hlw8032);
    data->active_power = hlw8032_get_active_power(hlw8032);
    data->power_factor = hlw8032_get_power_factor(hlw8032);
    data->kwh          = hlw8032_get_kwh(hlw8032);
    data->timestamp    = 0;
    data->valid        = true;

    return true;
}

float hlw8032_get_voltage(hlw8032_driver_t * hlw8032)
{
    if ((hlw8032 == NULL) || !hlw8032->initialized || (hlw8032->vol_data == 0))
    {
        return 0.0f;
    }
    return (float) hlw8032->vol_par / (float) hlw8032->vol_data * hlw8032->vf / 1000.0f;
}

float hlw8032_get_current(hlw8032_driver_t * hlw8032)
{
    if ((hlw8032 == NULL) || !hlw8032->initialized || (hlw8032->current_data == 0))
    {
        return 0.0f;
    }
    return (float) hlw8032->current_par / (float) hlw8032->current_data * hlw8032->cf;
}

float hlw8032_get_active_power(hlw8032_driver_t * hlw8032)
{
    if ((hlw8032 == NULL) || !hlw8032->initialized || (hlw8032->power_data == 0))
    {
        return 0.0f;
    }
    return (float) hlw8032->power_par / (float) hlw8032->power_data * hlw8032->vf * hlw8032->cf;
}

float hlw8032_get_power_factor(hlw8032_driver_t * hlw8032)
{
    if ((hlw8032 == NULL) || !hlw8032->initialized)
    {
        return 0.0f;
    }
    float v = hlw8032_get_voltage(hlw8032);
    float i = hlw8032_get_current(hlw8032);
    if (v * i == 0.0f)
    {
        return 0.0f;
    }
    return hlw8032_get_active_power(hlw8032) / (v * i);
}

float hlw8032_get_kwh(hlw8032_driver_t * hlw8032)
{
    if ((hlw8032 == NULL) || !hlw8032->initialized || (hlw8032->power_par == 0))
    {
        return 0.0f;
    }
    float apparent = hlw8032_get_voltage(hlw8032) * hlw8032_get_current(hlw8032);
    if (apparent <= 0.0f)
    {
        return 0.0f;
    }
    double pf_cnt = (1000000000.0 * 3600.0) / ((double) hlw8032->power_par * (double) apparent);
    if (pf_cnt <= 0.0)
    {
        return 0.0f;
    }
    return (float) ((double) (hlw8032->pf_data * hlw8032->pf) / pf_cnt);
}
