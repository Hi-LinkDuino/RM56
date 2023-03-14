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

#include "obex_headers.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include "log.h"
#include "obex_utils.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
/************************* OBEX ObexHdrType *******************************/
const uint8_t ObexHdrType::UNICODE_TEXT;
const uint8_t ObexHdrType::BYTES;
const uint8_t ObexHdrType::BYTE;
const uint8_t ObexHdrType::WORD;
const uint8_t ObexHdrType::MASK;
/************************* OBEX ObexHdrSrmMode *******************************/
const uint8_t ObexHdrSrmMode::NORMAL;
const uint8_t ObexHdrSrmMode::SINGLE;
/************************* OBEX ObexHdrSrmpMode *******************************/
const uint8_t ObexHdrSrmpMode::WAIT;

const uint8_t ObexHeader::MIN_PACKET_LENGTH;
const uint8_t ObexHeader::TARGET_LENGTH;
const uint8_t ObexHeader::U16STRING_NULL_LENGTH;
const uint8_t ObexHeader::U16STRING_LENGTH;
const uint8_t ObexHeader::UINT16_LENGTH;
const uint8_t ObexHeader::UINT32_LENGTH;
const uint8_t ObexHeader::UINT64_LENGTH;
const uint8_t ObexHeader::HDR_UNICODE_PREFIX_LENGTH;
const uint8_t ObexHeader::HDR_BYTES_PREFIX_LENGTH;

const std::unordered_map<uint8_t, std::string> ObexHeader::HEADER_ID_NAME_MAP = {
    {0x00, "UNKNOWN"},
    {0xC0, "COUNT"},
    {0x01, "NAME"},
    {0x42, "TYPE"},
    {0xC3, "LENGTH"},
    {0x44, "TIME_ISO8601"},
    {0xC4, "TIME_4BYTE_VERSION"},
    {0x05, "DESCRIPTION"},
    {0x46, "TARGET"},
    {0x47, "HTTP"},
    {0x48, "BODY"},
    {0x49, "END_OF_BODY"},
    {0x4A, "WHO"},
    {0xCB, "CONNECTION_ID"},
    {0x4C, "APP_PARAMETERS"},
    {0x4D, "AUTH_CHALLENGE"},
    {0x4E, "AUTH_RESPONSE"},
    {0xCF, "CREATOR_ID"},
    {0x50, "WAN_UUID"},
    {0x51, "OBJECT_CLASS"},
    {0x52, "SESSION_PARAMETERS"},
    {0x93, "SESSION_SEQUENCE_NUMBER"},
    {0x94, "ACTION_ID"},
    {0x15, "DEST_NAME"},
    {0xD6, "PERMISSIONS"},
    {0x97, "SRM"},
    {0x98, "SRMP"}
};

ObexHeader::ObexHeader()
    : code_(0x00),
      packetLength_(0x00),
      obexVersionNum_(nullptr),
      flags_(nullptr),
      maxPacketLength_(nullptr),
      constants_(nullptr)
{}

const std::string &ObexHeader::GetHeaderName(uint8_t headerId)
{
    if (HEADER_ID_NAME_MAP.find(headerId) != HEADER_ID_NAME_MAP.end()) {
        return HEADER_ID_NAME_MAP.at(headerId);
    }
    return HEADER_ID_NAME_MAP.at(0x00);
}

void ObexHeader::ParseFields(const uint8_t *buf, const uint8_t &code, uint16_t &pos, bool isRequest, bool isConnect)
{
    if (isRequest) {
        if (code == static_cast<uint8_t>(ObexOpeId::CONNECT)) {
            uint8_t version = buf[pos++];
            uint8_t flags = buf[pos++];
            uint16_t maxPacketLength = ObexUtils::GetBufData16(buf, pos);
            pos += UINT16_LENGTH;
            SetFieldObexVersionNum(version);
            SetFieldFlags(flags);
            SetFieldMaxPacketLength(maxPacketLength);
        } else if (code == static_cast<uint8_t>(ObexOpeId::SETPATH)) {
            uint8_t flags = buf[pos++];
            uint8_t constants = buf[pos++];
            SetFieldFlags(flags);
            SetFieldConstants(constants);
        }
    } else if (isConnect) {
        // Connect response
        uint8_t version = buf[pos++];
        uint8_t flags = buf[pos++];
        uint16_t maxPacketLength = ObexUtils::GetBufData16(buf, pos);
        pos += UINT16_LENGTH;
        SetFieldObexVersionNum(version);
        SetFieldFlags(flags);
        SetFieldMaxPacketLength(maxPacketLength);
    }
}

bool ObexHeader::ParseOptionalHeaders(const uint8_t *buf, const uint16_t &size, uint16_t &pos)
{
    while (pos < size) {
        uint8_t headerId = buf[pos++];
        uint8_t headerDataType = headerId & ObexHdrType::MASK;
        OBEX_LOG_DEBUG(
            "ObexHeader Parse headerId=%02X(%{public}s), type=%02X",
            headerId, GetHeaderName(headerId).c_str(), headerDataType);
        switch (headerDataType) {
            case ObexHdrType::BYTE:
                AppendByte(headerId, buf[pos++]);
                break;
            case ObexHdrType::BYTES:
                ParseBytes(headerId, buf, pos);
                break;
            case ObexHdrType::UNICODE_TEXT:
                ParseUnicodeText(headerId, buf, pos);
                break;
            case ObexHdrType::WORD:
                AppendWord(headerId, ObexUtils::GetBufData32(buf, pos));
                pos += UINT32_LENGTH;
                break;
            default:
                OBEX_LOG_ERROR("parse error");
                return false;
        }
    }
    return true;
}

bool ObexHeader::Parse(const uint8_t *buf, const uint16_t size, bool isRequest, bool isConnect)
{
    OBEX_LOG_DEBUG("ObexHeader Parse: isRequest[%{public}d],isConnect:[%{public}d], buf size=[%{public}d]", isRequest, isConnect, size);
    isRequest_ = isRequest;
    uint16_t pos = 0;
    uint8_t code = buf[pos++];
    uint16_t packetLengthForCheck = ObexUtils::GetBufData16(buf, pos);
    OBEX_LOG_DEBUG("ObexHeader Parse: packetLength=%{public}d", int(packetLengthForCheck));
    pos += UINT16_LENGTH;
    if (packetLengthForCheck != size) {
        OBEX_LOG_ERROR("packetLength size is wrong packetSize=%{public}d, actualSize=%{public}d",
        int(packetLengthForCheck), int(size));
        return false;
    }
    code_ = code;
    packetLength_ = MIN_PACKET_LENGTH;

    ParseFields(buf, code, pos, isRequest, isConnect);

    if (!ParseOptionalHeaders(buf, size, pos)) {
        return false;
    }

    if (packetLength_ != packetLengthForCheck) {
        OBEX_LOG_ERROR(
            "parse error packetLength_[%{public}d] != packetLengthForCheck[%{public}d]", packetLength_, packetLengthForCheck);
        ObexUtils::ObexHeaderDebug(*this);
        return false;
    }
    return true;
}

