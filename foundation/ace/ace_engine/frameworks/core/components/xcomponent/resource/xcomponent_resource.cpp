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

#include "core/components/xcomponent/resource/xcomponent_resource.h"

#include <sstream>

#include "base/log/log.h"

namespace OHOS::Ace {
const char XCOMPONENT_PARAM_INIT[] = "initParam";
const char XCOMPONENT_PARAM_NONE[] = "";
const char XCOMPONENT_PARAM_AND[] = "#HWJS-&-#";
const char XCOMPONENT_PARAM_VALUE[] = "value";
const char XCOMPONENT_PARAM_EQUALS[] = "#HWJS-=-#";
const char XCOMPONENT_PARAM_BEGIN[] = "#HWJS-?-#";
const char XCOMPONENT_METHOD[] = "method";
const char XCOMPONENT_EVENT[] = "event";
const char XCOMPONENT_RESULT_FAIL[] = "fail";

void XComponentResource::Release(const std::function<void(bool)>& onRelease)
{
    if (id_ == X_INVALID_ID) {
        return;
    }

    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to release resource due to context is null");
        return;
    }

    auto resRegister = context->GetPlatformResRegister();
    if (!resRegister) {
        LOGE("fail to release resource due to resRegister is null");
        return;
    }

    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(),
                                                         TaskExecutor::TaskType::PLATFORM);
    auto releaseTask = [this, resRegister, onRelease] {
        bool ret = resRegister->ReleaseResource(hash_);
        if (ret) {
            id_ = X_INVALID_ID;
            hash_.clear();
        }

        if (onRelease) {
            onRelease(ret);
        }
    };
    if (platformTaskExecutor.IsRunOnCurrentThread()) {
        releaseTask();
    } else {
        platformTaskExecutor.PostTask(releaseTask);
    }
}

double XComponentResource::GetDoubleParam(const std::string& param, const std::string& name) const
{
    size_t len = name.length();
    size_t pos = param.find(name);
    double result = 0.0;

    if (pos != std::string::npos) {
        std::stringstream ss;

        ss << param.substr(pos + 1 + len);
        ss >> result;
    }

    return result;
}

int32_t XComponentResource::GetIntParam(const std::string& param, const std::string& name) const
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

std::string XComponentResource::MakeResourceHash() const
{
    std::stringstream hashCode;
    hashCode << type_ << "@" << id_;

    return hashCode.str();
}

std::string XComponentResource::MakeEventHash(const std::string& event) const
{
    std::string eventHash = hash_;

    eventHash += std::string(XCOMPONENT_EVENT);
    eventHash += std::string(XCOMPONENT_PARAM_EQUALS);
    eventHash += event;
    eventHash += std::string(XCOMPONENT_PARAM_BEGIN);

    return eventHash;
}

std::string XComponentResource::MakeMethodHash(const std::string& method) const
{
    std::string methodHash = hash_;

    methodHash += std::string(XCOMPONENT_METHOD);
    methodHash += std::string(XCOMPONENT_PARAM_EQUALS);
    methodHash += method;
    methodHash += std::string(XCOMPONENT_PARAM_BEGIN);

    return methodHash;
}

void XComponentResource::OnError(const std::string& errorCode, const std::string& errorMsg)
{
    if (onError_) {
        onError_(errorCode, errorMsg);
    }
}

void XComponentResource::CallResRegisterMethod(const std::string& method,
                                               const std::string& param,
                                               const std::function<void(std::string&)>& callback)
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
    if (!resRegister) {
        LOGE("fail to CallResRegisterMethod due to resRegister is null");
        return;
    }

    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(),
                                                         TaskExecutor::TaskType::PLATFORM);

    platformTaskExecutor.PostTask([method, param, resRegister, callback] {
        std::string result;
        resRegister->OnMethodCall(method, param, result);
        if (callback) {
            callback(result);
        }
    });
}

std::string XComponentResource::GetStringParam(const std::string& param, const std::string& name) const
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