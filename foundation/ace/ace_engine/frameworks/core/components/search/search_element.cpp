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

#include "core/components/search/search_element.h"

#include "core/components/search/search_component.h"
#include "core/components/text_field/text_field_element.h"

namespace OHOS::Ace {

void SearchElement::PerformBuild()
{
    RefPtr<SearchComponent> search = AceType::DynamicCast<SearchComponent>(component_);
    if (!search) {
        return;
    }
    const auto& child = children_.empty() ? nullptr : children_.front();
    UpdateChild(child, search->GetChild());
}

bool SearchElement::OnKeyEvent(const KeyEvent& keyEvent)
{
    if ((keyEvent.code == KeyCode::KEY_ENTER || keyEvent.code == KeyCode::KEY_NUMPAD_ENTER) &&
        keyEvent.action == KeyAction::UP) {
        const auto& search = AceType::DynamicCast<RenderSearch>(renderNode_);
        if (search) {
            if (!search->HandleEnterEvent()) {
                const auto& textField = AceType::DynamicCast<TextFieldElement>(GetChildren().front());
                if (textField) {
                    return textField->OnKeyEvent(keyEvent);
                }
            } else {
                return true;
            }
        }
        return false;
    }
    return FocusGroup::OnKeyEvent(keyEvent);
}

bool SearchElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    const auto& renderSearch = AceType::DynamicCast<RenderSearch>(renderNode_);
    if (!renderSearch) {
        return false;
    }
    return renderSearch->HandleFocusEvent(vertical, reverse);
}

} // namespace OHOS::Ace
