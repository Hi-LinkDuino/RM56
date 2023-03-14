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

#include "platform/include/mutex.h"
#include <stdlib.h>
#include <pthread.h>
#include "platform/include/platform_def.h"

typedef struct Mutex {
    pthread_mutex_t mutex;
} MutexInternal;

Mutex *MutexCreate()
{
    Mutex *mutex = (Mutex *)malloc(sizeof(Mutex));
    pthread_mutexattr_t attr = {0};
    pthread_mutexattr_init(&attr);

    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    if (mutex != NULL) {
        if (pthread_mutex_init(&mutex->mutex, &attr) != 0) {
            free(mutex);
            return NULL;
        }
    }
    return mutex;
}

void MutexDelete(Mutex *mutex)
{
    if (mutex == NULL) {
        return;
    }

    pthread_mutex_destroy(&mutex->mutex);
    free(mutex);
}

void MutexLock(Mutex *mutex)
{
    ASSERT(mutex);
    pthread_mutex_lock(&mutex->mutex);
}

void MutexUnlock(Mutex *mutex)
{
    ASSERT(mutex);
    pthread_mutex_unlock(&mutex->mutex);
}

int32_t MutexTryLock(Mutex *mutex)
{
    ASSERT(mutex);
    if (pthread_mutex_trylock(&mutex->mutex) != 0) {
        return -1;
    }
    return 0;
}