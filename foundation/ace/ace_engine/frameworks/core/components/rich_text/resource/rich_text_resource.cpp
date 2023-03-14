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

#include "core/components/rich_text/resource/rich_text_resource.h"

#include <sstream>

#include "base/log/log.h"

namespace OHOS::Ace {

const char RICHTEXT_PARAM_NONE[] = "";
const char RICHTEXT_PARAM_AND[] = "#HWJS-&-#";
const char RICHTEXT_PARAM_VALUE[] = "value";
const char RICHTEXT_PARAM_EQUALS[] = "#HWJS-=-#";
const char RICHTEXT_PARAM_BEGIN[] = "#HWJS-?-#";
const char RICHTEXT_METHOD[] = "method";
const char RICHTEXT_EVENT[] = "event";
const char RICHTEXT_RESULT_FAIL[] = "fail";

void RichTextResource::Release(const std::function<void(bool)>& onRelease)
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
            LOGW("rich text resource has released");
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

double RichTextResource::GetDoubleParam(const std::string& param, const std::string& name) const
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

int32_t RichTextResource::GetIntParam(const std::string& param, const std::string& name) const
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

std::string RichTextResource::MakeResourceHash() const
{
    std::stringstream hashCode;
    hashCode << type_ << "@" << id_;

    return hashCode.str();
}

std::string RichTextResource::MakeEventHash(const std::string& event) const
{
    std::string eventHash = hash_;

    eventHash += std::string(RICHTEXT_EVENT);
    eventHash += std::string(RICHTEXT_PARAM_EQUALS);
    eventHash += event;
    eventHash += std::string(RICHTEXT_PARAM_BEGIN);

    return eventHash;
}

std::string RichTextResource::MakeMethodHash(const std::string& method) const
{
    std::string methodHash = hash_;

    methodHash += std::string(RICHTEXT_METHOD);
    methodHash += std::string(RICHTEXT_PARAM_EQUALS);
    methodHash += method;
    methodHash += std::string(RICHTEXT_PARAM_BEGIN);

    return methodHash;
}

void RichTextResource::OnError(const std::string& errorCode, const std::string& errorMsg)
{
    if (onError_) {
        onError_(errorCode, errorMsg);
    }
}

void RichTextResource::CallResRegisterMethod(const std::string& method, const std::string& param,
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
    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(),
                                                         TaskExecutor::TaskType::PLATFORM);

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

std::string RichTextResource::GetStringParam(const std::string& param, const std::string& name) const
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