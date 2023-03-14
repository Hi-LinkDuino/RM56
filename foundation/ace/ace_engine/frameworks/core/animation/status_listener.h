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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_STATUS_LISTENER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_STATUS_LISTENER_H

#include <unordered_map>

#include "base/log/log.h"
#include "base/utils/event_callback.h"

namespace OHOS::Ace {

class StatusListenable {
public:
    using StatusCallback = EventCallback<void()>;

    StatusListenable() = default;
    virtual ~StatusListenable() = default;

    void AddStartListener(const StatusCallback& callback)
    {
        startCallbacks_.emplace(callback.GetId(), callback);
    }

    void AddStopListener(const StatusCallback& callback)
    {
        stopCallbacks_.emplace(callback.GetId(), callback);
    }

    void AddRepeatListener(const StatusCallback& callback)
    {
        repeatCallbacks_.emplace(callback.GetId(), callback);
    }

    void AddPauseListener(const StatusCallback& callback)
    {
        pauseCallbacks_.emplace(callback.GetId(), callback);
    }

    void AddResumeListener(const StatusCallback& callback)
    {
        resumeCallbacks_.emplace(callback.GetId(), callback);
    }

    void AddIdleListener(const StatusCallback& callback)
    {
        idleCallbacks_.emplace(callback.GetId(), callback);
    }

    void AddPrepareListener(const StatusCallback& callback)
    {
        prepareCallbacks_.emplace(callback.GetId(), callback);
    }

    template<class F>
    StatusCallback::IdType AddStartListener(const F& funcObject)
    {
        StatusCallback callback(funcObject);
        AddStartListener(callback);
        return callback.GetId();
    }

    template<class F>
    StatusCallback::IdType AddStopListener(const F& funcObject)
    {
        StatusCallback callback(funcObject);
        AddStopListener(callback);
        return callback.GetId();
    }

    template<class F>
    StatusCallback::IdType AddRepeatListener(const F& funcObject)
    {
        StatusCallback callback(funcObject);
        AddRepeatListener(callback);
        return callback.GetId();
    }

    template<class F>
    StatusCallback::IdType AddPauseListener(const F& funcObject)
    {
        StatusCallback callback(funcObject);
        AddPauseListener(callback);
        return callback.GetId();
    }

    template<class F>
    StatusCallback::IdType AddResumeListener(const F& funcObject)
    {
        StatusCallback callback(funcObject);
        AddResumeListener(callback);
        return callback.GetId();
    }

    template<class F>
    StatusCallback::IdType AddIdleListener(const F& funcObject)
    {
        StatusCallback callback(funcObject);
        AddIdleListener(callback);
        return callback.GetId();
    }

    template<class F>
    StatusCallback::IdType AddPrepareListener(const F& funcObject)
    {
        StatusCallback callback(funcObject);
        AddPrepareListener(callback);
        return callback.GetId();
    }

    void RemoveStartListener(const StatusCallback& callback)
    {
        startCallbacks_.erase(callback.GetId());
    }

    void RemoveStartListener(typename StatusCallback::IdType id)
    {
        startCallbacks_.erase(id);
    }

    void RemoveStopListener(const StatusCallback& callback)
    {
        stopCallbacks_.erase(callback.GetId());
    }

    void RemoveStopListener(typename StatusCallback::IdType id)
    {
        stopCallbacks_.erase(id);
    }

    void RemoveRepeatListener(const StatusCallback& callback)
    {
        repeatCallbacks_.erase(callback.GetId());
    }

    void RemoveRepeatListener(typename StatusCallback::IdType id)
    {
        repeatCallbacks_.erase(id);
    }

    void RemovePauseListener(const StatusCallback& callback)
    {
        pauseCallbacks_.erase(callback.GetId());
    }

    void RemovePauseListener(typename StatusCallback::IdType id)
    {
        pauseCallbacks_.erase(id);
    }

    void RemoveResumeListener(const StatusCallback& callback)
    {
        resumeCallbacks_.erase(callback.GetId());
    }

    void RemoveResumeListener(typename StatusCallback::IdType id)
    {
        resumeCallbacks_.erase(id);
    }

    void RemoveIdleListener(const StatusCallback& callback)
    {
        idleCallbacks_.erase(callback.GetId());
    }

    void RemoveIdleListener(typename StatusCallback::IdType id)
    {
        idleCallbacks_.erase(id);
    }

    void RemovePrepareListener(const StatusCallback& callback)
    {
        prepareCallbacks_.erase(callback.GetId());
    }

    void RemovePrepareListener(typename StatusCallback::IdType id)
    {
        prepareCallbacks_.erase(id);
    }

    void ClearStartListeners()
    {
        startCallbacks_.clear();
    }

    void ClearStopListeners()
    {
        stopCallbacks_.clear();
    }

    void ClearRepeatListeners()
    {
        repeatCallbacks_.clear();
    }

    void ClearPauseListeners()
    {
        pauseCallbacks_.clear();
    }

    void ClearResumeListeners()
    {
        resumeCallbacks_.clear();
    }

    void ClearIdleListeners()
    {
        idleCallbacks_.clear();
    }

    void ClearPrepareListeners()
    {
        prepareCallbacks_.clear();
    }

    void ClearAllListeners()
    {
        ClearStartListeners();
        ClearStopListeners();
        ClearRepeatListeners();
        ClearPauseListeners();
        ClearResumeListeners();
        ClearIdleListeners();
        ClearPrepareListeners();
    }

    void NotifyStartListener() const
    {
        for (auto&& [id, callback] : startCallbacks_) {
            if (callback) {
                callback();
            }
        }
    }

    void NotifyStopListener() const
    {
        for (auto&& [id, callback] : stopCallbacks_) {
            if (callback) {
                callback();
            }
        }
    }

    void NotifyRepeatListener() const
    {
        for (auto&& [id, callback] : repeatCallbacks_) {
            if (callback) {
                callback();
            }
        }
    }

    void NotifyPauseListener() const
    {
        for (auto&& [id, callback] : pauseCallbacks_) {
            if (callback) {
                callback();
            }
        }
    }

    void NotifyResumeListener() const
    {
        for (auto&& [id, callback] : resumeCallbacks_) {
            if (callback) {
                callback();
            }
        }
    }

    void NotifyIdleListener() const
    {
        for (auto&& [id, callback] : idleCallbacks_) {
            if (callback) {
                callback();
            }
        }
    }

    void NotifyPrepareListener() const
    {
        for (auto&& [id, callback] : prepareCallbacks_) {
            if (callback) {
                callback();
            }
        }
    }

private:
    std::unordered_map<StatusCallback::IdType, StatusCallback> startCallbacks_;
    std::unordered_map<StatusCallback::IdType, StatusCallback> stopCallbacks_;
    std::unordered_map<StatusCallback::IdType, StatusCallback> pauseCallbacks_;
    std::unordered_map<StatusCallback::IdType, StatusCallback> resumeCallbacks_;
    std::unordered_map<StatusCallback::IdType, StatusCallback> repeatCallbacks_;
    std::unordered_map<StatusCallback::IdType, StatusCallback> idleCallbacks_;
    std::unordered_map<StatusCallback::IdType, StatusCallback> prepareCallbacks_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_STATUS_LISTENER_H
