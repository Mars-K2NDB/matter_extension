#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "AppEvent.h"
#include "BaseApplication.h"

#include <platform/CHIPDeviceLayer.h>

class AppTask : public BaseApplication
{
public:
    AppTask() = default;

    static AppTask & GetAppTask() { return sAppTask; }

    static void AppTaskMain(void * pvParameter);

    CHIP_ERROR StartAppTask();

    static void EarlyHardwareInit();

    static void ButtonEventHandler(uint8_t button, uint8_t btnAction);

private:
    static AppTask sAppTask;

    CHIP_ERROR AppInit() override;

    static void SwitchActionEventHandler(AppEvent * aEvent);
};
