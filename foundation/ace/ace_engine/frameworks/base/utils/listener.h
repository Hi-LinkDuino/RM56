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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_LISTENER_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_LISTENER_H

#include <unordered_map>

#include "base/utils/event_callback.h"

namespace OHOS::Ace {

template<class T>
class ValueListenable {
public:
    using ValueCallback = EventCallback<void(const T&)>;

    ValueListenable() = default;
    virtual ~ValueListenable() = default;

    void AddListener(const ValueCallback& callback)
    {
        callbacks_.emplace(callback.GetId(), callback);
    }

    template<class F>
    typename ValueCallback::IdType AddListener(const F& funcObject)
    {
        ValueCallback callback(funcObject);
        AddListener(callback);
        return callback.GetId();
    }

    void RemoveListener(const ValueCallback& callback)
    {
        callbacks_.erase(callback.GetId());
    }

    void RemoveListener(typename ValueCallback::IdType id)
    {
        callbacks_.erase(id);
    }

    void ClearListeners()
    {
        callbacks_.clear();
    }

    void NotifyListener(const T& value) const
    {
        for (auto&& [id, callback] : callbacks_) {
            if (callback) {
                callback(value);
            }
        }
    }

private:
    std::unordered_map<typename ValueCallback::IdType, ValueCallback> callbacks_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_LISTENER_H
