/*
 * WS2814 灯带动画（移植自 connectedhomeip telink zephyr_ws2814_effects.c）。
 */

#include "ws2814_strip_effects.h"

#include "rgbcw_strip_driver.h"
#include "ws2814_strip_config.h"
#include "ws2814_strip_hal.h"

#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>
#include <system/SystemClock.h>
#include <system/SystemLayer.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>

namespace ws2814_effects {
namespace {

constexpr uint16_t kLedCount = WS2814_LED_COUNT;

static const uint8_t kSin8Table[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   2,   2,   3,
    3,   4,   5,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  17,  18,
   19,  20,  22,  23,  24,  26,  27,  29,  30,  32,  33,  35,  36,  38,  40,  41,
   43,  45,  47,  49,  50,  52,  54,  56,  58,  60,  62,  64,  66,  68,  70,  73,
   75,  77,  79,  81,  83,  86,  88,  90,  92,  95,  97,  99, 102, 104, 106, 109,
  111, 113, 116, 118, 121, 123, 126, 128, 131, 133, 136, 138, 141, 143, 146, 148,
  151, 153, 156, 158, 161, 163, 166, 168, 171, 173, 176, 178, 181, 183, 186, 188,
  191, 193, 196, 198, 200, 203, 205, 208, 210, 212, 215, 217, 219, 221, 224, 226,
  228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 249, 251, 253, 254, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 251, 250,
  249, 248, 247, 245, 244, 243, 241, 240, 238, 237, 235, 233, 231, 230, 228, 226,
  224, 222, 219, 217, 215, 213, 211, 208, 206, 203, 201, 198, 196, 193, 191, 188,
  185, 183, 180, 177, 174, 171, 169, 166, 163, 160, 157, 154, 151, 148, 145, 142,
  138, 135, 132, 129, 126, 122, 119, 116, 113, 109, 106, 103,  99,  96,  93,  89,
   86,  83,  79,  76,  73,  69,  66,  63,  59,  56,  53,  50,  46,  43,  40,  37,
   34,  31,  28,  25,  22,  19,  16,  13,  11,   8,   5,   3,   1,   0,   0,   0
};

static const char* kEffectNames[] = {
    "Static",         "Blink",           "Breath",         "Rainbow",        "Rainbow Cycle",
    "Color Wipe",     "Theater Chase",   "Scan",           "Dual Scan",      "Fade",
    "Comet",          "Twinkle",         "Sparkle",        "Running Lights", "Fireworks",
};

struct Params
{
    Mode mode;
    uint8_t speed;
    uint8_t intensity;
    bool running;
    uint32_t step;
    uint32_t aux0;
    uint32_t aux1;
};

Wrgb8 g_pixels[kLedCount];
Params g_params{};
bool g_timer_active = false;
uint32_t g_rand_state = 0xA5A5A5A5U;

uint8_t Sin8(uint8_t x)
{
    return kSin8Table[x];
}

uint32_t Rand32()
{
    g_rand_state = (g_rand_state * 1103515245U) + 12345U;
    return g_rand_state;
}

Wrgb8 Rgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    return Wrgb8{ r, g, b, w };
}

Wrgb8 ColorWheel(uint8_t pos)
{
    if (pos < 85)
    {
        return Rgbw(static_cast<uint8_t>(pos * 3), static_cast<uint8_t>(255 - pos * 3), 0, 0);
    }
    if (pos < 170)
    {
        pos = static_cast<uint8_t>(pos - 85);
        return Rgbw(static_cast<uint8_t>(255 - pos * 3), 0, static_cast<uint8_t>(pos * 3), 0);
    }
    pos = static_cast<uint8_t>(pos - 170);
    return Rgbw(0, static_cast<uint8_t>(pos * 3), static_cast<uint8_t>(255 - pos * 3), 0);
}

Wrgb8 ColorBlend(Wrgb8 c1, Wrgb8 c2, uint8_t blend)
{
    const uint16_t inv = static_cast<uint16_t>(255 - blend);
    return Rgbw(static_cast<uint8_t>((c1.r * inv + c2.r * blend) / 255),
              static_cast<uint8_t>((c1.g * inv + c2.g * blend) / 255),
              static_cast<uint8_t>((c1.b * inv + c2.b * blend) / 255),
              static_cast<uint8_t>((c1.w * inv + c2.w * blend) / 255));
}

Wrgb8 ColorScale(Wrgb8 color, uint8_t scale)
{
    return Rgbw(static_cast<uint8_t>((color.r * scale) / 255), static_cast<uint8_t>((color.g * scale) / 255),
              static_cast<uint8_t>((color.b * scale) / 255), static_cast<uint8_t>((color.w * scale) / 255));
}

void FadeAll(uint8_t fade_amount)
{
    for (uint16_t i = 0; i < kLedCount; ++i)
    {
        g_pixels[i] = ColorScale(g_pixels[i], static_cast<uint8_t>(255 - fade_amount));
    }
}

void ClearAll()
{
    std::memset(g_pixels, 0, sizeof(g_pixels));
}

void FillAll(Wrgb8 color)
{
    for (uint16_t i = 0; i < kLedCount; ++i)
    {
        g_pixels[i] = color;
    }
}

void PushFrame()
{
    ws2814_strip::WrgbPixel out[kLedCount];
    for (uint16_t i = 0; i < kLedCount; ++i)
    {
        out[i].w = g_pixels[i].w;
        out[i].r = g_pixels[i].r;
        out[i].g = g_pixels[i].g;
        out[i].b = g_pixels[i].b;
    }
    (void) ws2814_strip::FlushPixels(out, kLedCount);
}

uint32_t EffectStatic()
{
    const uint8_t color_index = static_cast<uint8_t>((g_params.step >> 4) & 0xFF);
    Wrgb8 color               = ColorWheel(color_index);
    const uint8_t brightness  = static_cast<uint8_t>(30 + ((g_params.intensity * 225) >> 8));
    color                     = ColorScale(color, brightness);
    FillAll(color);

    const uint32_t speed_factor = (g_params.speed >> 4) + 1;
    g_params.step += speed_factor;
    return 100;
}

uint32_t EffectBlink()
{
    const uint32_t half_period = 100 + (255 - g_params.speed) * 4;
    const bool is_on           = ((g_params.step / half_period) % 2) == 0;

    if (is_on && g_params.aux0 == 0)
    {
        g_params.aux1 = (g_params.aux1 + 17) & 0xFF;
    }
    g_params.aux0 = is_on ? 1 : 0;

    if (is_on)
    {
        Wrgb8 color = ColorWheel(static_cast<uint8_t>(g_params.aux1 & 0xFF));
        color       = ColorScale(color, static_cast<uint8_t>(30 + ((g_params.intensity * 225) >> 8)));
        FillAll(color);
    }
    else
    {
        ClearAll();
    }

    g_params.step += 20;
    return 20;
}

uint32_t EffectBreath()
{
    const uint32_t speed_factor = (g_params.speed >> 3) + 1;
    const uint32_t counter      = g_params.step * speed_factor;
    const uint8_t phase         = static_cast<uint8_t>((counter >> 4) & 0xFF);
    uint8_t brightness          = Sin8(phase);

    const uint8_t last_phase = static_cast<uint8_t>(g_params.aux0 & 0xFF);
    if (last_phase > 200 && phase < 50)
    {
        g_params.aux1 = (g_params.aux1 + 17) & 0xFF;
    }
    g_params.aux0 = phase;

    const uint16_t max_brightness = static_cast<uint16_t>(30 + ((g_params.intensity * 225) >> 8));
    brightness                    = static_cast<uint8_t>((brightness * max_brightness) >> 8);

    Wrgb8 color = ColorWheel(static_cast<uint8_t>(g_params.aux1 & 0xFF));
    color       = ColorScale(color, brightness);
    FillAll(color);

    g_params.step++;
    return 20;
}

uint32_t EffectRainbow()
{
    const uint8_t color_index = static_cast<uint8_t>((g_params.step >> 2) & 0xFF);
    Wrgb8 color               = ColorWheel(color_index);
    color                     = ColorScale(color, static_cast<uint8_t>(30 + ((g_params.intensity * 225) >> 8)));
    FillAll(color);

    g_params.step += (g_params.speed >> 2) + 2;
    return 20;
}

uint32_t EffectRainbowCycle()
{
    const uint8_t base_index = static_cast<uint8_t>((g_params.step >> 2) & 0xFF);
    const uint8_t density    = static_cast<uint8_t>(1 + (g_params.intensity >> 5));

    for (uint16_t i = 0; i < kLedCount; ++i)
    {
        const uint8_t color_index =
            static_cast<uint8_t>(((static_cast<uint32_t>(i) * 255 * density) / kLedCount) + base_index);
        g_pixels[i] = ColorWheel(color_index);
    }

    g_params.step += (g_params.speed >> 2) + 2;
    return 20;
}

uint32_t EffectColorWipe()
{
    const uint32_t cycle_time = 750 + (255 - g_params.speed) * 150;
    const uint32_t perc       = g_params.step % cycle_time;
    uint16_t prog             = static_cast<uint16_t>((perc * 65535U) / cycle_time);
    const bool back           = (prog > 32767);

    if (back)
    {
        prog = static_cast<uint16_t>(prog - 32767);
    }

    const uint16_t led_index    = static_cast<uint16_t>((prog * kLedCount) >> 15);
    const uint8_t color_offset  = static_cast<uint8_t>((g_params.step >> 5) & 0xFF);
    const uint8_t color_span    = static_cast<uint8_t>(85 + (g_params.intensity >> 1));
    const Wrgb8 color1          = ColorWheel(color_offset);
    const Wrgb8 color2          = ColorWheel(static_cast<uint8_t>((color_offset + color_span) & 0xFF));

    for (uint16_t i = 0; i < kLedCount; ++i)
    {
        if (i < led_index)
        {
            g_pixels[i] = back ? color2 : color1;
        }
        else
        {
            g_pixels[i] = back ? color1 : color2;
        }
    }

    g_params.step += 20;
    return 20;
}

uint32_t EffectTheaterChase()
{
    const uint8_t width       = static_cast<uint8_t>(3 + (g_params.intensity >> 4));
    const uint32_t cycle_time = 50 + (255 - g_params.speed);
    const uint8_t color_index = static_cast<uint8_t>((g_params.step >> 3) & 0xFF);

    for (uint16_t i = 0; i < kLedCount; ++i)
    {
        if ((i % width) == g_params.aux0)
        {
            g_pixels[i] = ColorWheel(color_index);
        }
        else
        {
            g_pixels[i] = Rgbw(0, 0, 0, 0);
        }
    }

    if (g_params.step >= cycle_time)
    {
        g_params.aux0 = (g_params.aux0 + 1) % width;
        g_params.step = 0;
    }

    g_params.step += 20;
    return 20;
}

uint32_t EffectScan()
{
    const uint32_t cycle_time = 750 + (255 - g_params.speed) * 150;
    const uint32_t perc       = g_params.step % cycle_time;
    const uint16_t prog       = static_cast<uint16_t>((perc * 65535U) / cycle_time);
    const uint16_t size       = static_cast<uint16_t>(1 + ((g_params.intensity * kLedCount) >> 9));
    const uint16_t led_index  = static_cast<uint16_t>((prog * ((kLedCount * 2) - size * 2)) >> 16);

    const uint8_t bg_color_index = static_cast<uint8_t>((g_params.step >> 4) & 0xFF);
    FillAll(ColorScale(ColorWheel(bg_color_index), 20));

    int16_t led_offset = static_cast<int16_t>(led_index) - static_cast<int16_t>(kLedCount - size);
    if (led_offset < 0)
    {
        led_offset = static_cast<int16_t>(-led_offset);
    }

    const Wrgb8 scan_color = ColorWheel(static_cast<uint8_t>((bg_color_index + 128) & 0xFF));
    for (uint16_t j = static_cast<uint16_t>(led_offset); j < static_cast<uint16_t>(led_offset) + size && j < kLedCount;
         ++j)
    {
        g_pixels[j] = scan_color;
    }

    g_params.step += 20;
    return 20;
}

uint32_t EffectDualScan()
{
    const uint32_t cycle_time = 750 + (255 - g_params.speed) * 150;
    const uint32_t perc       = g_params.step % cycle_time;
    const uint16_t prog       = static_cast<uint16_t>((perc * 65535U) / cycle_time);
    const uint16_t size       = static_cast<uint16_t>(1 + ((g_params.intensity * kLedCount) >> 9));
    const uint16_t led_index  = static_cast<uint16_t>((prog * ((kLedCount * 2) - size * 2)) >> 16);

    const uint8_t bg_color_index = static_cast<uint8_t>((g_params.step >> 4) & 0xFF);
    FillAll(ColorScale(ColorWheel(bg_color_index), 20));

    int16_t led_offset = static_cast<int16_t>(led_index) - static_cast<int16_t>(kLedCount - size);
    if (led_offset < 0)
    {
        led_offset = static_cast<int16_t>(-led_offset);
    }

    const Wrgb8 scan_color1 = ColorWheel(static_cast<uint8_t>((bg_color_index + 85) & 0xFF));
    const Wrgb8 scan_color2 = ColorWheel(static_cast<uint8_t>((bg_color_index + 170) & 0xFF));

    for (uint16_t j = static_cast<uint16_t>(led_offset); j < static_cast<uint16_t>(led_offset) + size && j < kLedCount;
         ++j)
    {
        g_pixels[j] = scan_color1;
        const uint16_t mirror_j = static_cast<uint16_t>(kLedCount - 1 - j);
        if (mirror_j < kLedCount)
        {
            g_pixels[mirror_j] = scan_color2;
        }
    }

    g_params.step += 20;
    return 20;
}

uint32_t EffectFade()
{
    const uint32_t counter = g_params.step * ((g_params.speed >> 3) + 10);
    uint16_t tri           = static_cast<uint16_t>(counter & 0xFFFF);
    if (tri > 32767)
    {
        tri = static_cast<uint16_t>(65535 - tri);
    }
    const uint8_t lum = static_cast<uint8_t>(tri >> 7);

    const uint8_t color_offset = static_cast<uint8_t>((g_params.step >> 6) & 0xFF);
    const uint8_t color_span   = static_cast<uint8_t>(85 + (g_params.intensity >> 1));
    const Wrgb8 color        = ColorBlend(ColorWheel(color_offset),
                                   ColorWheel(static_cast<uint8_t>((color_offset + color_span) & 0xFF)), lum);
    FillAll(color);

    g_params.step++;
    return 20;
}

uint32_t EffectComet()
{
    const uint32_t counter = g_params.step * ((g_params.speed >> 2) + 1);
    uint16_t index         = static_cast<uint16_t>((counter * kLedCount) >> 16);
    index                  = static_cast<uint16_t>(index % kLedCount);

    FadeAll(g_params.intensity);

    const uint8_t color_index = static_cast<uint8_t>((index * 255) / kLedCount);
    g_pixels[index]           = ColorWheel(color_index);

    g_params.step++;
    return 20;
}

uint32_t EffectTwinkle()
{
    FadeAll(32);

    const uint8_t num_twinkles = static_cast<uint8_t>(1 + (g_params.intensity >> 6));
    for (uint8_t i = 0; i < num_twinkles; ++i)
    {
        const uint16_t index       = static_cast<uint16_t>(Rand32() % kLedCount);
        const uint8_t color_index  = static_cast<uint8_t>(Rand32() & 0xFF);
        g_pixels[index]            = ColorWheel(color_index);
    }

    g_params.step++;
    return 20 + (255 - g_params.speed) * 2;
}

uint32_t EffectSparkle()
{
    const uint8_t bg_color_index = static_cast<uint8_t>((g_params.step >> 4) & 0xFF);
    FillAll(ColorScale(ColorWheel(bg_color_index), 30));

    const uint8_t num_sparkles = static_cast<uint8_t>(1 + (g_params.intensity >> 6));
    for (uint8_t i = 0; i < num_sparkles; ++i)
    {
        const uint16_t index      = static_cast<uint16_t>(Rand32() % kLedCount);
        const uint8_t color_index = static_cast<uint8_t>(Rand32() & 0xFF);
        g_pixels[index]           = ColorWheel(color_index);
    }

    g_params.step++;
    return 20 + (255 - g_params.speed);
}

uint32_t EffectRunningLights()
{
    const uint8_t x_scale    = static_cast<uint8_t>(g_params.intensity >> 2);
    const uint32_t counter   = (g_params.step * g_params.speed) >> 4;

    for (uint16_t i = 0; i < kLedCount; ++i)
    {
        const uint16_t phase     = static_cast<uint16_t>(i * x_scale - counter);
        const uint8_t brightness = Sin8(static_cast<uint8_t>(phase & 0xFF));
        const uint8_t color_index =
            static_cast<uint8_t>((i * 255) / kLedCount + (g_params.step & 0xFF));
        g_pixels[i] = ColorScale(ColorWheel(color_index), brightness);
    }

    g_params.step++;
    return 20;
}

uint32_t EffectFireworks()
{
    FadeAll(128);

    if ((Rand32() & 0xFF) < (129 - (g_params.intensity >> 1)))
    {
        const uint16_t index      = static_cast<uint16_t>(Rand32() % kLedCount);
        const uint8_t color_pos   = static_cast<uint8_t>(Rand32() & 0xFF);
        g_pixels[index]           = ColorWheel(color_pos);
    }

    g_params.step++;
    return 20 + (255 - g_params.speed);
}

void OnEffectTimer(chip::System::Layer* layer, void* app_state);

uint32_t RunEffectStep()
{
    switch (g_params.mode)
    {
    case Mode::kStatic:
        return EffectStatic();
    case Mode::kBlink:
        return EffectBlink();
    case Mode::kBreath:
        return EffectBreath();
    case Mode::kRainbow:
        return EffectRainbow();
    case Mode::kRainbowCycle:
        return EffectRainbowCycle();
    case Mode::kColorWipe:
        return EffectColorWipe();
    case Mode::kTheaterChase:
        return EffectTheaterChase();
    case Mode::kScan:
        return EffectScan();
    case Mode::kDualScan:
        return EffectDualScan();
    case Mode::kFade:
        return EffectFade();
    case Mode::kComet:
        return EffectComet();
    case Mode::kTwinkle:
        return EffectTwinkle();
    case Mode::kSparkle:
        return EffectSparkle();
    case Mode::kRunningLights:
        return EffectRunningLights();
    case Mode::kFireworks:
        return EffectFireworks();
    default:
        return EffectStatic();
    }
}

void CancelEffectTimer()
{
    if (!g_timer_active)
    {
        return;
    }

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    chip::DeviceLayer::SystemLayer().CancelTimer(OnEffectTimer, nullptr);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    g_timer_active = false;
}

void OnEffectTimer(chip::System::Layer* /*layer*/, void* /*app_state*/)
{
    if (!g_params.running)
    {
        g_timer_active = false;
        return;
    }

    const uint32_t delay_ms = RunEffectStep();
    PushFrame();

    if (!g_params.running)
    {
        g_timer_active = false;
        return;
    }

    g_timer_active = true;
    (void) chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::Milliseconds32(delay_ms), OnEffectTimer,
                                                       nullptr);
}