void ObexHeader::ParseBytes(const uint8_t &headerId, const uint8_t *buf, uint16_t &pos)
{
    uint16_t dataLen = ObexUtils::GetBufData16(buf, pos) - HDR_BYTES_PREFIX_LENGTH;
    pos += UINT16_LENGTH;
    std::unique_ptr<ObexOptionalHeader> header = nullptr;
    switch (headerId) {
        case ObexHeader::TYPE:
            header = std::make_unique<ObexOptionalStringHeader>(headerId, &buf[pos], dataLen);
            Append(header);
            break;
        case ObexHeader::APP_PARAMETERS:
        case ObexHeader::SESSION_PARAMETERS:
        case ObexHeader::AUTH_CHALLENGE:
        case ObexHeader::AUTH_RESPONSE:
            header = std::make_unique<ObexOptionalTlvHeader>(headerId, &buf[pos], dataLen);
            Append(header);
            break;
        default:
            header = std::make_unique<ObexOptionalBytesHeader>(headerId, &buf[pos], dataLen);
            Append(header);
            break;
    }
    pos += dataLen;
}

void ObexHeader::ParseUnicodeText(const uint8_t &headerId, const uint8_t *buf, uint16_t &pos)
{
    uint16_t bytesLen = ObexUtils::GetBufData16(buf, pos) - HDR_UNICODE_PREFIX_LENGTH;
    pos += UINT16_LENGTH;
    if (bytesLen > 0) {
        auto tmpBuf = std::make_unique<uint8_t[]>(bytesLen);
        if (memcpy_s(tmpBuf.get(), bytesLen, &buf[pos], bytesLen) != EOK) {
            OBEX_LOG_ERROR("ParseUnicodeText, memcpy_s fail");
            return;
        }
        if (!ObexUtils::SysIsBigEndian()) {
            ObexUtils::DataReverse(tmpBuf.get(), bytesLen, UINT16_LENGTH);
        }
        std::u16string str((char16_t *)tmpBuf.get());
        AppendUnicode(headerId, str);
    } else {
        AppendUnicode(headerId, u"");
    }
    pos += bytesLen;
}

ObexHeader::ObexHeader(const ObexHeader &header, bool fieldOnly)
{
    code_ = header.GetFieldCode();
    isRequest_ = header.isRequest_;
    uint16_t filedPacketLength = MIN_PACKET_LENGTH;
    if (header.GetFieldObexVersionNum() != nullptr) {
        obexVersionNum_ = std::make_unique<uint8_t>();
        *obexVersionNum_ = *header.GetFieldObexVersionNum();
        filedPacketLength += 1;
    }
    if (header.GetFieldFlags() != nullptr) {
        flags_ = std::make_unique<uint8_t>();
        *flags_ = *header.GetFieldFlags();
        filedPacketLength += 1;
    }
    if (header.GetFieldMaxPacketLength() != nullptr) {
        maxPacketLength_ = std::make_unique<uint16_t>();
        *maxPacketLength_ = *header.GetFieldMaxPacketLength();
        filedPacketLength += UINT16_LENGTH;
    }
    if (header.GetFieldConstants() != nullptr) {
        constants_ = std::make_unique<uint8_t>();
        *constants_ = *header.GetFieldConstants();
        filedPacketLength += 1;
    }
    if (fieldOnly) {
        packetLength_ = filedPacketLength;
    } else {
        packetLength_ = header.GetFieldPacketLength();
        for (auto &optHeader : header.GetOptionalHeaders()) {
            headerIndexMap_.insert(std::pair<uint8_t, size_t>(optHeader->GetHeaderId(), optionalHeaders_.size()));
            optionalHeaders_.push_back(optHeader->Clone());
        }
        SetExtendBodyObject(header.GetExtendBodyObject());
    }
}

std::unique_ptr<ObexHeader> ObexHeader::CreateRequest(ObexOpeId opcode)
{
    auto header = std::make_unique<ObexHeader>();
    header->isRequest_ = true;
    header->code_ = static_cast<uint8_t>(opcode);
    // code + pack length
    header->packetLength_ = MIN_PACKET_LENGTH;
    if (opcode == ObexOpeId::CONNECT) {
        header->SetFieldObexVersionNum(OBEX_VERSION);
        header->SetFieldFlags(0x00);
        header->SetFieldMaxPacketLength(OBEX_DEFAULT_MTU);
    } else if (opcode == ObexOpeId::SETPATH) {
        // 0bit backup a level before applying name (equivalent to ../ on many systems)
        // 1bit Don’t create folder if it does not exist, return an error instead.
        header->SetFieldFlags(0x00);
        header->SetFieldConstants(0x00);
    }
    return header;
}

std::unique_ptr<ObexHeader> ObexHeader::CreateResponse(ObexRspCode rspCode, bool isConnectResp)
{
    auto header = std::make_unique<ObexHeader>();
    header->isRequest_ = false;
    header->code_ = static_cast<uint8_t>(rspCode);
    // code + pack length
    header->packetLength_ = MIN_PACKET_LENGTH;
    if (isConnectResp) {
        header->SetFieldObexVersionNum(OBEX_VERSION);
        header->SetFieldFlags(0x00);
        header->SetFieldMaxPacketLength(OBEX_DEFAULT_MTU);
    }
    return header;
}

std::unique_ptr<ObexHeader> ObexHeader::ParseRequest(const uint8_t *buf, const uint16_t size)
{
    auto header = std::make_unique<ObexHeader>();
    if (header->Parse(buf, size, true, false)) {
        return header;
    }
    return nullptr;
}

