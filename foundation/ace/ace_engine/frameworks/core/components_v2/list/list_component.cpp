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

#include "core/components_v2/list/list_component.h"

#include "core/components/foreach/for_each_component.h"
#include "core/components/ifelse/if_else_component.h"
#include "core/components_v2/list/list_element.h"
#include "core/components_v2/list/list_item_component.h"
#include "core/components_v2/list/render_list.h"

namespace OHOS::Ace::V2 {

RefPtr<RenderNode> ListComponent::CreateRenderNode()
{
    return RenderList::Create();
}

RefPtr<Element> ListComponent::CreateElement()
{
    return AceType::MakeRefPtr<ListElement>();
}

uint32_t ListComponent::Compare(const RefPtr<Component>& component) const
{
    auto list = AceType::DynamicCast<ListComponent>(component);
    if (!list) {
        return static_cast<uint32_t>(UpdateRenderType::LAYOUT);
    }
    uint32_t updateType = static_cast<uint32_t>(UpdateRenderType::NONE);
    updateType |= static_cast<uint32_t>(list->GetSpace() == propSpace_ ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(list->GetDirection() == propDirection_ ? UpdateRenderType::NONE :
        UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(list->GetItemDivider()->strokeWidth == itemDivider_->strokeWidth ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(list->GetEditMode() == list->GetEditMode() ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(list->GetItemDivider()->startMargin == itemDivider_->startMargin ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(list->GetItemDivider()->endMargin == itemDivider_->endMargin ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    if (updateType == static_cast<uint32_t>(UpdateRenderType::LAYOUT)) {
        return updateType;
    }
    updateType |= static_cast<uint32_t>(list->GetItemDivider()->color == itemDivider_->color ?
        UpdateRenderType::NONE : UpdateRenderType::PAINT);
    return updateType;
}
} // namespace OHOS::Ace::V2
