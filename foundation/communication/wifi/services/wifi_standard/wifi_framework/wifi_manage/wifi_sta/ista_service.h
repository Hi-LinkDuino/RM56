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

#ifndef OHOS_WIFI_STA_SERVICE_H
#define OHOS_WIFI_STA_SERVICE_H

#include "wifi_errcode.h"
#include "wifi_msg.h"
#include "sta_service_callback.h"

namespace OHOS {
namespace Wifi {
class IStaService {
public:
    virtual ~IStaService() = default;
    /**
     * @Description  Enable wifi
     *
     * @Output: Return operating results to Interface Service after enable wifi
               successfully through callback function instead of returning
               result immediately.
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode EnableWifi() = 0;
    /**
     * @Description  Disable wifi
     *
     * @Output: Return operating results to Interface Service after enable wifi
                successfully through callback function instead of returning
                result immediately.
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode DisableWifi() = 0;
    /**
     * @Description  Connect to a new network
     *
     * @param config - the configuration of network which is going to connect.(in)
     * @Output: Return operating results to Interface Service after enable wifi
                successfully through callback function instead of returning
                result immediately.
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode ConnectToDevice(const WifiDeviceConfig &config) = 0;
    /**
     * @Description  Connecting to a specified network.
     *
     * @param networkId - interior saved network index.(in)
     * @Output: Return operating results to Interface Service after enable wifi
                successfully through callback function instead of returning
                result immediately.
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode ConnectToNetwork(int networkId) = 0;
    /**
     * @Description  Disconnect to the network
     *
     * @Output: Return operating results to Interface Service after enable wifi
                successfully through callback function instead of returning
                result immediately.
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode Disconnect() = 0;
    /**
     * @Description  ReConnect network
     *
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode ReConnect() = 0;
    /**
     * @Description  ReAssociate network
     *
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode ReAssociate() = 0;
    /**
     * @Description  Add a network to config
     *
     * @param config -The Network info(in)
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual int AddDeviceConfig(const WifiDeviceConfig &config) = 0;
    /**
     * @Description  Update a network to config
     *
     * @param config -The Network info(in)
     * @Return success: networkId  fail: -1
     */
    virtual int UpdateDeviceConfig(const WifiDeviceConfig &config) = 0;
    /**
     * @Description  Remove network
     *
     * @param networkId -The NetworkId is going to be removed.(in)
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode RemoveDevice(int networkId) = 0;
    /**
     * @Description  Remove all network configs
     *
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode RemoveAllDevice() = 0;
    /**
     * @Description  Enable WI-FI device configuration
     *
     * @param networkId - The NetworkId (in)
     * @param networkId - if set true, disable other device configuration (in)
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode EnableDeviceConfig(int networkId, bool attemptEnable) = 0;
    /**
     * @Description Disable WI-FI device configuration
     *
     * @param networkId - device configuration's network id
     * @return ErrCode - success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode DisableDeviceConfig(int networkId) = 0;
    /**
     * @Description  Start WPS Connection
     *
     * @Output: Return operating results to Interface Service after enable wifi
                successfully through callback function instead of returning
                result immediately.
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode StartWps(const WpsConfig &config) = 0;
    /**
     * @Description  Close WPS Connection
     *
     * @Output: Return operating results to Interface Service after enable wifi
                successfully through callback function instead of returning
                result immediately.
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode CancelWps() = 0;
    /**
     * @Description  Set country code
     *
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode SetCountryCode(const std::string &countryCode) = 0;
    /**
     * @Description  ConnectivityManager process scan results.
     *
     * @Output: Return operating results to Interface Service after enable wifi
                successfully through callback function instead of returning
                result immediately.
     * @Return success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode ConnectivityManager(const std::vector<InterScanInfo> &scanInfos) = 0;
    /**
     * @Description Register sta callback function
     *
     * @param callbacks - Callback function pointer storage structure
     * @return ErrCode - success: WIFI_OPT_SUCCESS  fail: WIFI_OPT_FAILED
     */
    virtual ErrCode RegisterStaServiceCallback(const StaServiceCallback &callbacks) = 0;
};
}  // namespace Wifi
}  // namespace OHOS
#endif