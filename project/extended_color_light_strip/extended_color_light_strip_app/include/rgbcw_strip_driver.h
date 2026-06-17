/*
 * RGBCW SPI 幻彩灯带（WS2814 SPI + 四混五）。
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
    static void SetOn(chip::EndpointId endpoint, bool on);
    static void SetLevel(uint8_t level);
    static void SetHueSat(uint8_t hue, uint8_t saturation);
    static void SetXy(uint16_t current_x, uint16_t current_y);
    static void SetColorTemperatureMireds(uint16_t mireds);
    static void ApplyClusterLevel(chip::EndpointId endpoint, uint8_t cluster_level);
    static void SyncFromMatterEndpoint(chip::EndpointId endpoint);
    static void ForceOffForFault();
    static void ForceOffForFaultFromIsr();
    /** 停止渐变定时器，供灯效引擎接管 SPI 输出 */
    static void HaltFadeAnimation();
    static void RecoverFromFault();
    static void RestoreToPreFault(bool on, uint8_t level, uint16_t ct_mireds);
    static bool GetPreFaultState(bool& on, uint8_t& level, uint16_t& ct_mireds);
    static uint8_t ResolveLevelForCluster(chip::EndpointId endpoint, bool on, uint8_t cluster_level);

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
    static void ScheduleFade(FadeKind kind, bool restart_fade);
    static void ApplyFadeFrame(uint16_t step);
    static void OnFadeTimer(chip::System::Layer* layer, void* app_state);
    static void ComputeTargetOutput(Rgbw1024& out);
    static void ApplyDisplayOutput(const Rgbw1024& rgbw);
    static void ApplyOutputImmediate();
    static void ApplyRgbwLumaCompensation(Rgbw1024& current, const Rgbw1024& start, const Rgbw1024& target,
                                          float progress);

    enum class ColorPath : uint8_t
    {
        kCt  = 0,
        kHsv = 1,
        kXy  = 2,
    };

    static ColorPath ResolveColorPathFromMatter(chip::EndpointId endpoint);
    static void PublishMatterColorMode(ColorPath path);

    static bool on_;
    static uint8_t level_;
    static uint8_t hue_;
    static uint8_t sat_;
    static uint16_t ct_mireds_;
    static uint16_t current_x_;
    static uint16_t current_y_;
    static ColorPath color_path_;
    static bool pre_fault_saved_;
    static bool pre_fault_on_;
    static uint8_t pre_fault_level_;
    static uint16_t pre_fault_ct_mireds_;
    static bool last_on_valid_;
    static uint8_t last_on_level_;
    static uint16_t last_on_ct_mireds_;
    static bool last_on_use_ct_;
    static ColorPath last_on_color_path_;
    static uint8_t last_on_hue_;
    static uint8_t last_on_sat_;

    static Rgbw1024 display_;
    static Rgbw1024 fade_start_;
    static Rgbw1024 fade_target_;
    static FadeKind fade_kind_;
    static uint16_t fade_step_;
    static uint16_t fade_steps_total_;
    static bool fade_active_;
};
