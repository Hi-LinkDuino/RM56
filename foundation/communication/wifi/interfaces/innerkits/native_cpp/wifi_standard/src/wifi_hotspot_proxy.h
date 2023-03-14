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

#ifndef OHOS_WIFI_HOTSPOT_PROXY_H
#define OHOS_WIFI_HOTSPOT_PROXY_H

#include "iremote_proxy.h"
#include "i_wifi_hotspot.h"
#include "wifi_errcode.h"
#include "wifi_ap_msg.h"

namespace OHOS {
namespace Wifi {
class WifiHotspotProxy : public IRemoteProxy<IWifiHotspot>, public IRemoteObject::DeathRecipient {
public:
    explicit WifiHotspotProxy(const sptr<IRemoteObject> &impl);

    ~WifiHotspotProxy();

    /**
     * @Description Check whether the hotspot is active.
     *
     * @param bActive - active / inactive
     * @return ErrCode - operation result
     */
    ErrCode IsHotspotActive(bool &bActive) override;

    /**
     * @Description Get the Hotspot Config object
     *
     * @param config - HotapotConfig object
     * @return ErrCode - operation result
     */
    ErrCode GetHotspotConfig(HotspotConfig &config) override;

    /**
     * @Description Get the Hotspot State object
     *
     * @param state - current Hotspot state
     * @return ErrCode - operation result
     */
    ErrCode GetHotspotState(int &state) override;

    /**
     * @Description Set the configuration of Hotspot
     *
     * @param config - HotspotConfig object
     * @return ErrCode - operation result
     */
    ErrCode SetHotspotConfig(const HotspotConfig &config) override;

    /**
     * @Description Get the Station List object
     *
     * @param result - Get result vector of connect Station Info
     * @return ErrCode - operation result
     */
    ErrCode GetStationList(std::vector<StationInfo> &result) override;

    /**
     * @Description Disconnects a specified sta connection
     *
     * @param info - Station object
     * @return ErrCode - operation result
     */
    ErrCode DisassociateSta(const StationInfo &info) override;

    /**
     * @Description Enable Hotspot
     *
     * @return ErrCode - operation result
     */
    ErrCode EnableHotspot(void) override;

    /**
     * @Description Disable Hotspot
     *
     * @return ErrCode - operation result
     */
    ErrCode DisableHotspot(void) override;

    /**
     * @Description Get the Block Lists object
     *
     * @param infos - Get Blocklist result vector of StationInfo
     * @return ErrCode - operation result
     */
    ErrCode GetBlockLists(std::vector<StationInfo> &infos) override;

    /**
     * @Description Add a StationInfo object to Blocklist
     *
     * @param info - Station object
     * @return ErrCode - operation result
     */
    ErrCode AddBlockList(const StationInfo &info) override;

    /**
     * @Description Del a StationInfo object from Blocklist
     *
     * @param info - StationInfo object
     * @return ErrCode - operation result
     */
    ErrCode DelBlockList(const StationInfo &info) override;

    /**
     * @Description Get the Valid Bands object
     *
     * @param bands - Get result vector of BandType
     * @return ErrCode - operation result
     */
    ErrCode GetValidBands(std::vector<BandType> &bands) override;

    /**
     * @Description Get the Valid Channels object
     *
     * @param band - Specified band
     * @param validchannels - Obtains the channels corresponding to the specified band
     * @return ErrCode - operation result
     */
    ErrCode GetValidChannels(BandType band, std::vector<int32_t> &validchannels) override;

    /**
     * @Description Register callback client
     *
     * @param callback - callback struct
     * @return ErrCode - operation result
     */
    ErrCode RegisterCallBack(const sptr<IWifiHotspotCallback> &callback) override;

    /**
     * @Description Get supported features
     *
     * @param features - return supported features
     * @return ErrCode - operation result
     */
    ErrCode GetSupportedFeatures(long &features) override;

    /**
    * @Description Handle remote object died event.
    * @param remoteObject remote object.
    */
    void OnRemoteDied(const wptr<IRemoteObject>& remoteObject) override;

private:
    static BrokerDelegator<WifiHotspotProxy> g_delegator;

    bool mRemoteDied;
};
}  // namespace Wifi
}  // namespace OHOS
#endif