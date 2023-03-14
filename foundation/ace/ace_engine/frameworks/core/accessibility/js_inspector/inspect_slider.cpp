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

#include "inspect_slider.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
InspectSlider::InspectSlider(NodeId nodeId, const std::string& nodeName) : InspectNode(nodeId, nodeName) {
}

void InspectSlider::PackAttrAndStyle()
{
    // add for the attrs
    attrs_.insert(std::make_pair("min", "0"));
    attrs_.insert(std::make_pair("max", "100"));
    attrs_.insert(std::make_pair("step", "1"));
    attrs_.insert(std::make_pair("value", "0"));
    attrs_.insert(std::make_pair("type", "continuous"));
    attrs_.insert(std::make_pair("mode", "outset"));
    attrs_.insert(std::make_pair("showsteps", "false"));
    attrs_.insert(std::make_pair("showtips", "false"));
    attrs_.insert(std::make_pair("disabled", "false"));
    attrs_.insert(std::make_pair("focusable", "true"));

    // add for the styles
    DeviceType deviceType = SystemProperties::GetDeviceType();
    if (deviceType == DeviceType::PHONE) {
        styles_.insert(std::make_pair("color", "#19000000"));
        styles_.insert(std::make_pair("selected-color", "#ff007dff"));
    } else if (deviceType == DeviceType::TV) {
        styles_.insert(std::make_pair("color", "#33ffffff"));
        styles_.insert(std::make_pair("selected-color", "#ff0d9ffb"));
    } else if (deviceType == DeviceType::WATCH) {
        styles_.insert(std::make_pair("color", "#26ffffff"));
        styles_.insert(std::make_pair("selected-color", "#ff007dff"));
    }
    styles_.insert(std::make_pair("block-color", "#ffffff"));
}
} // namespace OHOS::Ace::Framework
