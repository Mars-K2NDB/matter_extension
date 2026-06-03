/*
 * Matter Thread WS2814 SPI 幻彩灯带。
 */

#include "CustomerAppTask.h"

#include "AppConfig.h"
#include "device_user_flash.h"
#include "light_output.h"
#include "rgbcw_strip_driver.h"

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

bool hue_sat_apply_scheduled_ = false;
EndpointId hue_sat_apply_endpoint_ = 0;

bool xy_apply_scheduled_ = false;
EndpointId xy_apply_endpoint_ = 0;

/*
 * Matter updateHueSatCommand 先写 CurrentHue 再写 CurrentSaturation。
 * Hue 的 attribute 回调在 Saturation 写入之前同步执行，此时读到的 sat 是旧值，
 * 会先发一帧错误颜色（例如 Hue=0 时 R≈253 G≈203 B≈202）。延迟到本轮 ZCL 写完后再读。
 */
void HueSatApplyWork(intptr_t /*arg*/)
{
    hue_sat_apply_scheduled_ = false;
    if (!outputs_ready_)
    {
        return;
    }

    uint8_t hue = 0;
    uint8_t sat = 0;
    PlatformMgr().LockChipStack();
    ColorControl::Attributes::CurrentHue::Get(hue_sat_apply_endpoint_, &hue);
    ColorControl::Attributes::CurrentSaturation::Get(hue_sat_apply_endpoint_, &sat);
    PlatformMgr().UnlockChipStack();

    light_output::SetHueSat(hue, sat);
}

void ScheduleHueSatApply(EndpointId endpoint)
{
    hue_sat_apply_endpoint_ = endpoint;
    if (hue_sat_apply_scheduled_)
    {
        return;
    }
    hue_sat_apply_scheduled_ = true;
    (void) PlatformMgr().ScheduleWork(HueSatApplyWork, 0);
}

void XyApplyWork(intptr_t /*arg*/)
{
    xy_apply_scheduled_ = false;
    if (!outputs_ready_)
    {
        return;
    }

    uint16_t currentX = 0;
    uint16_t currentY = 0;
    PlatformMgr().LockChipStack();
    ColorControl::Attributes::CurrentX::Get(xy_apply_endpoint_, &currentX);
    ColorControl::Attributes::CurrentY::Get(xy_apply_endpoint_, &currentY);
    PlatformMgr().UnlockChipStack();

    light_output::SetXy(currentX, currentY);
}

void ScheduleXyApply(EndpointId endpoint)
{
    xy_apply_endpoint_ = endpoint;
    if (xy_apply_scheduled_)
    {
        return;
    }
    xy_apply_scheduled_ = true;
    (void) PlatformMgr().ScheduleWork(XyApplyWork, 0);
}

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

void PostLightStripEvent(const AppEvent& event_template)
{
    if (!outputs_ready_)
    {
        return;
    }
    AppEvent event = event_template;
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
    const bool scheduleFactoryReset = device_user_flash::ProcessPowerCycleReset();

    device_user_flash::Init();
    device_user_flash::LoadSavedLightState();

    ReturnErrorOnFailure(AppTask::InitLight());

    light_output::Init();

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

    light_output::SyncFromMatterEndpoint(LIGHT_ENDPOINT);
    light_output::SetOn(false);

    device_user_flash::EnablePersistedLightStateSave();
    outputs_ready_ = true;

    if (scheduleFactoryReset)
    {
        ChipLogProgress(DeviceLayer, "Factory reset scheduled; light outputs remain usable until reset");
    }

    return CHIP_NO_ERROR;
}

void CustomerAppTask::LightActionEventHandlerImpl(AppEvent* event)
{
    bool was_on = false;
    PlatformMgr().LockChipStack();
    OnOffServer::Instance().getOnOffValue(LIGHT_ENDPOINT, &was_on);
    PlatformMgr().UnlockChipStack();

    AppTask::LightActionEventHandler(event);

    AppEvent strip_event{};
    strip_event.Type                 = AppEvent::kEventType_LightStrip;
    strip_event.light_strip_event.kind = AppEvent::kOnOff;
    strip_event.light_strip_event.on   = !was_on;
    PostLightStripEvent(strip_event);
}

