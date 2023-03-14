/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "hks_rwlock.h"

#include <pthread.h>

#include "hks_mem.h"

struct HksRwlock {
    pthread_rwlock_t lock;
};

HksRwlock *HksRwlockCreate(void)
{
    HksRwlock *rwLock = (HksRwlock *)HksMalloc(sizeof(HksRwlock));
    if (rwLock != NULL) {
        int result = pthread_rwlock_init(&rwLock->lock, NULL);
        if (result != 0) {
            HksFree(rwLock);
            rwLock = NULL;
        }
    }
    return rwLock;
}

int32_t HksRwlockLockRead(HksRwlock *lock)
{
    if (lock == NULL) {
        return -1;
    }

    return pthread_rwlock_rdlock(&lock->lock);
}

int32_t HksRwlockUnlockRead(HksRwlock *lock)
{
    if (lock == NULL) {
        return -1;
    }
    return pthread_rwlock_unlock(&lock->lock);
}

int32_t HksRwlockLockWrite(HksRwlock *lock)
{
    if (lock == NULL) {
        return -1;
    }
    return pthread_rwlock_wrlock(&lock->lock);
}

int32_t HksRwlockUnlockWrite(HksRwlock *lock)
{
    if (lock == NULL) {
        return -1;
    }
    return pthread_rwlock_unlock(&lock->lock);
}

void HksRwlockClose(HksRwlock *lock)
{
    if (lock == NULL) {
        return;
    }

    pthread_rwlock_destroy(&lock->lock);
    HksFree(lock);
}