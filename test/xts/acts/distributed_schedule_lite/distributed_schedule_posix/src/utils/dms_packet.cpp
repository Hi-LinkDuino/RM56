/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "dms_packet.h"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <limits>
#include <string>

namespace {
#define CHECK_BOUNDARY()                                                                                    \
    if ((integerValue < std::numeric_limits<T>::min()) || (integerValue > std::numeric_limits<T>::max())) { \
        printf("CHECK_BOUNDARY: bad boundary\n");                                                           \
        return false;                                                                                       \
    }

constexpr uint8_t TLV_LENGTH_SHIFT_BITS = 7;
constexpr uint8_t LOW_BIT_MASK = 0x7F;
constexpr uint8_t HIGH_BIT_MASK = 0x80;
constexpr uint8_t BYTE_MASK = 0xFF;

constexpr uint8_t ONE_BYTE_BITS_NUM = 8;
constexpr uint8_t ONE_BYTE_LENGTH = 1;
constexpr uint8_t TYPE_FILED_LENGTH = 1;

constexpr uint8_t MIN_BYTE_NUM_OF_LENGTH_FILED = 1;
constexpr uint8_t MAX_BYTE_NUM_OF_LENGTH_FILED = 2;
}

DmsPacket::DmsPacket(char* buffer, uint16_t bufferCapacity)
    : buffer_(buffer), bufferCapacity_(bufferCapacity), counter_(0) {}

bool DmsPacket::BuildDmsPacket(const DmsMsgInfo& dmsMsgInfo, uint16_t& bufferSize)
{
    bufferSize = 0;
    if (buffer_ == nullptr || bufferCapacity_ > PACKET_DATA_SIZE) {
        printf("DmsPacket:BuildDmsPacket bad parameters [bufferCapacity = %d]", bufferCapacity_);
        return false;
    }

    if (!MarshallDmsMsgInfo(dmsMsgInfo)) {
        printf("DmsPacket:MarshallDmsMsgInfo failed\n");
        return false;
    }

    bufferSize = counter_;
    return true;
}

bool DmsPacket::MarshallStringField(const std::string& field, MSG_TYPE_SEQ type, uint16_t& dataLength)
{
    if (field.empty()) {
        return false;
    }

    // one more byte for '\0'
    size_t sz = field.size() + 1;
    uint8_t bytesNum = 0;
    IntToHex<uint8_t>(static_cast<uint8_t>(type));
    bytesNum = EncodeLengthOfTlv(sz);
    dataLength += TYPE_FILED_LENGTH + bytesNum;
    if (dataLength + sz > bufferCapacity_) {
        printf("DmsPacket:MarshallStringField DmsMsgInfo is too big to fit\n");
        return false;
    }
    StringToHex(field);
    dataLength += sz;

    return true;
}

bool DmsPacket::MarshallDmsMsgInfo(const DmsMsgInfo& dmsMsgInfo)
{
    uint16_t dataLength = 0;
    uint8_t bytesNum = 0;

    // marshall command id
    IntToHex<uint8_t>(static_cast<uint8_t>(MSG_TYPE_SEQ::COMMAND_ID));
    bytesNum = EncodeLengthOfTlv(ONE_BYTE_LENGTH);
    IntToHex<uint8_t>(dmsMsgInfo.commandId);
    dataLength += (TYPE_FILED_LENGTH + bytesNum + ONE_BYTE_LENGTH);

    // marshall callee package name
    if (!MarshallStringField(dmsMsgInfo.calleePkgName, MSG_TYPE_SEQ::CALLEE_PKG_NAME, dataLength)) {
        return false;
    }

    // marshall callee ability name
    if (!MarshallStringField(dmsMsgInfo.calleeAbilityName, MSG_TYPE_SEQ::CALLEE_ABILITY_NAME, dataLength)) {
        return false;
    }

    // marshall caller signature
    if (!MarshallStringField(dmsMsgInfo.callerSignature, MSG_TYPE_SEQ::CALLER_SIGNATURE, dataLength)) {
        return false;
    }

    return true;
}

bool DmsPacket::StringToHex(const std::string& stringValue)
{
    if (stringValue.empty()) {
        return false;
    }

    for_each(std::begin(stringValue), std::end(stringValue), [this] (char ch) {
        buffer_[counter_++] = ch;
    });
    buffer_[counter_++] = '\0';

    return true;
}

template<typename T>
typename std::enable_if<std::is_unsigned<T>::value, bool>::type DmsPacket::IntToHex(uint64_t integerValue)
{
    CHECK_BOUNDARY();
    ToHex(integerValue, sizeof(T));
    return true;
}

template<typename T>
typename std::enable_if<std::is_signed<T>::value, bool>::type DmsPacket::IntToHex(int64_t integerValue)
{
    CHECK_BOUNDARY();
    ToHex(integerValue, sizeof(T));
    return true;
}

uint8_t DmsPacket::EncodeLengthOfTlv(uint16_t length)
{
    uint8_t bytesNum = MIN_BYTE_NUM_OF_LENGTH_FILED;
    buffer_[counter_] = ((length >> TLV_LENGTH_SHIFT_BITS) & LOW_BIT_MASK);
    if (buffer_[counter_]) {
        buffer_[counter_++] |= HIGH_BIT_MASK;
        buffer_[counter_++] = (length & LOW_BIT_MASK);
        bytesNum = MAX_BYTE_NUM_OF_LENGTH_FILED;
    } else {
        buffer_[counter_++] = (length & LOW_BIT_MASK);
    }

    return bytesNum;
}

void DmsPacket::ToHex(uint64_t value, uint8_t typeSize)
{
    for (int8_t i = typeSize - 1; i >= 0; i--) {
        uint8_t val = (value >> (static_cast<uint8_t>(i) * ONE_BYTE_BITS_NUM)) & BYTE_MASK;
        buffer_[counter_++] = val;
    }
}
