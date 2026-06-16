/*
 * Provisioning reminder: cool-white 500 ms on/off blink at 100% brightness.
 * Runs at most 60 s on boot when unprovisioned; light-switch button stops it.
 */

#include "provision_reminder.h"

#include "AppConfig.h"
#include "light_output.h"

#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/silabs/platformAbstraction/SilabsPlatform.h>
#include <system/SystemClock.h>

using namespace chip;
using namespace chip::DeviceLayer;

namespace provision_reminder {
namespace {

constexpr uint32_t kBlinkPeriodMs = 500;
constexpr uint32_t kMaxDurationMs = 60000;

bool handler_registered_ = false;
bool active_               = false;
bool blink_on_             = false;

void OnBlinkTimer(System::Layer* layer, void* context);
void OnMaxDurationTimer(System::Layer* layer, void* context);
void DeferredStartWork(intptr_t context);

bool IsDeviceProvisioned()
{
    bool provisioned = false;
    PlatformMgr().LockChipStack();
#ifdef SL_WIFI
    provisioned = ConnectivityMgr().IsWiFiStationProvisioned();
#else
    provisioned = ConnectivityMgr().IsThreadProvisioned();
#endif
    PlatformMgr().UnlockChipStack();
    return provisioned;
}

void CancelTimers()
{
    PlatformMgr().LockChipStack();
    SystemLayer().CancelTimer(OnBlinkTimer, nullptr);
    SystemLayer().CancelTimer(OnMaxDurationTimer, nullptr);
    PlatformMgr().UnlockChipStack();
}

void RestoreMatterLightOutput()
{
    light_output::SyncFromMatterEndpoint(LIGHT_ENDPOINT);
    light_output::SetOn(false);
}

void StopActive(bool restore_matter_output)
{
    if (!active_)
    {
        return;
    }

    active_   = false;
    blink_on_ = false;
    CancelTimers();
    light_output::ApplyProvisionReminderOutput(false);

    if (restore_matter_output)
    {
        RestoreMatterLightOutput();
    }

    SILABS_LOG("Provision reminder: stopped");
    ChipLogProgress(DeviceLayer, "Provision reminder: stopped");
}

void ScheduleBlinkTimer()
{
    PlatformMgr().LockChipStack();
    const CHIP_ERROR err =
        SystemLayer().StartTimer(System::Clock::Milliseconds32(kBlinkPeriodMs), OnBlinkTimer, nullptr);
    PlatformMgr().UnlockChipStack();
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(DeviceLayer, "Provision reminder: blink timer failed: %" CHIP_ERROR_FORMAT, err.Format());
        StopActive(true);
    }
}

void ScheduleMaxDurationTimer()
{
    PlatformMgr().LockChipStack();
    const CHIP_ERROR err =
        SystemLayer().StartTimer(System::Clock::Milliseconds32(kMaxDurationMs), OnMaxDurationTimer, nullptr);
    PlatformMgr().UnlockChipStack();
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(DeviceLayer, "Provision reminder: max-duration timer failed: %" CHIP_ERROR_FORMAT, err.Format());
        StopActive(true);
    }
}

void StartIfNeeded()
{
    if (active_ || IsDeviceProvisioned())
    {
        return;
    }

    active_   = true;
    blink_on_ = true;
    light_output::ApplyProvisionReminderOutput(true);
    ScheduleBlinkTimer();
    ScheduleMaxDurationTimer();

    SILABS_LOG("Provision reminder: cool-white blink %ums (max %ums)", kBlinkPeriodMs, kMaxDurationMs);
    ChipLogProgress(DeviceLayer, "Provision reminder: cool-white blink started (%ums/%ums max)", kBlinkPeriodMs,
                    kMaxDurationMs);
}

void OnBlinkTimer(System::Layer* /*layer*/, void* /*context*/)
{
    if (!active_)
    {
        return;
    }

    blink_on_ = !blink_on_;
    light_output::ApplyProvisionReminderOutput(blink_on_);
    ScheduleBlinkTimer();
}

void OnMaxDurationTimer(System::Layer* /*layer*/, void* /*context*/)
{
    StopActive(true);
}

void ConnectivityEventHandler(const ChipDeviceEvent* event, intptr_t /*arg*/)
{
    if (!active_)
    {
        return;
    }

    switch (event->Type)
    {
    case DeviceEventType::kCommissioningComplete:
        StopActive(true);
        break;

    case DeviceEventType::kThreadStateChange:
    case DeviceEventType::kThreadConnectivityChange:
        if (IsDeviceProvisioned())
        {
            StopActive(true);
        }
        break;

    default:
        break;
    }
}

void DeferredStartWork(intptr_t /*context*/)
{
    StartIfNeeded();
}

} // namespace

void Init()
{
    if (handler_registered_)
    {
        return;
    }

    TEMPORARY_RETURN_IGNORED PlatformMgr().AddEventHandler(ConnectivityEventHandler, 0);
    handler_registered_ = true;
}

void TryStartOnBoot()
{
    Init();
    TEMPORARY_RETURN_IGNORED PlatformMgr().ScheduleWork(DeferredStartWork, 0);
}

void OnLightSwitchPressed()
{
    StopActive(false);
}

bool IsActive()
{
    return active_;
}

} // namespace provision_reminder
