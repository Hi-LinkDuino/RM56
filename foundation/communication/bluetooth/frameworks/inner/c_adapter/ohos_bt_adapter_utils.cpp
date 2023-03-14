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

#include "ohos_bt_adapter_utils.h"
#include "__config"
#include "bluetooth_log.h"
#include "ohos_bt_def.h"
#include "securec.h"
#include <cstdlib>
#include "string"

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;

namespace OHOS {
namespace Bluetooth {
void ConvertAddr(const unsigned char in[6], std::string &out)
{
    char temp[18] = {0}; // convert addr len.
    int ret = sprintf_s(temp, sizeof(temp), "%02X:%02X:%02X:%02X:%02X:%02X",
        in[0], in[1], in[2], in[3], in[4], in[5]);
    if (ret == -1) {
        HILOGE("ConvertAddr sprintf_s return error, ret -1");
    }
    out = string(temp);
}

void GetAddrFromString(std::string in, unsigned char out[6]) {
    int j = 0;
    for (unsigned int i = 0; i < in.length(); i++) {
        if (in.at(i) != ':') {
            out[j] = strtoul(in.substr(i, 2).c_str(), 0, 16);
            i += 2;
            j++;
        }
    }
}

int GetGattcResult(int ret)
{
    return (ret == OHOS_BT_STATUS_SUCCESS) ? OHOS_BT_STATUS_SUCCESS : OHOS_BT_STATUS_FAIL;
}

void GetAddrFromByte(unsigned char in[6], std::string &out)
{
    char temp[18] = {0};
    (void)sprintf_s(temp, sizeof(temp), "%02X:%02X:%02X:%02X:%02X:%02X",
        in[0], in[1], in[2], in[3], in[4], in[5]);
    out = string(temp);
}
}  // namespace Bluetooth
}  // namespace OHOS
#ifdef __cplusplus
}
#endif
