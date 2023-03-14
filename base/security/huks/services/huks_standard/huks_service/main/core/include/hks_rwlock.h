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

#ifndef HKS_RWLOCK_H
#define HKS_RWLOCK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HksRwlock HksRwlock;

HksRwlock *HksRwlockCreate(void);
int32_t HksRwlockLockRead(HksRwlock *lock);
int32_t HksRwlockUnlockRead(HksRwlock *lock);
int32_t HksRwlockLockWrite(HksRwlock *lock);
int32_t HksRwlockUnlockWrite(HksRwlock *lock);
void HksRwlockClose(HksRwlock *lock);

#ifdef __cplusplus
}
#endif

#endif  // HKS_RWLOCK_H
