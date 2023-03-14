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

#include "inspect_list.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
InspectList::InspectList(NodeId nodeId, const std::string& nodeName) : InspectNode(nodeId, nodeName) {}

void InspectList::PackAttrAndStyle()
{
    // add for the attrs
    attrs_.insert(std::make_pair("scrollpage", "true"));
    attrs_.insert(std::make_pair("cachedcount", "0"));
    attrs_.insert(std::make_pair("scrollbar", "off"));
    attrs_.insert(std::make_pair("scrolleffect", "spring"));
    attrs_.insert(std::make_pair("indexer", "false"));
    attrs_.insert(std::make_pair("indexermulti", "false"));
    attrs_.insert(std::make_pair("indexerbubble", "true"));
    attrs_.insert(std::make_pair("divider", "false"));
    attrs_.insert(std::make_pair("shapemode", "default"));
    attrs_.insert(std::make_pair("itemscale", "true"));
    attrs_.insert(std::make_pair("itemcenter", "false"));
    attrs_.insert(std::make_pair("updateeffect", "false"));
    attrs_.insert(std::make_pair("chainanimation", "false"));
    attrs_.insert(std::make_pair("scrollvibrate", "true"));
    attrs_.insert(std::make_pair("initialindex", "0"));
    attrs_.insert(std::make_pair("initialoffset", "0"));
    attrs_.insert(std::make_pair("disabled", "false"));
    attrs_.insert(std::make_pair("focusable", "true"));

    // add for the styles
    styles_.insert(std::make_pair("divider-color", "transparent"));
    styles_.insert(std::make_pair("divider-height", "1"));
    styles_.insert(std::make_pair("divider-origin", "0"));
    styles_.insert(std::make_pair("flex-direction", "column"));
    styles_.insert(std::make_pair("columns", "1"));
    styles_.insert(std::make_pair("align-items", "stretch"));
    styles_.insert(std::make_pair("fade-color", "grey"));
}
} // namespace OHOS::Ace::Framework
