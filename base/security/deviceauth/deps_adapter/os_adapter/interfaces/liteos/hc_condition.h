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

#ifndef HC_CONDITION_H
#define HC_CONDITION_H

#include <semaphore.h>
#include "hc_mutex.h"
#include "hc_types.h"

typedef struct HcConditionT {
    int (*wait)(struct HcConditionT*);
    void (*notify)(struct HcConditionT*);
    int (*waitWithoutLock)(struct HcConditionT*);
    void (*notifyWithoutLock)(struct HcConditionT*);
    sem_t sem;
} HcCondition;

int32_t InitHcCond(HcCondition* hcCond, HcMutex* mutex);
void DestroyHcCond(HcCondition* hcCond);

#endif
