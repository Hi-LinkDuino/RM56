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

#include "core/components/test/json/grid_item_creator.h"

#include "base/log/log.h"
#include "base/utils/macros.h"
#include "base/utils/utils.h"
#include "core/components/grid/grid_item_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> GridItemCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson GridItem");
    auto children = factory.CreateChildComponent(componentJson);
    auto child = children.empty() ? nullptr : children.front();
    auto gridItem = AceType::MakeRefPtr<GridItemComponent>(child);
    if (componentJson.Contains(GRID_COLUMN_SPAN) && componentJson.GetValue(GRID_COLUMN_SPAN)->IsNumber()) {
        int32_t columnSpan = componentJson.GetInt(GRID_COLUMN_SPAN, DEFAULT_COLUMN_SPAN);
        gridItem->SetColumnSpan(columnSpan);
    }
    return gridItem;
}

} // namespace OHOS::Ace
