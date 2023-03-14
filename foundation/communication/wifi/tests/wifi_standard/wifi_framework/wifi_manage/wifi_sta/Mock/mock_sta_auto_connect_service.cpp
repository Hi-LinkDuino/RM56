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
#include "mock_sta_auto_connect_service.h"
#include "wifi_errcode.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_LABEL("MockStaAutoConnectService");

namespace OHOS {
namespace Wifi {
ErrCode MockStaAutoConnectService::InitAutoConnectService()
{
    WIFI_LOGI("Enter MockStaAutoConnectService::InitAutoConnectService.\n");
    return WIFI_OPT_SUCCESS;
}

void MockStaAutoConnectService::OnScanInfosReadyHandler(const std::vector<InterScanInfo> &scanInfos)
{
    (void) scanInfos;
    WIFI_LOGI("Enter MockStaAutoConnectService::OnScanInfosReadyHandler.\n");
}

bool MockStaAutoConnectService::EnableOrDisableBssid(std::string bssid, bool enable, int reason)
{
    (void) bssid;
    (void) enable;
    (void) reason;
    WIFI_LOGI("Enter MockStaAutoConnectService::EnableOrDisableBssid.\n");
    return true;
}

ErrCode MockStaAutoConnectService::AutoSelectDevice(WifiDeviceConfig &electedDevice,
    const std::vector<InterScanInfo> &scanInfos, std::vector<std::string> &blockedBssids, WifiLinkedInfo &info)
{
    (void) electedDevice;
    (void) scanInfos;
    (void) blockedBssids;
    (void) info;
    WIFI_LOGI("Enter MockStaAutoConnectService::AutoSelectDevice.\n");
    return WIFI_OPT_SUCCESS;
}

bool MockStaAutoConnectService::RegisterDeviceAppraisal(StaDeviceAppraisal *appraisal, int priority)
{
    (void) appraisal;
    (void) priority;
    WIFI_LOGI("Enter MockStaAutoConnectService::RegisterDeviceAppraisal.\n");
    return true;
}
}  // namespace Wifi
} //  namespace OHOS
