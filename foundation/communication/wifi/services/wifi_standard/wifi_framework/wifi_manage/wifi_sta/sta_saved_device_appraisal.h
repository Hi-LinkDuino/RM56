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

#ifndef OHOS_WIFI_SAVEDDEVICEAPPRAISAL_H
#define OHOS_WIFI_SAVEDDEVICEAPPRAISAL_H

#include "wifi_log.h"
#include "sta_device_appraisal.h"
#include "wifi_internal_msg.h"

namespace OHOS {
namespace Wifi {
class StaSavedDeviceAppraisal : public StaDeviceAppraisal {
    FRIEND_GTEST(StaSavedDeviceAppraisal);
public:
    explicit StaSavedDeviceAppraisal(bool supportFmRoamingFlag);
    ~StaSavedDeviceAppraisal() override;
    /**
     * @Description  Evaluate the Saved device from the scanning result and
                    return the Wi-Fi configuration of the selected device.
     *
     * @param scanInfos - Scan details list constructed based on the scan result(in)
     * @param Info - Current device(in)
     * @param electedDevice - elected device(out)
     * @Return: Configuration of the selected device; Null if no devices are available in this category
     */
    ErrCode DeviceAppraisals(
        WifiDeviceConfig &electedDevice, std::vector<InterScanInfo> &scanInfos, WifiLinkedInfo &info) override;

private:
    static const int MAX_SIGNAL_BAR_NUM = 5;
    int signalScorePerLevel;
    int signalBaseScore;
    int sameBssidScore;
    int sameDeviceScore;
    int frequency5GHzScore;
    int userSelectedDeviceScore;
    int safetyDeviceScore;
    bool firmwareRoamFlag;

    /**
     * @Description  Scoring mechanism.
     *
     * @param scanInfo - Scan Information.(in)
     * @param device - Saved device.(in)
     * @param info - Connection information.(in)
     * @param score - score points.(out)
     */
    void AppraiseDeviceQuality(int &score, InterScanInfo &scanInfo, WifiDeviceConfig &device, WifiLinkedInfo &info);
    /**
     * @Description  Signal strength converted to grids.
     *
     * @param rssi - Signal strength(in)
     * @param signalBars - Max Bars(in)
     * @Return: signal Bars
     */

    bool WhetherSkipDevice(WifiDeviceConfig &device);
    int CalculateSignalBars(int rssi, int signalBars);
    /**
     * @Description  Whether the device is a 5G device.
     *
     * @param frequency(in)
     * @Return success: true; failed: false
     */
    bool Whether5GDevice(int frequency);
};
}  // namespace Wifi
}  // namespace OHOS
#endif