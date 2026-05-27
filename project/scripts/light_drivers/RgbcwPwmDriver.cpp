/*
 * Five-channel RGBCW PWM: R/G/B = pwm0/1/2 (PC0-2), C/W = pwm3/4 (PB4-5).
 */

#include "RgbcwPwmDriver.h"
#include "OvercurrentProtector.h"

#include "sl_pwm_init_pwm0_config.h"
#include "sl_pwm_init_pwm1_config.h"
#include "sl_pwm_init_pwm2_config.h"
#include "sl_pwm_init_pwm3_config.h"
#include "sl_pwm_init_pwm4_config.h"
#include "sl_pwm_instances.h"

#include "sl_gpio.h"

#include <em_gpio.h>
#include <em_timer.h>

#include <app-common/zap-generated/attributes/Accessors.h>
#include <clusters/ColorControl/AttributeIds.h>
#include <clusters/ColorControl/Enums.h>
#include <clusters/LevelControl/AttributeIds.h>
#include <clusters/OnOff/AttributeIds.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>

#include <algorithm>

bool RgbcwPwmDriver::sOn     = false;
uint8_t RgbcwPwmDriver::sLevel = 254;
uint8_t RgbcwPwmDriver::sHue   = 0;
uint8_t RgbcwPwmDriver::sSat   = 0;
uint16_t RgbcwPwmDriver::sCtMireds = kDefaultCtMireds;
bool RgbcwPwmDriver::sUseCt  = false;
bool RgbcwPwmDriver::sPwmStarted      = false;
bool RgbcwPwmDriver::sRouteDisabled   = false;
bool RgbcwPwmDriver::sPreFaultSaved   = false;
bool RgbcwPwmDriver::sPreFaultOn      = false;
uint8_t RgbcwPwmDriver::sPreFaultLevel = 254;
uint16_t RgbcwPwmDriver::sPreFaultCtMireds = kDefaultCtMireds;
bool RgbcwPwmDriver::sPreFaultUseCt   = false;
uint8_t RgbcwPwmDriver::sPreFaultHue  = 0;
uint8_t RgbcwPwmDriver::sPreFaultSat  = 0;

namespace {

sl_pwm_instance_t * const kAllPwms[] = { &sl_pwm_pwm0, &sl_pwm_pwm1, &sl_pwm_pwm2, &sl_pwm_pwm3, &sl_pwm_pwm4 };

// SS6 PWM instance mapping (按实例号对应颜色通道):
//   pwm0/pwm1/pwm2 -> R/G/B
//   pwm3/pwm4     -> Cool/Warm
//
// 为了避免依赖 SL_PWM_PWMx_OUTPUT_PORT/PIN（有些配置可能不生成），这里直接按 SS6 实例约定取通道。
static sl_pwm_instance_t * const kPwmR    = &sl_pwm_pwm0;
static sl_pwm_instance_t * const kPwmG    = &sl_pwm_pwm1;
static sl_pwm_instance_t * const kPwmB    = &sl_pwm_pwm2;
static sl_pwm_instance_t * const kPwmCool = &sl_pwm_pwm3;
static sl_pwm_instance_t * const kPwmWarm = &sl_pwm_pwm4;

bool ActiveHighFor(const sl_pwm_instance_t * pwm)
{
    if (pwm == &sl_pwm_pwm0)
    {
        return SL_PWM_PWM0_POLARITY == PWM_ACTIVE_HIGH;
    }
    if (pwm == &sl_pwm_pwm1)
    {
        return SL_PWM_PWM1_POLARITY == PWM_ACTIVE_HIGH;
    }
    if (pwm == &sl_pwm_pwm2)
    {
        return SL_PWM_PWM2_POLARITY == PWM_ACTIVE_HIGH;
    }
    if (pwm == &sl_pwm_pwm3)
    {
        return SL_PWM_PWM3_POLARITY == PWM_ACTIVE_HIGH;
    }
    if (pwm == &sl_pwm_pwm4)
    {
        return SL_PWM_PWM4_POLARITY == PWM_ACTIVE_HIGH;
    }
    return true;
}

void PwmRouteDisable(const sl_pwm_instance_t & pwm)
{
#if defined(_GPIO_TIMER_ROUTEEN_MASK)
    GPIO->TIMERROUTE_CLR[TIMER_NUM(pwm.timer)].ROUTEEN = 1U << (pwm.channel + _GPIO_TIMER_ROUTEEN_CC0PEN_SHIFT);
#else
    switch (TIMER_NUM(pwm.timer))
    {
    case 0:
        GPIO->TIMER0ROUTE_CLR[0].ROUTEEN = 1U << (pwm.channel + _GPIO_TIMER0_ROUTEEN_CC0PEN_SHIFT);
        break;
    case 1:
        GPIO->TIMER1ROUTE_CLR[0].ROUTEEN = 1U << (pwm.channel + _GPIO_TIMER1_ROUTEEN_CC0PEN_SHIFT);
        break;
    case 2:
        GPIO->TIMER2ROUTE_CLR[0].ROUTEEN = 1U << (pwm.channel + _GPIO_TIMER2_ROUTEEN_CC0PEN_SHIFT);
        break;
    case 4:
        GPIO->TIMER4ROUTE_CLR[0].ROUTEEN = 1U << (pwm.channel + _GPIO_TIMER4_ROUTEEN_CC0PEN_SHIFT);
        break;
    default:
        break;
    }
#endif
}

void PwmCompareZero(const sl_pwm_instance_t & pwm)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
    TIMER_CompareSet(pwm.timer, pwm.channel, 0U);
    TIMER_CompareBufSet(pwm.timer, pwm.channel, 0U);
#else
    sl_pwm_set_duty_cycle(const_cast<sl_pwm_instance_t *>(&pwm), 0);
#endif
}

