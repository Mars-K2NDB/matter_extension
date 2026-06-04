/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    Copyright (c) 2018 Nest Labs, Inc.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once
#if (defined(SL_MATTER_RGB_LED_ENABLED) && SL_MATTER_RGB_LED_ENABLED == 1)
#include "RGBLEDWidget.h"
#endif //(defined(SL_MATTER_RGB_LED_ENABLED) && SL_MATTER_RGB_LED_ENABLED == 1)

#include "BaseAppEvent.h"

struct AppEvent : public BaseAppEvent
{
    enum AppEventTypes
    {
        kEventType_Light      = BaseAppEvent::kEventType_Max + 1,
        kEventType_LightStrip = BaseAppEvent::kEventType_Max + 2,
    };

    enum LightStripEventKind : uint8_t
    {
        kOnOff            = 0,
        kLevel            = 1,
        kColorTemp        = 2,
        kStripEffectSwitch = 3,
    };

    union
    {
        struct
        {
            uint8_t Action;
            int32_t Actor;
            uint8_t Value;
        } LightEvent;

        struct
        {
            uint8_t kind;
            bool on;
            uint8_t level;
            uint16_t ct_mireds;
        } light_strip_event;

#if (defined(SL_MATTER_RGB_LED_ENABLED) && SL_MATTER_RGB_LED_ENABLED == 1)
        struct
        {
            uint8_t Action;
            int32_t Actor;
            RGBLEDWidget::ColorData_t Value;
        } LightControlEvent;
#endif // (defined(SL_MATTER_RGB_LED_ENABLED) && SL_MATTER_RGB_LED_ENABLED == 1)
    };
};
