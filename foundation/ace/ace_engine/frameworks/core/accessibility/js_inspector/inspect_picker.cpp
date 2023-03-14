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

#include "inspect_picker.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
InspectPicker::InspectPicker(NodeId nodeId, const std::string& nodeName) : InspectNode(nodeId, nodeName) {
}

void InspectPicker::PackAttrAndStyle()
{
    // add for the attrs
    attrs_.insert(std::make_pair("disabled", "false"));
    attrs_.insert(std::make_pair("focusable", "true"));
    attrs_.insert(std::make_pair("vibrate", "true"));
    // type is text
    attrs_.insert(std::make_pair("selected", "0"));
    // type is date
    attrs_.insert(std::make_pair("start", "1970-1-1"));
    attrs_.insert(std::make_pair("end", "2100-12-31"));
    attrs_.insert(std::make_pair("lunar", "false"));
    attrs_.insert(std::make_pair("lunarswitch", "false"));
    // type is time
    attrs_.insert(std::make_pair("containsecond", "false"));
    attrs_.insert(std::make_pair("hours", "24"));

    // add for the styles
    styles_.insert(std::make_pair("text-color", "#e6000000"));
    styles_.insert(std::make_pair("font-size", "30px"));
    styles_.insert(std::make_pair("allow-scale", "true"));
    styles_.insert(std::make_pair("letter-spacing", "0"));
    styles_.insert(std::make_pair("font-style", "normal"));
    styles_.insert(std::make_pair("font-weight", "normal"));
    styles_.insert(std::make_pair("font-family", "sans-serif"));
    styles_.insert(std::make_pair("line-height", "0px"));
}
} // namespace OHOS::Ace::Framework
