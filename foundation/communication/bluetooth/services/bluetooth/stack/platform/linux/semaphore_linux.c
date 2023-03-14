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

#include "platform/include/semaphore.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include "platform/include/mutex.h"
#include "platform/include/platform_def.h"
#include "securec.h"

#if !defined(EFD_SEMAPHORE)
#define EFD_SEMAPHORE (1 << 0)
#endif

typedef struct Semaphore {
    Mutex *mutex;
    int fd;
} SemaphoreInternal;

Semaphore *SemaphoreCreate(uint32_t val)
{
    int efd = eventfd(val, EFD_SEMAPHORE);
    if (efd == -1) {
        LOG_ERROR("SemaphoreCreate: create eventfd failed, error no: %{public}d.", errno);
        return NULL;
    }

    Mutex *mutex = MutexCreate();
    if (mutex == NULL) {
        close(efd);
        LOG_ERROR("SemaphoreCreate: create Mutex failed.");
        return NULL;
    }

    Semaphore *semaphore = (Semaphore *)malloc(sizeof(Semaphore));
    if (semaphore == NULL) {
        close(efd);
        MutexDelete(mutex);
        LOG_ERROR("SemaphoreCreate: create Semaphore failed.");
        return NULL;
    }
    (void)memset_s(semaphore, sizeof(Semaphore), 0, sizeof(Semaphore));
    semaphore->fd = efd;
    semaphore->mutex = mutex;

    return semaphore;
}

void SemaphoreDelete(Semaphore *sem)
{
    if (sem == NULL) {
        return;
    }

    if (sem->fd != -1) {
        close(sem->fd);
    }

    MutexDelete(sem->mutex);
    free(sem);
}

void SemaphoreWait(const Semaphore *sem)
{
    ASSERT(sem);

    eventfd_t val;
    eventfd_read(sem->fd, &val);
}

int32_t SemaphoreTryWait(Semaphore *sem)
{
    ASSERT(sem);

    MutexLock(sem->mutex);
    int flags = fcntl(sem->fd, F_GETFL);
    if (flags == -1) {
        goto ERROR;
    }

    if (fcntl(sem->fd, F_SETFL, flags | EFD_NONBLOCK) == -1) {
        goto ERROR;
    }

    eventfd_t val;
    if (eventfd_read(sem->fd, &val) == -1) {
        goto ERROR;
    }

    if (fcntl(sem->fd, F_SETFL, flags) == -1) {
        goto ERROR;
    }

    MutexUnlock(sem->mutex);
    return 0;

ERROR:
    MutexUnlock(sem->mutex);
    return -1;
}

void SemaphorePost(const Semaphore *sem)
{
    ASSERT(sem);

    eventfd_write(sem->fd, 1);
}

int32_t SemaphoreTryPost(Semaphore *sem)
{
    ASSERT(sem);

    MutexLock(sem->mutex);
    int flags = fcntl(sem->fd, F_GETFL);
    if (flags == -1) {
        goto ERROR;
    }

    if (fcntl(sem->fd, F_SETFL, flags | EFD_NONBLOCK) == -1) {
        goto ERROR;
    }

    if (eventfd_write(sem->fd, 1) == -1) {
        goto ERROR;
    }

    if (fcntl(sem->fd, F_SETFL, flags) == -1) {
        goto ERROR;
    }

    MutexUnlock(sem->mutex);
    return 0;

ERROR:
    MutexUnlock(sem->mutex);
    return -1;
}

int SemaphoreGetfd(const Semaphore *sem)
{
    ASSERT(sem);

    return sem->fd;
}
