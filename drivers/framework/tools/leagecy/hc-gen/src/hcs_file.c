/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hcs_file.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <securec.h>
#include "hcs_parser.h"
#include "hcs_mem.h"
#include "hcs_log.h"

static char *g_outputFileName = NULL;
static FILE *g_outputFile = NULL;
static const char *g_inputFileName = NULL;
static bool g_dummyOutput = false;
static uint32_t g_outputWriteCount = 0;
static struct HcsFileQueue g_inputSourceFileQueue = { 0 };
static struct HcsSourceName *g_sourceNameSetHead = NULL;

void HcsSetInputFileName(const char *name)
{
    g_inputFileName = name;
}

const char *HcsGetInputFileName(void)
{
    return g_inputFileName;
}

int32_t HcsSourceNameSetPush(const char *name)
{
    if (g_sourceNameSetHead == NULL) {
        struct HcsSourceName *head = HcsMemZalloc(sizeof(*head));
        if (head == NULL) {
            HCS_ERROR("%s:%d OOM", __func__, __LINE__);
            return EOOM;
        }
        head->name = name;
        g_sourceNameSetHead = head;
        return NOERR;
    }

    struct HcsSourceName *pre = g_sourceNameSetHead;
    struct HcsSourceName *last = pre;
    while (last != NULL) {
        if (!strcmp(last->name, name)) {
            return EREOPENF;
        }
        pre = last;
        last = last->next;
    }

    struct HcsSourceName *newName = HcsMemZalloc(sizeof(struct HcsSourceName));
    if (newName == NULL) {
        HCS_ERROR("%s:%d OOM", __func__, __LINE__);
        return EOOM;
    }
    newName->name = name;
    pre->next = newName;
    return NOERR;
}

bool HcsSourceNameSetFind(const char *name)
{
    struct HcsSourceName *last = g_sourceNameSetHead;

    while (last != NULL) {
        if (!strcmp(last->name, name)) {
            return true;
        }
        last = last->next;
    }

    return false;
}

void HcsSourceNameSetClean(void)
{
    struct HcsSourceName *term = g_sourceNameSetHead;

    while (term != NULL) {
        struct HcsSourceName *temp = term->next;
        HcsMemFree((void *)term->name);
        HcsMemFree(term);
        term = temp;
    }

    g_sourceNameSetHead = NULL;
}

static const char *GetFileName(const char *path)
{
    if (path == NULL) {
        return NULL;
    }
    int32_t length = (int32_t)strlen(path);
    int32_t i = length - 1;
    for (; i >= 0; --i) {
        if (path[i] == OS_SEPARATOR) {
            break;
        }
    }

    return i >= 0 ? path + i + 1 : path;
}

static const char *CopyFileName(const char *path)
{
    return strdup(GetFileName(path));
}

static const char *GetFileNameWithoutSuffix(const char *path)
{
    char *fileNameRet = strdup(path);
    if (fileNameRet == NULL) {
        HCS_ERROR("oom");
        return NULL;
    }
    char *it = fileNameRet + strlen(fileNameRet);
    while (it != fileNameRet) {
        if (*it == '.') {
            *it = '\0';
            break;
        }
        if (*it == OS_SEPARATOR) {
            break;
        }
        it--;
    }

    return fileNameRet;
}

static int32_t CopyAndSaveFileName(const char *filePath, char **savedFileName, char **savedFilePath)
{
    const char *fileName = CopyFileName(filePath);
    if (fileName == NULL) {
        return EOOM;
    }

    /* if no specified output file name, use input name */
    if (HcsGetOutPutFilePath() == NULL && HcsSetOutPutNameWithCurrentWorkPath(fileName)) {
        HcsMemFree((void *)fileName);
        return EOOM;
    }

    char *path = strdup(filePath);
    if (path == NULL || HcsSourceNameSetPush(path)) {
        HcsMemFree((void *)fileName);
        HcsMemFree((void *)path);
        return EOOM;
    }
    *savedFileName = (char *)fileName;
    *savedFilePath = path;
    return NOERR;
}

int32_t HcsOpenSourceFile(const char *path, struct HcsFile **file, const char *flag)
{
    char pathBuf[PATH_MAX] = {'\0'};
#ifdef MINGW32
    char *realPath = _fullpath(pathBuf, path, PATH_MAX);
#else
    char *realPath = realpath(path, pathBuf);
#endif
    if (realPath == NULL) {
        HCS_ERROR("failed to open source file: %s", path);
        return EINVALF;
    }

    /* push to name set and check reopen */
    if (HcsSourceNameSetFind(realPath)) {
        return EREOPENF;
    }

    HCS_DEBUG("source file path: %s", realPath);
    FILE *f = fopen(realPath, flag ? flag : "r");
    if (f == NULL) {
        HCS_ERROR("failed to open source file: %s", realPath);
        return EINVALF;
    }

    char *fileName = NULL;
    char *filePath = NULL;
    int32_t ret = CopyAndSaveFileName(realPath, &fileName, &filePath);
    if (ret) {
        fclose(f);
        return EFAIL;
    }

    struct HcsFile *sourceFile = HcsMemZalloc(sizeof(struct HcsFile));
    if (sourceFile == NULL) {
        HcsMemFree(fileName);
        HcsMemFree(filePath);
        fclose(f);
        HCS_ERROR("oom");
        return EOOM;
    }
    sourceFile->name = fileName;
    sourceFile->fullPath = filePath;
    sourceFile->file = f;
    sourceFile->pos = 0;
    *file = sourceFile;
    return NOERR;
}

