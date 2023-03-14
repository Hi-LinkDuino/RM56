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

#include "core/components/test/json/grid_layout_item_creator.h"

#include "base/log/log.h"
#include "base/utils/macros.h"
#include "base/utils/utils.h"
#include "core/components/grid_layout/grid_layout_item_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> GridLayoutItemCreator::CreateFromJson(
    const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGI("Create grid layout item by json.");
    auto children = factory.CreateChildComponent(componentJson);
    auto child = children.empty() ? nullptr : children.front();
    auto gridLayoutItem = AceType::MakeRefPtr<GridLayoutItemComponent>(child);
    if (componentJson.Contains(GRID_COLUMN_INDEX) && componentJson.GetValue(GRID_COLUMN_INDEX)->IsNumber()) {
        int32_t columnIndex = componentJson.GetInt(GRID_COLUMN_INDEX, 0);
        gridLayoutItem->SetColumnIndex(columnIndex);
    }
    if (componentJson.Contains(GRID_ROW_INDEX) && componentJson.GetValue(GRID_ROW_INDEX)->IsNumber()) {
        int32_t rowIndex = componentJson.GetInt(GRID_ROW_INDEX, 0);
        gridLayoutItem->SetRowIndex(rowIndex);
    }
    if (componentJson.Contains(GRID_COLUMN_SPAN) && componentJson.GetValue(GRID_COLUMN_SPAN)->IsNumber()) {
        int32_t columnSpan = componentJson.GetInt(GRID_COLUMN_SPAN, 1);
        gridLayoutItem->SetColumnSpan(columnSpan);
    }
    if (componentJson.Contains(GRID_ROW_SPAN) && componentJson.GetValue(GRID_ROW_SPAN)->IsNumber()) {
        int32_t rowSpan = componentJson.GetInt(GRID_ROW_SPAN, 1);
        gridLayoutItem->SetRowSpan(rowSpan);
    }
    return gridLayoutItem;
}

} // namespace OHOS::Ace
