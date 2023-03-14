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

#ifndef I_WIFI_SCAN_H
#define I_WIFI_SCAN_H

#include <string_ex.h>
#include <iremote_broker.h>
#include "wifi_errcode.h"
#include "message_parcel.h"
#include "message_option.h"
#include "wifi_scan_msg.h"
#include "wifi_errcode.h"
#include "i_wifi_scan_callback.h"

namespace OHOS {
namespace Wifi {
class IWifiScan : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.wifi.IWifiScan");
    virtual ~IWifiScan()
    {}

    /**
     * @Description Set the Scan Control Info object
     *
     * @param info - ScanControlInfo object
     * @return ErrCode - operation result
     */
    virtual ErrCode SetScanControlInfo(const ScanControlInfo &info) = 0;

    /**
     * @Description Start scan Wifi
     *
     * @return ErrCode - operation result
     */
    virtual ErrCode Scan() = 0;

    /**
     * @Description Start scan with specified params
     *
     * @param params - WifiScanParams object
     * @return ErrCode - operation result
     */
    virtual ErrCode AdvanceScan(const WifiScanParams &params) = 0;

    /**
     * @Description Check whether the ScanAlways mode is enabled
     *
     * @param bOpen - true / false
     * @return ErrCode - operation result
     */
    virtual ErrCode IsWifiClosedScan(bool &bOpen) = 0;

    /**
     * @Description Obtain the scanning result
     *
     * @param result - Get result venctor of WifiScanInfo
     * @return ErrCode - operation result
     */
    virtual ErrCode GetScanInfoList(std::vector<WifiScanInfo> &result) = 0;

    virtual ErrCode RegisterCallBack(const sptr<IWifiScanCallback> &callback) = 0;

    /**
     * @Description Get supported features
     *
     * @param features - return supported features
     * @return ErrCode - operation result
     */
    virtual ErrCode GetSupportedFeatures(long &features) = 0;
};
}  // namespace Wifi
}  // namespace OHOS
#endif