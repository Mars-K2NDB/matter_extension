/*
 * Overcurrent protection: ADC average -> PWM off, Matter OnOff off, latch until cleared.
 */

#include "OvercurrentProtector.h"

#include "AppConfig.h"
#include "LightOutput.h"
#include "overcurrent_protect_config.h"
#include "ShortCircuitProtector.h"
#include "VoltageAdcDriver.h"

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/clusters/on-off-server/on-off-server.h>
#include <app/reporting/reporting.h>
#include <clusters/ColorControl/AttributeIds.h>
#include <clusters/LevelControl/AttributeIds.h>
#include <clusters/OnOff/AttributeIds.h>
#include <protocols/interaction_model/StatusCode.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>

#include <algorithm>

#include <em_gpio.h>
#include <sl_gpio.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::DeviceLayer;

bool OvercurrentProtector::sFaultActive       = false;
bool OvercurrentProtector::sMatterOffPending  = false;
bool OvercurrentProtector::sSnapValid         = false;
bool OvercurrentProtector::sSnapOn            = false;
uint8_t OvercurrentProtector::sSnapLevel      = 0;
uint16_t OvercurrentProtector::sSnapCtMireds    = LightOutput::kDefaultCtMireds;
uint32_t OvercurrentProtector::sAvgMillivolts = 0;
uint32_t OvercurrentProtector::sRecoveryMs   = 0;
uint32_t OvercurrentProtector::sSampleRing[OVERCURRENT_AVG_SAMPLES] = {};
uint8_t OvercurrentProtector::sSampleCount = 0;
uint8_t OvercurrentProtector::sSampleIndex = 0;

namespace {

#if defined(OVERCURRENT_GPIO_OK_PORT) && defined(OVERCURRENT_GPIO_OK_PIN)
GPIO_Port_TypeDef GpioOkPort()
{
    switch (OVERCURRENT_GPIO_OK_PORT)
    {
    case SL_GPIO_PORT_A:
        return gpioPortA;
    case SL_GPIO_PORT_B:
        return gpioPortB;
    case SL_GPIO_PORT_C:
        return gpioPortC;
    case SL_GPIO_PORT_D:
        return gpioPortD;
    default:
        return gpioPortA;
    }
}
#endif

void SetMatterOnOff(bool on)
{
    PlatformMgr().LockChipStack();
    OnOffServer::Instance().setOnOffValue(LIGHT_ENDPOINT, on ? 1U : 0U, false);
    MatterReportingAttributeChangeCallback(
        ConcreteAttributePath(LIGHT_ENDPOINT, OnOff::Id, OnOff::Attributes::OnOff::Id));
    PlatformMgr().UnlockChipStack();
}

void RestoreMatterLightState(bool on, uint8_t level, uint16_t ctMireds)
{
    using namespace chip::Protocols::InteractionModel;

    const uint16_t ct = std::clamp(ctMireds, LightOutput::kCtMinMireds, LightOutput::kCtMaxMireds);

    PlatformMgr().LockChipStack();

    // Match CtDualPwm: Matter CurrentLevel==1 while on maps to effective PWM brightness level.
    const uint8_t matterLevel = on ? LightOutput::ResolveLevelForPwmLocked(LIGHT_ENDPOINT, true, level) : level;

    if (ColorControl::Attributes::ColorTemperatureMireds::Set(LIGHT_ENDPOINT, ct) == Status::Success)
    {
        MatterReportingAttributeChangeCallback(ConcreteAttributePath(LIGHT_ENDPOINT, ColorControl::Id,
                                                                     ColorControl::Attributes::ColorTemperatureMireds::Id));
    }

    if (on && LevelControl::Attributes::CurrentLevel::Set(LIGHT_ENDPOINT, matterLevel) == Status::Success)
    {
        MatterReportingAttributeChangeCallback(
            ConcreteAttributePath(LIGHT_ENDPOINT, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id));
    }

    OnOffServer::Instance().setOnOffValue(LIGHT_ENDPOINT, on ? 1U : 0U, false);
    MatterReportingAttributeChangeCallback(
        ConcreteAttributePath(LIGHT_ENDPOINT, OnOff::Id, OnOff::Attributes::OnOff::Id));

    PlatformMgr().UnlockChipStack();
}

} // namespace

void OvercurrentProtector::Init()
{
    sFaultActive       = false;
    sMatterOffPending  = false;
    sSnapValid         = false;
    sAvgMillivolts     = 0;
    sRecoveryMs        = 0;
    sSampleCount       = 0;
    sSampleIndex       = 0;
}

