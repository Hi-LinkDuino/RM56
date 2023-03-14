/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "inspect_navigation_bar.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
InspectNavigationBar::InspectNavigationBar(NodeId nodeId, const std::string& nodeName) : InspectNode(nodeId, nodeName) {
}

void InspectNavigationBar::PackAttrAndStyle()
{
    // add for the attrs
    attrs_.insert(std::make_pair("type", "normal"));
    attrs_.insert(std::make_pair("backenabled", "false"));

    // add for the styles
    styles_.insert(std::make_pair("background-repeat", "repeat"));
    styles_.insert(std::make_pair("opacity", "1"));
    styles_.insert(std::make_pair("visibility", "visible"));
}
} // namespace OHOS::Ace::Framework
