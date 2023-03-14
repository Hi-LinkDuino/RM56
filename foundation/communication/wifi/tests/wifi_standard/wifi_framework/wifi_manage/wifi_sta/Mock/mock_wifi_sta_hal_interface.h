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
#ifndef OHOS_MOCK_WIFISTAHALINTERFACE_H
#define OHOS_MOCK_WIFISTAHALINTERFACE_H

#include <gmock/gmock.h>
#include <map>
#include <string>
#include <vector>
#include "wifi_msg.h"
#include "wifi_error_no.h"
#include "wifi_idl_struct.h"
#include "wifi_sta_request.h"
#include "wifi_event_callback.h"

namespace OHOS {
namespace Wifi {
class MockWifiStaHalInterface {
public:
    virtual ~MockWifiStaHalInterface() = default;
    virtual WifiErrorNo StartWifi() = 0;
    virtual WifiErrorNo StopWifi() = 0;
    virtual WifiErrorNo Connect(int networkId) = 0;
    virtual WifiErrorNo Reconnect() = 0;
    virtual WifiErrorNo Reassociate() = 0;
    virtual WifiErrorNo Disconnect() = 0;
    virtual WifiErrorNo GetStaCapabilities(unsigned int &capabilities) = 0;
    virtual WifiErrorNo GetStaDeviceMacAddress(std::string &mac) = 0;
    virtual WifiErrorNo GetSupportFrequencies(int band, std::vector<int> &frequencies) = 0;
    virtual WifiErrorNo SetConnectMacAddr(const std::string &mac) = 0;
    virtual WifiErrorNo SetScanMacAddress(const std::string &mac) = 0;
    virtual WifiErrorNo DisconnectLastRoamingBssid(const std::string &mac) = 0;
    virtual WifiErrorNo GetSupportFeature(long &feature) = 0;
    virtual WifiErrorNo SendRequest(const WifiStaRequest &request) = 0;
    virtual WifiErrorNo SetTxPower(int power) = 0;
    virtual WifiErrorNo RemoveDevice(int networkId) = 0;
    virtual WifiErrorNo ClearDeviceConfig() const = 0;
    virtual WifiErrorNo GetNextNetworkId(int &networkId) = 0;
    virtual WifiErrorNo EnableNetwork(int networkId) = 0;
    virtual WifiErrorNo DisableNetwork(int networkId) = 0;
    virtual WifiErrorNo SetDeviceConfig(int networkId, const WifiIdlDeviceConfig &config) = 0;
    virtual WifiErrorNo GetDeviceConfig(WifiIdlGetDeviceConfig &config) = 0;
    virtual WifiErrorNo SaveDeviceConfig() = 0;
    virtual WifiErrorNo RegisterStaEventCallback(const WifiEventCallback &callback) = 0;
    virtual WifiErrorNo StartWpsPbcMode(const WifiIdlWpsConfig &config) = 0;
    virtual WifiErrorNo StartWpsPinMode(const WifiIdlWpsConfig &config, int &pinCode) = 0;
    virtual WifiErrorNo StopWps() = 0;
    virtual WifiErrorNo GetRoamingCapabilities(WifiIdlRoamCapability &capability) = 0;
    virtual WifiErrorNo SetRoamConfig(const WifiIdlRoamConfig &config) = 0;
    virtual WifiErrorNo WpaAutoConnect(int enable) = 0;
    virtual WifiErrorNo WpaBlocklistClear() = 0;
    virtual WifiErrorNo GetNetworkList(std::vector<WifiWpaNetworkInfo> &networkList) = 0;
    virtual WifiErrorNo GetConnectSignalInfo(const std::string &endBssid, WifiWpaSignalInfo &info) = 0;
};

class WifiStaHalInterface : public MockWifiStaHalInterface {
public:
    static WifiStaHalInterface &GetInstance(void);

public:
    MOCK_METHOD0(StartWifi, WifiErrorNo());
    MOCK_METHOD0(StopWifi, WifiErrorNo());
    MOCK_METHOD1(Connect, WifiErrorNo(int networkId));
    MOCK_METHOD0(Reconnect, WifiErrorNo());
    MOCK_METHOD0(Reassociate, WifiErrorNo());
    MOCK_METHOD0(Disconnect, WifiErrorNo());
    MOCK_METHOD1(GetStaCapabilities, WifiErrorNo(unsigned int &capabilities));
    MOCK_METHOD1(GetStaDeviceMacAddress, WifiErrorNo(std::string &mac));
    MOCK_METHOD2(GetSupportFrequencies, WifiErrorNo(int band, std::vector<int> &frequencies));
    MOCK_METHOD1(SetConnectMacAddr, WifiErrorNo(const std::string &mac));
    MOCK_METHOD1(SetScanMacAddress, WifiErrorNo(const std::string &mac));
    MOCK_METHOD1(DisconnectLastRoamingBssid, WifiErrorNo(const std::string &mac));
    MOCK_METHOD1(GetSupportFeature, WifiErrorNo(long &feature));
    MOCK_METHOD1(SendRequest, WifiErrorNo(const WifiStaRequest &request));
    MOCK_METHOD1(SetTxPower, WifiErrorNo(int power));
    MOCK_METHOD1(RemoveDevice, WifiErrorNo(int networkId));
    MOCK_CONST_METHOD0(ClearDeviceConfig, WifiErrorNo());
    MOCK_METHOD1(GetNextNetworkId, WifiErrorNo(int &networkId));
    MOCK_METHOD1(EnableNetwork, WifiErrorNo(int networkId));
    MOCK_METHOD1(DisableNetwork, WifiErrorNo(int networkId));
    MOCK_METHOD2(SetDeviceConfig, WifiErrorNo(int networkId, const WifiIdlDeviceConfig &config));
    MOCK_METHOD1(GetDeviceConfig, WifiErrorNo(WifiIdlGetDeviceConfig &config));
    MOCK_METHOD0(SaveDeviceConfig, WifiErrorNo());
    MOCK_METHOD1(RegisterStaEventCallback, WifiErrorNo(const WifiEventCallback &callback));
    MOCK_METHOD1(StartWpsPbcMode, WifiErrorNo(const WifiIdlWpsConfig &config));
    MOCK_METHOD2(StartWpsPinMode, WifiErrorNo(const WifiIdlWpsConfig &config, int &pinCode));
    MOCK_METHOD0(StopWps, WifiErrorNo());
    MOCK_METHOD1(GetRoamingCapabilities, WifiErrorNo(WifiIdlRoamCapability &capability));
    MOCK_METHOD1(SetRoamConfig, WifiErrorNo(const WifiIdlRoamConfig &config));
    MOCK_METHOD1(WpaAutoConnect, WifiErrorNo(int enable));
    MOCK_METHOD0(WpaBlocklistClear, WifiErrorNo());
    MOCK_METHOD1(GetNetworkList, WifiErrorNo(std::vector<WifiWpaNetworkInfo> &networkList));
    MOCK_METHOD2(GetConnectSignalInfo, WifiErrorNo(const std::string &endBssid, WifiWpaSignalInfo &info));
};
}  // namespace OHOS
}  // namespace OHOS
#endif