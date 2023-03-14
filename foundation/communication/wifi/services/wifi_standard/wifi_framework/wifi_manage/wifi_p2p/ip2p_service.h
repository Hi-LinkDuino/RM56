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

#ifndef OHOS_IP2P_SERVICE_H
#define OHOS_IP2P_SERVICE_H

#include "wifi_errcode.h"
#include "wifi_msg.h"
#include "ip2p_service_callbacks.h"
#include "wifi_hid2d_msg.h"

namespace OHOS {
namespace Wifi {
class IP2pService {
public:
    /**
     * @Description Destroy the IP2pService object.
     */
    virtual ~IP2pService() = default;

    /**
     * @Description - The interface of enable p2p.
     * @return - ErrCode
     */
    virtual ErrCode EnableP2p() = 0;

    /**
     * @Description - The interface of disable p2p.
     * @return - ErrCode
     */
    virtual ErrCode DisableP2p() = 0;

    /**
     * @Description - The interface of start discover peers.
     * @return - ErrCode
     */
    virtual ErrCode DiscoverDevices() = 0;

    /**
     * @Description - The interface of stop discover peers.
     * @return - ErrCode
     */
    virtual ErrCode StopDiscoverDevices() = 0;

    /**
     * @Description - The interface of start discover services.
     * @return - ErrCode
     */
    virtual ErrCode DiscoverServices() = 0;

    /**
     * @Description - The interface of stop discover services.
     * @return - ErrCode
     */
    virtual ErrCode StopDiscoverServices() = 0;

    /**
     * @Description - The interface of add local p2p service.
     * @param  srvInfo - information of service.
     * @return - ErrCode
     */
    virtual ErrCode PutLocalP2pService(const WifiP2pServiceInfo &srvInfo) = 0;

    /**
     * @Description - The interface of delete local p2p service.
     * @param  srvInfo - information of service.
     * @return - ErrCode
     */
    virtual ErrCode DeleteLocalP2pService(const WifiP2pServiceInfo &srvInfo) = 0;

    /**
     * @Description - The interface of add service request.
     * @param  device - target device information.
     * @param  request - request information.
     * @return - ErrCode
     */
    virtual ErrCode RequestService(const WifiP2pDevice &device, const WifiP2pServiceRequest &request) = 0;

    /**
     * @Description - The interface of start p2p listen(milliseconds).
     * @param  period - time of period.
     * @param  interval - time of interval.
     * @return - ErrCode
     */
    virtual ErrCode StartP2pListen(int period, int interval) = 0;

    /**
     * @Description - The interface of stop p2p listen.
     * @return - ErrCode
     */
    virtual ErrCode StopP2pListen() = 0;

    /**
     * @DescriptionCreate - The interface of create group.
     * @param  config - configure of group.
     * @return - ErrCode
     */
    virtual ErrCode CreateGroup(const WifiP2pConfig &config) = 0;

    /**
     * @Description - The interface of remove current group.
     * @return - ErrCode
     */
    virtual ErrCode RemoveGroup() = 0;

    /**
     * @Description - The interface of delete a saved group.
     * @param  group - information of group.
     * @return - ErrCode
     */
    virtual ErrCode DeleteGroup(const WifiP2pGroupInfo &group) = 0;

    /**
     * @Description - The interface of p2p connect.
     * @param  config - configure of connect.
     * @return - ErrCode
     */
    virtual ErrCode P2pConnect(const WifiP2pConfig &config) = 0;

    /**
     * @Description - The interface of canceling a p2p connection.
     * @return - ErrCode
     */
    virtual ErrCode P2pCancelConnect() = 0;
    /**
     * @Description - Set this device name.
     *
     * @param devName - specified device name
     * @return ErrCode
     */
    virtual ErrCode SetP2pDeviceName(const std::string &devName) = 0;
    /**
     * @Description - The interface of query p2p information like the group state,device information and ip address.
     * @param  linkedInfo - struct WifiP2pLinkedInfo.
     * @return - ErrCode
     */
    virtual ErrCode QueryP2pLinkedInfo(WifiP2pLinkedInfo &linkedInfo) = 0;

    /**
     * @DescriptionGet - The interface of get current group information.
     * @param  group - struct WifiP2pGroupInfo.
     * @return - ErrCode
     */
    virtual ErrCode GetCurrentGroup(WifiP2pGroupInfo &group) = 0;

    /**
     * @Description - The interface of get p2p running status.
     * @param  status - information of status.
     * @return - ErrCode
     */
    virtual ErrCode GetP2pEnableStatus(int &status) = 0;

    /**
     * @Description - The interface of get p2p discover status.
     * @param  status - information of status.
     * @return - ErrCode
     */
    virtual ErrCode GetP2pDiscoverStatus(int &status) = 0;

    /**
     * @Description - The interface of get p2p connected status.
     * @param  status - information of status.
     * @return - ErrCode
     */
    virtual ErrCode GetP2pConnectedStatus(int &status) = 0;

    /**
     * @Description - The interface of query p2p devices information.
     * @param  devives - information of devices.
     * @return - ErrCode
     */
    virtual ErrCode QueryP2pDevices(std::vector<WifiP2pDevice> &devives) = 0;

    /**
     * @Description - Query the information about own device.
     * @param  device - own device
     * @return - ErrCode
     */
    virtual ErrCode QueryP2pLocalDevice(WifiP2pDevice &device) = 0;

    /**
     * @Description - The interface of query p2p group information.
     * @param  groups - information of groups.
     * @return - ErrCode
     */
    virtual ErrCode QueryP2pGroups(std::vector<WifiP2pGroupInfo> &groups) = 0;

    /**
     * @Description - The interface of query p2p services information.
     * @param  services - information of services.
     * @return - ErrCode
     */
    virtual ErrCode QueryP2pServices(std::vector<WifiP2pServiceInfo> &services) = 0;

    /**
     * @Description - The interface of register p2p service callbacks,
     * @param  callbacks - information of callbacks.
     * @return - ErrCode
     */
    virtual ErrCode RegisterP2pServiceCallbacks(const IP2pServiceCallbacks &callbacks) = 0;

    /**
     * @Description set p2p wifi display info
     *
     * @param wfdInfo - wifi display info
     * @return ErrCode - operate result
     */
    virtual ErrCode SetP2pWfdInfo(const WifiP2pWfdInfo &wfdInfo) = 0;

    /**
     * @Description Create hid2d group, used on the GO side.
     *
     * @param frequency - frequency
     * @param type - frequency type
     * @return ErrCode - operate result
     */
    virtual ErrCode Hid2dCreateGroup(const int frequency, FreqType type) = 0;

    /**
     * @Description Connect to a specified group using hid2d, used on the GC side.
     *
     * @param config - connection parameters
     * @return ErrCode - operate result
     */
    virtual ErrCode Hid2dConnect(const Hid2dConnectConfig& config) = 0;
};
} // namespace Wifi
} // namespace OHOS

#endif  // OHOS_IP2P_SERVICE_H
