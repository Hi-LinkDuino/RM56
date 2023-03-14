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

#include "core/components/ability_component/resource/ability_component_delegate.h"

#include "base/log/log.h"
#include "core/event/ace_event_helper.h"
#include "core/event/back_end_event_manager.h"
#include "frameworks/bridge/js_frontend/frontend_delegate_impl.h"

namespace OHOS::Ace {
namespace {

constexpr char ABILITY_COMPONENT_RESOURCE_NAME[] = "abilitycomponent";

constexpr char ABILITY_COMPONENT_METHOD_UPDATE_RENDER_RECT[] = "updateRenderRect";
constexpr char ABILITY_COMPONENT_METHOD_START_ABILITY[] = "startAbility";
constexpr char ABILITY_COMPONENT_METHOD_PERFORM_BACK_PRESS[] = "performBackPress";
constexpr char ABILITY_COMPONENT_METHOD_GET_STACK_COUNT[] = "getStackCount";

constexpr char ABILITY_COMPONENT_EVENT_READY[] = "onReady";
constexpr char ABILITY_COMPONENT_EVENT_DESTROY[] = "onDestroy";
constexpr char ABILITY_COMPONENT_EVENT_ABILITY_CREATED[] = "onAbilityCreated";
constexpr char ABILITY_COMPONENT_EVENT_ABILITY_MOVED_FRONT[] = "onAbilityMovedFront";
constexpr char ABILITY_COMPONENT_EVENT_ABILITY_WILL_REMOVE[] = "onAbilityWillRemove";

constexpr char NTC_PARAM_LEFT[] = "left";
constexpr char NTC_PARAM_TOP[] = "top";
constexpr char NTC_PARAM_WIDTH[] = "width";
constexpr char NTC_PARAM_HEIGHT[] = "height";
constexpr char NTC_PARAM_WANT[] = "want";
constexpr char NTC_PARAM_ABILITY_COMPONENT[] = "abilitycomponent";

} // namespace

AbilityComponentDelegate::AbilityComponentDelegate(
    const RefPtr<AbilityComponent>& abilityComponent, const WeakPtr<PipelineContext>& context, const std::string& type)
    : AbilityComponentResource(type, context), abilityComponent_(abilityComponent), state_(State::WAITING_FOR_SIZE)
{
    InitControllerImpl();
}

AbilityComponentDelegate::~AbilityComponentDelegate()
{
    OnDestroy();
    ReleasePlatformResource();
}

void AbilityComponentDelegate::InitControllerImpl()
{
    if (!abilityComponent_) {
        return;
    }
    auto controller = abilityComponent_->GetController();
    if (!controller) {
        return;
    }
    controller->SetStartAbilityImpl([weak = WeakClaim(this)](const std::string& want) {
        auto delegate = weak.Upgrade();
        if (delegate) {
            delegate->StartAbility(want);
        }
    });
    controller->SetPerformBackPressImpl([weak = WeakClaim(this)]() {
        auto delegate = weak.Upgrade();
        if (delegate) {
            delegate->PerformBackPress();
        }
    });
    controller->SetGetStackCountImpl([weak = WeakClaim(this)]() {
        auto delegate = weak.Upgrade();
        if (delegate) {
            return delegate->GetStackCount();
        }
        return 0;
    });
}

void AbilityComponentDelegate::ReleasePlatformResource()
{
    Stop();
    Release();
}

void AbilityComponentDelegate::Stop()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context when stop");
        return;
    }
    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    if (platformTaskExecutor.IsRunOnCurrentThread()) {
        UnregisterEvent();
    } else {
        platformTaskExecutor.PostTask([weak = WeakClaim(this)] {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->UnregisterEvent();
            }
        });
    }
}

void AbilityComponentDelegate::RegisterEvent()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto resRegister = context->GetPlatformResRegister();
    if (!resRegister) {
        return;
    }
    resRegister->RegisterEvent(
        MakeEventHash(ABILITY_COMPONENT_EVENT_READY), [weak = WeakClaim(this)](const std::string& param) {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->OnReady();
            }
        });
    resRegister->RegisterEvent(
        MakeEventHash(ABILITY_COMPONENT_EVENT_DESTROY), [weak = WeakClaim(this)](const std::string& param) {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->OnDestroy();
            }
        });
    resRegister->RegisterEvent(
        MakeEventHash(ABILITY_COMPONENT_EVENT_ABILITY_CREATED), [weak = WeakClaim(this)](const std::string& param) {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->OnAbilityCreated(param);
            }
        });
    resRegister->RegisterEvent(
        MakeEventHash(ABILITY_COMPONENT_EVENT_ABILITY_MOVED_FRONT), [weak = WeakClaim(this)](const std::string& param) {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->OnAbilityMovedFront();
            }
        });
    resRegister->RegisterEvent(
        MakeEventHash(ABILITY_COMPONENT_EVENT_ABILITY_WILL_REMOVE), [weak = WeakClaim(this)](const std::string& param) {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->OnAbilityWillRemove();
            }
        });
}

