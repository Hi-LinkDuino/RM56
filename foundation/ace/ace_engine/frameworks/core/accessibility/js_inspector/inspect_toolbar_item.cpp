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

#include "inspect_toolbar_item.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
InspectToolbarItem::InspectToolbarItem(NodeId nodeId, const std::string& nodeName) : InspectNode(nodeId, nodeName) {}

void InspectToolbarItem::PackAttrAndStyle()
{
    // add for the attrs
    attrs_.insert(std::make_pair("disabled", "false"));
    attrs_.insert(std::make_pair("focusable", "false"));

    // add for the styles
    std::unordered_map<std::string, std::string>().swap(styles_);
    styles_.clear();
    styles_.insert(std::make_pair("color", "#e6000000"));
    styles_.insert(std::make_pair("font-size", "16px"));
    styles_.insert(std::make_pair("allow-scale", "true"));
    styles_.insert(std::make_pair("font-style", "normal"));
    styles_.insert(std::make_pair("font-weight", "normal"));
    styles_.insert(std::make_pair("text-decoration", "none"));
    styles_.insert(std::make_pair("font-family", "sans-serif"));
    styles_.insert(std::make_pair("background-size", "auto"));
    styles_.insert(std::make_pair("background-repeat", "repeat"));
    styles_.insert(std::make_pair("background-position", "0px 0px"));
    styles_.insert(std::make_pair("opacity", "1"));
    styles_.insert(std::make_pair("display", "flex"));
    styles_.insert(std::make_pair("visibility", "visible"));
}
} // namespace OHOS::Ace::Framework
