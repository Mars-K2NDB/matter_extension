#pragma once

#include <lib/core/CHIPError.h>

class ElectricalSensorManager
{
    using OperationalStateEnum = chip::app::Clusters::OperationalState::OperationalStateEnum;

public:
    static ElectricalSensorManager & Instance();

    CHIP_ERROR Init();
    void Shutdown();
    void UpdateEPMAttributes(OperationalStateEnum state);

private:
    static ElectricalSensorManager sElectricalSensor;
};
