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

#include "core/components/test/json/grid_layout_creator.h"

#include "base/log/log.h"
#include "core/components/grid_layout/grid_layout_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> GridLayoutCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGI("Create grid layout by json.");
    auto children = factory.CreateChildComponent(componentJson);
    auto gridLayout = AceType::MakeRefPtr<GridLayoutComponent>(children);
    if (componentJson.Contains(GRID_DIRECTION) && componentJson.GetValue(GRID_DIRECTION)->IsNumber()) {
        FlexDirection direction = static_cast<FlexDirection>(componentJson.GetInt(GRID_DIRECTION, DEFAULT_DIRECTION));
        gridLayout->SetDirection(direction);
    }
    if (componentJson.Contains(GRID_ALIGN) && componentJson.GetValue(GRID_ALIGN)->IsNumber()) {
        FlexAlign flexAlign = static_cast<FlexAlign>(componentJson.GetInt(GRID_ALIGN, DEFAULT_ALIGN));
        gridLayout->SetFlexAlign(flexAlign);
    }
    if (componentJson.Contains(GRID_COLUMN_COUNT) && componentJson.GetValue(GRID_COLUMN_COUNT)->IsNumber()) {
        int32_t columnCount = componentJson.GetInt(GRID_COLUMN_COUNT, 1);
        gridLayout->SetColumnCount(columnCount);
    }
    if (componentJson.Contains(GRID_ROW_COUNT) && componentJson.GetValue(GRID_ROW_COUNT)->IsNumber()) {
        int32_t rowCount = componentJson.GetInt(GRID_ROW_COUNT, 1);
        gridLayout->SetRowCount(rowCount);
    }
    if (componentJson.Contains(GRID_WIDTH) && componentJson.GetValue(GRID_WIDTH)->IsNumber()) {
        double width = componentJson.GetDouble(GRID_WIDTH, DEFAULT_GRID_WIDTH);
        gridLayout->SetWidth(width);
    }
    if (componentJson.Contains(GRID_HEIGHT) && componentJson.GetValue(GRID_HEIGHT)->IsNumber()) {
        double height = componentJson.GetDouble(GRID_HEIGHT, DEFAULT_GRID_HEIGHT);
        gridLayout->SetHeight(height);
    }
    return gridLayout;
}

} // namespace OHOS::Ace
