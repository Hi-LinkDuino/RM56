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

#include "core/components/flex/flex_element.h"

#include "core/components/flex/flex_component.h"

namespace OHOS::Ace {

void FlexElement::Update()
{
    ComponentGroupElement::Update();

    auto flexComponent = AceType::DynamicCast<FlexComponent>(component_);
    if (flexComponent) {
        direction_ = flexComponent->GetDirection();
    }
}

bool FlexElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    if ((direction_ == FlexDirection::COLUMN) != vertical) {
        return false;
    }

    return GoToNextFocus(reverse, rect);
}

bool FlexElement::AcceptFocusByRectOfLastFocus(const Rect& rect)
{
    if (!rect.IsValid()) {
        LOGE("the rect is not valid");
        return false;
    }

    Offset offset;
    auto itNewFocusNode = focusNodes_.end();
    double minVal = std::numeric_limits<double>::max();
    for (auto it = focusNodes_.begin(); it != focusNodes_.end(); ++it) {
        if (!(*it)->IsFocusable()) {
            continue;
        }

        Rect childRect;
        if (!CalculateRect(*it, childRect)) {
            continue;
        }

        Offset vec = childRect.Center() - rect.Center();
        double val = (vec.GetX() * vec.GetX()) + (vec.GetY() * vec.GetY());
        if (minVal > val) {
            minVal = val;
            itNewFocusNode = it;
            offset = childRect.GetOffset();
        }
    }

    if (itNewFocusNode != focusNodes_.end() && (*itNewFocusNode)->AcceptFocusByRectOfLastFocus(rect - offset)) {
        itLastFocusNode_ = itNewFocusNode;
        return true;
    }
    return false;
}

} // namespace OHOS::Ace
