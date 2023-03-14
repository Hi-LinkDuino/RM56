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
#include "wifi_scan.h"
#include "iremote_broker.h"
#include "wifi_logger.h"
#include "wifi_scan_impl.h"

DEFINE_WIFILOG_SCAN_LABEL("WifiScan");

namespace OHOS {
namespace Wifi {
std::unique_ptr<WifiScan> WifiScan::CreateWifiScan(int systemAbilityId)
{
    std::unique_ptr<WifiScanImpl> pImpl = std::make_unique<WifiScanImpl>(systemAbilityId);
    if (pImpl != nullptr) {
        if (pImpl->Init()) {
            WIFI_LOGI("succeeded");
            return (pImpl);
        }
    }

    WIFI_LOGE("new wifi WifiScan failed");
    return nullptr;
}

std::unique_ptr<WifiScan> WifiScan::GetInstance(int systemAbilityId)
{
    std::unique_ptr<WifiScanImpl> pImpl = std::make_unique<WifiScanImpl>(systemAbilityId);
    if (pImpl != nullptr) {
        if (pImpl->Init()) {
            WIFI_LOGI("succeeded");
            return pImpl;
        }
    }

    WIFI_LOGE("new wifi WifiScan failed");
    return nullptr;
}
}  // namespace Wifi
}  // namespace OHOS