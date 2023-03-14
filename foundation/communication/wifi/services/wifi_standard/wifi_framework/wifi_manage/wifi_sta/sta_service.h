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

#ifndef OHOS_WIFI_SERVICE_H
#define OHOS_WIFI_SERVICE_H

#include "wifi_internal_msg.h"
#include "sta_auto_connect_service.h"
#include "sta_monitor.h"
#include "sta_state_machine.h"

namespace OHOS {
namespace Wifi {
class StaService {
    FRIEND_GTEST(StaService);
public:
    StaService();
    virtual ~StaService();
    /**
     * @Description  Initialize StaService module.
     *
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode InitStaService(const StaServiceCallback &callbacks);
    /**
     * @Description  Enable wifi
     *
     * @Output: Return operating results to Interface Service after enable wifi
               successfully through callback function instead of returning
               result immediately.
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode EnableWifi() const;
    /**
     * @Description  Disable wifi
     *
     * @Output: Return operating results to Interface Service after enable wifi
                successfully through callback function instead of returning
                result immediately.
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode DisableWifi() const;
    /**
     * @Description  Connect to a new network
     *
     * @param config - the configuration of network which is going to connect.(in)
     * @Output: Return operating results to Interface Service after enable wifi
                successfully through callback function instead of returning
                result immediately.
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode ConnectToDevice(const WifiDeviceConfig &config) const;
    /**
     * @Description  Connecting to a specified network.
     *
     * @param networkId - interior saved network index.(in)
     * @Output: Return operating results to Interface Service after enable wifi
                successfully through callback function instead of returning
                result immediately.
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode ConnectToNetwork(int networkId) const;
    /**
     * @Description  Disconnect to the network
     *
     * @Output: Return operating results to Interface Service after enable wifi
                successfully through callback function instead of returning
                result immediately.
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode Disconnect() const;
    /**
     * @Description  ReAssociate network
     *
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode ReAssociate() const;

    /**
     * @Description  Update a network to config
     *
     * @param config -The Network info(in)
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual int AddDeviceConfig(const WifiDeviceConfig &config) const;
    /**
     * @Description Update a network to config.
     *
     * @param config -The Network info(in)
     * @Return success: networkId  fail: -1
     */
    virtual int UpdateDeviceConfig(const WifiDeviceConfig &config) const;
    /**
     * @Description  Remove network config.
     *
     * @param networkId -The NetworkId is going to be removed.(in)
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode RemoveDevice(int networkId) const;
    /**
     * @Description  Remove all network configs.
     *
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode RemoveAllDevice() const;
    /**
     * @Description  Enable WI-FI device Configuration.
     *
     * @param networkId - The NetworkId (in)
     * @param networkId - if set true, disable other device config (in)
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode EnableDeviceConfig(int networkId, bool attemptEnable) const;
    /**
     * @Description Disable WI-FI device Configuration.
     *
     * @param networkId - device Configuration's network id
     * @return ErrCode - success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode DisableDeviceConfig(int networkId) const;
    /**
     * @Description  Start WPS Connection
     *
     * @Output: Return operating results to Interface Service after enable wifi
                successfully through callback function instead of returning
                result immediately.
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode StartWps(const WpsConfig &config) const;
    /**
     * @Description  Close WPS Connection
     *
     * @Output: Return operating results to Interface Service after enable wifi
                successfully through callback function instead of returning
                result immediately.
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode CancelWps() const;
    /**
     * @Description  Set country code
     *
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode SetCountryCode(const std::string &countryCode) const;
    /**
     * @Description  ConnectivityManager process scan results.
     *
     * @Output: Return operating results to Interface Service after enable wifi
                successfully through callback function instead of returning
                result immediately.
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode AutoConnectService(const std::vector<InterScanInfo> &scanInfos);
    /**
     * @Description Register sta callback function
     *
     * @param callbacks - Callback function pointer storage structure
     */
    virtual void RegisterStaServiceCallback(const StaServiceCallback &callbacks) const;

    /**
     * @Description  Reconnect network
     *
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode ReConnect() const;

private:
    StaStateMachine *pStaStateMachine;
    StaMonitor *pStaMonitor;
    StaAutoConnectService *pStaAutoConnectService;
};
}  // namespace Wifi
}  // namespace OHOS
#endif