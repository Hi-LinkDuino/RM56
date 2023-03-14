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

#ifndef RAW_ADDRESS_H
#define RAW_ADDRESS_H

#include <cstddef>
#include <stdint.h>
#include <string>
#include "iosfwd"

namespace OHOS {
namespace bluetooth {
class RawAddress {
public:
    // address length
    const static int BT_ADDRESS_STR_LEN = 17;
    const static int BT_ADDRESS_BYTE_LEN = 6;
    const static int BT_COLON_BYTE_SIZE = 1;

    const static int BT_LAP_HIGH_BYTE = 5;
    const static int BT_LAP_MIDDLE_BYTE = 4;
    const static int BT_LAP_LOW_BYTE = 3;
    const static int BT_UAP_BYTE = 2;
    const static int BT_NAP_HIGH_BYTE = 1;
    const static int BT_NAP_LOW_BYTE = 0;

    /**
     * @brief A constructor used to create an <b>RawAddress</b> instance.
     *
     * @since 6
     */
    RawAddress() = default;

    /**
     * @brief A constructor used to create an <b>RawAddress</b> instance.
     *
     * @param other Other RawAddress instance.
     * @since 6
     */
    RawAddress(const RawAddress &other) = default;

    /**
     * @brief A constructor used to create an <b>RawAddress</b> instance.
     *
     * @param address Address string.
     * @since 6
     */
    RawAddress(const std::string &address) : address_(address) {};

    /**
     * @brief A destructor used to delete the <b>RawAddress</b> instance.
     *
     * @since 6
     */
    ~RawAddress() = default;

    /**
     * @brief Get RawAddress address string.
     *
     * @return Returns address string.
     * @since 6
     */
    const std::string &GetAddress() const
    {
        return address_;
    };

    /**
     * @brief Convert RawAddress to uint8_t pointer.
     *
     * @param dst Out parameter uint8_t pointer.
     * @since 6
     */
    void ConvertToUint8(uint8_t *dst, const size_t size = BT_ADDRESS_BYTE_LEN) const;

    /**
     * @brief Convert RawAddress to uint8_t pointer.
     *
     * @param src Out parameter uint8_t pointer.
     * @return Returns address string.
     * @since 6
     */
    static RawAddress ConvertToString(const uint8_t *src, const size_t size = BT_ADDRESS_BYTE_LEN);

    /**
     * @brief Compare two RawAddress values.
     *
     * @param rhs Compared RawAddress instance.
     * @return Returns <b>true</b> if this RawAddress is smaller than compared RawAddress;
     *         returns <b>false</b> if this RawAddress is not smaller than compared RawAddress.
     * @since 6
     */
    bool operator<(const RawAddress &rhs) const;

    /**
     * @brief Compare two RawAddress whether are same or not.
     *
     * @param rhs Compared RawAddress instance.
     * @return Returns <b>true</b> if this RawAddress is same as compared RawAddress;
     *         returns <b>false</b> if this RawAddress is not same as compared RawAddress.
     * @since 6
     */
    bool operator==(const RawAddress &rhs) const;

protected:
    std::string address_ = "";
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // RAW_ADDRESS_H