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

#ifndef POWERMGR_RUNNING_LOCK_H
#define POWERMGR_RUNNING_LOCK_H

#include <stdint.h>

#include <ohos_types.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum {
    /**
     * RunningLock type: used to keep screen on.
     */
    RUNNINGLOCK_SCREEN,
    /**
     * RunningLock type: used to keep cpu running.
     */
    RUNNINGLOCK_BACKGROUND,
    /**
     * RunningLock type: used to keep the screen on/off when the proximity sensor is active.
     */
    RUNNINGLOCK_PROXIMITY_SCREEN_CONTROL,
    RUNNINGLOCK_BUTT
} RunningLockType;

typedef enum {
    RUNNINGLOCK_FLAG_NONE = 0,

    /**
     * Wakeup device when running lock is acquired.
     */
    RUNNINGLOCK_FLAG_WAKEUP_WHEN_ACQUIRED = 1 << 0,
} RunningLockFlag;

#define RUNNING_LOCK_NAME_LEN   64

typedef struct {
    char name[RUNNING_LOCK_NAME_LEN];
    RunningLockType type;
    RunningLockFlag flag;
} RunningLock;

const RunningLock *CreateRunningLock(const char *name, RunningLockType type, RunningLockFlag flag);
void DestroyRunningLock(const RunningLock *lock);
BOOL AcquireRunningLock(const RunningLock *lock);
BOOL ReleaseRunningLock(const RunningLock *lock);
BOOL IsRunningLockHolding(const RunningLock *lock);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // POWERMGR_RUNNING_LOCK_H
