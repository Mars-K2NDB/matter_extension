/*
 * Overcurrent protection: ADC average -> PWM off, Matter OnOff off, latch until cleared.
 */

#include "overcurrent_protector.h"

#include "adc_protect_config.h"
#include "AppConfig.h"
#include "light_output.h"
#include "overcurrent_protect_config.h"
#include "short_circuit_protector.h"
#include "voltage_adc_driver.h"

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
#include <system/SystemClock.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::DeviceLayer;

bool OvercurrentProtector::fault_active_       = false;
bool OvercurrentProtector::matter_off_pending_  = false;
bool OvercurrentProtector::snap_valid_         = false;
bool OvercurrentProtector::snap_on_            = false;
uint8_t OvercurrentProtector::snap_level_      = 0;
uint16_t OvercurrentProtector::snap_ct_mireds_    = light_output::kDefaultCtMireds;
uint32_t OvercurrentProtector::avg_millivolts_ = 0;
uint32_t OvercurrentProtector::recovery_ms_   = 0;
uint32_t OvercurrentProtector::sample_ring_[OVERCURRENT_AVG_SAMPLES] = {};
uint8_t OvercurrentProtector::sample_count_ = 0;
uint8_t OvercurrentProtector::sample_index_ = 0;

#if !DIMMABLE_LIGHT_ADC_PROTECT_ENABLE
bool OvercurrentProtector::poll_active_ = false;
#endif

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

void RestoreMatterLightState(bool on, uint8_t level, uint16_t /*ct_mireds*/)
{
    using namespace chip::Protocols::InteractionModel;

    PlatformMgr().LockChipStack();

    // Match CtDualPwm: Matter CurrentLevel==1 while on maps to effective PWM brightness level.
    const uint8_t matterLevel = on ? light_output::ResolveLevelForCluster(LIGHT_ENDPOINT, true, level) : level;

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
    fault_active_       = false;
    matter_off_pending_  = false;
    snap_valid_         = false;
    avg_millivolts_     = 0;
    recovery_ms_        = 0;
    sample_count_       = 0;
    sample_index_       = 0;

#if !DIMMABLE_LIGHT_ADC_PROTECT_ENABLE
    StartProtectionPoll();
#endif
}

