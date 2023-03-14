/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "bluetooth_utils.h"
#include "__config"
#include "bluetooth_def.h"
#include "bluetooth_log.h"
#include "iosfwd"
#include "string"

using namespace std;

namespace OHOS {
namespace Bluetooth {
constexpr int startPos = 6;
constexpr int endPos = 13;
std::string GetEncryptAddr(std::string addr)
{
    if (addr.empty() || addr.length() != ADDRESS_LENGTH) {
        HILOGE("addr is invalid.");
        return std::string("");
    }
    std::string tmp = "**:**:**:**:**:**";
    std::string out = addr;
    // 00:01:**:**:**:05
    for (int i = startPos; i <= endPos; i++) {
        out[i] = tmp[i];
    }
    return out;
}

std::string GetBtStateName(int state)
{
    switch (state) {
        case BTStateID::STATE_TURNING_ON:
            return "STATE_TURNING_ON(0)";
        case BTStateID::STATE_TURN_ON:
            return "STATE_TURN_ON(1)";
        case BTStateID::STATE_TURNING_OFF:
            return "STATE_TURNING_OFF(2)";
        case BTStateID::STATE_TURN_OFF:
            return "STATE_TURN_OFF(3)";
        default:
            return "Unknown";
    }
}

std::string GetBtTransportName(int transport)
{
    switch (transport) {
        case BTTransport::ADAPTER_BREDR:
            return "ADAPTER_BREDR(0)";
        case BTTransport::ADAPTER_BLE:
            return "ADAPTER_BLE(1)";
        default:
            return "Unknown";
    }
}

std::string GetProfileConnStateName(int state)
{
    switch (state) {
        case static_cast<int>(BTConnectState::CONNECTING):
            return "CONNECTING(0)";
        case static_cast<int>(BTConnectState::CONNECTED):
            return "CONNECTED(1)";
        case static_cast<int>(BTConnectState::DISCONNECTING):
            return "DISCONNECTING(2)";
        case static_cast<int>(BTConnectState::DISCONNECTED):
            return "DISCONNECTED(3)";
        default:
            return "Unknown";
    }
}

}  // namespace Bluetooth
}  // namespace OHOS