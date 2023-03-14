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
#ifndef OHOS_AP_SERVICE_H
#define OHOS_AP_SERVICE_H

#include "ap_define.h"
#include "wifi_internal_msg.h"
#include "i_ap_service.h"
#include "i_ap_service_callbacks.h"

namespace OHOS {
namespace Wifi {
class ApStateMachine;
class ApService {
    FRIEND_GTEST(ApService);

public:
    /**
     * @Description  construction method.
     * @param None
     * @return None
     */
    ApService(ApStateMachine &);

    /**
     * @Description  destructor method.
     * @param None
     * @return None
     */
    ~ApService();
    DISALLOW_COPY_AND_ASSIGN(ApService)

    /**
     * @Description  open hotspot.
     * @param None
     * @return ErrCode - success: WIFI_OPT_SUCCESS    failed: ERROR_CODE
     */
    ErrCode EnableHotspot() const;

    /**
     * @Description  close hotspot.
     * @param None
     * @return ErrCode - success: WIFI_OPT_SUCCESS    failed: ERROR_CODE
     */
    ErrCode DisableHotspot() const;

    /**
     * @Description  set ap config.
     * @param cfg - ap config
     * @return ErrCode - success: WIFI_OPT_SUCCESS    failed: ERROR_CODE
     */
    ErrCode SetHotspotConfig(const HotspotConfig &cfg) const;

    /**
     * @Description  add block list
     * @param stationInfo - sta infos
     * @return ErrCode - success: WIFI_OPT_SUCCESS    failed: ERROR_CODE
     */
    ErrCode AddBlockList(const StationInfo &stationInfo) const;

    /**
     * @Description  delete block list.
     * @param stationInfo - sta infos
     * @return ErrCode - success: WIFI_OPT_SUCCESS    failed: ERROR_CODE
     */
    ErrCode DelBlockList(const StationInfo &stationInfo) const;

    /**
     * @Description  Disconnect a specified STA.
     * @param stationInfo - sta infos
     * @return ErrCode - success: WIFI_OPT_SUCCESS    failed: ERROR_CODE
     */
    ErrCode DisconnetStation(const StationInfo &stationInfo) const;

    /**
     * @Description Get the Station List object.
     *
     * @param result - current connected station info
     * @return ErrCode - success: WIFI_OPT_SUCCESS    failed: ERROR_CODE
     */
    ErrCode GetStationList(std::vector<StationInfo> &result) const;

    /**
     * @Description Get valid bands.
     *
     * @param bands - return valid bands
     * @return ErrCode - success: WIFI_OPT_SUCCESS    failed: ERROR_CODE
     */
    ErrCode GetValidBands(std::vector<BandType> &bands);

    /**
     * @Description Get valid channels.
     *
     * @param band - input band
     * @param validchannel - band's valid channel
     * @return ErrCode - success: WIFI_OPT_SUCCESS    failed: ERROR_CODE
     */
    ErrCode GetValidChannels(BandType band, std::vector<int32_t> &validchannel);

    /**
     * @Description Sets the callback function for the state machine.
     *
     * @param callbacks - callbacks list.
     * @return ErrCode - success: WIFI_OPT_SUCCESS    failed: ERROR_CODE
     */
    ErrCode RegisterApServiceCallbacks(const IApServiceCallbacks &callbacks);

private:
    ApStateMachine &m_ApStateMachine;
};
}  // namespace Wifi
}  // namespace OHOS

#endif