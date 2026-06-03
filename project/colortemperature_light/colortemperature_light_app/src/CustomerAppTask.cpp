/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    Copyright (c) 2019 Google LLC.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
 * @file
 * @brief Customer-facing AppTask definition site.
 *
 * Maps Matter OnOff / Level / ColorTemperature to dual PWM outputs (PWM0=cool, PWM1=warm).
 */

#include "CustomerAppTask.h"

#include "AppConfig.h"
#include "light_output.h"
#include "device_user_flash.h"
#include "overcurrent_protector.h"
#include "short_circuit_protector.h"
#include "voltage_adc_driver.h"

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

bool outputs_ready_ = false;


/** Push CT/RemainingTime to active subscriptions (commissioner apps wait for these reports). */
void NotifyColorTempAttributeReports(EndpointId endpoint)
{
    MatterReportingAttributeChangeCallback(
        ConcreteAttributePath(endpoint, ColorControl::Id, ColorControl::Attributes::ColorTemperatureMireds::Id));
    MatterReportingAttributeChangeCallback(
        ConcreteAttributePath(endpoint, ColorControl::Id, ColorControl::Attributes::RemainingTime::Id));
}

void LightStripEventHandler(AppEvent* event)
{
    const auto& ev = event->light_strip_event;

    switch (ev.kind)
    {
    case AppEvent::kOnOff:
        if (ev.on && OvercurrentProtector::BlocksTurnOn())
        {
            ChipLogProgress(AppServer, "CtPwm: on event ignored (overcurrent fault)");
            break;
        }
        light_output::SetOn(ev.on);
        break;

    case AppEvent::kLevel:
        light_output::ApplyClusterLevel(LIGHT_ENDPOINT, ev.level);
        break;

    case AppEvent::kColorTemp:
        light_output::SetColorTemperatureMireds(ev.ct_mireds);
        break;

    default:
        break;
    }
}

void Postlight_strip_event(const AppEvent& eventTemplate)
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

    // Register reconnect reporting before stack may already be up.
    device_user_flash::Init();

    device_user_flash::LoadSavedLightState();

    ReturnErrorOnFailure(AppTask::InitLight());

    light_output::Init();
    VoltageAdcDriver::Init();
    OvercurrentProtector::Init();
    ShortCircuitProtector::Init();
    VoltageAdcDriver::StartPeriodicSampling();

    PlatformMgr().LockChipStack();
    using namespace chip::app::Clusters;
    using namespace chip::Protocols::InteractionModel;

    BitMask<ColorControl::ColorCapabilitiesBitmap> caps;
    if (ColorControl::Attributes::ColorCapabilities::Get(LIGHT_ENDPOINT, &caps) == Status::Success &&
        !caps.Has(ColorControl::ColorCapabilitiesBitmap::kColorTemperature))
    {
        caps.Set(ColorControl::ColorCapabilitiesBitmap::kColorTemperature);
        ColorControl::Attributes::ColorCapabilities::Set(LIGHT_ENDPOINT, caps);
    }

    device_user_flash::ApplyCachedLightStateToMatter(LIGHT_ENDPOINT);

    if (!device_user_flash::HasPersistedLightState())
    {
        uint16_t ct_mireds = 0;
        if (ColorControl::Attributes::ColorTemperatureMireds::Get(LIGHT_ENDPOINT, &ct_mireds) != Status::Success ||
            ct_mireds < light_output::kCtMinMireds || ct_mireds > light_output::kCtMaxMireds)
        {
            ColorControl::Attributes::ColorTemperatureMireds::Set(LIGHT_ENDPOINT, light_output::kDefaultCtMireds);
        }
    }
    PlatformMgr().UnlockChipStack();

    // Load restored level/CT into PWM driver while lamp stays off.
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
        Postlight_strip_event(event);
    }
}

void CustomerAppTask::LightTimerEventHandlerImpl(void* timer_cb_arg)
{
    AppTask::LightTimerEventHandler(timer_cb_arg);

    AppEvent event{};
    event.Type            = AppEvent::kEventType_LightStrip;
    event.light_strip_event.kind = AppEvent::kOnOff;
    event.light_strip_event.on   = false;
    Postlight_strip_event(event);
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
                ChipLogProgress(AppServer, "OnOff on rejected (overcurrent fault)");
                return;
            }

            AppEvent event{};
            event.Type            = AppEvent::kEventType_LightStrip;
            event.light_strip_event.kind = AppEvent::kOnOff;
            event.light_strip_event.on   = (*value != 0);
            Postlight_strip_event(event);
        }
        break; // fall through to AppTask for board LED

    case LevelControl::Id:
        if (attribute_id == LevelControl::Attributes::CurrentLevel::Id && value != nullptr && size == sizeof(uint8_t))
        {
            AppEvent event{};
            event.Type             = AppEvent::kEventType_LightStrip;
            event.light_strip_event.kind  = AppEvent::kLevel;
            event.light_strip_event.level = *value;
            Postlight_strip_event(event);
            device_user_flash::UpdateLightStateFromAttributeChange(attribute_path.mEndpointId, cluster_id, attribute_id);
        }
        break; // level/CT only; OnOff is not persisted

    case ColorControl::Id:
        if (attribute_id == ColorControl::Attributes::ColorTemperatureMireds::Id && value != nullptr &&
            size == sizeof(uint16_t))
        {
            AppEvent event{};
            event.Type               = AppEvent::kEventType_LightStrip;
            event.light_strip_event.kind    = AppEvent::kColorTemp;
            event.light_strip_event.ct_mireds  = *reinterpret_cast<uint16_t *>(value);
            Postlight_strip_event(event);
            // Attribute is already in the Matter store; push reports without waiting for PWM queue.
            NotifyColorTempAttributeReports(attribute_path.mEndpointId);
            device_user_flash::UpdateLightStateFromAttributeChange(attribute_path.mEndpointId, cluster_id, attribute_id);
            return;
        }
        if (attribute_id == ColorControl::Attributes::ColorMode::Id ||
            attribute_id == ColorControl::Attributes::EnhancedColorMode::Id)
        {
            // handleModeSwitch updates these before CT; apps often wait for mode reports.
            MatterReportingAttributeChangeCallback(attribute_path);
            device_user_flash::UpdateLightStateFromAttributeChange(attribute_path.mEndpointId, cluster_id, attribute_id);
            return;
        }
        // Skip AppTask RGB handler during other Color Control attributes.
        return;

    default:
        break;
    }

    AppTask::DMPostAttributeChangeCallback(attribute_path, type, size, value);
}
