/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hcs_opcode.h"
#include <string.h>


OpCodeMapEntry g_byteCodeMap[PARSEROP_COUNT] = {
    [PARSEROP_UINT8]    = {HCS_BYTE_OP,     BYTE_SIZE, "Uint8"},
    [PARSEROP_UINT16]   = {HCS_WORD_OP,     WORD_SIZE, "Uint16"},
    [PARSEROP_UINT32]   = {HCS_DWORD_OP,    DWORD_SIZE, "Uint32"},
    [PARSEROP_UINT64]   = {HCS_QWORD_OP,    QWORD_SIZE, "Uint64"},
    [PARSEROP_STRING]   = {HCS_STRING_OP,   0, "String"},
    [PARSEROP_ARRAY]    = {HCS_ARRAY_OP,    WORD_SIZE, "Array"}, /* ElementCount - WORD */
    [PARSEROP_CONFNODE] = {HCS_NODE_OP,     DWORD_SIZE, "ConfigNode"}, /* SubSize - DWORD */
    [PARSEROP_CONFTERM] = {HCS_TERM_OP,     0, "ConfigTerm"},
    [PARSEROP_NODEREF]  = {HCS_NODEREF_OP,  DWORD_SIZE, "NodeRef"}, /* RefHashCode - DWORD */
};

const OpCodeMapEntry *HcsGetOpCodeMap(void)
{
    return g_byteCodeMap;
}

const OpCodeMapEntry *HcsParserObjectTypeToByteCode(uint32_t objectType)
{
    if (objectType >= PARSEROP_COUNT) {
        return NULL;
    }
    return &g_byteCodeMap[objectType];
}

const char *HcsOpcodeToStr(uint16_t objectType)
{
    if (objectType >= PARSEROP_COUNT) {
        return "illegal OpCode, out of range";
    }

    return g_byteCodeMap[objectType].opStr;
}

void HcsCalculateOpcodeSize(ParserObject *object)
{
    const OpCodeMapEntry *byteCodeMap = HcsGetOpCodeMap();
    uint32_t size = HcsAlign(OPCODE_BYTE_WIDTH) + HcsAlign(byteCodeMap[object->objectBase.type].size);
    switch (object->objectBase.opCode) {
        case HCS_NODE_OP: /* fall-through */
        case HCS_TERM_OP:
            /* name string */
            size += HcsAlign(strlen(object->objectBase.name) + 1); // add 1 for '\0'
            break;
        case HCS_STRING_OP:
            size += HcsAlign(strlen(object->objectBase.stringValue) + 1);
            break;
        default:
            break;
    }

    ParserObjectBase *child = object->objectBase.child;
    uint32_t subSize = 0;
    while (child != NULL) {
        subSize += child->size;
        child = child->next;
    }

    object->objectBase.size += subSize + size;
    object->objectBase.subSize = subSize;
}