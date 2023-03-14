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

#include "wifi_p2p_hal_interface.h"
#include "wifi_log.h"
#include <mutex>

#undef LOG_TAG
#define LOG_TAG "WifiP2PHalInterface"

namespace OHOS {
namespace Wifi {
WifiP2PHalInterface &WifiP2PHalInterface::GetInstance(void)
{
    static WifiP2PHalInterface inst;
    static int initFlag = 0;
    static std::mutex initMutex;
    if (initFlag == 0) {
        std::unique_lock<std::mutex> lock(initMutex);
        if (initFlag == 0) {
            if (inst.InitIdlClient()) {
                initFlag = 1;
            }
        }
    }
    return inst;
}

WifiErrorNo WifiP2PHalInterface::StartP2p(void) const
{
    return mIdlClient->ReqP2pStart();
}

WifiErrorNo WifiP2PHalInterface::StopP2p(void) const
{
    return mIdlClient->ReqP2pStop();
}

WifiErrorNo WifiP2PHalInterface::RegisterP2pCallback(const P2pHalCallback &callbacks)
{
    WifiErrorNo err = mIdlClient->ReqP2pRegisterCallback(callbacks);
    if (err == WIFI_IDL_OPT_OK || callbacks.onConnectSupplicant == nullptr) {
        mP2pCallback = callbacks;
    }
    return err;
}

WifiErrorNo WifiP2PHalInterface::StartWpsPbc(const std::string &groupInterface, const std::string &bssid) const
{
    return mIdlClient->ReqP2pSetupWpsPbc(groupInterface, bssid);
}

WifiErrorNo WifiP2PHalInterface::StartWpsPin(
    const std::string &groupInterface, const std::string &address, const std::string &pin, std::string &result) const
{
    return mIdlClient->ReqP2pSetupWpsPin(groupInterface, address, pin, result);
}

WifiErrorNo WifiP2PHalInterface::RemoveNetwork(int networkId) const
{
    return mIdlClient->ReqP2pRemoveNetwork(networkId);
}

WifiErrorNo WifiP2PHalInterface::ListNetworks(std::map<int, WifiP2pGroupInfo> &mapGroups) const
{
    return mIdlClient->ReqP2pListNetworks(mapGroups);
}

WifiErrorNo WifiP2PHalInterface::SetP2pDeviceName(const std::string &name) const
{
    return mIdlClient->ReqP2pSetDeviceName(name);
}

WifiErrorNo WifiP2PHalInterface::SetP2pDeviceType(const std::string &type) const
{
    return mIdlClient->ReqP2pSetWpsDeviceType(type);
}

WifiErrorNo WifiP2PHalInterface::SetP2pSecondaryDeviceType(const std::string &type)
{
    return mIdlClient->ReqP2pSetWpsSecondaryDeviceType(type);
}

WifiErrorNo WifiP2PHalInterface::SetP2pConfigMethods(const std::string &methods) const
{
    return mIdlClient->ReqP2pSetWpsConfigMethods(methods);
}

WifiErrorNo WifiP2PHalInterface::SetP2pSsidPostfix(const std::string &postfixName) const
{
    return mIdlClient->ReqP2pSetSsidPostfixName(postfixName);
}

WifiErrorNo WifiP2PHalInterface::SetP2pGroupIdle(const std::string &groupInterface, size_t time) const
{
    return mIdlClient->ReqP2pSetGroupMaxIdle(groupInterface, time);
}

WifiErrorNo WifiP2PHalInterface::SetP2pPowerSave(const std::string &groupInterface, bool enable) const
{
    return mIdlClient->ReqP2pSetPowerSave(groupInterface, enable);
}

WifiErrorNo WifiP2PHalInterface::SetWfdEnable(bool enable) const
{
    return mIdlClient->ReqP2pSetWfdEnable(enable);
}

WifiErrorNo WifiP2PHalInterface::SetWfdDeviceConfig(const std::string &config) const
{
    return mIdlClient->ReqP2pSetWfdDeviceConfig(config);
}

WifiErrorNo WifiP2PHalInterface::P2pFind(size_t timeout) const
{
    return mIdlClient->ReqP2pStartFind(timeout);
}

WifiErrorNo WifiP2PHalInterface::P2pStopFind() const
{
    return mIdlClient->ReqP2pStopFind();
}

WifiErrorNo WifiP2PHalInterface::P2pConfigureListen(bool enable, size_t period, size_t interval) const
{
    return mIdlClient->ReqP2pSetExtListen(enable, period, interval);
}

WifiErrorNo WifiP2PHalInterface::SetListenChannel(size_t channel, unsigned char regClass) const
{
    return mIdlClient->ReqP2pSetListenChannel(channel, regClass);
}

WifiErrorNo WifiP2PHalInterface::P2pFlush() const
{
    return mIdlClient->ReqP2pFlush();
}

WifiErrorNo WifiP2PHalInterface::Connect(const WifiP2pConfigInternal &config, bool isJoinExistingGroup,
    std::string &pin) const
{
    return mIdlClient->ReqP2pConnect(config, isJoinExistingGroup, pin);
}

WifiErrorNo WifiP2PHalInterface::CancelConnect() const
{
    return mIdlClient->ReqP2pCancelConnect();
}

WifiErrorNo WifiP2PHalInterface::ProvisionDiscovery(const WifiP2pConfigInternal &config) const
{
    return mIdlClient->ReqP2pProvisionDiscovery(config);
}

WifiErrorNo WifiP2PHalInterface::GroupAdd(bool isPersistent, int networkId, int freq) const
{
    return mIdlClient->ReqP2pAddGroup(isPersistent, networkId, freq);
}

WifiErrorNo WifiP2PHalInterface::GroupRemove(const std::string &groupInterface) const
{
    return mIdlClient->ReqP2pRemoveGroup(groupInterface);
}

WifiErrorNo WifiP2PHalInterface::Invite(const WifiP2pGroupInfo &group, const std::string &deviceAddr) const
{
    return mIdlClient->ReqP2pInvite(group, deviceAddr);
}

WifiErrorNo WifiP2PHalInterface::Reinvoke(int networkId, const std::string &deviceAddr) const
{
    return mIdlClient->ReqP2pReinvoke(networkId, deviceAddr);
}

WifiErrorNo WifiP2PHalInterface::GetDeviceAddress(std::string &deviceAddress) const
{
    return mIdlClient->ReqP2pGetDeviceAddress(deviceAddress);
}

WifiErrorNo WifiP2PHalInterface::GetGroupCapability(const std::string &deviceAddress, uint32_t &cap) const
{
    return mIdlClient->ReqP2pGetGroupCapability(deviceAddress, cap);
}

WifiErrorNo WifiP2PHalInterface::P2pServiceAdd(const WifiP2pServiceInfo &info) const
{
    return mIdlClient->ReqP2pAddService(info);
}

WifiErrorNo WifiP2PHalInterface::P2pServiceRemove(const WifiP2pServiceInfo &info) const
{
    return mIdlClient->ReqP2pRemoveService(info);
}

WifiErrorNo WifiP2PHalInterface::FlushService() const
{
    return mIdlClient->ReqP2pFlushService();
}

WifiErrorNo WifiP2PHalInterface::SaveConfig() const
{
    return mIdlClient->ReqP2pSaveConfig();
}

WifiErrorNo WifiP2PHalInterface::ReqServiceDiscovery(
    const std::string &deviceAddress, const std::vector<unsigned char> &tlvs, std::string &reqID) const
{
    return mIdlClient->ReqP2pReqServiceDiscovery(deviceAddress, tlvs, reqID);
}

WifiErrorNo WifiP2PHalInterface::CancelReqServiceDiscovery(const std::string &id) const
{
    return mIdlClient->ReqP2pCancelServiceDiscovery(id);
}

WifiErrorNo WifiP2PHalInterface::SetRandomMacAddr(bool enable) const
{
    return mIdlClient->ReqP2pSetRandomMac(enable);
}

WifiErrorNo WifiP2PHalInterface::SetMiracastMode(int type) const
{
    return mIdlClient->ReqP2pSetMiracastType(type);
}

WifiErrorNo WifiP2PHalInterface::SetPersistentReconnect(int mode) const
{
    return mIdlClient->ReqSetPersistentReconnect(mode);
}

WifiErrorNo WifiP2PHalInterface::RespServiceDiscovery(
    const WifiP2pDevice &device, int frequency, int dialogToken, const std::vector<unsigned char> &tlvs) const
{
    return mIdlClient->ReqRespServiceDiscovery(device, frequency, dialogToken, tlvs);
}

WifiErrorNo WifiP2PHalInterface::SetServiceDiscoveryExternal(bool isExternalProcess) const
{
    return mIdlClient->ReqSetServiceDiscoveryExternal(isExternalProcess);
}

WifiErrorNo WifiP2PHalInterface::GetP2pPeer(const std::string &deviceAddress, WifiP2pDevice &device) const
{
    return mIdlClient->ReqGetP2pPeer(deviceAddress, device);
}

WifiErrorNo WifiP2PHalInterface::P2pGetSupportFrequenciesByBand(int band, std::vector<int> &frequencies) const
{
    return mIdlClient->ReqP2pGetSupportFrequencies(band, frequencies);
}

WifiErrorNo WifiP2PHalInterface::P2pSetGroupConfig(int networkId, const IdlP2pGroupConfig &config) const
{
    return mIdlClient->ReqP2pSetGroupConfig(networkId, config);
}

WifiErrorNo WifiP2PHalInterface::P2pGetGroupConfig(int networkId, IdlP2pGroupConfig &config) const
{
    return mIdlClient->ReqP2pGetGroupConfig(networkId, config);
}

WifiErrorNo WifiP2PHalInterface::P2pAddNetwork(int &networkId) const
{
    return mIdlClient->ReqP2pAddNetwork(networkId);
}

const P2pHalCallback &WifiP2PHalInterface::GetP2pCallbackInst(void) const
{
    return mP2pCallback;
}

WifiErrorNo WifiP2PHalInterface::Hid2dConnect(const Hid2dConnectConfig &config) const
{
    return mIdlClient->ReqP2pHid2dConnect(config);
}
}  // namespace Wifi
}  // namespace OHOS