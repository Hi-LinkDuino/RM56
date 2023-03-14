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

#include "core/components_v2/indexer/popup_list_component.h"

#include "core/components_v2/indexer/popup_list_element.h"
#include "core/components_v2/indexer/render_popup_list.h"

namespace OHOS::Ace::V2 {
RefPtr<Element> PopupListComponent::CreateElement()
{
    return AceType::MakeRefPtr<PopupListElement>();
}

RefPtr<RenderNode> PopupListComponent::CreateRenderNode()
{
    return RenderPopupList::Create();
}

void PopupListComponent::BuildBackground()
{
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetFlex(BoxFlex::FLEX_NO);
    boxComponent->SetAlignment(Alignment::CENTER);
    boxComponent->SetWidth(POPUP_ZERO_SIZE, DimensionUnit::VP);
    boxComponent->SetHeight(POPUP_ZERO_SIZE, DimensionUnit::VP);

    Border border;
    Radius radiusTop = Radius(Dimension(POPUP_BOX_RADIUS_SIZE, DimensionUnit::VP));
    Radius radiusBottom = Radius(Dimension(POPUP_BORDER_RADIUS_SIZE, DimensionUnit::VP));
    border.SetTopLeftRadius(radiusTop);
    border.SetTopRightRadius(radiusTop);
    border.SetBottomLeftRadius(radiusBottom);
    border.SetBottomRightRadius(radiusBottom);

    RefPtr<Decoration> boxDecoration = AceType::MakeRefPtr<Decoration>();
    boxDecoration->SetBorder(border);
    boxDecoration->SetBackgroundColor(Color(POPUP_BG_COLOR).BlendOpacity(POPUP_BG_OPACITY));
    boxComponent->SetBackDecoration(boxDecoration);

    AppendChild(boxComponent);
}
} // namespace OHOS::Ace::V2
