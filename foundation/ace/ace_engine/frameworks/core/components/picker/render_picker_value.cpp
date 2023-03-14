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

#include "core/components/picker/render_picker_value.h"

#include "base/utils/system_properties.h"
#include "core/components/picker/render_picker_option.h"
#include "core/components/stack/stack_element.h"

namespace OHOS::Ace {

RenderPickerValue::RenderPickerValue(const PickerValueCallback& clickCallback)
{
    clickCallback_ = clickCallback;
    clickRecognizer_ = AceType::MakeRefPtr<ClickRecognizer>();
    clickRecognizer_->SetOnClick([weak = WeakClaim(this)](const ClickInfo&) {
        auto refPtr = weak.Upgrade();
        if (refPtr) {
            refPtr->HandleClickCallback();
        }
    });
}

void RenderPickerValue::PerformLayout()
{
    if (GetChildren().empty()) {
        LOGE("No child.");
        return;
    }

    auto child = GetChildren().front();
    child->Layout(GetLayoutParam());
    SetLayoutSize(child->GetLayoutSize());
}

void RenderPickerValue::HandleClickCallback() const
{
    if (!clickCallback_) {
        LOGE("click callback is null.");
        return;
    }
    clickCallback_();
}

void RenderPickerValue::HandleFocus(const RefPtr<RenderBox>& box, bool focus)
{
    if (!box) {
        LOGE("box is null");
        return;
    }

    box_ = box;
    focus_ = focus;
    HandleTextFocus(focus);
    auto boxOldColor = box->GetColor();
    if (focus) {
        if (boxOldColor != focusBackColor_) {
            box->SetColor(focusBackColor_, true);
        }
    } else {
        if (boxOldColor != normalBackColor_) {
            box->SetColor(normalBackColor_, true);
        }
    }
    HandleAnimation(box);
}

void RenderPickerValue::HandleTextFocus(bool focus)
{
    if (GetChildren().empty()) {
        LOGE("No child.");
        return;
    }

    auto option = AceType::DynamicCast<RenderPickerOption>(GetChildren().front());
    if (!option) {
        LOGE("get render picker option failed!");
        return;
    }

    focusBackColor_ = option->GetFocusBackColor();
    focusAnimationColor_ = option->GetFocusAnimationColor();
    normalBackColor_ = option->GetNormalBackColor();
    if (SystemProperties::GetDeviceType() != DeviceType::TV) {
        focusBackColor_ = normalBackColor_;
    }
    if (SystemProperties::GetDeviceType() != DeviceType::PHONE) {
        option->UpdateTextFocus(focus);
    }
}

void RenderPickerValue::HandleAnimation(const RefPtr<RenderBox>& box)
{
    if (!box) {
        LOGE("box is null");
        return;
    }

    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        LOGE("pipeline is null.");
        return;
    }

    box_ = box;
    if (!focus_) {
        LOGD("cancel focus of picker value.");
    } else {
        auto width = box->GetLayoutSize().Width();
        auto height = box->GetLayoutSize().Height();
        RRect rrect;
        rrect.SetRect(Rect(0, 0, width, height));
        auto back = box->GetBackDecoration();
        if (back) {
            auto border = back->GetBorder();
            Corner corner;
            corner.topLeftRadius = border.TopLeftRadius();
            corner.topRightRadius = border.TopRightRadius();
            corner.bottomRightRadius = border.BottomRightRadius();
            corner.bottomLeftRadius = border.BottomLeftRadius();
            rrect.SetCorner(corner);
        }
        pipeline->ShowFocusAnimation(rrect, focusAnimationColor_, box->GetGlobalOffset());
    }
}

} // namespace OHOS::Ace
