/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <pthread.h>

#include "hilog_wrapper.h"
#include "power/running_lock_hub.h"
#include "power/suspend_ops.h"

static const char* WAKEUP_HOLDER = "OHOSPowerMgr.WakeupHolder";

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static struct AutoSuspendOps* g_suspendOps = NULL;
static BOOL g_suspendEnabled = FALSE;

void ScAcquireRunningLock(const char *name)
{
    RunningLockHubLock(name);
}

void ScReleaseRunningLock(const char *name)
{
    RunningLockHubUnlock(name);
}

void EnableSuspend()
{
    if (!g_suspendOps) {
        return;
    }

    pthread_mutex_lock(&g_mutex);
    if (g_suspendEnabled == FALSE) {
        ScReleaseRunningLock(WAKEUP_HOLDER);
        g_suspendEnabled = TRUE;
    }
    pthread_mutex_unlock(&g_mutex);
    g_suspendOps->Enable();
}

void DisableSuspend()
{
    pthread_mutex_lock(&g_mutex);
    if (g_suspendEnabled == TRUE) {
        ScAcquireRunningLock(WAKEUP_HOLDER);
        g_suspendEnabled = FALSE;
    }
    pthread_mutex_unlock(&g_mutex);
}

void SuspendControllerInit()
{
    g_suspendOps = AutoSuspendOpsInit();
    if (!g_suspendOps) {
        POWER_HILOGE("Failed to init auto suspend ops");
        return;
    }
    if (RunningLockHubInit(g_suspendOps) == FALSE) {
        POWER_HILOGE("Failed to init running lock hub");
        return;
    }
    ScAcquireRunningLock(WAKEUP_HOLDER);
}
