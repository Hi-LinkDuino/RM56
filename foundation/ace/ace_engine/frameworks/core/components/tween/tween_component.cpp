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

#include "core/components/tween/tween_component.h"

namespace OHOS::Ace {
namespace {

const char* PREFIX = "BackendTween";
uint32_t g_tweenComponentId = 0;

} // namespace

void TweenComponent::SetTweenOption(const TweenOption& option)
{
    SetOptionCssChanged(true);
    optionCss_ = std::move(option);
}

const TweenOption& TweenComponent::GetTweenOption() const
{
    return optionCss_;
}

void TweenComponent::SetCustomTweenOption(const TweenOption& option)
{
    SetOptionCustomChanged(true);
    optionCustom_ = std::move(option);
}

const TweenOption& TweenComponent::GetCustomTweenOption() const
{
    return optionCustom_;
}

void TweenComponent::SetAnimationOperation(const AnimationOperation& operation)
{
    SetOperationCssChanged(true);
    operationCss_ = operation;
}

const AnimationOperation& TweenComponent::GetAnimationOperation() const
{
    return operationCss_;
}

void TweenComponent::SetCustomAnimationOperation(const AnimationOperation& operation)
{
    SetOperationCustomChanged(true);
    operationCustom_ = operation;
}

const AnimationOperation& TweenComponent::GetCustomAnimationOperation() const
{
    return operationCustom_;
}

RefPtr<Element> TweenComponent::CreateElement()
{
    return AceType::MakeRefPtr<TweenElement>(GetId());
}

ComposeId TweenComponent::AllocTweenComponentId()
{
    return PREFIX + (std::to_string(g_tweenComponentId++));
}

void TweenComponent::SetIsFirstFrameShow(bool isFirstFrameShow)
{
    isFirstFrameShow_ = isFirstFrameShow;
}

bool TweenComponent::GetIsFirstFrameShow() const
{
    return isFirstFrameShow_;
}

void TweenComponent::SetAnimator(const RefPtr<Animator>& animator)
{
    animator_ = animator;
}

RefPtr<Animator> TweenComponent::GetAnimator() const
{
    return animator_;
}

} // namespace OHOS::Ace