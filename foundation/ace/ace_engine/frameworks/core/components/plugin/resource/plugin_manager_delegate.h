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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_RESOURCE_PLUGIN_MANAGER_DELEGATE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_RESOURCE_PLUGIN_MANAGER_DELEGATE_H

#include <list>

#include "core/components/common/layout/constants.h"
#include "core/components/plugin/resource/plugin_manager_resource.h"
#include "core/components/plugin/resource/plugin_request_data.h"

#ifdef OHOS_STANDARD_SYSTEM
#include "ohos/aafwk/content/want.h"
#endif

namespace OHOS::Ace {
class PluginManagerDelegate : public PluginManagerResource {
    DECLARE_ACE_TYPE(PluginManagerDelegate, PluginManagerResource);

public:
    using OnPluginCompleteCallback = std::function<void()>;
    using OnPluginUpdateCallback = std::function<void(int64_t, const std::string&)>;
    using OnPluginErrorCallback = std::function<void(const std::string&, const std::string&)>;

    enum class State: char {
        WAITINGFORSIZE,
        CREATING,
        CREATED,
        CREATEFAILED,
        RELEASED,
    };

    PluginManagerDelegate() = delete;
    ~PluginManagerDelegate() override;
    explicit PluginManagerDelegate(const WeakPtr<PipelineContext>& context)
        : PluginManagerResource("pluginAdaptor", context), state_(State::WAITINGFORSIZE) {}

    void AddPlugin(const WeakPtr<PipelineContext>& context, const RequestPluginInfo& info);
    void AddPluginCompleteCallback(const OnPluginCompleteCallback& layoutChangeCallback);
    void AddPluginUpdateCallback(const OnPluginUpdateCallback& layoutChangeCallback);
    void AddPluginErrorCallback(const OnPluginErrorCallback& layoutChangeCallback);

    void OnActionEvent(const std::string& action);
    void OnPluginComplete(const std::string& param);
    void OnPluginUpdate(const std::string& param);
    void OnPluginError(const std::string& param);

#ifdef OHOS_STANDARD_SYSTEM
    void ProcessPluginUninstall(const int64_t PluginId);
    void OnDeathReceived() {}
#endif

private:
    void CreatePlatformResource(const WeakPtr<PipelineContext>& context, const RequestPluginInfo& info);
    void RegisterEvent();
    void UnregisterEvent();

    OnPluginCompleteCallback onPluginCompleteCallback_;
    OnPluginUpdateCallback onPluginUpdateCallback_;
    OnPluginErrorCallback onPluginErrorCallback_;

    State state_ { State::WAITINGFORSIZE };
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_RESOURCE_PLUGIN_MANAGER_DELEGATE_H
