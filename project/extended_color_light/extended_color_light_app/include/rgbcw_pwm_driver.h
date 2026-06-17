/*
 * Five-channel RGBCW PWM driver (R/G/B on PC0-2, C/W on PB4-5).
 */

#pragma once

#include <cstdint>
#include <lib/core/DataModelTypes.h>

namespace chip {
namespace System {
class Layer;
} // namespace System
} // namespace chip

class RgbcwPwmDriver
{
public:
    static constexpr uint16_t kCtMinMireds     = 111;
    static constexpr uint16_t kCtMaxMireds     = 500;
    static constexpr uint16_t kDefaultCtMireds = 250;

    static void Init();
    static void SetOn(chip::EndpointId endpoint, bool on);
    static void SetLevel(uint8_t level);
    static void SetHueSat(uint8_t hue, uint8_t saturation);
    static void SetColorTemperatureMireds(uint16_t mireds);
    static void ApplyClusterLevel(chip::EndpointId endpoint, uint8_t cluster_level);
    static void SyncFromMatterEndpoint(chip::EndpointId endpoint);
    static void ForceOffForFault();
    static void ForceOffForFaultFromIsr();
    static void RecoverFromFault();
    static void RestoreToPreFault(bool on, uint8_t level, uint16_t ct_mireds);
    static bool GetPreFaultState(bool& on, uint8_t& level, uint16_t& ct_mireds);
    static uint8_t ResolveLevelForCluster(chip::EndpointId endpoint, bool on, uint8_t cluster_level);

    /** Provisioning reminder overlay; does not change Matter light state. */
    static void ApplyProvisionReminderOutput(bool on);

private:
    struct LightState
    {
        bool on             = false;
        uint8_t level       = 254;
        uint8_t hue         = 0;
        uint8_t saturation  = 0;
        uint16_t ct_mireds   = kDefaultCtMireds;
        bool use_ct = false;
    };

    struct RuntimeState
    {
        LightState light;
        bool pwm_started    = false;
        bool route_disabled = false;
    };

    struct FaultSnapshot
    {
        bool saved = false;
        LightState light;
    };

    struct LastOnSnapshot
    {
        bool valid = false;
        LightState light;
    };

    struct OutputFrame
    {
        uint8_t r    = 0;
        uint8_t g    = 0;
        uint8_t b    = 0;
        uint8_t cool = 0;
        uint8_t warm = 0;
    };

    enum class FadeKind : uint8_t
    {
        kOnOff = 0,
        kLevel = 1,
        kColor = 2,
    };

    struct FadeState
    {
        OutputFrame start;
        OutputFrame target;
        FadeKind kind       = FadeKind::kOnOff;
        uint16_t step       = 0;
        uint16_t steps_total = 0;
        bool active         = false;
    };

    static void SaveStateBeforeFault();
    static void PwmOutputKillRegisters();
    static void PwmOutputRestoreRegisters();
    static void CancelFadeTimer();
    static void ScheduleFade(FadeKind kind, bool restart_fade);
    static void ApplyFadeFrame(uint16_t step);
    static void OnFadeTimer(chip::System::Layer* layer, void* app_state);
    static void ComputeTargetDuties(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& cool, uint8_t& warm);
    static void ApplyDisplayDuties(uint8_t r, uint8_t g, uint8_t b, uint8_t cool, uint8_t warm);
    static void CaptureLastOnState();
    static void RestoreLastOnStateIfNeeded();
    static void SyncLastOnStateIfOn();
    static void ApplyOutputImmediate();
    static uint8_t LevelToBrightnessPercent(uint8_t level);

    static RuntimeState runtime_;
    static FaultSnapshot fault_;
    static LastOnSnapshot last_on_;
    static OutputFrame display_;
    static FadeState fade_;
};
