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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_BACK_END_EVENT_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_BACK_END_EVENT_MANAGER_H

#include <functional>
#include <list>
#include <mutex>
#include <unordered_map>

#include "base/log/log.h"
#include "base/utils/singleton.h"
#include "core/event/ace_event_handler.h"

namespace OHOS::Ace {

// Note that this class is not thread-safe, and is processed on the UI thread of ACE uniformly.
// Uses BackEndEventManager to get available id.
// This class is only used by BackEndEventManager.
class ACE_EXPORT BackEndEventIdManager final : public Singleton<BackEndEventIdManager> {
    DECLARE_SINGLETON(BackEndEventIdManager);

public:
    std::string GetAvailableId();
};


// Note that this class is not thread-safe, and is processed on the UI thread of ACE uniformly.
// Need to use the void return type event, if you need to return the result, use the reference out parameter.
template<class>
class BackEndEventManager;

template<class... Args>
class BackEndEventManager<void(Args...)> final : public Singleton<BackEndEventManager<void(Args...)>> {
    DECLARE_SINGLETON(BackEndEventManager<void(Args...)>);

public:
    // Gets the globally unique event ID of the backend for subsequent event binding.
    EventMarker GetAvailableMarker() const
    {
        // For back end event, the page id is -1.
        constexpr int32_t pageId = -1;
        return EventMarker(BackEndEventIdManager::GetInstance().GetAvailableId(), "", pageId, false);
    }

    void BindBackendEvent(const EventMarker& marker, const std::function<void(Args...)>& event)
    {
        LOGD("the bind event id is %{public}s, the manager is %{private}p", marker.GetData().eventId.c_str(), this);

        bool isSuccess = false;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto result = eventMap_.try_emplace(marker.GetData().eventId, event);
            isSuccess = result.second;
        }
        if (!isSuccess) {
            LOGE("fail to bind back end event due to event id is duplicate!");
        }
    }

    void FireBackEndEvent(const EventMarker& marker, Args&&... args)
    {
        LOGD("the Fire event id is %{public}s, the manager is %{private}p", marker.GetData().eventId.c_str(), this);
        std::function<void(Args...)> func;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto iter = eventMap_.find(marker.GetData().eventId);
            if (iter != eventMap_.end()) {
                func = iter->second;
            }
        }
        if (!func) {
            LOGE("fail to trigger back end event due to no such event!");
            return;
        }
        func(std::forward<Args>(args)...);
    }

    // When the event does not need to be used, release the corresponding event ID
    void RemoveBackEndEvent(const EventMarker& marker)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        eventMap_.erase(marker.GetData().eventId);
    }

private:
    std::mutex mutex_;
    std::unordered_map<std::string, std::function<void(Args...)>> eventMap_;
};

template<class... Args>
BackEndEventManager<void(Args...)>::BackEndEventManager() = default;

template<class... Args>
BackEndEventManager<void(Args...)>::~BackEndEventManager() = default;

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_BACK_END_EVENT_MANAGER_H
