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

#include "core/components/test/json/border_edge_creator.h"

#include <algorithm>

#include "base/log/log.h"
#include "core/components/test/json/color_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

BorderEdge BorderEdgeCreator::CreateFromJson(const JsonValue& json)
{
    LOGD("CreateFromJson BorderEdge");
    std::string classType = json.GetValue(CLASS_NAME)->GetString();
    if (classType != BORDER_EDGE_NAME) {
        LOGE("Create borderEdge err: not a borderEdge json.");
        return BorderEdge();
    }
    Color color;
    if (json.Contains(BORDER_EDGE_COLOR) && json.GetValue(BORDER_EDGE_COLOR)->IsObject()) {
        auto colorJson = json.GetValue(BORDER_EDGE_COLOR);
        color = ColorCreator::CreateFromJson(*colorJson);
    }
    double width = 0.0;
    if (json.Contains(BORDER_EDGE_WIDTH) && json.GetValue(BORDER_EDGE_WIDTH)->IsNumber()) {
        width = std::max(json.GetValue(BORDER_EDGE_WIDTH)->GetDouble(), 0.0);
    }
    BorderStyle style = BorderStyle::NONE;
    if (json.Contains(BORDER_EDGE_STYLE) && json.GetValue(BORDER_EDGE_STYLE)->IsString()) {
        auto styleStr = json.GetValue(BORDER_EDGE_STYLE)->GetString();
        style = BorderEdgeCreator::ConvertStrToBorderStyle(styleStr);
    }
    return BorderEdge(color, Dimension(width, DimensionUnit::PX), style);
}

BorderStyle BorderEdgeCreator::ConvertStrToBorderStyle(const std::string& styleStr)
{
    BorderStyle style;
    if (styleStr == "solid") {
        style = BorderStyle::SOLID;
    } else if (styleStr == "dashed") {
        style = BorderStyle::DASHED;
    } else if (styleStr == "dotted") {
        style = BorderStyle::DOTTED;
    } else if (styleStr == "none") {
        style = BorderStyle::NONE;
    } else {
        style = BorderStyle::SOLID;
    }
    return style;
}

} // namespace OHOS::Ace