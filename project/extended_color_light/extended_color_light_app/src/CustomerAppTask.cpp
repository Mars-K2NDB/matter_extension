/*
 * Matter Thread 五路 RGBCW PWM 灯。
 */

#include "CustomerAppTask.h"

#include "AppConfig.h"
#include "DeviceUserFlash.h"
#include "LightOutput.h"
#include "OvercurrentProtector.h"
#include "RgbcwPwmDriver.h"
#include "ShortCircuitProtector.h"
#include "VoltageAdcDriver.h"

#include <app/clusters/on-off-server/on-off-server.h>
#include <app/reporting/reporting.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <clusters/ColorControl/AttributeIds.h>
#include <clusters/ColorControl/Enums.h>
#include <clusters/LevelControl/AttributeIds.h>
#include <clusters/OnOff/AttributeIds.h>
#include <lib/support/BitMask.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>
#include <protocols/interaction_model/StatusCode.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::DeviceLayer;

CustomerAppTask CustomerAppTask::sAppTask;

namespace {

bool sOutputsReady = false;

void NotifyColorTempAttributeReports(EndpointId endpoint)
{
    MatterReportingAttributeChangeCallback(
        ConcreteAttributePath(endpoint, ColorControl::Id, ColorControl::Attributes::ColorTemperatureMireds::Id));
    MatterReportingAttributeChangeCallback(
        ConcreteAttributePath(endpoint, ColorControl::Id, ColorControl::Attributes::RemainingTime::Id));
}

void PwmEventHandler(AppEvent * aEvent)
{
    const auto & ev = aEvent->CtPwmEvent;

    switch (ev.Kind)
    {
    case AppEvent::kCtPwmOn:
        if (ev.On && OvercurrentProtector::BlocksTurnOn())
        {
            break;
        }
        LightOutput::SetOn(ev.On);
        break;

    case AppEvent::kCtPwmLevel:
        LightOutput::ApplyClusterLevel(LIGHT_ENDPOINT, ev.Level);
        break;

    case AppEvent::kCtPwmCt:
        LightOutput::SetColorTemperatureMireds(ev.CtMireds);
        break;

    default:
        break;
    }
}

void PostPwmEvent(const AppEvent & eventTemplate)
{
    if (!sOutputsReady)
    {
        return;
    }
    AppEvent event = eventTemplate;
    event.Handler  = PwmEventHandler;
    CustomerAppTask::GetAppTask().PostEvent(&event);
}

} // namespace

AppTask & AppTask::GetAppTask()
{
    return CustomerAppTask::GetAppTask();
}

CHIP_ERROR CustomerAppTask::InitLightImpl()
{
    if (DeviceUserFlash::ProcessPowerCycleReset())
    {
        return CHIP_NO_ERROR;
    }

    DeviceUserFlash::Init();
    DeviceUserFlash::LoadSavedLightState();

    ReturnErrorOnFailure(AppTask::InitLight());

    LightOutput::Init();
    VoltageAdcDriver::Init();
    OvercurrentProtector::Init();
    ShortCircuitProtector::Init();
    VoltageAdcDriver::StartPeriodicSampling();

    PlatformMgr().LockChipStack();
    using namespace chip::Protocols::InteractionModel;

    BitMask<ColorControl::ColorCapabilitiesBitmap> caps;
    if (ColorControl::Attributes::ColorCapabilities::Get(LIGHT_ENDPOINT, &caps) == Status::Success)
    {
        caps.Set(ColorControl::ColorCapabilitiesBitmap::kHueSaturation);
        caps.Set(ColorControl::ColorCapabilitiesBitmap::kColorTemperature);
        caps.Set(ColorControl::ColorCapabilitiesBitmap::kXy);
        ColorControl::Attributes::ColorCapabilities::Set(LIGHT_ENDPOINT, caps);
    }

    DeviceUserFlash::ApplyCachedLightStateToMatter(LIGHT_ENDPOINT);

    if (!DeviceUserFlash::HasPersistedLightState())
    {
        uint16_t ctMireds = 0;
        if (ColorControl::Attributes::ColorTemperatureMireds::Get(LIGHT_ENDPOINT, &ctMireds) != Status::Success ||
            ctMireds < LightOutput::kCtMinMireds || ctMireds > LightOutput::kCtMaxMireds)
        {
            ColorControl::Attributes::ColorTemperatureMireds::Set(LIGHT_ENDPOINT, LightOutput::kDefaultCtMireds);
        }
    }
    PlatformMgr().UnlockChipStack();

    LightOutput::SyncFromMatterEndpoint(LIGHT_ENDPOINT);
    LightOutput::SetOn(false);

    DeviceUserFlash::EnablePersistedLightStateSave();
    sOutputsReady = true;

    return CHIP_NO_ERROR;
}

