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

#ifndef HC_MUTEX_H
#define HC_MUTEX_H

#include <stdint.h>
#include "pthread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HcMutexT {
    int (*lock)(struct HcMutexT* mutex);
    void (*unlock)(struct HcMutexT* mutex);
    pthread_mutex_t mutex;
} HcMutex;

int32_t InitHcMutex(HcMutex* mutex);
void DestroyHcMutex(HcMutex* mutex);

#ifdef __cplusplus
}
#endif
#endif