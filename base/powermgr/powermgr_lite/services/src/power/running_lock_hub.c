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

#include "hilog_wrapper.h"
#include "power/suspend_ops.h"

static struct AutoSuspendOps* g_suspendOps = NULL;
static struct RunningLockOps* g_runningLockOps = NULL;

void RunningLockHubLock(const char* name)
{
    if (!g_runningLockOps || !g_suspendOps) {
        POWER_HILOGE("Invalid running lock or suspend ops");
        return;
    }
    g_runningLockOps->Acquire(name);
    /*
     * Because the upper layer call can ensure the acquirment of locks, it is simple to implement here.
     * It's better to maintain a running lock map by name.
     */
    g_suspendOps->IncSuspendBlockCounter();
}

void RunningLockHubUnlock(const char* name)
{
    if (!g_runningLockOps || !g_suspendOps) {
        POWER_HILOGE("Invalid running lock or suspend ops");
        return;
    }
    g_runningLockOps->Release(name);
    g_suspendOps->DecSuspendBlockCounter();
}

BOOL RunningLockHubInit(struct AutoSuspendOps* suspendOps)
{
    g_runningLockOps = RunningLockOpsInit();
    if (!g_runningLockOps) {
        POWER_HILOGE("Failed to init runninglock ops");
        return FALSE;
    }
    g_suspendOps = suspendOps;
    return TRUE;
}