uint64_t HcsSourceFileGetSize(struct HcsFile *file)
{
    uint64_t currentPos = ftell(file->file);

    fseek(file->file, 0, SEEK_END);
    uint64_t fileSize = ftell(file->file);
    fseek(file->file, currentPos, SEEK_SET);

    return fileSize;
}

int32_t HcsSourceFileRead(struct HcsFile *file, uint8_t *out, uint32_t readSize)
{
    struct HcsFile *currentSrc = file ? file : HcsSourceQueueTop();
    if (currentSrc == NULL) {
        HCS_ERROR("source file not valid");
        return -EINVALF;
    }

    return fread(out, 1, readSize, currentSrc->file);
}

uint32_t HcsGetSourceFilePos(struct HcsFile *file)
{
    struct HcsFile *currentSrc = file ? file : HcsSourceQueueTop();
    return ftell(currentSrc->file);
}

void HcsCloseFile(struct HcsFile *file)
{
    if (file == NULL) {
        return;
    }

    fclose(file->file);
    HcsMemFree((void *)file->name);
    HcsMemFree(file);
}

int32_t HcsSetOutPutNameWithCurrentWorkPath(const char *name)
{
    char outPutPath[PATH_MAX] = {0};
    char *cwd = getcwd(outPutPath, PATH_MAX);
    if (cwd == NULL || !strlen(cwd)) {
        return EFAIL;
    }

    if (sprintf_s(outPutPath + strlen(outPutPath), PATH_MAX - strlen(outPutPath), "%c", OS_SEPARATOR) < 0) {
        return EFAIL;
    }

    if (strcat_s(outPutPath, PATH_MAX, name) != EOK) {
        return EFAIL;
    }

    return HcsSetOutPutName(outPutPath);
}

int32_t HcsSetOutPutName(const char *name)
{
    if (g_outputFileName != NULL) {
        HcsMemFree(g_outputFileName);
    }

    g_outputFileName = strdup(name);
    if (g_outputFileName == NULL) {
        HCS_ERROR("oom");
        return EOOM;
    }
#ifdef OS_WIN
    char *temp = g_outputFileName;
    while (*temp != '\0') {
        if (*temp == UNIX_SEPARATOR) {
            *temp = WIN_SEPARATOR;
        }
        temp++;
    }
#endif

    return NOERR;
}

bool HcsSetOutputFileSuffix(const char *suffix)
{
    const char *fileNameBefore = g_outputFileName;
    const char *fileNameWithoutSuffix = GetFileNameWithoutSuffix(fileNameBefore);
    if (fileNameWithoutSuffix == NULL) {
        return false;
    }

    uint32_t newNameSize = strlen(g_outputFileName) + strlen(suffix) + 1;
    char *newOutputFilename = HcsMemZalloc(newNameSize);
    if (newOutputFilename == NULL) {
        HcsMemFree((void *)fileNameWithoutSuffix);
        return false;
    }

    int32_t ret = strcpy_s(newOutputFilename, newNameSize, fileNameWithoutSuffix);
    HcsMemFree((void *)fileNameWithoutSuffix);
    if (ret) {
        HCS_ERROR("failed to copy string");
        HcsMemFree(newOutputFilename);
        return false;
    }
    ret = strcat_s(newOutputFilename, newNameSize, suffix);
    if (ret) {
        HCS_ERROR("failed to copy string");
        HcsMemFree(newOutputFilename);
        return false;
    }

    HcsMemFree((void *)fileNameBefore);
    g_outputFileName = newOutputFilename;
    return true;
}

// return output file name only
const char *HcsGetOutputFileName(void)
{
    return GetFileName(g_outputFileName);
}

char *HcsGetOutputFileNameWithoutSuffix(void)
{
    const char *fileName = HcsGetOutputFileName();
    if (fileName == NULL) {
        return NULL;
    }

    char *retName = strdup(fileName);
    if (retName == NULL) {
        HCS_ERROR("oom");
        return NULL;
    }

    char *dot = strchr(retName, '.');
    if (dot != NULL) {
        *dot = '\0';
    }
    return retName;
}


// return full path of output file
const char *HcsGetOutPutFilePath(void)
{
    return g_outputFileName;
}

bool HcsOutputNameVerify()
{
    const char *fileName = g_outputFileName;
    char lastChar = fileName[strlen(fileName) - 1];
    if (lastChar == OS_SEPARATOR) {
        HCS_ERROR("output name is DIR");
        return false;
    }

    return true;
}

