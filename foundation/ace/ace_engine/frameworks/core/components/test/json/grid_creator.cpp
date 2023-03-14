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

#include "core/components/test/json/grid_creator.h"

#include "base/log/log.h"
#include "core/components/grid/grid_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> GridCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    auto children = factory.CreateChildComponent(componentJson);
    auto grid = AceType::MakeRefPtr<GridComponent>(children);
    if (componentJson.Contains(GRID_SCROLL_PAGE) && componentJson.GetValue(GRID_SCROLL_PAGE)->IsBool()) {
        bool scrollPage = componentJson.GetBool(GRID_SCROLL_PAGE, false);
        grid->SetScrollPage(scrollPage);
    }
    if (componentJson.Contains(GRID_DIRECTION) && componentJson.GetValue(GRID_DIRECTION)->IsNumber()) {
        FlexDirection direction = static_cast<FlexDirection>(componentJson.GetInt(GRID_DIRECTION, DEFAULT_DIRECTION));
        grid->SetDirection(direction);
    }
    if (componentJson.Contains(GRID_ALIGN) && componentJson.GetValue(GRID_ALIGN)->IsNumber()) {
        FlexAlign flexAlign = static_cast<FlexAlign>(componentJson.GetInt(GRID_ALIGN, DEFAULT_ALIGN));
        grid->SetFlexAlign(flexAlign);
    }
    if (componentJson.Contains(GRID_COLUMN_COUNT) && componentJson.GetValue(GRID_COLUMN_COUNT)->IsNumber()) {
        int32_t columnCount = componentJson.GetInt(GRID_COLUMN_COUNT, DEFAULT_COLUMN_COUNT);
        grid->SetColumnCount(columnCount);
    }
    if (componentJson.Contains(GRID_COLUMN_EXTENT) && componentJson.GetValue(GRID_COLUMN_EXTENT)->IsNumber()) {
        int32_t columnExtent = componentJson.GetInt(GRID_COLUMN_EXTENT, DEFAULT_COLUMN_EXTENT);
        grid->SetColumnExtent(columnExtent);
    }
    if (componentJson.Contains(GRID_ITEM_COUNT) && componentJson.GetValue(GRID_ITEM_COUNT)->IsNumber()) {
        int32_t itemCount = componentJson.GetInt(GRID_ITEM_COUNT, DEFAULT_ITEM_COUNT);
        grid->SetTotalCount(itemCount);
    }
    if (componentJson.Contains(GRID_CACHED_COUNT) && componentJson.GetValue(GRID_CACHED_COUNT)->IsNumber()) {
        int32_t cachedCount = componentJson.GetInt(GRID_CACHED_COUNT, DEFAULT_CACHED_COUNT);
        grid->SetCachedCount(cachedCount);
    }
    if (componentJson.Contains(GRID_EVENT_REQUEST_ITEM) &&
        componentJson.GetValue(GRID_EVENT_REQUEST_ITEM)->IsString()) {
        std::string requestItemEvent = componentJson.GetString(GRID_EVENT_REQUEST_ITEM);
        grid->SetOnRequestItem(EventMarker { requestItemEvent });
    }
    if (componentJson.Contains(GRID_WIDTH) && componentJson.GetValue(GRID_WIDTH)->IsNumber()) {
        double width = componentJson.GetDouble(GRID_WIDTH, DEFAULT_GRID_WIDTH);
        grid->SetWidth(width);
    }
    if (componentJson.Contains(GRID_HEIGHT) && componentJson.GetValue(GRID_HEIGHT)->IsNumber()) {
        double height = componentJson.GetDouble(GRID_HEIGHT, DEFAULT_GRID_HEIGHT);
        grid->SetHeight(height);
    }
    if (componentJson.Contains(GRID_INITIAL_ITEM) && componentJson.GetValue(GRID_INITIAL_ITEM)->IsNumber()) {
        int32_t initialItem = componentJson.GetInt(GRID_INITIAL_ITEM, DEFAULT_INITIAL_ITEM);
        grid->GetGridController()->SetInitialIndex(initialItem);
    }
    return grid;
}

} // namespace OHOS::Ace
