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

#include "inspect_popup.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
InspectPopup::InspectPopup(NodeId nodeId, const std::string& nodeName) : InspectNode(nodeId, nodeName) {
}

void InspectPopup::PackAttrAndStyle()
{
    // add for the attrs
    attrs_.insert(std::make_pair("placement", "bottom"));
    attrs_.insert(std::make_pair("keepalive", "false"));
    attrs_.insert(std::make_pair("clickable", "true"));
    attrs_.insert(std::make_pair("arrowoffset", "0"));
    attrs_.insert(std::make_pair("disabled", "false"));

    // add for the styles
    styles_.insert(std::make_pair("mask-color", "#00000000"));
    styles_.erase("flex-grow");
    styles_.erase("flex-shrink");
    styles_.erase("position");
}
} // namespace OHOS::Ace::Framework
