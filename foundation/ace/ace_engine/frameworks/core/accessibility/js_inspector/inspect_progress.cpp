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

#include "inspect_progress.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
InspectProgress::InspectProgress(NodeId nodeId, const std::string& nodeName) : InspectNode(nodeId, nodeName) {
}

void InspectProgress::PackAttrAndStyle()
{
    // add for the attrs
    attrs_.insert(std::make_pair("type", "horizontal"));
    attrs_.insert(std::make_pair("focusable", "true"));
    attrs_.insert(std::make_pair("percent", "0"));
    attrs_.insert(std::make_pair("secondarypercent", "0"));
    attrs_.insert(std::make_pair("clockwise", "true"));

    // add for the styles
    DeviceType deviceType = SystemProperties::GetDeviceType();
    if (deviceType == DeviceType::PHONE) {
        styles_.insert(std::make_pair("color", "#ff007dff"));
    } else if (deviceType == DeviceType::TV) {
        styles_.insert(std::make_pair("color", "#e5ffffff"));
    } else if (deviceType == DeviceType::WATCH) {
        styles_.insert(std::make_pair("color", "#ff45a5ff"));
    }
    styles_.insert(std::make_pair("stroke-width", "4px"));
    styles_.erase("background-size");
    styles_.erase("background-repeat");
    styles_.erase("background-position");
}
} // namespace OHOS::Ace::Framework
