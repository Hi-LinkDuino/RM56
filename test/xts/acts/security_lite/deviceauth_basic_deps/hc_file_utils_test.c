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

#include "hc_file_utils_test.h"

#include <hctest.h>
#include <securec.h>
#include <stdlib.h>
#include <unistd.h>

#include "hc_file_common.h"
#include "print_log.h"
#include "test_timer.h"
#include "utils_file.h"

#ifdef __cplusplus
extern "C" {
#endif

#if UTILS_FILE_IMPLEMENTED

static void TestHcFileUtilsFileOpenAndUtilsFileClose(void)
{
    const char *fileName = TEST_FILE_NAME;
    LOGI("begin to open file: %s", fileName);

    int fd;
    TEST_ASSERT_EQUAL(-1, UtilsFileOpen(fileName, O_RDONLY_FS, 0));
    RUN_AND_PRINT_ELAPSED_TIME(fd, UtilsFileOpen(fileName, O_RDONLY_FS | O_CREAT_FS, 0));
    TEST_ASSERT_GREATER_OR_EQUAL(0, fd);

    int ret;
    LOGI("begin to close file: %s", fileName);
    RUN_AND_PRINT_ELAPSED_TIME(ret, UtilsFileClose(fd));
    TEST_ASSERT_EQUAL(0, ret);
}

static void TestHcFileUtilsFileReadAndFileWrite(void)
{
    const char *fileName = TEST_FILE_NAME;
    for (int i = 0; i < TEST_FILE_SIZE_LIST_LEN; i++) {
        int fd = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
        TEST_ASSERT_GREATER_OR_EQUAL(0, fd);
        unsigned int size = testFileSizeList[i];

        LOGI("begin to write file [%s] for size: %d", fileName, size);
        char *writeBuffer = GenerateTestingText(size);
        TEST_ASSERT_NOT_NULL(writeBuffer);
        RUN_AND_PRINT_ELAPSED_TIME_WITHOUT_RESULT(UtilsFileWrite(fd, writeBuffer, size));
        TEST_ASSERT_EQUAL(0, UtilsFileClose(fd));

        fd = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
        TEST_ASSERT_GREATER_OR_EQUAL(0, fd);
        unsigned int fileSize;
        TEST_ASSERT_EQUAL(0, UtilsFileStat(fileName, &fileSize));
        TEST_ASSERT_EQUAL(size, fileSize);

        LOGI("begin to read file [%s] for size: %d", fileName, fileSize);
        char *readBuffer = (char *)malloc(fileSize + 1);
        TEST_ASSERT_NOT_NULL(readBuffer);
        TEST_ASSERT_EQUAL(0, memset_s(readBuffer, fileSize + 1, 0, fileSize + 1));
        RUN_AND_PRINT_ELAPSED_TIME_WITHOUT_RESULT(UtilsFileRead(fd, readBuffer, fileSize));
        TEST_ASSERT_EQUAL_STRING(writeBuffer, readBuffer);

        free(readBuffer);
        free(writeBuffer);
        TEST_ASSERT_EQUAL(0, UtilsFileClose(fd));
        sleep(1);
    }
}

static void TestHcFileUtilsFileSize(void)
{
    const char *fileName = TEST_FILE_NAME;
    LOGI("begin to count file size");

    unsigned int size;
    int ret;
    RUN_AND_PRINT_ELAPSED_TIME(ret, UtilsFileStat(fileName, &size));
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(TEN_KILOBYTE, size);
    LOGI("the file size is: %d", size);

    RUN_AND_PRINT_ELAPSED_TIME(ret, UtilsFileDelete(fileName));
    TEST_ASSERT_EQUAL(0, ret);
}

static void TestHcFileUtilsFileDelete(void)
{
    const char *fileName = TEST_FILE_NAME;
    LOGI("begin to remove file: %s", fileName);

    int ret;
    RUN_AND_PRINT_ELAPSED_TIME(ret, UtilsFileDelete(fileName));
    TEST_ASSERT_EQUAL(0, ret);
}

void TestHcFileUtilsFile(void)
{
    LOGI("test opening and closing file");
    TestHcFileUtilsFileOpenAndUtilsFileClose();

    LOGI("test removing file");
    TestHcFileUtilsFileDelete();

    LOGI("test reading and writing file");
    TestHcFileUtilsFileReadAndFileWrite();

    LOGI("test counting file size");
    TestHcFileUtilsFileSize();
}

#else // UTILS_FILE_IMPLEMENTED
void TestHcFileUtilsFile(void)
{
    LOGE("no UTILS_FILE_IMPLEMENTED, do not test utils file!");
}
#endif // UTILS_FILE_IMPLEMENTED

#ifdef __cplusplus
}
#endif
