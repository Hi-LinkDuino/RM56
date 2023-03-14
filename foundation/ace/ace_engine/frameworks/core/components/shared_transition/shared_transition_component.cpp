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

#include "core/components/shared_transition/shared_transition_component.h"

#include "base/utils/system_properties.h"
#include "core/components/shared_transition/shared_transition_element.h"

namespace OHOS::Ace {

SharedTransitionComponent::SharedTransitionComponent(const ComposeId& id, const std::string& name, ShareId shareId)
    : ComposedComponent(id, name), shareId_(std::move(shareId))
{
    InitEnableStatus();
}

void SharedTransitionComponent::SetOption(const TweenOption& option)
{
    option_ = option;
}

const TweenOption& SharedTransitionComponent::GetOption() const
{
    return option_;
}

const ShareId& SharedTransitionComponent::GetShareId() const
{
    return shareId_;
}

RefPtr<Element> SharedTransitionComponent::CreateElement()
{
    return AceType::MakeRefPtr<SharedTransitionElement>(GetId());
}

void SharedTransitionComponent::SetEffect(RefPtr<SharedTransitionEffect>& effect)
{
    if (effect) {
        effect_ = effect;
    } else {
        effect_ = SharedTransitionEffect::GetSharedTransitionEffect(
            SharedTransitionEffectType::SHARED_EFFECT_EXCHANGE, shareId_);
    }
}

const RefPtr<SharedTransitionEffect>& SharedTransitionComponent::GetEffect() const
{
    return effect_;
}

void SharedTransitionComponent::InitEnableStatus()
{
    // In the watch scene, drag to return to the previous page, and disable shared element transitions
    if (SystemProperties::GetDeviceType() == DeviceType::WATCH) {
        enablePopEnter_ = false;
        enablePopExit_ = false;
    }
}

bool SharedTransitionComponent::IsEnablePopEnter() const
{
    return enablePopEnter_;
}

void SharedTransitionComponent::SetEnablePopEnter(bool enablePopEnter)
{
    enablePopEnter_ = enablePopEnter;
}

bool SharedTransitionComponent::IsEnablePushEnter() const
{
    return enablePushEnter_;
}

void SharedTransitionComponent::SetEnablePushEnter(bool enablePushEnter)
{
    enablePushEnter_ = enablePushEnter;
}

bool SharedTransitionComponent::IsEnablePopExit() const
{
    return enablePopExit_;
}

void SharedTransitionComponent::SetEnablePopExit(bool enablePopExit)
{
    enablePopExit_ = enablePopExit;
}

bool SharedTransitionComponent::IsEnablePushExit() const
{
    return enablePushExit_;
}

void SharedTransitionComponent::SetEnablePushExit(bool enablePushExit)
{
    enablePushExit_ = enablePushExit;
}

void SharedTransitionComponent::SetShareId(const ShareId& shareId)
{
    shareId_ = shareId;
}

void SharedTransitionComponent::SetOpacity(float opacity)
{
    opacity_ = opacity;
}

float SharedTransitionComponent::GetOpacity() const
{
    return opacity_;
}

void SharedTransitionComponent::SetZIndex(int32_t index)
{
    zIndex_ = index;
}

int32_t SharedTransitionComponent::GetZIndex() const
{
    return zIndex_;
}

} // namespace OHOS::Ace