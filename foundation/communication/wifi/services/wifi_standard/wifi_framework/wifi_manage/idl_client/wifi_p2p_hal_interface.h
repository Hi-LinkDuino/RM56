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

#ifndef OHOS_WIFI_P2P_HAL_INTERFACE_H
#define OHOS_WIFI_P2P_HAL_INTERFACE_H

#include <string>
#include <vector>
#include <map>

#include "wifi_base_hal_interface.h"
#include "wifi_idl_struct.h"
#include "wifi_p2p_event_callback.h"

namespace OHOS {
namespace Wifi {
class WifiP2PHalInterface : public WifiBaseHalInterface {
public:
    static WifiP2PHalInterface &GetInstance(void);

    /**
     * @Description Open P2p
     *
     * @return WifiErrorNo
     */
    WifiErrorNo StartP2p(void) const;

    /**
     * @Description Close p2p
     *
     * @return WifiErrorNo
     */
    WifiErrorNo StopP2p(void) const;

    /**
     * @Description P2P hal-layer registration event
     *
     * @param callbacks - register callback functions
     * @return WifiErrorNo
     */
    WifiErrorNo RegisterP2pCallback(const P2pHalCallback &callbacks);

    /**
     * @Description Enable WPS PBC
     *
     * @param groupInterface
     * @param bssid
     * @return WifiErrorNo
     */
    WifiErrorNo StartWpsPbc(const std::string &groupInterface, const std::string &bssid) const;

    /**
     * @Description Enable Wps Pin mode
     *
     * @param groupInterface - p2p group
     * @param address
     * @param pin - pin code
     * @param result - when pin is empty, represent use pin display mode, this return pin code
     * @return WifiErrorNo
     */
    WifiErrorNo StartWpsPin(const std::string &groupInterface, const std::string &address, const std::string &pin,
        std::string &result) const;

    /**
     * @Description Remove Network
     *
     * @param networkId
     * @return WifiErrorNo
     */
    WifiErrorNo RemoveNetwork(int networkId) const;

    /**
     * @Description Get p2p Network list.
     *
     * @param mapGroups
     * @return WifiErrorNo
     */
    WifiErrorNo ListNetworks(std::map<int, WifiP2pGroupInfo> &mapGroups) const;

    /**
     * @Description Requesting P2P Setting Device Name
     *
     * @param name
     * @return WifiErrorNo
     */
    WifiErrorNo SetP2pDeviceName(const std::string &name) const;

    /**
     * @Description Send a request for setting the WPS primary device type in P2P mode
     *
     * @param type
     * @return WifiErrorNo
     */
    WifiErrorNo SetP2pDeviceType(const std::string &type) const;

    /**
     * @Description Send a request for setting the WPS secondary device type in P2P mode
     *
     * @param type
     * @return WifiErrorNo
     */
    WifiErrorNo SetP2pSecondaryDeviceType(const std::string &type);

    /**
     * @Description Sends a request for setting the WPS configuration method to
     * the P2P.
     *
     * @param methods
     * @return WifiErrorNo
     */
    WifiErrorNo SetP2pConfigMethods(const std::string &methods) const;

    /**
     * @Description Send a P2P request for setting the SSID suffix
     *
     * @param postfixName
     * @return WifiErrorNo
     */
    WifiErrorNo SetP2pSsidPostfix(const std::string &postfixName) const;

    /**
     * @Description Sends a request for set group max idle to the P2P
     *
     * @param groupInterface - p2p group
     * @param time
     * @return WifiErrorNo
     */
    WifiErrorNo SetP2pGroupIdle(const std::string &groupInterface, size_t time) const;

    /**
     * @Description Sends a request for set power save to the P2P
     *
     * @param groupInterface - p2p group
     * @param enable
     * @return WifiErrorNo
     */
    WifiErrorNo SetP2pPowerSave(const std::string &groupInterface, bool enable) const;

