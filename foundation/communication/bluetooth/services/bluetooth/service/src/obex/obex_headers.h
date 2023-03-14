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

#ifndef OBEX_HEADERS_H
#define OBEX_HEADERS_H

#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>
#include <unordered_map>
#include "obex_body.h"
#include "obex_packet.h"
#include "obex_types.h"

namespace OHOS {
namespace bluetooth {
class ObexHdrType {
public:
    /************************* The 2 high order bits of HI have the following meanings *******************************/
    // 00, null terminated Unicode text, length prefixed with 2 byte unsigned integer
    static const uint8_t UNICODE_TEXT = 0x00;
    // 01, byte sequence, length prefixed with 2 byte unsigned integer
    // 00, null terminated Unicode text, length prefixed with 2 byte unsigned integer
    static const uint8_t BYTES = 0x40;
    // 10, 1 byte quantity
    // 00, null terminated Unicode text, length prefixed with 2 byte unsigned integer
    static const uint8_t BYTE = 0x80;
    // 11, 4 byte quantity – transmitted in network byte order (high byte first)
    // 00, null terminated Unicode text, length prefixed with 2 byte unsigned integer
    static const uint8_t WORD = 0xC0;
    // Mask 11000000
    static const uint8_t MASK = 0xC0;
};

/************************* OBEX the type of Action to perform *******************************/
enum class ObexActionType : uint8_t {
    COPY = 0x00,            // Copy
    MOVE_OR_RENAME = 0x01,  // Move/Rename
    SET_PERMISSIONS = 0x02  // Set Permissions
};

class ObexHdrSrmMode {
public:
    // normal response mode
    static const uint8_t NORMAL = 0x00;
    // single response mode (SRM)
    static const uint8_t SINGLE = 0x01;
};
class ObexHdrSrmpMode {
public:
    // only "wait" is supported by Bluetooth
    static const uint8_t WAIT = 0x01;
};

// Tag-Length-Value encoding scheme,
// used by Application parameters , Authenticate Challenge , Authenticate Response, and session_parameter
// Optional Header
class TlvTriplet {
public:
    virtual ~TlvTriplet() = default;
    explicit TlvTriplet(const uint8_t tagId, const uint8_t len, const uint8_t *val, const uint8_t unitLen = 1);
    explicit TlvTriplet(const uint8_t tagId, const uint8_t val);
    explicit TlvTriplet(const uint8_t tagId, const uint16_t val);
    explicit TlvTriplet(const uint8_t tagId, const uint32_t val);
    explicit TlvTriplet(const uint8_t tagId, const uint64_t val);
    explicit TlvTriplet(const TlvTriplet &tlvTriplet);
    uint8_t GetTagId() const;
    uint8_t GetLen() const;
    uint8_t GetUnitLen() const;
    const uint8_t *GetVal() const;
    uint16_t GetUint16() const;
    uint32_t GetUint32() const;
    uint64_t GetUint64() const;

private:
    TlvTriplet();
    uint8_t tagId_ = 0;
    uint8_t len_ = 0;
    std::vector<uint8_t> val_ {};
    uint8_t unitLen_ = 1;
};
/**
 * @brief ObexTlvParamters
 * ObexTlvParamters
 */
class ObexTlvParamters {
public:
    ObexTlvParamters() = default;
    virtual ~ObexTlvParamters() = default;
    void AppendTlvtriplet(const TlvTriplet &tlvTriplet);
    const std::vector<std::unique_ptr<TlvTriplet>> &GetTlvTriplets() const;
    const TlvTriplet *GetTlvtriplet(const uint8_t tagId) const;

protected:
    std::vector<std::unique_ptr<TlvTriplet>> tlvTriplets_ {};
};
/**
 * @brief ObexDigestChallenge
 * ObexDigestChallenge
 */
class ObexDigestChallenge : public ObexTlvParamters {
public:
    /************************* OBEX Auth Digest Challenge *******************************/
    // String of bytes representing the nonce. Value Len 16
    static const uint8_t NONCE = 0x00;
    // Optional Challenge Information Value Len 1
    static const uint8_t OPTIONS = 0x01;
    // A displayable string indicating which userid and/or
    // password to use. The first byte of the string is the
    // character set to use. The character set uses the
    // same values as those defined in IrLMP for the nickname.
    // Value Len N
    static const uint8_t REALM = 0x02;

