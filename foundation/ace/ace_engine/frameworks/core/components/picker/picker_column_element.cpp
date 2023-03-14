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

#include "core/components/picker/picker_column_element.h"

#include "core/components/picker/picker_column_component.h"
#include "core/components/picker/render_picker_column.h"

namespace OHOS::Ace {

void PickerColumnElement::PerformBuild()
{
    auto context = context_.Upgrade();
    auto column = AceType::DynamicCast<PickerColumnComponent>(component_);
    if (column && context && context->GetAccessibilityManager()) {
        column->Initialize();
        column->SetRequestFocusCallback([weak = AceType::WeakClaim(this)]() {
            auto element = weak.Upgrade();
            if (element) {
                element->RequestFocus();
            }
        });
    } else {
        LOGE("column is null or can not get accessibility manager.");
    }
    SetFocusable(true);
    ComponentGroupElement::PerformBuild();
    auto render = AceType::DynamicCast<RenderPickerColumn>(renderNode_);
    if (render) {
        render->UpdateRenders();
        render->FlushCurrentOptions();
    } else {
        LOGE("render picker column is null.");
    }
}

void PickerColumnElement::OnClick()
{
    auto render = AceType::DynamicCast<RenderPickerColumn>(renderNode_);
    if (!render) {
        LOGE("render node is null.");
        return;
    }
    render->HandleFinished(true);
}

void PickerColumnElement::OnFocus()
{
    auto render = AceType::DynamicCast<RenderPickerColumn>(renderNode_);
    if (!render) {
        LOGE("render node is null.");
        return;
    }

    render->HandleFocus(true);
}

void PickerColumnElement::OnBlur()
{
    auto render = AceType::DynamicCast<RenderPickerColumn>(renderNode_);
    if (!render) {
        LOGE("render node is null.");
        return;
    }

    render->HandleFocus(false);
}

bool PickerColumnElement::OnKeyEvent(const KeyEvent& keyEvent)
{
    LOGD("key.event.action=%{private}d, key.event.code=%{private}d.", keyEvent.action, keyEvent.code);

    auto render = AceType::DynamicCast<RenderPickerColumn>(renderNode_);
    if (!render) {
        LOGE("render node is null.");
        return false;
    }

    if (keyEvent.action == KeyAction::CLICK) {
        if (keyEvent.code == KeyCode::KEY_DPAD_UP || keyEvent.code == KeyCode::TV_CONTROL_UP) {
            return render->HandleScroll(false);
        }
        if (keyEvent.code == KeyCode::KEY_DPAD_DOWN || keyEvent.code == KeyCode::TV_CONTROL_DOWN) {
            return render->HandleScroll(true);
        }
    }

    if (keyEvent.action == KeyAction::UP) {
        if (keyEvent.code == KeyCode::KEY_ESCAPE || keyEvent.code == KeyCode::KEY_BACK ||
            keyEvent.code == KeyCode::TV_CONTROL_BACK) {
            return render->HandleFinished(false);
        }
    }

    return false;
}

} // namespace OHOS::Ace