std::unique_ptr<ObexHeader> ObexHeader::ParseResponse(const uint8_t *buf, const uint16_t size, bool isConnect)
{
    auto header = std::make_unique<ObexHeader>();
    if (header->Parse(buf, size, false, isConnect)) {
        return header;
    }
    return nullptr;
}

uint8_t ObexHeader::GetFieldCode() const
{
    return code_;
}

uint16_t ObexHeader::GetFieldPacketLength() const
{
    return packetLength_;
}

const uint8_t *ObexHeader::GetFieldObexVersionNum() const
{
    if (obexVersionNum_ == nullptr) {
        return nullptr;
    }
    return obexVersionNum_.get();
}

const uint8_t *ObexHeader::GetFieldFlags() const
{
    if (flags_ == nullptr) {
        return nullptr;
    }
    return flags_.get();
}

const uint16_t *ObexHeader::GetFieldMaxPacketLength() const
{
    if (maxPacketLength_ == nullptr) {
        return nullptr;
    }
    return maxPacketLength_.get();
}

const uint8_t *ObexHeader::GetFieldConstants() const
{
    if (constants_ == nullptr) {
        return nullptr;
    }
    return constants_.get();
}

void ObexHeader::SetRespCode(uint8_t code)
{
    if (!isRequest_) {
        code_ = code;
    }
}

void ObexHeader::SetFinalBit(bool finalBit)
{
    if (finalBit) {
        code_ = code_ | OBEX_FINAL_BIT_MASK;
    } else {
        code_ = code_ & ~OBEX_FINAL_BIT_MASK;
    }
}

void ObexHeader::SetFieldObexVersionNum(const uint8_t obexVersionNum)
{
    if (obexVersionNum_ == nullptr) {
        obexVersionNum_ = std::make_unique<uint8_t>();
        packetLength_ += 1;
    }
    *obexVersionNum_ = obexVersionNum;
}

void ObexHeader::SetFieldFlags(const uint8_t flags)
{
    if (flags_ == nullptr) {
        flags_ = std::make_unique<uint8_t>();
        packetLength_ += 1;
    }
    *flags_ = flags;
}

void ObexHeader::SetFieldMaxPacketLength(const uint16_t maxPacketLength)
{
    if (maxPacketLength_ == nullptr) {
        maxPacketLength_ = std::make_unique<uint16_t>();
        packetLength_ += UINT16_LENGTH;
    }
    *maxPacketLength_ = maxPacketLength;
}

void ObexHeader::SetFieldConstants(uint8_t constants)
{
    if (constants_ == nullptr) {
        constants_ = std::make_unique<uint8_t>();
        packetLength_ += 1;
    }
    *constants_ = constants;
}

void ObexHeader::Append(std::unique_ptr<ObexOptionalHeader> &header)
{
    uint8_t headerId = header->GetHeaderId();
    if (HasHeader(headerId)) {
        OBEX_LOG_ERROR("duplicate headerId: 0x%02X", headerId);
        return;
    }
    headerIndexMap_[headerId] = optionalHeaders_.size();
    packetLength_ += header->GetHeaderTotalSize();
    optionalHeaders_.push_back(std::move(header));
}

template<typename T>
const T ObexHeader::GetItem(uint8_t headerId) const
{
    const ObexOptionalHeader *headerItem = Get(headerId);
    if (headerItem == nullptr) {
        return nullptr;
    }
    return static_cast<T>(const_cast<ObexOptionalHeader *>(headerItem));
}

void ObexHeader::AppendUnicode(const uint8_t headerId, const std::u16string &text)
{
    std::unique_ptr<ObexOptionalHeader> header = std::make_unique<ObexOptionalUnicodeHeader>(headerId, text);
    Append(header);
}

void ObexHeader::AppendByte(const uint8_t headerId, const uint8_t byte)
{
    std::unique_ptr<ObexOptionalHeader> header = std::make_unique<ObexOptionalByteHeader>(headerId, byte);
    Append(header);
}

void ObexHeader::AppendBytes(const uint8_t headerId, const uint8_t *byteBuf, const uint32_t size)
{
    std::unique_ptr<ObexOptionalHeader> header = std::make_unique<ObexOptionalBytesHeader>(headerId, byteBuf, size);
    Append(header);
}

void ObexHeader::AppendWord(const uint8_t headerId, const uint32_t word)
{
    std::unique_ptr<ObexOptionalHeader> header = std::make_unique<ObexOptionalWordHeader>(headerId, word);
    Append(header);
}

void ObexHeader::AppendString(const uint8_t headerId, const std::string &str)
{
    std::unique_ptr<ObexOptionalHeader> header = std::make_unique<ObexOptionalStringHeader>(headerId, str);
    Append(header);
}

void ObexHeader::AppendTlvTriplets(const uint8_t headerId, ObexTlvParamters &tlvParamters)
{
    std::unique_ptr<ObexOptionalHeader> header = std::make_unique<ObexOptionalTlvHeader>(headerId, tlvParamters);
    Append(header);
}

std::vector<std::unique_ptr<ObexOptionalHeader>> const &ObexHeader::GetOptionalHeaders() const
{
    return optionalHeaders_;
}

// Header add methods
// bytes
bool ObexHeader::AppendItemTarget(const uint8_t *target, const uint16_t length)
{
    if (optionalHeaders_.size() != 0) {
        bool error = true;
        if (optionalHeaders_.size() == 1) {
            if (optionalHeaders_.at(0)->GetHeaderId() == ObexHeader::SESSION_SEQUENCE_NUMBER) {
                error = false;
            }
        }
        if (error) {
            OBEX_LOG_ERROR("Target must the first header in the request");
            return false;
        }
    }
    if (length != TARGET_LENGTH) {
        OBEX_LOG_ERROR("AppendItemTarget length is wrong!");
        return false;
    }
    AppendBytes(ObexHeader::TARGET, target, length);
    return true;
}

void ObexHeader::AppendItemTimeIso8601(const uint8_t *time, const uint16_t length)
{
    AppendBytes(ObexHeader::TIME_ISO8601, time, length);
}

void ObexHeader::AppendItemHttp(const uint8_t *http, const uint16_t length)
{
    AppendBytes(ObexHeader::HTTP, http, length);
}

void ObexHeader::AppendItemBody(const uint8_t *body, const uint16_t length)
{
    AppendBytes(ObexHeader::BODY, body, length);
}

void ObexHeader::AppendItemEndBody(const uint8_t *endBody, const uint16_t length)
{
    AppendBytes(ObexHeader::END_OF_BODY, endBody, length);
}

