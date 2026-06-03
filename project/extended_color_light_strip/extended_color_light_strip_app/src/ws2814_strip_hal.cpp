/*
 * WS2814 over SPI（Silicon Labs SPIDRV，对齐 Telink/Zephyr ws2814-spi 编码）。
 */

#include "ws2814_strip_hal.h"

#include "ws2814_strip_config.h"

#include <dmadrv.h>
#include <em_core.h>
#include <em_device.h>
#include <lib/support/logging/CHIPLogging.h>
#include <sl_clock_manager.h>
#include <sl_gpio.h>
#include <sl_spidrv_eusart_SPI_config.h>
#include <sl_spidrv_instances.h>
#include <spidrv.h>

#if defined(_SILICON_LABS_32B_SERIES_2)
#include <em_bus.h>
#else
#include <sl_hal_bus.h>
#endif

#include <array>
#include <cstring>

namespace ws2814_strip {
namespace {

constexpr size_t kFrameBytes      = WS2814_FRAME_TOTAL_BYTES;
constexpr int kDmaMaxChunkBytes   = DMADRV_MAX_XFER_COUNT;

static_assert(WS2814_SPI_BITRATE_HZ == static_cast<uint32_t>(SL_SPIDRV_EUSART_SPI_BITRATE),
              "WS2814_SPI_BITRATE_HZ must match SL_SPIDRV_EUSART_SPI_BITRATE");
static_assert(WS2814_SPI_BITRATE_HZ == WS2814_SPI_ACTUAL_BITRATE_HZ,
              "WS2814 SPI bitrate must divide WS2814_EUSART_REF_CLK_HZ (see ws2814_strip_config.h)");
/* WS2814 4-bit 编码：0x80=1000(1H+3L)，0xC0=1100(2H+2L)，按实际 SPI 位时间校验 */
static_assert((1ULL * 1000000000ULL / WS2814_SPI_ACTUAL_BITRATE_HZ) >= 220ULL, "T0H too short");
static_assert((1ULL * 1000000000ULL / WS2814_SPI_ACTUAL_BITRATE_HZ) <= 380ULL, "T0H too long");
static_assert((3ULL * 1000000000ULL / WS2814_SPI_ACTUAL_BITRATE_HZ) >= 580ULL, "T0L too short");
static_assert((2ULL * 1000000000ULL / WS2814_SPI_ACTUAL_BITRATE_HZ) >= 580ULL, "T1H too short");
static_assert((2ULL * 1000000000ULL / WS2814_SPI_ACTUAL_BITRATE_HZ) <= 1000ULL, "T1L too long");
static_assert(WS2814_NS_PER_LOGIC_BIT >= 1200ULL, "logic bit period too short for SPI encoding");
static_assert((WS2814_SPI_RESET_BYTES * WS2814_NS_PER_SPI_BYTE) >= (WS2814_RESET_TIME_US * 1000ULL),
              "WS2814_SPI_RESET_BYTES too small for datasheet RES time");
static_assert(kFrameBytes > 0, "WS2814 frame size must be positive");
static_assert(kDmaMaxChunkBytes > 0, "DMADRV_MAX_XFER_COUNT invalid");
static_assert(WS2814_FRAME_PAYLOAD_OFFSET + WS2814_FRAME_PAYLOAD_BYTES == WS2814_FRAME_RESET_OFFSET,
              "WS2814 frame layout mismatch");
static_assert(WS2814_FRAME_RESET_OFFSET + WS2814_SPI_RESET_BYTES == WS2814_FRAME_TOTAL_BYTES,
              "WS2814 frame total size mismatch");

std::array<uint8_t, kFrameBytes> g_tx_frame{};
bool g_initialized = false;

sl_gpio_t MosiGpio()
{
    sl_gpio_t gpio = {};
    gpio.port      = SL_SPIDRV_EUSART_SPI_TX_PORT;
    gpio.pin       = SL_SPIDRV_EUSART_SPI_TX_PIN;
    return gpio;
}

void WaitEusartTxIdle(EUSART_TypeDef* eusart)
{
    if (eusart == nullptr)
    {
        return;
    }

    while ((eusart->STATUS & EUSART_STATUS_TXFL) == 0U)
    {
    }
}

void MosiRouteEnable(bool enable)
{
    const uint8_t port_num = static_cast<uint8_t>(SL_SPIDRV_EUSART_SPI_PERIPHERAL_NO);
#if defined(_SILICON_LABS_32B_SERIES_2)
    BUS_RegMaskedWrite(&GPIO->EUSARTROUTE[port_num].ROUTEEN, _GPIO_EUSART_ROUTEEN_TXPEN_MASK,
                       enable ? GPIO_EUSART_ROUTEEN_TXPEN : 0U);
#else
    sl_hal_bus_reg_write_mask(&GPIO->EUSARTROUTE[port_num].ROUTEEN, _GPIO_EUSART_ROUTEEN_TXPEN_MASK,
                              enable ? GPIO_EUSART_ROUTEEN_TXPEN : 0U);
#endif
}

void MosiEnableSpiRoute()
{
#if WS2814_SPI_MOSI_IDLE_GPIO_LOW
    MosiRouteEnable(true);
#endif
}

void MosiHoldLowGpio()
{
#if WS2814_SPI_MOSI_IDLE_GPIO_LOW
    sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_GPIO);

    WaitEusartTxIdle(SL_SPIDRV_EUSART_SPI_PERIPHERAL);
    MosiRouteEnable(false);

