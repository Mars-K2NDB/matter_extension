/*
 * Dual-channel PWM driver for Matter color-temperature lighting.
 * PWM0 = cool (cold), PWM1 = warm.
 */

#include "CtDualPwmDriver.h"
#include "CtPwmVersion.h"
#include "OvercurrentProtector.h"

#include "sl_pwm_init_pwm0_config.h"
#include "sl_pwm_init_pwm1_config.h"
#include "sl_pwm_instances.h"

#include <em_gpio.h>
#include <em_timer.h>

#include "sl_gpio.h"

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/clusters/on-off-server/on-off-server.h>
#include <app/data-model/Nullable.h>
#include <clusters/ColorControl/AttributeIds.h>
#include <clusters/LevelControl/AttributeIds.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>
#include <silabs_utils.h>

#include <algorithm>

bool CtDualPwmDriver::sOn         = false;
uint8_t CtDualPwmDriver::sLevel     = 254;
uint16_t CtDualPwmDriver::sCtMireds = kDefaultCtMireds;
bool CtDualPwmDriver::sPwmStarted        = false;
bool CtDualPwmDriver::sRouteDisabled     = false;
bool CtDualPwmDriver::sPreFaultSaved     = false;
bool CtDualPwmDriver::sPreFaultOn        = false;
uint8_t CtDualPwmDriver::sPreFaultLevel    = 254;
uint16_t CtDualPwmDriver::sPreFaultCtMireds = kDefaultCtMireds;

namespace {

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

void PwmSetDutyImmediate(const sl_pwm_instance_t & pwm, uint8_t percent)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
    const uint32_t top  = TIMER_TopGet(pwm.timer);
    const uint32_t cmp  = (top * static_cast<uint32_t>(percent)) / 100U;
    TIMER_CompareSet(pwm.timer, pwm.channel, cmp);
    TIMER_CompareBufSet(pwm.timer, pwm.channel, cmp);
#else
    sl_pwm_set_duty_cycle(const_cast<sl_pwm_instance_t *>(&pwm), percent);
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

/** Register-level PWM kill: compare 0 + disconnect TIMER route (no GPIO hold). */
void PwmOutputKillRegisters()
{
    PwmCompareZero(sl_pwm_pwm0);
    PwmCompareZero(sl_pwm_pwm1);
    PwmRouteDisable(sl_pwm_pwm0);
    PwmRouteDisable(sl_pwm_pwm1);
}

} // namespace

void CtDualPwmDriver::PwmOutputRestoreRegisters()
{
    RestorePwmPinMode(sl_pwm_pwm0, SL_PWM_PWM0_POLARITY == PWM_ACTIVE_HIGH);
    RestorePwmPinMode(sl_pwm_pwm1, SL_PWM_PWM1_POLARITY == PWM_ACTIVE_HIGH);

    sl_pwm_start(&sl_pwm_pwm0);
    sl_pwm_start(&sl_pwm_pwm1);
    sPwmStarted    = true;
    sRouteDisabled = false;
}

void CtDualPwmDriver::SaveStateBeforeFault()
{
    if (sPreFaultSaved)
    {
        return;
    }

    sPreFaultOn        = sOn;
    sPreFaultLevel     = sLevel;
    sPreFaultCtMireds  = sCtMireds;
    sPreFaultSaved     = true;
}

bool CtDualPwmDriver::GetPreFaultState(bool & on, uint8_t & level, uint16_t & ctMireds)
{
    if (!sPreFaultSaved)
    {
        return false;
    }

    on      = sPreFaultOn;
    level   = sPreFaultLevel;
    ctMireds = sPreFaultCtMireds;
    return true;
}

void CtDualPwmDriver::RecoverFromFault()
{
    PwmOutputRestoreRegisters();

    if (sPreFaultSaved)
    {
        sOn       = sPreFaultOn;
        sLevel    = sPreFaultLevel;
        sCtMireds = sPreFaultCtMireds;
        sPreFaultSaved = false;
    }

    ApplyOutput();

    ChipLogProgress(AppServer, "CtPwm RECOVER: on=%u level=%u ct=%u", sOn, sLevel, sCtMireds);
}

void CtDualPwmDriver::LogVersion()
{
    SILABS_LOG("CtDualPwm driver v%s (PWM0=cool, PWM1=warm, mired %u-%u)", CT_DUAL_PWM_DRIVER_VERSION,
               static_cast<unsigned>(kCtMinMireds), static_cast<unsigned>(kCtMaxMireds));
    ChipLogProgress(AppServer, "CtDualPwm driver v%s (PWM0=cool, PWM1=warm)", CT_DUAL_PWM_DRIVER_VERSION);
}

void CtDualPwmDriver::Init()
{
    LogVersion();

    if (!sPwmStarted)
    {
        sl_pwm_start(&sl_pwm_pwm0);
        sl_pwm_start(&sl_pwm_pwm1);
        sPwmStarted = true;
    }
    ApplyOutput();
}

void CtDualPwmDriver::ForceOffForFaultFromIsr()
{
    SaveStateBeforeFault();
    sOn = false;
    PwmOutputKillRegisters();
    sRouteDisabled = true;
}

void CtDualPwmDriver::ForceOffForFault()
{
    SaveStateBeforeFault();
    sOn = false;
    PwmOutputKillRegisters();
    sRouteDisabled = true;
}

