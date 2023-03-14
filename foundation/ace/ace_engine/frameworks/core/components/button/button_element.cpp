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

#include "core/components/button/button_element.h"

#include "core/event/key_event.h"

namespace OHOS::Ace {

RefPtr<RenderNode> ButtonElement::CreateRenderNode()
{
    RefPtr<RenderNode> node = ComponentGroupElement::CreateRenderNode();
    if (node) {
        auto wp = AceType::WeakClaim(this);
        node->SetOnChangeCallback([wp]() {
            auto button = wp.Upgrade();
            if (button) {
                button->OnChange();
            }
        });
    }
    return node;
}

void ButtonElement::Update()
{
    RenderElement::Update();

    button_ = AceType::DynamicCast<RenderButton>(renderNode_);
    auto buttonComponent = AceType::DynamicCast<ButtonComponent>(component_);
    if (buttonComponent) {
        SetFocusable(!buttonComponent->GetDisabledState() && buttonComponent->GetFocusable());
    }

    auto labelTarget = AceType::DynamicCast<LabelTarget>(component_);
    if (!labelTarget) {
        // switch is not label target, button are label target
        LOGW("not find label target");
        return;
    }
    auto trigger = labelTarget->GetTrigger();
    if (!trigger) {
        // component not set label trigger
        LOGW("get label trigger failed");
        return;
    }

    auto weak = AceType::WeakClaim(this);
    trigger->clickHandler_ = [weak]() {
        auto button = weak.Upgrade();
        if (button) {
            button->OnClick();
        }
    };
}

void ButtonElement::Apply(const RefPtr<Element>& child)
{
    RenderElement::Apply(child);
    auto buttonComponent = AceType::DynamicCast<ButtonComponent>(component_);
    if (!buttonComponent) {
        return;
    }
    if (buttonComponent->GetAutoFocusState()) {
        auto wp = AceType::WeakClaim(this);
        auto buttonElement = wp.Upgrade();
        if (!buttonElement) {
            return;
        }

        buttonElement->RequestFocus();
    }
}

void ButtonElement::OnFocus()
{
    if (!button_) {
        return;
    }
    button_->HandleFocusEvent(true);
    button_->PlayFocusAnimation(true);
    button_->ChangeStatus(RenderStatus::FOCUS);
}

void ButtonElement::OnClick()
{
    if (!button_) {
        return;
    }
    button_->HandleClickEvent();
    button_->HandleKeyEnterEvent();
}

void ButtonElement::OnClick(const KeyEvent& event)
{
    if (!button_) {
        return;
    }
    ClickInfo info(-1);
    info.SetTimeStamp(event.timeStamp);
    info.SetGlobalLocation(
        Offset((GetRect().Left() + GetRect().Right()) / 2, (GetRect().Top() + GetRect().Bottom()) / 2));
    info.SetLocalLocation(
        Offset((GetRect().Right() - GetRect().Left()) / 2, (GetRect().Bottom() - GetRect().Top()) / 2));
    info.SetSourceDevice(event.sourceType);
    info.SetDeviceId(event.deviceId);
    button_->HandleClickEvent(info);
    button_->HandleKeyEnterEvent(info);
}

void ButtonElement::OnBlur()
{
    if (!button_) {
        return;
    }
    button_->HandleFocusEvent(false);
    button_->PlayFocusAnimation(false);
    button_->ChangeStatus(RenderStatus::BLUR);
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Pipeline context is nullptr");
        return;
    }
    context->CancelFocusAnimation();
    context->CancelShadow();
}

void ButtonElement::OnChange()
{
    if (button_ && IsCurrentFocus()) {
        LOGD("Button OnChange: %{private}p", AceType::RawPtr(button_));
        button_->DisplayFocusAnimation();
    }
}

} // namespace OHOS::Ace