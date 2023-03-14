/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_MOCK_WIFI_AP_HAL_INTERFACE_H
#define OHOS_MOCK_WIFI_AP_HAL_INTERFACE_H

#include <gmock/gmock.h>
#include <map>
#include <string>
#include <vector>

#include "i_wifi_struct.h"
#include "wifi_error_no.h"
#include "wifi_ap_msg.h"
#include <map>
#include <string>
#include <vector>

namespace OHOS {
namespace Wifi {
typedef struct IWifiApMonitorEventCallback {
    std::function<void(CStationInfo *)> onStaJoinOrLeave;
    std::function<void(int)> onApEnableOrDisable;
} IWifiApMonitorEventCallback;
class MockWifiApHalInterface {
public:
    virtual ~MockWifiApHalInterface() = default;
    virtual WifiErrorNo StartAp(void) = 0;
    virtual WifiErrorNo StopAp(void) = 0;
    virtual WifiErrorNo StopAp1(void) = 0;
    virtual WifiErrorNo SetSoftApConfig(const HotspotConfig &config) = 0;
    virtual WifiErrorNo GetStationList(std::vector<std::string> &result) = 0;
    virtual WifiErrorNo SetHotspotConfig(const int &channel, const std::string &mscb) = 0;
    virtual WifiErrorNo AddBlockByMac(const std::string &mac) = 0;
    virtual WifiErrorNo DelBlockByMac(const std::string &mac) = 0;
    virtual WifiErrorNo RemoveStation(const std::string &mac) = 0;
    virtual WifiErrorNo GetFrequenciesByBand(int band, std::vector<int> &frequencies) = 0;
    virtual WifiErrorNo RegisterApEvent(IWifiApMonitorEventCallback callback) = 0;
    virtual WifiErrorNo SetWifiCountryCode(const std::string &code) = 0;
    virtual WifiErrorNo DisconnectStaByMac(const std::string &mac) = 0;
};

class WifiApHalInterface : public MockWifiApHalInterface {
public:
    static WifiApHalInterface &GetInstance(void);
    MOCK_METHOD0(StartAp, WifiErrorNo(void));
    MOCK_METHOD0(StopAp1, WifiErrorNo(void));
    MOCK_METHOD0(StopAp, WifiErrorNo(void));
    MOCK_METHOD1(SetSoftApConfig, WifiErrorNo(const HotspotConfig &config));
    MOCK_METHOD1(GetStationList, WifiErrorNo(std::vector<std::string> &result));
    MOCK_METHOD2(SetHotspotConfig, WifiErrorNo(const int &channel, const std::string &mscb));
    MOCK_METHOD1(AddBlockByMac, WifiErrorNo(const std::string &mac));
    MOCK_METHOD1(DelBlockByMac, WifiErrorNo(const std::string &mac));
    MOCK_METHOD1(RemoveStation, WifiErrorNo(const std::string &mac));
    MOCK_METHOD2(GetFrequenciesByBand, WifiErrorNo(int band, std::vector<int> &frequencies));
    MOCK_METHOD1(RegisterApEvent, WifiErrorNo(IWifiApMonitorEventCallback callback));
    MOCK_METHOD1(SetWifiCountryCode, WifiErrorNo(const std::string &code));
    MOCK_METHOD1(DisconnectStaByMac, WifiErrorNo(const std::string &mac));
};
} // namespace Wifi
} // namespace OHOS
#endif