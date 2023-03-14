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

#include "core/components/video/resource/resource.h"

#include <sstream>

#include "base/log/log.h"

namespace OHOS::Ace {

const char PARAM_NONE[] = "";
const char PARAM_AND[] = "#HWJS-&-#";
const char PARAM_VALUE[] = "value";
const char PARAM_EQUALS[] = "#HWJS-=-#";
const char PARAM_BEGIN[] = "#HWJS-?-#";
const char METHOD[] = "method";
const char EVENT[] = "event";
const char RESULT_FAIL[] = "fail";

void Resource::Release(const std::function<void(bool)>& onRelease)
{
    if (id_ == INVALID_ID) {
        return;
    }

    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to release resource due to context is null");
        return;
    }

    auto resRegister = context->GetPlatformResRegister();
    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    auto weakRes = AceType::WeakClaim(AceType::RawPtr(resRegister));
    auto releaseTask = [weak = AceType::WeakClaim(this), weakRes, onRelease] {
        auto resource = weak.Upgrade();
        auto resRegister = weakRes.Upgrade();
        if (resource == nullptr || resRegister == nullptr) {
            LOGE("resource or resRegister is nullptr");
            return;
        }
        bool ret = resRegister->ReleaseResource(resource->hash_);
        if (ret) {
            resource->id_ = INVALID_ID;
            resource->hash_.clear();
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

double Resource::GetDoubleParam(const std::string& param, const std::string& name) const
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

int32_t Resource::GetIntParam(const std::string& param, const std::string& name) const
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

std::map<std::string, std::string> Resource::ParseMapFromString(const std::string& param)
{
    size_t equalsLen = sizeof(PARAM_EQUALS) - 1;
    size_t andLen = sizeof(PARAM_EQUALS) - 1;
    size_t totalLen = param.length();
    size_t index = 0;
    std::map<std::string, std::string> result;
    while (index < totalLen) {
        size_t end = param.find(PARAM_AND, index);
        if (end == std::string::npos) {
            end = totalLen;
        }

        size_t mid = param.find(PARAM_EQUALS, index);
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

std::string Resource::MakeResourceHash() const
{
    std::stringstream hashCode;
    hashCode << type_ << "@" << id_;

    return hashCode.str();
}

std::string Resource::MakeEventHash(const std::string& event) const
{
    std::string eventHash = hash_;

    eventHash += std::string(EVENT);
    eventHash += std::string(PARAM_EQUALS);
    eventHash += event;
    eventHash += std::string(PARAM_BEGIN);

    return eventHash;
}

std::string Resource::MakeMethodHash(const std::string& method) const
{
    std::string methodHash = hash_;

    methodHash += std::string(METHOD);
    methodHash += std::string(PARAM_EQUALS);
    methodHash += method;
    methodHash += std::string(PARAM_BEGIN);

    return methodHash;
}

bool Resource::IsResultSuccess(const std::string& result) const
{
    size_t pos = result.find(RESULT_FAIL);

    return pos != 0;
}

void Resource::OnError(const std::string& errorCode, const std::string& errorMsg)
{
    if (onError_) {
        onError_(errorCode, errorMsg);
    }
}

void Resource::CallResRegisterMethod(
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

} // namespace OHOS::Ace