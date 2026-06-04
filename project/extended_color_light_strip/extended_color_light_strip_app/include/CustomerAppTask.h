/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    Copyright (c) 2019 Google LLC.
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

#include "AppTaskImpl.h"
#include "AppEvent.h"
#include <app/ConcreteAttributePath.h>
#include <lib/core/CHIPError.h>
#include <cstdint>

/**
 * @brief Minimal AppTaskImpl-derived class.
 *
 * Any *Impl() method you do not override keeps the default AppTask behavior.
 * Override *Impl() methods here to customize individual behaviors.
 *
 * See the app README ("Override API Reference") for the full list of
 * overridable methods.
 */
class CustomerAppTask : public AppTaskImpl<CustomerAppTask>
{
public:
    static CustomerAppTask & GetAppTask() { return sAppTask; }

private:
    friend class AppTaskImpl<CustomerAppTask>;

    CHIP_ERROR InitLightImpl();
    void ButtonEventHandlerImpl(uint8_t button, uint8_t btnAction);
    void LightActionEventHandlerImpl(AppEvent* event);
    void LightTimerEventHandlerImpl(void* timer_cb_arg);
    void DMPostAttributeChangeCallbackImpl(const chip::app::ConcreteAttributePath& attribute_path, uint8_t type,
                                           uint16_t size, uint8_t* value);

    static CustomerAppTask sAppTask;
};
