/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <string.h>
#include <hcs_gener.h>
#include "hcs_compiler.h"
#include "hcs_file.h"
#include "hcs_opcode.h"
#include "hcs_log.h"
#include "hcs_option.h"

#define DEBUG_PRINT_TAB_SIZE 2

static int32_t ByteCodeConvert(ParserObject *current, int32_t walkDepth)
{
    (void)walkDepth;
    /* template should not output */
    if (current->objectBase.type == PARSEROP_CONFNODE && current->configNode.nodeType == CONFIG_NODE_TEMPLATE) {
        HcsAstRemoveChildLink(current->objectBase.parent, &current->objectBase);
        HcsDeleteParserObjectTree(current);
        return NOERR;
    }
    const OpCodeMapEntry *opcodeEntry = HcsParserObjectTypeToByteCode(current->objectBase.type);
    if (opcodeEntry == NULL) {
        return EFAIL;
    }
    current->objectBase.opCode = opcodeEntry->opCode;

    /* calculate leaf size when forward walk */
    HcsCalculateOpcodeSize(current);

    return NOERR;
}

static void DumpParserObjectValue(const ParserObject *obj)
{
    switch (obj->objectBase.opCode) {
        case HCS_BYTE_OP:
        case HCS_WORD_OP:
        case HCS_DWORD_OP:
        case HCS_QWORD_OP:
            HCS_PRINT("%"PRIu64, obj->objectBase.integerValue);
            break;
        case HCS_STRING_OP:
            HCS_PRINT("%s", obj->objectBase.stringValue);
            break;
        default:
            break;
    }
}

static int32_t OpCodeDumpPrint(ParserObject *current, int32_t walkDepth)
{
    int32_t printTab = walkDepth * DEBUG_PRINT_TAB_SIZE;
    uint8_t opCode = current->objectBase.opCode;
    uint32_t size = current->objectBase.size;
    HCS_PRINT("%-10x%-15s%-10u%-10u%*c%-10s", opCode, HcsOpcodeToStr(current->objectBase.type), size,
        current->objectBase.subSize, printTab, ' ', current->configNode.name);

    DumpParserObjectValue(current);
    HCS_PRINT("\n");
    return NOERR;
}

static void DumpFinalAstWithOpCode()
{
    HCS_PRINT("\nDump final AST with OpCode\n");
    HCS_PRINT("%-10s%-15s%-10s%-10s%*c%-20s\n", "OpCode", "OpCodeName", "Size", "SubSize", 1, ' ', "ObjectName");
    HcsWalkAst(NULL, AST_WALK_FORWARD, OpCodeDumpPrint, NULL);
}

static int32_t ByteCodeWriteWalk(ParserObject *current, int32_t walkDepth)
{
    (void)walkDepth;
    current->objectBase.hash = HcsGetOutputCurrentCount();
    if (HcsOutputWriteAlign(&current->objectBase.opCode, sizeof(current->objectBase.opCode))) {
        return EOUTPUT;
    }
    int32_t ret = NOERR;
    switch (current->objectBase.opCode) {
        case HCS_BYTE_OP: /* fall-through */
        case HCS_WORD_OP: /* fall-through */
        case HCS_DWORD_OP: /* fall-through */
        case HCS_QWORD_OP: {
            const OpCodeMapEntry *byteCodeMap = HcsGetOpCodeMap();
            ret = HcsOutputWriteAlign(&current->objectBase.integerValue, byteCodeMap[current->objectBase.type].size);
            break;
        }
        case HCS_STRING_OP:
            ret = HcsOutputWriteAlign(current->objectBase.stringValue, strlen(current->objectBase.stringValue) + 1);
            break;
        case HCS_TERM_OP:
            ret = HcsOutputWriteAlign((void *)current->objectBase.name, strlen(current->objectBase.name) + 1);
            break;
        case HCS_NODE_OP:
            if (HcsOutputWriteAlign((void *)current->objectBase.name, strlen(current->objectBase.name) + 1)) {
                return EOUTPUT;
            }
            ret = HcsOutputWriteAlign(&current->objectBase.subSize, sizeof(current->objectBase.subSize));
            break;
        case HCS_ARRAY_OP: {
            uint16_t size = HcsCountArraySize(current);
            ret = HcsOutputWriteAlign(&size, sizeof(size));
        }
            break;
        case HCS_NODEREF_OP: {
            ParserObject *ref = (ParserObject *)current->objectBase.value;
            uint32_t hashCode = ref->objectBase.hash;
            ret = HcsOutputWriteAlign(&hashCode, sizeof(hashCode));
        }
            break;
        default:
            break;
    }

    return ret;
}

int32_t HcsBytecodeOutput(void)
{
    ParserObject *astRoot = HcsGetParserRoot();
    if (astRoot == NULL) {
        return EFAIL;
    }

    /* generate OpCode for every object on AST and calculate size for each object */
    int32_t ret = HcsWalkAst(astRoot, AST_WALK_BACKEND, NULL, ByteCodeConvert);
    if (ret) {
        return ret;
    }

    /* ast data is ready, do bindary output */
    struct HcsFile *outputFIle = HcsOpenOutputFile(HCS_OUTPUT_FILE_SUFFIX);

    HbcHeader header = {
        .magicNumber = HBC_MAGIC_NUM,
        .checkSum = 0,
        .totalSize = HcsOptShouldAlign() ? -astRoot->objectBase.size : astRoot->objectBase.size,
        .versionMajor = HCS_COMPILER_VERSION_MAJOR,
        .versionMinor = HCS_COMPILER_VERSION_MINOR,
    };

    HcsMockOutPut(true);
    /* Generate hashcode for each object */
    if (HcsOutputWriteAlign(&header, sizeof(header))) {
        HcsCloseOutput(outputFIle);
        return EOUTPUT;
    }
    ret = HcsWalkAst(astRoot, AST_WALK_FORWARD, ByteCodeWriteWalk, NULL);
    if (ret) {
        HcsCloseOutput(outputFIle);
        return ret;
    }
    HcsResetOutputCurrentCount();
    HcsMockOutPut(false);

    if (HcsVerbosePrint()) {
        DumpFinalAstWithOpCode();
    }

    /* write bytecode to output file */
    if (HcsOutputWriteAlign(&header, sizeof(header))) {
        HcsCloseOutput(outputFIle);
        return EOUTPUT;
    }
    ret = HcsWalkAst(astRoot, AST_WALK_FORWARD, ByteCodeWriteWalk, NULL);

    /* verify file size */
    uint64_t fileSize = HcsSourceFileGetSize(outputFIle);
    if (fileSize != (sizeof(header) + abs(header.totalSize))) {
        HCS_ERROR("output file size mismatch");
        ret = EOUTPUT;
    }

    HcsCloseOutput(outputFIle);
    HCS_DEBUG("Total size: %u ", astRoot->objectBase.size);
    if (ret == NOERR && HcsOptShouldGenHexdump()) {
        ret = HcsBinaryToHexdump(HcsGetOutPutFilePath());
    }
    return ret;
}
