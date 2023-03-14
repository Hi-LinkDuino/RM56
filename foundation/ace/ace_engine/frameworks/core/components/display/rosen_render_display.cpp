/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components/display/rosen_render_display.h"

#include "render_service_client/core/animation/rs_transition.h"
#include "render_service_client/core/ui/rs_node.h"

#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {

void RosenRenderDisplay::Update(const RefPtr<Component>& component)
{
    RenderDisplay::Update(component);
    if (auto rsNode = GetRSNode()) {
        rsNode->SetAlpha(opacity_ / 255.0);
        if (pendingAppearingTransition_ && hasAppearTransition_) {
            OnRSTransition(TransitionType::APPEARING);
            pendingAppearingTransition_ = false;
        }
    }
}

void RosenRenderDisplay::OnHiddenChanged(bool hidden)
{
    RenderDisplay::OnHiddenChanged(hidden);
    if (auto rsNode = GetRSNode()) {
        rsNode->SetVisible(RenderDisplay::GetVisible() && !GetHidden());
    }
}

void RosenRenderDisplay::OnVisibleChanged()
{
    if (auto rsNode = GetRSNode()) {
        rsNode->SetVisible(RenderDisplay::GetVisible() && !GetHidden());
    }
}

void RosenRenderDisplay::OnOpacityAnimationCallback()
{
    double value = animatableOpacity_.GetValue();
    opacity_ = static_cast<uint8_t>(round(value * UINT8_MAX));

    if (auto rsNode = GetRSNode()) {
        rsNode->SetAlpha(value);
    }
}

void RosenRenderDisplay::Paint(RenderContext& context, const Offset& offset)
{
    if (visible_ == VisibleType::VISIBLE) {
        RenderNode::Paint(context, offset);
    }
    if (auto rsNode = static_cast<RosenRenderContext*>(&context)->GetRSNode()) {
        rsNode->SetVisible(RenderDisplay::GetVisible());
    }
}

void RosenRenderDisplay::OnRSTransition(TransitionType type)
{
    if (GetRSNode() == nullptr) {
        return;
    }
    if (type == TransitionType::APPEARING && hasAppearTransition_) {
        GetRSNode()->NotifyTransition(Rosen::RSTransitionEffect::OPACITY, true);
    } else if (type == TransitionType::DISAPPEARING && hasDisappearTransition_) {
        GetRSNode()->NotifyTransition(Rosen::RSTransitionEffect::OPACITY, false);
    }
}

void RosenRenderDisplay::OnRemove()
{
    if (auto rsNodePtr = GetRSNode()) {
        rsNodePtr->SetAlpha(1);
        rsNodePtr->SetVisible(true);
    }
    RenderDisplay::OnRemove();
}

void RosenRenderDisplay::ClearRenderObject()
{
    if (auto rsNode = GetRSNode()) {
        rsNode->SetAlpha(1);
        rsNode->SetVisible(true);
    }
    RenderDisplay::ClearRenderObject();
    pendingAppearingTransition_ = false;
}
} // namespace OHOS::Ace