void OvercurrentProtector::SavePreFaultSnapshot()
{
    if (sSnapValid)
    {
        return;
    }

    bool on           = false;
    uint8_t level     = 254;
    uint16_t ctMireds = LightOutput::kDefaultCtMireds;

    // ISR already saved real PWM levels (e.g. level=254 while Matter CurrentLevel==1).
    if (LightOutput::GetPreFaultState(on, level, ctMireds))
    {
        sSnapOn       = on;
        sSnapLevel    = level;
        sSnapCtMireds = ctMireds;
        sSnapValid    = true;
        ChipLogProgress(AppServer, "Protection snapshot (driver): on=%u level=%u ct=%u", on, level, ctMireds);
        return;
    }

    using namespace chip::Protocols::InteractionModel;

    PlatformMgr().LockChipStack();

    OnOffServer::Instance().getOnOffValue(LIGHT_ENDPOINT, &on);

    DataModel::Nullable<uint8_t> currentLevel;
    if (LevelControl::Attributes::CurrentLevel::Get(LIGHT_ENDPOINT, currentLevel) == Status::Success && !currentLevel.IsNull())
    {
        level = currentLevel.Value();
    }
    level = LightOutput::ResolveLevelForPwmLocked(LIGHT_ENDPOINT, on, level);

    if (ColorControl::Attributes::ColorTemperatureMireds::Get(LIGHT_ENDPOINT, &ctMireds) != Status::Success)
    {
        ctMireds = LightOutput::kDefaultCtMireds;
    }

    PlatformMgr().UnlockChipStack();

    sSnapOn       = on;
    sSnapLevel    = level;
    sSnapCtMireds = ctMireds;
    sSnapValid    = true;

    ChipLogProgress(AppServer, "Protection snapshot (matter): on=%u level=%u ct=%u", on, level, ctMireds);
}

void OvercurrentProtector::UpdateAverage(uint32_t millivolts)
{
    if (sSampleCount < OVERCURRENT_AVG_SAMPLES)
    {
        sSampleRing[sSampleCount] = millivolts;
        ++sSampleCount;
    }
    else
    {
        sSampleRing[sSampleIndex] = millivolts;
        sSampleIndex              = static_cast<uint8_t>((sSampleIndex + 1U) % OVERCURRENT_AVG_SAMPLES);
    }

    uint32_t sum = 0;
    for (uint8_t i = 0; i < sSampleCount; ++i)
    {
        sum += sSampleRing[i];
    }
    sAvgMillivolts = sum / sSampleCount;
}

bool OvercurrentProtector::IsGpioOk()
{
    if (!ShortCircuitProtector::IsPinOk())
    {
        return false;
    }

#if defined(OVERCURRENT_GPIO_OK_PORT) && defined(OVERCURRENT_GPIO_OK_PIN)
    const unsigned int level = GPIO_PinInGet(GpioOkPort(), OVERCURRENT_GPIO_OK_PIN);
    return level == OVERCURRENT_GPIO_OK_LEVEL;
#else
    return VoltageAdcDriver::IsInitialized();
#endif
}

void OvercurrentProtector::TripFromIsr()
{
    if (sFaultActive)
    {
        return;
    }

    sFaultActive      = true;
    sRecoveryMs       = 0;
    sMatterOffPending = true;

    LightOutput::ForceOffForFaultFromIsr();
}

void OvercurrentProtector::ProcessDeferredTrip()
{
    if (!sMatterOffPending)
    {
        return;
    }

    sMatterOffPending = false;
    SavePreFaultSnapshot();
    SetMatterOnOff(false);
    ChipLogError(AppServer, "Short-circuit TRIP (PB00 falling edge)");
}

void OvercurrentProtector::Trip()
{
    if (sFaultActive)
    {
        return;
    }

    sFaultActive  = true;
    sRecoveryMs   = 0;

    LightOutput::ForceOffForFault();
    SavePreFaultSnapshot();

    SetMatterOnOff(false);

    ChipLogError(AppServer, "Overcurrent TRIP: avg=%lu mV > %u mV", static_cast<unsigned long>(sAvgMillivolts),
                 OVERCURRENT_TRIP_MV);
}

void OvercurrentProtector::Recover()
{
    if (!sFaultActive)
    {
        return;
    }

    sFaultActive      = false;
    sRecoveryMs       = 0;
    sMatterOffPending = false;

    ShortCircuitProtector::OnRecover();

    bool restoreOn     = false;
    uint8_t restoreLevel = 0;
    uint16_t restoreCt   = LightOutput::kDefaultCtMireds;

    if (sSnapValid)
    {
        restoreOn     = sSnapOn;
        restoreLevel  = sSnapLevel;
        restoreCt     = sSnapCtMireds;
        sSnapValid    = false;
    }
    else if (LightOutput::GetPreFaultState(restoreOn, restoreLevel, restoreCt))
    {
        // Fallback: driver snapshot from ISR trip before deferred Matter save.
    }
    else
    {
        ChipLogProgress(AppServer, "Protection RECOVER: no snapshot, stay off");
        return;
    }

    LightOutput::RestoreToPreFault(restoreOn, restoreLevel, restoreCt);
    RestoreMatterLightState(restoreOn, restoreLevel, restoreCt);

    ChipLogProgress(AppServer, "Protection RECOVER: on=%u level=%u ct=%u (avg=%lu mV)", restoreOn, restoreLevel, restoreCt,
                    static_cast<unsigned long>(sAvgMillivolts));
}

void OvercurrentProtector::OnAdcSample(uint32_t millivolts, bool sampleValid)
{
    ProcessDeferredTrip();

    if (!sampleValid)
    {
        sRecoveryMs = 0;
        return;
    }

    UpdateAverage(millivolts);

    if (sFaultActive)
    {
        if (sAvgMillivolts <= OVERCURRENT_CLEAR_MV && IsGpioOk())
        {
            sRecoveryMs += OVERCURRENT_SAMPLE_MS;
            if (sRecoveryMs >= OVERCURRENT_RECOVERY_MS)
            {
                Recover();
            }
        }
        else
        {
            sRecoveryMs = 0;
        }
        return;
    }

    if (sAvgMillivolts > OVERCURRENT_TRIP_MV)
    {
        Trip();
    }
}