void CustomerAppTask::LightActionEventHandlerImpl(AppEvent * aEvent)
{
    bool wasOn = false;
    PlatformMgr().LockChipStack();
    OnOffServer::Instance().getOnOffValue(LIGHT_ENDPOINT, &wasOn);
    PlatformMgr().UnlockChipStack();

    AppTask::LightActionEventHandler(aEvent);

    if (!OvercurrentProtector::IsFaultActive())
    {
        AppEvent event{};
        event.Type            = AppEvent::kEventType_CtPwm;
        event.CtPwmEvent.Kind = AppEvent::kCtPwmOn;
        event.CtPwmEvent.On   = !wasOn;
        PostPwmEvent(event);
    }
}

void CustomerAppTask::LightTimerEventHandlerImpl(void * timerCbArg)
{
    AppTask::LightTimerEventHandler(timerCbArg);

    AppEvent event{};
    event.Type            = AppEvent::kEventType_CtPwm;
    event.CtPwmEvent.Kind = AppEvent::kCtPwmOn;
    event.CtPwmEvent.On   = false;
    PostPwmEvent(event);
}

void CustomerAppTask::DMPostAttributeChangeCallbackImpl(const chip::app::ConcreteAttributePath & attributePath, uint8_t type,
                                                        uint16_t size, uint8_t * value)
{
    const ClusterId clusterId     = attributePath.mClusterId;
    const AttributeId attributeId = attributePath.mAttributeId;

    switch (clusterId)
    {
    case OnOff::Id:
        if (attributeId == OnOff::Attributes::OnOff::Id && value != nullptr && size == sizeof(uint8_t))
        {
            if (OvercurrentProtector::IsFaultActive() && *value != 0)
            {
                PlatformMgr().LockChipStack();
                OnOffServer::Instance().setOnOffValue(LIGHT_ENDPOINT, 0, false);
                MatterReportingAttributeChangeCallback(
                    ConcreteAttributePath(LIGHT_ENDPOINT, OnOff::Id, OnOff::Attributes::OnOff::Id));
                PlatformMgr().UnlockChipStack();
                return;
            }

            AppEvent event{};
            event.Type            = AppEvent::kEventType_CtPwm;
            event.CtPwmEvent.Kind = AppEvent::kCtPwmOn;
            event.CtPwmEvent.On   = (*value != 0);
            PostPwmEvent(event);
        }
        break;

    case LevelControl::Id:
        if (attributeId == LevelControl::Attributes::CurrentLevel::Id && value != nullptr && size == sizeof(uint8_t))
        {
            AppEvent event{};
            event.Type             = AppEvent::kEventType_CtPwm;
            event.CtPwmEvent.Kind  = AppEvent::kCtPwmLevel;
            event.CtPwmEvent.Level = *value;
            PostPwmEvent(event);
            DeviceUserFlash::UpdateLightStateFromAttributeChange(attributePath.mEndpointId, clusterId, attributeId);
        }
        break;

    case ColorControl::Id:
        if (attributeId == ColorControl::Attributes::ColorTemperatureMireds::Id && value != nullptr &&
            size == sizeof(uint16_t))
        {
            AppEvent event{};
            event.Type              = AppEvent::kEventType_CtPwm;
            event.CtPwmEvent.Kind   = AppEvent::kCtPwmCt;
            event.CtPwmEvent.CtMireds = *reinterpret_cast<uint16_t *>(value);
            PostPwmEvent(event);
            NotifyColorTempAttributeReports(attributePath.mEndpointId);
            DeviceUserFlash::UpdateLightStateFromAttributeChange(attributePath.mEndpointId, clusterId, attributeId);
            return;
        }
        if ((attributeId == ColorControl::Attributes::CurrentHue::Id ||
             attributeId == ColorControl::Attributes::CurrentSaturation::Id) &&
            value != nullptr)
        {
            uint8_t hue = 0;
            uint8_t sat = 0;
            PlatformMgr().LockChipStack();
            ColorControl::Attributes::CurrentHue::Get(attributePath.mEndpointId, &hue);
            ColorControl::Attributes::CurrentSaturation::Get(attributePath.mEndpointId, &sat);
            PlatformMgr().UnlockChipStack();
            if (attributeId == ColorControl::Attributes::CurrentHue::Id && size == sizeof(uint8_t))
            {
                hue = *value;
            }
            if (attributeId == ColorControl::Attributes::CurrentSaturation::Id && size == sizeof(uint8_t))
            {
                sat = *value;
            }
            LightOutput::SetHueSat(hue, sat);
            DeviceUserFlash::UpdateLightStateFromAttributeChange(attributePath.mEndpointId, clusterId, attributeId);
            return;
        }
        if (attributeId == ColorControl::Attributes::ColorMode::Id ||
            attributeId == ColorControl::Attributes::EnhancedColorMode::Id)
        {
            MatterReportingAttributeChangeCallback(attributePath);
            DeviceUserFlash::UpdateLightStateFromAttributeChange(attributePath.mEndpointId, clusterId, attributeId);
            LightOutput::SyncFromMatterEndpoint(attributePath.mEndpointId);
            return;
        }
        return;

    default:
        break;
    }

    AppTask::DMPostAttributeChangeCallback(attributePath, type, size, value);
}
