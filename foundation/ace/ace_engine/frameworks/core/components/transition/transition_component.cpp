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

#include "core/components/transition/transition_component.h"

#include "core/components/transition/transition_element.h"
#include "core/components/tween/tween_component.h"

namespace OHOS::Ace {
namespace {

const ComposeId PREFIX = "BackendTransition";
uint32_t g_transitionComponentId = 0;

} // namespace

RefPtr<Element> TransitionComponent::CreateElement()
{
    return AceType::MakeRefPtr<TransitionElement>(GetId());
}

void TransitionComponent::SetTransitionOption(const TweenOption& in, const TweenOption& out)
{
    inOption_ = in;
    outOption_ = out;
}

const TweenOption& TransitionComponent::GetTransitionInOption() const
{
    return inOption_;
}

const TweenOption& TransitionComponent::GetTransitionOutOption() const
{
    return outOption_;
}

ComposeId TransitionComponent::AllocTransitionComponentId()
{
    return PREFIX + (std::to_string(g_transitionComponentId++));
}

bool TransitionComponent::IsFirstFrameShow() const
{
    return isFirstFrameShow_;
}

void TransitionComponent::SetIsFirstFrameShow(bool isFirstFrameShow)
{
    isFirstFrameShow_ = isFirstFrameShow;
}

} // namespace OHOS::Ace