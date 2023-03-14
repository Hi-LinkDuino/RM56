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

#include "core/components/ability_component/resource/ability_component_resource.h"

#include <sstream>

#include "base/log/log.h"

namespace OHOS::Ace {

const char ABILITY_COMPONENT_PARAM_AND[] = "#HWJS-&-#";
const char ABILITY_COMPONENT_PARAM_EQUALS[] = "#HWJS-=-#";
const char ABILITY_COMPONENT_PARAM_BEGIN[] = "#HWJS-?-#";
const char ABILITY_COMPONENT_METHOD[] = "method";
const char ABILITY_COMPONENT_EVENT[] = "event";

void AbilityComponentResource::Release(const std::function<void(bool)>& onRelease)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to release resource due to context is null");
        return;
    }

    auto resRegister = context->GetPlatformResRegister();
    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    auto weakRes = AceType::WeakClaim(AceType::RawPtr(resRegister));
    auto releaseTask = [id = id_, hashCode = GetHashCode(), weakRes, onRelease] {
        if (id == INVALID_ID) {
            LOGW("ability component resource has released");
            return;
        }

        auto resRegister = weakRes.Upgrade();
        if (resRegister) {
            bool ret = resRegister->ReleaseResource(hashCode);
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

std::string AbilityComponentResource::MakeResourceHash() const
{
    std::stringstream hashCode;
    hashCode << type_ << "@" << id_;

    return hashCode.str();
}

std::string AbilityComponentResource::MakeEventHash(const std::string& event) const
{
    std::string eventHash = hash_;

    eventHash += std::string(ABILITY_COMPONENT_EVENT);
    eventHash += std::string(ABILITY_COMPONENT_PARAM_EQUALS);
    eventHash += event;
    eventHash += std::string(ABILITY_COMPONENT_PARAM_BEGIN);

    return eventHash;
}

std::string AbilityComponentResource::MakeMethodHash(const std::string& method) const
{
    std::string methodHash = hash_;

    methodHash += std::string(ABILITY_COMPONENT_METHOD);
    methodHash += std::string(ABILITY_COMPONENT_PARAM_EQUALS);
    methodHash += method;
    methodHash += std::string(ABILITY_COMPONENT_PARAM_BEGIN);

    return methodHash;
}

std::string AbilityComponentResource::CallResRegisterMethod(
    const std::string& method, const std::string& param, bool useSyncTask)
{
    if (method.empty()) {
        return "";
    }

    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context to call res register method");
        return "";
    }

    auto resRegister = context->GetPlatformResRegister();
    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    auto weakRes = AceType::WeakClaim(AceType::RawPtr(resRegister));

    if (useSyncTask) {
        std::string result;
        platformTaskExecutor.PostSyncTask([method, param, weakRes, &result] {
            auto resRegister = weakRes.Upgrade();
            if (resRegister != nullptr) {
                resRegister->OnMethodCall(method, param, result);
            }
        });
        return result;
    } else {
        platformTaskExecutor.PostTask([method, param, weakRes] {
            auto resRegister = weakRes.Upgrade();
            if (resRegister != nullptr) {
                std::string result;
                resRegister->OnMethodCall(method, param, result);
            }
        });
    }
    return "";
}

} // namespace OHOS::Ace