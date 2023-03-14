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
#include "bluetooth_utils_server.h"
#include "bluetooth_def.h"
#include "bluetooth_log.h"

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

std::string GetScanModeName(int mode)
{
    switch (mode) {
        case SCAN_MODE_LOW_POWER:
            return "SCAN_MODE_LOW_POWER(0)";
        case SCAN_MODE_BALANCED:
            return "SCAN_MODE_BALANCED(1)";
        case SCAN_MODE_LOW_LATENCY:
            return "SCAN_MODE_LOW_LATENCY(2)";
        case SCAN_MODE_OP_P2_60_3000:
            return "SCAN_MODE_OP_P2_60_3000(3)";
        case SCAN_MODE_OP_P10_60_600:
            return "SCAN_MODE_OP_P10_60_600(4)";
        case SCAN_MODE_OP_P25_60_240:
            return "SCAN_MODE_OP_P25_60_240(5)";
        case SCAN_MODE_OP_P100_1000_1000:
            return "SCAN_MODE_OP_P100_1000_1000(6)";
        default:
            return "Unknown";
    }
}
}  // namespace Bluetooth
}  // namespace OHOS