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
#include "wifi_ap_hal_interface.h"
#include <mutex>
#include "wifi_log.h"
#include "wifi_error_no.h"

#undef LOG_TAG
#define LOG_TAG "WifiApHalInterface"

namespace OHOS {
namespace Wifi {
WifiApHalInterface &WifiApHalInterface::GetInstance(void)
{
    static WifiApHalInterface inst;
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

WifiErrorNo WifiApHalInterface::StartAp(void)
{
    return mIdlClient->StartAp();
}

WifiErrorNo WifiApHalInterface::StopAp(void)
{
    return mIdlClient->StopAp();
}

WifiErrorNo WifiApHalInterface::SetSoftApConfig(const HotspotConfig &config)
{
    return mIdlClient->SetSoftApConfig(config);
}

WifiErrorNo WifiApHalInterface::GetStationList(std::vector<std::string> &result)
{
    return mIdlClient->GetStationList(result);
}

WifiErrorNo WifiApHalInterface::AddBlockByMac(const std::string &mac)
{
    return mIdlClient->AddBlockByMac(mac);
}

WifiErrorNo WifiApHalInterface::DelBlockByMac(const std::string &mac)
{
    return mIdlClient->DelBlockByMac(mac);
}

WifiErrorNo WifiApHalInterface::RemoveStation(const std::string &mac)
{
    return mIdlClient->RemoveStation(mac);
}

WifiErrorNo WifiApHalInterface::GetFrequenciesByBand(int band, std::vector<int> &frequencies)
{
    return mIdlClient->GetFrequenciesByBand(band, frequencies);
}

WifiErrorNo WifiApHalInterface::RegisterApEvent(IWifiApMonitorEventCallback callback)
{
    WifiErrorNo err = mIdlClient->RegisterApEvent(callback);
    if (err == WIFI_IDL_OPT_OK || callback.onStaJoinOrLeave == nullptr) {
        mApCallback = callback;
    }
    return err;
}

WifiErrorNo WifiApHalInterface::SetWifiCountryCode(const std::string &code)
{
    return mIdlClient->SetWifiCountryCode(code);
}

WifiErrorNo WifiApHalInterface::DisconnectStaByMac(const std::string &mac)
{
    return mIdlClient->ReqDisconnectStaByMac(mac);
}

const IWifiApMonitorEventCallback &WifiApHalInterface::GetApCallbackInst(void) const
{
    return mApCallback;
}
}  // namespace Wifi
}  // namespace OHOS