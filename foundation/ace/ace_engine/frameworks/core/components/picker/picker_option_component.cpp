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

#include "core/components/picker/picker_option_component.h"

#include "core/components/picker/picker_option_element.h"
#include "core/components/picker/render_picker_option.h"

namespace OHOS::Ace {
namespace {

constexpr uint32_t PICKER_OPTION_TEXT_MAX_LINES = 1;

} // namespace

PickerOptionComponent::PickerOptionComponent()
{
    textComponent_ = AceType::MakeRefPtr<TextComponent>("");
    boxComponent_ = AceType::MakeRefPtr<BoxComponent>();
}

RefPtr<RenderNode> PickerOptionComponent::CreateRenderNode()
{
    return AceType::MakeRefPtr<RenderPickerOption>();
}

RefPtr<Element> PickerOptionComponent::CreateElement()
{
    auto element = AceType::MakeRefPtr<PickerOptionElement>();
    element->SetAutoAccessibility(false);
    return element;
}

void PickerOptionComponent::Initialize()
{
    if (!theme_) {
        LOGE("Theme is null.");
        return;
    }
    auto style = GetDisappear() ? theme_->GetDisappearOptionStyle() : theme_->GetOptionStyle(GetSelected(), false);
    auto isRtl = GetTextDirection() == TextDirection::RTL;
    style.SetMaxLines(PICKER_OPTION_TEXT_MAX_LINES);
    style.SetTextOverflow(TextOverflow::ELLIPSIS);
    textComponent_->SetData(text_);
    textComponent_->SetFocusColor(style.GetTextColor());
    textComponent_->SetTextStyle(style);
    textComponent_->SetTextDirection(GetTextDirection());

    boxComponent_->SetDeliverMinToChild(false);
    if (GetSelected()) {
        boxComponent_->SetBackDecoration(theme_->GetOptionDecoration(false));
    } else {
        auto back = AceType::MakeRefPtr<Decoration>();
        boxComponent_->SetBackDecoration(back);
    }
    boxComponent_->SetAlignment(Alignment::CENTER);
    boxComponent_->SetChild(textComponent_);
    boxComponent_->SetTextDirection(GetTextDirection());
    if (isRtl) {
        boxComponent_->SetAlignment(Alignment::CENTER_RIGHT);
    }
    SetChild(boxComponent_);
}

} // namespace OHOS::Ace