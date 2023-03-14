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

#ifndef BLE_UTILS_H
#define BLE_UTILS_H

#include <cstring>
#include <vector>

#include "ble_defs.h"

/*
 * @brief The bluetooth system.
 */
namespace OHOS {
namespace bluetooth {
/**
 * @brief BLE Utils.
 */
class BleUtils {
public:
    /**
     * @brief Convert int to hex string
     *
     * @param [in] int vector value
     * @return hex string.
     */
    static std::string ConvertIntToHexString(const std::vector<uint8_t> &key);

    /**
     * @brief Convert int to hex string
     *
     * @param [in] int value
     * @return hex string.
     */
    static std::string IntToHexString(uint8_t value);

    /**
     * @brief Convert hex string to int
     *
     * @param  [in] hex string
     * @param  [out] int vector value
     */
    static void ConvertHexStringToInt(const std::string &str, std::vector<uint8_t> &key);

    /**
     * @brief Generate 16 hex uint8_t vector
     *
     * @param  [out] int vector value
     */
    static void Rand16hex(std::vector<uint8_t> &key);

    /**
     * @brief Get random address
     *
     * @param  [out] uint8_t value
     * @param  [in] bool true:non res pri addr;false:static address
     */
    static void GetRandomAddress(std::vector<uint8_t> &addr, bool isNonResPriAddr);

private:
    const static uint8_t HEX_FORMAT_SIZE = 3;
    const static uint8_t HEX = 16;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // BLE_UTILS_H