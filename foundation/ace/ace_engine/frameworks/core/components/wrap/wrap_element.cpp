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

#include "core/components/wrap/wrap_element.h"

#include "core/components/wrap/wrap_component.h"

namespace OHOS::Ace {

void WrapElement::Update()
{
    ComponentGroupElement::Update();
    auto wrapComponent = AceType::DynamicCast<WrapComponent>(component_);
    if (wrapComponent != nullptr) {
        direction_ = wrapComponent->GetDirection();
        dialogStretch_ = wrapComponent->GetDialogStretch();
    }
}

bool WrapElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    // (1) When the wrap direction is vertical and focus moves in horizontal, focus cannot move next.
    // (2) When the wrap direction is horizontal and focus moves in vertical, focus cannot move next.
    RefPtr<RenderWrap> renderWrap = AceType::DynamicCast<RenderWrap>(renderNode_);
    auto direction = (dialogStretch_ && renderWrap) ? renderWrap->GetDialogDirection() : direction_;
    if ((direction == WrapDirection::VERTICAL) != vertical) {
        return false;
    }
    // For others, move to next focus node.
    return GoToNextFocus(reverse, rect);
}

} // namespace OHOS::Ace