    const TlvTriplet *GetNonce() const;
    const TlvTriplet *GetOptions() const;
    const TlvTriplet *GetRealm() const;
    void AppendNonce(const uint8_t *nonce, const uint8_t length);
    void AppendOptions(const uint8_t options);
    void AppendRealm(const uint8_t *realm, const uint8_t length);
};
class ObexDigestResponse : public ObexTlvParamters {
public:
    // String of bytes representing the request digest. Value Len 16
    static const uint8_t REQUEST_DIGEST = 0x00;
    // User ID string of length n. Max size is 20 bytes.
    static const uint8_t USER_ID = 0x01;
    // The nonce sent in the digest challenge string. Value Len 16
    static const uint8_t NONCE = 0x02;
    static const uint8_t MAX_USER_ID_LEN = 20;
    const TlvTriplet *GetRequestDigest() const;
    const TlvTriplet *GetUserId() const;
    const TlvTriplet *GetNonce() const;
    void AppendRequestDigest(const uint8_t *requestDigest, const uint8_t length);
    void AppendUserId(const uint8_t *userId, const uint8_t length);
    void AppendNonce(const uint8_t *nonce, const uint8_t length);
};
/**
 * @brief ObexSessionParameters
 * ObexSessionParameters
 */
class ObexSessionParameters : public ObexTlvParamters {
public:
    // Device Address
    // The device address of the device sending the header.
    // If running over IrDA this is the 32-bit device address.
    // For Bluetooth this is the 48-bit Bluetooth address.
    // If Running over TCP/IP this is the IP address.
    static const uint8_t DEVICE_ADDRESS = 0x00;
    // Nonce
    // The nonce is a value provided by the device, which will be used in the
    // creation of the session ID. This number must be unique for each session
    // created by the device. One method for creating the nonce is to start with a
    // random number then increment the value each time a new session is
    // created. The Nonce should be at least 4 bytes and at most 16 bytes in size.
    static const uint8_t NONCE = 0x01;
    // Session ID
    // This is a 16-byte value, which is formed by taking the device
    // address and nonce from the client and server and running the MD5 algorithm
    // over the resulting string. The Session ID is created as follows: MD5(“Client
    // Device Address” “Client Nonce” “Server Device Address” “Server Nonce”)
    static const uint8_t SESSION_ID = 0x02;
    // Next Sequence Number
    // This is a one-byte value sent by the server, which indicates the next
    // sequence number expected when the session is resumed.
    static const uint8_t NEXT_SEQUENCE_NUMBER = 0x03;
    // Timeout
    // This is a 4-byte value that contains the number of seconds a session can be
    // in suspend mode before it is considered closed. The value of 0xffffffff
    // indicates a timeout of infinity. This is the default timeout. If a device does not
    // send a timeout field then it can be assumed that the desired timeout is infinity.
    // The timeout in affect is the smallest timeout sent by the client or server.
    static const uint8_t TIMEOUT = 0x04;
    // Session Opcode
    // The session opcode is a 1-byte value sent by the client to indicate the
    // Session command that is to be performed. This tag-length-value is only sent
    // in SESSION commands and must be the first tag in the Session-Parameters
    // header. The session opcode definitions are defined in the bulleted list below.
    static const uint8_t SESSION_OPCODE = 0x05;
    /************************* OBEX Session Opcode *******************************/
    enum class SessionOpcode : uint8_t {
        CREATE = 0x00,      // Create Session
        CLOSE = 0x01,       // Close Session
        SUSPEND = 0x02,     // Suspend Session
        RESUME = 0x03,      // Resume Session
        SET_TIMEOUT = 0x04  // Set Timeout
    };
    const TlvTriplet *GetDeviceAddress() const;
    const TlvTriplet *GetNonce() const;
    const TlvTriplet *GetSessionId() const;
    const TlvTriplet *GetNextSeqNum() const;
    const TlvTriplet *GetTimeout() const;
    const TlvTriplet *GetSessionOpcode() const;
    void AppendDeviceAddress(const uint8_t *deviceAddress, const uint8_t length);
    void AppendNonce(const uint8_t *nonce, const uint8_t length);
    void AppendSessionId(const uint8_t *sessionId, const uint8_t length);
    void AppendNextSeqNum(const uint8_t nextSeqNum);
    void AppendTimeout(const uint32_t timeout);
    void AppendSessionOpcode(const SessionOpcode opcode);
};
/**
 * @brief ObexOptionalHeader's Data Type
 *
 */
enum class ObexHeaderDataType : uint8_t {
    UNICODE_TEXT = 0x00,  // Unicode text
    BYTES = 0x40,         // BYTES
    STRING,               // STRING
    TLV,                  // TLV
    BYTE = 0x80,          // BYTE
    WORD = 0XC0           // WORD
};
// Optional Header
class ObexOptionalHeader {
public:
    virtual ~ObexOptionalHeader() = default;
    virtual std::unique_ptr<ObexOptionalHeader> Clone() const = 0;
    uint8_t GetHeaderId() const;
    uint16_t GetHeaderTotalSize() const;
    uint16_t GetHeaderDataSize() const;
    uint8_t GetHeaderUnitLen() const;

