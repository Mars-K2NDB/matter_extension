/*
 * WS2814 SPI 硬件抽象（移植自 Zephyr worldsemi,ws2814-spi）。
 */
#pragma once

#include <cstdint>

namespace ws2814_strip {

struct WrgbPixel
{
    uint8_t w;
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

bool Init();
bool ControlRgbw(uint16_t red, uint16_t green, uint16_t blue, uint16_t white);
/** 按 WRGB 顺序刷新整条灯带（像素数不超过 WS2814_LED_COUNT） */
bool FlushPixels(const WrgbPixel* pixels, uint16_t count);

/** 无 SPI 发送时保持 MOSI（PC9）为 GPIO 推挽低；可在 sl_spidrv_init 之后调用 */
void MosiHoldLow();

} // namespace ws2814_strip
