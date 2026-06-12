#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/AttributeAccessInterface.h>
#include <app/EventLogging.h>
#include <app/reporting/reporting.h>
#include <app/server/Server.h>
#include <app/util/attribute-storage.h>
#include <lib/core/ClusterEnums.h>
#include <system/SystemClock.h>

#include "AppConfig.h"
#include "AppTask.h"
#include "ElectricalEnergyMeasurementInstance.h"
#include "OnOffPlugManager.h"
#include <ElectricalPowerMeasurementDelegateImpl.h>
#include <silabs_utils.h>

#define mWms_TO_mWh(power) ((power) / 3600'000)

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ElectricalEnergyMeasurement;
using namespace chip::app::Clusters::ElectricalEnergyMeasurement::Attributes;
using namespace chip::app::Clusters::ElectricalEnergyMeasurement::Structs;
using namespace chip::app::DataModel;

static const MeasurementAccuracyRangeStruct::Type kEnergyAccuracyRanges[] = {
    { .rangeMin   = 0,
      .rangeMax   = 1'000'000'000'000'000,
      .percentMax = MakeOptional(static_cast<chip::Percent100ths>(500)),
      .percentMin = MakeOptional(static_cast<chip::Percent100ths>(50)) }
};

static const MeasurementAccuracyStruct::Type kAccuracy = {
    .measurementType  = MeasurementTypeEnum::kElectricalEnergy,
    .measured         = false,
    .minMeasuredValue = 0,
    .maxMeasuredValue = 1'000'000'000'000'000,
    .accuracyRanges = List<const ElectricalEnergyMeasurement::Structs::MeasurementAccuracyRangeStruct::Type>(kEnergyAccuracyRanges)
};

static const CumulativeEnergyResetStruct::Type kResetStruct = {
    .importedResetTimestamp = MakeOptional(MakeNullable(static_cast<uint32_t>(0))),
    .exportedResetTimestamp = MakeOptional(MakeNullable(static_cast<uint32_t>(0))),
    .importedResetSystime   = MakeOptional(MakeNullable(static_cast<uint64_t>(0))),
    .exportedResetSystime   = MakeOptional(MakeNullable(static_cast<uint64_t>(0))),
};

static EnergyMeasurementStruct::Type sCumulativeImported = {
    .energy = static_cast<int64_t>(0),
};

static const EnergyMeasurementStruct::Type sCumulativeExported = {
    .energy = static_cast<int64_t>(0),
};

namespace {

ElectricalPowerMeasurementDelegate * gEpmDelegate;
EndpointId gEndpointId;
int64_t sCumulativeActivePower = 0;
uint8_t sSecondsSinceUpdate    = 0;

} // namespace

CHIP_ERROR ElectricalEnergyMeasurementInstance::Init()
{
    TEMPORARY_RETURN_IGNORED SetMeasurementAccuracy(mEndpointId, kAccuracy);
    TEMPORARY_RETURN_IGNORED SetCumulativeReset(mEndpointId, MakeOptional(kResetStruct));

    gEpmDelegate = GetEPMDelegate();
    gEndpointId  = mEndpointId;

    uint32_t currentTimestamp;
    ReturnErrorOnFailure(System::Clock::GetClock_MatterEpochS(currentTimestamp));

    sCumulativeImported.startTimestamp.SetValue(currentTimestamp);
    sCumulativeImported.startSystime.SetValue(System::SystemClock().GetMonotonicTimestamp().count());

    ReturnErrorOnFailure(InitTimer());
    StartTimer(kTimerPeriodms);

    return ElectricalEnergyMeasurementAttrAccess::Init();
}

void ElectricalEnergyMeasurementInstance::Shutdown()
{
    CancelTimer();
    ElectricalEnergyMeasurementAttrAccess::Shutdown();
}

CHIP_ERROR ElectricalEnergyMeasurementInstance::InitTimer()
{
    mTimer = osTimerNew(TimerEventHandler, osTimerPeriodic, (void *) this, NULL);
    VerifyOrReturnError(mTimer != NULL, APP_ERROR_CREATE_TIMER_FAILED);
    return CHIP_NO_ERROR;
}

void ElectricalEnergyMeasurementInstance::StartTimer(uint32_t aTimeoutMs)
{
    if (osTimerStart(mTimer, pdMS_TO_TICKS(aTimeoutMs)) != osOK)
    {
        appError(APP_ERROR_START_TIMER_FAILED);
    }
}

void ElectricalEnergyMeasurementInstance::CancelTimer()
{
    if (osTimerStop(mTimer) == osError)
    {
        appError(APP_ERROR_STOP_TIMER_FAILED);
    }
}

void ElectricalEnergyMeasurementInstance::TimerEventHandler(void * timerCbArg)
{
    if (!PlugMgr().IsPlugOn())
    {
        sSecondsSinceUpdate = 0;
        return;
    }

    Nullable<int64_t> activePowerValue = gEpmDelegate ? gEpmDelegate->GetActivePower() : Nullable<int64_t>(0);
    int64_t activePower              = (activePowerValue.IsNull()) ? 0 : activePowerValue.Value();

    sCumulativeActivePower += activePower;
    sSecondsSinceUpdate += 1;

    if ((sSecondsSinceUpdate % ElectricalEnergyMeasurementInstance::kAttributeFrequency) == 0)
    {
        SILABS_LOG("EEM accumulate: P=%ldmW sum=%ld period=%us", (long) activePower, (long) sCumulativeActivePower,
                   sSecondsSinceUpdate);
    }

    if (sSecondsSinceUpdate >= ElectricalEnergyMeasurementInstance::kAttributeFrequency)
    {
        sSecondsSinceUpdate = 0;

        AppEvent event;
        event.Type    = AppEvent::kEventType_Timer;
        event.Handler = UpdateEnergyAttributesAndNotify;
        AppTask::GetAppTask().PostEvent(&event);
    }
}

void ElectricalEnergyMeasurementInstance::UpdateEnergyAttributesAndNotify(AppEvent * aEvent)
{
    sCumulativeImported.energy = mWms_TO_mWh(sCumulativeActivePower * kTimerPeriodms);

    uint32_t currentTimestamp;
    if (System::Clock::GetClock_MatterEpochS(currentTimestamp) == CHIP_NO_ERROR)
    {
        sCumulativeImported.endTimestamp.SetValue(currentTimestamp);
    }

    sCumulativeImported.endSystime.SetValue(System::SystemClock().GetMonotonicTimestamp().count());

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    bool notified = NotifyCumulativeEnergyMeasured(gEndpointId, MakeOptional(sCumulativeImported), MakeOptional(sCumulativeExported));
    MatterReportingAttributeChangeCallback(gEndpointId, ElectricalEnergyMeasurement::Id, CumulativeEnergyImported::Id);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

    SILABS_LOG("EEM ep=%u report: energy=%ldmWh notified=%d", gEndpointId, (long) sCumulativeImported.energy, notified ? 1 : 0);
}
