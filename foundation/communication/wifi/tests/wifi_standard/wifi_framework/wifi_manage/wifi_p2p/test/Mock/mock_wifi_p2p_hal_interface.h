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

#ifndef OHOS_MOCK_WIFIP2PHALINTERFACE_H
#define OHOS_MOCK_WIFIP2PHALINTERFACE_H
#include <gmock/gmock.h>
#include <map>
#include <string>
#include <vector>
#include "wifi_p2p_msg.h"
#include "wifi_error_no.h"
#include "wifi_idl_struct.h"
#include "wifi_p2p_event_callback.h"
#include "wifi_hid2d_msg.h"

namespace OHOS {
namespace Wifi {
class MockWifiP2PHalInterface {
public:
    virtual ~MockWifiP2PHalInterface() = default;
    virtual WifiErrorNo StartP2p(void) const = 0;
    virtual WifiErrorNo StopP2p(void) const = 0;
    virtual WifiErrorNo ListNetworks(std::map<int, WifiP2pGroupInfo> &value) const = 0;
    virtual WifiErrorNo StartWpsPbc(const std::string &groupInterface, const std::string &bssid) const = 0;
    virtual WifiErrorNo StartWpsPin(const std::string &groupInterface, const std::string &address,
        const std::string &pin, std::string &result) const = 0;
    virtual WifiErrorNo RemoveNetwork(int networkId) const = 0;
    virtual WifiErrorNo SetP2pDeviceName(const std::string &name) const = 0;
    virtual WifiErrorNo SetP2pDeviceType(const std::string &type) const = 0;
    virtual WifiErrorNo SetP2pConfigMethods(const std::string &config) const = 0;
    virtual WifiErrorNo SetP2pSsidPostfix(const std::string &postfixName) const = 0;
    virtual WifiErrorNo SetP2pGroupIdle(const std::string &groupInterface, size_t time) const = 0;
    virtual WifiErrorNo SetP2pPowerSave(const std::string &groupInterface, bool enable) const = 0;
    virtual WifiErrorNo SetWfdEnable(bool enable) const = 0;
    virtual WifiErrorNo SetWfdDeviceConfig(const std::string &config) const = 0;
    virtual WifiErrorNo P2pFind(size_t timeout) const = 0;
    virtual WifiErrorNo P2pStopFind() const = 0;
    virtual WifiErrorNo P2pConfigureListen(bool enable, size_t period, size_t interval) const = 0;
    virtual WifiErrorNo SetListenChannel(size_t channel, unsigned char regClass) const = 0;
    virtual WifiErrorNo P2pFlush() const = 0;
    virtual WifiErrorNo Connect(const WifiP2pConfigInternal &config, bool isJoinExistingGroup,
        std::string &pin) const = 0;
    virtual WifiErrorNo CancelConnect() const = 0;
    virtual WifiErrorNo ProvisionDiscovery(const WifiP2pConfigInternal &config) const = 0;
    virtual WifiErrorNo GroupAdd(bool isPersistent, int networkId, int freq) const = 0;
    virtual WifiErrorNo GroupRemove(const std::string &groupInterface) const = 0;
    virtual WifiErrorNo Invite(const WifiP2pGroupInfo &group, const std::string &deviceAddr) const = 0;
    virtual WifiErrorNo Reinvoke(int networkId, const std::string &deviceAddr) const = 0;
    virtual WifiErrorNo GetDeviceAddress(std::string &deviceAddress) const = 0;
    virtual WifiErrorNo GetGroupCapability(const std::string &deviceAddress, uint32_t &cap) const = 0;
    virtual WifiErrorNo P2pServiceAdd(const WifiP2pServiceInfo &info) const = 0;
    virtual WifiErrorNo P2pServiceRemove(const WifiP2pServiceInfo &info) const = 0;
    virtual WifiErrorNo FlushService() const = 0;
    virtual WifiErrorNo ReqServiceDiscovery(
        const std::string &deviceAddress, const std::vector<unsigned char> &tlvs, std::string &reqID) const = 0;
    virtual WifiErrorNo CancelReqServiceDiscovery(const std::string &id) const = 0;
    virtual WifiErrorNo SaveConfig() const = 0;
    virtual WifiErrorNo SetRandomMacAddr(bool enable) const = 0;
    virtual WifiErrorNo SetMiracastMode(int type) const = 0;
    virtual WifiErrorNo RegisterP2pCallback(const P2pHalCallback &callbacks) = 0;
    virtual WifiErrorNo RespServiceDiscovery(
        const WifiP2pDevice &device, int frequency, int dialogToken, const std::vector<unsigned char> &tlvs) const = 0;
    virtual WifiErrorNo SetServiceDiscoveryExternal(bool isExternalProcess) const = 0;
    virtual WifiErrorNo GetP2pPeer(const std::string &deviceAddress, WifiP2pDevice &device) const = 0;
    virtual WifiErrorNo P2pGetSupportFrequenciesByBand(int band, std::vector<int> &frequencies) const = 0;
    virtual WifiErrorNo P2pSetGroupConfig(int networkId, const IdlP2pGroupConfig &config) const = 0;
    virtual WifiErrorNo P2pGetGroupConfig(int networkId, IdlP2pGroupConfig &config) const = 0;
    virtual WifiErrorNo P2pAddNetwork(int &networkId) const = 0;
    virtual WifiErrorNo SetPersistentReconnect(int mode) const = 0;
    virtual WifiErrorNo SetP2pSecondaryDeviceType(const std::string &type) = 0;
    virtual WifiErrorNo Hid2dConnect(const Hid2dConnectConfig &config) const = 0;
};

class WifiP2PHalInterface : public MockWifiP2PHalInterface {
public:
    static WifiP2PHalInterface &GetInstance();

public:
    MOCK_CONST_METHOD0(StartP2p, WifiErrorNo());
    MOCK_CONST_METHOD0(StopP2p, WifiErrorNo());
    MOCK_CONST_METHOD1(ListNetworks, WifiErrorNo(std::map<int, WifiP2pGroupInfo> &value));
    MOCK_CONST_METHOD2(StartWpsPbc, WifiErrorNo(const std::string &groupInterface, const std::string &bssid));
    MOCK_CONST_METHOD4(StartWpsPin, WifiErrorNo(const std::string &groupInterface, const std::string &address,
                                        const std::string &pin, std::string &result));
    MOCK_CONST_METHOD1(RemoveNetwork, WifiErrorNo(int));
    MOCK_CONST_METHOD1(SetP2pDeviceName, WifiErrorNo(const std::string &name));
    MOCK_CONST_METHOD1(SetP2pDeviceType, WifiErrorNo(const std::string &type));
    MOCK_CONST_METHOD1(SetP2pConfigMethods, WifiErrorNo(const std::string &config));
    MOCK_CONST_METHOD1(SetP2pSsidPostfix, WifiErrorNo(const std::string &postfixName));
    MOCK_CONST_METHOD2(SetP2pGroupIdle, WifiErrorNo(const std::string &groupInterface, size_t time));
    MOCK_CONST_METHOD2(SetP2pPowerSave, WifiErrorNo(const std::string &groupInterface, bool enable));
    MOCK_CONST_METHOD1(SetWfdEnable, WifiErrorNo(bool));
    MOCK_CONST_METHOD1(SetWfdDeviceConfig, WifiErrorNo(const std::string &config));
    MOCK_CONST_METHOD1(P2pFind, WifiErrorNo(size_t));
    MOCK_CONST_METHOD0(P2pStopFind, WifiErrorNo());
    MOCK_CONST_METHOD3(P2pConfigureListen, WifiErrorNo(bool enable, size_t period, size_t interval));
    MOCK_CONST_METHOD2(SetListenChannel, WifiErrorNo(size_t channel, unsigned char regClass));
    MOCK_CONST_METHOD0(P2pFlush, WifiErrorNo());
    MOCK_CONST_METHOD3(Connect,
        WifiErrorNo(const WifiP2pConfigInternal &config, bool isJoinExistingGroup, std::string &pin));
    MOCK_CONST_METHOD0(CancelConnect, WifiErrorNo());
    MOCK_CONST_METHOD1(ProvisionDiscovery, WifiErrorNo(const WifiP2pConfigInternal &));
    MOCK_CONST_METHOD3(GroupAdd, WifiErrorNo(bool isPersistent, int networkId, int freq));
    MOCK_CONST_METHOD1(GroupRemove, WifiErrorNo(const std::string &groupInterface));
    MOCK_CONST_METHOD2(Invite, WifiErrorNo(const WifiP2pGroupInfo &group, const std::string &deviceAddr));
    MOCK_CONST_METHOD2(Reinvoke, WifiErrorNo(int networkId, const std::string &deviceAddr));
    MOCK_CONST_METHOD1(GetDeviceAddress, WifiErrorNo(std::string &deviceAddress));
    MOCK_CONST_METHOD2(GetGroupCapability, WifiErrorNo(const std::string &deviceAddress, uint32_t &cap));
    MOCK_CONST_METHOD1(P2pServiceAdd, WifiErrorNo(const WifiP2pServiceInfo &info));
    MOCK_CONST_METHOD1(P2pServiceRemove, WifiErrorNo(const WifiP2pServiceInfo &info));
    MOCK_CONST_METHOD0(FlushService, WifiErrorNo());
    MOCK_CONST_METHOD3(ReqServiceDiscovery,
        WifiErrorNo(const std::string &deviceAddress, const std::vector<unsigned char> &tlvs, std::string &reqID));
    MOCK_CONST_METHOD1(CancelReqServiceDiscovery, WifiErrorNo(const std::string &id));
    MOCK_CONST_METHOD0(SaveConfig, WifiErrorNo());
    MOCK_CONST_METHOD1(SetRandomMacAddr, WifiErrorNo(bool));
    MOCK_CONST_METHOD1(SetMiracastMode, WifiErrorNo(int));
    MOCK_METHOD1(RegisterP2pCallback, WifiErrorNo(const P2pHalCallback &callbacks));
    MOCK_CONST_METHOD4(RespServiceDiscovery, WifiErrorNo(const WifiP2pDevice &device, int frequency, int dialogToken,
                                                 const std::vector<unsigned char> &tlvs));
    MOCK_CONST_METHOD1(SetServiceDiscoveryExternal, WifiErrorNo(bool));
    MOCK_CONST_METHOD2(GetP2pPeer, WifiErrorNo(const std::string &deviceAddress, WifiP2pDevice &device));
    MOCK_CONST_METHOD2(P2pGetSupportFrequenciesByBand, WifiErrorNo(int band, std::vector<int> &frequencies));
    MOCK_CONST_METHOD2(P2pSetGroupConfig, WifiErrorNo(int networkId, const IdlP2pGroupConfig &config));
    MOCK_CONST_METHOD2(P2pGetGroupConfig, WifiErrorNo(int networkId, IdlP2pGroupConfig &config));
    MOCK_CONST_METHOD1(P2pAddNetwork, WifiErrorNo(int &networkId));
    MOCK_CONST_METHOD1(SetPersistentReconnect, WifiErrorNo(int mode));
    MOCK_METHOD1(SetP2pSecondaryDeviceType, WifiErrorNo(const std::string &type));
    MOCK_CONST_METHOD1(Hid2dConnect, WifiErrorNo(const Hid2dConnectConfig &config));
};
}  // namespace Wifi
}  // namespace OHOS
#endif