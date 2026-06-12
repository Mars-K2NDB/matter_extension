#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/ConcreteAttributePath.h>
#include <app/server/Server.h>
#include <app/util/attribute-storage.h>
#include <lib/core/ClusterEnums.h>

#include "AppConfig.h"
#include "AppTask.h"
#include "DataModelHelper.h"
#include "ElectricalEnergyMeasurementInstance.h"
#include "ElectricalSensorManager.h"
#include <ElectricalPowerMeasurementDelegateImpl.h>
#include <PowerTopologyDelegateImpl.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ElectricalEnergyMeasurement;
using namespace chip::app::Clusters::ElectricalEnergyMeasurement::Structs;
using namespace chip::app::Clusters::ElectricalPowerMeasurement;
using namespace chip::app::Clusters::OperationalState;
using namespace chip::app::Clusters::PowerTopology;
using namespace chip::app::DataModel;

ElectricalSensorManager ElectricalSensorManager::sElectricalSensor;

ElectricalSensorManager & ElectricalSensorManager::Instance()
{
    return sElectricalSensor;
}

static std::unique_ptr<ElectricalEnergyMeasurementInstance> gEemInstance;
static std::unique_ptr<ElectricalPowerMeasurementDelegate> gEpmDelegate;
static std::unique_ptr<ElectricalPowerMeasurementInstance> gEpmInstance;
static std::unique_ptr<PowerTopologyDelegate> gPtDelegate;
static std::unique_ptr<PowerTopologyInstance> gPtInstance;

static const struct
{
    PowerModeEnum powerMode;
    int64_t voltageMv;
    int64_t activeCurrentMa;
    int64_t activePowerMw;
} kPlugMeterAttributes[2] = {
    { PowerModeEnum::kAc, 220'000, 0, 0 },
    { PowerModeEnum::kAc, 220'000, 4'545, 1'000'000 },
};

CHIP_ERROR ElectricalSensorManager::Init()
{
    EndpointId epmEndpointId = DataModelHelper::GetEndpointIdFromCluster(ElectricalPowerMeasurement::Id);
    EndpointId eemEndpointId = DataModelHelper::GetEndpointIdFromCluster(ElectricalEnergyMeasurement::Id);
    EndpointId ptEndpointId  = DataModelHelper::GetEndpointIdFromCluster(PowerTopology::Id);
    CHIP_ERROR err;

    VerifyOrReturnError((epmEndpointId != kInvalidEndpointId) && (eemEndpointId != kInvalidEndpointId) &&
                            (ptEndpointId != kInvalidEndpointId),
                        CHIP_ERROR_INCORRECT_STATE);

    VerifyOrReturnError(!gEpmDelegate && !gEpmInstance, CHIP_ERROR_INCORRECT_STATE);

    gEpmDelegate = std::make_unique<ElectricalPowerMeasurementDelegate>();
    VerifyOrReturnError(gEpmDelegate, CHIP_ERROR_NO_MEMORY);

    gEpmInstance = std::make_unique<ElectricalPowerMeasurementInstance>(
        epmEndpointId, *gEpmDelegate,
        BitMask<ElectricalPowerMeasurement::Feature, uint32_t>(ElectricalPowerMeasurement::Feature::kAlternatingCurrent),
        BitMask<ElectricalPowerMeasurement::OptionalAttributes, uint32_t>(
            ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeVoltage,
            ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeActiveCurrent));
    VerifyOrReturnError(gEpmInstance, CHIP_ERROR_NO_MEMORY);

    ReturnErrorOnFailure(gEpmInstance->Init());
    UpdateEPMAttributes(OperationalStateEnum::kStopped);

    VerifyOrReturnError(!gEemInstance, CHIP_ERROR_INCORRECT_STATE);

    gEemInstance = std::make_unique<ElectricalEnergyMeasurementInstance>(
        eemEndpointId, *gEpmDelegate,
        BitMask<ElectricalEnergyMeasurement::Feature, uint32_t>(ElectricalEnergyMeasurement::Feature::kImportedEnergy,
                                                                ElectricalEnergyMeasurement::Feature::kCumulativeEnergy),
        BitMask<ElectricalEnergyMeasurement::OptionalAttributes, uint32_t>());
    VerifyOrReturnError(gEemInstance, CHIP_ERROR_NO_MEMORY);

    ReturnErrorOnFailure(gEemInstance->Init());

    VerifyOrReturnError(!gPtDelegate && !gPtInstance, CHIP_ERROR_INCORRECT_STATE);

    gPtDelegate = std::make_unique<PowerTopologyDelegate>();
    VerifyOrReturnError(gPtDelegate, CHIP_ERROR_NO_MEMORY);

    gPtInstance = std::make_unique<PowerTopologyInstance>(
        ptEndpointId, *gPtDelegate, BitMask<PowerTopology::Feature, uint32_t>(PowerTopology::Feature::kNodeTopology));
    VerifyOrReturnError(gPtInstance, CHIP_ERROR_NO_MEMORY);

    err = gPtInstance->Init();
    VerifyOrReturnError(CHIP_NO_ERROR == err, err);

    return CHIP_NO_ERROR;
}

void ElectricalSensorManager::Shutdown()
{
    if (gPtInstance)
    {
        gPtInstance->Shutdown();
        gPtInstance.reset();
    }
    gPtDelegate.reset();

    if (gEemInstance)
    {
        gEemInstance->Shutdown();
        gEemInstance.reset();
    }

    if (gEpmInstance)
    {
        gEpmInstance->Shutdown();
        gEpmInstance.reset();
    }
    gEpmDelegate.reset();
}

void ElectricalSensorManager::UpdateEPMAttributes(OperationalStateEnum state)
{
    if (!gEpmDelegate)
    {
        return;
    }

    uint8_t index = (state == OperationalStateEnum::kRunning) ? 1 : 0;
    TEMPORARY_RETURN_IGNORED gEpmDelegate->SetPowerMode(kPlugMeterAttributes[index].powerMode);
    TEMPORARY_RETURN_IGNORED gEpmDelegate->SetVoltage(MakeNullable(kPlugMeterAttributes[index].voltageMv));
    TEMPORARY_RETURN_IGNORED gEpmDelegate->SetActiveCurrent(MakeNullable(kPlugMeterAttributes[index].activeCurrentMa));
    TEMPORARY_RETURN_IGNORED gEpmDelegate->SetActivePower(MakeNullable(kPlugMeterAttributes[index].activePowerMw));
}
