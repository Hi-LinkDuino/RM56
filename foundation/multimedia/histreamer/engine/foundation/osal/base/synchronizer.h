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

#ifndef HISTREAMER_FOUNDATION_OSAL_BASE_SYNCHRONIZER_H
#define HISTREAMER_FOUNDATION_OSAL_BASE_SYNCHRONIZER_H

#include <functional>
#include <map>
#include <set>
#include <string>

#include "foundation/log.h"
#include "foundation/osal/thread/condition_variable.h"
#include "foundation/osal/thread/mutex.h"

namespace OHOS {
namespace Media {
namespace OSAL {
template <typename SyncIdType, typename ResultType = void>
class Synchronizer {
public:
    explicit Synchronizer(std::string name) : name_(std::move(name))
    {
    }

    Synchronizer(const Synchronizer<SyncIdType, ResultType>&) = delete;

    Synchronizer<SyncIdType, ResultType>& operator=(const Synchronizer<SyncIdType, ResultType>&) = delete;

    virtual ~Synchronizer() = default;

    void Wait(SyncIdType syncId, const std::function<void()>& asyncOps)
    {
        MEDIA_LOG_I("Synchronizer " PUBLIC_LOG_S " Wait for " PUBLIC_LOG_D32,
                    name_.c_str(), static_cast<int>(syncId));
        if (asyncOps) {
            OSAL::ScopedLock lock(mutex_);
            waitSet_.insert(syncId);
            asyncOps();
            cv_.Wait(lock, [this, syncId] { return syncMap_.find(syncId) != syncMap_.end(); });
            syncMap_.erase(syncId);
        }
    }

    bool WaitFor(SyncIdType syncId, const std::function<void()>& asyncOps, int timeoutMs)
    {
        MEDIA_LOG_I("Synchronizer " PUBLIC_LOG_S " Wait for " PUBLIC_LOG_D32 ", timeout: " PUBLIC_LOG_D32,
                    name_.c_str(), static_cast<int>(syncId), timeoutMs);
        if (!asyncOps) {
            return false;
        }
        OSAL::ScopedLock lock(mutex_);
        waitSet_.insert(syncId);
        asyncOps();
        auto rtv = cv_.WaitFor(lock, timeoutMs, [this, syncId] { return syncMap_.find(syncId) != syncMap_.end(); });
        if (rtv) {
            syncMap_.erase(syncId);
        } else {
            waitSet_.erase(syncId);
        }
        return rtv;
    }

    void Wait(SyncIdType syncId, const std::function<void()>& asyncOps, ResultType& result)
    {
        MEDIA_LOG_I("Synchronizer " PUBLIC_LOG_S " Wait for " PUBLIC_LOG_D32,
                    name_.c_str(), static_cast<int>(syncId));
        if (asyncOps) {
            OSAL::ScopedLock lock(mutex_);
            waitSet_.insert(syncId);
            asyncOps();
            cv_.Wait(lock, [this, syncId] { return syncMap_.find(syncId) != syncMap_.end(); });
            result = syncMap_[syncId];
            syncMap_.erase(syncId);
        }
    }

    bool WaitFor(SyncIdType syncId, const std::function<bool()>& asyncOps, int timeoutMs, ResultType& result)
    {
        MEDIA_LOG_I("Synchronizer " PUBLIC_LOG_S " Wait for " PUBLIC_LOG_D32 ", timeout: " PUBLIC_LOG_D32,
                    name_.c_str(), static_cast<int>(syncId), timeoutMs);
        if (!asyncOps) {
            return false;
        }
        OSAL::ScopedLock lock(mutex_);
        waitSet_.insert(syncId);
        if (!asyncOps()) {
            waitSet_.erase(syncId);
            return false;
        }
        auto rtv = cv_.WaitFor(lock, timeoutMs, [this, syncId] { return syncMap_.find(syncId) != syncMap_.end(); });
        if (rtv) {
            result = syncMap_[syncId];
            syncMap_.erase(syncId);
            MEDIA_LOG_D("Synchronizer " PUBLIC_LOG_S " Wait for " PUBLIC_LOG_D32 " return.", name_.c_str(),
                        static_cast<int>(syncId));
        } else {
            waitSet_.erase(syncId);
        }
        return rtv;
    }

    void Notify(SyncIdType syncId, ResultType result = ResultType())
    {
        MEDIA_LOG_I("Synchronizer " PUBLIC_LOG_S " Notify: " PUBLIC_LOG_D32,
                    name_.c_str(), static_cast<int>(syncId));
        OSAL::ScopedLock lock(mutex_);
        if (waitSet_.find(syncId) != waitSet_.end()) {
            waitSet_.erase(syncId);
            syncMap_.insert({syncId, result});
            cv_.NotifyAll();
        }
    }

private:
    Mutex mutex_;
    ConditionVariable cv_;
    std::string name_;
    std::map<SyncIdType, ResultType> syncMap_;
    std::set<SyncIdType> waitSet_;
};
} // namespace OSAL
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_FOUNDATION_OSAL_BASE_SYNCHRONIZER_H