    virtual bool HasLengthField() const = 0;
    virtual ObexHeaderDataType GetHeaderClassType() const = 0;
    virtual std::string GetHeaderClassTypeName() const = 0;
    virtual std::unique_ptr<uint8_t[]> GetBytes() const = 0;

protected:
    ObexOptionalHeader(uint8_t headerId);
    ObexOptionalHeader();
    uint8_t headerId_ = 0;
    uint16_t dataSize_ = 0;
    uint8_t unitLen_ = 1;
};
class ObexOptionalBytesHeader : public ObexOptionalHeader {
public:
    ObexOptionalBytesHeader(const uint8_t headerId, const uint8_t *data,
        const uint16_t dataSize, const uint16_t unitLen = 1);
    ~ObexOptionalBytesHeader() override = default;
    std::unique_ptr<uint8_t[]> GetBytes() const override;
    bool HasLengthField() const override;
    ObexHeaderDataType GetHeaderClassType() const override;
    std::string GetHeaderClassTypeName() const override;
    std::unique_ptr<ObexOptionalHeader> Clone() const override;

protected:
    std::vector<uint8_t> data_ {};
};

class ObexOptionalByteHeader : public ObexOptionalBytesHeader {
public:
    ObexOptionalByteHeader(const uint8_t headerId, const uint8_t byte);
    ~ObexOptionalByteHeader() override = default;
    bool HasLengthField() const override;
    uint8_t GetByte() const;
    ObexHeaderDataType GetHeaderClassType() const override;
    std::string GetHeaderClassTypeName() const override;
    std::unique_ptr<ObexOptionalHeader> Clone() const override;
};
class ObexOptionalWordHeader : public ObexOptionalBytesHeader {
public:
    ObexOptionalWordHeader(const uint8_t headerId, const uint32_t word);
    ~ObexOptionalWordHeader() override = default;
    bool HasLengthField() const override;
    uint32_t GetWord() const;
    ObexHeaderDataType GetHeaderClassType() const override;
    std::string GetHeaderClassTypeName() const override;
    std::unique_ptr<ObexOptionalHeader> Clone() const override;
};
class ObexOptionalStringHeader : public ObexOptionalBytesHeader {
public:
    ObexOptionalStringHeader(const uint8_t headerId, const std::string &str);
    ObexOptionalStringHeader(const uint8_t headerId, const uint8_t *data, const uint16_t dataSize);

