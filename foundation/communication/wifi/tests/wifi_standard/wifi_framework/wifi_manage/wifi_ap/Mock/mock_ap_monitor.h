/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_MOCK_AP_MONITOR_H
#define OHOS_MOCK_AP_MONITOR_H
#include <gmock/gmock.h>

#include "wifi_msg.h"
#include "ap_macro.h"
#include "ap_monitor.h"

namespace OHOS {
namespace Wifi {
class MockApMonitor : public ApMonitor {
public:
    MOCK_CONST_METHOD2(StationChangeEvent, void(StationInfo &staInfo, const int event));
    MOCK_CONST_METHOD1(OnHotspotStateEvent, void(int state));
    MOCK_METHOD0(StartMonitor, void());
    MOCK_METHOD0(StopMonitor, void());
};

} // namespace Wifi
} // namespace OHOS
#endif