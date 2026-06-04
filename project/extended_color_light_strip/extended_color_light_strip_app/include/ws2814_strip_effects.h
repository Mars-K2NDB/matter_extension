/*
 * WS2814 灯带动画（移植自 Telink zephyr_ws2814_effects.c）。
 */
#pragma once

#include <cstdint>

namespace ws2814_effects {

enum class Mode : uint8_t
{
    kStatic = 0,
    kBlink,
    kBreath,
    kRainbow,
    kRainbowCycle,
    kColorWipe,
    kTheaterChase,
    kScan,
    kDualScan,
    kFade,
    kComet,
    kTwinkle,
    kSparkle,
    kRunningLights,
    kFireworks,
    kCount
};

struct Wrgb8
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t w;
};

void Init();
bool IsRunning();
bool Start(Mode mode, uint8_t speed = 128, uint8_t intensity = 128);
bool Stop();
bool NextMode();
Mode GetMode();
const char* GetModeName(Mode mode);

} // namespace ws2814_effects
