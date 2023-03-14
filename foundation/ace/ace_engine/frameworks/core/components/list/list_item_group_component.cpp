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

#include "core/components/list/list_item_group_component.h"

#include "core/components/list/list_item_group_element.h"
#include "core/components/list/render_list_item_group.h"

namespace OHOS::Ace {

ListItemGroupComponent::ListItemGroupComponent(const std::string& type)
    : ListItemComponent(type, RefPtr<Component>()) {}

RefPtr<Element> ListItemGroupComponent::CreateElement()
{
    return AceType::MakeRefPtr<ListItemGroupElement>();
}

RefPtr<RenderNode> ListItemGroupComponent::CreateRenderNode()
{
    return RenderListItemGroup::Create();
}

} // namespace OHOS::Ace