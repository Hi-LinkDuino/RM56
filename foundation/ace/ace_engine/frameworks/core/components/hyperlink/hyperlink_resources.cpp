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

#include "core/components/hyperlink/hyperlink_resources.h"

#include <sstream>

#include "base/log/log.h"

namespace OHOS::Ace {

const char HYPERLINK_COMPONENT_PARAM_EQUALS[] = "#HWJS-=-#";
const char HYPERLINK_COMPONENT_PARAM_BEGIN[] = "#HWJS-?-#";
const char HYPERLINK_COMPONENT_METHOD[] = "method";
const char HYPERLINK_STARTABILITY[] = "startAbility";
const char HYPERLINK_URI[] = "url";
const char HYPERLINK_PARAM[] = "hyperlink";
const char HYPER_CREATE_PARAM[] = "hyperlink#HWJS-=-#startAbility";

HyperlinkResources::~HyperlinkResources()
{
    ReleasePluginResource();
}

std::string HyperlinkResources::MakeResourceHash() const
{
    std::stringstream hashCode;
    hashCode << HYPERLINK_PARAM << "@" << id_;
    return hashCode.str();
}

std::string HyperlinkResources::MakeMethodHash(const std::string& method) const
{
    // method hash is hyperlink@(id)method#HWJS-=-#(method)#HWJS-?-#
    std::string methodHash = MakeResourceHash();
    methodHash += std::string(HYPERLINK_COMPONENT_METHOD);
    methodHash += std::string(HYPERLINK_COMPONENT_PARAM_EQUALS);
    methodHash += method;
    methodHash += std::string(HYPERLINK_COMPONENT_PARAM_BEGIN);
    return methodHash;
}

std::string HyperlinkResources::CallResRegisterMethod(const std::string& method, const std::string& param)
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
    if (id_ == INVALID_ID) {
        LOGE("fail to get a valid id");
        return "";
    }
    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    auto weakRes = AceType::WeakClaim(AceType::RawPtr(resRegister));

    platformTaskExecutor.PostTask([method, param, weakRes] {
        auto resRegister = weakRes.Upgrade();
        if (resRegister != nullptr) {
            std::string result;
            resRegister->OnMethodCall(method, param, result);
        }
    });
    return "";
}

void HyperlinkResources::StartAbility(const std::string& address)
{
    std::string startAbilityMethod = MakeMethodHash(HYPERLINK_STARTABILITY);
    std::stringstream paramStream;
    paramStream << HYPERLINK_URI << HYPERLINK_COMPONENT_PARAM_EQUALS << address;
    std::string param = paramStream.str();
    CallResRegisterMethod(startAbilityMethod, param);
}

void HyperlinkResources::CreatePlatformResource(const WeakPtr<PipelineContext>& context)
{
    context_ = context;
    CreatePluginResource(context);
}

void HyperlinkResources::CreatePluginResource(const WeakPtr<PipelineContext>& context)
{
    // create AceHyperlinkOhos with id
    auto pipelineContext = context.Upgrade();
    if (!pipelineContext) {
        return;
    }
    context_ = context;
    auto platformTaskExecutor =
        SingleTaskExecutor::Make(pipelineContext->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    auto resRegister = pipelineContext->GetPlatformResRegister();
    auto weakRes = AceType::WeakClaim(AceType::RawPtr(resRegister));
    platformTaskExecutor.PostTask([weak = WeakClaim(this), weakRes] {
        auto hyperlinkResource = weak.Upgrade();
        if (!hyperlinkResource) {
            return;
        }
        auto resRegister = weakRes.Upgrade();
        if (!resRegister) {
            return;
        }
        hyperlinkResource->id_ = resRegister->CreateResource(HYPERLINK_PARAM, HYPER_CREATE_PARAM);
    });
}

void HyperlinkResources::ReleasePluginResource()
{
    // release AceHyperlinkOhos by id
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to release resource due to context is null");
        return;
    }
    auto resRegister = context->GetPlatformResRegister();
    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    auto weakRes = AceType::WeakClaim(AceType::RawPtr(resRegister));
    hash_ = MakeResourceHash();
    auto releaseTask = [id = id_, hash = hash_, weakRes] {
        if (id == INVALID_ID) {
            LOGW("ability component resource has released");
            return;
        }

        auto resRegister = weakRes.Upgrade();
        if (resRegister) {
            resRegister->ReleaseResource(hash);
        }
    };
    if (platformTaskExecutor.IsRunOnCurrentThread()) {
        releaseTask();
    } else {
        platformTaskExecutor.PostTask(releaseTask);
    }
}

} // namespace OHOS::Ace
