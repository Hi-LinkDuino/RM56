/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hc_file_f_test.h"

#include <errno.h>
#include <fcntl.h>
#include <hctest.h>
#include <securec.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "hc_file_common.h"
#include "print_log.h"
#include "test_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#if F_API_IMPLEMENTED

static void ReadFile(char *buffer, FILE *file, int size)
{
    int total = 0;
    while (total < size) {
        int readCount = fread(buffer + total, 1, size - total, file);
        TEST_ASSERT_GREATER_OR_EQUAL(0, readCount);
        TEST_ASSERT_LESS_OR_EQUAL(size - total, readCount);
        total += readCount;
        if (readCount <= 0) {
            break;
        }
    }
}

static void WriteFile(const char *buffer, FILE *file, int size)
{
    int total = 0;
    while (total < size) {
        int writeCount = fwrite(buffer + total, 1, size - total, file);
        TEST_ASSERT_GREATER_OR_EQUAL(0, writeCount);
        TEST_ASSERT_LESS_OR_EQUAL(size - total, writeCount);
        total += writeCount;
        if (writeCount <= 0) {
            break;
        }
    }
}

static void TestHcFileFopenAndFclose(void)
{
    const char *fileName = TEST_FILE_NAME;
    LOGI("begin to open file: %s", fileName);

    FILE *file;
    RUN_AND_PRINT_ELAPSED_TIME(file, fopen(fileName, "w+"));
    TEST_ASSERT_NOT_NULL(file);

    int ret;
    LOGI("begin to close file: %s", fileName);
    RUN_AND_PRINT_ELAPSED_TIME(ret, fclose(file));
    TEST_ASSERT_EQUAL(0, ret);
}

static void TestHcFileFreadAndFwrite(void)
{
    const char *fileName = TEST_FILE_NAME;
    for (int i = 0; i < TEST_FILE_SIZE_LIST_LEN; i++) {
        FILE *file = fopen(fileName, "w+");
        TEST_ASSERT_NOT_NULL(file);
        int size = testFileSizeList[i];

        LOGI("begin to write file [%s] for size: %d", fileName, size);
        char *writeBuffer = GenerateTestingText(size);
        TEST_ASSERT_NOT_NULL(writeBuffer);
        RUN_AND_PRINT_ELAPSED_TIME_WITHOUT_RESULT(WriteFile(writeBuffer, file, size));

        TEST_ASSERT_EQUAL(0, fseek(file, 0, SEEK_END));
        const int fileSize = ftell(file);
        TEST_ASSERT_EQUAL(size, fileSize);
        TEST_ASSERT_EQUAL(0, fseek(file, 0, SEEK_SET));

        LOGI("begin to read file [%s] for size: %d", fileName, fileSize);
        TEST_ASSERT_LESS_OR_EQUAL(TEN_KILOBYTE, fileSize);
        char *readBuffer = (char *)malloc(fileSize + 1);
        TEST_ASSERT_NOT_NULL(readBuffer);
        TEST_ASSERT_EQUAL(0, memset_s(readBuffer, fileSize + 1, 0, fileSize + 1));
        RUN_AND_PRINT_ELAPSED_TIME_WITHOUT_RESULT(ReadFile(readBuffer, file, fileSize));
        TEST_ASSERT_EQUAL_STRING(writeBuffer, readBuffer);

        free(readBuffer);
        free(writeBuffer);
        TEST_ASSERT_EQUAL(0, fclose(file));
        sleep(1);
    }
}

