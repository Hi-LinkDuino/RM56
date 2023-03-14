/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "wifi_hotspot.h"
#include "wifi_hotspot_impl.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_HOTSPOT_LABEL("WifiHotspot");

namespace OHOS {
namespace Wifi {
std::unique_ptr<WifiHotspot> WifiHotspot::CreateWifiHotspot(int systemAbilityId)
{
    std::unique_ptr<WifiHotspotImpl> hotspot = std::make_unique<WifiHotspotImpl>(systemAbilityId);
    if (hotspot != nullptr) {
        if (hotspot->Init()) {
            WIFI_LOGI("succeeded");
            return hotspot;
        }
    }

    WIFI_LOGE("new wifi hotspot failed");
    return nullptr;
}

std::unique_ptr<WifiHotspot> WifiHotspot::GetInstance(int systemAbilityId)
{
    std::unique_ptr<WifiHotspotImpl> hotspot = std::make_unique<WifiHotspotImpl>(systemAbilityId);
    if (hotspot != nullptr) {
        if (hotspot->Init()) {
            WIFI_LOGI("hotspot init succeeded");
            return hotspot;
        }
    }

    WIFI_LOGE("new wifi hotspot failed");
    return nullptr;
}

WifiHotspot::~WifiHotspot()
{}
}  // namespace Wifi
}  // namespace OHOS