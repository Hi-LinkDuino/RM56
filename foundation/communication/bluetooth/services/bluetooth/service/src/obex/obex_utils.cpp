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

#include "obex_utils.h"
#include <algorithm>
#include <codecvt>
#include <cstring>
#include <endian.h>
#include <iomanip>
#include <locale>
#include <sstream>
#include "log.h"
#include "raw_address.h"
#include "securec.h"
#include "stub/message_digest.h"

namespace OHOS {
namespace bluetooth {
int ObexUtils::g_bigEndian = -1;

bool ObexUtils::SysIsBigEndian()
{

    if (g_bigEndian == -1) {
        union {
            int aInt;
            char bChar;
        } tempUnion;
        tempUnion.aInt = 1;
        if (tempUnion.bChar == 1) {
            g_bigEndian = 0;
        } else {
            g_bigEndian = 1;
        }
    }
    return g_bigEndian == 1;
}

std::mutex ObexUtils::g_mutex;

void ObexUtils::DataReverse(uint8_t *data, const uint16_t &len, const uint8_t &unitLen)
{
    if (unitLen < 1) {
        OBEX_LOG_ERROR("LittleToBigEndian: unitLen is incorrect");
        return;
    }
    if (unitLen == 1) {
        return;
    }
    for (uint16_t pos = 0; pos < len; pos += unitLen) {
        for (uint16_t subPos = 0; subPos < unitLen / 0x02; subPos++) {
            uint8_t temp = data[pos + subPos];
            data[pos + subPos] = data[pos + (unitLen - subPos - 1)];
            data[pos + (unitLen - subPos - 1)] = temp;
        }
    }
}

void ObexUtils::SetBufData16(uint8_t *bigEndData, uint16_t pos, const uint16_t &value)
{
    *(reinterpret_cast<uint16_t *>(&bigEndData[pos])) = htobe16(value);
}

void ObexUtils::SetBufData32(uint8_t *bigEndData, uint16_t pos, const uint32_t &value)
{
    *(reinterpret_cast<uint32_t *>(&bigEndData[pos])) = htobe32(value);
}

void ObexUtils::SetBufData64(uint8_t *bigEndData, uint16_t pos, const uint64_t &value)
{
    *(reinterpret_cast<uint64_t *>(&bigEndData[pos])) = htobe64(value);
}

uint16_t ObexUtils::GetBufData16(const uint8_t *bigEndData, uint16_t pos)
{
    return be16toh(*(reinterpret_cast<uint16_t *>(const_cast<uint8_t *>(&bigEndData[pos]))));
}

uint32_t ObexUtils::GetBufData32(const uint8_t *bigEndData, uint16_t pos)
{
    return be32toh(*(reinterpret_cast<uint32_t *>(const_cast<uint8_t *>(&bigEndData[pos]))));
}

uint64_t ObexUtils::GetBufData64(const uint8_t *bigEndData, uint16_t pos)
{
    return be64toh(*(reinterpret_cast<uint64_t *>(const_cast<uint8_t *>(&bigEndData[pos]))));
}

std::string ObexUtils::ToDebugString(Packet &obexPacket)
{
    Buffer *tmpBuffer = PacketContinuousPayload(&obexPacket);
    uint8_t *packetBuf = (uint8_t *)BufferPtr(tmpBuffer);
    size_t packetBufSize = BufferGetSize(tmpBuffer);
    return ToDebugString(packetBuf, packetBufSize, true);
}

std::string ObexUtils::ToDebugString(const uint8_t *v, const size_t &s, bool wrap)
{
    std::stringstream ss;
    ss << std::uppercase << std::hex << std::setfill('0');
    for (size_t i = 0; i < s; i++) {
        if (wrap && i > 0 && i % DEBUG_MAX_COL_COUNT == 0) {
            ss << std::endl;
        }
        if (i == DEBUG_MAX_DATA_LEN) {
            ss << "......";
            break;
        }
        ss << std::hex << std::setw(DEBUG_COL_ITEM_LEN) << static_cast<int>(v[i]) << " ";
    }
    return ss.str();
}

void ObexUtils::ObexHeaderItemDebug(const ObexOptionalHeader &hi)
{
    OBEX_LOG_INFO("HeaderId:0x%02X", hi.GetHeaderId());
        OBEX_LOG_INFO("DataSize(dec):%{public}d, AllSize(dec):%{public}d",
        int(hi.GetHeaderDataSize()), int(hi.GetHeaderTotalSize()));
    OBEX_LOG_INFO("DataType:0x%02hhX, DataTypeName:%{public}s", hi.GetHeaderClassType(), hi.GetHeaderClassTypeName().c_str());
    switch (hi.GetHeaderClassType()) {
        case ObexHeaderDataType::BYTES: {
            auto bytes = hi.GetBytes();
            if (bytes) {
                OBEX_LOG_INFO("BYTES:%{public}s", ToDebugString(bytes.get(), hi.GetHeaderDataSize(), false).c_str());
            } else {
                OBEX_LOG_INFO("BYTES: null");
            }
            break;
        }
        case ObexHeaderDataType::BYTE:
            OBEX_LOG_INFO("BYTE:0x%02X", static_cast<const ObexOptionalByteHeader *>(&hi)->GetByte());
            break;
        case ObexHeaderDataType::WORD:
            OBEX_LOG_INFO("WORD(dec):%{public}d", static_cast<const ObexOptionalWordHeader *>(&hi)->GetWord());
            break;
        case ObexHeaderDataType::UNICODE_TEXT:
            OBEX_LOG_INFO("UNICODE_TEXT:%{public}s",
                UnicodeToUtf8(static_cast<const ObexOptionalUnicodeHeader *>(&hi)->GetUnicodeText()).c_str());
            break;
        case ObexHeaderDataType::STRING:
            OBEX_LOG_INFO("String:%{public}s", static_cast<const ObexOptionalStringHeader *>(&hi)->GetString().c_str());
            break;
        case ObexHeaderDataType::TLV: {
            auto &tlvParamters = static_cast<const ObexOptionalTlvHeader *>(&hi)->GetTlvParamters();
            if (tlvParamters != nullptr) {
                int index = 0;
                for (auto &pm : static_cast<const ObexOptionalTlvHeader *>(&hi)->GetTlvParamters()->GetTlvTriplets()) {
                    OBEX_LOG_INFO("----TlvHeader At:%{public}d, TAG:0x%02X, LEN:%{public}d", index, pm->GetTagId(), pm->GetLen());
                    OBEX_LOG_INFO("--VAL:%{public}s", ObexUtils::ToDebugString(pm->GetVal(), pm->GetLen(), false).c_str());
                    index++;
                }
            }
            break;
        }
        default:
            OBEX_LOG_ERROR("HdrType is illegal");
            break;
    }
    OBEX_LOG_INFO("DATA HEX:\n%{public}s", ObexUtils::ToDebugString(hi.GetBytes().get(), hi.GetHeaderDataSize(), true).c_str());
}

void ObexUtils::ObexHeaderDebug(const ObexHeader &header)
{
    OBEX_LOG_INFO("----------ObexHeader Debug Start----------");

    OBEX_LOG_INFO("Code:0x%02X", header.GetFieldCode());
    OBEX_LOG_INFO("PacketLength(dec):%{public}d", int(header.GetFieldPacketLength()));
    if (header.GetFieldObexVersionNum() != nullptr) {
        OBEX_LOG_INFO("ObexVersionNum:0x%02X", header.GetFieldObexVersionNum()[0]);
    }
    if (header.GetFieldFlags() != nullptr) {
        OBEX_LOG_INFO("Flags:0x%02X", header.GetFieldFlags()[0]);
    }
    if (header.GetFieldMaxPacketLength() != nullptr) {
        OBEX_LOG_INFO("MaxPacketLength(dec):%{public}d", int(header.GetFieldMaxPacketLength()[0]));
    }
    if (header.GetFieldConstants() != nullptr) {
        OBEX_LOG_INFO("Constants:0x%02X", header.GetFieldConstants()[0]);
    }

    auto &headerList = header.GetOptionalHeaders();
    OBEX_LOG_INFO("--------------------OptionalHeaders Start");

    for (size_t i = 0; i < headerList.size(); i++) {
        OBEX_LOG_INFO("----------Header At:%zu", i);
        auto &headerItem = headerList.at(i);
        ObexHeaderItemDebug(*headerItem);
    }
    OBEX_LOG_INFO("--------------------OptionalHeaders End");

    auto obexPacket = header.Build();
    OBEX_LOG_INFO(
        "ALL HEX:\n%{public}s", ObexUtils::ToDebugString(obexPacket->GetBuffer(), obexPacket->GetSize(), true).c_str());

    OBEX_LOG_INFO("----------ObexHeader Debug End----------");
}

std::string ObexUtils::BtAddr2String(const BtAddr &addr)
{
    return RawAddress::ConvertToString(addr.addr).GetAddress();
}

std::vector<uint8_t> ObexUtils::MakeNonce(const uint32_t &privateKey)
{
    std::unique_lock<std::mutex> unilock(g_mutex);

    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    const int bufSize = 20;
    char secBuf[bufSize] = {0};
    (void)sprintf_s(secBuf, sizeof(secBuf), "%ld", tp.tv_sec);
    std::string sec(secBuf);

    char nsecBuf[bufSize] = {0};
    (void)sprintf_s(nsecBuf, sizeof(nsecBuf), "%ld", tp.tv_nsec);
    std::string nsec(nsecBuf);

    std::string timeStamp = sec + nsec;

    char key[bufSize] = {0};
    (void)sprintf_s(key, sizeof(key), "%{public}d", int(privateKey));
    std::string priKey(key);

    std::string nonce = timeStamp + ":" + priKey;

    stub::MessageDigest *digest = stub::MessageDigestFactory::GetInstance(stub::DIGEST_TYPE_MD5);

    return digest->Digest((uint8_t *)nonce.data(), nonce.size());
}

std::vector<uint8_t> ObexUtils::MakeRequestDigest(const uint8_t *nonce, int sz, const std::string &password)
{
    std::unique_lock<std::mutex> unilock(g_mutex);
    const int bufSize = 256;
    uint8_t nonceBuf[bufSize];
    (void)memcpy_s(nonceBuf, sizeof(nonceBuf), nonce, sz);
    nonceBuf[sz] = ':';
    (void)memcpy_s(&nonceBuf[sz + 1], sizeof(nonceBuf) - sz - 1,
        reinterpret_cast<uint8_t *>(const_cast<char *>(password.c_str())), password.size());

    stub::MessageDigest *digest = stub::MessageDigestFactory::GetInstance(stub::DIGEST_TYPE_MD5);
    std::vector<uint8_t> vc = digest->Digest((uint8_t *)nonceBuf, sz + password.size() + 1);

    return vc;
}

std::string ObexUtils::UnicodeToUtf8(const std::u16string &str16)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.to_bytes(str16);
}

std::u16string ObexUtils::Utf8ToUnicode(const std::string &strUtf8)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.from_bytes(strUtf8);
}
}  // namespace bluetooth
}  // namespace OHOS