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
#include "mock_wifi_manager.h"

namespace OHOS {
namespace Wifi {
WifiManager &WifiManager::GetInstance()
{
    static WifiManager gWifiManager;
    return gWifiManager;
}

WifiManager::WifiManager()
{
    InitScanCallback();
}

IScanSerivceCallbacks WifiManager::GetScanCallback(void)
{
    return mScanCallback;
}

void WifiManager::InitScanCallback(void)
{
    using namespace std::placeholders;
    mScanCallback.OnScanStartEvent = std::bind(&WifiManager::DealScanOpenRes, this);
    mScanCallback.OnScanStopEvent = std::bind(&WifiManager::DealScanCloseRes, this);
    mScanCallback.OnScanFinishEvent = std::bind(&WifiManager::DealScanFinished, this, _1);
    mScanCallback.OnScanInfoEvent = std::bind(&WifiManager::DealScanInfoNotify, this, _1);
    return;
}
} // namespace Wifi
} // namespace OHOS