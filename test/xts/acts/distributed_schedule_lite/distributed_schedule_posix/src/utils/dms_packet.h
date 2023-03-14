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

#ifndef DMS_TEST_UTILS_PACKET_H
#define DMS_TEST_UTILS_PACKET_H

#include <cstdint>
#include <string>

constexpr int PACKET_DATA_SIZE = 2048;

enum class MSG_TYPE_SEQ : uint8_t {
    COMMAND_ID = 1,
    CALLEE_PKG_NAME,
    CALLEE_ABILITY_NAME,
    CALLER_SIGNATURE,
};

enum class DMSLITE_COMMAND : uint8_t {
    START_ABILITY_FROM_REMOTE = 1,
};

struct DmsMsgInfo {
    uint8_t commandId {0};
    std::string calleePkgName;
    std::string calleeAbilityName;
    std::string callerSignature;

    DmsMsgInfo(DMSLITE_COMMAND cmdId, std::string pkgName, std::string abilityName, std::string callerSignature)
        : commandId(static_cast<uint8_t>(cmdId)), calleePkgName(std::move(pkgName)),
        calleeAbilityName(std::move(abilityName)), callerSignature(std::move(callerSignature)) {}
};

class DmsPacket {
public:
    DmsPacket(char* buffer, uint16_t bufferSize);
    virtual ~DmsPacket() = default;

    bool BuildDmsPacket(const DmsMsgInfo& dmsMsgInfo, uint16_t& bufferSize);

private:
    bool MarshallStringField(const std::string& field, MSG_TYPE_SEQ type, uint16_t& dataLength);
    bool MarshallDmsMsgInfo(const DmsMsgInfo& dmsMsgInfo);

    bool StringToHex(const std::string& stringValue);
    void ToHex(uint64_t value, uint8_t typeSize);
    uint8_t EncodeLengthOfTlv(uint16_t length);

    template<typename T>
    typename std::enable_if<std::is_unsigned<T>::value, bool>::type IntToHex(uint64_t integerValue);
    template<typename T>
    typename std::enable_if<std::is_signed<T>::value, bool>::type IntToHex(int64_t integerValue);

private:
    char* buffer_ {nullptr};
    uint16_t bufferCapacity_ {0};
    uint16_t counter_ {0};
};

#endif