void RestorePwmPinMode(const sl_pwm_instance_t & pwm, bool activeHigh)
{
    sl_gpio_t gpio = {
        .port = pwm.port,
        .pin  = pwm.pin,
    };
    (void) sl_gpio_set_pin_mode(&gpio, SL_GPIO_MODE_PUSH_PULL, activeHigh);
}

void GpioForceOff(const sl_pwm_instance_t & pwm, bool activeHigh)
{
    sl_gpio_t gpio = {
        .port = pwm.port,
        .pin  = pwm.pin,
    };
    (void) sl_gpio_set_pin_mode(&gpio, SL_GPIO_MODE_PUSH_PULL, activeHigh ? 0 : 1);
}

void KillPwmInstance(sl_pwm_instance_t * pwm)
{
    if (pwm == nullptr)
    {
        return;
    }
    PwmCompareZero(*pwm);
    PwmRouteDisable(*pwm);
    GpioForceOff(*pwm, ActiveHighFor(pwm));
}

void RestorePwmInstance(sl_pwm_instance_t * pwm)
{
    if (pwm == nullptr)
    {
        return;
    }
    RestorePwmPinMode(*pwm, ActiveHighFor(pwm));
    sl_pwm_start(pwm);
}

void ForEachChannel(void (*fn)(sl_pwm_instance_t *))
{
    fn(kPwmR);
    fn(kPwmG);
    fn(kPwmB);
    fn(kPwmCool);
    fn(kPwmWarm);
}

void SetAllDuty(uint8_t r, uint8_t g, uint8_t b, uint8_t cool, uint8_t warm)
{
    sl_pwm_set_duty_cycle(kPwmR, r);
    sl_pwm_set_duty_cycle(kPwmG, g);
    sl_pwm_set_duty_cycle(kPwmB, b);
    sl_pwm_set_duty_cycle(kPwmCool, cool);
    sl_pwm_set_duty_cycle(kPwmWarm, warm);
}

void HsvToRgb(uint8_t hue, uint8_t sat, uint8_t brightness, uint8_t & r, uint8_t & g, uint8_t & b)
{
    if (brightness == 0 || sat == 0)
    {
        r = g = b = 0;
        return;
    }
    const float h = static_cast<float>(hue) * 360.0f / 254.0f;
    const float s = static_cast<float>(sat) / 254.0f;
    const float v = static_cast<float>(brightness) / 100.0f;
    const int hi = static_cast<int>(h / 60.0f) % 6;
    const float f  = h / 60.0f - static_cast<float>(hi);
    const float p  = v * (1.0f - s);
    const float q  = v * (1.0f - f * s);
    const float t  = v * (1.0f - (1.0f - f) * s);
    float rf = 0, gf = 0, bf = 0;
    switch (hi)
    {
    case 0:
        rf = v;
        gf = t;
        bf = p;
        break;
    case 1:
        rf = q;
        gf = v;
        bf = p;
        break;
    case 2:
        rf = p;
        gf = v;
        bf = t;
        break;
    case 3:
        rf = p;
        gf = q;
        bf = v;
        break;
    case 4:
        rf = t;
        gf = p;
        bf = v;
        break;
    default:
        rf = v;
        gf = p;
        bf = q;
        break;
    }
    r = static_cast<uint8_t>(rf * 100.0f);
    g = static_cast<uint8_t>(gf * 100.0f);
    b = static_cast<uint8_t>(bf * 100.0f);
}

