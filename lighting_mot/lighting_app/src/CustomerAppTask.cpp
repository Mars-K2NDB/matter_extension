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
#include "CtDualPwmDriver.h"

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

/** Push CT/RemainingTime to active subscriptions (commissioner apps wait for these reports). */
void NotifyColorTempAttributeReports(EndpointId endpoint)
{
    MatterReportingAttributeChangeCallback(
        ConcreteAttributePath(endpoint, ColorControl::Id, ColorControl::Attributes::ColorTemperatureMireds::Id));
    MatterReportingAttributeChangeCallback(
        ConcreteAttributePath(endpoint, ColorControl::Id, ColorControl::Attributes::RemainingTime::Id));
}

void CtPwmEventHandler(AppEvent * aEvent)
{
    const auto & ev = aEvent->CtPwmEvent;

    switch (ev.Kind)
    {
    case AppEvent::kCtPwmOn:
        CtDualPwmDriver::SetOn(ev.On);
        break;

    case AppEvent::kCtPwmLevel:
        CtDualPwmDriver::ApplyClusterLevel(LIGHT_ENDPOINT, ev.Level);
        break;

    case AppEvent::kCtPwmCt:
        CtDualPwmDriver::SetColorTemperatureMireds(ev.CtMireds);
        break;

    default:
        break;
    }
}

void PostCtPwmEvent(const AppEvent & eventTemplate)
{
    AppEvent event = eventTemplate;
    event.Handler  = CtPwmEventHandler;
    CustomerAppTask::GetAppTask().PostEvent(&event);
}

} // namespace

AppTask & AppTask::GetAppTask()
{
    return CustomerAppTask::GetAppTask();
}

CHIP_ERROR CustomerAppTask::InitLightImpl()
{
    ReturnErrorOnFailure(AppTask::InitLight());

    CtDualPwmDriver::Init();

    // Ensure Color Control cluster advertises CT and has a valid starting mired value.
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

    uint16_t ctMireds = 0;
    if (ColorControl::Attributes::ColorTemperatureMireds::Get(LIGHT_ENDPOINT, &ctMireds) != Status::Success ||
        ctMireds < CtDualPwmDriver::kCtMinMireds || ctMireds > CtDualPwmDriver::kCtMaxMireds)
    {
        ctMireds = CtDualPwmDriver::kDefaultCtMireds;
        ColorControl::Attributes::ColorTemperatureMireds::Set(LIGHT_ENDPOINT, ctMireds);
    }
    PlatformMgr().UnlockChipStack();

    CtDualPwmDriver::SyncFromMatterEndpoint(LIGHT_ENDPOINT);

    return CHIP_NO_ERROR;
}

void CustomerAppTask::LightActionEventHandlerImpl(AppEvent * aEvent)
{
    bool wasOn = false;
    PlatformMgr().LockChipStack();
    OnOffServer::Instance().getOnOffValue(LIGHT_ENDPOINT, &wasOn);
    PlatformMgr().UnlockChipStack();

    AppTask::LightActionEventHandler(aEvent);

    AppEvent event{};
    event.Type            = AppEvent::kEventType_CtPwm;
    event.CtPwmEvent.Kind = AppEvent::kCtPwmOn;
    event.CtPwmEvent.On   = !wasOn;
    PostCtPwmEvent(event);
}

void CustomerAppTask::LightTimerEventHandlerImpl(void * timerCbArg)
{
    AppTask::LightTimerEventHandler(timerCbArg);

    AppEvent event{};
    event.Type            = AppEvent::kEventType_CtPwm;
    event.CtPwmEvent.Kind = AppEvent::kCtPwmOn;
    event.CtPwmEvent.On   = false;
    PostCtPwmEvent(event);
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
            AppEvent event{};
            event.Type            = AppEvent::kEventType_CtPwm;
            event.CtPwmEvent.Kind = AppEvent::kCtPwmOn;
            event.CtPwmEvent.On   = (*value != 0);
            PostCtPwmEvent(event);
        }
        break; // fall through to AppTask for board LED

    case LevelControl::Id:
        if (attributeId == LevelControl::Attributes::CurrentLevel::Id && value != nullptr && size == sizeof(uint8_t))
        {
            AppEvent event{};
            event.Type             = AppEvent::kEventType_CtPwm;
            event.CtPwmEvent.Kind  = AppEvent::kCtPwmLevel;
            event.CtPwmEvent.Level = *value;
            PostCtPwmEvent(event);
        }
        break;

    case ColorControl::Id:
        if (attributeId == ColorControl::Attributes::ColorTemperatureMireds::Id && value != nullptr &&
            size == sizeof(uint16_t))
        {
            AppEvent event{};
            event.Type               = AppEvent::kEventType_CtPwm;
            event.CtPwmEvent.Kind    = AppEvent::kCtPwmCt;
            event.CtPwmEvent.CtMireds  = *reinterpret_cast<uint16_t *>(value);
            PostCtPwmEvent(event);
            // Attribute is already in the Matter store; push reports without waiting for PWM queue.
            NotifyColorTempAttributeReports(attributePath.mEndpointId);
            return;
        }
        if (attributeId == ColorControl::Attributes::ColorMode::Id ||
            attributeId == ColorControl::Attributes::EnhancedColorMode::Id)
        {
            // handleModeSwitch updates these before CT; apps often wait for mode reports.
            MatterReportingAttributeChangeCallback(attributePath);
            return;
        }
        // Skip AppTask RGB handler during other Color Control attributes.
        return;

    default:
        break;
    }

    AppTask::DMPostAttributeChangeCallback(attributePath, type, size, value);
}
