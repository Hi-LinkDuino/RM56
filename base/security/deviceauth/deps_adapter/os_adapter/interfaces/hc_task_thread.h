/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef HC_TASK_THREAD_H
#define HC_TASK_THREAD_H

#include "hc_thread.h"
#include "hc_vector.h"

typedef struct HcTaskBaseT {
    void (*doAction) (struct HcTaskBaseT*);
    void (*destroy) (struct HcTaskBaseT*);
} HcTaskBase;

typedef struct {
    HcTaskBase* task;
} HcTaskWrap;

DECLARE_HC_VECTOR(TaskVec, HcTaskWrap)

typedef struct HcTaskThreadT {
    HcThread thread;
    TaskVec tasks;
    int32_t (*startThread)(struct HcTaskThreadT* thread);
    void (*pushTask) (struct HcTaskThreadT* thread, HcTaskBase* task);
    void (*clear) (struct HcTaskThreadT* thread);
    void (*stopAndClear) (struct HcTaskThreadT* thread);
    HcMutex queueLock;
    HcBool quit;
} HcTaskThread;

int32_t InitHcTaskThread(HcTaskThread* thread, size_t stackSize, const char* threadName);
void DestroyHcTaskThread(HcTaskThread* thread);
#endif