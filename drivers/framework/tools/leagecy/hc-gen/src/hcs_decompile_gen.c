/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <string.h>
#include <securec.h>
#include "hcs_file.h"
#include "hcs_ast.h"
#include "hcs_mem.h"
#include "hcs_log.h"

#define WRITE_BUFFER_LEN 256
#define HCS_TAB_SIZE 4

#define HCS_DECOMPILE_FILE_HEADER           \
    "/*\n"                                  \
    " * HDF decompile hcs file\n"           \
    " */\n\n"

static char *HcsAssembleNodeRefName(char *buff, uint32_t buffSize, const char *name)
{
    int32_t res = strcat_s(buff, buffSize, name);
    if (res) {
        HCS_ERROR("%s:%d: string cat fail", __func__, __LINE__);
        return NULL;
    }
    char *str = strdup(buff);
    if (str == NULL) {
        HCS_ERROR("%s:%d OOM", __func__, __LINE__);
        return NULL;
    }
    return str;
}

static char *HcsGetNodeRefPath(uint64_t hash)
{
    char pathBuffer[WRITE_BUFFER_LEN] = {'\0'};
    char *path = pathBuffer;
    ParserObjectBase *it = (ParserObjectBase *)HcsGetParserRoot();
    if (it == NULL) {
        return NULL;
    }

    while (it->child) {
        int32_t res = strcat_s(path, WRITE_BUFFER_LEN, it->name);
        if (res) {
            HCS_ERROR("%s:%d: strcat fail", __func__, __LINE__);
            return NULL;
        }
        res = strcat_s(path, WRITE_BUFFER_LEN, ".");
        if (res) {
            HCS_ERROR("%s:%d: strcat fail", __func__, __LINE__);
            return NULL;
        }
        ParserObjectBase *child = it->child;
        bool deepIn = false;
        while (child != NULL) {
            if (child->type != PARSEROP_CONFNODE) {
                child = child->next;
                continue;
            }
            if (child->hash == hash) {
                return HcsAssembleNodeRefName(pathBuffer, WRITE_BUFFER_LEN, child->name);
            }

            if (hash > child->hash && hash < (child->hash + child->size)) {
                it = child;
                deepIn = true;
                break;
            }
            child = child->next;
        }
        if (!deepIn) {
            HCS_ERROR("ref unknown node, hash = %"PRIu64, hash);
            return NULL;
        }
    }

    return NULL;
}

static int32_t HcsDecompilePrintBaseType(const ParserObject *object);

static int32_t HcsDecompilePrintArrayType(const ParserObject *object)
{
    int32_t res;
    PRINTF_CHECK_AND_RETURN(HcsFormatOutputWrite("["));
    ParserObject *arrayElement = (ParserObject *)object->objectBase.child;
    while (arrayElement->objectBase.next) {
        res = HcsDecompilePrintBaseType(arrayElement);
        if (res) {
            return EOUTPUT;
        }
        PRINTF_CHECK_AND_RETURN(HcsFormatOutputWrite(", "));
        arrayElement = (ParserObject *)arrayElement->objectBase.next;
    }
    res = HcsDecompilePrintBaseType(arrayElement);
    if (res) {
        return EOUTPUT;
    }

    return HcsFormatOutputWrite("]");
}

static int32_t HcsDecompilePrintBaseType(const ParserObject *object)
{
    int32_t res = NOERR;
    switch (object->objectBase.type) {
        case PARSEROP_UINT8:
        case PARSEROP_UINT16:
        case PARSEROP_UINT32:
        case PARSEROP_UINT64:
            res = HcsFormatOutputWrite("0x%"PRIx64, object->objectBase.integerValue);
            break;
        case PARSEROP_STRING:
            res = HcsFormatOutputWrite("\"%s\"", object->objectBase.stringValue);
            break;
        case PARSEROP_NODEREF: {
            char *refPath = HcsGetNodeRefPath(object->objectBase.value);
            if (refPath == NULL) {
                return EOUTPUT;
            }
            res = HcsFormatOutputWrite("&%s", refPath);
            HcsMemFree(refPath);
        }
            break;
        case PARSEROP_ARRAY:
            return HcsDecompilePrintArrayType(object);
        default:
            HCS_ERROR("unknown OpCode %u", object->objectBase.type);
            return EFAIL;
    }

    return res;
}

static int32_t HcsDecompileOutputWalk(ParserObject *current, int32_t walkDepth)
{
    if (current->objectBase.type != PARSEROP_CONFNODE && current->objectBase.type != PARSEROP_CONFTERM) {
        return NOERR;
    }

    int32_t res;
    int32_t tabSize = walkDepth * HCS_TAB_SIZE;
    if (walkDepth) {
        PRINTF_CHECK_AND_RETURN(HcsFormatOutputWrite("%*c", tabSize, ' '));
    }

    switch (current->objectBase.type) {
        case PARSEROP_CONFNODE:
            OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite("%s {\n", current->configNode.name));
            if (current->objectBase.child == NULL) {
                return HcsFormatOutputWrite("%*c}\n", tabSize, ' ');
            }
            break;
        case PARSEROP_CONFTERM:
            OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite("%s = ", current->configNode.name));
            res = HcsDecompilePrintBaseType(
                (ParserObject *) current->configNode.child);
            if (res) {
                return res;
            }
            return HcsFormatOutputWrite(";\n");
        default:
            break;
    }

    return NOERR;
}

static int32_t HcsDecompileCloseBraceGen(ParserObject *current, int32_t walkDepth)
{
    if (current->objectBase.type != PARSEROP_CONFNODE) {
        return NOERR;
    }
    int32_t tabSize = walkDepth * HCS_TAB_SIZE;

    return current != HcsGetParserRoot() ? HcsFormatOutputWrite("%*c}\n", tabSize, ' ') :
        HcsFormatOutputWrite("}\n");
}

int32_t HcsDecompileOutput(void)
{
    ParserObject *astRoot = HcsGetParserRoot();
    if (astRoot == NULL) {
        return EOUTPUT;
    }

    int32_t ret = EOUTPUT;
    struct HcsFile *outputFIle = HcsOpenOutputFile(HCS_DECOMPILE_OUTPUT_FILE_SUFFIX);
    if (outputFIle == NULL) {
        goto OUT;
    }

    if (HcsFormatOutputWrite(HCS_DECOMPILE_FILE_HEADER) != EOK) {
        goto OUT;
    }

    ret = HcsWalkAst(astRoot, AST_WALK_ROUND, HcsDecompileOutputWalk, HcsDecompileCloseBraceGen);

OUT:
    HcsCloseOutput(outputFIle);
    return ret;
}