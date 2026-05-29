/*
 * RGBCW SPI 幻彩灯带（接口与 RgbcwPwmDriver 对齐，输出走 SPI）。
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
    enum class FadeKind : uint8_t
    {
        kOnOff = 0,
        kLevel = 1,
        kColor = 2,
    };

    static void CancelFadeTimer();
    static void ScheduleFade(FadeKind kind, bool restartFade);
    static void ApplyFadeFrame(uint16_t step);
    static void OnFadeTimer(chip::System::Layer * layer, void * appState);
    static void ComputeTargetOutput(uint8_t & r, uint8_t & g, uint8_t & b, uint8_t & c, uint8_t & w);
    static void ApplyDisplayOutput(uint8_t r, uint8_t g, uint8_t b, uint8_t c, uint8_t w);
    static void ApplyOutputImmediate();
    static uint8_t LevelToBrightnessPercent(uint8_t level);

    static bool sOn;
    static uint8_t sLevel;
    static uint8_t sHue;
    static uint8_t sSat;
    static uint16_t sCtMireds;
    static bool sUseCt;
    static bool sPreFaultSaved;
    static bool sPreFaultOn;
    static uint8_t sPreFaultLevel;
    static uint16_t sPreFaultCtMireds;
    static bool sLastOnValid;
    static uint8_t sLastOnLevel;
    static uint16_t sLastOnCtMireds;
    static bool sLastOnUseCt;
    static uint8_t sLastOnHue;
    static uint8_t sLastOnSat;

    static uint8_t sDisplayR;
    static uint8_t sDisplayG;
    static uint8_t sDisplayB;
    static uint8_t sDisplayC;
    static uint8_t sDisplayW;
    static uint8_t sFadeStartR;
    static uint8_t sFadeStartG;
    static uint8_t sFadeStartB;
    static uint8_t sFadeStartC;
    static uint8_t sFadeStartW;
    static uint8_t sFadeTargetR;
    static uint8_t sFadeTargetG;
    static uint8_t sFadeTargetB;
    static uint8_t sFadeTargetC;
    static uint8_t sFadeTargetW;
    static FadeKind sFadeKind;
    static uint16_t sFadeStep;
    static uint16_t sFadeStepsTotal;
    static bool sFadeActive;
};
