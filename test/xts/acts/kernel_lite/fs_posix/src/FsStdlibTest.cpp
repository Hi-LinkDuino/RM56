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
#include "FileSystemTest.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <ftw.h>
#include <libgen.h>

#include <gtest/gtest.h>

#include "utils.h"
#include "log.h"
#include "KernelConstants.h"
#include "libfs.h"

using namespace testing::ext;
#if 0
/**
 * @tc.number   SUB_KERNEL_FS_STDLIB_0100
 * @tc.name     basic function test : mkstemp create a temporary file
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testMkstemp, Function | MediumTest | Level2)
{
    int fd = 0;
    char tmpFileName[]= "tmpFile_XXXXXX";
    fd = mkstemp(tmpFileName);
    EXPECT_NE(fd, -1) << "> mkstemp errno = " << errno;
    WriteCloseTest(fd);
    fd = open(tmpFileName, O_RDWR);
    EXPECT_NE(fd, -1) << "> open errno = " << errno;
    ReadCloseTest(fd);
    EXPECT_NE(unlink(tmpFileName), -1) << "> unlink errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_STDLIB_0200
 * @tc.name     basic function test : mkstemps create a temporary file
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testMkstemps, Function | MediumTest | Level2)
{
    int fd = 0;
    char tmpFileName[] = "tmpFile_XXXXXX";
    fd = mkstemps(tmpFileName, 0);
    EXPECT_NE(fd, -1) << "> mkstemp errno = " << errno;
    WriteCloseTest(fd);
    fd = open(tmpFileName, O_RDWR);
    EXPECT_NE(fd, -1) << "> open errno = " << errno;
    ReadCloseTest(fd);
    EXPECT_NE(unlink(tmpFileName), -1) << "> unlink errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_STDLIB_0300
 * @tc.name     basic function test : mkostemp create a temporary file with read and write
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testMkostemp, Function | MediumTest | Level2)
{
    int fd = 0;
    char tmpFileName[] = "tmpFile_XXXXXX";
    fd = mkostemp(tmpFileName, O_RDWR);
    EXPECT_NE(fd, -1) << "> mkstemp errno = " << errno;
    WriteCloseTest(fd);
    fd = open(tmpFileName, O_RDWR);
    EXPECT_NE(fd, -1) << "> open errno = " << errno;
    ReadCloseTest(fd);
    EXPECT_NE(unlink(tmpFileName), -1) << "> unlink errno = " << errno;
}
#endif
/**
 * @tc.number   SUB_KERNEL_FS_STDLIB_0400
 * @tc.name     basic function test : mktemp create a temporary file name, mkdtemp create a directory
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testMktempMkdtemp, Function | MediumTest | Level2)
{
    char tmpFileNamePre[] = "tmpFile_XXXXXX";
    char tmpFileName[] = "tmpFile_XXXXXX";
    EXPECT_NE(mktemp(tmpFileName), nullptr) << "> mktemp errno = " << errno;
    EXPECT_STRNE(tmpFileName, tmpFileNamePre) << "> tmpFileName no change";

    char tmpDirName[] = "tmpDir_XXXXXXX";
    EXPECT_NE(mkdtemp(tmpDirName), nullptr) << "> mktemp errno = " << errno;
    EXPECT_NE(remove(tmpDirName), -1) << "> remove errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_STDLIB_0500
 * @tc.name     basic function test : Use the realpath function to obtain the path.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testRealpath, Function | MediumTest | Level3)
{
    int fd = 0;
    fd = creat(FILE0, 0777);
    EXPECT_NE(fd, -1) << "> creat faild errno = " << errno;
    EXPECT_NE(close(fd), -1) << "> close errno = " << errno;

    // get Absolute Path
    const char *realPathStandard = TOP_DIR "/" FILE0;
    char *realPath = (char *)malloc(256);
    if (realpath(FILE0, realPath) == nullptr)
    {
        LOG("> realpath errno == %d", errno);
        free(realPath);
    }
    else
    {
        EXPECT_STREQ(realPath, realPathStandard);
        LOG("> realPath = %s", realPath);
        free(realPath);
    }
}
