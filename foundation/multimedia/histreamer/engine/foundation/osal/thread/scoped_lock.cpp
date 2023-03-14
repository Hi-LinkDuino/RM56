/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "ScopedLock"

#include "scoped_lock.h"

#include <utility>

namespace OHOS {
namespace Media {
namespace OSAL {
ScopedLock::ScopedLock() : mutex_(nullptr)
{
}

ScopedLock::ScopedLock(Mutex& mutex) : mutex_(&mutex)
{
    mutex_->Lock();
}

ScopedLock::~ScopedLock()
{
    if (mutex_ != nullptr) {
        mutex_->Unlock();
        mutex_ = nullptr;
    }
}

ScopedLock::ScopedLock(ScopedLock&& other) noexcept
{
    mutex_ = nullptr;
    *this = std::move(other);
}

ScopedLock& ScopedLock::operator=(ScopedLock&& other) noexcept
{
    if (this != &other) {
        if (mutex_) {
            mutex_->Unlock();
        }
        mutex_ = other.mutex_;
        other.mutex_ = nullptr;
    }
    return *this;
}

void ScopedLock::Lock()
{
    if (mutex_ != nullptr) {
        mutex_->Lock();
    }
}

bool ScopedLock::TryLock()
{
    bool ret = false;
    if (mutex_ != nullptr) {
        ret = mutex_->TryLock();
    }
    return ret;
}

void ScopedLock::Unlock()
{
    if (mutex_ != nullptr) {
        mutex_->Unlock();
    }
}
} // namespace OSAL
} // namespace Media
} // namespace OHOS