    ~ObexOptionalStringHeader() override = default;
    std::string GetString() const;
    ObexHeaderDataType GetHeaderClassType() const override;
    std::string GetHeaderClassTypeName() const override;
    std::unique_ptr<ObexOptionalHeader> Clone() const override;
};
class ObexOptionalUnicodeHeader : public ObexOptionalBytesHeader {
public:
    ObexOptionalUnicodeHeader(const uint8_t headerId, const std::u16string &unicodeText);
    ~ObexOptionalUnicodeHeader() override = default;
    std::u16string GetUnicodeText() const;
    ObexHeaderDataType GetHeaderClassType() const override;
    std::string GetHeaderClassTypeName() const override;
    std::unique_ptr<ObexOptionalHeader> Clone() const override;
};
class ObexOptionalTlvHeader : public ObexOptionalHeader {
public:
    explicit ObexOptionalTlvHeader(const uint8_t headerId, const uint8_t *data, const uint16_t dataSize);
    ObexOptionalTlvHeader(const uint8_t headerId);
    ObexOptionalTlvHeader(const uint8_t headerId, const ObexTlvParamters &tlvParamters);
    ~ObexOptionalTlvHeader() override = default;
    std::unique_ptr<uint8_t[]> GetBytes() const override;
    bool HasLengthField() const override;
    ObexHeaderDataType GetHeaderClassType() const override;
    std::string GetHeaderClassTypeName() const override;
    std::unique_ptr<ObexOptionalHeader> Clone() const override;
    const std::unique_ptr<ObexTlvParamters> &GetTlvParamters() const;
    // Get TlvTriplet By TagId
    const TlvTriplet *GetTlvtriplet(const uint8_t tagId) const;

private:
    static const uint8_t TAG_PREFIX_LEN = 2;
    std::unique_ptr<ObexTlvParamters> tlvParamters_ = nullptr;
};
// Obex Header Struct

class ObexHeader {
public:
    /************************* The Header identifiers *******************************/
    // Number of objects (used by Connect)
    static const uint8_t COUNT = 0xC0;
    // name of the object (often a file name)
    static const uint8_t NAME = 0x01;
    // type of object - e.g. text, html, binary, manufacturer specific
    static const uint8_t TYPE = 0x42;
    // the length of the object in bytes
    static const uint8_t LENGTH = 0xC3;
    // date/time stamp – ISO 8601 version - preferred
    static const uint8_t TIME_ISO8601 = 0x44;
    // date/time stamp – 4 byte version (for compatibility only)
    static const uint8_t TIME_4BYTE_VERSION = 0xC4;
    // text description of the object
    static const uint8_t DESCRIPTION = 0x05;
    // name of service that operation is targeted to
    static const uint8_t TARGET = 0x46;
    // an HTTP 1.x header
    static const uint8_t HTTP = 0x47;
    // a chunk of the object body.
    static const uint8_t BODY = 0x48;
    // the final chunk of the object body
    static const uint8_t END_OF_BODY = 0x49;
    // identifies the OBEX application, used to tell if talking to a peer
    static const uint8_t WHO = 0x4A;
    // an identifier used for OBEX connection multiplexing
    static const uint8_t CONNECTION_ID = 0xCB;
    // extended application request & response information
    static const uint8_t APP_PARAMETERS = 0x4C;
    // authentication digest-challenge
    static const uint8_t AUTH_CHALLENGE = 0x4D;
    // authentication digest-response
    static const uint8_t AUTH_RESPONSE = 0x4E;
    // indicates the creator of an object
    static const uint8_t CREATOR_ID = 0xCF;
    // uniquely identifies the network client (OBEX server)
    static const uint8_t WAN_UUID = 0x50;
    // OBEX Object class of object
    static const uint8_t OBJECT_CLASS = 0x51;
    // Parameters used in session commands/responses
    static const uint8_t SESSION_PARAMETERS = 0x52;
    // Sequence number used in each OBEX packet for reliability
    static const uint8_t SESSION_SEQUENCE_NUMBER = 0x93;
    // Action (copy, move/rename, or set permissions)
    static const uint8_t ACTION_ID = 0x94;
    // Indicates the destination name for a Copy or Move/Rename action.
    static const uint8_t DEST_NAME = 0x15;
    // Indicates the permissions for a Set Permissions action.
    static const uint8_t PERMISSIONS = 0xD6;
    // Single Response Mode
    static const uint8_t SRM = 0x97;
    // Single Response Mode Parameter
    static const uint8_t SRMP = 0x98;
    // HeaderId Name Map
    static const std::unordered_map<uint8_t, std::string> HEADER_ID_NAME_MAP;
    /************************* The Header const *******************************/
    static const uint8_t MIN_PACKET_LENGTH = 3;
    static const uint8_t TARGET_LENGTH = 16;
    static const uint8_t U16STRING_NULL_LENGTH = 2;
    static const uint8_t U16STRING_LENGTH = 2;
    static const uint8_t UINT16_LENGTH = 2;
    static const uint8_t UINT32_LENGTH = 4;
    static const uint8_t UINT64_LENGTH = 8;
    static const uint8_t HDR_UNICODE_PREFIX_LENGTH = 3;
    static const uint8_t HDR_BYTES_PREFIX_LENGTH = 3;
    /**
     * @brief Create a Request object
     *
     * @param opcode Opcode code
     * @return std::unique_ptr<ObexHeader>
     */
    static std::unique_ptr<ObexHeader> CreateRequest(ObexOpeId opcode);
    /**
     * @brief Create a Response object
     *
     * @param rspCode Response Code
     * @param isConnectResp the response of connect request is set to true
     *                  otherwise otherwise set to false
     * @return std::unique_ptr<ObexHeader>
     */
    static std::unique_ptr<ObexHeader> CreateResponse(ObexRspCode rspCode, bool isConnectResp = false);
    static std::unique_ptr<ObexHeader> ParseRequest(const uint8_t *buf, const uint16_t size);
    static std::unique_ptr<ObexHeader> ParseResponse(const uint8_t *buf, const uint16_t size, bool isConnect);
    bool Parse(const uint8_t *buf, const uint16_t size, bool isRequest, bool isConnect);
    // note: please don't use this construction method
    ObexHeader();
    ObexHeader(const ObexHeader &header, bool fieldOnly = false);
    virtual ~ObexHeader() = default;
    uint8_t GetFieldCode() const;
    uint16_t GetFieldPacketLength() const;
    const uint8_t *GetFieldObexVersionNum() const;
    const uint8_t *GetFieldFlags() const;
    const uint16_t *GetFieldMaxPacketLength() const;
    const uint8_t *GetFieldConstants() const;

