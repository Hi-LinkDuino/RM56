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

#include "ble_utils.h"

#include <ctime>
#include <random>
#include <sstream>

#include "securec.h"

namespace OHOS {
namespace bluetooth {
std::string BleUtils::IntToHexString(uint8_t value)
{
    std::stringstream strStream;
    char token[HEX_FORMAT_SIZE] = {0};
    (void)sprintf_s(token, HEX_FORMAT_SIZE, "%02X", value);
    strStream << token;
    return strStream.str();
}

std::string BleUtils::ConvertIntToHexString(const std::vector<uint8_t> &key)
{
    std::string strs;
    for (auto iter = key.begin(); iter != key.end(); iter++) {
        uint8_t temp = *iter;
        strs += IntToHexString(temp);
    }
    return strs;
}

void BleUtils::ConvertHexStringToInt(const std::string &str, std::vector<uint8_t> &key)
{
    for (size_t i = 0; i < str.size(); i = i + 2) {  // 2 is the length of str to be truncated
        long k = std::stol(str.substr(i, 2), nullptr, HEX);
        key.push_back(k);
    }
}

void BleUtils::Rand16hex(std::vector<uint8_t> &key)
{
    uint8_t result = 0;
    uint8_t n3 = 0;

    std::random_device rd;
    std::default_random_engine re(rd());
    std::uniform_int_distribution<int> random_value(0, (((unsigned int)(-1)) >> 1));

    for (int i = 0; i < BLE_IRK_HEX_ELN; i++) {
        if (n3 == 0) {
            result = random_value(re);
            n3 = BLE_IRK_RAND_HEX_LEN;
        }
        key.push_back(result & BLE_IRK_RAND_ELN);
        result >>= BLE_IRK_RAND_LEFT_SHIFT;
        --n3;
    }
}

void BleUtils::GetRandomAddress(std::vector<uint8_t> &addr, bool isNonResPriAddr)
{
    std::random_device rd;
    std::default_random_engine re(rd());
    std::uniform_int_distribution<uint8_t> random_value(0, BLE_STATIC_PRI_ADDR);

    for (int i = 0; i < BT_ADDRESS_SIZE - 1; ++i) {
        addr.push_back(random_value(re) & 0xff);
    }

    if (isNonResPriAddr) {
        addr.push_back(random_value(re) & BLE_NON_RES_PRI_ADDR);
    } else {
        addr.push_back((random_value(re) & 0xff) | BLE_STATIC_PRI_ADDR);
    }

    return;
}
}  // namespace bluetooth
}  // namespace OHOS