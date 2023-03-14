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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines uuid for framework.
 *
 * @since 6
 */

/**
 * @file uuid.h
 *
 * @brief framework uuid interface.
 *
 * @since 6
 */

#ifndef DUMMY_UUID_H
#define DUMMY_UUID_H

#include "sys/time.h"
#include <string>
#include <array>
#include <ctime>
#include <regex>

namespace OHOS {
namespace Bluetooth {
const std::regex uuidRegex("^[0-9a-fA-F]{8}-([0-9a-fA-F]{4}-){3}[0-9a-fA-F]{12}$");

/**
 * @brief This class provides framework uuid.
 *
 * @since 6
 */
class UUID {
public:
    //128 bits uuid length
    const static int UUID128_BYTES_LEN = 16;

    /**
     * @brief A constructor used to create an <b>UUID</b> instance.
     *
     * @since 6
     */
    UUID(){};

    /**
     * @brief A destructor used to delete the <b>UUID</b> instance.
     *
     * @since 6
     */
    ~UUID(){};

    /**
     * @brief A constructor used to create an <b>UUID</b> instance. Constructor a new UUID using most significant 64
     * bits and least significant 64 bits.
     *
     * @param[in] mostSigBits  : The most significant 64 bits of UUID.
     * @param[in] leastSigBits : The least significant 64 bits of UUID.
     * @since 6
     */
    UUID(const long mostSigBits, const long leastSigBits)
    {
        this->uuid_[15] = (uint8_t)(leastSigBits & 0x00000000000000FF);
        this->uuid_[14] = (uint8_t)((leastSigBits & 0x000000000000FF00) >> 8);
        this->uuid_[13] = (uint8_t)((leastSigBits & 0x0000000000FF0000) >> 16);
        this->uuid_[12] = (uint8_t)((leastSigBits & 0x00000000FF000000) >> 24);
        this->uuid_[11] = (uint8_t)((leastSigBits & 0x000000FF00000000) >> 32);
        this->uuid_[10] = (uint8_t)((leastSigBits & 0x0000FF0000000000) >> 40);
        this->uuid_[9] = (uint8_t)((leastSigBits & 0x00FF000000000000) >> 48);
        this->uuid_[8] = (uint8_t)((leastSigBits & 0xFF00000000000000) >> 56);
        this->uuid_[7] = (uint8_t)(mostSigBits & 0x00000000000000FF);
        this->uuid_[6] = (uint8_t)((mostSigBits & 0x000000000000FF00) >> 8);
        this->uuid_[5] = (uint8_t)((mostSigBits & 0x0000000000FF0000) >> 16);
        this->uuid_[4] = (uint8_t)((mostSigBits & 0x00000000FF000000) >> 24);
        this->uuid_[3] = (uint8_t)((mostSigBits & 0x000000FF00000000) >> 32);
        this->uuid_[2] = (uint8_t)((mostSigBits & 0x0000FF0000000000) >> 40);
        this->uuid_[1] = (uint8_t)((mostSigBits & 0x00FF000000000000) >> 48);
        this->uuid_[0] = (uint8_t)((mostSigBits & 0xFF00000000000000) >> 56);
    }

    /**
     * @brief A constructor used to create an <b>UUID</b> instance. Constructor a new UUID from string.
     *
     * @param[in] name : The value of string to create UUID.
     *      for example : "00000000-0000-1000-8000-00805F9B34FB"
     * @return Returns a specified UUID.
     * @since 6
     */
    static UUID FromString(const std::string &name)
    {
        UUID ret;
        std::string tmp = name;
        std::size_t pos = tmp.find("-");

        while (pos != std::string::npos) {
            tmp.replace(pos, 1, "");
            pos = tmp.find("-");
        }

        for (std::size_t i = 0; (i + 1) < tmp.length();) {
            ret.uuid_[i / 2] = std::stoi(tmp.substr(i, 2), nullptr, 16);
            i += 2;
        }

        return ret;
    }

    /**
     * @brief A constructor used to create an <b>UUID</b> instance. Constructor a new random UUID.
     *
     * @return Returns a random UUID.
     * @since 6
     */
    static UUID RandomUUID()
    {
        UUID random;

        struct timeval tv;
        struct timezone tz;
        struct tm randomTime;
        unsigned int randNum = 0;

        rand_r(&randNum);
        gettimeofday(&tv, &tz);
        localtime_r(&tv.tv_sec, &randomTime);

        random.uuid_[15] = (uint8_t)(tv.tv_usec & 0x00000000000000FF);
        random.uuid_[14] = (uint8_t)((tv.tv_usec & 0x000000000000FF00) >> 8);
        random.uuid_[13] = (uint8_t)((tv.tv_usec & 0x0000000000FF0000) >> 16);
        random.uuid_[12] = (uint8_t)((tv.tv_usec & 0x00000000FF000000) >> 24);
        random.uuid_[10] = (uint8_t)((tv.tv_usec & 0x000000FF00000000) >> 32);
        random.uuid_[9] = (uint8_t)((tv.tv_usec & 0x0000FF0000000000) >> 40);
        random.uuid_[8] = (uint8_t)((tv.tv_usec & 0x00FF000000000000) >> 48);
        random.uuid_[7] = (uint8_t)((tv.tv_usec & 0xFF00000000000000) >> 56);
        random.uuid_[6] = (uint8_t)((randomTime.tm_sec + randNum) & 0xFF);
        random.uuid_[5] = (uint8_t)((randomTime.tm_min + (randNum >> 8)) & 0xFF);
        random.uuid_[4] = (uint8_t)((randomTime.tm_hour + (randNum >> 16)) & 0xFF);
        random.uuid_[3] = (uint8_t)((randomTime.tm_mday + (randNum >> 24)) & 0xFF);
        random.uuid_[2] = (uint8_t)(randomTime.tm_mon & 0xFF);
        random.uuid_[1] = (uint8_t)(randomTime.tm_year & 0xFF);
        random.uuid_[0] = (uint8_t)((randomTime.tm_year & 0xFF00) >> 8);

        return random;
    }

