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

#include "bt_uuid.h"

#include <sys/time.h>
#include <algorithm>
#include <cstdlib>
#include "array"
#include "securec.h"
#include <cstdlib>
#include "string"
#include "sys/time.h"
#include <ctime>

namespace OHOS {
namespace bluetooth {
Uuid Uuid::Random()
{
    Uuid random;

    struct timeval tv = {};
    struct timezone tz = {};
    struct tm randomTime = {};
    unsigned int randNum = 0;
    unsigned long int tvUsec = 0;

    rand_r(&randNum);
    gettimeofday(&tv, &tz);
    localtime_r(&tv.tv_sec, &randomTime);

    tvUsec = static_cast<unsigned long int>(tv.tv_usec);

    random.uuid_[UUID_NODE_SIXTH_BYTE] =
        static_cast<uint8_t>(tvUsec & 0x00000000000000FF);
    random.uuid_[UUID_NODE_FIFTH_BYTE] =
        static_cast<uint8_t>((tvUsec & 0x000000000000FF00) >> BASE_BIT_OPT_SIZE);
    random.uuid_[UUID_NODE_FOURTH_BYTE] =
        static_cast<uint8_t>((tvUsec & 0x0000000000FF0000) >> BIT_OPT_TWO_BYTE * BASE_BIT_OPT_SIZE);
    random.uuid_[UUID_NODE_THIRD_BYTE] =
        static_cast<uint8_t>((tvUsec & 0x00000000FF000000) >> BIT_OPT_THREE_BYTE * BASE_BIT_OPT_SIZE);
    random.uuid_[UUID_NODE_FIRST_BYTE] =
        static_cast<uint8_t>((tvUsec & 0x000000FF00000000) >> BIT_OPT_FOUR_BYTE * BASE_BIT_OPT_SIZE);
    random.uuid_[UUID_CLOCK_SEQ] =
        static_cast<uint8_t>((tvUsec & 0x0000FF0000000000) >> BIT_OPT_FIVE_BYTE * BASE_BIT_OPT_SIZE);
    random.uuid_[UUID_VARIANT] =
        static_cast<uint8_t>((tvUsec & 0x00FF000000000000) >> BIT_OPT_SIX_BYTE * BASE_BIT_OPT_SIZE);
    random.uuid_[UUID_TIME_HIGH] =
        static_cast<uint8_t>((tvUsec & 0xFF00000000000000) >> BIT_OPT_SEVEN_BYTE * BASE_BIT_OPT_SIZE);
    random.uuid_[UUID_VERSION] =
        static_cast<uint8_t>((static_cast<unsigned int>(randomTime.tm_sec) + randNum) & 0xFF);
    random.uuid_[UUID_TIME_MID_SECEND_BYTE] =
        static_cast<uint8_t>((static_cast<unsigned int>(randomTime.tm_min) + (randNum >> BASE_BIT_OPT_SIZE)) & 0xFF);
    random.uuid_[UUID_TIME_MID_FIRST_BYTE] =
        static_cast<uint8_t>((static_cast<unsigned int>(randomTime.tm_hour) +
        (randNum >> BIT_OPT_TWO_BYTE * BASE_BIT_OPT_SIZE)) & 0xFF);
    random.uuid_[UUID_TIME_LOW_FOURTH_BYTE] =
        static_cast<uint8_t>((static_cast<unsigned int>(randomTime.tm_mday) +
        (randNum >> BIT_OPT_THREE_BYTE * BASE_BIT_OPT_SIZE)) & 0xFF);
    random.uuid_[UUID_TIME_LOW_THIRD_BYTE] =
        static_cast<uint8_t>(static_cast<unsigned int>(randomTime.tm_mon) & 0xFF);
    random.uuid_[UUID_TIME_LOW_SECEND_BYTE] =
        static_cast<uint8_t>(static_cast<unsigned int>(randomTime.tm_year) & 0xFF);
    random.uuid_[UUID_TIME_LOW_FIRST_BYTE] =
        static_cast<uint8_t>((static_cast<unsigned int>(randomTime.tm_year) & 0xFF00) >> BASE_BIT_OPT_SIZE);

    return random;
}

Uuid Uuid::ConvertFromString(const std::string &name)
{
    Uuid ret;
    std::string tmp = name;
    std::size_t pos = tmp.find("-");

    while (pos != std::string::npos) {
        tmp.replace(pos, 1, "");
        pos = tmp.find("-");
    }

    for (std::size_t i = 0; (i + 1) < tmp.length(); i += SIZE_STRING_TO_INT) {
        ret.uuid_[i / SIZE_STRING_TO_INT] = std::stoi(tmp.substr(i, SIZE_STRING_TO_INT), nullptr, UUID128_BYTES_TYPE);
    }

    return ret;
}

Uuid Uuid::ConvertFrom16Bits(uint16_t uuid)
{
    Uuid tmp;
    tmp.uuid_[UUID_TIME_LOW_THIRD_BYTE] = static_cast<uint8_t>((uuid & 0xFF00) >> BASE_BIT_OPT_SIZE);
    tmp.uuid_[UUID_TIME_LOW_FOURTH_BYTE] = static_cast<uint8_t>(uuid & 0x00FF);
    return tmp;
}

Uuid Uuid::ConvertFrom32Bits(uint32_t uuid)
{
    Uuid tmp;
    tmp.uuid_[UUID_TIME_LOW_FIRST_BYTE] =
        static_cast<uint8_t>((uuid & 0xFF000000) >> BIT_OPT_THREE_BYTE * BASE_BIT_OPT_SIZE);
    tmp.uuid_[UUID_TIME_LOW_SECEND_BYTE] =
        static_cast<uint8_t>((uuid & 0x00FF0000) >> BIT_OPT_TWO_BYTE * BASE_BIT_OPT_SIZE);
    tmp.uuid_[UUID_TIME_LOW_THIRD_BYTE] =
        static_cast<uint8_t>((uuid & 0x0000FF00) >> BASE_BIT_OPT_SIZE);
    tmp.uuid_[UUID_TIME_LOW_FOURTH_BYTE] =
        static_cast<uint8_t>(uuid & 0x000000FF);
    return tmp;
}

Uuid Uuid::ConvertFromBytesLE(const uint8_t *uuid, const size_t size)
{
    Uuid leUuid;
    if (size < UUID128_BYTES_TYPE) {
        return leUuid;
    }
    UUID128Bit le;
    if (memcpy_s(le.data(), UUID128_BYTES_TYPE, uuid, UUID128_BYTES_TYPE) != EOK) {
        return leUuid;
    }
    std::reverse_copy(le.data(), le.data() + UUID128_BYTES_TYPE, leUuid.uuid_.begin());
    return leUuid;
}

Uuid Uuid::ConvertFromMostAndLeastBit(uint64_t mostSigBits, uint64_t leastSigBits)
{
    Uuid tmp;
    int division = UUID128_BYTES_TYPE / SIZE_STRING_TO_INT;

    for (int i = 0; i < division; i++) {
        tmp.uuid_[i] = (mostSigBits >> (BASE_BIT_OPT_SIZE * (division - i - 1))) & 0xFF;
    }

    for (int i = division; i < UUID128_BYTES_TYPE; i++) {
        tmp.uuid_[i] = (leastSigBits >> (BASE_BIT_OPT_SIZE * (2*division - i - 1))) & 0xFF;
    }

    return tmp;
}

Uuid Uuid::ConvertFrom128Bits(const UUID128Bit &uuid)
{
    Uuid tmp;
    tmp.uuid_ = uuid;
    return tmp;
}

uint16_t Uuid::ConvertTo16Bits() const
{
    uint16_t ret = uuid_[UUID_TIME_LOW_THIRD_BYTE] & 0xFF;
    ret = (ret << BASE_BIT_OPT_SIZE) + uuid_[UUID_TIME_LOW_FOURTH_BYTE];
    return ret;
}

uint32_t Uuid::ConvertTo32Bits() const
{
    uint32_t ret = 0;
    for (int i = 0; i < UUID32_BYTES_TYPE; i++) {
        ret = (ret << BASE_BIT_OPT_SIZE) + uuid_[i];
    }
    return ret;
}

bool Uuid::ConvertToBytesLE(uint8_t *value, const size_t size) const
{
    if (size < UUID128_BYTES_TYPE) {
        return false;
    }

    UUID128Bit le;
    std::reverse_copy(uuid_.data(), uuid_.data() + UUID128_BYTES_TYPE, le.begin());
    if (memcpy_s(value, UUID128_BYTES_TYPE, le.data(), UUID128_BYTES_TYPE) != EOK) {
        return false;
    }
    return true;
}

Uuid::UUID128Bit Uuid::ConvertTo128Bits() const
{
    return uuid_;
}

int Uuid::GetUuidType() const
{
    for (int i = UUID32_BYTES_TYPE; i < UUID128_BYTES_TYPE; i++) {
        if (BASE_UUID[i] != uuid_[i]) {
            return UUID128_BYTES_TYPE;
        }
    }

    if ((uuid_[0] == 0) && (uuid_[1] == 0)) {
        return UUID16_BYTES_TYPE;
    }

    return UUID32_BYTES_TYPE;
}

bool Uuid::operator==(const Uuid &rhs) const
{
    return (uuid_ == rhs.uuid_);
}

bool Uuid::operator!=(const Uuid &rhs) const
{
    return (uuid_ != rhs.uuid_);
}
}  // namespace bluetooth
}  // namespace OHOS