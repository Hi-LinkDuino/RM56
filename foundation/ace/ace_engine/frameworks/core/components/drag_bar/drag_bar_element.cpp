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

#include "core/components/drag_bar/drag_bar_element.h"

#include "core/components/drag_bar/render_drag_bar.h"

namespace OHOS::Ace {

void DragBarElement::OnFocus()
{
    FocusNode::OnFocus();
    if (renderNode_) {
        renderNode_->ChangeStatus(RenderStatus::FOCUS);
    }
}

void DragBarElement::OnBlur()
{
    FocusNode::OnBlur();
    if (renderNode_) {
        renderNode_->ChangeStatus(RenderStatus::BLUR);
    }
}

bool DragBarElement::OnKeyEvent(const KeyEvent& keyEvent)
{
    if ((keyEvent.code == KeyCode::KEY_ENTER) || (keyEvent.code == KeyCode::KEY_NUMPAD_ENTER) ||
        (keyEvent.code == KeyCode::KEY_SPACE)) {
        auto dragBar = AceType::DynamicCast<RenderDragBar>(renderNode_);
        if (dragBar) {
            dragBar->HandleClick(Offset());
        }
    }
    return FocusNode::OnKeyEvent(keyEvent);
}

} // namespace OHOS::Ace