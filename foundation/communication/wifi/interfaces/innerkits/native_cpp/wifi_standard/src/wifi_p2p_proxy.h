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
#ifndef OHOS_WIFI_P2P_PROXY_H
#define OHOS_WIFI_P2P_PROXY_H

#include "i_wifi_p2p.h"
#include "iremote_proxy.h"
#include "wifi_errcode.h"
#include "wifi_p2p_msg.h"

namespace OHOS {
namespace Wifi {
class WifiP2pProxy : public IRemoteProxy<IWifiP2p>, public IRemoteObject::DeathRecipient {
public:
    explicit WifiP2pProxy(const sptr<IRemoteObject> &impl);
    ~WifiP2pProxy();

    /**
     * @Description Enabling the P2P Mode
     *
     * @return ErrCode - operate result
     */
    ErrCode EnableP2p(void) override;

    /**
     * @Description Disable the P2P mode
     *
     * @return ErrCode - operate result
     */
    ErrCode DisableP2p(void) override;

    /**
     * @Description Start Wi-Fi P2P device search
     *
     * @return ErrCode - operate result
     */
    ErrCode DiscoverDevices(void) override;

    /**
     * @Description Stop Wi-Fi P2P device search
     *
     * @return ErrCode - operate result
     */
    ErrCode StopDiscoverDevices(void) override;

    /**
     * @Description Start the search for the Wi-Fi P2P service
     *
     * @return ErrCode - operate result
     */
    ErrCode DiscoverServices(void) override;

    /**
     * @Description Stop the search for the Wi-Fi P2P service
     *
     * @return ErrCode - operate result
     */
    ErrCode StopDiscoverServices(void) override;

    /**
     * @Description request the P2P service
     *
     * @param device - WifiP2pDevice object
     * @param request - WifiP2pServiceRequest object
     * @return ErrCode - operate result
     */
    ErrCode RequestService(const WifiP2pDevice &device, const WifiP2pServiceRequest &request) override;

    /**
     * @Description Register the local P2P service
     *
     * @param srvInfo - WifiP2pServiceInfo object
     * @return ErrCode - operate result
     */
    ErrCode PutLocalP2pService(const WifiP2pServiceInfo &srvInfo) override;

    /**
     * @Description Delete the local P2P service
     *
     * @param srvInfo - WifiP2pServiceInfo object
     * @return ErrCode - operate result
     */
    ErrCode DeleteLocalP2pService(const WifiP2pServiceInfo &srvInfo) override;

    /**
     * @Description Enable Wi-Fi P2P listening
     *
     * @param period - period
     * @param interval - interval
     * @return ErrCode - operate result
     */
    ErrCode StartP2pListen(int period, int interval) override;

    /**
     * @Description Disable Wi-Fi P2P listening
     *
     * @return ErrCode - operate result
     */
    ErrCode StopP2pListen(void) override;

    /**
     * @Description Creating a P2P Group
     *
     * @param config - WifiP2pGroupInfo object
     * @return ErrCode - operate result
     */
    ErrCode CreateGroup(const WifiP2pConfig &config) override;

    /**
     * @Description Remove a P2P Group
     *
     *
     * @return ErrCode - operate result
     */
    ErrCode RemoveGroup(void) override;

    /**
     * @Description Delete a p2p Group
     *
     * @param group - WifiP2pGroupInfo object
     * @return ErrCode - operate result
     */
    ErrCode DeleteGroup(const WifiP2pGroupInfo &group) override;

    /**
     * @Description P2P connection
     *
     * @param config - WifiP2pConfig object
     * @return ErrCode - operate result
     */
    ErrCode P2pConnect(const WifiP2pConfig &config) override;

    /**
     * @Description Canceling a P2P connection
     *
     * @return ErrCode - operate result
     */
    ErrCode P2pCancelConnect(void) override;

    /**
     * @Description Querying Wi-Fi P2P Connection Information
     *
     * @param linkedInfo - Get the WifiP2pLinkedInfo msg
     * @return ErrCode - operate result
     */
    ErrCode QueryP2pLinkedInfo(WifiP2pLinkedInfo &linkedInfo) override;

    /**
     * @Description Get the Current Group object
     *
     * @param group - the WifiP2pGroupInfo object
     * @return ErrCode - operate result
     */
    ErrCode GetCurrentGroup(WifiP2pGroupInfo &group) override;

    /**
     * @Description Obtains the P2P switch status
     *
     * @param status - the P2P switch status
     * @return ErrCode - operate result
     */
    ErrCode GetP2pEnableStatus(int &status) override;

    /**
     * @Description Obtains the P2P discovery status
     *
     * @param status - the P2P discovery status
     * @return ErrCode
     */
    ErrCode GetP2pDiscoverStatus(int &status) override;

    /**
     * @Description Obtains the P2P connection status
     *
     * @param status - the P2P connection status
     * @return ErrCode - operate result
     */
    ErrCode GetP2pConnectedStatus(int &status) override;

    /**
     * @Description Query the information about the found devices
     *
     * @param devices - Get result vector of WifiP2pDevice
     * @return ErrCode - operate result
     */
    ErrCode QueryP2pDevices(std::vector<WifiP2pDevice> &devices) override;

    /**
     * @Description Query the information about the found groups
     *
     * @param groups - Get result vector of WifiP2pGroupInfo
     * @return ErrCode - operate result
     */
    ErrCode QueryP2pGroups(std::vector<WifiP2pGroupInfo> &groups) override;

    /**
     * @Description Query the service information
     *
     * @param services - Get result vector of Device
     * @return ErrCode - operate result
     */
    ErrCode QueryP2pServices(std::vector<WifiP2pServiceInfo> &services) override;

