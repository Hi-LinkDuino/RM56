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

#include "core/components_v2/indexer/indexer_item_component.h"

#include "core/components/transform/transform_component.h"
#include "core/components_v2/indexer/indexer_component.h"
#include "core/components_v2/indexer/render_indexer_item.h"

namespace OHOS::Ace::V2 {
RefPtr<RenderNode> IndexerItemComponent::CreateRenderNode()
{
    return RenderIndexerItem::Create();
}

void IndexerItemComponent::BuildItem()
{
    box_ = AceType::MakeRefPtr<BoxComponent>();
    if (rotate_) {
        image_ = AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::INDEXER_ARROW_PNG);
        image_->SetHeight(itemSize_);
        image_->SetWidth(itemSize_);
        box_->SetChild(image_);
    } else {
        text_ = AceType::MakeRefPtr<TextComponent>(StringUtils::Str16ToStr8(strLabel_));
        TextStyle textStyle;
        textStyle.SetTextAlign(TextAlign::CENTER);
        textStyle.SetFontWeight(FontWeight::W400);
        text_->SetTextStyle(textStyle);
        box_->SetChild(text_);
    }
    box_->SetFlex(BoxFlex::FLEX_NO);
    box_->SetWidth(itemSize_.Value(), itemSize_.Unit());
    box_->SetHeight(itemSize_.Value(), itemSize_.Unit());
    box_->SetAlignment(Alignment::CENTER);
    // generate background
    RefPtr<Decoration> background = AceType::MakeRefPtr<Decoration>();
    background->SetBackgroundColor(Color::TRANSPARENT);
    // generate circle
    Radius radius = Radius(Dimension(4.0, DimensionUnit::VP));
    background->SetBorderRadius(radius);
    box_->SetBackDecoration(background);
    if (rotate_) {
        RefPtr<TransformComponent> rotate = AceType::MakeRefPtr<TransformComponent>();
        rotate->SetChild(box_);
        SetChild(rotate);
    } else {
        SetChild(box_);
    }
}

void IndexerItemComponent::SetTextStyle(bool active)
{
    if (!text_) {
        LOGE("[indexer] Get child text failed.");
        return;
    }
    if (active) {
        text_->SetTextStyle(activeStyle_);
    } else {
        text_->SetTextStyle(normalStyle_);
    }
}

void IndexerItemComponent::UpdateSize()
{
    if (rotate_) {
        image_->SetHeight(itemSize_);
        image_->SetWidth(itemSize_);
    }
    box_->SetWidth(itemSize_.Value(), itemSize_.Unit());
    box_->SetHeight(itemSize_.Value(), itemSize_.Unit());
}
} // namespace OHOS::Ace::V2