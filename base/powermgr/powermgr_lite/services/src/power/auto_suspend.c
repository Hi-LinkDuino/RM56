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

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include "hilog_wrapper.h"
#include "power/suspend_ops.h"

#define SUSPEND_CHECK_INTERVAL_US       500000

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;
static pthread_t g_suspendThread;
static AutoSuspendLoop g_suspendLoop = NULL;
static uint32_t g_suspendBlockCounter = 0;

int usleep(useconds_t usec);
static BOOL SuspendConditionSatisfied()
{
    return (g_suspendBlockCounter == 0) ? TRUE : FALSE;
}

static void WaitingSuspendCondition()
{
    pthread_mutex_lock(&g_mutex);
    while (SuspendConditionSatisfied() == FALSE) {
        pthread_cond_wait(&g_cond, &g_mutex);
    }
    pthread_mutex_unlock(&g_mutex);
}

static void *SuspendThreadLoop(void *arg)
{
    if (!g_suspendLoop) {
        return NULL;
    }

    POWER_HILOGI("Suspend thread enter loop");
    while (1) {
        //usleep(SUSPEND_CHECK_INTERVAL_US);
        
        if (g_suspendLoop(WaitingSuspendCondition) == FALSE) {
            break;
        }
    }
    return NULL;
}

static void Enable()
{
    static BOOL started = FALSE;
    if (started == TRUE) {
        return;
    }

    int32_t ret = pthread_create(&g_suspendThread, NULL, SuspendThreadLoop, NULL);
    if (ret != 0) {
        POWER_HILOGE("Failed to create suspend thread");
        return;
    }
    pthread_detach(g_suspendThread);
    started = TRUE;
}

static void IncSuspendBlockCounter()
{
    pthread_mutex_lock(&g_mutex);
    g_suspendBlockCounter++;
    POWER_HILOGD("Suspend block counter: %{public}d", g_suspendBlockCounter);
    pthread_mutex_unlock(&g_mutex);
}

static void DecSuspendBlockCounter()
{
    pthread_mutex_lock(&g_mutex);
    g_suspendBlockCounter--;
    POWER_HILOGD("Suspend block counter: %{public}d", g_suspendBlockCounter);
    if (SuspendConditionSatisfied() == TRUE) {
        pthread_cond_signal(&g_cond);
    }
    pthread_mutex_unlock(&g_mutex);
}

static struct AutoSuspendOps g_ops = {
    .Enable = Enable,
    .IncSuspendBlockCounter = IncSuspendBlockCounter,
    .DecSuspendBlockCounter = DecSuspendBlockCounter,
};

struct AutoSuspendOps* AutoSuspendOpsInit()
{
    g_suspendLoop = AutoSuspendLoopInit();
    if (!g_suspendLoop) {
        POWER_HILOGE("Failed to init auto suspend loop");
        return NULL;
    }
    return &g_ops;
}
