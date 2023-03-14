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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_NODE_LISTENABLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_NODE_LISTENABLE_H

#include <unordered_map>

#include "base/utils/event_callback.h"
#include "base/utils/utils.h"

namespace OHOS::Ace {

class SpringNodeListenable {
public:
    using UpdateCallback = EventCallback<void(const double&, const double&)>; // value and velocity

    void AddUpdateListener(const UpdateCallback& callback)
    {
        updateCallbacks_.emplace(callback.GetId(), callback);
    }

    template<class F>
    UpdateCallback::IdType AddUpdateListener(const F& funcObject)
    {
        UpdateCallback callback(funcObject);
        AddUpdateListener(callback);
        return callback.GetId();
    }

    void NotifyUpdateListener(double value, double velocity) const
    {
        for (auto&& [id, callback] : updateCallbacks_) {
            if (callback) {
                callback(value, velocity);
            }
        }
    }

    void RemoveUpdateListener(typename UpdateCallback::IdType id)
    {
        updateCallbacks_.erase(id);
    }

private:
    std::unordered_map<UpdateCallback::IdType, UpdateCallback> updateCallbacks_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_NODE_LISTENABLE_H
