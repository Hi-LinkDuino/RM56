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

#ifndef OBEX_UTILS_H
#define OBEX_UTILS_H

#include <cstdint>
#include <mutex>
#include "btstack.h"
#include "obex_headers.h"
#include "packet.h"

namespace OHOS {
namespace bluetooth {
class ObexUtils {
public:
    static bool SysIsBigEndian();
    static void DataReverse(uint8_t *data, const uint16_t &len, const uint8_t &unitLen);

    static void SetBufData16(uint8_t *bigEndData, uint16_t pos, const uint16_t &value);
    static void SetBufData32(uint8_t *bigEndData, uint16_t pos, const uint32_t &value);
    static void SetBufData64(uint8_t *bigEndData, uint16_t pos, const uint64_t &value);
    static uint16_t GetBufData16(const uint8_t *bigEndData, uint16_t pos = 0);
    static uint32_t GetBufData32(const uint8_t *bigEndData, uint16_t pos = 0);
    static uint64_t GetBufData64(const uint8_t *bigEndData, uint16_t pos = 0);
    static std::string ToDebugString(Packet &obexPacket);
    static std::string ToDebugString(const uint8_t *v, const size_t &s, bool wrap = false);
    static void ObexHeaderDebug(const ObexHeader &header);
    static std::string BtAddr2String(const BtAddr &addr);
    static std::vector<uint8_t> MakeNonce(const uint32_t &privateKey);
    static std::vector<uint8_t> MakeRequestDigest(const uint8_t *nonce, int sz, const std::string &password);
    static std::string UnicodeToUtf8(const std::u16string &str16);
    static std::u16string Utf8ToUnicode(const std::string &strUtf8);

private:
    static const uint16_t DEBUG_MAX_COL_COUNT = 10;
    static const uint16_t DEBUG_COL_ITEM_LEN = 2;
    static const size_t DEBUG_MAX_DATA_LEN = 100;
    static std::mutex g_mutex;
    static int g_bigEndian;
    ObexUtils() = default;
    virtual ~ObexUtils() = default;
    static void ObexHeaderItemDebug(const ObexOptionalHeader &hi);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // OBEX_UTILS_H