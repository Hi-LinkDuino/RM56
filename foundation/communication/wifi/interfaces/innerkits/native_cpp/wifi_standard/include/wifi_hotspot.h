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

#ifndef OHOS_WIFI_HOTSPOT_H
#define OHOS_WIFI_HOTSPOT_H

#include <string>
#include <vector>
#include <memory>

#include "wifi_errcode.h"
#include "wifi_ap_msg.h"
#include "i_wifi_hotspot_callback.h"

namespace OHOS {
namespace Wifi {
class WifiHotspot {
public:
    static std::unique_ptr<WifiHotspot> CreateWifiHotspot(int system_ability_id);

    static std::unique_ptr<WifiHotspot> GetInstance(int system_ability_id);

    virtual ~WifiHotspot();

    /**
     * @Description Check whether the hotspot is active.
     *
     * @return bool - operation result
     */
    virtual bool IsHotspotActive(void) = 0;

    /**
     * @Description Get the Hotspot Config object
     *
     * @param state - Result of obtaining the hotspot status
     * @return ErrCode - operation result
     */
    virtual ErrCode GetHotspotState(int &state) = 0;

    /**
     * @Description Get the Hotspot State object
     *
     * @param config - Current hotspot configuration
     * @return ErrCode - operation result
     */
    virtual ErrCode GetHotspotConfig(HotspotConfig &config) = 0;

    /**
     * @Description Set the configuration of Hotspot
     *
     * @param config - HotspotConfig object,
     * @return ErrCode - operation result
     */
    virtual ErrCode SetHotspotConfig(const HotspotConfig &config) = 0;

    /**
     * @Description Get the Station List object
     *
     * @param result - Get result vector of connect Station Info
     * @return ErrCode - operation result
     */
    virtual ErrCode GetStationList(std::vector<StationInfo> &result) = 0;

    /**
     * @Description Disconnects a specified sta connection when ap is opened
     *
     * @param info - StationInfo object
     * @return ErrCode - operation result
     */
    virtual ErrCode DisassociateSta(const StationInfo &info) = 0;

    /**
     * @Description Enable Hotspot
     *
     * @return ErrCode - operation result
     */
    virtual ErrCode EnableHotspot(void) = 0;

    /**
     * @Description Disable Hotspot
     *
     * @return ErrCode - operation result
     */
    virtual ErrCode DisableHotspot(void) = 0;

    /**
     * @Description Get the Block Lists object
     *
     * @param infos - Get Blocklist result vector of StationInfo
     * @return ErrCode - operation result
     */
    virtual ErrCode GetBlockLists(std::vector<StationInfo> &infos) = 0;

    /**
     * @Description Add a StationInfo object to Blocklist when ap is opened
     *
     * @param info - StationInfo object
     * @return ErrCode - operation result
     */
    virtual ErrCode AddBlockList(const StationInfo &info) = 0;

    /**
     * @Description Del a StationInfo object from Blocklist
     *
     * @param info - StationInfo object
     * @return ErrCode - operation result
     */
    virtual ErrCode DelBlockList(const StationInfo &info) = 0;

    /**
     * @Description Get the Valid Bands object
     *
     * @param bands - Get result vector of BandType when ap is opened
     * @return ErrCode - operation result
     */
    virtual ErrCode GetValidBands(std::vector<BandType> &bands) = 0;

    /**
     * @Description Get the Valid Channels object when ap is opened
     *
     * @param band - Specified Valid Band.
     * @param validchannels - Obtains the channels corresponding to the specified band
     * @return ErrCode - operation result
     */
    virtual ErrCode GetValidChannels(BandType band, std::vector<int32_t> &validchannels) = 0;

    /**
     * @Description Register callback client
     *
     * @param callback - callback struct
     * @return ErrCode - operation result
     */
    virtual ErrCode RegisterCallBack(const sptr<IWifiHotspotCallback> &callback) = 0;

    /**
     * @Description Get supported features
     *
     * @param features - return supported features
     * @return ErrCode - operation result
     */
    virtual ErrCode GetSupportedFeatures(long &features) = 0;

    /**
     * @Description Check if supported input feature
     *
     * @param feature - input feature
     * @return true - supported
     * @return false - unsupported
     */
    virtual bool IsFeatureSupported(long feature) = 0;
};
}  // namespace Wifi
}  // namespace OHOS
#endif