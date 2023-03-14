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

#ifndef POWERMGR_RUNNING_LOCK_ENTRY_H
#define POWERMGR_RUNNING_LOCK_ENTRY_H

#include <common.h>

#include "running_lock.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct {
    uint32_t pid;
    uint64_t token;
} RunningLockIdentity;

typedef struct {
    BOOL isHolding;
} RunningLockStatus;

typedef struct {
    RunningLock lock;
    RunningLockIdentity identity;
    RunningLockStatus status;
} RunningLockEntry;

static inline RunningLockEntry *GetRunningLockEntry(const RunningLock *lock)
{
    return GET_OBJECT(lock, RunningLockEntry, lock);
}

static inline BOOL IsValidRunningLockEntry(RunningLockEntry *entry)
{
    return (entry != NULL) && ((entry->lock.type >= 0) && (entry->lock.type < RUNNINGLOCK_BUTT)) ? TRUE : FALSE;
}

static inline BOOL IsSameRunningLockIdentity(const RunningLockIdentity *a, const RunningLockIdentity *b)
{
    return ((a->pid == b->pid) && (a->token == b->token)) ? TRUE : FALSE;
}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // POWERMGR_RUNNING_LOCK_ENTRY_H
