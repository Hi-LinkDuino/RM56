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

#include "core/components/plugin/resource/plugin_manager_delegate.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "base/log/log.h"
#include "frameworks/base/json/json_util.h"

#ifdef OHOS_STANDARD_SYSTEM
#include "frameworks/core/components/plugin/resource/plugin_callback_client.h"
#endif

namespace OHOS::Ace {
namespace {
constexpr char PLUGIN_EVENT_ON_PLUGIN_COMPLETE[] = "onPluginComplete";
constexpr char PLUGIN_EVENT_ON_UPDATE_PLUGIN[] = "onUpdatePlugin";
constexpr char PLUGIN_EVENT_ON_ERROR[] = "onPluginError";
constexpr char PLUGIN_ADAPTOR_RESOURCE_NAME[] = "pluginAdaptor";
constexpr char NTC_PARAM_RICH_TEXT[] = "pluginAdaptor";
} // namespace

PluginManagerDelegate::~PluginManagerDelegate()
{
}

void PluginManagerDelegate::UnregisterEvent()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context when unregister event");
        return;
    }
    auto resRegister = context->GetPlatformResRegister();
    resRegister->UnregisterEvent(MakeEventHash(PLUGIN_EVENT_ON_PLUGIN_COMPLETE));
    resRegister->UnregisterEvent(MakeEventHash(PLUGIN_EVENT_ON_UPDATE_PLUGIN));
    resRegister->UnregisterEvent(MakeEventHash(PLUGIN_EVENT_ON_ERROR));
}

void PluginManagerDelegate::RegisterEvent()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("register event error due null context, will not receive plugin manager event");
        return;
    }
    auto resRegister = context->GetPlatformResRegister();
    resRegister->RegisterEvent(
        MakeEventHash(PLUGIN_EVENT_ON_PLUGIN_COMPLETE), [weak = WeakClaim(this)](const std::string& param) {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->OnPluginComplete(param);
            }
        });
    resRegister->RegisterEvent(
        MakeEventHash(PLUGIN_EVENT_ON_UPDATE_PLUGIN), [weak = WeakClaim(this)](const std::string& param) {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->OnPluginUpdate(param);
            }
        });
    resRegister->RegisterEvent(
        MakeEventHash(PLUGIN_EVENT_ON_ERROR), [weak = WeakClaim(this)](const std::string& param) {
        auto delegate = weak.Upgrade();
        if (delegate) {
            delegate->OnPluginError(param);
        }
    });
}

void PluginManagerDelegate::CreatePlatformResource(
    const WeakPtr<PipelineContext>& context, const RequestPluginInfo& info)
{
    context_ = context;
    state_ = State::CREATING;

    auto pipelineContext = context.Upgrade();
    if (!pipelineContext) {
        state_ = State::CREATEFAILED;
        OnPluginError("internal error");
        return;
    }
    auto platformTaskExecutor =
        SingleTaskExecutor::Make(pipelineContext->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    auto resRegister = pipelineContext->GetPlatformResRegister();
    auto weakRes = AceType::WeakClaim(AceType::RawPtr(resRegister));
    platformTaskExecutor.PostTask([weak = WeakClaim(this), weakRes, info] {
        auto delegate = weak.Upgrade();
        if (!delegate) {
            LOGE("delegate is null");
            return;
        }
        auto resRegister = weakRes.Upgrade();
        auto context = delegate->context_.Upgrade();
        if (!resRegister || !context) {
            return;
        }

        delegate->id_ = CREATING_ID;

        std::stringstream paramStream;
        paramStream << NTC_PARAM_RICH_TEXT << PLUGIN_MANAGER_PARAM_EQUALS << delegate->id_ << PLUGIN_MANAGER_PARAM_AND
                    << "bundle" << PLUGIN_MANAGER_PARAM_EQUALS << info.bundleName << PLUGIN_MANAGER_PARAM_AND
                    << "ability" << PLUGIN_MANAGER_PARAM_EQUALS << info.abilityName << PLUGIN_MANAGER_PARAM_AND
                    << "module" << PLUGIN_MANAGER_PARAM_EQUALS << info.moduleName << PLUGIN_MANAGER_PARAM_AND
                    << "name" << PLUGIN_MANAGER_PARAM_EQUALS << info.pluginName << PLUGIN_MANAGER_PARAM_AND
                    << "dimension" << PLUGIN_MANAGER_PARAM_EQUALS << info.dimension << PLUGIN_MANAGER_PARAM_AND
                    << "id" << PLUGIN_MANAGER_PARAM_EQUALS << info.id << PLUGIN_MANAGER_PARAM_AND;

        std::string param = paramStream.str();
        delegate->id_ = resRegister->CreateResource(PLUGIN_ADAPTOR_RESOURCE_NAME, param);
        if (delegate->id_ == INVALID_ID) {
            return;
        }
        delegate->state_ = State::CREATED;
        delegate->hash_ = delegate->MakeResourceHash();
        delegate->RegisterEvent();
    });
    OnPluginComplete("Complete");
}

void PluginManagerDelegate::AddPlugin(const WeakPtr<PipelineContext>& context, const RequestPluginInfo& info)
{
    LOGI("PluginManagerDelegate::AddPlugin");
    CreatePlatformResource(context, info);
}

void PluginManagerDelegate::AddPluginCompleteCallback(const OnPluginCompleteCallback& callback)
{
    LOGI("PluginManagerDelegate::AddPluginCompleteCallback");
    if (!callback || state_ == State::RELEASED) {
        LOGE("callback is null or has released");
        return;
    }
    onPluginCompleteCallback_ = callback;
}

void PluginManagerDelegate::AddPluginUpdateCallback(const OnPluginUpdateCallback& callback)
{
    if (!callback || state_ == State::RELEASED) {
        LOGE("callback is null or has released");
        return;
    }
    onPluginUpdateCallback_ = callback;
}

void PluginManagerDelegate::AddPluginErrorCallback(const OnPluginErrorCallback& callback)
{
    if (!callback || state_ == State::RELEASED) {
        LOGE("callback is null or has released");
        return;
    }
    onPluginErrorCallback_ = callback;
}

void PluginManagerDelegate::OnActionEvent(const std::string& action)
{
    auto eventAction = JsonUtil::ParseJsonString(action);
    if (!eventAction->IsValid()) {
        LOGE("get event action failed");
        return;
    }
    auto actionType = eventAction->GetValue("action");
    if (!actionType->IsValid()) {
        LOGE("get event key failed");
        return;
    }

    auto type = actionType->GetString();
    if (type != "router" && type != "message") {
        LOGE("undefined event type");
        return;
    }
}

void PluginManagerDelegate::OnPluginComplete(const std::string& param)
{
    LOGI("PluginManagerDelegate::OnPluginComplete");
    auto result = ParseMapFromString(param);
    if (onPluginCompleteCallback_) {
        LOGI("PluginManagerDelegate::OnPluginComplete");
        onPluginCompleteCallback_();
    }
}

void PluginManagerDelegate::OnPluginUpdate(const std::string& param)
{
    auto result = ParseMapFromString(param);
    if (onPluginUpdateCallback_) {
        onPluginUpdateCallback_(StringUtils::StringToLongInt(result["pluginId"]), result["data"]);
    }
}

void PluginManagerDelegate::OnPluginError(const std::string& param)
{
    auto result = ParseMapFromString(param);
    if (onPluginErrorCallback_) {
        if (result["code"].empty()) {
            result["code"] = "-1";
            result["msg"] = param;
        }
        onPluginErrorCallback_(result["code"], result["msg"]);
    }
}
} // namespace OHOS::Ace