void CtToCoolWarm(uint16_t mireds, uint8_t brightness, uint8_t & cool, uint8_t & warm)
{
    const uint16_t ct = std::clamp(mireds, RgbcwPwmDriver::kCtMinMireds, RgbcwPwmDriver::kCtMaxMireds);
    const uint32_t warmWeight = ct - RgbcwPwmDriver::kCtMinMireds;
    const uint32_t sum        = (RgbcwPwmDriver::kCtMaxMireds - RgbcwPwmDriver::kCtMinMireds);
    if (sum == 0)
    {
        cool = brightness / 2;
        warm = brightness - cool;
        return;
    }
    warm = static_cast<uint8_t>((brightness * warmWeight) / sum);
    cool = brightness - warm;
}

} // namespace

void RgbcwPwmDriver::SaveStateBeforeFault()
{
    if (sPreFaultSaved)
    {
        return;
    }

    sPreFaultOn       = sOn;
    sPreFaultLevel    = sLevel;
    sPreFaultCtMireds = sCtMireds;
    sPreFaultUseCt    = sUseCt;
    sPreFaultHue      = sHue;
    sPreFaultSat      = sSat;
    sPreFaultSaved    = true;
}

void RgbcwPwmDriver::PwmOutputKillRegisters()
{
    ForEachChannel(KillPwmInstance);
}

void RgbcwPwmDriver::PwmOutputRestoreRegisters()
{
    ForEachChannel(RestorePwmInstance);
    sPwmStarted    = true;
    sRouteDisabled = false;
}

void RgbcwPwmDriver::Init()
{
    for (auto * pwm : kAllPwms)
    {
        sl_pwm_start(pwm);
    }
    sPwmStarted    = true;
    sRouteDisabled = false;
    SetOn(false);
}

uint8_t RgbcwPwmDriver::LevelToBrightnessPercent(uint8_t level)
{
    if (level == 0)
    {
        return 0;
    }
    return static_cast<uint8_t>((static_cast<uint32_t>(level) * 100U) / 254U);
}

void RgbcwPwmDriver::ApplyOutputImmediate()
{
    if (OvercurrentProtector::IsFaultActive())
    {
        PwmOutputKillRegisters();
        sRouteDisabled = true;
        return;
    }

    if (sRouteDisabled || !sPwmStarted)
    {
        PwmOutputRestoreRegisters();
    }

    if (!sOn)
    {
        SetAllDuty(0, 0, 0, 0, 0);
        return;
    }

    const uint8_t br = LevelToBrightnessPercent(sLevel);
    if (sUseCt)
    {
        uint8_t cool = 0;
        uint8_t warm = 0;
        CtToCoolWarm(sCtMireds, br, cool, warm);
        SetAllDuty(0, 0, 0, cool, warm);
    }
    else
    {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        HsvToRgb(sHue, sSat, br, r, g, b);
        SetAllDuty(r, g, b, 0, 0);
    }
}

void RgbcwPwmDriver::SetOn(bool on)
{
    if (on && OvercurrentProtector::BlocksTurnOn())
    {
        ChipLogProgress(AppServer, "RgbcwPwm: on rejected (protection fault)");
        return;
    }

    sOn = on;
    if (on && sLevel <= 1)
    {
        sLevel = 254;
    }
    ApplyOutputImmediate();
}

void RgbcwPwmDriver::SetLevel(uint8_t level)
{
    sLevel = level;
    ApplyOutputImmediate();
}