void ObexHeader::AppendItemWho(const uint8_t *who, const uint16_t length)
{
    AppendBytes(ObexHeader::WHO, who, length);
}

void ObexHeader::AppendItemObjectClass(const uint8_t *objectClass, const uint16_t length)
{
    AppendBytes(ObexHeader::OBJECT_CLASS, objectClass, length);
}

// tlv
void ObexHeader::AppendItemAppParams(ObexTlvParamters &params)
{
    AppendTlvTriplets(ObexHeader::APP_PARAMETERS, params);
}

void ObexHeader::AppendItemAuthChallenges(ObexDigestChallenge &challenges)
{
    AppendTlvTriplets(ObexHeader::AUTH_CHALLENGE, challenges);
}

void ObexHeader::AppendItemAuthResponse(ObexDigestResponse &responses)
{
    AppendTlvTriplets(ObexHeader::AUTH_RESPONSE, responses);
}

bool ObexHeader::AppendItemSessionParams(ObexSessionParameters &params)
{
    if (optionalHeaders_.size() != 0) {
        OBEX_LOG_ERROR("SessionParams must the first header in the request");
        return false;
    }
    AppendTlvTriplets(ObexHeader::SESSION_PARAMETERS, params);
    return true;
}

// String
void ObexHeader::AppendItemType(const std::string &type)
{
    AppendString(ObexHeader::TYPE, type);
}

// Word
bool ObexHeader::AppendItemConnectionId(const uint32_t connectionId)
{
    if (optionalHeaders_.size() != 0) {
        bool error = true;
        if (optionalHeaders_.size() == 1) {
            if (optionalHeaders_.at(0)->GetHeaderId() == ObexHeader::SESSION_SEQUENCE_NUMBER) {
                error = false;
            }
        }
        if (error) {
            OBEX_LOG_ERROR("ConnectionId must the first header in the request");
            return false;
        }
    }
    AppendWord(ObexHeader::CONNECTION_ID, connectionId);
    return true;
}

void ObexHeader::AppendItemCount(const uint32_t count)
{
    AppendWord(ObexHeader::COUNT, count);
}

void ObexHeader::AppendItemLength(const uint32_t length)
{
    AppendWord(ObexHeader::LENGTH, length);
}

void ObexHeader::AppendItemTime4byte(const uint32_t time)
{
    AppendWord(ObexHeader::TIME_4BYTE_VERSION, time);
}

void ObexHeader::AppendItemPermissions(const uint32_t permissions)
{
    AppendWord(ObexHeader::PERMISSIONS, permissions);
}

// Unicode
void ObexHeader::AppendItemName(const std::u16string &name)
{
    AppendUnicode(ObexHeader::NAME, name);
}

void ObexHeader::AppendItemDescription(const std::u16string &description)
{
    AppendUnicode(ObexHeader::DESCRIPTION, description);
}

void ObexHeader::AppendItemDestName(const std::u16string &destName)
{
    AppendUnicode(ObexHeader::DEST_NAME, destName);
}

// byte
bool ObexHeader::AppendItemSessionSeqNum(const uint8_t num)
{
    if (optionalHeaders_.size() != 0) {
        OBEX_LOG_ERROR("SessionSeqNum must the first header in the request");
        return false;
    }
    AppendByte(ObexHeader::SESSION_SEQUENCE_NUMBER, num);
    return true;
}

void ObexHeader::AppendItemActionId(const ObexActionType actionId)
{
    AppendByte(ObexHeader::ACTION_ID, static_cast<uint8_t>(actionId));
}

void ObexHeader::AppendItemSrm(const bool srm)
{
    AppendByte(ObexHeader::SRM, srm ? ObexHdrSrmMode::SINGLE : ObexHdrSrmMode::NORMAL);
}

void ObexHeader::AppendItemSrmp()
{
    AppendByte(ObexHeader::SRMP, ObexHdrSrmpMode::WAIT);
}

// obex header get methods
const ObexOptionalHeader *ObexHeader::Get(uint8_t headerId) const
{
    if (!HasHeader(headerId)) {
        return nullptr;
    }
    size_t index = headerIndexMap_.at(headerId);
    return optionalHeaders_.at(index).get();
}

// ObexOptionalHeader
const ObexOptionalHeader *ObexHeader::GetItemTarget() const
{
    return Get(ObexHeader::TARGET);
}

const ObexOptionalHeader *ObexHeader::GetItemTimeIso8601() const
{
    return Get(ObexHeader::TIME_ISO8601);
}

const ObexOptionalHeader *ObexHeader::GetItemHttp() const
{
    return Get(ObexHeader::HTTP);
}

const ObexOptionalHeader *ObexHeader::GetItemBody() const
{
    return Get(ObexHeader::BODY);
}

const ObexOptionalHeader *ObexHeader::GetItemEndBody() const
{
    return Get(ObexHeader::END_OF_BODY);
}

const ObexOptionalHeader *ObexHeader::GetItemWho() const
{
    return Get(ObexHeader::WHO);
}

const ObexOptionalHeader *ObexHeader::GetItemObjectClass() const
{
    return Get(ObexHeader::OBJECT_CLASS);
}

// ObexOptionalTlvHeader
const ObexOptionalTlvHeader *ObexHeader::GetItemAuthChallenges() const
{
    return GetItem<ObexOptionalTlvHeader *>(ObexHeader::AUTH_CHALLENGE);
}

const ObexOptionalTlvHeader *ObexHeader::GetItemSessionParams() const
{
    return GetItem<ObexOptionalTlvHeader *>(ObexHeader::SESSION_PARAMETERS);
}

const ObexOptionalTlvHeader *ObexHeader::GetItemAppParams() const
{
    return GetItem<ObexOptionalTlvHeader *>(ObexHeader::APP_PARAMETERS);
}

const ObexOptionalTlvHeader *ObexHeader::GetItemAuthResponse() const
{
    return GetItem<ObexOptionalTlvHeader *>(ObexHeader::AUTH_RESPONSE);
}

// ObexOptionalStringHeader
const ObexOptionalStringHeader *ObexHeader::GetItemType() const
{
    return GetItem<ObexOptionalStringHeader *>(ObexHeader::TYPE);
}

// ObexOptionalWordHeader
const ObexOptionalWordHeader *ObexHeader::GetItemCount() const
{
    return GetItem<ObexOptionalWordHeader *>(ObexHeader::COUNT);
}

