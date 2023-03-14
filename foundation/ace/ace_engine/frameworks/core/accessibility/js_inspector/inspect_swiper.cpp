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

#include "inspect_swiper.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
InspectSwiper::InspectSwiper(NodeId nodeId, const std::string& nodeName) : InspectNode(nodeId, nodeName) {
}

void InspectSwiper::PackAttrAndStyle()
{
    // add for the attrs
    attrs_.insert(std::make_pair("index", "0"));
    attrs_.insert(std::make_pair("autoplay", "false"));
    attrs_.insert(std::make_pair("interval", "3000"));
    attrs_.insert(std::make_pair("indicator", "true"));
    attrs_.insert(std::make_pair("digital", "false"));
    attrs_.insert(std::make_pair("indicatormask", "false"));
    attrs_.insert(std::make_pair("indicatordisabled", "false"));
    attrs_.insert(std::make_pair("loop", "true"));
    attrs_.insert(std::make_pair("vertical", "false"));
    attrs_.insert(std::make_pair("disabled", "false"));
    attrs_.insert(std::make_pair("focusable", "true"));
    attrs_.insert(std::make_pair("cachedsize", "-1"));
    attrs_.insert(std::make_pair("scrolleffect", "spring"));
    attrs_.insert(std::make_pair("displaymode", "stretch"));
    // add for the styles
    DeviceType deviceType = SystemProperties::GetDeviceType();
    if (deviceType == DeviceType::PHONE) {
        styles_.insert(std::make_pair("indicator-selected-color", "#ff007dff"));
    } else if (deviceType == DeviceType::TV || deviceType == DeviceType::WATCH) {
        styles_.insert(std::make_pair("indicator-selected-color", "#ffffffff"));
    }
    styles_.insert(std::make_pair("indicator-size", "4px"));
}
} // namespace OHOS::Ace::Framework
