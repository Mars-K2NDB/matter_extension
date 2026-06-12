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

#include "sl_component_catalog.h"
#if defined(SL_CATALOG_CUSTOM_MAIN_PRESENT)
#include "sl_system_init.h"
#else
#include "sl_main_init.h"
#endif
#include "AppConfig.h"
#include "AppTask.h"
#include <MatterConfig.h>
#include <platform/silabs/tracing/SilabsTracingMacros.h>

#if SILABS_LOG_OUT_UART
#include "uart.h"
#endif

#if defined(OPENTHREAD_FTD) && OPENTHREAD_FTD
#include <openthread/instance.h>
#include <openthread/thread.h>
#include <openthread/thread_ftd.h>

extern "C" {
otInstance * otGetInstance(void);
}
#endif

using TimeTraceOperation = chip::Tracing::Silabs::TimeTraceOperation;

#if defined(OPENTHREAD_FTD) && OPENTHREAD_FTD
namespace {

// Flash 中若残留旧 ICD/MTD 固件保存的 link mode，Matter 在 SetRouterEligible 前会报 OT_ERROR_NOT_CAPABLE
void EnsureThreadFtdLinkModeBeforeMatterInit()
{
    otInstance * inst = otGetInstance();
    if (inst == nullptr)
    {
        return;
    }

    otLinkModeConfig mode = otThreadGetLinkMode(inst);
    if (mode.mDeviceType)
    {
        return;
    }

    mode.mDeviceType   = 1;
    mode.mRxOnWhenIdle = 1;
    if (otThreadSetLinkMode(inst, mode) != OT_ERROR_NONE)
    {
        return;
    }

    (void) otThreadSetRouterEligible(inst, true);
}

} // namespace
#endif

void app_init_early(void)
{
    SILABS_TRACE_BEGIN(chip::Tracing::Silabs::TimeTraceOperation::kBootup);
    SILABS_TRACE_BEGIN(chip::Tracing::Silabs::TimeTraceOperation::kSilabsInit);
}

void app_init(void)
{
    SILABS_TRACE_END(chip::Tracing::Silabs::TimeTraceOperation::kSilabsInit);
    SILABS_TRACE_BEGIN(chip::Tracing::Silabs::TimeTraceOperation::kMatterInit);

#if defined(OPENTHREAD_FTD) && OPENTHREAD_FTD
    EnsureThreadFtdLinkModeBeforeMatterInit();
#endif

    // 初始化 LED1，并修正板载 BTN0 GPIO（与 onoff_plug 一致）
    AppTask::GetAppTask().EarlyHardwareInit();

    SilabsMatterConfig::AppInit();

#if SILABS_LOG_OUT_UART
    uartForceTransmit("\r\n=== Generic Switch Boot ===\r\n", 31);
    SILABS_LOG("UART ready after AppInit");
#endif
}

#if defined(SL_CATALOG_CUSTOM_MAIN_PRESENT)
int main(void)
{
    app_init_early();
    sl_system_init();
    app_init();
}
#endif
