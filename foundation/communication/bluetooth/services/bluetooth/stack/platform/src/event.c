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

#include "platform/include/event.h"
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include "platform/include/platform_def.h"

typedef struct Event {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool signal;
    bool autoClear;
} EventInternal;

Event *EventCreate(bool isAutoClear)
{
    Event *event = (Event *)malloc(sizeof(Event));
    if (event != NULL) {
        pthread_mutex_init(&event->mutex, NULL);
        pthread_cond_init(&event->cond, 0);
        event->autoClear = isAutoClear;
        event->signal = false;
    }
    return event;
}

void EventDelete(Event *event)
{
    if (event == NULL) {
        return;
    }

    while (EBUSY == pthread_cond_destroy(&event->cond)) {
        EventClear(event);
    };
    pthread_mutex_destroy(&event->mutex);
    free(event);
}

void EventSet(Event *event)
{
    ASSERT(event);

    pthread_mutex_lock(&event->mutex);
    if (event->signal) {
        pthread_mutex_unlock(&event->mutex);
        return;
    }

    event->signal = true;
    pthread_cond_broadcast(&event->cond);
    pthread_mutex_unlock(&event->mutex);
}

void EventClear(Event *event)
{
    ASSERT(event);

    pthread_mutex_lock(&event->mutex);
    event->signal = false;
    pthread_cond_broadcast(&event->cond);
    pthread_mutex_unlock(&event->mutex);
}

static int32_t EventWaitInternal(Event *event, int64_t ms)
{
    int ret;
    if (ms < 0) {
        ret = pthread_cond_wait(&event->cond, &event->mutex);
    } else {
        struct timeval now;
        struct timespec timeout;
        gettimeofday(&now, NULL);
        timeout.tv_sec = now.tv_sec + ms / MS_PER_SECOND;
        timeout.tv_nsec = now.tv_usec * MS_PER_SECOND + (ms % MS_PER_SECOND) * NS_PER_MS;
        ret = pthread_cond_timedwait(&event->cond, &event->mutex, &timeout);
    }

    return ret;
}

int32_t EventWait(Event *event, int64_t ms)
{
    ASSERT(event);

    int32_t ret = 0;
    pthread_mutex_lock(&event->mutex);

    if (event->signal == false) {
        int32_t err;
        err = EventWaitInternal(event, ms);
        if (err == ETIMEDOUT) {
            ret = EVENT_WAIT_TIMEOUT_ERR;
        } else if (err) {
            ret = EVENT_WAIT_OTHER_ERR;
        }
        if (event->autoClear && event->signal) {
            event->signal = false;
        }
    } else {
        if (event->autoClear) {
            event->signal = false;
        }
    }

    pthread_mutex_unlock(&event->mutex);
    return ret;
}