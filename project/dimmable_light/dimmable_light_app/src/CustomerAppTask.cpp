/*
 * Matter Thread 单路灯：OnOff + Level -> PB4 PWM (SinglePwmDriver).
 */

#include "CustomerAppTask.h"

#include "adc_protect_config.h"
#include "AppConfig.h"
#include "device_user_flash.h"
#include "light_output.h"
#include "overcurrent_protector.h"
#include "short_circuit_protector.h"
#include "single_pwm_driver.h"
#include "voltage_adc_driver.h"

#include <app/clusters/on-off-server/on-off-server.h>
#include <app/reporting/reporting.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <clusters/LevelControl/AttributeIds.h>
#include <clusters/OnOff/AttributeIds.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::DeviceLayer;

CustomerAppTask CustomerAppTask::sAppTask;

namespace {

bool outputs_ready_ = false;

void LightStripEventHandler(AppEvent* event)
{
    const auto& ev = event->light_strip_event;

    switch (ev.kind)
    {
    case AppEvent::kOnOff:
        if (ev.on && OvercurrentProtector::BlocksTurnOn())
        {
            ChipLogProgress(AppServer, "SinglePwm: on ignored (overcurrent fault)");
            break;
        }
        light_output::SetOn(ev.on);
        break;

    case AppEvent::kLevel:
        light_output::ApplyClusterLevel(LIGHT_ENDPOINT, ev.level);
        break;

    default:
        break;
    }
}

void PostLightStripEvent(const AppEvent& eventTemplate)
{
    if (!outputs_ready_)
    {
        return;
    }
    AppEvent event = eventTemplate;
    event.Handler  = LightStripEventHandler;
    CustomerAppTask::GetAppTask().PostEvent(&event);
}

} // namespace

AppTask & AppTask::GetAppTask()
{
    return CustomerAppTask::GetAppTask();
}

CHIP_ERROR CustomerAppTask::InitLightImpl()
{
    if (device_user_flash::ProcessPowerCycleReset())
    {
        return CHIP_NO_ERROR;
    }

    device_user_flash::Init();
    device_user_flash::LoadSavedLightState();

    ReturnErrorOnFailure(AppTask::InitLight());

    light_output::Init();
    OvercurrentProtector::Init();
    ShortCircuitProtector::Init();
#if DIMMABLE_LIGHT_ADC_PROTECT_ENABLE
    VoltageAdcDriver::Init();
    VoltageAdcDriver::StartPeriodicSampling();
#endif

    PlatformMgr().LockChipStack();
    device_user_flash::ApplyCachedLightStateToMatter(LIGHT_ENDPOINT);
    PlatformMgr().UnlockChipStack();

    light_output::SyncFromMatterEndpoint(LIGHT_ENDPOINT);
    light_output::SetOn(false);

    device_user_flash::EnablePersistedLightStateSave();
    outputs_ready_ = true;

    return CHIP_NO_ERROR;
}

void CustomerAppTask::LightActionEventHandlerImpl(AppEvent* event)
{
    bool was_on = false;
    PlatformMgr().LockChipStack();
    OnOffServer::Instance().getOnOffValue(LIGHT_ENDPOINT, &was_on);
    PlatformMgr().UnlockChipStack();

    AppTask::LightActionEventHandler(event);

    if (!OvercurrentProtector::IsFaultActive())
    {
        AppEvent event{};
        event.Type            = AppEvent::kEventType_LightStrip;
        event.light_strip_event.kind = AppEvent::kOnOff;
        event.light_strip_event.on   = !was_on;
        PostLightStripEvent(event);
    }
}

void CustomerAppTask::LightTimerEventHandlerImpl(void* timer_cb_arg)
{
    AppTask::LightTimerEventHandler(timer_cb_arg);

    AppEvent event{};
    event.Type            = AppEvent::kEventType_LightStrip;
    event.light_strip_event.kind = AppEvent::kOnOff;
    event.light_strip_event.on   = false;
    PostLightStripEvent(event);
}

void CustomerAppTask::DMPostAttributeChangeCallbackImpl(const chip::app::ConcreteAttributePath& attribute_path, uint8_t type,
                                                        uint16_t size, uint8_t* value)
{
    const ClusterId cluster_id     = attribute_path.mClusterId;
    const AttributeId attribute_id = attribute_path.mAttributeId;

    switch (cluster_id)
    {
    case OnOff::Id:
        if (attribute_id == OnOff::Attributes::OnOff::Id && value != nullptr && size == sizeof(uint8_t))
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
            event.Type            = AppEvent::kEventType_LightStrip;
            event.light_strip_event.kind = AppEvent::kOnOff;
            event.light_strip_event.on   = (*value != 0);
            PostLightStripEvent(event);
        }
        break;

    case LevelControl::Id:
        if (attribute_id == LevelControl::Attributes::CurrentLevel::Id && value != nullptr && size == sizeof(uint8_t))
        {
            AppEvent event{};
            event.Type             = AppEvent::kEventType_LightStrip;
            event.light_strip_event.kind  = AppEvent::kLevel;
            event.light_strip_event.level = *value;
            PostLightStripEvent(event);
            device_user_flash::UpdateLightStateFromAttributeChange(attribute_path.mEndpointId, cluster_id, attribute_id);
        }
        break;

    default:
        break;
    }

    AppTask::DMPostAttributeChangeCallback(attribute_path, type, size, value);
}