void RgbcwPwmDriver::SetHueSat(uint8_t hue, uint8_t saturation)
{
    sHue   = hue;
    sSat   = saturation;
    sUseCt = false;
    ApplyOutputImmediate();
}

void RgbcwPwmDriver::SetColorTemperatureMireds(uint16_t mireds)
{
    sCtMireds = std::clamp(mireds, kCtMinMireds, kCtMaxMireds);
    sUseCt    = true;
    ApplyOutputImmediate();
}

void RgbcwPwmDriver::ApplyClusterLevel(chip::EndpointId endpoint, uint8_t clusterLevel)
{
    chip::DeviceLayer::PlatformMgr().LockChipStack();
    const uint8_t level = ResolveLevelForPwmLocked(endpoint, sOn, clusterLevel);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    SetLevel(level);
}

uint8_t RgbcwPwmDriver::ResolveLevelForPwmLocked(chip::EndpointId endpoint, bool on, uint8_t clusterLevel)
{
    (void) endpoint;
    if (!on)
    {
        return sLevel;
    }
    if (clusterLevel <= 1)
    {
        return clusterLevel;
    }
    return clusterLevel;
}

void RgbcwPwmDriver::SyncFromMatterEndpoint(chip::EndpointId endpoint)
{
    using namespace chip::app::Clusters;

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    bool on = false;
    uint8_t level = 254;
    uint8_t hue = 0;
    uint8_t sat = 0;
    uint16_t ct = kDefaultCtMireds;
    ColorControl::ColorModeEnum colorMode = ColorControl::ColorModeEnum::kCurrentHueAndCurrentSaturation;

    OnOff::Attributes::OnOff::Get(endpoint, &on);
    chip::app::DataModel::Nullable<uint8_t> currentLevel;
    if (LevelControl::Attributes::CurrentLevel::Get(endpoint, currentLevel) == chip::Protocols::InteractionModel::Status::Success &&
        !currentLevel.IsNull())
    {
        level = currentLevel.Value();
    }
    ColorControl::Attributes::CurrentHue::Get(endpoint, &hue);
    ColorControl::Attributes::CurrentSaturation::Get(endpoint, &sat);
    ColorControl::Attributes::ColorTemperatureMireds::Get(endpoint, &ct);
    ColorControl::Attributes::ColorMode::Get(endpoint, &colorMode);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

    sOn    = on;
    sLevel = level;
    sHue   = hue;
    sSat   = sat;
    sCtMireds = ct;
    sUseCt = (colorMode == ColorControl::ColorModeEnum::kColorTemperatureMireds);
    ApplyOutputImmediate();
}

void RgbcwPwmDriver::ForceOffForFaultFromIsr()
{
    SaveStateBeforeFault();
    sOn = false;
    PwmOutputKillRegisters();
    sRouteDisabled = true;
}

void RgbcwPwmDriver::ForceOffForFault()
{
    SaveStateBeforeFault();
    sOn = false;
    PwmOutputKillRegisters();
    sRouteDisabled = true;
}

void RgbcwPwmDriver::RecoverFromFault()
{
    if (sPreFaultSaved)
    {
        RestoreToPreFault(sPreFaultOn, sPreFaultLevel, sPreFaultCtMireds);
        return;
    }

    PwmOutputRestoreRegisters();
    ApplyOutputImmediate();
}

void RgbcwPwmDriver::RestoreToPreFault(bool on, uint8_t level, uint16_t ctMireds)
{
    sOn            = on;
    sLevel         = level;
    sCtMireds      = std::clamp(ctMireds, kCtMinMireds, kCtMaxMireds);
    sUseCt         = sPreFaultUseCt;
    sHue           = sPreFaultHue;
    sSat           = sPreFaultSat;
    sPreFaultSaved = false;

    PwmOutputRestoreRegisters();
    ApplyOutputImmediate();

    ChipLogProgress(AppServer, "RgbcwPwm RECOVER: on=%u level=%u ct=%u useCt=%u", sOn, sLevel, sCtMireds, sUseCt);
}

bool RgbcwPwmDriver::GetPreFaultState(bool & on, uint8_t & level, uint16_t & ctMireds)
{
    if (!sPreFaultSaved)
    {
        return false;
    }
    on       = sPreFaultOn;
    level    = sPreFaultLevel;
    ctMireds = sPreFaultCtMireds;
    return true;
}