void CtDualPwmDriver::SetOn(bool on)
{
    if (on && OvercurrentProtector::BlocksTurnOn())
    {
        ChipLogProgress(AppServer, "CtPwm: on rejected (overcurrent fault)");
        return;
    }

    sOn = on;
    if (on && sLevel <= 1)
    {
        sLevel = 254;
    }
    ApplyOutput();
    ChipLogProgress(Zcl, "CtPwm On -> %u (ct=%u level=%u)", on, sCtMireds, sLevel);
}

void CtDualPwmDriver::SetLevel(uint8_t level)
{
    sLevel = level;
    ApplyOutput();
}

void CtDualPwmDriver::ApplyClusterLevel(chip::EndpointId endpoint, uint8_t clusterLevel)
{
    if (sOn && clusterLevel <= 1)
    {
        return;
    }

    if (!sOn)
    {
        sLevel = clusterLevel;
        return;
    }

    sLevel = ResolveLevelForPwm(endpoint, true, clusterLevel);
    ApplyOutput();
}

void CtDualPwmDriver::SetColorTemperatureMireds(uint16_t mireds)
{
    sCtMireds = mireds;
    ApplyOutput();
    ChipLogProgress(Zcl, "CtPwm CT -> %u mireds (on=%u level=%u)", mireds, sOn, sLevel);
}

uint8_t CtDualPwmDriver::ResolveLevelForPwmLocked(chip::EndpointId endpoint, bool on, uint8_t clusterLevel)
{
    if (!on)
    {
        return 0;
    }

    uint8_t level = clusterLevel;
    if (level > 1)
    {
        return level;
    }

    using namespace chip;
    using namespace chip::app;
    using namespace chip::app::Clusters;
    using namespace chip::Protocols::InteractionModel;

    DataModel::Nullable<uint8_t> onLevel;
    if (LevelControl::Attributes::OnLevel::Get(endpoint, onLevel) == Status::Success && !onLevel.IsNull() &&
        onLevel.Value() > level)
    {
        return onLevel.Value();
    }

    if (sLevel > 1)
    {
        return sLevel;
    }

    return 254;
}

uint8_t CtDualPwmDriver::ResolveLevelForPwm(chip::EndpointId endpoint, bool on, uint8_t clusterLevel)
{
    chip::DeviceLayer::PlatformMgr().LockChipStack();
    const uint8_t level = ResolveLevelForPwmLocked(endpoint, on, clusterLevel);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    return level;
}

void CtDualPwmDriver::RefreshFromMatterEndpoint(chip::EndpointId endpoint)
{
    if (OvercurrentProtector::IsFaultActive())
    {
        sOn = false;
        ApplyOutput();
        return;
    }

    using namespace chip;
    using namespace chip::app;
    using namespace chip::app::Clusters;
    using namespace chip::Protocols::InteractionModel;

    bool on           = false;
    uint8_t level     = sLevel;
    uint16_t ctMireds = sCtMireds;

    OnOffServer::Instance().getOnOffValue(endpoint, &on);

    DataModel::Nullable<uint8_t> currentLevel;
    if (LevelControl::Attributes::CurrentLevel::Get(endpoint, currentLevel) == Status::Success && !currentLevel.IsNull())
    {
        level = ResolveLevelForPwmLocked(endpoint, on, currentLevel.Value());
    }

    if (ColorControl::Attributes::ColorTemperatureMireds::Get(endpoint, &ctMireds) != Status::Success)
    {
        ctMireds = kDefaultCtMireds;
    }

    sOn       = on;
    sLevel    = level;
    sCtMireds = ctMireds;

    ApplyOutput();

    ChipLogProgress(Zcl, "CtPwm sync: on=%u level=%u ct=%u", on, level, ctMireds);
}

void CtDualPwmDriver::SyncFromMatterEndpoint(chip::EndpointId endpoint)
{
    chip::DeviceLayer::PlatformMgr().LockChipStack();
    RefreshFromMatterEndpoint(endpoint);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
}

uint8_t CtDualPwmDriver::LevelToBrightnessPercent(uint8_t level)
{
    if (level == 0)
    {
        return 0;
    }
    return static_cast<uint8_t>(1U + (static_cast<uint32_t>(level - 1U) * 99U) / 253U);
}

void CtDualPwmDriver::ApplyOutput()
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
        PwmSetDutyImmediate(sl_pwm_pwm0, 0);
        PwmSetDutyImmediate(sl_pwm_pwm1, 0);
        return;
    }

    const uint16_t ct = std::clamp(sCtMireds, kCtMinMireds, kCtMaxMireds);

    const uint32_t coolWeight = kCtMaxMireds - ct;
    const uint32_t warmWeight = ct - kCtMinMireds;
    const uint32_t weightSum  = coolWeight + warmWeight;
    const uint8_t brightness  = LevelToBrightnessPercent(sLevel);

    uint8_t coolDuty = 0;
    uint8_t warmDuty = 0;
    if (weightSum > 0)
    {
        coolDuty = static_cast<uint8_t>((static_cast<uint32_t>(brightness) * coolWeight) / weightSum);
        warmDuty = static_cast<uint8_t>((static_cast<uint32_t>(brightness) * warmWeight) / weightSum);
    }

    PwmSetDutyImmediate(sl_pwm_pwm0, coolDuty);
    PwmSetDutyImmediate(sl_pwm_pwm1, warmDuty);

    ChipLogDetail(Zcl, "CtPwm duty: cool=%u warm=%u (ct=%u brightness=%u)", coolDuty, warmDuty, ct, brightness);
}
