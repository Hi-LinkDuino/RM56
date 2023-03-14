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

#include "hc_condition.h"

int HcCondWait(struct HcConditionT* hcCond)
{
    if (hcCond == NULL) {
        return -1;
    }

    return sem_wait(&hcCond->sem);
}

void HcCondNotify(struct HcConditionT* hcCond)
{
    if (hcCond == NULL) {
        return;
    }

    sem_post(&hcCond->sem);
}

int32_t InitHcCond(HcCondition* hcCond, HcMutex* mutex)
{
    (void)mutex;
    if (hcCond == NULL) {
        return -1;
    }
    hcCond->wait = HcCondWait;
    hcCond->notify = HcCondNotify;
    hcCond->waitWithoutLock = HcCondWait;
    hcCond->notifyWithoutLock = HcCondNotify;

    // init the signal value to zero
    return sem_init(&hcCond->sem, 0, 0);
}

void DestroyHcCond(HcCondition* hcCond)
{
    if (hcCond == NULL) {
        return;
    }

    sem_destroy(&hcCond->sem);
}