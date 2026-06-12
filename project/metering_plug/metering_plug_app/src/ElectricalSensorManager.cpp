#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/ConcreteAttributePath.h>
#include <app/server/Server.h>
#include <app/util/attribute-storage.h>
#include <lib/core/ClusterEnums.h>

#include "AppConfig.h"
#include "AppTask.h"
#include "DataModelHelper.h"
#include "OnOffPlugManager.h"
#include "ElectricalEnergyMeasurementInstance.h"
#include "ElectricalSensorManager.h"
#include "hlw8032_config.h"
#include <ElectricalPowerMeasurementDelegateImpl.h>
#include <PowerTopologyDelegateImpl.h>

#include <app/reporting/reporting.h>
#include <cmsis_os2.h>
#include <platform/CHIPDeviceLayer.h>
#include <silabs_utils.h>

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
static EndpointId gEpmEndpointId = kInvalidEndpointId;
static EndpointId gEemEndpointId = kInvalidEndpointId;

namespace {

constexpr uint32_t kHlw8032TaskStackSize = 2048;
constexpr osPriority_t kHlw8032TaskPriority = osPriorityBelowNormal;

static const osThreadAttr_t kHlw8032TaskAttr = {
    .name       = "HLW8032",
    .stack_size = kHlw8032TaskStackSize,
    .priority   = kHlw8032TaskPriority,
};

static void DoUpdateEpmAttributesWork(intptr_t context)
{
    auto state = static_cast<OperationalStateEnum>(context);
    ElectricalSensorManager::Instance().UpdateEPMAttributes(state);
}

#if CONFIG_HLW8032_ENERGY_READING_TEST_MODE
static void GenerateTestEnergyData(uint32_t counter, hlw8032_energy_data_t * data)
{
    if (data == NULL)
    {
        return;
    }

    /* 电压 200~250V，电流 2~10A，功率约 400~2400W */
    data->voltage      = 200.0f + (float) (counter % 51);
    data->current      = 2.0f + (float) (counter % 81) * 0.1f;
    data->power_factor = 0.70f + (float) (counter % 31) * 0.01f;
    data->active_power = data->voltage * data->current * data->power_factor;
    data->kwh          = 0.0f;
    data->timestamp    = 0;
    data->valid        = true;
}
#endif

} // namespace

bool ElectricalSensorManager::GetLatestEnergyData(hlw8032_energy_data_t * data) const
{
    if (data == NULL)
    {
        return false;
    }
    *data = mEnergyData;
    return mEnergyData.valid;
}

void ElectricalSensorManager::Hlw8032ReadingTask(void * arg)
{
    ElectricalSensorManager * mgr = static_cast<ElectricalSensorManager *>(arg);
#if CONFIG_HLW8032_ENERGY_READING_TEST_MODE
    uint32_t test_counter = 0;
#endif

    while (mgr->mTaskRunning)
    {
#if CONFIG_HLW8032_ENERGY_READING_TEST_MODE
        osDelay(pdMS_TO_TICKS(HLW8032_ENERGY_POLL_MS));
        test_counter++;
        hlw8032_energy_data_t data = {};
        GenerateTestEnergyData(test_counter, &data);
        mgr->mEnergyData      = data;
        mgr->mLastValidPowerW = data.active_power;
        SILABS_LOG("HLW8032 test: V=%d.%dV I=%d.%dmA P=%d.%dW",
                   (int) data.voltage, (int) (data.voltage * 10) % 10, (int) (data.current * 1000),
                   (int) ((data.current * 1000) * 10) % 10, (int) data.active_power,
                   (int) (data.active_power * 10) % 10);
#else
        osDelay(pdMS_TO_TICKS(HLW8032_ENERGY_POLL_MS));
        hlw8032_energy_data_t data = {};
        if (hlw8032_read_energy_data(&g_hlw8032, &data))
        {
            mgr->mEnergyData      = data;
            mgr->mLastValidPowerW = data.active_power;
            SILABS_LOG("HLW8032: V=%d.%dV I=%d.%dmA P=%d.%dW",
                       (int) data.voltage, (int) (data.voltage * 10) % 10, (int) (data.current * 1000),
                       (int) ((data.current * 1000) * 10) % 10, (int) data.active_power,
                       (int) (data.active_power * 10) % 10);
        }
#endif
        if (PlugMgr().IsPlugOn())
        {
            mgr->ScheduleUpdateEPMAttributes(OperationalStateEnum::kRunning);
        }
    }
}

