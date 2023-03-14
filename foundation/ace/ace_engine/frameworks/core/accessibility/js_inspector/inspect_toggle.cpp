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

#include "inspect_toggle.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
InspectToggle::InspectToggle(NodeId nodeId, const std::string& nodeName) : InspectNode(nodeId, nodeName) {}

void InspectToggle::PackAttrAndStyle()
{
    // add for the attrs
    attrs_.insert(std::make_pair("checked", "false"));
    attrs_.insert(std::make_pair("disabled", "false"));
    attrs_.insert(std::make_pair("focusable", "false"));

    // add for the styles
    DeviceType deviceType = SystemProperties::GetDeviceType();
    if (deviceType == DeviceType::PHONE) {
        styles_.insert(std::make_pair("color", "#ff007dff"));
        styles_.insert(std::make_pair("font-size", "16px"));
    } else if (deviceType == DeviceType::TV) {
        styles_.insert(std::make_pair("color", "#e5ffffff"));
        styles_.insert(std::make_pair("font-size", "18px"));
    } else if (deviceType == DeviceType::WATCH) {
        styles_.insert(std::make_pair("color", "#ff45a5ff"));
        styles_.insert(std::make_pair("font-size", "16px"));
    }
    styles_.insert(std::make_pair("allow-scale", "true"));
    styles_.insert(std::make_pair("font-style", "normal"));
    styles_.insert(std::make_pair("font-weight", "normal"));
    styles_.insert(std::make_pair("font-family", "sans-serif"));
    styles_.erase("border");
    styles_.erase("border-left-style");
    styles_.erase("border-top-style");
    styles_.erase("border-right-style");
    styles_.erase("border-bottom-style");
    styles_.erase("border-left-width");
    styles_.erase("border-top-width");
    styles_.erase("border-right-width");
    styles_.erase("border-bottom-width");
    styles_.erase("border-left-color");
    styles_.erase("border-top-color");
    styles_.erase("border-right-color");
    styles_.erase("border-bottom-color");
}
} // namespace OHOS::Ace::Framework
