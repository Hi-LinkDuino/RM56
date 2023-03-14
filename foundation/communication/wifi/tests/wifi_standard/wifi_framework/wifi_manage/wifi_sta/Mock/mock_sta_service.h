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
#ifndef OHOS_MOCK_WIFISTASERVICE_H
#define OHOS_MOCK_WIFISTASERVICE_H

#include <gmock/gmock.h>
#include "sta_service.h"

namespace OHOS {
namespace Wifi {
class MockWifiStaService : public StaService {
public:
    MockWifiStaService() {}
    virtual ~MockWifiStaService() {}
    MOCK_METHOD1(InitStaService, ErrCode(const StaServiceCallback &callbacks));
    MOCK_CONST_METHOD0(EnableWifi, ErrCode());
    MOCK_CONST_METHOD0(DisableWifi, ErrCode());
    MOCK_CONST_METHOD1(ConnectToDevice, ErrCode(const WifiDeviceConfig &config));
    MOCK_CONST_METHOD1(ConnectToNetwork, ErrCode(int networkId));
    MOCK_CONST_METHOD0(Disconnect, ErrCode());
    MOCK_CONST_METHOD0(ReAssociate, ErrCode());
    MOCK_CONST_METHOD1(AddDeviceConfig, int(const WifiDeviceConfig &config));
    MOCK_CONST_METHOD1(UpdateDeviceConfig, int(const WifiDeviceConfig &config));
    MOCK_CONST_METHOD1(RemoveDevice, ErrCode(int networkId));
    MOCK_CONST_METHOD2(EnableDeviceConfig, ErrCode(int networkId, bool attemptEnable));
    MOCK_CONST_METHOD1(DisableDeviceConfig, ErrCode(int networkId));
    MOCK_CONST_METHOD1(StartWps, ErrCode(const WpsConfig &config));
    MOCK_CONST_METHOD0(CancelWps, ErrCode());
    MOCK_CONST_METHOD1(SetCountryCode, ErrCode(const std::string &countryCode));
    MOCK_METHOD1(AutoConnectService, ErrCode(const std::vector<InterScanInfo> &scanInfos));
    MOCK_CONST_METHOD1(RegisterStaServiceCallback, void(const StaServiceCallback &callbacks));
};
}  // namespace OHOS
}  // namespace OHOS
#endif