    /**
     * @Description Register callback function
     *
     * @param callback - IWifiP2pCallback object
     * @return ErrCode - operate result
     */
    ErrCode RegisterCallBack(const sptr<IWifiP2pCallback> &callback) override;

    /**
     * @Description Get supported features
     *
     * @param features - return supported features
     * @return ErrCode - operation result
     */
    ErrCode GetSupportedFeatures(long &features) override;

    /**
     * @Description set the device name
     *
     * @param deviceName - device name
     * @return ErrCode - operate result
     */
    ErrCode SetP2pDeviceName(const std::string &deviceName) override;

    /**
     * @Description set p2p wifi display info
     *
     * @param wfdInfo - wifi display info
     * @return ErrCode - operate result
     */
    ErrCode SetP2pWfdInfo(const WifiP2pWfdInfo &wfdInfo) override;

    /**
     * @Description Request an IP address to the Gc from the IP address pool, used on the GO side.
     *
     * @param gcMac - gc mac address
     * @param ipAddr - applied ip address
     * @return ErrCode - operate result
     */
    ErrCode Hid2dRequestGcIp(const std::string& gcMac, std::string& ipAddr) override;

    /**
     * @Description Increase(+1) hid2d shared link reference counting
     *
     * @return ErrCode - operate result
     */
    ErrCode Hid2dSharedlinkIncrease() override;

    /**
     * @Description Decrease(-1) hid2d shared link reference counting
     *
     * @return ErrCode - operate result
     */
    ErrCode Hid2dSharedlinkDecrease() override;

    /**
     * @Description Create hid2d group, used on the GO side.
     *
     * @param frequency - frequency
     * @param type - frequency type
     * @return ErrCode - operate result
     */
    ErrCode Hid2dCreateGroup(const int frequency, FreqType type) override;

    /**
     * @Description The GC side actively disconnects from the GO, used on the GC side.
     *
     * @param gcIfName - network interface name
     * @return ErrCode - operate result
     */
    ErrCode Hid2dRemoveGcGroup(const std::string& gcIfName) override;

    /**
     * @Description Connect to a specified group using hid2d, used on the GC side.
     *
     * @param config - connection parameters
     * @return ErrCode - operate result
     */
    ErrCode Hid2dConnect(const Hid2dConnectConfig& config) override;

    /**
     * @Description Configuring IP addresses for P2P network interfaces, used on the GC side.
     *
     * @param ifName - network interface name
     * @param ipInfo - IP infos
     * @return ErrCode - operate result
     */
    ErrCode Hid2dConfigIPAddr(const std::string& ifName, const IpAddrInfo& ipInfo) override;

    /**
     * @Description Clear IP address when the P2P connection is disconnected, used on the GC side.
     *
     * @param ifName - network interface name
     * @return ErrCode - operate result
     */
    ErrCode Hid2dReleaseIPAddr(const std::string& ifName) override;

     /**
     * @Description Obtain the recommended channel and bandwidth for link setup
     *
     * @param request - request data
     * @param response - response result
     * @return ErrCode - operate result
     */
    ErrCode Hid2dGetRecommendChannel(const RecommendChannelRequest& request,
        RecommendChannelResponse& response) override;

    /**
     * @Description get 5G channel list
     *
     * @param vecChannelList - result for channel list
     * @return ErrCode - operate result
     */
    ErrCode Hid2dGetChannelListFor5G(std::vector<int>& vecChannelList) override;

    /**
     * @Description get the self wifi configuration information
     *
     * @param cfgType - configuration type
     * @param cfgData - the queried data of wifi configuration
     * @param getDatValidLen - the valid data length in the array `cfgData`
     * @return ErrCode - operate result
     */
    ErrCode Hid2dGetSelfWifiCfgInfo(SelfCfgType cfgType,
        char cfgData[CFG_DATA_MAX_BYTES], int* getDatValidLen) override;

    /**
     * @Description set the peer wifi configuration information
     *
     * @param cfgType - configuration type
     * @param cfgData - the wifi configuration data to be set
     * @param setDataValidLen - the valid data length in the array `cfgData`
     * @return ErrCode - operate result
     */
    ErrCode Hid2dSetPeerWifiCfgInfo(PeerCfgType cfgType,
        char cfgData[CFG_DATA_MAX_BYTES], int setDataValidLen) override;

    /**
    * @Description Handle remote object died event.
    * @param remoteObject remote object.
    */
    void OnRemoteDied(const wptr<IRemoteObject> &remoteObject) override;

private:
    void ReadWifiP2pServiceInfo(MessageParcel &reply, WifiP2pServiceInfo &info) const;
    void WriteWifiP2pServiceInfo(MessageParcel &data, const WifiP2pServiceInfo &info) const;
    void WriteWifiP2pServiceRequest(
        MessageParcel &data, const WifiP2pDevice &device, const WifiP2pServiceRequest &request) const;
    void WriteWifiP2pDeviceData(MessageParcel &data, const WifiP2pDevice &device) const;
    void ReadWifiP2pDeviceData(MessageParcel &reply, WifiP2pDevice &device) const;
    void WriteWifiP2pGroupData(MessageParcel &data, const WifiP2pGroupInfo &info) const;
    void ReadWifiP2pGroupData(MessageParcel &reply, WifiP2pGroupInfo &info) const;
    void WriteWifiP2pConfigData(MessageParcel &data, const WifiP2pConfig &config) const;

private:
    bool mRemoteDied;
    static BrokerDelegator<WifiP2pProxy> delegator;
};
}  // namespace Wifi
}  // namespace OHOS
#endif