const ObexOptionalWordHeader *ObexHeader::GetItemLength() const
{
    return GetItem<ObexOptionalWordHeader *>(ObexHeader::LENGTH);
}

const ObexOptionalWordHeader *ObexHeader::GetItemTime4byte() const
{
    return GetItem<ObexOptionalWordHeader *>(ObexHeader::TIME_4BYTE_VERSION);
}

const ObexOptionalWordHeader *ObexHeader::GetItemConnectionId() const
{
    return GetItem<ObexOptionalWordHeader *>(ObexHeader::CONNECTION_ID);
}

const ObexOptionalWordHeader *ObexHeader::GetItemPermissions() const
{
    return GetItem<ObexOptionalWordHeader *>(ObexHeader::PERMISSIONS);
}

// ObexOptionalUnicodeHeader
const ObexOptionalUnicodeHeader *ObexHeader::GetItemName() const
{
    return GetItem<ObexOptionalUnicodeHeader *>(ObexHeader::NAME);
}

const ObexOptionalUnicodeHeader *ObexHeader::GetItemDescription() const
{
    return GetItem<ObexOptionalUnicodeHeader *>(ObexHeader::DESCRIPTION);
}

const ObexOptionalUnicodeHeader *ObexHeader::GetItemDestName() const
{
    return GetItem<ObexOptionalUnicodeHeader *>(ObexHeader::DEST_NAME);
}

// ObexOptionalByteHeader
const ObexOptionalByteHeader *ObexHeader::GetItemSessionSeqNum() const
{
    return GetItem<ObexOptionalByteHeader *>(ObexHeader::SESSION_SEQUENCE_NUMBER);
}

const ObexOptionalByteHeader *ObexHeader::GetItemActionId() const
{
    return GetItem<ObexOptionalByteHeader *>(ObexHeader::ACTION_ID);
}

bool ObexHeader::HasHeader(const uint8_t headerId) const
{
    return headerIndexMap_.find(headerId) != headerIndexMap_.end();
}

void ObexHeader::RemoveItem(const uint8_t headerId)
{
    if (!HasHeader(headerId)) {
        return;
    }
    size_t index = headerIndexMap_.at(headerId);
    auto &header = optionalHeaders_.at(index);
    packetLength_ -= header->GetHeaderTotalSize();
    optionalHeaders_.erase(optionalHeaders_.begin() + index);
    headerIndexMap_.clear();
    for (size_t i = 0; i < optionalHeaders_.size(); i++) {
        auto &item = optionalHeaders_.at(i);
        headerIndexMap_[item->GetHeaderId()] = i;
    }
}

bool ObexHeader::GetItemSrm() const
{
    auto headerItem = GetItem<ObexOptionalByteHeader *>(ObexHeader::SRM);
    if (headerItem == nullptr) {
        return false;
    }
    return headerItem->GetByte() == ObexHdrSrmMode::SINGLE;
}

bool ObexHeader::GetItemSrmp() const
{
    auto headerItem = GetItem<ObexOptionalByteHeader *>(ObexHeader::SRMP);
    if (headerItem == nullptr) {
        return false;
    }
    return headerItem->GetByte() == ObexHdrSrmpMode::WAIT;
}

std::unique_ptr<ObexPacket> ObexHeader::Build() const
{
    auto obexPacket = std::make_unique<ObexPacket>(packetLength_);
    uint8_t *packetBuf = obexPacket->GetBuffer();
    uint16_t pos = 0;
    packetBuf[pos++] = code_;
    bool isBigEndian = ObexUtils::SysIsBigEndian();
    ObexUtils::SetBufData16(packetBuf, pos, packetLength_);
    pos += UINT16_LENGTH;
    if (obexVersionNum_ != nullptr) {
        packetBuf[pos++] = obexVersionNum_.get()[0];
    }
    if (flags_ != nullptr) {
        packetBuf[pos++] = flags_.get()[0];
    }
    if (constants_ != nullptr) {
        packetBuf[pos++] = constants_.get()[0];
    }
    if (maxPacketLength_ != nullptr) {
        ObexUtils::SetBufData16(packetBuf, pos, maxPacketLength_.get()[0]);
        pos += UINT16_LENGTH;
    }
    for (auto &headerItem : optionalHeaders_) {
        packetBuf[pos++] = headerItem->GetHeaderId();
        if (headerItem->HasLengthField()) {
            ObexUtils::SetBufData16(packetBuf, pos, headerItem->GetHeaderTotalSize());
            pos += UINT16_LENGTH;
        }
        (void)memcpy_s(
            &packetBuf[pos], packetLength_ - pos, headerItem->GetBytes().get(), headerItem->GetHeaderDataSize());
        if (!isBigEndian && headerItem->GetHeaderUnitLen() > 1) {
            ObexUtils::DataReverse(&packetBuf[pos], headerItem->GetHeaderDataSize(), headerItem->GetHeaderUnitLen());
        }
        pos += headerItem->GetHeaderDataSize();
    }
    return obexPacket;
}

const std::shared_ptr<ObexBodyObject> &ObexHeader::GetExtendBodyObject() const
{
    return extendBodyObject_;
}

void ObexHeader::SetExtendBodyObject(const std::shared_ptr<ObexBodyObject> &extendBodyObject)
{
    extendBodyObject_ = extendBodyObject;
}

TlvTriplet::TlvTriplet()
{
    tagId_ = 0x00;
    len_ = 0x00;
    unitLen_ = 1;
}

TlvTriplet::TlvTriplet(const uint8_t tagId, const uint8_t len, const uint8_t *val, const uint8_t unitLen)
{
    tagId_ = tagId;
    len_ = len;
    unitLen_ = unitLen;
    val_.insert(val_.end(), val, val + len);
}

TlvTriplet::TlvTriplet(const TlvTriplet &tlvTriplet)
    : TlvTriplet(tlvTriplet.tagId_, tlvTriplet.len_, tlvTriplet.val_.data(), tlvTriplet.unitLen_)
{}

TlvTriplet::TlvTriplet(const uint8_t tagId, const uint8_t val)
    : TlvTriplet(tagId, static_cast<uint8_t>(1), reinterpret_cast<const uint8_t *>(&val), static_cast<uint8_t>(1))
{}

TlvTriplet::TlvTriplet(const uint8_t tagId, const uint16_t val)
    : TlvTriplet(tagId, static_cast<uint8_t>(2), reinterpret_cast<const uint8_t *>(&val), static_cast<uint8_t>(2))
{}

