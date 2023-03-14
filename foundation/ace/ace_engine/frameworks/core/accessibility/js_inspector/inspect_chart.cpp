/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "inspect_chart.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
InspectChart::InspectChart(NodeId nodeId, const std::string& nodeName) : InspectNode(nodeId, nodeName) {
}

void InspectChart::PackAttrAndStyle()
{
    // add for the attrs
    attrs_.insert(std::make_pair("type", "line"));
    attrs_.insert(std::make_pair("effects", "true"));
    attrs_.insert(std::make_pair("disabled", "false"));
    attrs_.insert(std::make_pair("focusable", "false"));
    attrs_.insert(std::make_pair("strokeColor", "#ff6384"));
    attrs_.insert(std::make_pair("fillColor", "#ff6384"));
    attrs_.insert(std::make_pair("gradient", "false"));
    attrs_.insert(std::make_pair("min", "0"));
    attrs_.insert(std::make_pair("max", "100"));
    attrs_.insert(std::make_pair("axisTick", "10"));
    attrs_.insert(std::make_pair("display", "false"));
    attrs_.insert(std::make_pair("color", "#c0c0c0"));
    attrs_.insert(std::make_pair("width", "1px"));
    attrs_.insert(std::make_pair("smooth", "false"));
    attrs_.insert(std::make_pair("shape", "circle"));
    attrs_.insert(std::make_pair("size", "5px"));
    attrs_.insert(std::make_pair("strokeWidth", "1px"));
    attrs_.insert(std::make_pair("strokeColor", "#ff0000"));
    attrs_.insert(std::make_pair("fillColor", "#ff0000"));
    attrs_.insert(std::make_pair("margin", "1"));
    attrs_.insert(std::make_pair("percent", "0"));

    // add for the styles
    styles_.insert(std::make_pair("stroke-width", "32px"));
    styles_.insert(std::make_pair("start-angle", "240"));
    styles_.insert(std::make_pair("total-angle", "240"));
}
} // namespace OHOS::Ace::Framework
