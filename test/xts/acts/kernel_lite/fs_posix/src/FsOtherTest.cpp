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
#include <sys/statvfs.h>
#include <sys/statfs.h>
#include <sys/mount.h>
#include <wchar.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <ftw.h>
#include <libgen.h>
#include <glob.h>

#include <gtest/gtest.h>

#include "utils.h"
#include "log.h"
#include "KernelConstants.h"
#include "libfs.h"

using namespace testing::ext;

/**
 * @tc.number   SUB_KERNEL_FS_OTHER_0100
 * @tc.name     basic function test : get dirname
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testPath, Function | MediumTest | Level1)
{
    int fd = 0;
    fd = creat(FILE0, 0777);
    EXPECT_NE(fd, -1) << "> creat faild errno = " << errno;
    EXPECT_NE(close(fd), -1) << "> close errno = " << errno;

    // get dir
    char *workDir = dirname((char*)FILE0);
    ASSERT_NE(workDir, nullptr) << "> dirname errno = " << errno;
    EXPECT_STREQ(".", workDir);
    LOG("> workDir = %s", workDir);
}

/**
 * @tc.number   SUB_KERNEL_FS_OTHER_0110
 * @tc.name     basic function test : get current dir name
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testGetCurrentDirName, Function | MediumTest | Level1)
{
    int fd = 0;
    fd = creat(FILE0, 0777);
    EXPECT_NE(fd, -1) << "> creat faild errno = " << errno;
    EXPECT_NE(close(fd), -1) << "> close errno = " << errno;

    // get current dir name
    const char *currentDirStandard = TOP_DIR;
    char *currentDir = get_current_dir_name();
    ASSERT_NE(currentDir, nullptr);
    EXPECT_STREQ(currentDir, currentDirStandard);
    LOG("> currentDir = %s", currentDir);
}

/**
 * @tc.number   SUB_KERNEL_FS_OTHER_0120
 * @tc.name     basic function test : get basename
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testBasename, Function | MediumTest | Level1)
{
    int fd = 0;
    fd = creat(FILE0, 0777);
    EXPECT_NE(fd, -1) << "> creat faild errno = " << errno;
    EXPECT_NE(close(fd), -1) << "> close errno = " << errno;

    // get file name
    char *desName = basename((char*)FILE0);
    ASSERT_NE(desName, nullptr) << "> basename errno = " << errno;
    EXPECT_STREQ(desName, FILE0);
    LOG("> desName = %s", desName);
}

int FtwCheckDirTree(const char *path, const struct stat *sb, int flag)
{
    const char file0[] = DIR0 "/" DIR0_FILE0;
    const char file1[] = DIR0 "/" DIR0_DIR1 "/" DIR0_DIR1_FILE0;
    const char dir0[] = DIR0;
    const char dir1[] = DIR0 "/" DIR0_DIR0;
    const char dir2[] = DIR0 "/" DIR0_DIR1;
    const char dir3[] = DIR0 "/" DIR0_DIR1"/" DIR0_DIR1_DIR0;
    if (flag == FTW_F) {
        if (strncmp(path, file0, sizeof(file0)) == 0) {
            LOG("> File %s", file0);
        } else if (strncmp(path, file1, sizeof(file1)) == 0) {
            LOG("> File %s", file1);
        } else {
            LOG("> File error %s", path);
            return -1;
        }
    } else if (flag == FTW_D) {
        if (strncmp(path, dir0, sizeof(dir0)) == 0) {
            LOG("> Dir  %s", DIR0);
        } else if (strncmp(path, dir1, sizeof(dir1)) == 0) {
            LOG("> Dir  %s", dir1);
        } else if (strncmp(path, dir2, sizeof(dir2)) == 0) {
            LOG("> Dir  %s", dir2);
        } else if (strncmp(path, dir3, sizeof(dir3)) == 0) {
            LOG("> Dir  %s", dir3);
        } else {
            LOG("> File error  %s", path);
            return -1;
        }
    }
    return 0;
}

/**
 * @tc.number   SUB_KERNEL_FS_OTHER_0200
 * @tc.name     basic function test : use ftw check file tree
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFtw, Function | MediumTest | Level3)
{
    CreateTestFolder();
    EXPECT_EQ(ftw(DIR0, FtwCheckDirTree, 100), 0) << "> ftw error";
}

int NftwCheckDirTree(const char *path, const struct stat *sb, int flag, struct FTW *s)
{
    const char file0[] = DIR0 "/" DIR0_FILE0;
    const char file1[] = DIR0 "/" DIR0_DIR1 "/" DIR0_DIR1_FILE0;
    const char dir0[] = DIR0;
    const char dir1[] = DIR0 "/" DIR0_DIR0;
    const char dir2[] = DIR0 "/" DIR0_DIR1;
    const char dir3[] = DIR0 "/" DIR0_DIR1"/" DIR0_DIR1_DIR0;
    if (flag == FTW_F) {
        if (strncmp(path, file0, sizeof(file0)) == 0) {
            LOG("> File %s", file0);
        } else if (strncmp(path, file1, sizeof(file1)) == 0) {
            LOG("> File %s", file1);
        } else {
            LOG("> File %s", path);
            return -1;
        }
    } else if (flag == FTW_D) {
        if (strncmp(path, dir0, sizeof(dir0)) == 0) {
            LOG("> Dir  %s", DIR0);
        } else if (strncmp(path, dir1, sizeof(dir1)) == 0) {
            LOG("> Dir  %s", dir1);
        } else if (strncmp(path, dir2, sizeof(dir2)) == 0) {
            LOG("> Dir  %s", dir2);
        } else if (strncmp(path, dir3, sizeof(dir3)) == 0) {
            LOG("> Dir  %s", dir3);
        } else {
            LOG("> File %s", path);
            return -1;
        }
    }
    return 0;
}

/**
 * @tc.number   SUB_KERNEL_FS_OTHER_0210
 * @tc.name     basic function test : use nftw check file tree
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testNftw, Function | MediumTest | Level3)
{
    CreateTestFolder();
    EXPECT_EQ(nftw(DIR0, NftwCheckDirTree, 100, 0), 0) << "> ftw error";
}

/**
 * @tc.number   SUB_KERNEL_FS_OTHER_0300
 * @tc.name     basic function test : use statvfs and statfs check file system information.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testStatvfs, Function | MediumTest | Level2)
{
    struct statvfs vfsBuf = {0};
    struct statfs fsBuf = {0};

    statvfs(".", &vfsBuf);
    LOG("vfsBuf.f_bsize = %lu", vfsBuf.f_bsize);        // File system block size.
    LOG("vfsBuf.f_frsize = %lu", vfsBuf.f_frsize);      // Fundamental file system block size.
    LOG("vfsBuf.f_blocks = %lu", vfsBuf.f_blocks);      // Total number of blocks on file system in units of f_frsize.
    LOG("vfsBuf.f_bfree = %lu", vfsBuf.f_bfree);        // Total number of free blocks.
    LOG("vfsBuf.f_bavail = %lu", vfsBuf.f_bavail);      // Number of free blocks available to non-privileged process.
    LOG("vfsBuf.f_files = %lu", vfsBuf.f_files);        // Total number of file serial numbers.
    LOG("vfsBuf.f_ffree = %lu", vfsBuf.f_ffree);        // Total number of free file serial numbers.
    LOG("vfsBuf.f_favail = %lu", vfsBuf.f_favail);      // Number of i-nodes available to unprivileged process.
    LOG("vfsBuf.f_fsid = %lu", vfsBuf.f_fsid);          // File system ID.
    LOG("vfsBuf.f_flag = %lu", vfsBuf.f_flag);          // Bit mask of f_flag values.
    LOG("vfsBuf.f_namemax = %lu", vfsBuf.f_namemax);    // Maximum filename length.

    statfs(".", &fsBuf);
    LOG("fsBuf.f_type = %lu", fsBuf.f_type);            // Type of filesystem.
    LOG("fsBuf.f_bsize = %lu", fsBuf.f_bsize);          // Optimal transfer block size.
    LOG("fsBuf.f_blocks = %lu", fsBuf.f_blocks);        // Total data blocks in filesystem.
    LOG("fsBuf.f_bfree = %lu", fsBuf.f_bfree);          // Total data blocks in filesystem.
    LOG("fsBuf.f_bavail = %lu", fsBuf.f_bavail);        // Free blocks available to unprivileged user.
    LOG("fsBuf.f_files = %lu", fsBuf.f_files);          // Total file nodes in filesystem.
    LOG("fsBuf.f_ffree = %lu", fsBuf.f_ffree);          // Free file nodes in filesystem.
    LOG("fsBuf.f_fsid.__val[0] = %d", fsBuf.f_fsid.__val[0]);   // Filesystem ID.
    LOG("fsBuf.f_fsid.__val[1] = %d", fsBuf.f_fsid.__val[1]);   // Filesystem ID.
    LOG("fsBuf.f_namelen = %ld", fsBuf.f_namelen);      // Maximum length of filenames.

    EXPECT_EQ(vfsBuf.f_bsize, fsBuf.f_bsize);
    EXPECT_EQ(vfsBuf.f_blocks, fsBuf.f_blocks);
    EXPECT_EQ(vfsBuf.f_files, fsBuf.f_files);
    EXPECT_EQ(vfsBuf.f_ffree, fsBuf.f_ffree);
    EXPECT_EQ(vfsBuf.f_namemax, fsBuf.f_namelen);
}

/**
 * @tc.number   SUB_KERNEL_FS_OTHER_0400
 * @tc.name     basic function test : Use glob function and globfree function for path generation and release
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testGlob, Function | MediumTest | Level3)
{
    glob_t buf;
    CreateTestFolder();
    EXPECT_EQ(glob(TOP_DIR "/" DIR0 "/*", GLOB_ERR, NULL, &buf), 0) << "> glod errno = " << errno;
    if (buf.gl_pathc == 3) {
        EXPECT_STREQ(buf.gl_pathv[0], TOP_DIR "/" DIR0 "/" DIR0_DIR0);
        EXPECT_STREQ(buf.gl_pathv[1], TOP_DIR "/" DIR0 "/" DIR0_DIR1);
        EXPECT_STREQ(buf.gl_pathv[2], TOP_DIR "/" DIR0 "/" DIR0_FILE0);
    } else {
        ADD_FAILURE();
    }
    globfree(&buf);
}
#if 0
/**
 * @tc.number   SUB_KERNEL_FS_OTHER_0500
 * @tc.name     basic function test : Use fwprintf function to write wide characters
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFwprintf, Function | MediumTest | Level3)
{
    const char filePath[] = TOP_DIR "/" DIR0 "/" DIR0_FILE0;
    FILE *fp = nullptr;
    wchar_t writeBuf[30] = L"this is a file";
    wchar_t readBuf[30];

    // write
    CreateTestFolder();
    fp = fopen(filePath, "w+");
    EXPECT_NE(fwprintf(fp, L"%ls", writeBuf), -1);
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" << errno;

    // read
    fp = fopen(filePath, "r");
    ASSERT_NE(fp, nullptr) << "> fopen errno = " << errno;
    EXPECT_NE(fgetws(readBuf, 30, fp), nullptr) << "fgetws error";
    EXPECT_TRUE(wcscmp(writeBuf, readBuf) == 0) << "writeBuf != readBuf";
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" << errno;
}
#endif