CHIP_ERROR ElectricalSensorManager::Init()
{
    EndpointId epmEndpointId = DataModelHelper::GetEndpointIdFromCluster(ElectricalPowerMeasurement::Id);
    EndpointId eemEndpointId = DataModelHelper::GetEndpointIdFromCluster(ElectricalEnergyMeasurement::Id);
    EndpointId ptEndpointId  = DataModelHelper::GetEndpointIdFromCluster(PowerTopology::Id);
    CHIP_ERROR err;

    mEnergyData       = {};
    mLastValidPowerW  = 0.0f;
    mHlw8032Ready     = false;
    mTaskRunning      = false;
    mTaskHandle       = NULL;

#if !CONFIG_HLW8032_ENERGY_READING_TEST_MODE
    if (hlw8032_init(&g_hlw8032))
    {
        mHlw8032Ready = true;
    }
    else
    {
        SILABS_LOG("HLW8032: init failed, using zero readings");
    }
#else
    mHlw8032Ready = true;
    GenerateTestEnergyData(1, &mEnergyData);
    mLastValidPowerW = mEnergyData.active_power;
    SILABS_LOG("HLW8032: test mode enabled");
#endif

    VerifyOrReturnError((epmEndpointId != kInvalidEndpointId) && (eemEndpointId != kInvalidEndpointId) &&
                            (ptEndpointId != kInvalidEndpointId) && (epmEndpointId == eemEndpointId) &&
                            (epmEndpointId == ptEndpointId),
                        CHIP_ERROR_INCORRECT_STATE);

    gEpmEndpointId = epmEndpointId;
    gEemEndpointId = eemEndpointId;
    SILABS_LOG("Metering endpoints: EPM=%u EEM=%u PT=%u plug=%s", epmEndpointId, eemEndpointId, ptEndpointId,
               PlugMgr().IsPlugOn() ? "ON" : "OFF");

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

    if (mHlw8032Ready)
    {
        mTaskRunning = true;
        mTaskHandle  = osThreadNew(Hlw8032ReadingTask, this, &kHlw8032TaskAttr);
        VerifyOrReturnError(mTaskHandle != NULL, APP_ERROR_CREATE_TASK_FAILED);
        SILABS_LOG("HLW8032: energy reading task started");
    }

    return CHIP_NO_ERROR;
}

void ElectricalSensorManager::Shutdown()
{
    mTaskRunning = false;
    if (mTaskHandle != NULL)
    {
        osThreadTerminate(static_cast<osThreadId_t>(mTaskHandle));
        mTaskHandle = NULL;
    }

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

void ElectricalSensorManager::ScheduleUpdateEPMAttributes(OperationalStateEnum state)
{
    chip::DeviceLayer::PlatformMgr().ScheduleWork(DoUpdateEpmAttributesWork, static_cast<intptr_t>(state));
}

void ElectricalSensorManager::OnPlugStateChanged(bool isOn)
{
    if (gEemInstance)
    {
        gEemInstance->SetMeteringActive(isOn);
    }

    ScheduleUpdateEPMAttributes(isOn ? OperationalStateEnum::kRunning : OperationalStateEnum::kStopped);
}

void ElectricalSensorManager::UpdateEPMAttributes(OperationalStateEnum state)
{
    if (!gEpmDelegate)
    {
        return;
    }

    if (state != OperationalStateEnum::kRunning)
    {
        SILABS_LOG("EPM ep=%u: plug off, skip metering update/report", gEpmEndpointId);
        return;
    }

    PowerModeEnum powerMode = PowerModeEnum::kAc;
    int64_t voltageMv       = 0;
    int64_t activeCurrentMa = 0;
    int64_t activePowerMw   = 0;

    hlw8032_energy_data_t data = {};
    if (GetLatestEnergyData(&data) && data.valid)
    {
        voltageMv       = (int64_t) (data.voltage * 1000.0f);
        activeCurrentMa = (int64_t) (data.current * 1000.0f);
        activePowerMw   = (int64_t) (data.active_power * 1000.0f);
    }
    else if (mLastValidPowerW > 0.0f)
    {
        voltageMv       = 220'000;
        activeCurrentMa = (int64_t) ((mLastValidPowerW / 220.0f) * 1000.0f);
        activePowerMw   = (int64_t) (mLastValidPowerW * 1000.0f);
    }
#if CONFIG_HLW8032_ENERGY_READING_TEST_MODE
    else
    {
        voltageMv       = 220'000;
        activeCurrentMa = (int64_t) ((HLW8032_TEST_DEFAULT_POWER_W / 220.0f) * 1000.0f);
        activePowerMw   = (int64_t) (HLW8032_TEST_DEFAULT_POWER_W * 1000.0f);
    }
#endif

    TEMPORARY_RETURN_IGNORED gEpmDelegate->SetPowerMode(powerMode);
    TEMPORARY_RETURN_IGNORED gEpmDelegate->SetVoltage(MakeNullable(voltageMv));
    TEMPORARY_RETURN_IGNORED gEpmDelegate->SetActiveCurrent(MakeNullable(activeCurrentMa));
    TEMPORARY_RETURN_IGNORED gEpmDelegate->SetActivePower(MakeNullable(activePowerMw));

    SILABS_LOG("EPM ep=%u: V=%ldmV I=%ldmA P=%ldmW", gEpmEndpointId, static_cast<long>(voltageMv),
               static_cast<long>(activeCurrentMa), static_cast<long>(activePowerMw));

    if (gEpmEndpointId != kInvalidEndpointId)
    {
        MatterReportingAttributeChangeCallback(gEpmEndpointId, ElectricalPowerMeasurement::Id,
                                               ElectricalPowerMeasurement::Attributes::Voltage::Id);
        MatterReportingAttributeChangeCallback(gEpmEndpointId, ElectricalPowerMeasurement::Id,
                                               ElectricalPowerMeasurement::Attributes::ActiveCurrent::Id);
        MatterReportingAttributeChangeCallback(gEpmEndpointId, ElectricalPowerMeasurement::Id,
                                               ElectricalPowerMeasurement::Attributes::ActivePower::Id);
    }
}