void CustomerAppTask::LightTimerEventHandlerImpl(void* timer_cb_arg)
{
    AppTask::LightTimerEventHandler(timer_cb_arg);

    AppEvent strip_event{};
    strip_event.Type                 = AppEvent::kEventType_LightStrip;
    strip_event.light_strip_event.kind = AppEvent::kOnOff;
    strip_event.light_strip_event.on   = false;
    PostLightStripEvent(strip_event);
}

void CustomerAppTask::DMPostAttributeChangeCallbackImpl(const chip::app::ConcreteAttributePath& attribute_path,
                                                        uint8_t type, uint16_t size, uint8_t* value)
{
    const ClusterId cluster_id     = attribute_path.mClusterId;
    const AttributeId attribute_id = attribute_path.mAttributeId;

    switch (cluster_id)
    {
    case OnOff::Id:
        if (attribute_id == OnOff::Attributes::OnOff::Id && value != nullptr && size == sizeof(uint8_t))
        {
            AppEvent strip_event{};
            strip_event.Type                 = AppEvent::kEventType_LightStrip;
            strip_event.light_strip_event.kind = AppEvent::kOnOff;
            strip_event.light_strip_event.on   = (*value != 0);
            PostLightStripEvent(strip_event);
        }
        break;

    case LevelControl::Id:
        if (attribute_id == LevelControl::Attributes::CurrentLevel::Id && value != nullptr && size == sizeof(uint8_t))
        {
            AppEvent strip_event{};
            strip_event.Type                  = AppEvent::kEventType_LightStrip;
            strip_event.light_strip_event.kind  = AppEvent::kLevel;
            strip_event.light_strip_event.level = *value;
            PostLightStripEvent(strip_event);
            device_user_flash::UpdateLightStateFromAttributeChange(attribute_path.mEndpointId, cluster_id,
                                                                   attribute_id);
        }
        break;

    case ColorControl::Id:
        if (attribute_id == ColorControl::Attributes::ColorTemperatureMireds::Id && value != nullptr &&
            size == sizeof(uint16_t))
        {
            AppEvent strip_event{};
            strip_event.Type                    = AppEvent::kEventType_LightStrip;
            strip_event.light_strip_event.kind    = AppEvent::kColorTemp;
            strip_event.light_strip_event.ct_mireds = *reinterpret_cast<uint16_t*>(value);
            PostLightStripEvent(strip_event);
            NotifyColorTempAttributeReports(attribute_path.mEndpointId);
            device_user_flash::UpdateLightStateFromAttributeChange(attribute_path.mEndpointId, cluster_id,
                                                                   attribute_id);
            return;
        }
        if ((attribute_id == ColorControl::Attributes::CurrentX::Id ||
             attribute_id == ColorControl::Attributes::CurrentY::Id) &&
            value != nullptr && size == sizeof(uint16_t))
        {
            ScheduleXyApply(attribute_path.mEndpointId);
            device_user_flash::UpdateLightStateFromAttributeChange(attribute_path.mEndpointId, cluster_id,
                                                                   attribute_id);
            return;
        }
        if ((attribute_id == ColorControl::Attributes::CurrentHue::Id ||
             attribute_id == ColorControl::Attributes::CurrentSaturation::Id) &&
            value != nullptr)
        {
            ScheduleHueSatApply(attribute_path.mEndpointId);
            device_user_flash::UpdateLightStateFromAttributeChange(attribute_path.mEndpointId, cluster_id,
                                                                   attribute_id);
            return;
        }
        if (attribute_id == ColorControl::Attributes::ColorMode::Id ||
            attribute_id == ColorControl::Attributes::EnhancedColorMode::Id)
        {
            MatterReportingAttributeChangeCallback(attribute_path);
            device_user_flash::UpdateLightStateFromAttributeChange(attribute_path.mEndpointId, cluster_id,
                                                                   attribute_id);
            light_output::SyncFromMatterEndpoint(attribute_path.mEndpointId);
            return;
        }
        return;

    default:
        break;
    }

    AppTask::DMPostAttributeChangeCallback(attribute_path, type, size, value);
}
