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

#include "core/components/plugin/resource/plugin_manager_resource.h"

#include "base/log/log.h"

namespace OHOS::Ace {
const char PLUGIN_MANAGER_PARAM_NONE[] = "";
const char PLUGIN_MANAGER_PARAM_AND[] = "#HWJS-&-#";
const char PLUGIN_MANAGER_PARAM_VALUE[] = "value";
const char PLUGIN_MANAGER_PARAM_EQUALS[] = "#HWJS-=-#";
const char PLUGIN_MANAGER_PARAM_BEGIN[] = "#HWJS-?-#";
const char PLUGIN_MANAGER_METHOD[] = "method";
const char PLUGIN_MANAGER_EVENT[] = "event";
const char PLUGIN_MANAGER_RESULT_FAIL[] = "fail";

void PluginManagerResource::Release(const std::function<void(bool)>& onRelease)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to release resource due to context is null");
        return;
    }

    auto resRegister = context->GetPlatformResRegister();
    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(),
                                                         TaskExecutor::TaskType::PLATFORM);
    auto weakRes = AceType::WeakClaim(AceType::RawPtr(resRegister));
    auto releaseTask = [weak = WeakClaim(this), weakRes, onRelease] {
        auto resource = weak.Upgrade();
        if (!resource || resource->id_ == INVALID_ID) {
            LOGW("plugin manager resource has released");
            return;
        }

        auto resRegister = weakRes.Upgrade();
        if (resRegister) {
            bool ret = resRegister->ReleaseResource(resource->GetHashCode());
            if (ret) {
                resource->Reset();
            }
            if (onRelease) {
                onRelease(ret);
            }
        }
    };
    if (platformTaskExecutor.IsRunOnCurrentThread()) {
        releaseTask();
    } else {
        platformTaskExecutor.PostTask(releaseTask);
    }
}

void PluginManagerResource::CallResRegisterMethod(
    const std::string& method, const std::string& param, const std::function<void(std::string&)>& callback)
{
    if (method.empty()) {
        return;
    }

    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context to call res register method");
        return;
    }

    auto resRegister = context->GetPlatformResRegister();
    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);

    auto weakRes = AceType::WeakClaim(AceType::RawPtr(resRegister));
    platformTaskExecutor.PostTask([method, param, weakRes, callback] {
        auto resRegister = weakRes.Upgrade();
        if (resRegister == nullptr) {
            LOGE("resRegister is nullptr");
            return;
        }
        std::string result;
        resRegister->OnMethodCall(method, param, result);
        if (callback) {
            callback(result);
        }
    });
}

int32_t PluginManagerResource::GetIntParam(const std::string& param, const std::string& name) const
{
    size_t len = name.length();
    size_t pos = param.find(name);
    int32_t result = 0;

    if (pos != std::string::npos) {
        std::stringstream ss;

        ss << param.substr(pos + 1 + len);
        ss >> result;
    }

    return result;
}

std::map<std::string, std::string> PluginManagerResource::ParseMapFromString(const std::string& param)
{
    size_t equalsLen = sizeof(PLUGIN_MANAGER_PARAM_EQUALS) - 1;
    size_t andLen = sizeof(PLUGIN_MANAGER_PARAM_EQUALS) - 1;
    size_t totalLen = param.length();
    size_t index = 0;
    std::map<std::string, std::string> result;
    while (index < totalLen) {
        size_t end = param.find(PLUGIN_MANAGER_PARAM_AND, index);
        if (end == std::string::npos) {
            end = totalLen;
        }

        size_t mid = param.find(PLUGIN_MANAGER_PARAM_EQUALS, index);
        if (mid == std::string::npos) {
            index = end + andLen;
            continue;
        }
        std::string key = param.substr(index, mid - index);
        std::string value = param.substr(mid + equalsLen, end - mid - equalsLen);
        result[key] = value;
        index = end + andLen;
    }
    return result;
}

std::string PluginManagerResource::MakeResourceHash() const
{
    std::stringstream hashCode;
    hashCode << type_ << "@" << id_;

    return hashCode.str();
}

std::string PluginManagerResource::MakeEventHash(const std::string& event) const
{
    std::string eventHash = hash_;

    eventHash += std::string(PLUGIN_MANAGER_EVENT);
    eventHash += std::string(PLUGIN_MANAGER_PARAM_EQUALS);
    eventHash += event;
    eventHash += std::string(PLUGIN_MANAGER_PARAM_BEGIN);

    LOGI("MakeEventHash  hash:%{public}s", eventHash.c_str());

    return eventHash;
}

std::string PluginManagerResource::MakeMethodHash(const std::string& method) const
{
    std::string methodHash = hash_;

    methodHash += std::string(PLUGIN_MANAGER_METHOD);
    methodHash += std::string(PLUGIN_MANAGER_PARAM_EQUALS);
    methodHash += method;
    methodHash += std::string(PLUGIN_MANAGER_PARAM_BEGIN);

    return methodHash;
}

std::string PluginManagerResource::GetStringParam(const std::string& param, const std::string& name) const
{
    size_t len = name.length();
    size_t pos = param.find(name);
    std::string result;

    if (pos != std::string::npos) {
        std::stringstream ss;

        ss << param.substr(pos + 1 + len);
        ss >> result;
    }
    return result;
}
} // namespace OHOS::Ace