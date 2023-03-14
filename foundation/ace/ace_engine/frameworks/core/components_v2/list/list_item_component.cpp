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

#include "core/components_v2/list/list_item_component.h"

#include "core/components_v2/list/list_item_element.h"
#include "core/components_v2/list/render_list_item.h"

namespace OHOS::Ace::V2 {

RefPtr<RenderNode> ListItemComponent::CreateRenderNode()
{
    return RenderListItem::Create();
}

RefPtr<Element> ListItemComponent::CreateElement()
{
    return AceType::MakeRefPtr<ListItemElement>();
}

RefPtr<ListItemComponent> ListItemComponent::FindListItem(const RefPtr<Component>& component)
{
    RefPtr<ListItemComponent> itemComponent;
    RefPtr<Component> item = component;
    while (item) {
        itemComponent = AceType::DynamicCast<ListItemComponent>(item);
        if (itemComponent) {
            break;
        }
        auto parent = AceType::DynamicCast<SingleChild>(item);
        if (!parent) {
            break;
        }
        item = parent->GetChild();
    }
    return itemComponent;
}

uint32_t ListItemComponent::Compare(const RefPtr<Component> &component) const
{
    auto listItem = AceType::DynamicCast<ListItemComponent>(component);
    if (!listItem) {
        return static_cast<uint32_t>(UpdateRenderType::LAYOUT);
    }
    uint32_t updateRenderType = static_cast<uint32_t>(UpdateRenderType::NONE);
    updateRenderType |= static_cast<uint32_t>(listItem->GetEditMode() == propEditMode_ ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateRenderType |= static_cast<uint32_t>(listItem->GetSticky() == propSticky_ ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    return updateRenderType;
}
} // namespace OHOS::Ace::V2
