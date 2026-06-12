#pragma once

#include "hlw8032_driver.h"
#include <lib/core/CHIPError.h>

class ElectricalSensorManager
{
    using OperationalStateEnum = chip::app::Clusters::OperationalState::OperationalStateEnum;

public:
    static ElectricalSensorManager & Instance();

    CHIP_ERROR Init();
    void Shutdown();
    void UpdateEPMAttributes(OperationalStateEnum state);
    void ScheduleUpdateEPMAttributes(OperationalStateEnum state);
    void OnPlugStateChanged(bool isOn);

    bool GetLatestEnergyData(hlw8032_energy_data_t * data) const;

private:
    static ElectricalSensorManager sElectricalSensor;

    static void Hlw8032ReadingTask(void * arg);

    mutable hlw8032_energy_data_t mEnergyData;
    float mLastValidPowerW;
    bool mHlw8032Ready;
    bool mTaskRunning;
    void * mTaskHandle;
};
