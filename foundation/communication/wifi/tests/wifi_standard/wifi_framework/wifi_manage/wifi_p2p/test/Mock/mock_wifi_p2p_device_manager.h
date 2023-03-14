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
#ifndef OHOS_MOCK_WIFIP2PDEVICEMANAGER_H
#define OHOS_MOCK_WIFIP2PDEVICEMANAGER_H

#include <string>
#include <gmock/gmock.h>

#include "wifi_msg.h"

namespace OHOS {
namespace Wifi {
class MockWifiP2PDeviceManager {
public:
    virtual void Initialize(void) = 0;
    virtual bool AddDevice(const WifiP2pDevice &device) = 0;
    virtual bool RemoveDevice(const WifiP2pDevice &device) = 0;
    virtual int ClearAll();
    virtual int GetDevicesList(std::vector<WifiP2pDevice> &devices) = 0;
    virtual bool UpdateDeviceSupplicantInf(const WifiP2pDevice &device) = 0;
    virtual bool UpdateDevice(const WifiP2pDevice &device) = 0;
    virtual bool UpdateDeviceGroupCap(const WifiP2pDevice &device) = 0;
    virtual bool UpdateDeviceStatus(const WifiP2pDevice &device) = 0;
    virtual WifiP2pDevice GetDevices(std::string deviceAddress) = 0;
    MockWifiP2PDeviceManager() = default;
    virtual ~MockWifiP2PDeviceManager() = default;
};

class WifiP2PDeviceManager : public MockWifiP2PDeviceManager {
public:
    MOCK_METHOD0(Initialize, void());
    MOCK_METHOD1(AddDevice, bool(const WifiP2pDevice &device));
    MOCK_METHOD1(RemoveDevice, bool(const WifiP2pDevice &device));
    MOCK_METHOD0(ClearAll, int());
    MOCK_METHOD1(GetDevicesList, int(std::vector<WifiP2pDevice> &devices));
    MOCK_METHOD1(UpdateDeviceSupplicantInf, bool(const WifiP2pDevice &device));
    MOCK_METHOD1(UpdateDevice, bool(const WifiP2pDevice &device));
    MOCK_METHOD1(UpdateDeviceGroupCap, bool(const WifiP2pDevice &device));
    MOCK_METHOD1(UpdateDeviceStatus, bool(const WifiP2pDevice &device));
    MOCK_METHOD1(GetDevices, WifiP2pDevice(std::string deviceAddress));
};
}  // namespace Wifi
}  // namespace OHOS
#endif