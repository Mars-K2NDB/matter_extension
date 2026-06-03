/*
 * 色温/HSV 转换与四混五（源自 connectedhomeip Telink ColorFormat）。
 */
#pragma once

#include <cstdint>

namespace color_format {

struct Rgb8
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct Hsv8
{
    uint8_t h;
    uint8_t s;
    uint8_t v;
};

/** Matter level(0..254) + mireds -> R,G,B,W（各 0..1024） */
void MixRgbwFromMireds(uint16_t mireds, uint8_t brightness, uint16_t* out_r, uint16_t* out_g, uint16_t* out_b,
                       uint16_t* out_w);

Rgb8 HsvToRgb(Hsv8 hsv);

/** Matter CurrentX/CurrentY + level(0..254) -> RGB（CIE xyY -> sRGB） */
Rgb8 XyToRgb(uint8_t level, uint16_t current_x, uint16_t current_y);

inline uint16_t ScaleTo1024(uint8_t value_255)
{
    return static_cast<uint16_t>((static_cast<uint32_t>(value_255) * 1024U) / 255U);
}

} // namespace color_format
