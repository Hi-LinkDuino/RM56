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

#if defined(LITE_FS_NFS) || defined(LITE_FS_VFAT)
/**
 * @tc.number   SUB_KERNEL_FS_FCNTL_0100
 * @tc.name     basic function test : Test fcntl, use F_DUPFD to copy
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFcntl, Function | MediumTest | Level3)
{
    int fd = 0;
    EXPECT_NE((fd = open(FILE0, O_CREAT | O_RDWR, 0777)), -1) << "> open faild errno = " << errno;
    WriteCloseTest(fd);

    // F_DUPFD
    int fdNew = 0;
    EXPECT_NE((fd = open(FILE0, O_RDONLY, 0777)), -1) << "> open faild errno = " << errno;
    // openharmony not support F_DUPFD, after support change to NE
    EXPECT_EQ((fdNew = fcntl(fd, F_DUPFD)), -1) << "> fcntl errno = " << errno;
    if (fdNew != -1) {
        ReadCloseTest(fdNew);
    }
    EXPECT_NE(close(fd), -1) << "> close errno = " << errno;
}
#endif

/**
 * @tc.number   SUB_KERNEL_FS_FCNTL_0110
 * @tc.name     basic function test : Test fcntl with F_GETFL
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFcntlFgetfl, Function | MediumTest | Level3)
{
    int fd = 0;
    EXPECT_NE((fd = open(FILE0, O_CREAT | O_RDWR, 0777)), -1) << "> open faild errno = " << errno;
    WriteCloseTest(fd);

    int flagReadWrite = GetRandom(3) - 1;       // 0 is O_RDONLY, 1 is O_WRONLY, 2 is O_RDWR
    EXPECT_NE((fd = open(FILE0, flagReadWrite, 0777)), -1) << "> open faild errno = " << errno;

    // F_GETFL
    int flagGet = 0;
    EXPECT_NE((flagGet = fcntl(fd, F_GETFL)), -1) << "> fcntl errno = " << errno;
    EXPECT_EQ(flagReadWrite, (flagGet & O_ACCMODE)) << "> flagReadWrite != (flagGet & O_ACCMODE)";
    EXPECT_NE(close(fd), -1) << "> close errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_FCNTL_0120
 * @tc.name     basic function test : Test fcntl with F_SETFL
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFcntlFsetfl, Function | MediumTest | Level3)
{
    int fd = 0;
    EXPECT_NE((fd = open(FILE0, O_CREAT | O_RDWR, 0777)), -1) << "> open faild errno = " << errno;
    WriteCloseTest(fd);

    int flagReadWrite = GetRandom(3) - 1;       // 0 is O_RDONLY, 1 is O_WRONLY, 2 is O_RDWR
    EXPECT_NE((fd = open(FILE0, flagReadWrite, 0777)), -1) << "> open faild errno = " << errno;

    // F_SETFL
    int flagSet = fcntl(fd, F_GETFL) | O_APPEND | O_NONBLOCK;
    EXPECT_NE(fcntl(fd, F_SETFL, flagSet), -1) << "> fcntl errno = " << errno;

    // use F_GETFL check
    int flagGet = fcntl(fd, F_GETFL);
    EXPECT_NE(flagGet, -1) << "> fcntl errno = " << errno;
    EXPECT_EQ(flagGet & O_APPEND, O_APPEND) << "> O_APPEND faild";
    EXPECT_EQ(flagGet & O_NONBLOCK, O_NONBLOCK) << "> O_NONBLOCK faild";
    EXPECT_NE(close(fd), -1) << "> close errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_FCNTL_0200
 * @tc.name     basic function test : test lseek with SEEK_CUR
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testLseek, Function | MediumTest | Level3)
{
    int reLseek;
    int fd = 0;
    char writeBuf[100];
    for (int i = 0; i < 100; i++) {
        writeBuf[i] = '1';
    }

    fd = open(FILE0, O_CREAT | O_RDWR, 0777);
    EXPECT_NE(fd, -1) << "> open faild errno = " << errno;
    EXPECT_NE(write(fd, writeBuf, 20), -1) << "> write errno = " << errno;

    // reLseek 0 -> 20
    reLseek = lseek(fd, 0, SEEK_CUR);
    EXPECT_NE(reLseek, -1) << "> lseek errno = " << errno;
    EXPECT_EQ(reLseek, 20) << "> reLseek = " << reLseek;

    // reLseek 20 -> 40
    EXPECT_NE(write(fd, writeBuf, 20), -1) << "> write errno = " << errno;
    reLseek = lseek(fd, 0, SEEK_CUR);
    EXPECT_NE(reLseek, -1) << "> lseek errno = " << errno;
    EXPECT_EQ(reLseek, 40) << "> reLseek = " << reLseek;

    EXPECT_NE(close(fd), -1) << "> close errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_FCNTL_0210
 * @tc.name     basic function test : test lseek wioth SEEK_SET
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testLseekSeekSet, Function | MediumTest | Level3)
{
    int reLseek;
    int fd = 0;
    char writeBuf[100];
    for (int i = 0; i < 100; i++) {
        writeBuf[i] = '1';
    }

    fd = open(FILE0, O_CREAT | O_RDWR, 0777);
    EXPECT_NE(fd, -1) << "> open faild errno = " << errno;
    EXPECT_NE(write(fd, writeBuf, 20), -1) << "> write errno = " << errno;

    // reLseek 20 -> 0
    reLseek = lseek(fd, 0, SEEK_SET);
    EXPECT_NE(reLseek, -1) << "> lseek errno = " << errno;
    EXPECT_EQ(reLseek, 0) << "> reLseek = " << reLseek;

    // reLseek 0 -> 10
    reLseek = lseek(fd, 10, SEEK_SET);
    EXPECT_NE(reLseek, -1) << "> lseek errno = " << errno;
    EXPECT_EQ(reLseek, 10) << "> reLseek = " << reLseek;

    EXPECT_NE(close(fd), -1) << "> close errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_FCNTL_0220
 * @tc.name     basic function test : test lseek wioth SEEK_END
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testLseekSeekEnd, Function | MediumTest | Level3)
{
    int reLseek;
    int fd = 0;
    char writeBuf[100];
    for (int i = 0; i < 100; i++) {
        writeBuf[i] = '1';
    }

    fd = open(FILE0, O_CREAT | O_RDWR, 0777);
    EXPECT_NE(fd, -1) << "> open faild errno = " << errno;
    EXPECT_NE(write(fd, writeBuf, 20), -1) << "> write errno = " << errno;

    // reLseek 20 -> 10
    reLseek = lseek(fd, -10, SEEK_END);
    EXPECT_NE(reLseek, -1) << "> lseek errno = " << errno;
    EXPECT_EQ(reLseek, 10) << "> reLseek = " << reLseek;

    EXPECT_NE(close(fd), -1) << "> close errno = " << errno;
}

#if defined(LITE_FS_VFAT)
/**
 * @tc.number   SUB_KERNEL_FS_FCNTL_0300
 * @tc.name     basic function test : Use the fallocate function to pre-allocate large space to the file
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFallocate, Function | MediumTest | Level2)
{
    char writeBuf[] = "123456789ABCDE";

    unsigned int file_size;
    file_size = GetRandom(1024) * 102400 + 20;   // >20
    LOG("> GetRandom = %u", file_size);

    struct statvfs vfsBufOld;
    statvfs(".", &vfsBufOld);        // old system info.
    LOG("> vfsBufOld.f_bfree = %lu", vfsBufOld.f_bfree);

    int fd = open(FILE0, O_RDWR | O_CREAT | O_EXCL, 0777);
    ASSERT_NE(fd, -1) << "> open faild errno = " << errno;
    EXPECT_NE(write(fd, writeBuf, sizeof(writeBuf)), -1) << "> write errno = " << errno;

    int reInt = fallocate(fd, FALLOC_FL_KEEP_SIZE, 0, file_size);
    EXPECT_EQ(reInt, 0) << "> fallocate errno = " << errno;
    EXPECT_NE(close(fd), -1) << "> close errno = " << errno;

    // expect not changed
    struct stat statbuf;
    EXPECT_NE(stat(FILE0, &statbuf), -1) << "> fstat errno = " << errno;
    EXPECT_TRUE(statbuf.st_size == sizeof(writeBuf));

    struct statvfs vfsBufNew;
    statvfs(".", &vfsBufNew);        // new system info.
    LOG("> vfsBufNew.f_bfree = %lu", vfsBufNew.f_bfree);

    int ret = strcmp(TOP_DIR, "sdcard");
    if(ret == 0) {
    int bNumExpect = file_size / vfsBufNew.f_bsize;
    int bNum = vfsBufOld.f_bfree - vfsBufNew.f_bfree;
    LOG("> bNumExpect = %d", bNumExpect);
    LOG("> bNum = %d", bNum);
    EXPECT_GE(bNum, bNumExpect);
    }
}
#endif

#if defined(LITE_FS_VFAT)
/**
 * @tc.number   SUB_KERNEL_FS_FCNTL_0310
 * @tc.name     basic function test : Use the fallocate function to pre-allocate a small space to the file,
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFallocateSmallSize, Function | MediumTest | Level2)
{
    char writeBuf[] = "123456789ABCDE";

    unsigned int file_size;
    file_size = GetRandom(1024) + 20;   // >20
    LOG("> GetRandom = %u", file_size);

    struct statvfs vfsBufOld;
    statvfs(".", &vfsBufOld);        // old system info.
    LOG("> vfsBufOld.f_bfree = %lu", vfsBufOld.f_bfree);

    int fd = open(FILE0, O_RDWR | O_CREAT | O_EXCL, 0777);
    ASSERT_NE(fd, -1) << "> open faild errno = " << errno;
    EXPECT_NE(write(fd, writeBuf, sizeof(writeBuf)), -1) << "> write errno = " << errno;

    int reInt = fallocate(fd, FALLOC_FL_KEEP_SIZE, 0, file_size);
    EXPECT_EQ(reInt, 0) << "> fallocate errno = " << errno;
    EXPECT_NE(close(fd), -1) << "> close errno = " << errno;

    // expect not changed
    struct stat statbuf;
    EXPECT_NE(stat(FILE0, &statbuf), -1) << "> fstat errno = " << errno;
    EXPECT_TRUE(statbuf.st_size == sizeof(writeBuf));

    struct statvfs vfsBufNew;
    statvfs(".", &vfsBufNew);        // new system info.
    LOG("> vfsBufNew.f_bfree = %lu", vfsBufNew.f_bfree);

    int ret = strcmp(TOP_DIR, "sdcard");
    if (ret == 0) {
    int bNumExpect = file_size / vfsBufNew.f_bsize;
    int bNum = vfsBufOld.f_bfree - vfsBufNew.f_bfree;
    LOG("> bNumExpect = %d", bNumExpect);
    LOG("> bNum = %d", bNum);
    EXPECT_GE(bNum, bNumExpect);
    }
}
#endif

/**
 * @tc.number   SUB_KERNEL_FS_FCNTL_0400
 * @tc.name     basic function test : test open with O_RDWR O_CREAT O_EXCL when the file does not exist
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testOpen, Function | MediumTest | Level2)
{
    int fd = open(FILE0, O_RDWR | O_CREAT | O_EXCL, 0777);
    EXPECT_NE(fd, -1) << "> open faild errno = " << errno;
    EXPECT_NE(close(fd), -1) << "> close errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_FCNTL_0410
 * @tc.name     basic function test : test open with error number EEXIST
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testOpenEexist, Function | MediumTest | Level2)
{
    int fd = 0;
    fd = creat(FILE0, 0777);
    EXPECT_NE(fd, -1) << "> creat faild errno = " << errno;
    EXPECT_NE(close(fd), -1) << "> close errno = " << errno;

    fd = open(FILE0, O_RDWR | O_CREAT | O_EXCL, 0777);
    EXPECT_EQ(fd, -1) << "> Should open failed";
    EXPECT_EQ(errno, EEXIST);
    close(fd);
}

/**
 * @tc.number   SUB_KERNEL_FS_FCNTL_0420
 * @tc.name     basic function test : test open with error number EISDIR
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testOpenEisdir, Function | MediumTest | Level2)
{
    int fd = 0;
    CreateTestFolder();

    fd = open(DIR0, O_RDWR, 0777);
    if (fd != -1) {
    EXPECT_EQ(errno, EISDIR);
    close(fd);
    }
}

/**
 * @tc.number   SUB_KERNEL_FS_FCNTL_0430
 * @tc.name     basic function test : test open with error number ENOENT
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testOpenEnoent, Function | MediumTest | Level2)
{
    int fd = 0;

    fd = open(FILE0, O_RDWR, 0777);
    EXPECT_EQ(fd, -1) << "> Should open failed";
    EXPECT_EQ(errno, ENOENT);
    close(fd);
}

/**
 * @tc.number   SUB_KERNEL_FS_FCNTL_0440
 * @tc.name     basic function test : test open with error number ENAMETOOLONG
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testOpenEnametoolong, Function | MediumTest | Level2)
{
    int fd = 0;
    const char *fileName = "12345678901234567890123456789012345678901234567890\
                            12345678901234567890123456789012345678901234567890\
                            12345678901234567890123456789012345678901234567890\
                            12345678901234567890123456789012345678901234567890\
                            12345678901234567890123456789012345678901234567890\
                            12345678901234567890123456789012345678901234567890\
                            12345678901234567890123456789012345678901234567890\
                            12345678901234567890123456789012345678901234567890";

    fd = open(fileName, O_RDWR | O_CREAT | O_EXCL, 0777);
    EXPECT_EQ(fd, -1) << "> Should open failed";
    EXPECT_EQ(errno, ENAMETOOLONG);
    close(fd);
}