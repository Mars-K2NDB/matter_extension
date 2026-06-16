/*
 * Restore FTD link mode before Matter InitOpenThread().
 */

#include "thread_ftd_boot_fix.h"

#if defined(OPENTHREAD_FTD) && OPENTHREAD_FTD

#include <openthread/instance.h>
#include <openthread/thread.h>
#include <openthread/thread_ftd.h>

extern "C" {
otInstance * otGetInstance(void);
}

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

#else

void EnsureThreadFtdLinkModeBeforeMatterInit() {}

#endif
