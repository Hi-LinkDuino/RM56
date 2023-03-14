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

#include "core/components/bubble/bubble_element.h"

#include "core/components/bubble/bubble_component.h"
#include "core/components/bubble/render_bubble.h"

namespace OHOS::Ace {

void BubbleElement::PerformBuild()
{
    RefPtr<BubbleComponent> bubble = AceType::DynamicCast<BubbleComponent>(component_);
    if (bubble) {
        id_ = bubble->GetId();
        stateChangeEvent_ = bubble->GetStateChangeEvent();
        const auto& child = children_.empty() ? nullptr : children_.front();
        UpdateChild(child, bubble->GetChild());
    }
}

bool BubbleElement::OnKeyEvent(const KeyEvent& keyEvent)
{
    if (keyEvent.code == KeyCode::KEY_BACK && keyEvent.action == KeyAction::UP) {
        const auto& bubble = AceType::DynamicCast<RenderBubble>(renderNode_);
        if (bubble) {
            return bubble->PopBubble();
        }
        return false;
    }
    return FocusNode::OnKeyEvent(keyEvent);
}

bool BubbleElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    return false;
}

void BubbleElement::FirePopEvent()
{
    auto bubble = AceType::DynamicCast<RenderBubble>(renderNode_);
    if (bubble) {
        bubble->FirePopEvent();
    }

    if (IsDeclarative() && stateChangeEvent_) {
        stateChangeEvent_(false);
    }
}

bool BubbleElement::IsDeclarative()
{
    auto context = context_.Upgrade();
    if (!context) {
        return false;
    }

    return context->GetIsDeclarative();
}

} // namespace OHOS::Ace