    void SetRespCode(uint8_t code);
    void SetFinalBit(bool finalBit);
    void SetFieldObexVersionNum(const uint8_t obexVersionNum);
    void SetFieldFlags(const uint8_t flags);
    void SetFieldMaxPacketLength(const uint16_t maxPacketLength);
    void SetFieldConstants(uint8_t constants);

    // obex header add methods
    // bytes
    bool AppendItemTarget(const uint8_t *target, const uint16_t length);
    void AppendItemTimeIso8601(const uint8_t *time, const uint16_t length);
    void AppendItemHttp(const uint8_t *http, const uint16_t length);
    void AppendItemBody(const uint8_t *body, const uint16_t length);
    void AppendItemEndBody(const uint8_t *endBody, const uint16_t length);
    void AppendItemWho(const uint8_t *who, const uint16_t length);
    void AppendItemObjectClass(const uint8_t *objectClass, const uint16_t length);

    // tlv
    void AppendItemAppParams(ObexTlvParamters &params);
    void AppendItemAuthChallenges(ObexDigestChallenge &challenges);
    void AppendItemAuthResponse(ObexDigestResponse &responses);
    bool AppendItemSessionParams(ObexSessionParameters &params);

    // String
    void AppendItemType(const std::string &type);

    // Word
    bool AppendItemConnectionId(const uint32_t connectionId);
    void AppendItemCount(const uint32_t count);
    void AppendItemLength(const uint32_t length);
    void AppendItemTime4byte(const uint32_t time);
    void AppendItemPermissions(const uint32_t permissions);

    // Unicode
    void AppendItemName(const std::u16string &name);
    void AppendItemDescription(const std::u16string &description);
    void AppendItemDestName(const std::u16string &destName);

    // byte
    bool AppendItemSessionSeqNum(const uint8_t num);
    /************************* OBEX the type of Action to perform *******************************/
    void AppendItemActionId(const ObexActionType actionId);
    void AppendItemSrm(const bool srm);
    void AppendItemSrmp();

    // obex header get methods
    const ObexOptionalHeader *Get(uint8_t headerId) const;

