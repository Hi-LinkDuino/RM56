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
#ifndef OHOS_MOCK_WIFI_CHIP_HAL_INTERFACE_H
#define OHOS_MOCK_WIFI_CHIP_HAL_INTERFACE_H

#include <vector>
#include <gmock/gmock.h>
#include "sta_state_machine.h"
#include "wifi_error_no.h"
#include "i_wifi_struct.h"
#include "wifi_chip_event_callback.h"

namespace OHOS {
namespace Wifi {
class MockWifiChipHalInterface {
public:
    virtual ~MockWifiChipHalInterface() = default;
    virtual WifiErrorNo GetWifiChipObject(int id, IWifiChip &chip) = 0;
    virtual WifiErrorNo GetChipIds(std::vector<int> &ids) = 0;
    virtual WifiErrorNo GetUsedChipId(int &id) = 0;
    virtual WifiErrorNo GetChipCapabilities(int &capabilities) = 0;
    virtual WifiErrorNo GetSupportedModes(std::vector<int> &modes) = 0;
    virtual WifiErrorNo ConfigRunModes(int mode) = 0;
    virtual WifiErrorNo GetCurrentMode(int &mode) = 0;
    virtual WifiErrorNo RegisterChipEventCallback(WifiChipEventCallback &callback) = 0;
    virtual WifiErrorNo RequestFirmwareDebugInfo(std::string &debugInfo) = 0;
    virtual WifiErrorNo SetWifiPowerMode(int mode) = 0;
    virtual WifiErrorNo GetChipIfaceFactoryMacAddress(const std::string &ifaceName, std::string &macAddress) = 0;
};

class WifiChipHalInterface : public MockWifiChipHalInterface {
public:
    static WifiChipHalInterface &GetInstance(void);

    MOCK_METHOD2(GetWifiChipObject, WifiErrorNo(int id, IWifiChip &chip));
    MOCK_METHOD1(GetChipIds, WifiErrorNo(std::vector<int> &ids));
    MOCK_METHOD1(GetUsedChipId, WifiErrorNo(int &id));
    MOCK_METHOD1(GetChipCapabilities, WifiErrorNo(int &capabilities));
    MOCK_METHOD1(GetSupportedModes, WifiErrorNo(std::vector<int> &modes));
    MOCK_METHOD1(ConfigRunModes, WifiErrorNo(int mode));
    MOCK_METHOD1(GetCurrentMode, WifiErrorNo(int &mode));
    MOCK_METHOD1(RegisterChipEventCallback, WifiErrorNo(WifiChipEventCallback &callback));
    MOCK_METHOD1(RequestFirmwareDebugInfo, WifiErrorNo(std::string &debugInfo));
    MOCK_METHOD1(SetWifiPowerMode, WifiErrorNo(int mode));
    MOCK_METHOD2(GetChipIfaceFactoryMacAddress, WifiErrorNo(const std::string &ifaceName, std::string &macAddress));
};
}  // namespace OHOS
}  // namespace Wifi

#endif