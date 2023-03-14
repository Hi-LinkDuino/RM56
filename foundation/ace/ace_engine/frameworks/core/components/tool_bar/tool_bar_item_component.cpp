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

#include "core/components/tool_bar/tool_bar_item_component.h"

#include "base/log/log.h"
#include "core/components/tool_bar/render_tool_bar_item.h"
#include "core/components/tool_bar/tool_bar_item_element.h"

namespace OHOS::Ace {

RefPtr<RenderNode> ToolBarItemComponent::CreateRenderNode()
{
    return RenderToolBarItem::Create();
}

RefPtr<Element> ToolBarItemComponent::CreateElement()
{
    return AceType::MakeRefPtr<ToolBarItemElement>();
}

} // namespace OHOS::Ace
