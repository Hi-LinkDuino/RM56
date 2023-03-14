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

#include "inspect_rating.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
InspectRating::InspectRating(NodeId nodeId, const std::string& nodeName) : InspectNode(nodeId, nodeName) {
}

void InspectRating::PackAttrAndStyle()
{
    // add for the attrs
    attrs_.insert(std::make_pair("numstars", "5"));
    attrs_.insert(std::make_pair("rating", "0"));
    attrs_.insert(std::make_pair("stepsize", "0.5"));
    attrs_.insert(std::make_pair("indicator", "false"));
    attrs_.insert(std::make_pair("disabled", "false"));
    attrs_.insert(std::make_pair("focusable", "true"));

    // add for the styles
    DeviceType deviceType = SystemProperties::GetDeviceType();
    if (deviceType == DeviceType::PHONE) {
        styles_.insert(std::make_pair("width", "120px"));
        styles_.insert(std::make_pair("height", "24px"));
    } else if (deviceType == DeviceType::TV) {
        styles_.insert(std::make_pair("width", "360px"));
        styles_.insert(std::make_pair("height", "72px"));
    }
    styles_.insert(std::make_pair("rtl-flip", "true"));
}
} // namespace OHOS::Ace::Framework