static void TestHcFileFseekAndFtell(void)
{
    const char *fileName = TEST_FILE_NAME;
    LOGI("begin to count file size");

    FILE *file = fopen(fileName, "rb");
    TEST_ASSERT_NOT_NULL(file);

    int ret;
    RUN_AND_PRINT_ELAPSED_TIME(ret, fseek(file, 0, SEEK_END));
    TEST_ASSERT_EQUAL(0, ret);

    int size;
    RUN_AND_PRINT_ELAPSED_TIME(size, ftell(file));
    TEST_ASSERT_EQUAL(TEN_KILOBYTE, size);
    TEST_ASSERT_EQUAL(0, fseek(file, 0, SEEK_SET));
    LOGI("the file size is: %d", size);

    TEST_ASSERT_EQUAL(0, fclose(file));

    RUN_AND_PRINT_ELAPSED_TIME(ret, unlink(fileName));
    TEST_ASSERT_EQUAL(0, ret);
}

static void TestHcFileMkdir(void)
{
#if MKDIR_IMPLEMENTED
    const char *dir = TEST_FILE_DIR;
    LOGI("begin to make directory: %s", dir);
    int ret;
    RUN_AND_PRINT_ELAPSED_TIME(ret, mkdir(dir, DEFAULT_FILE_PERMISSION));
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(-1, mkdir(dir, DEFAULT_FILE_PERMISSION));
    TEST_ASSERT_EQUAL(0, rmdir(dir));
#endif
}

static void TestHcFileStat(void)
{
#if STAT_IMPLEMENTED
    const char *fileName = TEST_FILE_NAME;
    LOGI("begin to check [%s] state", fileName);

    FILE *file = fopen(fileName, "w+");
    TEST_ASSERT_NOT_NULL(file);
    TEST_ASSERT_EQUAL(0, fclose(file));

    struct stat fileStat;
    int ret;
    ret = memset_s(&fileStat, sizeof(fileStat), 0, sizeof(fileStat));
    TEST_ASSERT_EQUAL(0, ret);
    RUN_AND_PRINT_ELAPSED_TIME(ret, stat(fileName, &fileStat));
    TEST_ASSERT_EQUAL(0, ret);

    TEST_ASSERT_EQUAL(0, unlink(fileName));
    ret = memset_s(&fileStat, sizeof(fileStat), 0, sizeof(fileStat));
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(-1, stat(fileName, &fileStat));
    TEST_ASSERT_EQUAL(ENOENT, errno);
#endif
}

static void TestHcFileRemove(void)
{
    const char *fileName = TEST_FILE_NAME;
    LOGI("begin to remove file: %s", fileName);

    int ret;
    RUN_AND_PRINT_ELAPSED_TIME(ret, unlink(fileName));
    TEST_ASSERT_EQUAL(0, ret);
}

static void TestHcFileAccess(void)
{
#if ACCESS_IMPLEMENTED
    const char *fileName = TEST_FILE_NAME;
    LOGI("begin to check access of [%s]", fileName);

    FILE *file = fopen(fileName, "w+");
    TEST_ASSERT_NOT_NULL(file);
    TEST_ASSERT_EQUAL(0, fclose(file));

    int ret;
    RUN_AND_PRINT_ELAPSED_TIME(ret, access(fileName, F_OK));
    TEST_ASSERT_EQUAL(0, ret);

    TEST_ASSERT_EQUAL(0, unlink(fileName));
    TEST_ASSERT_EQUAL(-1, access(fileName, F_OK));
#endif
}

void TestHcFileFApi(void)
{
    LOGI("test opening and closing file");
    TestHcFileFopenAndFclose();

    LOGI("test removing file");
    TestHcFileRemove();

    LOGI("test reading and writing file");
    TestHcFileFreadAndFwrite();

    LOGI("test counting file size");
    TestHcFileFseekAndFtell();

    LOGI("test making directory");
    TestHcFileMkdir();
    
    LOGI("test getting file state");
    TestHcFileStat();

    LOGI("test accessing a file");
    TestHcFileAccess();
}

#else // F_API_IMPLEMENTED
void TestHcFileFApi(void)
{
    LOGE("no F_API_IMPLEMENTED, do not test fopen series!");
}
#endif // F_API_IMPLEMENTED

#ifdef __cplusplus
}
#endif
