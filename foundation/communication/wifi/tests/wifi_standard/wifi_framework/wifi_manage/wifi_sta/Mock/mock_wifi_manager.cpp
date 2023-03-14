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
    InitStaCallback();
}

StaServiceCallback WifiManager::GetStaCallback()
{
    return mStaCallback;
}

void WifiManager::InitStaCallback(void)
{
    using namespace std::placeholders;
    mStaCallback.OnStaOpenRes = std::bind(&WifiManager::DealStaOpenRes, this, _1);
    mStaCallback.OnStaCloseRes = std::bind(&WifiManager::DealStaCloseRes, this, _1);
    mStaCallback.OnStaConnChanged = std::bind(&WifiManager::DealStaConnChanged, this, _1, _2);
    mStaCallback.OnWpsChanged = std::bind(&WifiManager::DealWpsChanged, this, _1, _2);
    mStaCallback.OnStaStreamChanged = std::bind(&WifiManager::DealStreamChanged, this, _1);
    mStaCallback.OnStaRssiLevelChanged = std::bind(&WifiManager::DealRssiChanged, this, _1);
    return;
}
}  // namespace Wifi
}  // namespace OHOS