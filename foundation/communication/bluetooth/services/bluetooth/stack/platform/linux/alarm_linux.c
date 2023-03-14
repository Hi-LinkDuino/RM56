/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "platform/include/alarm.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include "platform/include/thread.h"
#include "platform/include/reactor.h"
#include "platform/include/mutex.h"
#include "platform/include/platform_def.h"
#include "securec.h"

#define BT_CLOCK_MONOTONIC CLOCK_MONOTONIC

static const char *g_defaultName = "bt-alarm";

static Thread *g_alarmThread = NULL;

typedef struct {
    void *parameter;
    AlarmCallback run;
} AlarmContext;

typedef struct Alarm {
    int timerFd;
    Mutex *mutex;
    bool isPeriodic;
    AlarmContext context;
    ReactorItem *reactorItem;
    char name[ALARM_NAME_SIZE + 1];
} AlarmInternal;

int32_t AlarmModuleInit()
{
    g_alarmThread = ThreadCreate("Stack-Alarm");
    if (g_alarmThread == NULL) {
        LOG_ERROR("Alarm thread create failed.");
        return -1;
    }
    return 0;
}

void AlarmModuleCleanup()
{
    if (g_alarmThread == NULL) {
        return;
    }

    ThreadDelete(g_alarmThread);
    g_alarmThread = NULL;
}

static void AlarmNotify(void *parameter)
{
    Alarm *alarm = (Alarm *)parameter;

    MutexLock(alarm->mutex);

    AlarmCallback run = alarm->context.run;
    void *param = alarm->context.parameter;

    uint64_t value = 0;
    int ret = read(alarm->timerFd, &value, sizeof(uint64_t));
    if (ret == sizeof(uint64_t)) {
        if (value > 1) {
            LOG_WARN("Alarm has expired more than one times.");
        }
    } else {
        if (errno == EAGAIN) {
            LOG_INFO("Alarm is stopped or reset before callback called.");
        } else {
            LOG_ERROR("Alarm read value failed, error no: %{public}d.", errno);
        }
        run = NULL;
    }
    MutexUnlock(alarm->mutex);

    if (run) {
        run(param);
    }
}

Alarm *AlarmCreate(const char *name, const bool isPeriodic)
{
    Alarm *alarm = (Alarm *)calloc(1, (sizeof(Alarm)));
    if (alarm == NULL) {
        LOG_ERROR("Failed to call calloc in func AlarmCreate");
        return NULL;
    }

    if (name != NULL) {
        (void)strncpy_s(alarm->name, ALARM_NAME_SIZE + 1, name, ALARM_NAME_SIZE);
    } else {
        (void)strncpy_s(alarm->name, ALARM_NAME_SIZE + 1, g_defaultName, ALARM_NAME_SIZE);
    }

    int timerFd = timerfd_create(BT_CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timerFd == -1) {
        LOG_ERROR("Alarm create timer-fd failed, error no: %{public}d.", errno);
        goto ERROR;
    }

    Mutex *mutex = MutexCreate();
    if (mutex == NULL) {
        close(timerFd);
        LOG_ERROR("Alarm create mutex failed.");
        goto ERROR;
    }

    alarm->timerFd = timerFd;
    alarm->isPeriodic = isPeriodic;
    alarm->mutex = mutex;

    ReactorItem *item =
        ReactorRegister(ThreadGetReactor(g_alarmThread), alarm->timerFd, (void *)alarm, AlarmNotify, NULL);
    if (item == NULL) {
        close(timerFd);
        LOG_ERROR("Alarm register reactor failed.");
        goto ERROR;
    }

    alarm->reactorItem = item;

    return alarm;

ERROR:

    if (alarm != NULL) {
        MutexDelete(alarm->mutex);
        free(alarm);
    }
    return NULL;
}

void AlarmDelete(Alarm *alarm)
{
    if (alarm == NULL) {
        return;
    }

    ReactorUnregister(alarm->reactorItem);
    close(alarm->timerFd);
    MutexDelete(alarm->mutex);
    free(alarm);
}

int32_t AlarmSet(Alarm *alarm, uint64_t timeMs, AlarmCallback callback, void *parameter)
{
    ASSERT(alarm);

    MutexLock(alarm->mutex);

    alarm->context.parameter = parameter;
    alarm->context.run = callback;

    struct itimerspec its = {0};
    its.it_value.tv_sec = timeMs / MS_PER_SECOND;
    its.it_value.tv_nsec = (timeMs % MS_PER_SECOND) * NS_PER_MS;
    if (alarm->isPeriodic) {
        its.it_interval = its.it_value;
    }

    if (timerfd_settime(alarm->timerFd, 0, &its, NULL) == -1) {
        LOG_ERROR("Alarm settime failed, error no: %{public}d.", errno);
        MutexUnlock(alarm->mutex);
        return -1;
    }

    MutexUnlock(alarm->mutex);
    return 0;
}

void AlarmCancel(Alarm *alarm)
{
    ASSERT(alarm);

    MutexLock(alarm->mutex);
    struct itimerspec its = {0};
    if (timerfd_settime(alarm->timerFd, 0, &its, NULL) == -1) {
        LOG_ERROR("Alarm cancel: settime 0 failed, error no: %{public}d.", errno);
    }
    MutexUnlock(alarm->mutex);
}