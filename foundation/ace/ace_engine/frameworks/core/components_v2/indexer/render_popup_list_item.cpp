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

#include "core/components_v2/indexer/render_popup_list_item.h"

#include "core/components/button/button_component.h"
#include "core/components/image/image_component.h"

namespace OHOS::Ace::V2 {
RefPtr<RenderNode> RenderPopupListItem::Create()
{
    return AceType::MakeRefPtr<RenderPopupListItem>();
}

void RenderPopupListItem::Update(const RefPtr<Component>& component)
{
    component_ = AceType::DynamicCast<PopupListItemComponent>(component);
    if (!component_) {
        LOGW("Not PopupListItemComponent, nothing to do");
        return;
    }

    boxComponent_ = AceType::DynamicCast<BoxComponent>(component_->GetChildren().front());
    if (!boxComponent_) {
        LOGW("Not BoxComponent, nothing to do");
        return;
    }

    textComponent_ = AceType::DynamicCast<TextComponent>(boxComponent_->GetChild());
    if (!textComponent_) {
        LOGW("Not TextComponent, nothing to do");
        return;
    }

    // create render node
    if (!renderBox_) {
        renderBox_ = AceType::DynamicCast<RenderBox>(RenderBox::Create());
    }
    AddChild(renderBox_);
    renderBox_->Attach(GetContext());
    renderBox_->Update(boxComponent_);

    if (!renderText_) {
        renderText_ = AceType::DynamicCast<RenderText>(RenderText::Create());
    }
    renderBox_->AddChild(renderText_);
    renderText_->Attach(GetContext());
    renderText_->Update(textComponent_);

    MarkNeedLayout();
}

void RenderPopupListItem::PerformLayout()
{
    if (GetChildren().empty()) {
        LOGE("RenderPopupListItem not items");
        return;
    }

    if (!renderBox_) {
        LOGE("RenderPopupListItem RenderBox is nullptr!");
        return;
    }

    LayoutParam boxLayout = GetLayoutParam();
    boxLayout.SetMinSize(Size(0.0, 0.0));
    renderBox_->Layout(boxLayout);
    renderBox_->SetPosition(Offset::Zero());

    auto width = renderBox_->GetLayoutSize().Width();
    auto height = renderBox_->GetLayoutSize().Height();
    Size layoutSizeAfterConstrain = GetLayoutParam().Constrain(Size(width, height));
    SetLayoutSize(layoutSizeAfterConstrain);
}

void RenderPopupListItem::UpdateBoxSelected()
{
    if (!renderBox_ || !boxComponent_) {
        LOGE("box is nullptr");
        return;
    }

    boxComponent_->SetColor(Color::GRAY);
    boxComponent_->SetDecorationUpdateFlag(true);
    renderBox_->Update(boxComponent_);
    renderBox_->PerformLayout();
}

void RenderPopupListItem::UpdateBoxNormal()
{
    if (!renderBox_ || !boxComponent_) {
        LOGE("box is nullptr");
        return;
    }

    boxComponent_->SetColor(Color::TRANSPARENT);
    boxComponent_->SetDecorationUpdateFlag(true);
    renderBox_->Update(boxComponent_);
    renderBox_->PerformLayout();
}
} // namespace OHOS::Ace::V2