    /**
     * @Description enable/disable Wi-Fi Display
     *
     * @param enable
     * @return WifiErrorNo
     */
    WifiErrorNo SetWfdEnable(bool enable) const;

    /**
     * @Description Sends a request for set Wi-Fi Display config
     *
     * @param config
     * @return WifiErrorNo
     */
    WifiErrorNo SetWfdDeviceConfig(const std::string &config) const;

    /**
     * @Description Sends a request for start p2p find to the P2P
     *
     * @param timeout
     * @return WifiErrorNo
     */
    WifiErrorNo P2pFind(size_t timeout) const;

    /**
     * @Description Sends a request for stop p2p find to the P2P
     *
     * @return WifiErrorNo
     */
    WifiErrorNo P2pStopFind() const;

    /**
     * @Description Sends a request for set ext listen to the P2P
     *
     * @param enable
     * @param period
     * @param interval
     * @return WifiErrorNo
     */
    WifiErrorNo P2pConfigureListen(bool enable, size_t period, size_t interval) const;

    /**
     * @Description Sends a request for set listen channel to the P2P
     *
     * @param channel
     * @param regClass
     * @return WifiErrorNo
     */
    WifiErrorNo SetListenChannel(size_t channel, unsigned char regClass) const;

    /**
     * @Description Sends a request for flush to the P2P.
     *
     * @return WifiErrorNo
     */
    WifiErrorNo P2pFlush() const;

    /**
     * @Description Sends a request for connect to the P2P
     *
     * @param config
     * @param isJoinExistingGroup
     * @param pin - if using pin mode, return pin code
     * @return WifiErrorNo
     */
    WifiErrorNo Connect(const WifiP2pConfigInternal &config, bool isJoinExistingGroup, std::string &pin) const;

    /**
     * @Description Sends a request for cancel connect to the P2P
     *
     * @return WifiErrorNo
     */
    WifiErrorNo CancelConnect() const;

    /**
     * @Description Provision Discovery
     *
     * @param config
     * @return WifiErrorNo
     */
    WifiErrorNo ProvisionDiscovery(const WifiP2pConfigInternal &config) const;

    /**
     * @Description Add Group
     *
     * @param isPersistent
     * @param networkId - if networkid > 0, update it's freq; else create a new group
     * @param freq
     * @return WifiErrorNo
     */
    WifiErrorNo GroupAdd(bool isPersistent, int networkId, int freq) const;

    /**
     * @Description Sends a request for remove group to the P2P
     *
     * @param groupInterface - p2p group
     * @return WifiErrorNo
     */
    WifiErrorNo GroupRemove(const std::string &groupInterface) const;

    /**
     * @Description Sends a request for invite to the P2P
     *
     * @param group - p2p group info
     * @param deviceAddr - invite device address
     * @return WifiErrorNo
     */
    WifiErrorNo Invite(const WifiP2pGroupInfo &group, const std::string &deviceAddr) const;

    /**
     * @Description Sends a request for reinvoke to the P2P
     *
     * @param networkId
     * @param deviceAddr
     * @return WifiErrorNo
     */
    WifiErrorNo Reinvoke(int networkId, const std::string &deviceAddr) const;

    /**
     * @Description Sends a request for get device address to the P2P.
     *
     * @param deviceAddress
     * @return WifiErrorNo
     */
    WifiErrorNo GetDeviceAddress(std::string &deviceAddress) const;

    /**
     * @Description Sends a request for add service to the P2P
     *
     * @param info
     * @return WifiErrorNo
     */
    WifiErrorNo P2pServiceAdd(const WifiP2pServiceInfo &info) const;

    /**
     * @Description Sends a request for remove service to the P2P
     *
     * @param info
     * @return WifiErrorNo
     */
    WifiErrorNo P2pServiceRemove(const WifiP2pServiceInfo &info) const;

    /**
     * @Description Sends a request for get group capability to the P2P
     *
     * @param deviceAddress
     * @param cap
     * @return WifiErrorNo
     */
    WifiErrorNo GetGroupCapability(const std::string &deviceAddress, uint32_t &cap) const;