void OvercurrentProtector::SavePreFaultSnapshot()
{
    if (snap_valid_)
    {
        return;
    }

    bool on           = false;
    uint8_t level     = 254;
    uint16_t ct_mireds = light_output::kDefaultCtMireds;

    // ISR already saved real PWM levels (e.g. level=254 while Matter CurrentLevel==1).
    if (light_output::GetPreFaultState(on, level, ct_mireds))
    {
        snap_on_       = on;
        snap_level_    = level;
        snap_ct_mireds_ = ct_mireds;
        snap_valid_    = true;
        ChipLogProgress(AppServer, "Protection snapshot (driver): on=%u level=%u ct=%u", on, level, ct_mireds);
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
    level = light_output::ResolveLevelForCluster(LIGHT_ENDPOINT, on, level);

    if (ColorControl::Attributes::ColorTemperatureMireds::Get(LIGHT_ENDPOINT, &ct_mireds) != Status::Success)
    {
        ct_mireds = light_output::kDefaultCtMireds;
    }

    PlatformMgr().UnlockChipStack();

    snap_on_       = on;
    snap_level_    = level;
    snap_ct_mireds_ = ct_mireds;
    snap_valid_    = true;

    ChipLogProgress(AppServer, "Protection snapshot (matter): on=%u level=%u ct=%u", on, level, ct_mireds);
}

void OvercurrentProtector::UpdateAverage(uint32_t millivolts)
{
    if (sample_count_ < OVERCURRENT_AVG_SAMPLES)
    {
        sample_ring_[sample_count_] = millivolts;
        ++sample_count_;
    }
    else
    {
        sample_ring_[sample_index_] = millivolts;
        sample_index_              = static_cast<uint8_t>((sample_index_ + 1U) % OVERCURRENT_AVG_SAMPLES);
    }

    uint32_t sum = 0;
    for (uint8_t i = 0; i < sample_count_; ++i)
    {
        sum += sample_ring_[i];
    }
    avg_millivolts_ = sum / sample_count_;
}

bool OvercurrentProtector::IsGpioOk()
{
    if (!ShortCircuitProtector::IsPinOk())
    {
        return false;
    }

#if DIMMABLE_LIGHT_ADC_PROTECT_ENABLE
#if defined(OVERCURRENT_GPIO_OK_PORT) && defined(OVERCURRENT_GPIO_OK_PIN)
    const unsigned int level = GPIO_PinInGet(GpioOkPort(), OVERCURRENT_GPIO_OK_PIN);
    return level == OVERCURRENT_GPIO_OK_LEVEL;
#else
    return VoltageAdcDriver::IsInitialized();
#endif
#else
    return true;
#endif
}

void OvercurrentProtector::TripFromIsr()
{
    if (fault_active_)
    {
        return;
    }

    fault_active_      = true;
    recovery_ms_       = 0;
    matter_off_pending_ = true;

    light_output::ForceOffForFaultFromIsr();
}

void OvercurrentProtector::ProcessDeferredTrip()
{
    if (!matter_off_pending_)
    {
        return;
    }

    matter_off_pending_ = false;
    SavePreFaultSnapshot();
    SetMatterOnOff(false);
    ChipLogError(AppServer, "Short-circuit TRIP (PB00 falling edge)");
}

void OvercurrentProtector::Trip()
{
    if (fault_active_)
    {
        return;
    }

    fault_active_  = true;
    recovery_ms_   = 0;

    light_output::ForceOffForFault();
    SavePreFaultSnapshot();

    SetMatterOnOff(false);

    ChipLogError(AppServer, "Overcurrent TRIP: avg=%lu mV > %u mV", static_cast<unsigned long>(avg_millivolts_),
                 OVERCURRENT_TRIP_MV);
}

void OvercurrentProtector::Recover()
{
    if (!fault_active_)
    {
        return;
    }

    fault_active_      = false;
    recovery_ms_       = 0;
    matter_off_pending_ = false;

    ShortCircuitProtector::OnRecover();

    bool restoreOn     = false;
    uint8_t restoreLevel = 0;
    uint16_t restoreCt   = light_output::kDefaultCtMireds;

    if (snap_valid_)
    {
        restoreOn     = snap_on_;
        restoreLevel  = snap_level_;
        restoreCt     = snap_ct_mireds_;
        snap_valid_    = false;
    }
    else if (light_output::GetPreFaultState(restoreOn, restoreLevel, restoreCt))
    {
        // Fallback: driver snapshot from ISR trip before deferred Matter save.
    }
    else
    {
        ChipLogProgress(AppServer, "Protection RECOVER: no snapshot, stay off");
        return;
    }

    light_output::RestoreToPreFault(restoreOn, restoreLevel, restoreCt);
    RestoreMatterLightState(restoreOn, restoreLevel, restoreCt);

    ChipLogProgress(AppServer, "Protection RECOVER: on=%u level=%u ct=%u (avg=%lu mV)", restoreOn, restoreLevel, restoreCt,
                    static_cast<unsigned long>(avg_millivolts_));
}

#if !DIMMABLE_LIGHT_ADC_PROTECT_ENABLE
void OvercurrentProtector::OnProtectionPollTimer(System::Layer* layer, void* app_state)
{
    (void) layer;
    (void) app_state;

    if (!poll_active_)
    {
        return;
    }

    OnProtectionPoll();

    if (poll_active_)
    {
        (void) DeviceLayer::SystemLayer().StartTimer(System::Clock::Milliseconds32(OVERCURRENT_SAMPLE_MS),
                                                    OnProtectionPollTimer, nullptr);
    }
}

void OvercurrentProtector::StartProtectionPoll()
{
    PlatformMgr().LockChipStack();
    poll_active_ = true;
    (void) DeviceLayer::SystemLayer().StartTimer(System::Clock::Milliseconds32(OVERCURRENT_SAMPLE_MS),
                                                 OnProtectionPollTimer, nullptr);
    PlatformMgr().UnlockChipStack();
}

void OvercurrentProtector::OnProtectionPoll()
{
    ProcessDeferredTrip();

    if (!fault_active_)
    {
        return;
    }

    if (ShortCircuitProtector::IsPinOk())
    {
        recovery_ms_ += OVERCURRENT_SAMPLE_MS;
        if (recovery_ms_ >= OVERCURRENT_RECOVERY_MS)
        {
            Recover();
        }
    }
    else
    {
        recovery_ms_ = 0;
    }
}
#endif

void OvercurrentProtector::OnAdcSample(uint32_t millivolts, bool sample_valid)
{
    ProcessDeferredTrip();

#if !DIMMABLE_LIGHT_ADC_PROTECT_ENABLE
    return;
#endif

    if (!sample_valid)
    {
        recovery_ms_ = 0;
        return;
    }

    UpdateAverage(millivolts);

    if (fault_active_)
    {
        if (avg_millivolts_ <= OVERCURRENT_CLEAR_MV && IsGpioOk())
        {
            recovery_ms_ += OVERCURRENT_SAMPLE_MS;
            if (recovery_ms_ >= OVERCURRENT_RECOVERY_MS)
            {
                Recover();
            }
        }
        else
        {
            recovery_ms_ = 0;
        }
        return;
    }

    if (avg_millivolts_ > OVERCURRENT_TRIP_MV)
    {
        Trip();
    }
}
