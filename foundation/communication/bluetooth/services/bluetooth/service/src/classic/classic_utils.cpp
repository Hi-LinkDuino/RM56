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

#include "classic_utils.h"

#include <sstream>

#include "classic_defs.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
std::string ClassicUtils::IntToHexString(uint8_t value)
{
    std::stringstream strStream;
    char token[HEX_FORMAT_SIZE] = {0};
    (void)sprintf_s(token, HEX_FORMAT_SIZE, "%02X", value);
    strStream << token;
    return strStream.str();
}

std::string ClassicUtils::ConvertIntToHexString(const std::vector<uint8_t> &value)
{
    std::string strs;
    for (auto iter = value.begin(); iter != value.end(); iter++) {
        uint8_t temp = *iter;
        strs += IntToHexString(temp);
    }
    return strs;
}

void ClassicUtils::ConvertHexStringToInt(const std::string &str, std::vector<uint8_t> &value)
{
    for (size_t i = 0; i < str.size(); i = i + 2) {  // 2 is the length of str to be truncated
        long k = std::stol(str.substr(i, 2), nullptr, 16);
        value.push_back(k);
    }
}

std::string ClassicUtils::ConvertUuidToString(const std::vector<Uuid> &uuids)
{
    std::string uuidStr = "";
    if (uuids.empty()) {
        return uuidStr;
    }

    for (auto &uuid : uuids) {
        std::array<uint8_t, UUID128_BYTES_TYPE> value;
        value = uuid.ConvertTo128Bits();
        std::vector<uint8_t> vecUuid;
        for (int idx = 0; idx < UUID128_BYTES_TYPE; idx++) {
            vecUuid.push_back(value[idx]);
        }
        uuidStr += ConvertIntToHexString(vecUuid) + ",";
    }
    return uuidStr;
}

std::vector<Uuid> ClassicUtils::ConvertStringToUuid(const std::string &value)
{
    std::vector<Uuid> uuids;
    if (value.empty()) {
        return uuids;
    }

    size_t index = value.find(',', 0);
    int startPos = 0;
    while (index != (value.length() - 1)) {
        std::string uuidStr = value.substr(startPos, (index - startPos));
        Uuid uuid = Uuid::ConvertFromString(uuidStr);
        uuids.push_back(uuid);
        startPos = index + 1;
        index = value.find(',', startPos);
    }
    return uuids;
}
}  // namespace bluetooth
}  // namespace OHOS