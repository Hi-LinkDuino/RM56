/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef TEST_WIFI_UTILS_H
#define TEST_WIFI_UTILS_H

#include "net_trans_common.h"
#include "wifi_device.h"

class WiFiUtils {
public:
    static int EnableWifi();
    static int DisableWifi();
    static int DisableThenEnable(int delaySeconds);
    static int ConnectTo(const std::string& ssid, const std::string& passwd);
    static int ConnectToNew(const std::string& ssid, const std::string& passwd);
    static int ConnectToOpenAP(const std::string& ssid);
    static int EnableThenConnect(const std::string& ssid,
                                 const std::string& passwd);
    static int DisableThenEnableAndConnect(int delaySeconds,
                                           const std::string& ssid,
                                           const std::string& passwd);
    static int CheckIsConnectToDefault(void);
};

#endif