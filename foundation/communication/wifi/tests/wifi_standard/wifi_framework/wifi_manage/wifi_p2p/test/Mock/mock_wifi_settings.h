/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_MOCK_WIFISETTINGS_H
#define OHOS_MOCK_WIFISETTINGS_H

#include <map>
#include <string>
#include <vector>
#include "wifi_p2p_msg.h"
#include <gmock/gmock.h>

namespace OHOS {
namespace Wifi {
const int MODE_ADD = 0;
const int MODE_DEL = 1;
const int MODE_UPDATE = 2;

class MockWifiSettings {
public:
    virtual ~MockWifiSettings() = default;
    virtual int SetCountryCode(const std::string &countryCode) = 0;
    virtual int GetCountryCode(std::string &countryCode) = 0;
    virtual int SetWifiP2pGroupInfo(const std::vector<WifiP2pGroupInfo> &groups) = 0;
    virtual int SetP2pVendorConfig(const P2pVendorConfig &config) = 0;
    virtual int GetP2pVendorConfig(P2pVendorConfig &config) = 0;
    virtual int GetP2pInfo(WifiP2pLinkedInfo &linkedInfo) = 0;
    virtual int SaveP2pInfo(WifiP2pLinkedInfo &linkedInfo) = 0;
    virtual int GetWifiP2pGroupInfo(std::vector<WifiP2pGroupInfo> &groups) = 0;
};

class WifiSettings : public MockWifiSettings {
public:
    WifiSettings() = default;
    ~WifiSettings() = default;
    static WifiSettings &GetInstance(void);
    MOCK_METHOD1(GetWifiP2pGroupInfo, int(std::vector<WifiP2pGroupInfo> &groups));
    MOCK_METHOD1(SaveP2pInfo, int(WifiP2pLinkedInfo &linkedInfo));
    MOCK_METHOD1(GetP2pInfo, int(WifiP2pLinkedInfo &linkedInfo));
    MOCK_METHOD1(SetP2pVendorConfig, int(const P2pVendorConfig &config));
    MOCK_METHOD1(GetP2pVendorConfig, int(P2pVendorConfig &config));
    MOCK_METHOD1(SetWifiP2pGroupInfo, int(const std::vector<WifiP2pGroupInfo> &groups));
    MOCK_METHOD1(SetCountryCode, int(const std::string &countryCode));
    MOCK_METHOD1(GetCountryCode, int(std::string &countryCode));
};
}  // namespace Wifi
}  // namespace OHOS
#endif