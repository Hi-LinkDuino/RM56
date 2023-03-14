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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_REF_COUNTER_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_REF_COUNTER_H

#include <atomic>

#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"

namespace OHOS::Ace {

// Reference counter.
class RefCounter {
public:
    virtual int32_t IncStrongRef() = 0;
    virtual int32_t DecStrongRef() = 0;
    virtual int32_t TryIncStrongRef() = 0;
    virtual int32_t StrongRefCount() const = 0;
    virtual int32_t IncWeakRef() = 0;
    virtual int32_t DecWeakRef() = 0;

protected:
    virtual ~RefCounter() = default;
};

// Define thread-safe counter using 'std::atomic' to implement Increase/Decrease count.
class ThreadSafeCounter final {
public:
    explicit ThreadSafeCounter(int32_t count) : count_(count) {}
    ~ThreadSafeCounter() = default;

    int32_t Increase()
    {
        int32_t count = count_.fetch_add(1, std::memory_order_relaxed) + 1;
        ACE_DCHECK(count > 0);
        return count;
    }
    int32_t Decrease()
    {
        int32_t count = count_.fetch_sub(1, std::memory_order_release);
        ACE_DCHECK(count > 0);
        return count - 1;
    }
    int32_t CurrentCount() const
    {
        return count_.load(std::memory_order_relaxed);
    }

    // Try to increase reference count while current value is not zero.
    int32_t TryIncrease()
    {
        int32_t count = CurrentCount();
        do {
            if (count == 0) {
                return 0;
            }
            ACE_DCHECK(count > 0);
        } while (!count_.compare_exchange_weak(count, count + 1, std::memory_order_relaxed));
        return count + 1;
    }

private:
    std::atomic<int32_t> count_ { 0 };

    ACE_DISALLOW_COPY_AND_MOVE(ThreadSafeCounter);
};

// Define thread-unsafe counter.
class ThreadUnsafeCounter final {
public:
    explicit ThreadUnsafeCounter(int32_t count) : count_(count) {}
    ~ThreadUnsafeCounter() = default;

    int32_t Increase()
    {
        int32_t count = ++count_;
        ACE_DCHECK(count > 0);
        return count;
    }
    int32_t Decrease()
    {
        int32_t count = --count_;
        ACE_DCHECK(count >= 0);
        return count;
    }
    int32_t CurrentCount() const
    {
        return count_;
    }

    // Try to increase count while current value is not zero.
    int32_t TryIncrease()
    {
        return CurrentCount() == 0 ? 0 : Increase();
    }

private:
    int32_t count_ { 0 };

    ACE_DISALLOW_COPY_AND_MOVE(ThreadUnsafeCounter);
};

template<class T>
class RefCounterImpl final : public RefCounter {
public:
    static RefCounter* Create()
    {
        return new RefCounterImpl();
    }

    int32_t IncStrongRef() final
    {
        return strongRef_.Increase();
    }
    int32_t DecStrongRef() final
    {
        return strongRef_.Decrease();
    }
    int32_t TryIncStrongRef() final
    {
        return strongRef_.TryIncrease();
    }
    int32_t StrongRefCount() const final
    {
        return strongRef_.CurrentCount();
    }

    int32_t IncWeakRef() final
    {
        return weakRef_.Increase();
    }
    int32_t DecWeakRef() final
    {
        int32_t refCount = weakRef_.Decrease();
        if (refCount == 0) {
            // Release this reference counter, while its weak reference have reduced to zero.
            delete this;
        }
        return refCount;
    }

private:
    T strongRef_ { 0 };
    // Weak reference count should start with 1,
    // because instance MUST hold the reference counter for itself.
    T weakRef_ { 1 };
};

using ThreadSafeRef = RefCounterImpl<ThreadSafeCounter>;
using ThreadUnsafeRef = RefCounterImpl<ThreadUnsafeCounter>;

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_REF_COUNTER_H
