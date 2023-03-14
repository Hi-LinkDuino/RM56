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
#ifndef OHOS_MOCK_STA_AUTO_CONNECT_SERVICE_H
#define OHOS_MOCK_STA_AUTO_CONNECT_SERVICE_H

#include "sta_auto_connect_service.h"
#include <gmock/gmock.h>
#include <string>
#include <vector>
#include "wifi_msg.h"
#include "wifi_internal_msg.h"
#include "sta_device_appraisal.h"
#include "wifi_errcode.h"


namespace OHOS {
namespace Wifi {
class MockStaAutoConnectService : public StaAutoConnectService {
public:
    explicit MockStaAutoConnectService(StaStateMachine *staStateMachine):StaAutoConnectService(staStateMachine) {}
    ~MockStaAutoConnectService(){}
    ErrCode InitAutoConnectService() override;
    void OnScanInfosReadyHandler(const std::vector<InterScanInfo> &scanInfos) override;
    bool EnableOrDisableBssid(std::string bssid, bool enable, int reason) override;
    ErrCode AutoSelectDevice(WifiDeviceConfig &electedDevice, const std::vector<InterScanInfo> &scanInfos,
        std::vector<std::string> &blockedBssids, WifiLinkedInfo &info) override;
    bool RegisterDeviceAppraisal(StaDeviceAppraisal *appraisal, int priority) override;
};
}  // namespace Wifi
} //  namespace OHOS
#endif