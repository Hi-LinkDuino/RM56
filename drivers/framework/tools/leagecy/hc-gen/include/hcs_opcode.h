/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HCS_COMPILER_OPCODE_H
#define HCS_COMPILER_OPCODE_H

#include "hcs_parser.h"

#define HCS_NODE_OP         0x01
#define HCS_TERM_OP         0x02
#define HCS_NODEREF_OP      0x03
#define HCS_ARRAY_OP        0x04
#define HCS_BYTE_OP         0x10
#define HCS_WORD_OP         0x11
#define HCS_DWORD_OP        0x12
#define HCS_QWORD_OP        0x13
#define HCS_STRING_OP       0x14

typedef struct {
    uint8_t opCode;
    uint32_t size;
    const char *opStr;
} OpCodeMapEntry;

const OpCodeMapEntry *HcsGetOpCodeMap(void);

const OpCodeMapEntry *HcsParserObjectTypeToByteCode(uint32_t objectType);

void HcsCalculateOpcodeSize(ParserObject *object);

const char *HcsOpcodeToStr(uint16_t opCode);

#endif // HCS_COMPILER_OPCODE_H
