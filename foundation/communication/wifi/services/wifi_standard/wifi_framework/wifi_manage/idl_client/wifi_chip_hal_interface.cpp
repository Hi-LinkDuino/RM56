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
#include "wifi_chip_hal_interface.h"
#include <mutex>
#include "wifi_log.h"

#undef LOG_TAG
#define LOG_TAG "WifiChipHalInterface"

namespace OHOS {
namespace Wifi {
WifiChipHalInterface &WifiChipHalInterface::GetInstance(void)
{
    static WifiChipHalInterface inst;
    static int initFlag = 0;
    static std::mutex initMutex;
    if (initFlag == 0) {
        std::unique_lock<std::mutex> lock(initMutex);
        if (initFlag == 0) {
            if (inst.InitIdlClient()) {
                initFlag = 1;
            }
        }
    }
    return inst;
}

WifiErrorNo WifiChipHalInterface::GetWifiChipObject(int id, IWifiChip &chip)
{
    return mIdlClient->GetWifiChipObject(id, chip);
}

WifiErrorNo WifiChipHalInterface::GetChipIds(std::vector<int> &ids)
{
    return mIdlClient->GetChipIds(ids);
}

WifiErrorNo WifiChipHalInterface::GetUsedChipId(int &id)
{
    return mIdlClient->GetUsedChipId(id);
}

WifiErrorNo WifiChipHalInterface::GetChipCapabilities(int &capabilities)
{
    return mIdlClient->GetChipCapabilities(capabilities);
}

WifiErrorNo WifiChipHalInterface::GetSupportedModes(std::vector<int> &modes)
{
    return mIdlClient->GetSupportedModes(modes);
}

WifiErrorNo WifiChipHalInterface::ConfigRunModes(int mode)
{
    return mIdlClient->ConfigRunModes(mode);
}

WifiErrorNo WifiChipHalInterface::GetCurrentMode(int &mode)
{
    return mIdlClient->GetCurrentMode(mode);
}

WifiErrorNo WifiChipHalInterface::RegisterChipEventCallback(WifiChipEventCallback &callback)
{
    return mIdlClient->RegisterChipEventCallback(callback);
}

WifiErrorNo WifiChipHalInterface::RequestFirmwareDebugInfo(std::string &debugInfo)
{
    return mIdlClient->RequestFirmwareDebugInfo(debugInfo);
}

WifiErrorNo WifiChipHalInterface::SetWifiPowerMode(int mode)
{
    return mIdlClient->SetWifiPowerMode(mode);
}
}  // namespace Wifi
}  // namespace OHOS