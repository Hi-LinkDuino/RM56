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

#include "bridge/common/plugin_adapter/plugin_bridge.h"

#include "base/utils/system_properties.h"
#include "core/common/ace_application_info.h"
#include "core/common/container.h"

namespace OHOS::Ace::Framework {
void PluginBridge::ProcessSystemParam(std::unique_ptr<JsonValue>& infoList)
{
    std::string tmp = SystemProperties::GetBrand();
    if (tmp != SystemProperties::INVALID_PARAM) {
        infoList->Put("brand", tmp.c_str());
    }
    tmp = SystemProperties::GetManufacturer();
    if (tmp != SystemProperties::INVALID_PARAM) {
        infoList->Put("manufacturer", tmp.c_str());
    }
    tmp = SystemProperties::GetModel();
    if (tmp != SystemProperties::INVALID_PARAM) {
        infoList->Put("model", tmp.c_str());
    }
    tmp = SystemProperties::GetProduct();
    if (tmp != SystemProperties::INVALID_PARAM) {
        infoList->Put("product", tmp.c_str());
    }
    tmp = SystemProperties::GetApiVersion();
    if (tmp != SystemProperties::INVALID_PARAM) {
        char* tmpEnd = nullptr;
        infoList->Put(
            "apiVersion", static_cast<int32_t>(std::strtol(SystemProperties::GetApiVersion().c_str(), &tmpEnd, 10)));
    }
    tmp = SystemProperties::GetReleaseType();
    if (tmp != SystemProperties::INVALID_PARAM) {
        infoList->Put("releaseType", tmp.c_str());
    }
    tmp = SystemProperties::GetParamDeviceType();
    if (tmp != SystemProperties::INVALID_PARAM) {
        infoList->Put("deviceType", tmp.c_str());
    }
}

std::pair<std::string, bool> PluginBridge::GetDeviceInfo()
{
    static constexpr uint8_t paramNumber = 13;
    auto infoList = JsonUtil::Create(true);
    ProcessSystemParam(infoList);

    if (!AceApplicationInfo::GetInstance().GetLanguage().empty()) {
        infoList->Put("language", AceApplicationInfo::GetInstance().GetLanguage().c_str());
    }
    if (!AceApplicationInfo::GetInstance().GetCountryOrRegion().empty()) {
        infoList->Put("region", AceApplicationInfo::GetInstance().GetCountryOrRegion().c_str());
    }

    auto container = Container::Current();
    int32_t width = container ? container->GetViewWidth() : 0;
    if (width != 0) {
        infoList->Put("windowWidth", width);
    }
    int32_t height = container ? container->GetViewHeight() : 0;
    if (height != 0) {
        infoList->Put("windowHeight", height);
    }
    infoList->Put("screenDensity", SystemProperties::GetResolution());

    bool isRound = SystemProperties::GetIsScreenRound();
    if (isRound) {
        infoList->Put("screenShape", "circle");
    } else {
        infoList->Put("screenShape", "rect");
    }

    bool isParamValid = false;
    uint8_t count = 0;
    auto child = infoList->GetChild();
    while (child->IsValid()) {
        child = child->GetNext();
        ++count;
    }
    if (count == paramNumber) {
        isParamValid = true;
    }
    return std::make_pair<std::string, bool>(infoList->ToString(), std::move(isParamValid));
}
} // namespace OHOS::Ace::Framework
