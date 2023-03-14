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
#ifndef OHOS_MOCK_P2P_MONITOR_H
#define OHOS_MOCK_P2P_MONITOR_H

#include <gmock/gmock.h>
#include "p2p_monitor.h"
namespace OHOS {
namespace Wifi {
class MockP2pMonitor : public P2pMonitor {
public:
    MOCK_METHOD0(Initialize, void());
    MOCK_METHOD1(MonitorBegins, void(const std::string &iface));
    MOCK_METHOD1(MonitorEnds, void(const std::string &iface));
    MOCK_METHOD2(RegisterIfaceHandler, void(const std::string &iface, const std::function<HandlerMethod> &handler));
    MOCK_METHOD1(UnregisterHandler, void(const std::string &iface));
};
}  // namespace Wifi
}  // namespace OHOS
#endif