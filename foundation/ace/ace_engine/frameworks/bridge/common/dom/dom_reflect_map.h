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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_REFLECT_MAP_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_REFLECT_MAP_H

#include <map>

#include "core/components/common/layout/constants.h"
#include "core/components/common/layout/grid_layout_info.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace::Framework {

const std::map<std::string, FlexDirection> FlexDirectionMap = {
    { DOM_FLEX_COLUMN, FlexDirection::COLUMN },
    { DOM_FLEX_ROW, FlexDirection::ROW }
};

const std::map<std::string, FlexAlign> FlexJustifyContentMap = {
    { DOM_JUSTIFY_CONTENT_START, FlexAlign::FLEX_START },
    { DOM_JUSTIFY_CONTENT_CENTER, FlexAlign::CENTER },
    { DOM_JUSTIFY_CONTENT_END, FlexAlign::FLEX_END },
    { DOM_JUSTIFY_CONTENT_BETWEEN, FlexAlign::SPACE_BETWEEN },
    { DOM_JUSTIFY_CONTENT_AROUND, FlexAlign::SPACE_AROUND },
    { DOM_JUSTIFY_CONTENT_EVENLY, FlexAlign::SPACE_EVENLY },
};

const std::map<std::string, FlexAlign> FlexAlignItemsMap = {
    { DOM_ALIGN_ITEMS_STRETCH, FlexAlign::STRETCH },
    { DOM_ALIGN_ITEMS_START, FlexAlign::FLEX_START },
    { DOM_ALIGN_ITEMS_CENTER, FlexAlign::CENTER },
    { DOM_ALIGN_ITEMS_END, FlexAlign::FLEX_END },
    { DOM_ALIGN_ITEMS_BASELINE, FlexAlign::BASELINE },
};

const std::map<std::string, WrapDirection> WrapDirectionMap = {
    { DOM_FLEX_ROW, WrapDirection::HORIZONTAL },
    { DOM_FLEX_COLUMN, WrapDirection::VERTICAL }
};

const std::map<std::string, WrapAlignment> WrapJustifyContentMap = {
    { DOM_JUSTIFY_CONTENT_START, WrapAlignment::START },
    { DOM_JUSTIFY_CONTENT_CENTER, WrapAlignment::CENTER },
    { DOM_JUSTIFY_CONTENT_END, WrapAlignment::END },
    { DOM_JUSTIFY_CONTENT_BETWEEN, WrapAlignment::SPACE_BETWEEN },
    { DOM_JUSTIFY_CONTENT_AROUND, WrapAlignment::SPACE_AROUND }
};

const std::map<std::string, WrapAlignment> WrapAlignItemsMap = {
    { DOM_ALIGN_ITEMS_STRETCH, WrapAlignment::STRETCH },
    { DOM_ALIGN_ITEMS_START,  WrapAlignment::START },
    { DOM_ALIGN_ITEMS_CENTER, WrapAlignment::CENTER },
    { DOM_ALIGN_ITEMS_END,  WrapAlignment::END}
};

const std::map<std::string, WrapAlignment> WrapAlignContentMap = {
    { DOM_ALIGN_CONTENT_STRETCH, WrapAlignment::STRETCH },
    { DOM_ALIGN_CONTENT_START, WrapAlignment::START },
    { DOM_ALIGN_CONTENT_CENTER, WrapAlignment::CENTER },
    { DOM_ALIGN_CONTENT_END, WrapAlignment::END },
    { DOM_ALIGN_CONTENT_BETWEEN, WrapAlignment::SPACE_BETWEEN },
    { DOM_ALIGN_CONTENT_AROUND, WrapAlignment::SPACE_AROUND }
};

const std::map<std::string, GridSizeType> GridSizeTypeMap = {
    { DOM_GRID_SIZE_TYPE_XS, GridSizeType::XS },
    { DOM_GRID_SIZE_TYPE_SM, GridSizeType::SM },
    { DOM_GRID_SIZE_TYPE_MD, GridSizeType::MD },
    { DOM_GRID_SIZE_TYPE_LG, GridSizeType::LG }
};

const std::map<std::string, GridTemplateType> GridTemplateMap = {
    { DOM_GRID_TEMPLATE_TYPE_NORMAL, GridTemplateType::NORMAL },
    { DOM_GRID_TEMPLATE_TYPE_GRID, GridTemplateType::GRID },
};

const std::map<std::string, GridColumnType> GridColumnTypeMap = {
    { DOM_COLUMN_TYPE_NONE, GridColumnType::NONE },
    { DOM_COLUMN_TYPE_BUBBLE, GridColumnType::BUBBLE_TYPE },
    { DOM_COLUMN_TYPE_BUTTON, GridColumnType::BUTTON_TYPE },
    { DOM_COLUMN_TYPE_CARD, GridColumnType::CARD_TYPE },
    { DOM_COLUMN_TYPE_CONTENT, GridColumnType::CONTENT_TYPE },
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_REFLECT_MAP_H
