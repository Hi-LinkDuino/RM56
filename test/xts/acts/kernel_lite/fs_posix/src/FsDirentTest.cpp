/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

/**
 * @tc.number   SUB_KERNEL_FS_DIRENT_0100
 * @tc.name     basic function test : readdir read directory
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testReaddir, Function | MediumTest | Level3)
{
    struct dirent *dResult = nullptr;
    DIR *dirp = nullptr;
    int reIntDir = 0;
    int reIntFile = 0;
    CreateTestFolder();

    dirp = opendir(TOP_DIR "/" DIR0);
    ASSERT_NE(dirp, nullptr) << "> opendir errno = " << errno;
    for (int i = 0; i < 10; i++) {          // Prevents infinite loops.
        dResult = readdir(dirp);
        if (dResult == nullptr) {
            break;
        } else if (strcmp(dResult->d_name, DIR0_DIR0) == 0) {
            LOG("> Read : %s OK", dResult->d_name);
            reIntDir++;
        }else if (strcmp(dResult->d_name, DIR0_DIR1) == 0) {
            LOG("> Read : %s OK", dResult->d_name);
            reIntDir++;
        } else if (strcmp(dResult->d_name, DIR0_FILE0) == 0) {
            LOG("> Read : %s OK", dResult->d_name);
            reIntFile++;
        } else {
            LOG("> Read : %s", dResult->d_name);
        }
    }
    EXPECT_EQ(closedir(dirp), 0) << "> closedir errno = " << errno;
    EXPECT_EQ(reIntDir, 2) << "> read reIntDir = " << reIntDir;
    EXPECT_EQ(reIntFile, 1) << "> read reIntFile = " << reIntFile;
}

/**
 * @tc.number   SUB_KERNEL_FS_DIRENT_0110
 * @tc.name     basic function test : test readdir with error number EBADF
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testReaddirEbadf, Function | MediumTest | Level3)
{
    struct dirent *dResult = nullptr;
    DIR *dirp = nullptr;
    CreateTestFolder();

    dirp = opendir(TOP_DIR "/" DIR0);
    ASSERT_NE(dirp, nullptr) << "> opendir errno = " << errno;
    EXPECT_EQ(closedir(dirp), 0) << "> closedir errno = " << errno;

    dResult = readdir(dirp);
    EXPECT_EQ(dResult, nullptr);
    EXPECT_EQ(errno, EBADF);
}

/**
 * @tc.number   SUB_KERNEL_FS_DIRENT_0200
 * @tc.name     basic function test : readdir_r read directory
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testReaddirR, Function | MediumTest | Level3)
{
    struct dirent dEntry = {0};
    struct dirent *dResult = nullptr;
    DIR *dirp = nullptr;
    int reIntDir = 0;
    int reIntFile = 0;
    CreateTestFolder();

    dirp = opendir(TOP_DIR "/" DIR0);
    ASSERT_NE(dirp, nullptr) << "> opendir errno = " << errno;
    for (int i = 0; i < 10; i++) {   // Prevents infinite loops.
        if ((readdir_r(dirp, &dEntry, &dResult)) != 0) {
            break;
        }
        if (dResult == nullptr) {
            break;
        } else if (strcmp(dResult->d_name, DIR0_DIR0) == 0) {
            LOG("> Read : %s OK", dResult->d_name);
            reIntDir++;
        } else if (strcmp(dResult->d_name, DIR0_DIR1) == 0) {
            LOG("> Read : %s OK", dResult->d_name);
            reIntDir++;
        } else if (strcmp(dResult->d_name, DIR0_FILE0) == 0) {
            LOG("> Read : %s OK", dResult->d_name);
            reIntFile++;
        } else {
            LOG("> Read : %s", dResult->d_name);
        }
    }
    EXPECT_EQ(closedir(dirp), 0) << "> closedir errno = " << errno;
    EXPECT_EQ(reIntDir, 2) << "> read reIntDir = " << reIntDir;
    EXPECT_EQ(reIntFile, 1) << "> read reIntFile = " << reIntFile;
}

/**
 * @tc.number   SUB_KERNEL_FS_DIRENT_0300
 * @tc.name     basic function test : scandir: scans a directory.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testScandir, Function | MediumTest | Level3)
{
    int sum = 0;
    int reInt = 0;
    struct dirent **fileList;
    const char *scanDir = TOP_DIR "/" DIR0;
    CreateTestFolder();

    sum = scandir(scanDir, &fileList, nullptr, alphasort);
    EXPECT_NE(sum, -1) << "> scandir errno = " << errno;
    for (int i = 0; i < sum; i++) {
        if (i == 0) {
            reInt = strncmp(fileList[i]->d_name, DIR0_DIR0, sizeof(DIR0_DIR0));
        } else if (i == 1) {
            reInt = strncmp(fileList[i]->d_name, DIR0_DIR1, sizeof(DIR0_DIR1));
        } else if (i == 2) {
            reInt = strncmp(fileList[i]->d_name, DIR0_FILE0, sizeof(DIR0_FILE0));
        }
        EXPECT_EQ(reInt, 0) << "> the " << i << "th d_name = " << fileList[i]->d_name;
        LOG("> %s", fileList[i]->d_name);
        free(fileList[i]);
    }
    free(fileList);
}

#ifndef COMMERCIAL
/**
 * @tc.number   SUB_KERNEL_FS_DIRENT_0400
 * @tc.name     basic function test : Sets the location of the next {readdir} in the directory stream.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testSeekdir, Function | MediumTest | Level3)
{
    struct dirent *dResult = nullptr;
    DIR *dirp = nullptr;
    CreateTestFolder();

    dirp = opendir(TOP_DIR "/" DIR0);
    ASSERT_NE(dirp, nullptr) << "> opendir errno = " << errno;

    dResult = readdir(dirp);
    ASSERT_FALSE(dResult == nullptr) << "> readdir errno = " << errno;
    EXPECT_EQ(telldir(dirp), dResult->d_off);
    LOG("> dResult->d_name = %s", dResult->d_name);
    LOG("> dResult->d_off = %lu", dResult->d_off);
    long tellDir0 = dResult->d_off;

    dResult = readdir(dirp);
    ASSERT_FALSE(dResult == nullptr) << "> readdir errno = " << errno;
    EXPECT_EQ(telldir(dirp), dResult->d_off);
    LOG("> dResult->d_name = %s", dResult->d_name);
    LOG("> dResult->d_off = %lu", dResult->d_off);
    long tellDir1 = dResult->d_off;

    dResult = readdir(dirp);
    ASSERT_FALSE(dResult == nullptr) << "> readdir errno = " << errno;
    LOG("> 111");
    EXPECT_EQ(telldir(dirp), dResult->d_off);
    LOG("> 222");
    LOG("> dResult->d_name = %s", dResult->d_name);
    LOG("> dResult->d_off = %lu", dResult->d_off);
    long tellDir2 = dResult->d_off;

    rewinddir(dirp);
    dResult = readdir(dirp);
    ASSERT_FALSE(dResult == nullptr) << "> readdir errno = " << errno;
    EXPECT_EQ(telldir(dirp), dResult->d_off);
    EXPECT_EQ(telldir(dirp), tellDir0);
    LOG("> dResult->d_name = %s", dResult->d_name);
    LOG("> dResult->d_off = %lu", dResult->d_off);

    seekdir(dirp, tellDir1);
    dResult = readdir(dirp);
    ASSERT_FALSE(dResult == nullptr) << "> readdir errno = " << errno;
    EXPECT_EQ(telldir(dirp), dResult->d_off);
    EXPECT_EQ(telldir(dirp), tellDir2);
    LOG("> dResult->d_name = %s", dResult->d_name);
    LOG("> dResult->d_off = %lu", dResult->d_off);

    EXPECT_EQ(closedir(dirp), 0) << "> closedir errno = " << errno;
}
#endif

/**
 * @tc.number   SUB_KERNEL_FS_DIRENT_0500
 * @tc.name     basic function test : test opendir return normal
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testOpendir, Function | MediumTest | Level2)
{
    DIR *dirp = nullptr;
    CreateTestFolder();

    errno = 0;
    dirp = opendir(TOP_DIR "/" DIR0);
    if (dirp == nullptr) {
        LOG("OPENDIR ERRNO +++");
        ADD_FAILURE();
    }
    EXPECT_EQ(errno, 0);
    EXPECT_EQ(closedir(dirp), 0) << "> closedir errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_DIRENT_0510
 * @tc.name     basic function test : test opendir with error number EINVAL
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testOpendirEinval, Function | MediumTest | Level3)
{
    DIR *dirp = nullptr;
    CreateTestFolder();

    dirp = opendir(nullptr);
    EXPECT_EQ(dirp, nullptr);
    EXPECT_EQ(errno, EINVAL);
    if (dirp != nullptr) {
        EXPECT_EQ(closedir(dirp), 0) << "> closedir errno = " << errno;
    }
}

/**
 * @tc.number   SUB_KERNEL_FS_DIRENT_0520
 * @tc.name     basic function test : test opendir with error number ENAMETOOLONG
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testOpendirEnametoolong, Function | MediumTest | Level3)
{
    const char *dirName = "12345678901234567890123456789012345678901234567890\
    12345678901234567890123456789012345678901234567890\
    12345678901234567890123456789012345678901234567890\
    12345678901234567890123456789012345678901234567890\
    12345678901234567890123456789012345678901234567890\
    12345678901234567890123456789012345678901234567890\
    12345678901234567890123456789012345678901234567890\
    12345678901234567890123456789012345678901234567890";
    DIR *dirp = nullptr;

    dirp = opendir(dirName);
    EXPECT_EQ(dirp, nullptr);
    EXPECT_EQ(errno, ENAMETOOLONG);
    if (dirp != nullptr) {
        EXPECT_EQ(closedir(dirp), 0) << "> closedir errno = " << errno;
    }
}

/**
 * @tc.number   SUB_KERNEL_FS_DIRENT_0530
 * @tc.name     basic function test : test opendir with error number ENOENT
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testOpendirENOENT, Function | MediumTest | Level3)
{
    DIR *dirp = nullptr;
    CreateTestFolder();

    dirp = opendir("noExistFile");
    EXPECT_EQ(dirp, nullptr);
    EXPECT_EQ(errno, ENOENT);
    if (dirp != nullptr) {
        EXPECT_EQ(closedir(dirp), 0) << "> closedir errno = " << errno;
    }
}

/**
 * @tc.number   SUB_KERNEL_FS_DIRENT_0540
 * @tc.name     basic function test : test opendir with error number ENOTDIR
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testOpendirEnotdir, Function | MediumTest | Level3)
{
    DIR *dirp = nullptr;
    CreateTestFolder();

    dirp = opendir(TOP_DIR "/" DIR0 "/" DIR0_FILE0);
    EXPECT_EQ(dirp, nullptr);
    EXPECT_EQ(errno, ENOTDIR);
    if (dirp != nullptr) {
        EXPECT_EQ(closedir(dirp), 0) << "> closedir errno = " << errno;
    }
}

/**
 * @tc.number   SUB_KERNEL_FS_DIRENT_0600
 * @tc.name     basic function test : test closedir
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testClosedir, Function | MediumTest | Level3)
{
    DIR *dirp = nullptr;
    CreateTestFolder();

    dirp = opendir(TOP_DIR "/" DIR0);
    if (dirp == nullptr) {
        LOG("opendir errno ++");
        ADD_FAILURE();
    }
    EXPECT_EQ(closedir(dirp), 0) << "> closedir errno = " << errno;
}
