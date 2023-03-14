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

#ifndef BT_UUID_H
#define BT_UUID_H

#include <cstddef>
#include <stdint.h>
#include <array>
#include "cstdint"
#include "iosfwd"

/**
 * @brief The bluetooth subsystem.
 */
namespace OHOS {
namespace bluetooth {
/**
 * @brief This class provides service uuid.
 *
 * @since 6
 */
class Uuid {
public:
    // 128 bits uuid length type
    const static int UUID128_BYTES_TYPE = 16;
    // 32 bits uuid length
    const static int UUID32_BYTES_TYPE = 4;
    // 16 bits uuid length
    const static int UUID16_BYTES_TYPE = 2;
    using UUID128Bit = std::array<uint8_t, UUID128_BYTES_TYPE>;

    const static int UUID_TIME_LOW_FIRST_BYTE = 0;
    const static int UUID_TIME_LOW_SECEND_BYTE = 1;
    const static int UUID_TIME_LOW_THIRD_BYTE = 2;
    const static int UUID_TIME_LOW_FOURTH_BYTE = 3;
    const static int UUID_TIME_MID_FIRST_BYTE = 4;
    const static int UUID_TIME_MID_SECEND_BYTE = 5;
    const static int UUID_VERSION = 6;
    const static int UUID_TIME_HIGH = 7;
    const static int UUID_VARIANT = 8;
    const static int UUID_CLOCK_SEQ = 9;
    const static int UUID_NODE_FIRST_BYTE = 10;
    const static int UUID_NODE_SECEND_BYTE = 11;
    const static int UUID_NODE_THIRD_BYTE = 12;
    const static int UUID_NODE_FOURTH_BYTE = 13;
    const static int UUID_NODE_FIFTH_BYTE = 14;
    const static int UUID_NODE_SIXTH_BYTE = 15;

    const static int BASE_BIT_OPT_SIZE = 8;
    const static int BIT_OPT_TWO_BYTE = 2;
    const static int BIT_OPT_THREE_BYTE = 3;
    const static int BIT_OPT_FOUR_BYTE = 4;
    const static int BIT_OPT_FIVE_BYTE = 5;
    const static int BIT_OPT_SIX_BYTE = 6;
    const static int BIT_OPT_SEVEN_BYTE = 7;

    const static int SIZE_STRING_TO_INT = 2;
    /**
     * @brief A constructor used to create an <b>UUID</b> instance.
     *
     * @since 6
     */
    Uuid() = default;

    /**
     * @brief A constructor used to create an <b>UUID</b> instance.
     *
     * @param other Other uuid to create an <b>UUID</b> instance.
     * @since 6
     */
    Uuid(const Uuid &other) = default;

    /**
     * @brief The assignment constructor.
     *
     * @param other Other uuid object.
     * @return Returns the reference of Uuid.
     * @since 6
     */
    Uuid &operator=(const Uuid &other) = default;

    /**
     * @brief A destructor used to delete the <b>UUID</b> instance.
     *
     * @since 6
     */
    ~Uuid() = default;

    /**
     * @brief Create a random uuid.
     *
     * @return @c Uuid : The function return a random uuid.
     */
    static Uuid Random();

    /**
     * @brief Constructor a new Uuid from string.
     *
     * @param name The value of string to create Uuid.
     *      for example : "00000000-0000-1000-8000-00805F9B34FB"
     * @return Returns a specified Uuid.
     * @since 6
     */
    static Uuid ConvertFromString(const std::string &name);

    /**
     * @brief Create a new uuid from uint16_t.
     *
     * @param uuid The 16 bits is a part of 128 bits.
     * @return Returns a new uuid.
     * @since 6
     */
    static Uuid ConvertFrom16Bits(uint16_t uuid);

    /**
     * @brief Create a new uuid from uint32_t.
     *
     * @param uuid The 32 bits is a part of 128 bits.
     * @return Return a new uuid.
     * @since 6
     */
    static Uuid ConvertFrom32Bits(uint32_t uuid);

    /**
     * @brief Create a new uuid from little endian bytes.
     *
     * @param uuid The 128 bits value for a uuid.
     * @return Returns a new uuid.
     * @since 6
     */
    static Uuid ConvertFromBytesLE(const uint8_t *uuid, const size_t size = 16);

    /**
     * @brief Create a new uuid by most significant 64 bits and least signigicant 64 bits.
     *
     * @param uuid The 128 bits value for a uuid.
     * @return Returns a new uuid.
     * @since 6
     */
    static Uuid ConvertFromMostAndLeastBit(uint64_t mostSigBits, uint64_t leastSigBits);

    /**
     * @brief Create a new uuid from uint8_t array.
     *
     * @param uuid The 128 bits value for a uuid.
     * @return Returns a uuid from uint8_t array.
     * @since 6
     */
    static Uuid ConvertFrom128Bits(const UUID128Bit &uuid);

    /**
     * @brief Convert uuid to 16 bits.
     *
     * @return Returns a uint16_t value from uuid.
     * @since 6
     */
    uint16_t ConvertTo16Bits() const;

    /**
     * @brief Convert uuid to 32 bits.
     *
     * @return Returns a uint32_t value from uuid.
     * @since 6
     */
    uint32_t ConvertTo32Bits() const;

    /**
     * @brief Convert uuid to uint8_t* with little endian.
     *
     * @param[in] value : The 128 bits value for a uuid.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool ConvertToBytesLE(uint8_t *value, const size_t size = 16) const;

    /**
     * @brief Convert uuid to uint8_t array.
     *
     * @return Returns a new uuid.
     * @since 6
     */
    UUID128Bit ConvertTo128Bits() const;

    /**
     * @brief Get UUID type: 16 bits or 32 bits or 128 bits
     *
     * @return Returns uuid type.
     *         UUID128_BYTES_TYPE : 128 bits uuid.
     *         UUID32_BYTES_TYPE : 32 bits uuid.
     *         UUID16_BYTES_TYPE : 16 bits uuid.
     * @since 6
     */
    int GetUuidType() const;

    /**
     * @brief Compare two uuid whether are same or not.
     *
     * @param rhs Compared UUID instance.
     * @return Returns <b>true</b> if this UUID is the same as compared UUID;
     *         returns <b>false</b> if this UUID is not the same as compared UUID.
     * @since 6
     */
    bool operator==(const Uuid &rhs) const;

    /**
     * @brief Compare two uuid whether are different or not.
     *
     * @param rhs Compared UUID instance.
     * @return Returns <b>true</b> if this UUID is different as compared UUID;
     *         returns <b>false</b> if this UUID is not different as compared UUID.
     * @since 6
     */
    bool operator!=(const Uuid &rhs) const;

    /**
     * @brief In order to use the object key in the map object, overload the operator <.
     * @param[in] uuid : Uuid object.
     * @return @c bool : If the object uuid is the same, return true, otherwise return false.
     */
    bool operator<(const Uuid &uuid) const
    {
        return *this != uuid;
    }

protected:
    /**
     * @brief Constructor.
     */
    explicit Uuid(const UUID128Bit uuid) : uuid_(uuid) {};

    // base uuid value
    std::array<uint8_t, UUID128_BYTES_TYPE> BASE_UUID = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
        0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
    };

    std::array<uint8_t, UUID128_BYTES_TYPE> uuid_ = BASE_UUID;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // BT_UUID_H