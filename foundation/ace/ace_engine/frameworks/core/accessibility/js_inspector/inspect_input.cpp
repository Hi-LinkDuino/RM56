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

#include "inspect_input.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
InspectInput::InspectInput(NodeId nodeId, const std::string& nodeName) : InspectNode(nodeId, nodeName) {
}

void InspectInput::PackAttrAndStyle()
{
    // add for the attrs
    attrs_.insert(std::make_pair("type", "text"));
    attrs_.insert(std::make_pair("checked", "false"));
    attrs_.insert(std::make_pair("enterkeytype", "default"));
    attrs_.insert(std::make_pair("showcounter", "false"));
    attrs_.insert(std::make_pair("disabled", "false"));
    attrs_.insert(std::make_pair("focusable", "true"));

    // add for the styles
    DeviceType deviceType = SystemProperties::GetDeviceType();
    if (deviceType == DeviceType::PHONE) {
        styles_.insert(std::make_pair("color", "#e6000000"));
        styles_.insert(std::make_pair("font-size", "16px"));
        styles_.insert(std::make_pair("placeholder-color", "#99000000"));
    } else if (deviceType == DeviceType::TV) {
        styles_.insert(std::make_pair("color", "#e6ffffff"));
        styles_.insert(std::make_pair("font-size", "18px"));
        styles_.insert(std::make_pair("placeholder-color", "#99ffffff"));
    }
    styles_.insert(std::make_pair("allow-scale", "true"));
    styles_.insert(std::make_pair("font-weight", "normal"));
    styles_.insert(std::make_pair("font-family", "sans-serif"));
}
} // namespace OHOS::Ace::Framework