struct HcsFile *HcsOpenOutputFile(const char *suffix)
{
    if (!HcsOutputNameVerify()) {
        return NULL;
    }
    if (!HcsSetOutputFileSuffix(suffix)) {
        return NULL;
    }
    const char *outputFileName = HcsGetOutPutFilePath();
    g_outputFile = fopen(outputFileName, "wb");
    if (g_outputFile == NULL) {
        HCS_ERROR("failed to open output file: %s", outputFileName);
        return NULL;
    }

    HCS_DEBUG("Output: %s", outputFileName);
    struct HcsFile *outputFile = HcsMemZalloc(sizeof(struct HcsFile));
    if (outputFile == NULL) {
        HCS_ERROR("oom");
        fclose(g_outputFile);
        g_outputFile = NULL;
        return NULL;
    }
    outputFile->name = outputFileName;
    outputFile->file = g_outputFile;
    outputFile->pos = 0;
    outputFile->fullPath = outputFileName;
    g_outputWriteCount = 0;

    return outputFile;
}

void HcsCloseOutput(struct HcsFile *output)
{
    if (output == NULL || output->file == NULL) {
        return;
    }

    fflush(output->file);
    fclose(output->file);

    HcsMemFree(output);
    g_outputWriteCount = 0;
}

int32_t HcsOutputWrite(const void *buffer, uint32_t length)
{
    g_outputWriteCount += length;
    if (g_dummyOutput || !length) {
        return NOERR;
    }

    uint32_t writeLen = fwrite(buffer, 1, length, g_outputFile);
    if (writeLen != length) {
        HCS_ERROR("failed to write output file");
        return EOUTPUT;
    }

    return NOERR;
}

#define WRITE_MAX_PER_TIME 2048

int32_t HcsFormatOutputWrite(const char *format, ...)
{
    if (format == NULL) {
        return EINVALARG;
    }
    static char writeBuffer[WRITE_MAX_PER_TIME] = {'\0'};

    va_list argList;

    va_start(argList, format);
    int length = vsprintf_s(writeBuffer, WRITE_MAX_PER_TIME, format, argList);
    va_end(argList);
    (void)argList;

    if (length < 0) {
        HCS_ERROR("Output too long in one time");
        return EOUTPUT;
    }
    return HcsOutputWrite(writeBuffer, length);
}

int32_t HcsOutputWriteAlign(const void *buffer, uint32_t length)
{
    static const uint8_t alignData[ALIGN_SIZE]  = {0};
    int32_t ret = HcsOutputWrite(buffer, length);
    if (ret) {
        return ret;
    }
    int32_t appendSize = HcsAlign(length) - length;
    if (appendSize) {
        ret = HcsOutputWrite(alignData, appendSize);
    }
    return ret;
}

void HcsMockOutPut(bool dummyOutput)
{
    g_dummyOutput = dummyOutput;
}

uint32_t HcsGetOutputCurrentCount(void)
{
    return g_outputWriteCount;
}

void HcsResetOutputCurrentCount(void)
{
    g_outputWriteCount = 0;
}

void HcsSourceQueuePush(struct HcsFile *sourceFile)
{
    struct HcsFileQueue *sourceQueue = &g_inputSourceFileQueue;
    struct HcsFile *queueLast = sourceQueue->head;
    if (queueLast == NULL) {
        sourceQueue->head = sourceFile;
        sourceQueue->count = 1;
        return;
    }

    while (queueLast->next) {
        queueLast = queueLast->next;
    }

    queueLast->next = sourceFile;
    sourceQueue->count++;
}

void HcsSourceQueuePop(void)
{
    struct HcsFileQueue *sourceQueue = &g_inputSourceFileQueue;
    if (sourceQueue->head == NULL) {
        return;
    }
    sourceQueue->head = sourceQueue->head->next;
    sourceQueue->count--;
}

struct HcsFile *HcsSourceQueueTop(void)
{
    return g_inputSourceFileQueue.head;
}

uint32_t HcsSourceQueueSize(void)
{
    return g_inputSourceFileQueue.count;
}

const char *HcsGetCurrentSourceName(void)
{
    struct HcsFile *source = HcsSourceQueueTop();
    return source ? source->fullPath : "";
}

bool HcsFileCopyDir(char *dst, uint32_t dstBufferSize, const char *fullPath)
{
    const char *c = strrchr(fullPath, OS_SEPARATOR);
    if (c == NULL) {
        HCS_ERROR("%s: path '%s' not include dir", __func__, fullPath);
        return false;
    }
    int32_t len = (int32_t)(c - fullPath) + 1;
    int32_t ret = strncpy_s(dst, dstBufferSize, fullPath, len);
    if (ret) {
        HCS_ERROR("%s:string copy fail", __func__);
        return false;
    }
    dst[len] = '\0';

    return true;
}