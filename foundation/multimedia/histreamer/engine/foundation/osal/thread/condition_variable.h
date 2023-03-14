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

#ifndef HISTREAMER_FOUNDATION_OSAL_CONDITION_VARIABLE_H
#define HISTREAMER_FOUNDATION_OSAL_CONDITION_VARIABLE_H

#include <cerrno>
#include <ctime>
#include "scoped_lock.h"

namespace OHOS {
namespace Media {
namespace OSAL {
class ConditionVariable {
public:
    ConditionVariable() noexcept;

    virtual ~ConditionVariable() noexcept;

    ConditionVariable(const ConditionVariable& other) = delete;

    ConditionVariable operator=(const ConditionVariable& other) = delete;

    void NotifyOne() noexcept;

    void NotifyAll() noexcept;

    void Wait(ScopedLock& lock) noexcept;

    template <typename Predicate>
    void Wait(ScopedLock& lock, Predicate pred) noexcept
    {
        while (!pred()) {
            Wait(lock);
        }
    }

    bool WaitFor(ScopedLock& lock, int timeoutMs);

    template <typename Predicate>
    bool WaitFor(ScopedLock& lock, int timeoutMs, Predicate pred)
    {
        if (timeoutMs < 0) {
            return false;
        }
        struct timespec timeout = {0, 0};
#ifdef USING_CLOCK_REALTIME
        clock_gettime(CLOCK_REALTIME, &timeout);
#else
        clock_gettime(CLOCK_MONOTONIC, &timeout);
#endif
        timeout.tv_sec += timeoutMs / 1000;              // 1000
        timeout.tv_nsec += (timeoutMs % 1000) * 1000000; // 1000 1000000
        int status = 0;
        while (!pred() && (status == 0)) {
            status = pthread_cond_timedwait(&cond_, &(lock.mutex_->nativeHandle_),
                                            &timeout);
            if (status == ETIMEDOUT) {
                return pred();
            }
        }
        return status == 0;
    }

private:
    bool condInited_;
    pthread_cond_t cond_{};
};
} // namespace OSAL
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_FOUNDATION_OSAL_CONDITION_VARIABLE_H
