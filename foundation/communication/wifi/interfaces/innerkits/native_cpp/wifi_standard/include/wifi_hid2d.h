/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_WIFI_HID2D_H
#define OHOS_WIFI_HID2D_H

#include "wifi_errcode.h"
#include <vector>
#include "wifi_hid2d_msg.h"

namespace OHOS {
namespace Wifi {
class Hid2d {
public:
    static std::unique_ptr<Hid2d> CreateWifiHid2d(int system_ability_id);
    static std::unique_ptr<Hid2d> GetInstance(int system_ability_id);

    virtual ~Hid2d();

    /**
     * @Description Request an IP address to the Gc from the IP address pool, used on the GO side.
     *
     * @param gcMac - gc mac address
     * @param ipAddr - applied ip address
     * @return ErrCode - operate result
     */
    virtual ErrCode Hid2dRequestGcIp(const std::string& gcMac, std::string& ipAddr) = 0;

    /**
     * @Description Increase(+1) hid2d shared link reference counting
     *
     * @return ErrCode - operate result
     */
    virtual ErrCode Hid2dSharedlinkIncrease() = 0;

    /**
     * @Description Decrease(-1) hid2d shared link reference counting
     *
     * @return ErrCode - operate result
     */
    virtual ErrCode Hid2dSharedlinkDecrease() = 0;

    /**
     * @Description Create hid2d group, used on the GO side.
     *
     * @param frequency - frequency
     * @param type - frequency type
     * @return ErrCode - operate result
     */
    virtual ErrCode Hid2dCreateGroup(const int frequency, FreqType type) = 0;

    /**
     * @Description The GC side actively disconnects from the GO, used on the GC side.
     *
     * @param gcIfName - network interface name
     * @return ErrCode - operate result
     */
    virtual ErrCode Hid2dRemoveGcGroup(const std::string& gcIfName) = 0;

    /**
     * @Description Connect to a specified group using hid2d, used on the GC side.
     *
     * @param config - connection parameters
     * @return ErrCode - operate result
     */
    virtual ErrCode Hid2dConnect(const Hid2dConnectConfig& config) = 0;

    /**
     * @Description Configuring IP addresses for P2P network interfaces, used on the GC side.
     *
     * @param ifName - network interface name
     * @param ipInfo - IP infos
     * @return ErrCode - operate result
     */
    virtual ErrCode Hid2dConfigIPAddr(const std::string& ifName, const IpAddrInfo& ipInfo) = 0;

    /**
     * @Description Clear IP address when the P2P connection is disconnected, used on the GC side.
     *
     * @param ifName - network interface name
     * @return ErrCode - operate result
     */
    virtual ErrCode Hid2dReleaseIPAddr(const std::string& ifName) = 0;

     /**
     * @Description Obtain the recommended channel and bandwidth for link setup
     *
     * @param request - request data
     * @param response - response result
     * @return ErrCode - operate result
     */
    virtual ErrCode Hid2dGetRecommendChannel(const RecommendChannelRequest& request,
        RecommendChannelResponse& response) = 0;

    /**
     * @Description get 5G channel list
     *
     * @param vecChannelList - result for channel list
     * @return ErrCode - operate result
     */
    virtual ErrCode Hid2dGetChannelListFor5G(std::vector<int>& vecChannelList) = 0;

    /**
     * @Description get the self wifi configuration information
     *
     * @param cfgType - configuration type
     * @param cfgData - the queried data of wifi configuration
     * @param getDatValidLen - the valid data length in the array `cfgData`
     * @return ErrCode - operate result
     */
    virtual ErrCode Hid2dGetSelfWifiCfgInfo(SelfCfgType cfgType,
        char cfgData[CFG_DATA_MAX_BYTES], int* getDatValidLen) = 0;

    /**
     * @Description set the peer wifi configuration information
     *
     * @param cfgType - configuration type
     * @param cfgData - the wifi configuration data to be set
     * @param setDataValidLen - the valid data length in the array `cfgData`
     * @return ErrCode - operate result
     */
    virtual ErrCode Hid2dSetPeerWifiCfgInfo(PeerCfgType cfgType,
        char cfgData[CFG_DATA_MAX_BYTES], int setDataValidLen) = 0;
};
}  // namespace Wifi
}  // namespace OHOS
#endif