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

#include "hc_file_test.h"

#include <errno.h>
#include <fcntl.h>
#include <hctest.h>
#include <securec.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "hc_file_common.h"
#include "print_log.h"
#include "test_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#if TEST_HC_FILE_OPEN_SERIES

static void ReadFile(char *buffer, int fd, int size)
{
    int total = 0;
    while (total < size) {
        int readCount = read(fd, buffer + total, size - total);
        TEST_ASSERT_GREATER_OR_EQUAL(0, readCount);
        TEST_ASSERT_LESS_OR_EQUAL(size - total, readCount);
        total += readCount;
        if (readCount <= 0) {
            break;
        }
    }
}

static void WriteFile(char *buffer, int fd, int size)
{
    int total = 0;
    while (total < size) {
        int writeCount = write(fd, buffer + total, size - total);
        TEST_ASSERT_GREATER_OR_EQUAL(0, writeCount);
        TEST_ASSERT_LESS_OR_EQUAL(size - total, writeCount);
        total += writeCount;
        if (writeCount <= 0) {
            break;
        }
    }
}

static void TestHcFileOpenAndClose(void)
{
    const char *file = TEST_FILE_NAME;
    LOGI("begin to open file: %s", file);

    int fd;
    RUN_AND_PRINT_ELAPSED_TIME(fd, open(file, O_RDWR | O_CREAT));
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    int ret;
    LOGI("begin to close file: %s", file);
    RUN_AND_PRINT_ELAPSED_TIME(ret, close(fd));
    TEST_ASSERT_EQUAL(0, ret);
}

static void TestHcFileReadAndWrite(void)
{
    const char *file = TEST_FILE_NAME;
    for (int i = 0; i < TEST_FILE_SIZE_LIST_LEN; i++) {
        int fd = open(file, O_RDWR | O_CREAT);
        TEST_ASSERT_GREATER_OR_EQUAL(0, fd);
        int size = testFileSizeList[i];

        LOGI("begin to write file [%s] for size: %d", file, size);
        char *writeBuffer = GenerateTestingText(size);
        TEST_ASSERT_NOT_NULL(writeBuffer);
        RUN_AND_PRINT_ELAPSED_TIME_WITHOUT_RESULT(WriteFile(writeBuffer, fd, size));
        const int fileSize = lseek(fd, 0, SEEK_END);
        TEST_ASSERT_EQUAL(size, fileSize);
        TEST_ASSERT_EQUAL(0, lseek(fd, 0, SEEK_SET));

        LOGI("begin to read file [%s] for size: %d", file, fileSize);
        TEST_ASSERT_LESS_OR_EQUAL(TEN_KILOBYTE, fileSize);
        char *readBuffer = (char *)malloc(fileSize + 1);
        TEST_ASSERT_NOT_NULL(readBuffer);
        TEST_ASSERT_EQUAL(0, memset_s(readBuffer, fileSize + 1, 0, fileSize + 1));
        RUN_AND_PRINT_ELAPSED_TIME_WITHOUT_RESULT(ReadFile(readBuffer, fd, fileSize));
        TEST_ASSERT_EQUAL_STRING(writeBuffer, readBuffer);

        free(readBuffer);
        free(writeBuffer);
        TEST_ASSERT_EQUAL(0, close(fd));
        sleep(1);
    }
}

static void TestHcFileSize(void)
{
    const char *file = TEST_FILE_NAME;
    LOGI("begin to count file size");

    int fd = open(file, O_RDWR);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    int size;
    RUN_AND_PRINT_ELAPSED_TIME(size, lseek(fd, 0, SEEK_END));
    TEST_ASSERT_EQUAL(TEN_KILOBYTE, size);
    TEST_ASSERT_EQUAL(0, lseek(fd, 0, SEEK_SET));
    LOGI("the file size is: %d", size);

    TEST_ASSERT_EQUAL(0, close(fd));
    int ret;
    RUN_AND_PRINT_ELAPSED_TIME(ret, unlink(file));
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
#else
    LOGE("no MKDIR_IMPLEMENTED, do not test mkdir() !");
#endif
}

static void TestHcFileStat(void)
{
#if STAT_IMPLEMENTED
    const char *file = TEST_FILE_NAME;
    LOGI("begin to check [%s] state", file);
    TEST_ASSERT_GREATER_OR_EQUAL(0, open(file, O_RDWR | O_CREAT));

    struct stat fileStat;
    int ret;
    ret = memset_s(&fileStat, sizeof(fileStat), 0, sizeof(fileStat));
    TEST_ASSERT_EQUAL(0, ret);
    RUN_AND_PRINT_ELAPSED_TIME(ret, stat(file, &fileStat));
    TEST_ASSERT_EQUAL(0, ret);

    TEST_ASSERT_EQUAL(0, unlink(file));
    ret = memset_s(&fileStat, sizeof(fileStat), 0, sizeof(fileStat));
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(-1, stat(file, &fileStat));
    TEST_ASSERT_EQUAL(ENOENT, errno);
#else
    LOGE("no STAT_IMPLEMENTED, do not test stat() !");
#endif
}

static void TestHcFileRemove(void)
{
    const char *file = TEST_FILE_NAME;
    LOGI("begin to remove file: %s", file);

    int ret;
    RUN_AND_PRINT_ELAPSED_TIME(ret, unlink(file));
    TEST_ASSERT_EQUAL(0, ret);
}

void TestHcFile(void)
{
    LOGI("test opening and closing file");
    TestHcFileOpenAndClose();

    LOGI("test removing file");
    TestHcFileRemove();

    LOGI("test reading and writing file");
    TestHcFileReadAndWrite();

    LOGI("test counting file size");
    TestHcFileSize();

    LOGI("test making directory");
    TestHcFileMkdir();

    LOGI("test getting file state");
    TestHcFileStat();
}

#else // TEST_HC_FILE_OPEN_SERIES

void TestHcFile(void)
{
    LOGE("no TEST_HC_FILE_OPEN_SERIES, do not test hc_file open series!");
}

#endif // TEST_HC_FILE_OPEN_SERIES

#ifdef __cplusplus
}
#endif
