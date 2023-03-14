/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dm_anonymous.h"

namespace OHOS {
namespace DistributedHardware {
std::string GetAnonyString(const std::string &value)
{
    const int32_t INT32_SHORT_ID_LENGTH = 20;
    const int32_t INT32_PLAINTEXT_LENGTH = 4;
    const int32_t INT32_MIN_ID_LENGTH = 3;

    std::string tmpStr("******");
    size_t strLen = value.length();
    if (strLen < INT32_MIN_ID_LENGTH) {
        return tmpStr;
    }

    std::string res;
    if (strLen <= INT32_SHORT_ID_LENGTH) {
        res += value[0];
        res += tmpStr;
        res += value[strLen - 1];
    } else {
        res.append(value, 0, INT32_PLAINTEXT_LENGTH);
        res += tmpStr;
        res.append(value, strLen - INT32_PLAINTEXT_LENGTH, INT32_PLAINTEXT_LENGTH);
    }

    return res;
}

std::string GetAnonyInt32(const int32_t value)
{
    std::string tempString = std::to_string(value);
    size_t length = tempString.length();
    if (length == 0x01) {
        tempString[0] = '*';
        return tempString;
    }
    for (size_t i = 1; i < length - 1; i++) {
        tempString[i] = '*';
    }
    return tempString;
}
} // namespace DistributedHardware
} // namespace OHOS
