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

#ifndef OHOS_WIFI_SCAN_IMPL
#define OHOS_WIFI_SCAN_IMPL

#include "wifi_scan.h"
#include "wifi_scan_proxy.h"

namespace OHOS {
namespace Wifi {
class WifiScanImpl : public Wifi::WifiScan {
public:
    explicit WifiScanImpl(int systemAbilityId);
    ~WifiScanImpl();
    bool Init();

    /**
     * @Description Set the Scan Control Info object
     *
     * @param info - ScanControlInfo object
     * @return ErrCode - operation result
     */
    virtual ErrCode SetScanControlInfo(const ScanControlInfo &info) override;

    /**
     * @Description Start scan Wifi
     *
     * @return ErrCode - operation result
     */
    virtual ErrCode Scan() override;

    /**
     * @Description Obtain the scanning result
     *
     * @param result - Get result venctor of WifiScanInfo
     * @return ErrCode - operation result
     */
    virtual ErrCode GetScanInfoList(std::vector<WifiScanInfo> &result) override;

    virtual ErrCode RegisterCallBack(const sptr<IWifiScanCallback> &callback) override;

    /**
     * @Description Get supported features
     *
     * @param features - return supported features
     * @return ErrCode - operation result
     */
    ErrCode GetSupportedFeatures(long &features) override;

    /**
     * @Description Check if supported input feature
     *
     * @param feature - input feature
     * @return true - supported
     * @return false - unsupported
     */
    bool IsFeatureSupported(long feature) override;

    /**
     * @Description Check whether the ScanAlways mode is enabled
     *
     * @param bOpen - true / false
     * @return ErrCode - operation result
     */
    ErrCode IsWifiClosedScan(bool &bOpen);

    /**
     * @Description Start scan with specified params
     *
     * @param params - WifiScanParams object
     * @return ErrCode - operation result
     */
    ErrCode AdvanceScan(const WifiScanParams &params);

private:
    int systemAbilityId_;
    sptr<OHOS::Wifi::IWifiScan> client_;
};
}  // namespace Wifi
}  // namespace OHOS
#endif