void ScheduleEffectTimer(uint32_t delay_ms)
{
    CancelEffectTimer();
    chip::DeviceLayer::PlatformMgr().LockChipStack();
    g_timer_active = true;
    (void) chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::Milliseconds32(delay_ms), OnEffectTimer,
                                                       nullptr);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
}

} // namespace

void Init()
{
    std::memset(&g_params, 0, sizeof(g_params));
    g_params.mode = Mode::kRainbowCycle;
    ClearAll();
    (void) ws2814_strip::Init();
}

bool IsRunning()
{
    return g_params.running;
}

bool Start(Mode mode, uint8_t speed, uint8_t intensity)
{
    if (mode >= Mode::kCount)
    {
        return false;
    }

    RgbcwStripDriver::HaltFadeAnimation();

    g_params.mode      = mode;
    g_params.speed     = speed;
    g_params.intensity = intensity;
    g_params.step      = 0;
    g_params.aux0      = 0;
    g_params.aux1      = 0;
    g_params.running   = true;

    const uint32_t delay_ms = RunEffectStep();
    PushFrame();
    ScheduleEffectTimer(delay_ms);

    ChipLogProgress(AppServer, "WS2814 effect start: %s speed=%u intensity=%u", GetModeName(mode), speed,
                    intensity);
    return true;
}

bool Stop()
{
    if (!g_params.running && !g_timer_active)
    {
        return true;
    }

    g_params.running = false;
    CancelEffectTimer();
    ClearAll();
    PushFrame();
    ChipLogProgress(AppServer, "WS2814 effect stopped");
    return true;
}

bool NextMode()
{
    const auto next = static_cast<Mode>((static_cast<uint8_t>(g_params.mode) + 1) %
                                      static_cast<uint8_t>(Mode::kCount));
    return Start(next, g_params.speed, g_params.intensity);
}

Mode GetMode()
{
    return g_params.mode;
}

const char* GetModeName(Mode mode)
{
    if (mode >= Mode::kCount)
    {
        return "Unknown";
    }
    return kEffectNames[static_cast<uint8_t>(mode)];
}

} // namespace ws2814_effects
