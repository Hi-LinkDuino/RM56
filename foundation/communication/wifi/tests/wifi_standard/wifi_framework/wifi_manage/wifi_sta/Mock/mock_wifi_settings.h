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
#ifndef OHOS_MOCK_WIFISETTINGS_H
#define OHOS_MOCK_WIFISETTINGS_H

#include "wifi_msg.h"
#include <gmock/gmock.h>
#include "wifi_internal_msg.h"

namespace OHOS {
namespace Wifi {
class MockWifiSettings {
public:
    virtual ~MockWifiSettings() = default;
    virtual int SetWifiState(int state) = 0;
    virtual int AddDeviceConfig(const WifiDeviceConfig &config) = 0;
    virtual int RemoveDevice(int networkId) = 0;
    virtual void ClearDeviceConfig() = 0;
    virtual int GetDeviceConfig(std::vector<WifiDeviceConfig> &results) = 0;
    virtual int GetDeviceConfig(const int &networkId, WifiDeviceConfig &config) = 0;
    virtual int GetDeviceConfig(const std::string &ssid, const std::string &keymgmt, WifiDeviceConfig &config) = 0;
    virtual int GetDeviceConfig(const std::string &index, const int &indexType, WifiDeviceConfig &config) = 0;
    virtual int SetDeviceState(int networkId, int state, bool bSetOther = false) = 0;
    virtual int SyncDeviceConfig() = 0;
    virtual int ReloadDeviceConfig() = 0;
    virtual int GetIpInfo(IpInfo &info) = 0;
    virtual int SaveIpInfo(const IpInfo &info) = 0;
    virtual int GetLinkedInfo(WifiLinkedInfo &info) = 0;
    virtual int SaveLinkedInfo(const WifiLinkedInfo &info) = 0;
    virtual int SetMacAddress(const std::string &macAddress) = 0;
    virtual int GetMacAddress(std::string &macAddress) = 0;
    virtual int SetCountryCode(const std::string &countryCode) = 0;
    virtual int GetCountryCode(std::string &countryCode) = 0;
    virtual int GetSignalLevel(const int &rssi, const int &band) = 0;
    virtual bool EnableNetwork(int networkId, bool disableOthers) = 0;
    virtual void SetUserLastSelectedNetworkId(int networkId) = 0;
    virtual int GetUserLastSelectedNetworkId() = 0;
    virtual time_t GetUserLastSelectedNetworkTimeVal() = 0;
    virtual int GetDhcpIpType() = 0;
    virtual int SetDhcpIpType(int dhcpIpType) = 0;
    virtual int SetWhetherToAllowNetworkSwitchover(bool bSwitch) = 0;
    virtual bool GetWhetherToAllowNetworkSwitchover() = 0;
    virtual int AddWpsDeviceConfig(const WifiDeviceConfig &config) = 0;

    virtual int GetSavedDeviceAppraisalPriority() = 0;
    virtual int GetExternDeviceAppraisalPriority() = 0;
    virtual int GetScoretacticsScoreSlope() = 0;
    virtual int GetScoretacticsInitScore() = 0;
    virtual int GetScoretacticsSameBssidScore() = 0;
    virtual int GetScoretacticsSameNetworkScore() = 0;
    virtual int GetScoretacticsFrequency5GHzScore() = 0;
    virtual int GetScoretacticsLastSelectionScore() = 0;
    virtual int GetScoretacticsSecurityScore() = 0;
    virtual std::string GetStrDnsBak() const = 0;
    virtual int GetScanInfoList(std::vector<WifiScanInfo> &results) = 0;
};

class WifiSettings : public MockWifiSettings {
public:
    WifiSettings() = default;
    ~WifiSettings() = default;
    static WifiSettings &GetInstance(void);

    MOCK_METHOD1(SetWifiState, int(int state));
    MOCK_METHOD1(AddDeviceConfig, int(const WifiDeviceConfig &config));
    MOCK_METHOD1(RemoveDevice, int(int networkId));
    MOCK_METHOD0(ClearDeviceConfig, void());
    MOCK_METHOD1(GetDeviceConfig, int(std::vector<WifiDeviceConfig> &results));
    MOCK_METHOD2(GetDeviceConfig, int(const int &networkId, WifiDeviceConfig &config));
    MOCK_METHOD3(GetDeviceConfig, int(const std::string &ssid, const std::string &keymgmt, WifiDeviceConfig &config));
    MOCK_METHOD3(GetDeviceConfig, int(const std::string &index, const int &indexType, WifiDeviceConfig &config));
    MOCK_METHOD3(SetDeviceState, int(int networkId, int state, bool bSetOther));
    MOCK_METHOD0(SyncDeviceConfig, int());
    MOCK_METHOD0(ReloadDeviceConfig, int());
    MOCK_METHOD1(GetIpInfo, int(IpInfo &info));
    MOCK_METHOD1(SaveIpInfo, int(const IpInfo &info));
    MOCK_METHOD1(GetLinkedInfo, int(WifiLinkedInfo &info));
    MOCK_METHOD1(SaveLinkedInfo, int(const WifiLinkedInfo &info));
    MOCK_METHOD1(SetMacAddress, int(const std::string &macAddress));
    MOCK_METHOD1(GetMacAddress, int(std::string &macAddress));
    MOCK_METHOD1(SetCountryCode, int(const std::string &countryCode));
    MOCK_METHOD1(GetCountryCode, int(std::string &countryCode));
    MOCK_METHOD2(GetSignalLevel, int(const int &rssi, const int &band));
    MOCK_METHOD2(EnableNetwork, bool(int networkId, bool disableOthers));
    MOCK_METHOD1(SetUserLastSelectedNetworkId, void(int networkId));
    MOCK_METHOD0(GetUserLastSelectedNetworkId, int());
    MOCK_METHOD0(GetUserLastSelectedNetworkTimeVal, time_t());
    MOCK_METHOD0(GetDhcpIpType, int());
    MOCK_METHOD1(SetDhcpIpType, int(int dhcpIpType));
    MOCK_METHOD1(SetWhetherToAllowNetworkSwitchover, int(bool bSwitch));
    MOCK_METHOD0(GetWhetherToAllowNetworkSwitchover, bool());
    MOCK_METHOD1(AddWpsDeviceConfig, int(const WifiDeviceConfig &config));

    MOCK_METHOD0(GetSavedDeviceAppraisalPriority, int());
    MOCK_METHOD0(GetExternDeviceAppraisalPriority, int());
    MOCK_METHOD0(GetScoretacticsScoreSlope, int());
    MOCK_METHOD0(GetScoretacticsInitScore, int());
    MOCK_METHOD0(GetScoretacticsSameBssidScore, int());
    MOCK_METHOD0(GetScoretacticsSameNetworkScore, int());
    MOCK_METHOD0(GetScoretacticsFrequency5GHzScore, int());
    MOCK_METHOD0(GetScoretacticsLastSelectionScore, int());
    MOCK_METHOD0(GetScoretacticsSecurityScore, int());
    MOCK_CONST_METHOD0(GetStrDnsBak, std::string());
    MOCK_METHOD1(GetScanInfoList, int(std::vector<WifiScanInfo> &results));
};
}  // namespace OHOS
}  // namespace Wifi

#endif