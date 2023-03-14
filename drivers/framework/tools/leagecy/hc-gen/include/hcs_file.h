/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HCS_COMPILER_FILE_H
#define HCS_COMPILER_FILE_H

#include <stdio.h>
#include "hcs_types.h"

#define HCS_SOURCE_FILE_SUFFIX ".hcs"
#define HCS_OUTPUT_FILE_SUFFIX ".hcb"
#define HCS_DECOMPILE_OUTPUT_FILE_SUFFIX ".d.hcs"

struct HcsFile {
    const char *name;
    const char *fullPath;
    FILE *file;
    uint64_t pos;
    struct HcsFile *next;
};

struct HcsFileQueue {
    struct HcsFile *head;
    uint32_t count;
};

struct HcsSourceName {
    const char *name;
    struct HcsSourceName *next;
};

const char *HcsGetInputFileName(void);

void HcsSetInputFileName(const char *name);

int32_t HcsOpenSourceFile(const char *path, struct HcsFile **file, const char *flag);

void HcsCloseFile(struct HcsFile *file);

int32_t HcsSetOutPutName(const char *name);

int32_t HcsSetOutPutNameWithCurrentWorkPath(const char *name);

const char *HcsGetOutPutFilePath(void);

char *HcsGetOutputFileNameWithoutSuffix(void);

const char *HcsGetOutputFileName(void);

struct HcsFile *HcsOpenOutputFile(const char *suffix);

void HcsCloseOutput(struct HcsFile *output);

int32_t HcsOutputWrite(const void *buffer, uint32_t length);

int32_t HcsFormatOutputWrite(const char *format, ...);

int32_t HcsOutputWriteAlign(const void *buffer, uint32_t length);

void HcsMockOutPut(bool dummyOutput);

uint32_t HcsGetOutputCurrentCount(void);

void HcsResetOutputCurrentCount(void);

void HcsSourceQueuePush(struct HcsFile *sourceFile);

void HcsSourceQueuePop(void);

struct HcsFile *HcsSourceQueueTop(void);

uint32_t HcsSourceQueueSize(void);

const char *HcsGetCurrentSourceName(void);

int32_t HcsSourceNameSetPush(const char *name);

void HcsSourceNameSetClean(void);

bool HcsFileCopyDir(char *dst, uint32_t dstBufferSize, const char *fullPath);

uint64_t HcsSourceFileGetSize(struct HcsFile *file);

int32_t HcsSourceFileRead(struct HcsFile *file, uint8_t *out, uint32_t readSize);

uint32_t HcsGetSourceFilePos(struct HcsFile *file);

#endif // HCS_COMPILER_FILE_H
