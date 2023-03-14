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
#ifndef OHOS_MOCK_WIFISUPPLICANTHALINTERFACE_H
#define OHOS_MOCK_WIFISUPPLICANTHALINTERFACE_H

#include <gmock/gmock.h>
#include <string>
#include "supplicant_event_callback.h"
#include "wifi_error_no.h"
#include "i_wifi_struct.h"

namespace OHOS {
namespace Wifi {
class MockWifiSupplicantHalInterface {
public:
    virtual ~MockWifiSupplicantHalInterface() = default;
    virtual WifiErrorNo UnRegisterSupplicantEventCallback() = 0;
    virtual WifiErrorNo RegisterSupplicantEventCallback(SupplicantEventCallback &callback) = 0;
};

class WifiSupplicantHalInterface : public MockWifiSupplicantHalInterface {
public:
    static WifiSupplicantHalInterface &GetInstance(void);

public:
    MOCK_METHOD0(UnRegisterSupplicantEventCallback, WifiErrorNo());
    MOCK_METHOD1(RegisterSupplicantEventCallback, WifiErrorNo(SupplicantEventCallback &callback));
};
}  // namespace Wifi
}  // namespace OHOS
#endif
