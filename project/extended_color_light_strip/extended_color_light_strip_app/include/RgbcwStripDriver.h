/*
 * RGBCW SPI 幻彩灯带（接口与 RgbcwPwmDriver 对齐，输出走 WS2814 SPI + 四混五）。
 */
#pragma once

#include <cstdint>
#include <lib/core/DataModelTypes.h>

namespace chip {
namespace System {
class Layer;
} // namespace System
} // namespace chip

class RgbcwStripDriver
{
public:
    static constexpr uint16_t kCtMinMireds     = 111;
    static constexpr uint16_t kCtMaxMireds     = 500;
    static constexpr uint16_t kDefaultCtMireds = 250;

    static void Init();
    static void SetOn(bool on);
    static void SetLevel(uint8_t level);
    static void SetHueSat(uint8_t hue, uint8_t saturation);
    static void SetXy(uint16_t currentX, uint16_t currentY);
    static void SetColorTemperatureMireds(uint16_t mireds);
    static void ApplyClusterLevel(chip::EndpointId endpoint, uint8_t clusterLevel);
    static void SyncFromMatterEndpoint(chip::EndpointId endpoint);
    static void ForceOffForFault();
    static void ForceOffForFaultFromIsr();
    static void RecoverFromFault();
    static void RestoreToPreFault(bool on, uint8_t level, uint16_t ctMireds);
    static bool GetPreFaultState(bool & on, uint8_t & level, uint16_t & ctMireds);
    static uint8_t ResolveLevelForPwmLocked(chip::EndpointId endpoint, bool on, uint8_t clusterLevel);

private:
    struct Rgbw1024
    {
        uint16_t r;
        uint16_t g;
        uint16_t b;
        uint16_t w;
    };

    enum class FadeKind : uint8_t
    {
        kOnOff = 0,
        kLevel = 1,
        kColor = 2,
    };

    static void CancelFadeTimer();
    static void ScheduleFade(FadeKind kind, bool restartFade);
    static void ApplyFadeFrame(uint16_t step);
    static void OnFadeTimer(chip::System::Layer* layer, void* appState);
    static void ComputeTargetOutput(Rgbw1024 & out);
    static void ApplyDisplayOutput(const Rgbw1024 & rgbw);
    static void ApplyOutputImmediate();
    static void ApplyRgbwLumaCompensation(Rgbw1024 & current, const Rgbw1024 & start, const Rgbw1024 & target, float progress);

    enum class ColorPath : uint8_t
    {
        kCt  = 0,
        kHsv = 1,
        kXy  = 2,
    };

    static ColorPath ResolveColorPathFromMatter(chip::EndpointId endpoint);
    static void PublishMatterColorMode(ColorPath path);

    static bool sOn;
    static uint8_t sLevel;
    static uint8_t sHue;
    static uint8_t sSat;
    static uint16_t sCtMireds;
    static uint16_t sCurrentX;
    static uint16_t sCurrentY;
    static ColorPath sColorPath;
    static bool sPreFaultSaved;
    static bool sPreFaultOn;
    static uint8_t sPreFaultLevel;
    static uint16_t sPreFaultCtMireds;
    static bool sLastOnValid;
    static uint8_t sLastOnLevel;
    static uint16_t sLastOnCtMireds;
    static bool sLastOnUseCt;
    static ColorPath sLastOnColorPath;
    static uint8_t sLastOnHue;
    static uint8_t sLastOnSat;

    static Rgbw1024 sDisplay;
    static Rgbw1024 sFadeStart;
    static Rgbw1024 sFadeTarget;
    static FadeKind sFadeKind;
    static uint16_t sFadeStep;
    static uint16_t sFadeStepsTotal;
    static bool sFadeActive;
};
