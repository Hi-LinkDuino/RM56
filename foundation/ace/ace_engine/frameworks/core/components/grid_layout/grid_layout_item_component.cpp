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

#include "core/components/grid_layout/grid_layout_item_component.h"

#include "core/components/grid_layout/grid_layout_item_element.h"
#include "core/components/grid_layout/render_grid_layout_item.h"

namespace OHOS::Ace {

RefPtr<Element> GridLayoutItemComponent::CreateElement()
{
    return AceType::MakeRefPtr<GridLayoutItemElement>();
}

RefPtr<RenderNode> GridLayoutItemComponent::CreateRenderNode()
{
    return RenderGridLayoutItem::Create();
}

void GridLayoutItemComponent::SetColumnIndex(int32_t columnIndex)
{
    if (columnIndex < 0) {
        return;
    }
    columnIndex_ = columnIndex;
}

void GridLayoutItemComponent::SetRowIndex(int32_t rowIndex)
{
    if (rowIndex < 0) {
        return;
    }
    rowIndex_ = rowIndex;
}

void GridLayoutItemComponent::SetColumnSpan(int32_t columnSpan)
{
    if (columnSpan <= 0) {
        LOGW("Invalid columnSpan %{public}d", columnSpan);
        return;
    }
    columnSpan_ = columnSpan;
}

void GridLayoutItemComponent::SetRowSpan(int32_t rowSpan)
{
    if (rowSpan <= 0) {
        LOGW("Invalid rowSpan %{public}d", rowSpan);
        return;
    }
    rowSpan_ = rowSpan;
}

} // namespace OHOS::Ace
