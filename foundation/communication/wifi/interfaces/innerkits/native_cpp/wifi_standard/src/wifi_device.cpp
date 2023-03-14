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
#include "wifi_device.h"
#include "wifi_device_impl.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_LABEL("WifiDevice");

namespace OHOS {
namespace Wifi {
std::unique_ptr<WifiDevice> WifiDevice::CreateWifiDevice(int systemAbilityId)
{
    std::unique_ptr<WifiDeviceImpl> device = std::make_unique<WifiDeviceImpl>(systemAbilityId);
    if (device != nullptr) {
        if (device->Init()) {
            WIFI_LOGI("new device successfully!");
            return device;
        }
    }

    WIFI_LOGE("new wifi device failed");
    return nullptr;
}

std::unique_ptr<WifiDevice> WifiDevice::GetInstance(int systemAbilityId)
{
    std::unique_ptr<WifiDeviceImpl> device = std::make_unique<WifiDeviceImpl>(systemAbilityId);
    if (device != nullptr) {
        if (device->Init()) {
            WIFI_LOGI("init successfully!");
            return device;
        }
    }

    WIFI_LOGE("new wifi device failed");
    return nullptr;
}

WifiDevice::~WifiDevice()
{}
}  // namespace Wifi
}  // namespace OHOS