TlvTriplet::TlvTriplet(const uint8_t tagId, const uint32_t val)
    : TlvTriplet(tagId, static_cast<uint8_t>(4), reinterpret_cast<const uint8_t *>(&val), static_cast<uint8_t>(4))
{}

TlvTriplet::TlvTriplet(const uint8_t tagId, const uint64_t val)
    : TlvTriplet(tagId, static_cast<uint8_t>(8), reinterpret_cast<const uint8_t *>(&val), static_cast<uint8_t>(8))
{}

uint8_t TlvTriplet::GetTagId() const
{
    return tagId_;
}

uint8_t TlvTriplet::GetLen() const
{
    return len_;
}

uint8_t TlvTriplet::GetUnitLen() const
{
    return unitLen_;
}

const uint8_t *TlvTriplet::GetVal() const
{
    return val_.data();
}

uint16_t TlvTriplet::GetUint16() const
{
    if (len_ == ObexHeader::UINT16_LENGTH) {
        if (unitLen_ > 1) {
            return *(reinterpret_cast<const uint16_t *>(GetVal()));
        }
        return ObexUtils::GetBufData16(GetVal());
    }
    return 0;
}

uint32_t TlvTriplet::GetUint32() const
{
    if (len_ == ObexHeader::UINT32_LENGTH) {
        if (unitLen_ > 1) {
            return *(reinterpret_cast<const uint32_t *>(GetVal()));
        }
        return ObexUtils::GetBufData32(GetVal());
    }
    return 0;
}

uint64_t TlvTriplet::GetUint64() const
{
    if (len_ == ObexHeader::UINT64_LENGTH) {
        if (unitLen_ > 1) {
            return *(reinterpret_cast<const uint64_t *>(GetVal()));
        }
        return ObexUtils::GetBufData64(GetVal());
    }
    return 0;
}

ObexOptionalHeader::ObexOptionalHeader(uint8_t headerId)
{
    headerId_ = headerId;
    dataSize_ = 0;
    unitLen_ = 1;
}

uint8_t ObexOptionalHeader::GetHeaderId() const
{
    return headerId_;
}

uint16_t ObexOptionalHeader::GetHeaderTotalSize() const
{
    uint8_t prefixLen = 1;
    if (HasLengthField()) {
        prefixLen += ObexHeader::UINT16_LENGTH;
    }
    return prefixLen + GetHeaderDataSize();
}

uint16_t ObexOptionalHeader::GetHeaderDataSize() const
{
    return dataSize_;
}

uint8_t ObexOptionalHeader::GetHeaderUnitLen() const
{
    return unitLen_;
}

// ObexOptionalBytesHeader
ObexOptionalBytesHeader::ObexOptionalBytesHeader(
    const uint8_t headerId, const uint8_t *data, const uint16_t dataSize, const uint16_t unitLen)
    : ObexOptionalHeader(headerId)
{
    dataSize_ = dataSize;
    data_.insert(data_.end(), data, data + dataSize);
    unitLen_ = unitLen;
}

std::unique_ptr<uint8_t[]> ObexOptionalBytesHeader::GetBytes() const
{
    if (dataSize_ < 1) {
        return nullptr;
    }
    if (data_.size() == 0) {
        return nullptr;
    }
    auto buf = std::make_unique<uint8_t[]>(dataSize_);
    (void)memcpy_s(buf.get(), dataSize_, data_.data(), dataSize_);
    return buf;
}

bool ObexOptionalBytesHeader::HasLengthField() const
{
    return true;
}

ObexHeaderDataType ObexOptionalBytesHeader::GetHeaderClassType() const
{
    return ObexHeaderDataType::BYTES;
}

std::string ObexOptionalBytesHeader::GetHeaderClassTypeName() const
{
    return "ObexOptionalBytesHeader";
}

std::unique_ptr<ObexOptionalHeader> ObexOptionalBytesHeader::Clone() const
{
    return std::make_unique<ObexOptionalBytesHeader>(GetHeaderId(), data_.data(), dataSize_, unitLen_);
}

// ObexOptionalStringHeader
ObexOptionalStringHeader::ObexOptionalStringHeader(const uint8_t headerId, const std::string &str)
    : ObexOptionalBytesHeader(headerId, reinterpret_cast<uint8_t *>(const_cast<char *>(str.c_str())),
    (str.size() == 0) ? 0 : (str.size() + 1), 1)
{}

ObexOptionalStringHeader::ObexOptionalStringHeader(const uint8_t headerId,
    const uint8_t *data, const uint16_t dataSize)
    : ObexOptionalBytesHeader(headerId, data, dataSize)
{}

std::string ObexOptionalStringHeader::GetString() const
{
    if (dataSize_ == 0) {
        return "";
    }
    std::string str((char *)data_.data(), dataSize_ - 1);
    return str;
}

ObexHeaderDataType ObexOptionalStringHeader::GetHeaderClassType() const
{
    return ObexHeaderDataType::STRING;
}

std::string ObexOptionalStringHeader::GetHeaderClassTypeName() const
{
    return "ObexOptionalStringHeader";
}

std::unique_ptr<ObexOptionalHeader> ObexOptionalStringHeader::Clone() const
{
    return std::make_unique<ObexOptionalStringHeader>(GetHeaderId(), GetString());
}

// ObexOptionalUnicodeHeader
ObexOptionalUnicodeHeader::ObexOptionalUnicodeHeader(const uint8_t headerId, const std::u16string &unicodeText)
    : ObexOptionalBytesHeader(headerId, reinterpret_cast<uint8_t *>(const_cast<char16_t *>(unicodeText.c_str())),
    (unicodeText.size() == 0) ? 0 : ((unicodeText.size() + 1) * ObexHeader::U16STRING_LENGTH),
    ObexHeader::U16STRING_LENGTH)
{}
std::u16string ObexOptionalUnicodeHeader::GetUnicodeText() const
{
    if (dataSize_ == 0) {
        return u"";
    }
    std::u16string str(
        (char16_t *)data_.data(), (dataSize_ - ObexHeader::U16STRING_NULL_LENGTH) / ObexHeader::U16STRING_LENGTH);
    return str;
}

ObexHeaderDataType ObexOptionalUnicodeHeader::GetHeaderClassType() const
{
    return ObexHeaderDataType::UNICODE_TEXT;
}

