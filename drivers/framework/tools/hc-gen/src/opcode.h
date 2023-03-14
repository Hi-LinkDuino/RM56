/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HC_GEN_OPCODE_H
#define HC_GEN_OPCODE_H

#include <cstdint>

namespace OHOS {
namespace Hardware {
constexpr uint32_t HCB_MAGIC_NUM = 0xA00AA00A;

enum OpCodeType {
    HCS_NODE_OP = 0x01,
    HCS_TERM_OP = 0x02,
    HCS_NODEREF_OP = 0x03,
    HCS_ARRAY_OP = 0x04,
    HCS_BYTE_OP = 0x10,
    HCS_WORD_OP = 0x11,
    HCS_DWORD_OP = 0x12,
    HCS_QWORD_OP = 0x13,
    HCS_STRING_OP = 0x14,
};

struct HcbHeader {
    uint32_t magicNumber;
    uint32_t versionMajor;
    uint32_t versionMinor;
    uint32_t checkSum;
    int32_t totalSize;
};
} // namespace Hardware
} // namespace OHOS

#endif // HC_GEN_OPCODE_H