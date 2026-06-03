/*
 * 四混五与 HSV 转换（移植自 examples/platform/telink/util/ColorFormat.cpp）。
 */

#include "color_format_mix.h"

#include "ws2814_strip_config.h"

#include <algorithm>
#include <cmath>

namespace color_format {
namespace {

void KelvinToRgb255(float kelvin, float* out_r, float* out_g, float* out_b)
{
    if (out_r == nullptr || out_g == nullptr || out_b == nullptr)
    {
        return;
    }

    const float temp = kelvin / 100.0f;

    if (temp <= 66.0f)
    {
        *out_r = 255.0f;
        *out_g = 99.4708025861f * logf(temp) - 161.1195681661f;
    }
    else
    {
        *out_r = 329.698727446f * powf(temp - 60.0f, -0.1332047592f);
        *out_g = 288.1221695283f * powf(temp - 60.0f, -0.0755148492f);
    }

    if (temp >= 66.0f)
    {
        *out_b = 255.0f;
    }
    else if (temp <= 19.0f)
    {
        *out_b = 0.0f;
    }
    else
    {
        *out_b = 138.5177312231f * logf(temp - 10.0f) - 305.0447927307f;
    }

    *out_r = std::clamp(*out_r, 0.0f, 255.0f);
    *out_g = std::clamp(*out_g, 0.0f, 255.0f);
    *out_b = std::clamp(*out_b, 0.0f, 255.0f);
}

constexpr uint16_t kCtMinMireds = 111;
constexpr uint16_t kCtMaxMireds = 500;
constexpr uint8_t kCwMax        = 255;

} // namespace

Rgb8 HsvToRgb(Hsv8 hsv)
{
    Rgb8 rgb{ 0, 0, 0 };

    uint8_t region     = 0;
    uint8_t p          = 0;
    uint8_t q          = 0;
    uint8_t t          = 0;
    uint32_t h         = 0;
    uint32_t s         = 0;
    uint32_t v         = 0;
    uint32_t remainder = 0;

    if (hsv.v == 0)
    {
        return rgb;
    }

    const uint8_t min_brightness = (255U * STRIP_MIN_BRIGHTNESS_PERCENT) / 100U;
    v                            = min_brightness + (static_cast<uint32_t>(hsv.v) * (255U - min_brightness)) / 255U;

    if (hsv.s == 0)
    {
        rgb.r = rgb.g = rgb.b = static_cast<uint8_t>(v);
        return rgb;
    }

    h         = hsv.h;
    s         = hsv.s;
    region    = static_cast<uint8_t>(h / 43U);
    remainder = (h - (region * 43U)) * 6U;
    p         = static_cast<uint8_t>((v * (255U - s)) >> 8);
    q         = static_cast<uint8_t>((v * (255U - ((s * remainder) >> 8))) >> 8);
    t         = static_cast<uint8_t>((v * (255U - ((s * (255U - remainder)) >> 8))) >> 8);

    switch (region)
    {
    case 0:
        rgb.r = static_cast<uint8_t>(v);
        rgb.g = t;
        rgb.b = p;
        break;
    case 1:
        rgb.r = q;
        rgb.g = static_cast<uint8_t>(v);
        rgb.b = p;
        break;
    case 2:
        rgb.r = p;
        rgb.g = static_cast<uint8_t>(v);
        rgb.b = t;
        break;
    case 3:
        rgb.r = p;
        rgb.g = q;
        rgb.b = static_cast<uint8_t>(v);
        break;
    case 4:
        rgb.r = t;
        rgb.g = p;
        rgb.b = static_cast<uint8_t>(v);
        break;
    default:
        rgb.r = static_cast<uint8_t>(v);
        rgb.g = p;
        rgb.b = q;
        break;
    }

    return rgb;
}

Rgb8 XyToRgb(uint8_t level, uint16_t current_x, uint16_t current_y)
{
    Rgb8 rgb{ 0, 0, 0 };
    if (level == 0)
    {
        return rgb;
    }

    const float x = static_cast<float>(current_x) / 65535.0f;
    const float y = static_cast<float>(current_y) / 65535.0f;
    if (y <= 0.0f)
    {
        return rgb;
    }

    const float z = 1.0f - x - y;
    const float Y = static_cast<float>(level) / 254.0f;
    float X       = (Y / y) * x;
    float Z       = (Y / y) * z;

    /* 与 Matter Telink ColorFormat.cpp XYToRgb 一致：XYZ 先归一化再转 sRGB */
    X            = X / 100.0f;
    const float Yn = Y / 100.0f;
    Z            = Z / 100.0f;

    float r = (X * 3.2406f) - (Yn * 1.5372f) - (Z * 0.4986f);
    float g = -(X * 0.9689f) + (Yn * 1.8758f) + (Z * 0.0415f);
    float b = (X * 0.0557f) - (Yn * 0.2040f) + (Z * 1.0570f);

    auto gamma22 = [](float c) -> float {
        if (c <= 0.0031308f)
        {
            return 12.92f * c;
        }
        return (1.055f * powf(c, (1.0f / 2.4f))) - 0.055f;
    };

    r = gamma22(r);
    g = gamma22(g);
    b = gamma22(b);

    r = std::clamp(r, 0.0f, 1.0f);
    g = std::clamp(g, 0.0f, 1.0f);
    b = std::clamp(b, 0.0f, 1.0f);

    rgb.r = static_cast<uint8_t>(r * 255.0f);
    rgb.g = static_cast<uint8_t>(g * 255.0f);
    rgb.b = static_cast<uint8_t>(b * 255.0f);
    return rgb;
}

void MixRgbwFromMireds(uint16_t mireds, uint8_t brightness, uint16_t* out_r, uint16_t* out_g, uint16_t* out_b,
                       uint16_t* out_w)
{
    if (out_r == nullptr || out_g == nullptr || out_b == nullptr || out_w == nullptr)
    {
        return;
    }

    if (brightness == 0)
    {
        *out_r = 0;
        *out_g = 0;
        *out_b = 0;
        *out_w = 0;
        return;
    }

    const uint16_t clamped_mireds = std::clamp(mireds, kCtMinMireds, kCtMaxMireds);
    float white_ratio             = static_cast<float>(clamped_mireds - kCtMinMireds) /
                        static_cast<float>(kCtMaxMireds - kCtMinMireds);
    white_ratio = std::clamp(white_ratio, 0.0f, 1.0f);

    const float kelvin = 1000000.0f / static_cast<float>(clamped_mireds);
    float r255         = 0.0f;
    float g255         = 0.0f;
    float b255         = 0.0f;
    KelvinToRgb255(kelvin, &r255, &g255, &b255);

    const float rgb_sum = r255 + g255 + b255;
    const float norm_r  = (rgb_sum > 0.0f) ? (r255 / rgb_sum) : (1.0f / 3.0f);
    const float norm_g  = (rgb_sum > 0.0f) ? (g255 / rgb_sum) : (1.0f / 3.0f);
    const float norm_b  = (rgb_sum > 0.0f) ? (b255 / rgb_sum) : (1.0f / 3.0f);

    const uint8_t min_brightness =
        (kCwMax * STRIP_MIN_BRIGHTNESS_PERCENT) / 100U;
    const uint8_t effective_brightness =
        min_brightness + (brightness * (kCwMax - min_brightness)) / kCwMax;
    const float brightness_scale = effective_brightness / 255.0f;

    const float r_mix = norm_r * (1.0f - white_ratio) * brightness_scale;
    const float g_mix = norm_g * (1.0f - white_ratio) * brightness_scale;
    const float b_mix = norm_b * (1.0f - white_ratio) * brightness_scale;
    const float w_mix = white_ratio * brightness_scale;

    *out_r = static_cast<uint16_t>(std::clamp(roundf(r_mix * 1024.0f), 0.0f, 1024.0f));
    *out_g = static_cast<uint16_t>(std::clamp(roundf(g_mix * 1024.0f), 0.0f, 1024.0f));
    *out_b = static_cast<uint16_t>(std::clamp(roundf(b_mix * 1024.0f), 0.0f, 1024.0f));
    *out_w = static_cast<uint16_t>(std::clamp(roundf(w_mix * 1024.0f), 0.0f, 1024.0f));
}

} // namespace color_format