std::string ObexOptionalUnicodeHeader::GetHeaderClassTypeName() const
{
    return "ObexOptionalUnicodeHeader";
}

std::unique_ptr<ObexOptionalHeader> ObexOptionalUnicodeHeader::Clone() const
{
    return std::make_unique<ObexOptionalUnicodeHeader>(GetHeaderId(), GetUnicodeText());
}

// ObexOptionalByteHeader
ObexOptionalByteHeader::ObexOptionalByteHeader(const uint8_t headerId, const uint8_t byte)
    : ObexOptionalBytesHeader(headerId, reinterpret_cast<uint8_t *>(const_cast<uint8_t *>(&byte)), 1)
{}
bool ObexOptionalByteHeader::HasLengthField() const
{
    return false;
}

uint8_t ObexOptionalByteHeader::GetByte() const
{
    return data_.data()[0];
}

ObexHeaderDataType ObexOptionalByteHeader::GetHeaderClassType() const
{
    return ObexHeaderDataType::BYTE;
}

std::string ObexOptionalByteHeader::GetHeaderClassTypeName() const
{
    return "ObexOptionalByteHeader";
}

std::unique_ptr<ObexOptionalHeader> ObexOptionalByteHeader::Clone() const
{
    return std::make_unique<ObexOptionalByteHeader>(GetHeaderId(), GetByte());
}

// ObexOptionalWordHeader
ObexOptionalWordHeader::ObexOptionalWordHeader(const uint8_t headerId, const uint32_t word)
    : ObexOptionalBytesHeader(headerId, reinterpret_cast<uint8_t *>(const_cast<uint32_t *>(&word)),
    ObexHeader::UINT32_LENGTH, ObexHeader::UINT32_LENGTH)
{}
uint32_t ObexOptionalWordHeader::GetWord() const
{
    return ((const uint32_t *)data_.data())[0];
}

ObexHeaderDataType ObexOptionalWordHeader::GetHeaderClassType() const
{
    return ObexHeaderDataType::WORD;
}

std::string ObexOptionalWordHeader::GetHeaderClassTypeName() const
{
    return "ObexOptionalWordHeader";
}

std::unique_ptr<ObexOptionalHeader> ObexOptionalWordHeader::Clone() const
{
    return std::make_unique<ObexOptionalWordHeader>(GetHeaderId(), GetWord());
}

bool ObexOptionalWordHeader::HasLengthField() const
{
    return false;
}

// ObexOptionalTLVHeader
ObexOptionalTlvHeader::ObexOptionalTlvHeader(const uint8_t headerId) : ObexOptionalHeader(headerId)
{
    dataSize_ = 0;
    unitLen_ = 1;
}

ObexOptionalTlvHeader::ObexOptionalTlvHeader(const uint8_t headerId, const ObexTlvParamters &tlvParamters)
    : ObexOptionalTlvHeader(headerId)
{
    uint16_t dataSize = 0;
    auto newTlvParamters = std::make_unique<ObexTlvParamters>();
    for (auto &param : tlvParamters.GetTlvTriplets()) {
        dataSize += param->GetLen() + TAG_PREFIX_LEN;
        newTlvParamters->AppendTlvtriplet(*(param.get()));
    }
    tlvParamters_ = std::move(newTlvParamters);
    dataSize_ = dataSize;
}

ObexOptionalTlvHeader::ObexOptionalTlvHeader(const uint8_t headerId, const uint8_t *data, const uint16_t dataSize)
    : ObexOptionalTlvHeader(headerId)
{
    auto newTlvParamters = std::make_unique<ObexTlvParamters>();
    uint16_t pos = 0;
    while (pos < dataSize) {
        const uint8_t tagId = data[pos++];
        const uint8_t len = data[pos++];
        TlvTriplet tlv(tagId, len, &data[pos]);
        newTlvParamters->AppendTlvtriplet(tlv);
        pos += len;
    }
    tlvParamters_ = std::move(newTlvParamters);
    dataSize_ = dataSize;
}

const std::unique_ptr<ObexTlvParamters> &ObexOptionalTlvHeader::GetTlvParamters() const
{
    return tlvParamters_;
}

const TlvTriplet *ObexOptionalTlvHeader::GetTlvtriplet(const uint8_t tagId) const
{
    if (tlvParamters_ == nullptr) {
        return nullptr;
    }
    return tlvParamters_->GetTlvtriplet(tagId);
}

bool ObexOptionalTlvHeader::HasLengthField() const
{
    return true;
}

ObexHeaderDataType ObexOptionalTlvHeader::GetHeaderClassType() const
{
    return ObexHeaderDataType::TLV;
}

std::string ObexOptionalTlvHeader::GetHeaderClassTypeName() const
{
    return "ObexOptionalTlvHeader";
}

std::unique_ptr<uint8_t[]> ObexOptionalTlvHeader::GetBytes() const
{
    if (dataSize_ < 1) {
        return nullptr;
    }
    auto buf = std::make_unique<uint8_t[]>(dataSize_);
    (void)memset_s(buf.get(), dataSize_, 0x00, dataSize_);
    size_t p = 0;
    bool isBigEndian = ObexUtils::SysIsBigEndian();
    for (auto &param : GetTlvParamters()->GetTlvTriplets()) {
        *(buf.get() + p) = param->GetTagId();
        p++;
        *(buf.get() + p) = param->GetLen();
        p++;
        (void)memcpy_s(buf.get() + p, dataSize_ - p, param->GetVal(), param->GetLen());
        if (!isBigEndian && param->GetUnitLen() > 1) {
            ObexUtils::DataReverse(buf.get() + p, param->GetLen(), param->GetUnitLen());
        }
        p += param->GetLen();
    }
    return buf;
}

std::unique_ptr<ObexOptionalHeader> ObexOptionalTlvHeader::Clone() const
{
    return std::make_unique<ObexOptionalTlvHeader>(GetHeaderId(), *GetTlvParamters().get());
}

// ObexTlvParamters
const std::vector<std::unique_ptr<TlvTriplet>> &ObexTlvParamters::GetTlvTriplets() const
{
    return tlvTriplets_;
}

const TlvTriplet *ObexTlvParamters::GetTlvtriplet(const uint8_t tagId) const
{
    auto itr = std::find_if(tlvTriplets_.begin(), tlvTriplets_.end(),
        [tagId](const std::unique_ptr<TlvTriplet> &rhs)->bool {
        return (tagId == rhs->GetTagId());
    });
    if (itr != tlvTriplets_.end()) {
        return itr->get();
    }
    return nullptr;
}

