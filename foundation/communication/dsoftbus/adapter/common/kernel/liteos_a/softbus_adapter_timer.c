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

#include "softbus_adapter_timer.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "securec.h"
#include "softbus_adapter_log.h"
#include "softbus_adapter_socket.h"
#include "softbus_def.h"
#include "softbus_errcode.h"

#define MS_PER_SECOND 1000
#define US_PER_MSECOND 1000
#define NS_PER_USECOND 1000

static unsigned int g_timerType;

void *SoftBusCreateTimer(void **timerId, void *timerFunc, unsigned int type)
{
    if (timerId == NULL) {
        HILOG_ERROR(SOFTBUS_HILOG_ID, "timerId is null");
        return NULL;
    }
    struct sigevent envent;
    (void)memset_s(&envent, sizeof(envent), 0, sizeof(envent));
    envent.sigev_notify = SIGEV_SIGNAL;
    envent.sigev_signo = SIGUSR1;
    signal(SIGUSR1, timerFunc);

    g_timerType = type;
    if (timer_create(CLOCK_REALTIME, &envent, timerId) != 0) {
        HILOG_ERROR(SOFTBUS_HILOG_ID, "timer create error, errno code: [%{public}d]", errno);
        return NULL;
    }

    return *timerId;
}

int SoftBusStartTimer(void *timerId, unsigned int tickets)
{
    if (timerId < 0) {
        HILOG_ERROR(SOFTBUS_HILOG_ID, "timerId is null");
        return SOFTBUS_ERR;
    }
    struct itimerspec value;
    (void)memset_s(&value, sizeof(value), 0, sizeof(value));
    value.it_value.tv_sec = tickets / MS_PER_SECOND;
    value.it_value.tv_nsec = 0;
    if (g_timerType == TIMER_TYPE_ONCE) {
        value.it_interval.tv_sec = tickets = 0;
        value.it_interval.tv_nsec = 0;
    } else {
        value.it_interval.tv_sec = tickets / MS_PER_SECOND;
        value.it_interval.tv_nsec = 0;
    }

    if (timer_settime(timerId, 0, &value, NULL) != 0) {
        HILOG_ERROR(SOFTBUS_HILOG_ID, "timer start error, errno code: [%{public}d]", errno);
        return SOFTBUS_ERR;
    }

    return SOFTBUS_OK;
}

int SoftBusDeleteTimer(void *timerId)
{
    if (timerId == NULL) {
        HILOG_ERROR(SOFTBUS_HILOG_ID, "timerId is null");
        return SOFTBUS_ERR;
    }

    if (timer_delete(timerId) != 0) {
        HILOG_ERROR(SOFTBUS_HILOG_ID, "timer delete err, errno code: [%{public}d]", errno);
        return SOFTBUS_ERR;
    }

    return SOFTBUS_OK;
}

int SoftBusSleepMs(unsigned int ms)
{
    int ret;
    struct timeval tm;
    tm.tv_sec = ms / MS_PER_SECOND;
    tm.tv_usec = (ms % MS_PER_SECOND) * US_PER_MSECOND;

    do {
        ret = select(0, NULL, NULL, NULL, &tm);
    } while ((ret == -1) && (errno == EINTR));

    return SOFTBUS_ERR;
}

int32_t SoftBusGetTime(SoftBusSysTime *sysTime)
{
    if (sysTime == NULL) {
        HILOG_INFO(SOFTBUS_HILOG_ID, "sysTime is null");
        return SOFTBUS_INVALID_PARAM;
    }
    struct timespec time = {0};
    (void)clock_gettime(CLOCK_MONOTONIC_RAW, &time);

    sysTime->sec = time.tv_sec;
    sysTime->usec = time.tv_nsec / NS_PER_USECOND;
    return SOFTBUS_OK;
}
