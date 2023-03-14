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

#ifndef OHOS_ACELITE_ACE_AUTO_LOCK_H
#define OHOS_ACELITE_ACE_AUTO_LOCK_H

#include "acelite_config.h"

#include "memory_heap.h"
#include "non_copyable.h"
#if (defined(__LINUX__) || defined(__LITEOS_A__))
#include <pthread.h>
#endif

namespace OHOS {
namespace ACELite {
// The base lock type
class LockType final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(LockType);
    LockType();
    ~LockType();
    // Lock mutex, waiting for it if necessary.
    void Lock();
    void Unlock();

private:
#if (defined(__LINUX__) || defined(__LITEOS_A__))
    pthread_mutex_t mutex_;
#endif
    // the mutex initialization result, 0 means success
    int mutexInited_;
};

// The automatic lock and unlock wrapper binding with the scope.
// Usage: declare LockType lock inadvance, and declare AutoLockGuard(lock) in the target locking scope.
class AutoLockGuard final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(AutoLockGuard);
    explicit AutoLockGuard(LockType &lock);
    ~AutoLockGuard();

private:
    LockType &lock_;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_ACE_AUTO_LOCK_H