    /**
     * @Description Sends a request for flush service to the P2P
     *
     * @return WifiErrorNo
     */
    WifiErrorNo FlushService() const;

    /**
     * @Description Sends a request for save config to the P2P
     *
     * @return WifiErrorNo
     */
    WifiErrorNo SaveConfig() const;

    /**
     * @Description Sends a request for request service discovery to the P2P
     *
     * @param macAddr
     * @param queryMsg
     * @return WifiErrorNo
     */
    WifiErrorNo ReqServiceDiscovery(
        const std::string &deviceAddress, const std::vector<unsigned char> &tlvs, std::string &reqID) const;

    /**
     * @Description Sends a request for cancel request service discovery to the
     * P2P
     *
     * @param id
     * @return WifiErrorNo
     */
    WifiErrorNo CancelReqServiceDiscovery(const std::string &id) const;

    /**
     * @Description set enable/disable using random mac address
     *
     * @param enable
     * @return WifiErrorNo
     */
    WifiErrorNo SetRandomMacAddr(bool enable) const;

    /**
     * @Description Set the Miracast Type
     *
     * @param type
     * @return WifiErrorNo
     */
    WifiErrorNo SetMiracastMode(int type) const;

    /**
     * @Description Set the Persistent Reconnect mode.
     *
     * @param mode
     * @return WifiErrorNo
     */
    WifiErrorNo SetPersistentReconnect(int mode) const;

    /**
     * @Description
     *
     * @param deviceAddress
     * @param frequency
     * @param dialogToken
     * @param tlvs
     * @param tlvsLength
     * @return WifiErrorNo
     */
    WifiErrorNo RespServiceDiscovery(
        const WifiP2pDevice &device, int frequency, int dialogToken, const std::vector<unsigned char> &tlvs) const;

    /**
     * @Description Set the Service Discovery External.
     *
     * @param isExternalProcess
     * @return WifiErrorNo
     */
    WifiErrorNo SetServiceDiscoveryExternal(bool isExternalProcess) const;

    /**
     * @Description Show information about known P2P peer
     *
     * @param deviceAddress
     * @param device
     * @return WifiErrorNo
     */
    WifiErrorNo GetP2pPeer(const std::string &deviceAddress, WifiP2pDevice &device) const;

    /**
     * @Description Obtains the P2P frequency supported by a specified frequency band.
     *
     * @param band - Frequency band.
     * @param frequencies - Frequency list.
     * @return WifiErrorNo
     */
    WifiErrorNo P2pGetSupportFrequenciesByBand(int band, std::vector<int> &frequencies) const;

    /**
     * @Description Setting the P2P group config.
     *
     * @param networkId
     * @param config
     * @return WifiErrorNo
     */
    WifiErrorNo P2pSetGroupConfig(int networkId, const IdlP2pGroupConfig &config) const;

    /**
     * @Description Getting the P2P group config.
     *
     * @param networkId
     * @param config
     * @return WifiErrorNo
     */
    WifiErrorNo P2pGetGroupConfig(int networkId, IdlP2pGroupConfig &config) const;

    /**
     * @Description Request to obtain the next network ID.
     *
     * @param networkId
     * @return WifiErrorNo
     */
    WifiErrorNo P2pAddNetwork(int &networkId) const;

    /**
     * @Description Get register callback objects
     *
     * @return const P2pHalCallback& - register p2p callback objects
     */
    const P2pHalCallback &GetP2pCallbackInst(void) const;

    /**
     * @Description Sends a request for hid2d connect to the P2P
     *
     * @param config hid2d config
     * @return WifiErrorNo
     */
    WifiErrorNo Hid2dConnect(const Hid2dConnectConfig &config) const;

private:
    P2pHalCallback mP2pCallback;
};
}  // namespace Wifi
}  // namespace OHOS
#endif