    /**
     * @brief Convert UUID to string.
     *
     * @return Returns a String object representing this UUID.
     * @since 6
     */
    std::string ToString() const
    {
        std::string tmp = "";
        std::string ret = "";
        static const char *hex = "0123456789ABCDEF";

        for (auto it = this->uuid_.begin(); it != this->uuid_.end(); it++) {
            tmp.push_back(hex[((*it) >> 4 & 0xF)]);
            tmp.push_back(hex[(*it) & 0xF]);
        }

        ret = tmp.substr(0, 8) + "-" + tmp.substr(8, 4) + "-" + tmp.substr(12, 4) + "-" + tmp.substr(16, 4) + "-" +
              tmp.substr(20);

        return ret;
    }

    /**
     * @brief Compares this UUID with the specified UUID.
     *
     * @param[in] val : UUID which this UUID is to be compared.
     * @return Returns <b> <0 </b> if this UUID is less than compared UUID;
     *         returns <b> =0 </b> if this UUID is equal to compared UUID;
     *         returns <b> >0 </b> if this UUID is greater than compared UUID.
     * @since 6
     */
    int CompareTo(const UUID &val) const
    {
        UUID tmp = val;
        return this->ToString().compare(tmp.ToString());
    }

    /**
     * @brief Compares this object to the specified object.
     *
     * @param[in] val : UUID which this UUID is to be compared.
     * @return Returns <b>true</b> if this UUID is the same as compared UUID;
     *         returns <b>false</b> if this UUID is not the same as compared UUID.
     * @since 6
     */
    bool Equals(const UUID &val) const
    {
        for (int i = 0; i < UUID128_BYTES_LEN; i++) {
            if (this->uuid_[i] != val.uuid_[i]) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Returns the least significant 64 bits of this UUID's 128 bit value.
     *
     * @return Retruns the least significant 64 bits of this UUID's 128 bit value.
     * @since 6
     */
    uint64_t GetLeastSignificantBits() const
    {
        uint64_t leastSigBits = 0;
        for (int i = UUID128_BYTES_LEN / 2; i < UUID128_BYTES_LEN; i++) {
            leastSigBits = (leastSigBits << 8) | (uuid_[i] & 0xFF);
        }
        return leastSigBits;
    }

    /**
     * @brief Returns the most significant 64 bits of this UUID's 128 bit value.
     *
     * @return Returns the most significant 64 bits of this UUID's 128 bit value.
     * @since 6
     */
    uint64_t GetMostSignificantBits() const
    {
        uint64_t mostSigBits = 0;
        for (int i = 0 / 2; i < UUID128_BYTES_LEN / 2; i++) {
            mostSigBits = (mostSigBits << 8) | (uuid_[i] & 0xFF);
        }
        return mostSigBits;
    }

    /**
     * @brief Constructor a new UUID from uint8_t array.
     *
     * @param[in] name : The 128 bits value for a UUID.
     * @return Returns a specified UUID.
     * @since 6
     */
    static UUID ConvertFrom128Bits(const std::array<uint8_t, UUID128_BYTES_LEN> &name)
    {
        UUID tmp;
        for (int i = 0; i < UUID128_BYTES_LEN; i++) {
            tmp.uuid_[i] = name[i];
        }
        return tmp;
    }

    /**
     * @brief Returns uint8_t array from UUID.
     *
     * @return returns a specified array.
     * @since 6
     */
    std::array<uint8_t, UUID128_BYTES_LEN> ConvertTo128Bits() const
    {
        std::array<uint8_t, UUID128_BYTES_LEN> uuid;

        for (int i = 0; i < UUID128_BYTES_LEN; i++) {
            uuid[i] = uuid_[i];
        }

        return uuid;
    }

    /**
     * @brief In order to use the object key in the map object, overload the operator <.
     * @param[in] uuid : UUID object.
     * @return @c bool : If the object uuid is the same, return true, otherwise return false.
     */
    bool operator<(const UUID &uuid) const
    {
        return !Equals(uuid);
    }

private:
    std::array<uint8_t, UUID128_BYTES_LEN> uuid_ = {0x00};
};

/**
 * @brief This class provides framework ParcelUuid.
 *
 * @since 6
 */
using ParcelUuid = UUID;

} // namespace Bluetooth
} // namespace OHOS

#endif  //DUMMY_UUID_H