void ObexTlvParamters::AppendTlvtriplet(const TlvTriplet &tlvTriplet)
{
    tlvTriplets_.push_back(std::make_unique<TlvTriplet>(tlvTriplet));
}

// ObexDigestChallenge

/************************* OBEX Auth Digest Challenge *******************************/
const uint8_t ObexDigestChallenge::NONCE;
const uint8_t ObexDigestChallenge::OPTIONS;
const uint8_t ObexDigestChallenge::REALM;
const TlvTriplet *ObexDigestChallenge::GetNonce() const
{
    return GetTlvtriplet(ObexDigestChallenge::NONCE);
}

const TlvTriplet *ObexDigestChallenge::GetOptions() const
{
    return GetTlvtriplet(ObexDigestChallenge::OPTIONS);
}

const TlvTriplet *ObexDigestChallenge::GetRealm() const
{
    return GetTlvtriplet(ObexDigestChallenge::REALM);
}

void ObexDigestChallenge::AppendNonce(const uint8_t *nonce, const uint8_t length)
{
    tlvTriplets_.push_back(std::make_unique<TlvTriplet>(ObexDigestChallenge::NONCE, length, nonce));
}

void ObexDigestChallenge::AppendOptions(const uint8_t options)
{
    tlvTriplets_.push_back(std::make_unique<TlvTriplet>(ObexDigestChallenge::OPTIONS, options));
}

void ObexDigestChallenge::AppendRealm(const uint8_t *realm, const uint8_t length)
{
    tlvTriplets_.push_back(std::make_unique<TlvTriplet>(ObexDigestChallenge::REALM, length, realm));
}

/************************* OBEX Auth Digest Response *******************************/
const uint8_t ObexDigestResponse::REQUEST_DIGEST;
const uint8_t ObexDigestResponse::USER_ID;
const uint8_t ObexDigestResponse::NONCE;
const uint8_t ObexDigestResponse::MAX_USER_ID_LEN;

const TlvTriplet *ObexDigestResponse::GetRequestDigest() const
{
    return GetTlvtriplet(ObexDigestResponse::REQUEST_DIGEST);
}

const TlvTriplet *ObexDigestResponse::GetUserId() const
{
    return GetTlvtriplet(ObexDigestResponse::USER_ID);
}

const TlvTriplet *ObexDigestResponse::GetNonce() const
{
    return GetTlvtriplet(ObexDigestResponse::NONCE);
}

void ObexDigestResponse::AppendRequestDigest(const uint8_t *requestDigest, const uint8_t length)
{
    tlvTriplets_.push_back(std::make_unique<TlvTriplet>(ObexDigestResponse::REQUEST_DIGEST, length, requestDigest));
}

void ObexDigestResponse::AppendUserId(const uint8_t *userId, const uint8_t length)
{
    tlvTriplets_.push_back(std::make_unique<TlvTriplet>(ObexDigestResponse::USER_ID, length, userId));
}

void ObexDigestResponse::AppendNonce(const uint8_t *nonce, const uint8_t length)
{
    tlvTriplets_.push_back(std::make_unique<TlvTriplet>(ObexDigestResponse::NONCE, length, nonce));
}

/************************* OBEX Session Parameter ID *******************************/
const uint8_t ObexSessionParameters::DEVICE_ADDRESS;
const uint8_t ObexSessionParameters::NONCE;
const uint8_t ObexSessionParameters::SESSION_ID;
const uint8_t ObexSessionParameters::NEXT_SEQUENCE_NUMBER;
const uint8_t ObexSessionParameters::TIMEOUT;
const uint8_t ObexSessionParameters::SESSION_OPCODE;
const TlvTriplet *ObexSessionParameters::GetDeviceAddress() const
{
    return GetTlvtriplet(ObexSessionParameters::DEVICE_ADDRESS);
}

const TlvTriplet *ObexSessionParameters::GetNonce() const
{
    return GetTlvtriplet(ObexSessionParameters::NONCE);
}

const TlvTriplet *ObexSessionParameters::GetSessionId() const
{
    return GetTlvtriplet(ObexSessionParameters::SESSION_ID);
}

const TlvTriplet *ObexSessionParameters::GetNextSeqNum() const
{
    return GetTlvtriplet(ObexSessionParameters::NEXT_SEQUENCE_NUMBER);
}

const TlvTriplet *ObexSessionParameters::GetTimeout() const
{
    return GetTlvtriplet(ObexSessionParameters::TIMEOUT);
}

const TlvTriplet *ObexSessionParameters::GetSessionOpcode() const
{
    return GetTlvtriplet(ObexSessionParameters::SESSION_OPCODE);
}

void ObexSessionParameters::AppendDeviceAddress(const uint8_t *deviceAddress, const uint8_t length)
{
    tlvTriplets_.push_back(std::make_unique<TlvTriplet>(ObexSessionParameters::DEVICE_ADDRESS, length, deviceAddress));
}

void ObexSessionParameters::AppendNonce(const uint8_t *nonce, const uint8_t length)
{
    tlvTriplets_.push_back(std::make_unique<TlvTriplet>(ObexSessionParameters::NONCE, length, nonce));
}

void ObexSessionParameters::AppendSessionId(const uint8_t *sessionId, const uint8_t length)
{
    tlvTriplets_.push_back(std::make_unique<TlvTriplet>(ObexSessionParameters::SESSION_ID, length, sessionId));
}

void ObexSessionParameters::AppendNextSeqNum(const uint8_t nextSeqNum)
{
    tlvTriplets_.push_back(std::make_unique<TlvTriplet>(ObexSessionParameters::NEXT_SEQUENCE_NUMBER, nextSeqNum));
}

void ObexSessionParameters::AppendTimeout(const uint32_t timeout)
{
    tlvTriplets_.push_back(std::make_unique<TlvTriplet>(ObexSessionParameters::TIMEOUT, timeout));
}

void ObexSessionParameters::AppendSessionOpcode(const SessionOpcode opcode)
{
    tlvTriplets_.push_back(
        std::make_unique<TlvTriplet>(ObexSessionParameters::SESSION_OPCODE, static_cast<uint8_t>(opcode)));
}
}  // namespace bluetooth
}  // namespace OHOS