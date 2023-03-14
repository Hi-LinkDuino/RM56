/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "power_mgr_timer_util.h"

#include <common.h>
#include <errno.h>
#include <securec.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>

#include "hilog_wrapper.h"
#include "power_mgr_time_util.h"

typedef struct {
    PowerTimer timerId;
    BOOL isRunning;
    int64_t whenMsec;
    int64_t intervalMsec;
    PowerTimerCallback timerCb;
    void *data;
} PowerTimerInfo;

static inline PowerTimerInfo *GetPowerTimerInfo(PowerTimer *timer)
{
    return GET_OBJECT(timer, PowerTimerInfo, timerId);
}

static void SetTimeSpec(struct timespec *ts, int64_t msec)
{
    ts->tv_sec = MsecToSec(msec);
    msec -= SecToMsec(ts->tv_sec);
    ts->tv_nsec = MsecToNsec(msec);
}

static BOOL StartTimer(PowerTimer timer, int64_t whenMsec, int64_t intervalMsec)
{
    struct itimerspec ts;
    SetTimeSpec(&ts.it_value, whenMsec);
    SetTimeSpec(&ts.it_interval, intervalMsec);
    int32_t ret = timer_settime(timer, 0, &ts, NULL);
    if (ret < 0) {
        POWER_HILOGE("Failed to start timer");
        return FALSE;
    }
    return TRUE;
}

static void TimerHandle(union sigval v)
{
    PowerTimerInfo *info = (PowerTimerInfo *)v.sival_ptr;
    if (info == NULL) {
        POWER_HILOGE("Invalid timer info");
        return;
    }
    if (info->timerCb != NULL) {
        info->timerCb(info->data);
    }
}

PowerTimer *PowerMgrCreateTimer(int64_t whenMsec, int64_t intervalMsec, PowerTimerCallback cb)
{
    PowerTimerInfo *info = (PowerTimerInfo *)malloc(sizeof(PowerTimerInfo));
    if (info == NULL) {
        POWER_HILOGE("Failed allocate timer info");
        return NULL;
    }
    (void)memset_s(info, sizeof(PowerTimerInfo), 0, sizeof(PowerTimerInfo));
    info->isRunning = FALSE;
    info->whenMsec = whenMsec;
    info->intervalMsec = intervalMsec;
    info->timerCb = cb;

    struct sigevent evp;
    (void)memset_s(&evp, sizeof(evp), 0, sizeof(evp));
    evp.sigev_value.sival_ptr = info;
    evp.sigev_notify = SIGEV_THREAD;
    evp.sigev_notify_function = TimerHandle;
    int32_t ret = timer_create(CLOCK_REALTIME, &evp, &info->timerId);
    if (ret < 0) {
        POWER_HILOGE("Failed to create timer");
        free(info);
        return NULL;
    }
    POWER_HILOGD("Succeed to create timer, id: %p", info->timerId);

    return &info->timerId;
}

BOOL PowerMgrResetTimer(PowerTimer *timer, int64_t whenMsec, int64_t intervalMsec)
{
    if (timer == NULL) {
        POWER_HILOGE("Invalid timer");
        return FALSE;
    }
    PowerMgrStopTimer(timer);
    PowerTimerInfo *info = GetPowerTimerInfo(timer);
    info->whenMsec = whenMsec;
    info->intervalMsec = intervalMsec;
    return TRUE;
}

BOOL PowerMgrStartTimer(PowerTimer *timer, void *privateData)
{
    if (timer == NULL) {
        POWER_HILOGE("Invalid timer");
        return FALSE;
    }

    PowerTimerInfo *info = GetPowerTimerInfo(timer);
    info->data = privateData;
    info->isRunning = TRUE;
    return StartTimer(info->timerId, info->whenMsec, info->intervalMsec);
}

BOOL PowerMgrRestartTimer(PowerTimer *timer, void *privateData)
{
    if (timer == NULL) {
        POWER_HILOGE("Invalid timer");
        return FALSE;
    }

    return PowerMgrStartTimer(timer, privateData);
}

BOOL PowerMgrStopTimer(PowerTimer *timer)
{
    if (timer == NULL) {
        POWER_HILOGE("Invalid timer");
        return FALSE;
    }

    PowerTimerInfo *info = GetPowerTimerInfo(timer);
    info->isRunning = FALSE;
    return StartTimer(info->timerId, 0, 0);
}

void PowerMgrDestroyTimer(PowerTimer *timer)
{
    if (timer == NULL) {
        POWER_HILOGE("Invalid timer");
        return;
    }

    PowerTimerInfo *info = GetPowerTimerInfo(timer);
    int32_t ret = timer_delete(info->timerId);
    POWER_HILOGD("Destory timer: %d", ret);
    free(info);
}