    // ObexOptionalHeader
    const ObexOptionalHeader *GetItemTarget() const;
    const ObexOptionalHeader *GetItemTimeIso8601() const;
    const ObexOptionalHeader *GetItemHttp() const;
    const ObexOptionalHeader *GetItemBody() const;
    const ObexOptionalHeader *GetItemEndBody() const;
    const ObexOptionalHeader *GetItemWho() const;
    const ObexOptionalHeader *GetItemObjectClass() const;

    // ObexOptionalTlvHeader
    const ObexOptionalTlvHeader *GetItemAuthChallenges() const;
    const ObexOptionalTlvHeader *GetItemSessionParams() const;
    const ObexOptionalTlvHeader *GetItemAppParams() const;
    const ObexOptionalTlvHeader *GetItemAuthResponse() const;

    // ObexOptionalStringHeader
    const ObexOptionalStringHeader *GetItemType() const;

    // ObexOptionalWordHeader
    const ObexOptionalWordHeader *GetItemCount() const;
    const ObexOptionalWordHeader *GetItemLength() const;
    const ObexOptionalWordHeader *GetItemTime4byte() const;
    const ObexOptionalWordHeader *GetItemConnectionId() const;
    const ObexOptionalWordHeader *GetItemPermissions() const;

    // ObexOptionalUnicodeHeader
    const ObexOptionalUnicodeHeader *GetItemName() const;
    const ObexOptionalUnicodeHeader *GetItemDescription() const;
    const ObexOptionalUnicodeHeader *GetItemDestName() const;

    // ObexOptionalByteHeader
    const ObexOptionalByteHeader *GetItemSessionSeqNum() const;
    const ObexOptionalByteHeader *GetItemActionId() const;
    bool GetItemSrm() const;
    bool GetItemSrmp() const;

    bool HasHeader(const uint8_t headerId) const;
    void RemoveItem(const uint8_t headerId);
    std::vector<std::unique_ptr<ObexOptionalHeader>> const &GetOptionalHeaders() const;

    std::unique_ptr<ObexPacket> Build() const;

    const std::shared_ptr<ObexBodyObject> &GetExtendBodyObject() const;
    void SetExtendBodyObject(const std::shared_ptr<ObexBodyObject> &extendBodyObject);

private:
    // response code OR operation code
    uint8_t code_ = 0;
    // obex package total byte length
    uint16_t packetLength_ = 0;
    bool isRequest_ = false;
    // used by connect
    std::unique_ptr<uint8_t> obexVersionNum_ = nullptr;
    // used by set_path and connect
    std::unique_ptr<uint8_t> flags_ = nullptr;
    // obex package max byte length, used by connect
    std::unique_ptr<uint16_t> maxPacketLength_ = nullptr;
    // used by set_path
    std::unique_ptr<uint8_t> constants_ = nullptr;

    template<typename T>
    const T GetItem(uint8_t headerId) const;

    void Append(std::unique_ptr<ObexOptionalHeader> &header);
    void AppendUnicode(const uint8_t headerId, const std::u16string &text);
    void AppendByte(const uint8_t headerId, const uint8_t byte);
    void AppendBytes(const uint8_t headerId, const uint8_t *byteBuf, const uint32_t size);
    void AppendWord(const uint8_t headerId, const uint32_t word);
    void AppendString(const uint8_t headerId, const std::string &str);
    void AppendTlvTriplets(const uint8_t headerId, ObexTlvParamters &tlvParamters);

    void ParseBytes(const uint8_t &headerId, const uint8_t *buf, uint16_t &pos);
    void ParseUnicodeText(const uint8_t &headerId, const uint8_t *buf, uint16_t &pos);
    void ParseFields(const uint8_t *buf, const uint8_t &code, uint16_t &pos, bool isRequest, bool isConnect);
    bool ParseOptionalHeaders(const uint8_t *buf, const uint16_t &size, uint16_t &pos);
    static const std::string &GetHeaderName(uint8_t headerId);
    std::vector<std::unique_ptr<ObexOptionalHeader>> optionalHeaders_ {};
    std::unordered_map<uint8_t, size_t> headerIndexMap_ {};
    std::shared_ptr<ObexBodyObject> extendBodyObject_ = nullptr;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // OBEX_HEADERS_H
