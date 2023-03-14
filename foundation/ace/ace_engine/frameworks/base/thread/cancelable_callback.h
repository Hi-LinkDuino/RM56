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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_THREAD_CANCELABEL_CALLBACK_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_THREAD_CANCELABEL_CALLBACK_H

#include <atomic>
#include <functional>
#include <future>

#include "base/memory/referenced.h"
#include "base/utils/macros.h"

namespace OHOS::Ace {

template<class>
class CancelableCallback;

template<class... V>
class CancelableCallback<void(V...)> final {
public:
    using FunctionType = std::function<void(V...)>;

    CancelableCallback() = default;
    explicit CancelableCallback(const FunctionType& callback)
        : impl_(callback ? Referenced::MakeRefPtr<Callback>(callback) : nullptr)
    {}
    explicit CancelableCallback(FunctionType&& callback)
        : impl_(callback ? Referenced::MakeRefPtr<Callback>(std::move(callback)) : nullptr)
    {}
    ~CancelableCallback() = default;

    void Reset(const FunctionType& callback, bool needCancel = true, bool waitUntilCompleted = false);
    void Reset(FunctionType&& callback, bool needCancel = true, bool waitUntilCompleted = false);
    bool WaitUntilComplete();
    bool Cancel(bool waitUntilCompleted = false);
    void operator()(V&&... values) const;
    operator bool() const
    {
        return impl_ && impl_->callback_;
    }

private:
    enum : int32_t {
        READY,
        CANCELED,
        RUNNING,
        COMPLETED,
    };

    struct Callback final : public Referenced {
        explicit Callback(const FunctionType& callback) : callback_(callback) {}
        explicit Callback(FunctionType&& callback) : callback_(std::move(callback)) {}
        ~Callback() override
        {
            int32_t status = status_.load(std::memory_order_relaxed);
            ACE_DCHECK(status != RUNNING);
            if (status == READY) {
                promise_.set_value(CANCELED);
            }
        }

        FunctionType callback_;
        std::atomic<int32_t> status_ { READY };
        std::promise<int32_t> promise_;
        std::shared_future<int32_t> future_ { promise_.get_future() };
    };

    RefPtr<Callback> impl_;
};

template<class... V>
void CancelableCallback<void(V...)>::Reset(
    const CancelableCallback<void(V...)>::FunctionType& callback, bool needCancel, bool waitUntilCompleted)
{
    if (needCancel) {
        Cancel(waitUntilCompleted);
    }
    impl_ = callback ? Referenced::MakeRefPtr<Callback>(callback) : nullptr;
}

template<class... V>
void CancelableCallback<void(V...)>::Reset(
    CancelableCallback<void(V...)>::FunctionType&& callback, bool needCancel, bool waitUntilCompleted)
{
    if (needCancel) {
        Cancel(waitUntilCompleted);
    }
    impl_ = callback ? Referenced::MakeRefPtr<Callback>(std::move(callback)) : nullptr;
}

template<class... V>
bool CancelableCallback<void(V...)>::WaitUntilComplete()
{
    RefPtr<Callback> impl(std::move(impl_));
    if (!impl) {
        return false;
    }

    switch (impl->status_.load(std::memory_order_relaxed)) {
        case READY:
        case RUNNING: {
            std::shared_future<int32_t> future(impl->future_);
            impl.Reset();
            return future.get() == COMPLETED;
        }
        case COMPLETED:
            return true;
        case CANCELED:
        default:
            return false;
    }
}

template<class... V>
bool CancelableCallback<void(V...)>::Cancel(bool waitUntilCompleted)
{
    RefPtr<Callback> impl(std::move(impl_));
    if (!impl) {
        return true;
    }

    int32_t status = READY;
    if (impl->status_.compare_exchange_strong(status, CANCELED, std::memory_order_relaxed)) {
        impl->promise_.set_value(CANCELED);
        return true;
    } else if (status == CANCELED) {
        return true;
    }

    if (waitUntilCompleted && status == RUNNING) {
        std::shared_future<int32_t> future(impl->future_);
        impl.Reset();
        status = future.get();
        ACE_DCHECK(status == COMPLETED);
    }
    return false;
}

template<class... V>
void CancelableCallback<void(V...)>::operator()(V&&... values) const
{
    RefPtr<Callback> impl(std::move(impl_));
    if (!impl) {
        return;
    }

    int32_t status = READY;
    if (impl->status_.compare_exchange_strong(status, RUNNING, std::memory_order_relaxed)) {
        impl->callback_(std::forward<V>(values)...);
        impl->status_.store(COMPLETED);
        impl->promise_.set_value(COMPLETED);
    }
}

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_THREAD_CANCELABEL_CALLBACK_H
