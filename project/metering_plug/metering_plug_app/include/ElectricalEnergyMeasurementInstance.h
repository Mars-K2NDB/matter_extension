#pragma once

#include "AppConfig.h"
#include "AppEvent.h"
#include "hlw8032_config.h"
#include <ElectricalPowerMeasurementDelegateImpl.h>
#include <app/clusters/electrical-energy-measurement-server/ElectricalEnergyMeasurementCluster.h>
#include <app/clusters/electrical-energy-measurement-server/electrical-energy-measurement-server.h>
#include <cmsis_os2.h>

namespace chip {
namespace app {
namespace Clusters {
namespace ElectricalEnergyMeasurement {

using ElectricalPowerMeasurement::ElectricalPowerMeasurementDelegate;

class ElectricalEnergyMeasurementInstance : public ElectricalEnergyMeasurementAttrAccess
{
public:
    static constexpr uint32_t kTimerPeriodms      = 1000;
    static constexpr uint32_t kAttributeFrequency = HLW8032_CUMULATIVE_REPORT_INTERVAL_SECONDS;

    ElectricalEnergyMeasurementInstance(EndpointId aEndpointId,
                                        ElectricalPowerMeasurementDelegate & aEpmDelegate,
                                        BitMask<Feature> aFeature, BitMask<OptionalAttributes> aOptionalAttrs) :
        ElectricalEnergyMeasurementAttrAccess::ElectricalEnergyMeasurementAttrAccess(aFeature, aOptionalAttrs, aEndpointId),
        mEndpointId(aEndpointId)
    {
        mEpmDelegate = &aEpmDelegate;
    }

    ElectricalEnergyMeasurementInstance(const ElectricalEnergyMeasurementInstance &)             = delete;
    ElectricalEnergyMeasurementInstance(const ElectricalEnergyMeasurementInstance &&)            = delete;
    ElectricalEnergyMeasurementInstance & operator=(const ElectricalEnergyMeasurementInstance &) = delete;

    CHIP_ERROR Init();
    void Shutdown();

    ElectricalPowerMeasurementDelegate * GetEPMDelegate() { return mEpmDelegate; }

    void StartTimer(uint32_t aTimeoutMs);
    void CancelTimer();
    void SetMeteringActive(bool active);

private:
    ElectricalPowerMeasurementDelegate * mEpmDelegate;
    EndpointId mEndpointId;
    osTimerId_t mTimer;

    CHIP_ERROR InitTimer();

    static void TimerEventHandler(void * timerCbArg);
    static void UpdateEnergyAttributesAndNotify(AppEvent * aEvent);
};

} // namespace ElectricalEnergyMeasurement
} // namespace Clusters
} // namespace app
} // namespace chip
