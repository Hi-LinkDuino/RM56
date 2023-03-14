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

#include "core/components/grid/grid_item_component.h"

#include "core/components/grid/grid_item_element.h"
#include "core/components/grid/render_grid_item.h"

namespace OHOS::Ace {

RefPtr<Element> GridItemComponent::CreateElement()
{
    return AceType::MakeRefPtr<GridItemElement>();
}

RefPtr<RenderNode> GridItemComponent::CreateRenderNode()
{
    return RenderGridItem::Create();
}

void GridItemComponent::SetColumnSpan(int32_t columnSpan)
{
    if (columnSpan <= 0) {
        LOGW("Invalid columnSpan %{public}d", columnSpan);
        return;
    }
    columnSpan_ = columnSpan;
}

} // namespace OHOS::Ace
