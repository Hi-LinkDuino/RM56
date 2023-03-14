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
#ifndef OHOS_MOCK_WIFI_STA_MANAGER_H
#define OHOS_MOCK_WIFI_STA_MANAGER_H

#include <gmock/gmock.h>
#include "sta_service_callback.h"

namespace OHOS {
namespace Wifi {
class MockWifiManager {
public:
    virtual ~MockWifiManager() = default;
    virtual void DealStaOpenRes(OperateResState state) = 0;
    virtual void DealStaCloseRes(OperateResState state) = 0;
    virtual void DealStaConnChanged(OperateResState state, const WifiLinkedInfo &info) = 0;
    virtual void DealWpsChanged(WpsStartState state, const int pinCode) = 0;
    virtual void DealStreamChanged(StreamDirection state) = 0;
    virtual void DealRssiChanged(int rssi) = 0;
};

class WifiManager : public MockWifiManager {
public:
    WifiManager();
    ~WifiManager() = default;
    static WifiManager &GetInstance();
    StaServiceCallback GetStaCallback();
    MOCK_METHOD1(DealStaOpenRes, void(OperateResState state));
    MOCK_METHOD1(DealStaCloseRes, void(OperateResState state));
    MOCK_METHOD2(DealStaConnChanged, void(OperateResState type, const WifiLinkedInfo &info));
    MOCK_METHOD2(DealWpsChanged, void(WpsStartState state, const int pinCode));
    MOCK_METHOD1(DealStreamChanged, void(StreamDirection state));
    MOCK_METHOD1(DealRssiChanged, void(int rssi));
private:
    StaServiceCallback mStaCallback;
    void InitStaCallback(void);
};
}  // namespace OHOS
}  // namespace Wifi

#endif