void AbilityComponentDelegate::UnregisterEvent()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context when unregister event");
        return;
    }
    auto resRegister = context->GetPlatformResRegister();
    if (!resRegister) {
        LOGE("fail to get resRegister when unregister event");
        return;
    }
    resRegister->UnregisterEvent(MakeEventHash(ABILITY_COMPONENT_EVENT_READY));
    resRegister->UnregisterEvent(MakeEventHash(ABILITY_COMPONENT_EVENT_DESTROY));
    resRegister->UnregisterEvent(MakeEventHash(ABILITY_COMPONENT_EVENT_ABILITY_CREATED));
    resRegister->UnregisterEvent(MakeEventHash(ABILITY_COMPONENT_EVENT_ABILITY_MOVED_FRONT));
    resRegister->UnregisterEvent(MakeEventHash(ABILITY_COMPONENT_EVENT_ABILITY_WILL_REMOVE));
}

void AbilityComponentDelegate::UpdateRenderRect(const Rect& rect)
{
    if (id_ == INVALID_ID) {
        return;
    }

    hash_ = MakeResourceHash();
    Method updateRenderRectMethod = MakeMethodHash(ABILITY_COMPONENT_METHOD_UPDATE_RENDER_RECT);
    std::stringstream paramStream;
    paramStream << NTC_PARAM_LEFT << ABILITY_COMPONENT_PARAM_EQUALS << (int32_t)rect.Left()
                << ABILITY_COMPONENT_PARAM_AND << NTC_PARAM_TOP << ABILITY_COMPONENT_PARAM_EQUALS << (int32_t)rect.Top()
                << ABILITY_COMPONENT_PARAM_AND << NTC_PARAM_WIDTH << ABILITY_COMPONENT_PARAM_EQUALS
                << (int32_t)rect.Width() << ABILITY_COMPONENT_PARAM_AND << NTC_PARAM_HEIGHT
                << ABILITY_COMPONENT_PARAM_EQUALS << (int32_t)rect.Height();
    std::string param = paramStream.str();
    CallResRegisterMethod(updateRenderRectMethod, param, false);
}

void AbilityComponentDelegate::CreatePlatformResource(const WeakPtr<PipelineContext>& context, const Rect& rect)
{
    context_ = context;
    CreatePluginResource(context, rect);
}

