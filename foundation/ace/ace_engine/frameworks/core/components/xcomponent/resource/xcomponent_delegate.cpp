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

#include "core/components/xcomponent/resource/xcomponent_delegate.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "base/log/log.h"
#include "core/event/ace_event_helper.h"
#include "core/event/back_end_event_manager.h"
#include "frameworks/bridge/js_frontend/frontend_delegate_impl.h"

namespace OHOS::Ace {
namespace {
constexpr char XCOMPONENT_CREATE[] = "xcomponent";
constexpr char NTC_PARAM_XCOMPONENT[] = "xcomponent";
constexpr char NTC_PARAM_WIDTH[] = "width";
constexpr char NTC_PARAM_HEIGHT[] = "height";
constexpr char NTC_PARAM_LEFT[] = "left";
constexpr char NTC_PARAM_TOP[] = "top";
constexpr char NTC_PARAM_NAME[] = "name";
constexpr char NTC_PARAM_ID[] = "id";
constexpr char NTC_ERROR[] = "create error";
constexpr char XCOMPONENT_ERROR_CODE_CREATEFAIL[] = "error-xcomponent-delegate-000001";
constexpr char XCOMPONENT_ERROR_MSG_CREATEFAIL[] = "create xcomponent_delegate failed.";
} // namespace

XComponentDelegate::~XComponentDelegate()
{
    ReleasePlatformResource();
}

void XComponentDelegate::ReleasePlatformResource()
{
    auto delegate = WeakClaim(this).Upgrade();
    if (delegate) {
        delegate->Release();
    }
}

void XComponentDelegate::CreatePlatformResource(
    const Size& size, const Offset& position, const WeakPtr<PipelineContext>& context)
{
    ReleasePlatformResource();
    CreatePluginResource(size, position, context);
}

void XComponentDelegate::CreatePluginResource(
    const Size& size, const Offset& position, const WeakPtr<PipelineContext>& context)
{
    state_ = State::CREATING;

    auto xcomponentCom = xcomponentComponent_.Upgrade();
    if (!xcomponentCom) {
        state_ = State::CREATEFAILED;
        OnError(NTC_ERROR, "fail to call XComponentDelegate::Create due to xcomponentComponent is null");
        return;
    }

    auto pipelineContext = context.Upgrade();
    if (!pipelineContext) {
        state_ = State::CREATEFAILED;
        OnError(NTC_ERROR, "fail to call XComponentDelegate::Create due to context is null");
        return;
    }
    context_  = context;
    auto platformTaskExecutor = SingleTaskExecutor::Make(pipelineContext->GetTaskExecutor(),
                                                         TaskExecutor::TaskType::PLATFORM);
    auto resRegister = pipelineContext->GetPlatformResRegister();

    platformTaskExecutor.PostTask([this, resRegister, size, position] {
        auto xcomponentCom = this->xcomponentComponent_.Upgrade();
        if (!xcomponentCom) {
            OnError(NTC_ERROR, "fail to call XComponentDelegate::SetSrc PostTask");
        }

        if (!resRegister) {
            if (onError_) {
                onError_(XCOMPONENT_ERROR_CODE_CREATEFAIL, XCOMPONENT_ERROR_MSG_CREATEFAIL);
            }
            return;
        }
        auto context = this->context_.Upgrade();
        if (!context) {
            LOGE("context is null");
            return;
        }

        std::stringstream paramStream;
        paramStream << NTC_PARAM_XCOMPONENT << XCOMPONENT_PARAM_EQUALS
                    << id_ << XCOMPONENT_PARAM_AND
                    << NTC_PARAM_WIDTH << XCOMPONENT_PARAM_EQUALS
                    << size.Width() * context->GetViewScale() << XCOMPONENT_PARAM_AND
                    << NTC_PARAM_HEIGHT << XCOMPONENT_PARAM_EQUALS
                    << size.Height() * context->GetViewScale() << XCOMPONENT_PARAM_AND
                    << NTC_PARAM_LEFT << XCOMPONENT_PARAM_EQUALS
                    << position.GetX() * context->GetViewScale() << XCOMPONENT_PARAM_AND
                    << NTC_PARAM_TOP << XCOMPONENT_PARAM_EQUALS
                    << position.GetY() * context->GetViewScale() << XCOMPONENT_PARAM_AND
                    << NTC_PARAM_NAME << XCOMPONENT_PARAM_EQUALS
                    << xcomponentCom->GetName() << XCOMPONENT_PARAM_AND
                    << NTC_PARAM_ID << XCOMPONENT_PARAM_EQUALS
                    << xcomponentCom->GetId() << XCOMPONENT_PARAM_AND;
        std::string param = paramStream.str();
        id_ = resRegister->CreateResource(XCOMPONENT_CREATE, param);
        if (id_ == X_INVALID_ID) {
            if (onError_) {
                onError_(XCOMPONENT_ERROR_CODE_CREATEFAIL, XCOMPONENT_ERROR_MSG_CREATEFAIL);
            }
            return;
        }
        state_ = State::CREATED;
        hash_ = MakeResourceHash();
    });
}

// upper ui component which inherited from XComponentComponent
// could implement some curtain createdCallback to customized controller interface
void XComponentDelegate::AddCreatedCallback(const CreatedCallback& createdCallback)
{
    ACE_DCHECK(createdCallback != nullptr);
    ACE_DCHECK(state_ != State::RELEASED);
    createdCallbacks_.emplace_back(createdCallback);
}

void XComponentDelegate::RemoveCreatedCallback()
{
    ACE_DCHECK(state_ != State::RELEASED);
    createdCallbacks_.pop_back();
}

void XComponentDelegate::AddReleasedCallback(const ReleasedCallback& releasedCallback)
{
    ACE_DCHECK(releasedCallback != nullptr && state_ != State::RELEASED);
    releasedCallbacks_.emplace_back(releasedCallback);
}

void XComponentDelegate::RemoveReleasedCallback()
{
    ACE_DCHECK(state_ != State::RELEASED);
    releasedCallbacks_.pop_back();
}
} // namespace OHOS::Ace
