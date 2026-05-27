/*
 * RGBCW SPI 幻彩灯带（接口与 RgbcwPwmDriver 对齐，输出走 SPI）。
 */
#pragma once

#include <cstdint>
#include <lib/core/DataModelTypes.h>

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
};
