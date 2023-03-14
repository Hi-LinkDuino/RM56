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

#ifndef OHOS_I_WIFI_HOTSPOT_H
#define OHOS_I_WIFI_HOTSPOT_H

#include "iremote_broker.h"
#include "message_parcel.h"
#include "message_option.h"
#include "wifi_errcode.h"
#include "wifi_ap_msg.h"
#include "define.h"
#include "wifi_errcode.h"
#include "i_wifi_hotspot_callback.h"

namespace OHOS {
namespace Wifi {
class IWifiHotspot : public IRemoteBroker {
public:
    virtual ~IWifiHotspot()
    {}

    /**
     * @Description Check whether the hotspot is active.
     *
     * @param bActive - active / inactive
     * @return ErrCode - operation result
     */
    virtual ErrCode IsHotspotActive(bool &bActive) = 0;

    /**
     * @Description Get the Hotspot Config object
     *
     * @param config - HotapotConfig object
     * @return ErrCode - operation result
     */
    virtual ErrCode GetHotspotConfig(HotspotConfig &config) = 0;

    /**
     * @Description Get the Hotspot State object
     *
     * @param state - current Hotspot state
     * @return ErrCode - operation result
     */
    virtual ErrCode GetHotspotState(int &state) = 0;

    /**
     * @Description Set the configuration of Hotspot
     *
     * @param config - HotspotConfig object
     * @return ErrCode - operation result
     */
    virtual ErrCode SetHotspotConfig(const HotspotConfig &config) = 0;

    /**
     * @Description Get the Station List object.
     *
     * @param result - Get result vector of connect Station Info
     * @return ErrCode - operation result
     */
    virtual ErrCode GetStationList(std::vector<StationInfo> &result) = 0;

    /**
     * @Description Disconnects a specified sta connection
     *
     * @param info - Station object
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
     * @Description Add a StationInfo object to Blocklist
     *
     * @param info - Station object
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
     * @param bands - Get result vector of BandType
     * @return ErrCode - operation result
     */
    virtual ErrCode GetValidBands(std::vector<BandType> &bands) = 0;

    /**
     * @Description Get the Valid Channels object
     *
     * @param band - Specified band
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
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.wifi.IWifiHotspotService");
};
}  // namespace Wifi
}  // namespace OHOS
#endif