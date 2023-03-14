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
#ifndef OHOS_MOCK_WIFI_SCAN_HAL_INTERFACE_H
#define OHOS_MOCK_WIFI_SCAN_HAL_INTERFACE_H

#include <string>
#include <vector>
#include <gmock/gmock.h>
#include "wifi_error_no.h"
#include "wifi_internal_msg.h"
#include "wifi_idl_struct.h"
#include "wifi_scan_param.h"

namespace OHOS {
namespace Wifi {
class MockWifiStaHalInterface {
public:
    virtual ~MockWifiStaHalInterface() = default;
    virtual WifiErrorNo Scan(const WifiScanParam &scanParam) = 0;
    virtual WifiErrorNo QueryScanInfos(std::vector<InterScanInfo> &scanInfos) = 0;
    virtual WifiErrorNo StartPnoScan(const WifiPnoScanParam &scanParam) = 0;
    virtual WifiErrorNo StopPnoScan(void) = 0;
    virtual WifiErrorNo GetSupportFrequencies(int band, std::vector<int> &frequencies) = 0;
};

class WifiStaHalInterface : public MockWifiStaHalInterface {
public:
    static WifiStaHalInterface &GetInstance(void);

public:
    MOCK_METHOD1(Scan, WifiErrorNo(const WifiScanParam &scanParam));
    MOCK_METHOD1(QueryScanInfos, WifiErrorNo(std::vector<InterScanInfo> &scanInfos));
    MOCK_METHOD1(StartPnoScan, WifiErrorNo(const WifiPnoScanParam &scanParam));
    MOCK_METHOD0(StopPnoScan, WifiErrorNo(void));
    MOCK_METHOD2(GetSupportFrequencies, WifiErrorNo(int band, std::vector<int> &frequencies));
};
}  // namespace Wifi
}  // namespace OHOS
#endif