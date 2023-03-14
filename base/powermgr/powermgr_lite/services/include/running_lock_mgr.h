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

#ifndef POWERMGR_RUNNING_LOCK_MGR_H
#define POWERMGR_RUNNING_LOCK_MGR_H

#include "running_lock_entry.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void RunningLockMgrInit(void);
int32_t RunningLockMgrAcquireEntry(RunningLockEntry *entry, int32_t timeoutMs);
int32_t RunningLockMgrReleaseEntry(RunningLockEntry *entry);
uint32_t RunningLockMgrGetLockCount(RunningLockType type);
uint32_t RunningLockMgrGetTotalLockCount();

static inline BOOL RunningLockMgrIsLockHolding(RunningLockType type)
{
    return (RunningLockMgrGetLockCount(type) > 0) ? TRUE : FALSE;
}

static inline BOOL RunningLockMgrIsAnyLockHolding()
{
    return (RunningLockMgrGetTotalLockCount() > 0) ? TRUE : FALSE;
}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // POWERMGR_RUNNING_LOCK_MGR_H
