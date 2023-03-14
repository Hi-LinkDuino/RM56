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

#include "core/components_v2/indexer/popup_list_item_component.h"

#include "core/components_v2/indexer/popup_list_item_element.h"
#include "core/components_v2/indexer/render_popup_list_item.h"

namespace OHOS::Ace::V2 {
RefPtr<Element> PopupListItemComponent::CreateElement()
{
    return AceType::MakeRefPtr<PopupListItemElement>();
}

RefPtr<RenderNode> PopupListItemComponent::CreateRenderNode()
{
    return RenderPopupListItem::Create();
}

void PopupListItemComponent::BuildItem()
{
    if (!boxComponent_) {
        boxComponent_ = AceType::MakeRefPtr<BoxComponent>();
    }
    boxComponent_->SetFlex(BoxFlex::FLEX_NO);
    boxComponent_->SetAlignment(Alignment::CENTER);
    boxComponent_->SetWidth(POPUP_BOX_SIZE, DimensionUnit::VP);
    boxComponent_->SetHeight(POPUP_BOX_SIZE, DimensionUnit::VP);

    if (!boxDecoration_) {
        boxDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    boxDecoration_->SetBackgroundColor(Color::TRANSPARENT);
    boxComponent_->SetBackDecoration(boxDecoration_);

    if (!boxText_) {
        boxText_ = AceType::MakeRefPtr<TextComponent>(label_);
    }
    textStyle_.SetTextAlign(TextAlign::CENTER);
    textStyle_.SetFontSize(Dimension(POPUP_FONT_SIZE, DimensionUnit::VP));
    textStyle_.SetTextColor(Color::BLACK);
    boxText_->SetTextStyle(textStyle_);
    boxComponent_->SetChild(boxText_);

    AppendChild(boxComponent_);
}
} // namespace OHOS::Ace::V2
