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

#ifndef HC_THREAD_H
#define HC_THREAD_H

#include "pthread.h"
#include "hc_types.h"
#include "hc_string.h"
#include "hc_condition.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*ThreadFunc)(void*);

typedef struct HcThreadT {
    ThreadFunc threadFunc;
    int (*start)(struct HcThreadT* thread);
    void(*join)(struct HcThreadT* thread);
    void (*wait)(struct HcThreadT* thread);
    void (*notify)(struct HcThreadT* thread);
    pthread_t thread;
    size_t stackSize;
    HcBool running;
    HcString name;
    HcCondition threadWaitObj;
    HcMutex threadLock;
    HcCondition bizWaitObj;
} HcThread;

int32_t InitThread(HcThread* thread, ThreadFunc func, size_t stackSize, const char* threadName);
void DestroyThread(HcThread* thread);

#ifdef __cplusplus
}
#endif
#endif