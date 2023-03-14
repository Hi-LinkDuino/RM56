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

#ifndef HKS_LOCK_H
#define HKS_LOCK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HksLock HksLock;

HksLock *HksLockCreate(void);
int32_t HksLockLockRead(HksLock *lock);
int32_t HksLockUnlockRead(HksLock *lock);
int32_t HksLockLockWrite(HksLock *lock);
int32_t HksLockUnlockWrite(HksLock *lock);
void HksLockClose(HksLock *lock);

#ifdef __cplusplus
}
#endif

#endif  // HKS_RW_LOCK_H