    const sl_gpio_t mosi = MosiGpio();
    (void) sl_gpio_set_pin_mode(&mosi, SL_GPIO_MODE_PUSH_PULL, false);
    (void) sl_gpio_clear_pin(&mosi);
#endif
}

/** 线序 WRGB（Telink color-mapping），每逻辑位 4 个 SPI 位，每个 SPI 字节打包 2 个逻辑位 */
void EncodeChannelMsb(uint8_t value, uint8_t* out)
{
    for (int i = 0; i < 4; ++i)
    {
        const uint8_t bit1 = (value >> (7 - 2 * i)) & 0x01U;
        const uint8_t bit2 = (value >> (7 - 2 * i - 1)) & 0x01U;

        const uint8_t nibble1 = (bit1 != 0U) ? 0xC0U : 0x80U; // 1100 0000 or 1000 0000
        const uint8_t nibble2 = (bit2 != 0U) ? 0x0CU : 0x08U; // 0000 1100 or 0000 1000

        out[i] = nibble1 | nibble2;
    }
}

bool SpiTransmitBufferUninterrupted(const uint8_t* buffer, size_t length)
{
    if (buffer == nullptr || length == 0)
    {
        return false;
    }

    /*
     * 分块 DMA 时 MOSI 可能在块间保持低电平；若间隔 >280us 会被芯片当成 RES，
     * 在级联中间误锁存。整帧发送期间关中断，缩短块间间隙。
     */
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_CRITICAL();

    MosiEnableSpiRoute();

    size_t offset = 0;
    bool ok       = true;
    while (offset < length)
    {
        const size_t remain = length - offset;
        const int chunk =
            (remain > static_cast<size_t>(kDmaMaxChunkBytes)) ? kDmaMaxChunkBytes : static_cast<int>(remain);

        const Ecode_t status = SPIDRV_MTransmitB(sl_spidrv_eusart_SPI_handle, buffer + offset, chunk);
        if (status != ECODE_EMDRV_SPIDRV_OK)
        {
            ChipLogError(AppServer, "WS2814 SPI chunk failed: ec=0x%08lx off=%u len=%u chunk=%d (max=%d)",
                         static_cast<unsigned long>(status), static_cast<unsigned>(offset),
                         static_cast<unsigned>(length), chunk, kDmaMaxChunkBytes);
            ok = false;
            break;
        }
        offset += static_cast<size_t>(chunk);
    }

    MosiHoldLowGpio();

    CORE_EXIT_CRITICAL();
    return ok;
}

void FillFrameIdleRegions()
{
    if (WS2814_FRAME_PAYLOAD_OFFSET > 0)
    {
        std::memset(g_tx_frame.data(), 0x00, WS2814_FRAME_PAYLOAD_OFFSET);
    }
    std::memset(g_tx_frame.data() + WS2814_FRAME_RESET_OFFSET, 0x00, WS2814_SPI_RESET_BYTES);
}

} // namespace

void MosiHoldLow()
{
    MosiHoldLowGpio();
}

bool Init()
{
    if (g_initialized)
    {
        return true;
    }

    if (sl_spidrv_eusart_SPI_handle == nullptr)
    {
        ChipLogError(AppServer, "WS2814 SPI handle is null");
        return false;
    }

    MosiHoldLow();

    FillFrameIdleRegions();
    g_initialized = true;
    return ControlRgbw(0, 0, 0, 0);
}

bool ControlRgbw(uint16_t red, uint16_t green, uint16_t blue, uint16_t white)
{
    if (!g_initialized && !Init())
    {
        return false;
    }

    const uint8_t sr = static_cast<uint8_t>((static_cast<uint32_t>(red) * 255U) / STRIP_LEVEL_MAX);
    const uint8_t sg = static_cast<uint8_t>((static_cast<uint32_t>(green) * 255U) / STRIP_LEVEL_MAX);
    const uint8_t sb = static_cast<uint8_t>((static_cast<uint32_t>(blue) * 255U) / STRIP_LEVEL_MAX);
    const uint8_t sw = static_cast<uint8_t>((static_cast<uint32_t>(white) * 255U) / STRIP_LEVEL_MAX);

    FillFrameIdleRegions();

    size_t offset = WS2814_FRAME_PAYLOAD_OFFSET;
    for (uint16_t i = 0; i < WS2814_LED_COUNT; ++i)
    {
        EncodeChannelMsb(sw, &g_tx_frame[offset]);
        offset += WS2814_BYTES_PER_CHANNEL;
        EncodeChannelMsb(sr, &g_tx_frame[offset]);
        offset += WS2814_BYTES_PER_CHANNEL;
        EncodeChannelMsb(sg, &g_tx_frame[offset]);
        offset += WS2814_BYTES_PER_CHANNEL;
        EncodeChannelMsb(sb, &g_tx_frame[offset]);
        offset += WS2814_BYTES_PER_CHANNEL;
    }

    if (offset != WS2814_FRAME_RESET_OFFSET)
    {
        ChipLogError(AppServer, "WS2814 encode size mismatch: got %u expect %u", static_cast<unsigned>(offset),
                     static_cast<unsigned>(WS2814_FRAME_RESET_OFFSET));
        return false;
    }

    return SpiTransmitBufferUninterrupted(g_tx_frame.data(), g_tx_frame.size());
}

} // namespace ws2814_strip
