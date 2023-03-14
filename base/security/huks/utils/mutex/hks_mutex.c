/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hks_mutex.h"

#include <pthread.h>

#include "hks_mem.h"

struct HksMutex {
    pthread_mutex_t mutex;
};

HksMutex *HksMutexCreate(void)
{
    HksMutex *mutex = (HksMutex *)HksMalloc(sizeof(HksMutex));
    if (mutex != NULL) {
        int result = pthread_mutex_init(&mutex->mutex, NULL);
        if (result != 0) {
            HksFree(mutex);
            mutex = NULL;
        }
    }
    return mutex;
}

int32_t HksMutexLock(HksMutex *mutex)
{
    if (mutex == NULL) {
        return -1;
    }

    return pthread_mutex_lock(&mutex->mutex);
}

int32_t HksMutexUnlock(HksMutex *mutex)
{
    if (mutex == NULL) {
        return -1;
    }

    return pthread_mutex_unlock(&mutex->mutex);
}

void HksMutexClose(HksMutex *mutex)
{
    if (mutex == NULL) {
        return;
    }

    pthread_mutex_destroy(&mutex->mutex);
    HksFree(mutex);
}