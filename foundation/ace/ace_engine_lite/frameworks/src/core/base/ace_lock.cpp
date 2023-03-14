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
#include "ace_lock.h"

#if (defined(__LITEOS_M__) || (OHOS_ACELITE_PRODUCT_WATCH == 1))
#include "los_task.h"
#endif

namespace OHOS {
namespace ACELite {
#if (defined(__LINUX__) || defined(__LITEOS_A__))
LockType::LockType() : mutex_(PTHREAD_MUTEX_INITIALIZER), mutexInited_(0)
{
    mutexInited_ = pthread_mutex_init(&mutex_, nullptr);
}
#else
LockType::LockType() : mutexInited_(0)
{
    (void)mutexInited_;
}
#endif

LockType::~LockType()
{
#if (defined(__LINUX__) || defined(__LITEOS_A__))
    if (mutexInited_ == 0) {
        pthread_mutex_destroy(&mutex_);
    }
#endif
}

void LockType::Lock()
{
#if (defined(__LINUX__) || defined(__LITEOS_A__))
    if (mutexInited_ == 0) {
        pthread_mutex_lock(&mutex_);
    }
#endif
}
void LockType::Unlock()
{
#if (defined(__LINUX__) || defined(__LITEOS_A__))
    if (mutexInited_ == 0) {
        pthread_mutex_unlock(&mutex_);
    }
#endif
}

AutoLockGuard::AutoLockGuard(LockType &lock) : lock_(lock)
{
#if (defined(__LINUX__) || defined(__LITEOS_A__))
    lock_.Lock();
#elif (defined(__LITEOS_M__) || (OHOS_ACELITE_PRODUCT_WATCH == 1))
    LOS_TaskLock();
#else
    (void)lock_;
#endif
}

AutoLockGuard::~AutoLockGuard()
{
#if (defined(__LINUX__) || defined(__LITEOS_A__))
    lock_.Unlock();
#elif (defined(__LITEOS_M__) || (OHOS_ACELITE_PRODUCT_WATCH == 1))
    LOS_TaskUnlock();
#endif
}
} // namespace ACELite
} // namespace OHOS
