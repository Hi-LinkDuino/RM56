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

#include <stdlib.h>

#include <common.h>
#include <ohos_errno.h>
#include <pthread.h>
#include <securec.h>

#include "hilog_wrapper.h"
#include "power/suspend_controller.h"
#include "running_lock_entry.h"

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static Vector g_runningLocks[RUNNINGLOCK_BUTT];
static const char * const g_runningLockNames[RUNNINGLOCK_BUTT] = {
    "OHOSPowerMgr.Screen",
    "OHOSPowerMgr.Background",
    "OHOSPowerMgr.Proximity",
};

static RunningLockEntry *DupRunningLockEntry(RunningLockEntry *entry)
{
    RunningLockEntry *e = (RunningLockEntry *)malloc(sizeof(RunningLockEntry));
    if (e == NULL) {
        POWER_HILOGE("Failed allocate running lock entry");
        return NULL;
    }
    if (memcpy_s(e, sizeof(RunningLockEntry), entry, sizeof(RunningLockEntry)) != EOK) {
        POWER_HILOGE("Failed copy running lock entry");
        free(e);
        return NULL;
    }
    return e;
}

static BOOL AddRunningLockEntryLocked(Vector *vec, RunningLockEntry *entry)
{
    int16_t pos = VECTOR_FindByKey(vec, (void *)&entry->identity);
    if (pos >= 0) {
        POWER_HILOGD("Already acquired: %s", entry->lock.name);
        return TRUE;
    }
    RunningLockEntry *e = DupRunningLockEntry(entry);
    if (e == NULL) {
        POWER_HILOGE("Failed duplicate running lock entry");
        return FALSE;
    }
    pos = VECTOR_Add(vec, (void *)e);
    if (pos == INVALID_INDEX) {
        POWER_HILOGE("Failed to add entry to vector");
        free(e);
        return FALSE;
    }
    POWER_HILOGD("Add running lock entry, name: %s, type: %d", e->lock.name, e->lock.type);
    if (VECTOR_Num(vec) == 1) {
        ScAcquireRunningLock(g_runningLockNames[entry->lock.type]);
    }
    return TRUE;
}

static BOOL RemoveRunningLockEntryLocked(Vector *vec, RunningLockEntry *entry)
{
    int16_t pos = VECTOR_FindByKey(vec, (void *)&entry->identity);
    if (pos < 0) {
        POWER_HILOGE("Non-existent running lock");
        return TRUE;
    }
    RunningLockEntry *e = (RunningLockEntry *)VECTOR_Swap(vec, pos, NULL);
    free(e);
    POWER_HILOGD("Remove running lock entry, name: %s, type: %d", entry->lock.name, entry->lock.type);
    if (VECTOR_Num(vec) == 0) {
        ScReleaseRunningLock(g_runningLockNames[entry->lock.type]);
    }
    return TRUE;
}

static void ShowLocks()
{
#ifdef OHOS_DEBUG
    for (int32_t t = RUNNINGLOCK_SCREEN; t < RUNNINGLOCK_BUTT; t++) {
        Vector *vec = &g_runningLocks[t];
        int32_t size = VECTOR_Size(vec);
        POWER_HILOGD("type: %d, lock num: %d", t, VECTOR_Num(vec));
        for (int32_t i = 0; i < size; i++) {
            RunningLockEntry* e = VECTOR_At(vec, i);
            if (e != NULL) {
                POWER_HILOGD("No.%d, name: %s, pid: %u, token: %llu",
                    i, e->lock.name, e->identity.pid, (long long)e->identity.token);
            }
        }
    }
#endif
}

int32_t RunningLockMgrAcquireEntry(RunningLockEntry *entry, int32_t timeoutMs)
{
    if (IsValidRunningLockEntry(entry) == FALSE) {
        POWER_HILOGE("Invalid running lock entry");
        return EC_INVALID;
    }
    pthread_mutex_lock(&g_mutex);
    Vector *vec = &g_runningLocks[entry->lock.type];
    BOOL ret = AddRunningLockEntryLocked(vec, entry);
    ShowLocks();
    pthread_mutex_unlock(&g_mutex);
    return (ret == TRUE) ? EC_SUCCESS : EC_FAILURE;
}

int32_t RunningLockMgrReleaseEntry(RunningLockEntry *entry)
{
    if (IsValidRunningLockEntry(entry) == FALSE) {
        POWER_HILOGE("Invalid running lock entry");
        return EC_INVALID;
    }

    pthread_mutex_lock(&g_mutex);
    Vector *vec = &g_runningLocks[entry->lock.type];
    BOOL ret = RemoveRunningLockEntryLocked(vec, entry);
    ShowLocks();
    pthread_mutex_unlock(&g_mutex);
    return (ret == TRUE) ? EC_SUCCESS : EC_FAILURE;
}

uint32_t RunningLockMgrGetLockCount(RunningLockType type)
{
    uint32_t cnt = 0;
    if ((type >= 0) && (type < RUNNINGLOCK_BUTT)) {
        pthread_mutex_lock(&g_mutex);
        cnt = VECTOR_Num(&g_runningLocks[type]);
        pthread_mutex_unlock(&g_mutex);
    }
    return cnt;
}

uint32_t RunningLockMgrGetTotalLockCount()
{
    uint32_t cnt = 0;
    pthread_mutex_lock(&g_mutex);
    for (int32_t t = RUNNINGLOCK_SCREEN; t < RUNNINGLOCK_BUTT; t++) {
        cnt += VECTOR_Num(&g_runningLocks[t]);
    }
    pthread_mutex_unlock(&g_mutex);
    POWER_HILOGD("Total lock count: %u", cnt);
    return cnt;
}

static const RunningLockIdentity *GetRunningLockIdentity(const RunningLockEntry *entry)
{
    return &entry->identity;
}

static int32_t RunningLockIdentityCmp(const RunningLockIdentity *a, const RunningLockIdentity *b)
{
    return (IsSameRunningLockIdentity(a, b) == TRUE) ? 0 : -1;
}

void RunningLockMgrInit(void)
{
    for (int32_t i = 0; i < RUNNINGLOCK_BUTT; i++) {
        g_runningLocks[i] = VECTOR_Make((VECTOR_Key)GetRunningLockIdentity, (VECTOR_Compare)RunningLockIdentityCmp);
    }
}
