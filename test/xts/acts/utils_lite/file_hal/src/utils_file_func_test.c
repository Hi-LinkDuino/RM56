/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_types.h"
#include <securec.h>
#include "hctest.h"
#include "utils_file.h"

#define QUERY_TIMES    50
#define FILE_SEEK_OFFSET_10 10
#define FILE_SEEK_OFFSET_20 20
#define FILE_SEEK_OFFSET_50 50
#define FILE_SEEK_OFFSET_MINUS_10 (-10)
#define FILE_SEEK_OFFSET_MINUS_20 (-20)
#define FILE_SEEK_OFFSET_MINUS_50 (-50)
#define LENGTH_OF_READ_BUF 36

static const char* g_def = "utils_file_operation implement.";

/**
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is UtilsFileFuncTestSuite
 */
LITE_TEST_SUIT(utils, utilsFile, UtilsFileFuncTestSuite);

/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL UtilsFileFuncTestSuiteSetUp(void)
{
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL UtilsFileFuncTestSuiteTearDown(void)
{
    printf("+-------------------------------------------+\n");
    return TRUE;
}

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_0100
 * @tc.name      : File operation for file creation and close with parameter RDONLY
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileCreatAndClose001, Function | MediumTest | Level1)
{
    char* fileName = "testfile101";
    int fd = UtilsFileOpen(fileName, O_RDONLY_FS | O_CREAT_FS, 0);
    TEST_ASSERT_GREATER_THAN_INT(0, fd);
    int ret = UtilsFileClose(fd);
    TEST_ASSERT_EQUAL_INT(0, ret);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_0200
 * @tc.name      : File operation for file creat and close with parameter WRONLY
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileCreatAndClose002, Function | MediumTest | Level1)
{
    char* fileName = "testfile102";
    int fd = UtilsFileOpen(fileName, O_WRONLY_FS | O_CREAT_FS, 0);
    TEST_ASSERT_GREATER_THAN_INT(0, fd);
    int ret = UtilsFileClose(fd);
    TEST_ASSERT_EQUAL_INT(0, ret);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_0300
 * @tc.name      : File operation for file creat and close with parameter RDWR
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileCreatAndClose003, Function | MediumTest | Level1)
{
    char* fileName = "testfile103";
    int fd = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    TEST_ASSERT_GREATER_THAN_INT(0, fd);
    int ret = UtilsFileClose(fd);
    TEST_ASSERT_EQUAL_INT(0, ret);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_0400
 * @tc.name      : File operation for file open and close with parameter RDONLY
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileOpenAndClose001, Function | MediumTest | Level1)
{
    char* fileName = "testfile104";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDONLY_FS, 0);
    TEST_ASSERT_GREATER_THAN_INT(0, fd);
    int ret = UtilsFileClose(fd);
    TEST_ASSERT_EQUAL_INT(0, ret);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_0500
 * @tc.name      : File operation for file open and close with parameter WRONLY
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileOpenAndClose002, Function | MediumTest | Level1)
{
    char* fileName = "testfile105";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_WRONLY_FS, 0);
    TEST_ASSERT_GREATER_THAN_INT(0, fd);
    int ret = UtilsFileClose(fd);
    TEST_ASSERT_EQUAL_INT(0, ret);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_0600
 * @tc.name      : File operation for file open and close with parameter RDWR
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileOpenAndClose003, Function | MediumTest | Level1)
{
    char* fileName = "testfile106";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDWR_FS, 0);
    TEST_ASSERT_GREATER_THAN_INT(0, fd);
    int ret = UtilsFileClose(fd);
    TEST_ASSERT_EQUAL_INT(0, ret);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_0700
 * @tc.name      : File operation for file open and close with parameter TRUNC
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileOpenAndClose004, Function | MediumTest | Level1)
{
    char* fileName = "testfile107";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDWR_FS | O_TRUNC_FS, 0);
    TEST_ASSERT_GREATER_THAN_INT(0, fd);
    int ret = UtilsFileClose(fd);
    TEST_ASSERT_EQUAL_INT(0, ret);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_0800
 * @tc.name      : File operation for file open and close with parameter APPEND
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileOpenAndClose005, Function | MediumTest | Level1)
{
    char* fileName = "testfile108";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDWR_FS | O_APPEND_FS, 0);
    TEST_ASSERT_GREATER_THAN_INT(0, fd);
    int ret = UtilsFileClose(fd);
    TEST_ASSERT_EQUAL_INT(0, ret);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_0900
 * @tc.name      : File operation for file creat and close with parameter RDONLY and EXCL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileCreatAndClose004, Function | MediumTest | Level1)
{
    char* fileName = "testfile109";
    int fd = UtilsFileOpen(fileName, O_RDONLY_FS | O_CREAT_FS | O_EXCL_FS, 0);
    TEST_ASSERT_GREATER_THAN_INT(0, fd);
    int ret = UtilsFileClose(fd);
    TEST_ASSERT_EQUAL_INT(0, ret);
    int fd1 = UtilsFileOpen(fileName, O_RDONLY_FS | O_CREAT_FS | O_EXCL_FS, 0);
    TEST_ASSERT_EQUAL_INT(-1, fd1);
    int ret1 = UtilsFileClose(fd1);
    TEST_ASSERT_EQUAL_INT(-1, ret1);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_1000
 * @tc.name      : File operation for file creation and close with parameter WRONLY and EXCL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileCreatAndClose005, Function | MediumTest | Level1)
{
    char* fileName = "testfile110";
    int fd = UtilsFileOpen(fileName, O_WRONLY_FS | O_CREAT_FS | O_EXCL_FS, 0);
    TEST_ASSERT_GREATER_THAN_INT(0, fd);
    int ret = UtilsFileClose(fd);
    TEST_ASSERT_EQUAL_INT(0, ret);
    int fd1 = UtilsFileOpen(fileName, O_WRONLY_FS | O_CREAT_FS | O_EXCL_FS, 0);
    TEST_ASSERT_EQUAL_INT(-1, fd1);
    int ret1 = UtilsFileClose(fd1);
    TEST_ASSERT_EQUAL_INT(-1, ret1);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_1100
 * @tc.name      : File operation for file creat and close with parameter RDWR and EXCL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileCreatAndClose006, Function | MediumTest | Level1)
{
    char* fileName = "testfile111";
    int fd = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS | O_EXCL_FS, 0);
    TEST_ASSERT_GREATER_THAN_INT(0, fd);
    int ret = UtilsFileClose(fd);
    TEST_ASSERT_EQUAL_INT(0, ret);
    int fd1 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS | O_EXCL_FS, 0);
    TEST_ASSERT_EQUAL_INT(-1, fd1);
    int ret1 = UtilsFileClose(fd1);
    TEST_ASSERT_EQUAL_INT(-1, ret1);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_1200
 * @tc.name      : File operation for file creation and close with parameter TRUNC and EXCL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileCreatAndClose007, Function | MediumTest | Level1)
{
    char* fileName = "testfile112";
    int fd = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS | O_EXCL_FS | O_TRUNC_FS, 0);
    TEST_ASSERT_GREATER_THAN_INT(0, fd);
    int ret = UtilsFileClose(fd);
    TEST_ASSERT_EQUAL_INT(0, ret);
    int fd1 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS | O_EXCL_FS | O_TRUNC_FS, 0);
    TEST_ASSERT_EQUAL_INT(-1, fd1);
    int ret1 = UtilsFileClose(fd1);
    TEST_ASSERT_EQUAL_INT(-1, ret1);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_1300
 * @tc.name      : File operation for file creation and close with parameter APPEND and EXCL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileCreatAndClose008, Function | MediumTest | Level1)
{
    char* fileName = "testfile113";
    int fd = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS | O_EXCL_FS | O_APPEND_FS, 0);
    TEST_ASSERT_GREATER_THAN_INT(0, fd);
    int ret = UtilsFileClose(fd);
    TEST_ASSERT_EQUAL_INT(0, ret);
    int fd1 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS | O_EXCL_FS | O_APPEND_FS, 0);
    TEST_ASSERT_EQUAL_INT(-1, fd1);
    int ret1 = UtilsFileClose(fd1);
    TEST_ASSERT_EQUAL_INT(-1, ret1);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_1400
 * @tc.name      : File operation for file write
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileWrite001, Function | MediumTest | Level1)
{
    char* fileName = "testfile114";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDWR_FS, 0);
    int ret = UtilsFileWrite(fd, g_def, strlen(g_def));
    TEST_ASSERT_GREATER_THAN_INT(0, ret);
    UtilsFileClose(fd);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_1500
 * @tc.name      : File write operation with parameter APPEND and TRUNC
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileWrite002, Function | MediumTest | Level1)
{
    char* fileName = "testfile115";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDWR_FS | O_APPEND_FS, 0);
    int ret = UtilsFileWrite(fd, g_def, strlen(g_def));
    TEST_ASSERT_GREATER_THAN_INT(0, ret);
    UtilsFileClose(fd);
    int fd1 = UtilsFileOpen(fileName, O_RDWR_FS | O_TRUNC_FS, 0);
    int ret1 = UtilsFileWrite(fd1, g_def, strlen(g_def));
    TEST_ASSERT_GREATER_THAN_INT(0, ret1);
    UtilsFileClose(fd1);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_1600
 * @tc.name      : Creat file with long file name
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testCreatLongNameFile, Function | MediumTest | Level1)
{
    char* fileName = "testLongFileName116-Ab123456789";
    int fd = UtilsFileOpen(fileName, O_RDONLY_FS | O_CREAT_FS, 0);
    TEST_ASSERT_GREATER_THAN_INT(0, fd);
    UtilsFileClose(fd);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_1800
 * @tc.name      : Open none exist file
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testOpenNotExistFile, Function | MediumTest | Level1)
{
    char* fileName = "fileNotExist118";
    int fd = UtilsFileOpen(fileName, O_RDONLY_FS, 0);
    TEST_ASSERT_EQUAL_INT(-1, fd);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_1900
 * @tc.name      : Write none exist file
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testWriteNotExistFile, Function | MediumTest | Level1)
{
    int fd = 1073;
    int ret = UtilsFileWrite(fd, g_def, strlen(g_def));
    TEST_ASSERT_EQUAL_INT(-1, ret);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_2000
 * @tc.name      : Close none exist file
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testCloseNotExistFile, Function | MediumTest | Level1)
{
    int fd = 1073;
    int ret = UtilsFileClose(fd);
    TEST_ASSERT_EQUAL_INT(-1, ret);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_2100
 * @tc.name      : Creat file with invalid long file name
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testCreatInvalidlongNameFile, Function | MediumTest | Level1)
{
    char* fileName = "copyLongFileName403-Abcdefg123456789Abcdefg123456789Abcdefg123456789\
                    Abcdefg123456789Abcdefg123456789";
    int fd = UtilsFileOpen(fileName, O_RDONLY_FS | O_CREAT_FS, 0);
    TEST_ASSERT_EQUAL_INT(-1, fd);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_2200
 * @tc.name      : File operation for fileseek moved offset 20 from begin
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileSeek001, Function | MediumTest | Level1)
{
    char* fileName = "testfile201";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDWR_FS, 0);
    int ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_20, SEEK_SET_FS);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, ret);
    UtilsFileClose(fd);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_2300
 * @tc.name      : File operation for fileseek moved offset 20 from current
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileSeek002, Function | MediumTest | Level1)
{
    char* fileName = "testfile202";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDWR_FS, 0);
    int ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_20, SEEK_CUR_FS);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, ret);
    UtilsFileClose(fd);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_2400
 * @tc.name      : File operation for fileseek moved offset 20 backward from end
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileSeek003, Function | MediumTest | Level1)
{
    char* fileName = "testfile203";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDWR_FS, 0);
    int ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_MINUS_20, SEEK_END_FS);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, ret);
    UtilsFileClose(fd);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_2500
 * @tc.name      : File operation for fileseek moved offset 50 from begin
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileSeek004, Function | MediumTest | Level1)
{
    char* fileName = "testfile204";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDWR_FS, 0);
    int ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_50, SEEK_SET_FS);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    UtilsFileClose(fd);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_2600
 * @tc.name      : File operation for fileseek moved offset 50 from current
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileSeek005, Function | MediumTest | Level1)
{
    char* fileName = "testfile205";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDWR_FS, 0);
    int ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_50, SEEK_CUR_FS);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    UtilsFileClose(fd);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_2700
 * @tc.name      : File operation for fileseek moved offset 50 backward from end
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileSeek006, Function | MediumTest | Level1)
{
    char* fileName = "testfile206";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDWR_FS, 0);
    int ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_MINUS_50, SEEK_END_FS);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    UtilsFileClose(fd);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_2800
 * @tc.name      : File operation for fileseek moved offset 20 from begin, then moved offset 10 from current
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileSeek007, Function | MediumTest | Level1)
{
    char* fileName = "testfile207";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDWR_FS, 0);
    int ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_20, SEEK_SET_FS);
    ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_10, SEEK_CUR_FS);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, ret);
    UtilsFileClose(fd);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_2900
 * @tc.name      : File operation for fileseek moved offset 20 from begin, then moved offset 10 backward from current
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileSeek008, Function | MediumTest | Level1)
{
    char* fileName = "testfile208";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDWR_FS, 0);
    int ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_20, SEEK_SET_FS);
    ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_MINUS_10, SEEK_CUR_FS);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, ret);
    UtilsFileClose(fd);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_3000
 * @tc.name      : File operation for fileseek moved offset 20 from begin, then moved offset 50 from current
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileSeek009, Function | MediumTest | Level1)
{
    char* fileName = "testfile209";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDWR_FS, 0);
    int ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_20, SEEK_SET_FS);
    ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_50, SEEK_CUR_FS);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    UtilsFileClose(fd);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_3100
 * @tc.name      : File operation for fileseek moved offset 20 from begin, then moved offset 50 backward from current
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileSeek010, Function | MediumTest | Level1)
{
    char* fileName = "testfile210";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDWR_FS, 0);
    int ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_20, SEEK_SET_FS);
    ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_MINUS_50, SEEK_CUR_FS);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    UtilsFileClose(fd);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_3200
 * @tc.name      : File operation for fileseek moved multiples times
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileSeek011, Function | MediumTest | Level1)
{
    char* fileName = "testfile211";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    int fd = UtilsFileOpen(fileName, O_RDWR_FS, 0);
    int ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_20, SEEK_SET_FS);
    ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_MINUS_50, SEEK_CUR_FS);
    ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_MINUS_10, SEEK_CUR_FS);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, ret);
    UtilsFileClose(fd);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_3300
 * @tc.name      : File seek for none exist file
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileSeekForNotExistFile, Function | MediumTest | Level1)
{
    int fd = 1073;
    int ret = UtilsFileSeek(fd, FILE_SEEK_OFFSET_20, SEEK_SET_FS);
    TEST_ASSERT_EQUAL_INT(-1, ret);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_3400
 * @tc.name      : Obtaining file size
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testObtainFileSize, Function | MediumTest | Level1)
{
    char* fileName = "testfile301";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    unsigned int fileLen = 0;
    int ret = UtilsFileStat(fileName, &fileLen);
    TEST_ASSERT_EQUAL_INT(0, ret);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_3500
 * @tc.name      : Obtaining size for none exist file
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testObtainSizeForNotExistFile, Function | MediumTest | Level1)
{
    char* fileName = "fileNotExist302";
    unsigned int fileLen = 0;
    int ret = UtilsFileStat(fileName, &fileLen);
    TEST_ASSERT_EQUAL_INT(-1, ret);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_3600
 * @tc.name      : File operation for file read
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileRead, Function | MediumTest | Level1)
{
    char* fileName = "testfile303";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    char buf[LENGTH_OF_READ_BUF] = {0};
    int fd = UtilsFileOpen(fileName, O_RDWR_FS, 0);
    int ret = UtilsFileRead(fd, buf, LENGTH_OF_READ_BUF);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, ret);
    UtilsFileClose(fd);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_3700
 * @tc.name      : Read none exist file
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testReadNotExistFile, Function | MediumTest | Level1)
{
    char buf[LENGTH_OF_READ_BUF] = {0};
    int fd = 1073;
    int ret = UtilsFileRead(fd, buf, LENGTH_OF_READ_BUF);
    TEST_ASSERT_EQUAL_INT(-1, ret);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_3800
 * @tc.name      : Obtaining size for file with long file name
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testObtainLongNameFileSize, Function | MediumTest | Level1)
{
    char* fileName = "testLongFileName305-Ab123456789";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    unsigned int fileLen = 0;
    int ret = UtilsFileStat(fileName, &fileLen);
    TEST_ASSERT_EQUAL_INT(0, ret);
    UtilsFileDelete(fileName);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_4000
 * @tc.name      : File operation for file copy
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileCopy, Function | MediumTest | Level1)
{
    char* fileName = "testfile401";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    char* fileNameCopy = "copyFile401";
    int ret = UtilsFileCopy(fileName, fileNameCopy);
    TEST_ASSERT_EQUAL_INT(0, ret);
    UtilsFileDelete(fileName);
    UtilsFileDelete(fileNameCopy);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_4100
 * @tc.name      : Copy none exist file
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testCopyNotExistFile, Function | MediumTest | Level1)
{
    char* fileName = "fileNotExist402";
    char* fileNameCopy = "copyFile402";
    int ret = UtilsFileCopy(fileName, fileNameCopy);
    TEST_ASSERT_EQUAL_INT(-1, ret);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_4200
 * @tc.name      : Copy file with long file name
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testCopyLongNameFile, Function | MediumTest | Level1)
{
    char* fileName = "testLongFileName403-Ab123456789";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    char* fileNameCopy = "copyLongFileName403-Ab123456789";
    int ret = UtilsFileCopy(fileName, fileNameCopy);
    TEST_ASSERT_EQUAL_INT(0, ret);
    UtilsFileDelete(fileName);
    UtilsFileDelete(fileNameCopy);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_4400
 * @tc.name      : File operation for file move
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileMove, Function | MediumTest | Level1)
{
    char* fileName = "testfile501";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    char* fileNameMove = "moveFile501";
    int ret = UtilsFileMove(fileName, fileNameMove);
    TEST_ASSERT_EQUAL_INT(0, ret);
    UtilsFileDelete(fileNameMove);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_4500
 * @tc.name      : Move none exist file
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testMoveNotExistFile, Function | MediumTest | Level1)
{
    char* fileName = "fileNotExist502";
    char* fileNameMove = "moveFile502";
    int ret = UtilsFileMove(fileName, fileNameMove);
    TEST_ASSERT_EQUAL_INT(-1, ret);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_4600
 * @tc.name      : Move file with long file name
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testMoveLongNameFile, Function | MediumTest | Level1)
{
    char* fileName = "testLongFileName503-Ab123456789";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    char* fileNameMove = "moveLongFileName503-Ab123456789";
    int ret = UtilsFileMove(fileName, fileNameMove);
    TEST_ASSERT_EQUAL_INT(0, ret);
    UtilsFileDelete(fileNameMove);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_4800
 * @tc.name      : File operation for file delete
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testFileDelete, Function | MediumTest | Level1)
{
    char* fileName = "deleteFile601";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    int ret = UtilsFileDelete(fileName);
    TEST_ASSERT_EQUAL_INT(0, ret);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_4900
 * @tc.name      : Delete none exist file
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testDeleteNotExistFile, Function | MediumTest | Level1)
{
    char* fileName = "fileNotExist602";
    int ret = UtilsFileDelete(fileName);
    TEST_ASSERT_EQUAL_INT(-1, ret);
};

/**
 * @tc.number    : SUB_UTILS_FILE_OPERATION_5000
 * @tc.name      : Delete file with long file name
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(UtilsFileFuncTestSuite, testDeleteLongNameFile, Function | MediumTest | Level1)
{
    char* fileName = "deleteLongFileName603-Ab1234567";
    int fd0 = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS, 0);
    UtilsFileWrite(fd0, g_def, strlen(g_def));
    UtilsFileClose(fd0);
    int ret = UtilsFileDelete(fileName);
    TEST_ASSERT_EQUAL_INT(0, ret);
};

RUN_TEST_SUITE(UtilsFileFuncTestSuite);
