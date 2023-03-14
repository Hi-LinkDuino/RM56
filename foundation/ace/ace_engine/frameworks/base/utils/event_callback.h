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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_EVENT_CALLBACK_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_EVENT_CALLBACK_H

#include <functional>

#include "base/utils/base_id.h"
#include "base/utils/macros.h"

namespace OHOS::Ace {

template<class>
class EventCallback;

template<class R, class... Args>
class EventCallback<R(Args...)> : public BaseId {
public:
    using FunctionType = std::function<R(Args...)>;

    EventCallback() : callback_(nullptr) {}
    explicit EventCallback(FunctionType&& callback) : callback_(std::move(callback)) {}
    explicit EventCallback(const FunctionType& callback) : callback_(callback) {}

    ~EventCallback() override = default;

    // Notice: uses operator bool to judge its legitimacy.
    R operator()(Args&&... args) const
    {
        return callback_(std::forward<Args>(args)...);
    }

    explicit operator bool() const
    {
        return callback_ != nullptr;
    }

    bool operator==(std::nullptr_t) const
    {
        return callback_ == nullptr;
    }

    bool operator!=(std::nullptr_t) const
    {
        return callback_ != nullptr;
    }

    bool operator==(const EventCallback& callback) const
    {
        return GetId() == callback.GetId();
    }

    bool operator<(const EventCallback& callback) const
    {
        return GetId() < callback.GetId();
    }

    const FunctionType& GetCallback() const
    {
        return callback_;
    }

private:
    FunctionType callback_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_EVENT_CALLBACK_H
