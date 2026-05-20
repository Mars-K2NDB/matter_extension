/*
 * Dual-channel PWM driver for Matter color-temperature lighting.
 * PWM0 = cool (cold), PWM1 = warm.
 */

#include "CtDualPwmDriver.h"
#include "CtPwmVersion.h"

#include "sl_pwm_instances.h"

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
bool CtDualPwmDriver::sPwmStarted   = false;

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

void CtDualPwmDriver::SetOn(bool on)
{
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
    if (!sPwmStarted)
    {
        sl_pwm_start(&sl_pwm_pwm0);
        sl_pwm_start(&sl_pwm_pwm1);
        sPwmStarted = true;
    }

    if (!sOn)
    {
        sl_pwm_set_duty_cycle(&sl_pwm_pwm0, 0);
        sl_pwm_set_duty_cycle(&sl_pwm_pwm1, 0);
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

    sl_pwm_set_duty_cycle(&sl_pwm_pwm0, coolDuty);
    sl_pwm_set_duty_cycle(&sl_pwm_pwm1, warmDuty);

    ChipLogDetail(Zcl, "CtPwm duty: cool=%u warm=%u (ct=%u brightness=%u)", coolDuty, warmDuty, ct, brightness);
}
