#pragma once

#include "BaseAppEvent.h"

struct AppEvent : public BaseAppEvent
{
    enum AppEventTypes
    {
        kEventType_Switch = BaseAppEvent::kEventType_Max + 1,
    };

    union
    {
        struct
        {
            uint8_t ButtonIndex;
            uint8_t Action;
            uint16_t EndpointId;
        } SwitchEvent;
    };
};