void AbilityComponentDelegate::CreatePluginResource(const WeakPtr<PipelineContext>& context, const Rect& rect)
{
    state_ = State::CREATING;

    if (!abilityComponent_) {
        state_ = State::CREATE_FAILED;
        return;
    }

    auto pipelineContext = context.Upgrade();
    if (!pipelineContext) {
        state_ = State::CREATE_FAILED;
        return;
    }
    context_ = context;
    auto platformTaskExecutor =
        SingleTaskExecutor::Make(pipelineContext->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    auto resRegister = pipelineContext->GetPlatformResRegister();
    auto weakRes = AceType::WeakClaim(AceType::RawPtr(resRegister));
    platformTaskExecutor.PostTask([weak = WeakClaim(this), weakRes, rect] {
        auto delegate = weak.Upgrade();
        if (!delegate) {
            return;
        }
        auto component = delegate->abilityComponent_;
        if (!component) {
            return;
        }
        auto resRegister = weakRes.Upgrade();
        if (!resRegister) {
            return;
        }
        auto context = delegate->context_.Upgrade();
        if (!context) {
            return;
        }

        delegate->id_ = CREATING_ID;

        std::stringstream paramStream;
        paramStream << NTC_PARAM_ABILITY_COMPONENT << ABILITY_COMPONENT_PARAM_EQUALS << delegate->id_
                    << ABILITY_COMPONENT_PARAM_AND << NTC_PARAM_LEFT << ABILITY_COMPONENT_PARAM_EQUALS
                    << (int32_t)rect.Left() << ABILITY_COMPONENT_PARAM_AND << NTC_PARAM_TOP
                    << ABILITY_COMPONENT_PARAM_EQUALS << (int32_t)rect.Top() << ABILITY_COMPONENT_PARAM_AND
                    << NTC_PARAM_WIDTH << ABILITY_COMPONENT_PARAM_EQUALS << (int32_t)rect.Width()
                    << ABILITY_COMPONENT_PARAM_AND << NTC_PARAM_HEIGHT << ABILITY_COMPONENT_PARAM_EQUALS
                    << (int32_t)rect.Height() << ABILITY_COMPONENT_PARAM_AND << NTC_PARAM_WANT
                    << ABILITY_COMPONENT_PARAM_EQUALS << component->GetWant();

        std::string param = paramStream.str();
        delegate->id_ = resRegister->CreateResource(ABILITY_COMPONENT_RESOURCE_NAME, param);
        if (delegate->id_ == INVALID_ID) {
            return;
        }
        delegate->state_ = State::CREATED;
        delegate->hash_ = delegate->MakeResourceHash();
        delegate->RegisterEvent();
    });
}

void AbilityComponentDelegate::OnReady() const
{
    auto context = context_.Upgrade();
    if (!context || !abilityComponent_ || abilityComponent_->GetOnReady() == nullptr) {
        return;
    }
    auto onReady = *abilityComponent_->GetOnReady();
    context->GetTaskExecutor()->PostTask(
        [onReady]() {
            if (onReady) {
                onReady();
            }
        },
        TaskExecutor::TaskType::JS);
}

void AbilityComponentDelegate::OnDestroy() const
{
    auto context = context_.Upgrade();
    if (!context || !abilityComponent_ || abilityComponent_->GetOnDestroy() == nullptr) {
        return;
    }
    auto onDestroy = *abilityComponent_->GetOnDestroy();
    context->GetTaskExecutor()->PostTask(
        [onDestroy]() {
            if (onDestroy) {
                onDestroy();
            }
        },
        TaskExecutor::TaskType::JS);
}

void AbilityComponentDelegate::OnAbilityCreated(const std::string& param) const
{
    auto context = context_.Upgrade();
    if (!context || !abilityComponent_ || abilityComponent_->GetOnAbilityCreated() == nullptr) {
        return;
    }
    auto onAbilityCreated = *abilityComponent_->GetOnAbilityCreated();
    context->GetTaskExecutor()->PostTask(
        [onAbilityCreated, param]() {
            if (onAbilityCreated) {
                onAbilityCreated(param);
            }
        },
        TaskExecutor::TaskType::JS);
}

void AbilityComponentDelegate::OnAbilityMovedFront() const
{
    auto context = context_.Upgrade();
    if (!context || !abilityComponent_ || abilityComponent_->GetOnAbilityMovedFront() == nullptr) {
        return;
    }
    auto onAbilityMovedFront = *abilityComponent_->GetOnAbilityMovedFront();
    context->GetTaskExecutor()->PostTask(
        [onAbilityMovedFront]() {
            if (onAbilityMovedFront) {
                onAbilityMovedFront();
            }
        },
        TaskExecutor::TaskType::JS);
}

void AbilityComponentDelegate::OnAbilityWillRemove() const
{
    auto context = context_.Upgrade();
    if (!context || !abilityComponent_ || abilityComponent_->GetOnAbilityWillRemove() == nullptr) {
        return;
    }
    auto onAbilityWillRemove = *abilityComponent_->GetOnAbilityWillRemove();
    context->GetTaskExecutor()->PostTask(
        [onAbilityWillRemove]() {
            if (onAbilityWillRemove) {
                onAbilityWillRemove();
            }
        },
        TaskExecutor::TaskType::JS);
}

void AbilityComponentDelegate::StartAbility(const std::string& want)
{
    if (id_ == INVALID_ID) {
        return;
    }

    hash_ = MakeResourceHash();
    Method startAbilityMethod = MakeMethodHash(ABILITY_COMPONENT_METHOD_START_ABILITY);
    std::stringstream paramStream;
    paramStream << NTC_PARAM_WANT << ABILITY_COMPONENT_PARAM_EQUALS << want;
    std::string param = paramStream.str();
    CallResRegisterMethod(startAbilityMethod, param, false);
}

void AbilityComponentDelegate::PerformBackPress()
{
    if (id_ == INVALID_ID) {
        return;
    }

    hash_ = MakeResourceHash();
    Method performBackPressMethod = MakeMethodHash(ABILITY_COMPONENT_METHOD_PERFORM_BACK_PRESS);
    CallResRegisterMethod(performBackPressMethod, "", false);
}

int32_t AbilityComponentDelegate::GetStackCount()
{
    if (id_ == INVALID_ID) {
        return 0;
    }

    hash_ = MakeResourceHash();
    Method getStackCountMethod = MakeMethodHash(ABILITY_COMPONENT_METHOD_GET_STACK_COUNT);
    auto result = CallResRegisterMethod(getStackCountMethod, "", true);
    return StringUtils::StringToInt(result);
}

} // namespace OHOS::Ace
