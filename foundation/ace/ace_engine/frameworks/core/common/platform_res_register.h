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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_PLATFORM_RES_REGISTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_PLATFORM_RES_REGISTER_H

#include <string>
#include <unordered_map>

#include "base/log/log.h"
#include "base/memory/referenced.h"

namespace OHOS::Ace {

class PipelineContext;

class PlatformResRegister : public Referenced {
public:
    using EventCallback = std::function<void(const std::string&)>;

    ~PlatformResRegister() override = default;

    void RegisterEvent(const std::string& eventId, const EventCallback&& eventCallback)
    {
        eventMap_[eventId] = std::move(eventCallback);
    }

    void UnregisterEvent(const std::string& eventId)
    {
        eventMap_.erase(eventId);
    }

    void OnEvent(const std::string& eventId, const std::string& param)
    {
        auto event = eventMap_.find(eventId);
        if (event != eventMap_.end() && event->second) {
            event->second(param);
        } else {
            LOGW("failed to find eventId = %{public}s", eventId.c_str());
        }
    }

    virtual bool OnMethodCall(const std::string& method, const std::string& param, std::string& result) = 0;
    virtual int64_t CreateResource(const std::string& resourceType, const std::string& param) = 0;
    virtual bool ReleaseResource(const std::string& resourceHash) = 0;

    void SetPipelineContext(const WeakPtr<PipelineContext>& pipelineContext)
    {
        pipelineContext_ = pipelineContext;
    }

    WeakPtr<PipelineContext> GetPipelineContext() const
    {
        return pipelineContext_;
    }

private:
    std::unordered_map<std::string, EventCallback> eventMap_;
    WeakPtr<PipelineContext> pipelineContext_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_PLATFORM_RES_REGISTER_H
