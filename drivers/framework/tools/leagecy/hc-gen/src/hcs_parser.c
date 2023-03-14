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
#include "hcs_compiler.h"
#include "hcs_gener.h"
#include "hcs_ast.h"
#include "hcs_option.h"

const char *g_nodeTypeStringMap[CONFIG_NODE_TYPE_COUNT] = {
    [CONFIG_NODE_NOREF] = "NodeReference",
    [CONFIG_NODE_COPY] = "NodeCopy",
    [CONFIG_NODE_REF] = "NodeReference",
    [CONFIG_NODE_DELETE] = "NodeDelete",
    [CONFIG_NODE_INHERIT] = "NodeInherit",
    [CONFIG_NODE_TEMPLATE] = "NodeTemplate",
};

uint32_t HcsAlign(uint32_t size)
{
    if (HcsOptShouldAlign()) {
        return  (size + ALIGN_SIZE - 1) & (~(ALIGN_SIZE - 1));
    } else {
        return size;
    }
}

const char *HcsParserNodeTypeToStr(uint32_t nodeType)
{
    if (nodeType >= CONFIG_NODE_TYPE_COUNT) {
        return "";
    }

    return g_nodeTypeStringMap[nodeType];
}

bool HcsIsNumberObject(const ParserObject *obj)
{
    return obj->objectBase.type >= PARSEROP_UINT8 && obj->objectBase.type <= PARSEROP_UINT64;
}

bool HcsIsStringObject(const ParserObject *obj)
{
    return obj->objectBase.type == PARSEROP_STRING;
}

bool HcsIsSameTypeObject(const ParserObject *cs, const ParserObject *ct)
{
    if (HcsIsNumberObject(cs)) {
        return HcsIsNumberObject(ct);
    } else if (cs->objectBase.type == PARSEROP_CONFTERM || cs->objectBase.type == PARSEROP_ARRAY) {
        return ct->objectBase.type == cs->objectBase.type && cs->objectBase.child->type == ct->objectBase.child->type;
    } else {
        return ct->objectBase.type == cs->objectBase.type;
    }
}

void HcsSetCompilerIn(FILE *in);

static void InitParser(const struct HcsFile *sourceFile)
{
    HcsSetCompilerIn(sourceFile->file);
    HcsParserRestart(sourceFile->file);
    HcsSetCurrentSourceLine(1);
}

void ParserCleanUp()
{
    ParserObject *astRoot = HcsGetParserRoot();
    HcsDeleteParserObjectTree(astRoot);
    HcsSourceNameSetClean();
}

int32_t HcsProcessInclude(char *includePath, uint32_t lineNumber)
{
    if (includePath[0] == '/' || !strstr(includePath, HCS_SOURCE_FILE_SUFFIX)) {
        HCS_ERROR("File:%s Line:%u\n\tinclude file %s is invalid", HcsGetCurrentSourceName(), lineNumber, includePath);
        return EFAIL;
    }

    struct HcsFile *currentSource = HcsSourceQueueTop();
    /* assembly include path to current source file */
    uint32_t pathSize = strlen(currentSource->fullPath) + strlen(includePath) + 1;
    char *path = HcsMemZalloc(pathSize);
    if (path == NULL) {
        HCS_ERROR("oom");
        return EOOM;
    }

    if (HcsFileCopyDir(path, pathSize, currentSource->fullPath) != true) {
        HcsMemFree(path);
        return EFAIL;
    }

    int32_t res = strcat_s(path, pathSize, includePath);
    if (res != EOK) {
        HcsMemFree(path);
        HCS_ERROR("%s:string cat fail", __func__);
        return EFAIL;
    }
    HcsMemFree(includePath);

    struct HcsFile *source = NULL;
    uint32_t ret = HcsOpenSourceFile(path, &source, NULL);
    if (ret == EREOPENF) {
        HcsMemFree(path);
        return NOERR;
    }

    if (ret) {
        HCS_ERROR("File:%s Line:%u\n\tinclude file %s is invalid", HcsGetCurrentSourceName(), lineNumber, path);
        HcsMemFree(path);
        return EINVALF;
    }

    HcsSourceQueuePush(source);
    HcsMemFree(path);
    return NOERR;
}

int32_t HcsDoCompile(void)
{
    struct HcsFile *source = NULL;
    char *forestName = strdup("ForestRoot");
    if (forestName == NULL) {
        return EOOM;
    }
    ParserObject *astForest = HcsNewParserObject(forestName, PARSEROP_CONFNODE, 0);
    if (astForest == NULL) {
        HcsMemFree(forestName);
        return EFAIL;
    }

    uint32_t ret = HcsOpenSourceFile(HcsGetInputFileName(), &source, NULL);
    if (ret) {
        HCS_ERROR("Invalid source file: %s", HcsGetInputFileName());
        HcsAstFreeObject(astForest);
        return ret;
    }
    HcsSourceQueuePush(source);

    /* parse specified source file and included files */
    while (HcsSourceQueueSize()) {
        source = HcsSourceQueueTop();
        HCS_DEBUG("parsing file : %s%s", source->fullPath, source->name);
        InitParser(source);

        /* do parse */
        ret = HcsCompilerparse();
        ParserObject *currentRoot = HcsGetParserRoot();
        /* pop current file, parse next file which maybe imported by include */
        HcsSourceQueuePop();
        HcsCloseFile(source);
        if (ret) {
            goto OUT;
        }

        HcsAstAddChild(astForest, currentRoot);
        HcsSetParserRoot(NULL);
    }
    HcsSetParserRoot(astForest);

    if (HcsVerbosePrint()) {
        HcsDumpAst("Origin");
    }

    /* middle process */
    ret = HcsDoOptimize();
    if (ret) {
        goto OUT;
    }

    /* output */
    if (HcsOptShouldGenTextConfig()) {
        ret = HcsTextCodeOutput();
    } else if (HcsOptShouldGenByteCodeConfig()) {
        ret = HcsBytecodeOutput();
    }
OUT:
    ParserCleanUp();
    if (HcsGetParserRoot() != astForest) {
        HcsDeleteParserObjectTree(astForest);
    }
    return ret;
}
