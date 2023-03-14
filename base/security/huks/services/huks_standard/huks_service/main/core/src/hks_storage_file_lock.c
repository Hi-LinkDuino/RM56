/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hks_storage_file_lock.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "securec.h"

#include "hks_lock.h"
#include "hks_mem.h"
#include "hks_mutex.h"

struct HksStorageFileLock {
    char *path;
    HksLock *lock;
    uint32_t ref;
    HksStorageFileLock *next;
};

static HksStorageFileLock *g_lockListFirst = NULL;
static HksStorageFileLock *g_lockListLast = NULL;
static HksMutex *g_lockListLock = NULL;

static void FreeFileLock(HksStorageFileLock *lock)
{
    if (lock == NULL) {
        return;
    }

    if (lock->path) {
        HksFree(lock->path);
        lock->path = NULL;
    }

    if (lock->lock) {
        HksLockClose(lock->lock);
        lock->lock = NULL;
    }

    HksFree(lock);
}

static void ClearLockList(void)
{
    if (g_lockListFirst == NULL) {
        return;
    }

    HksStorageFileLock *iter = g_lockListFirst;
    HksStorageFileLock *temp = NULL;
    while (iter != NULL) {
        temp = iter->next;
        FreeFileLock(iter);
        iter = temp;
    }

    g_lockListFirst = NULL;
    g_lockListLast = NULL;
}

static HksStorageFileLock *FindFileLock(const char *path)
{
    HksStorageFileLock *iter = g_lockListFirst;
    while (iter != NULL) {
        if (strcmp(path, iter->path) == 0) {
            return iter;
        } else {
            iter = iter->next;
        }
    }
    return NULL;
}

static HksStorageFileLock *AllocFileLock(const char *path)
{
    HksStorageFileLock *lock = HksMalloc(sizeof(HksStorageFileLock));
    if (lock == NULL) {
        return NULL;
    }
    size_t len = strlen(path);
    lock->path = HksMalloc(len + 1);
    if (lock->path == NULL) {
        FreeFileLock(lock);
        return NULL;
    }
    if (strcpy_s(lock->path, len + 1, path) != EOK) {
        FreeFileLock(lock);
        return NULL;
    }
    lock->lock = HksLockCreate();
    lock->ref = 1;
    lock->next = NULL;
    if (lock->path == NULL || lock->lock == NULL) {
        FreeFileLock(lock);
        return NULL;
    }
    return lock;
}

static void AddRef(HksStorageFileLock *lock)
{
    if (lock == NULL) {
        return;
    }

    lock->ref++;
}

static void AppendFileLock(HksStorageFileLock *lock)
{
    if (g_lockListFirst == NULL) {
        g_lockListFirst = lock;
        g_lockListFirst->next = NULL;
        g_lockListLast = lock;
        return;
    }

    if (g_lockListLast != NULL) {
        g_lockListLast->next = lock;
        g_lockListLast = lock;
        g_lockListLast->next = NULL;
    }
}

HksStorageFileLock *HksStorageFileLockCreate(const char *path)
{
    if (path == NULL) {
        return NULL;
    }

    if (g_lockListLock == NULL) {
        return NULL;
    }

    if (HksMutexLock(g_lockListLock) != 0) {
        return NULL;
    }
    HksStorageFileLock *lock = FindFileLock(path);
    if (lock == NULL) {
        lock = AllocFileLock(path);
        if (lock != NULL) {
            AppendFileLock(lock);
        }
    } else {
        AddRef(lock);
    }
    (void)HksMutexUnlock(g_lockListLock);

    return lock;
}

int32_t HksStorageFileLockRead(HksStorageFileLock *lock)
{
    if (lock == NULL) {
        return -1;
    }
    return HksLockLockRead(lock->lock);
}

int32_t HksStorageFileUnlockRead(HksStorageFileLock *lock)
{
    if (lock == NULL) {
        return -1;
    }
    return HksLockUnlockRead(lock->lock);
}

int32_t HksStorageFileLockWrite(HksStorageFileLock *lock)
{
    if (lock == NULL) {
        return -1;
    }
    return HksLockLockWrite(lock->lock);
}

int32_t HksStorageFileUnlockWrite(HksStorageFileLock *lock)
{
    if (lock == NULL) {
        return -1;
    }
    return HksLockUnlockWrite(lock->lock);
}

static bool IsLockInList(HksStorageFileLock *lock)
{
    HksStorageFileLock *iter = g_lockListFirst;
    while (iter != NULL) {
        if (lock == iter) {
            return true;
        } else {
            iter = iter->next;
        }
    }
    return false;
}

static uint32_t Release(HksStorageFileLock *lock)
{
    uint32_t ref = 0;
    HksStorageFileLock *iter = g_lockListFirst;
    HksStorageFileLock *previous = NULL;
    bool remove = false;
    while (iter != NULL) {
        if (lock == iter) {
            lock->ref--;
            ref = lock->ref;
            if (ref == 0) {
                remove = true;
            }
            break;
        } else {
            previous = iter;
            iter = iter->next;
        }
    }

    if (remove) {
        if (previous != NULL) {
            previous->next = iter->next;
        } else {
            g_lockListFirst = iter->next;
        }
        if (g_lockListLast == lock) {
            g_lockListLast = previous;
        }
        FreeFileLock(lock);
    }

    return ref;
}

void HksStorageFileLockRelease(HksStorageFileLock *lock)
{
    if (lock == NULL) {
        return;
    }

    if (g_lockListLock == NULL) {
        return;
    }

    if (HksMutexLock(g_lockListLock) != 0) {
        return;
    }

    if (IsLockInList(lock)) {
        Release(lock);
    }

    (void)HksMutexUnlock(g_lockListLock);
}

__attribute__((constructor)) static void OnLoad(void)
{
    g_lockListLock = HksMutexCreate();
    g_lockListFirst = NULL;
    g_lockListLast = NULL;
}

__attribute__((destructor)) static void OnUnload(void)
{
    if (g_lockListLock != NULL) {
        HksMutexClose(g_lockListLock);
        g_lockListLock = NULL;
    }

    ClearLockList();
}