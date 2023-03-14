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
 * @tc.number   SUB_KERNEL_FS_STDIO_0100
 * @tc.name     basic function test : read and write with stream
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFILE, Function | MediumTest | Level2)
{
    FILE *fp = nullptr;
    int fd = 0;
    char writeBuf[] = "this is a file";
    char readBuf[20] = {0};

    // write
    fd = creat(FILE0, 0777);
    EXPECT_NE(fd, -1) << "> creat faild errno = " << errno;
    EXPECT_NE(close(fd), -1) << "> close errno = " << errno;
    fp = fopen(FILE0, "w+");
    ASSERT_NE(fp, nullptr) << "> fopen errno = " << errno;
    EXPECT_EQ(fwrite(writeBuf, sizeof(writeBuf), 1, fp), 1) << "> fwrite errno = " << errno;
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" << errno;

    // read
    fp = fopen(FILE0, "r+");
    ASSERT_NE(fp, nullptr) << "> fopen errno = " << errno;
    EXPECT_NE(fread(readBuf, sizeof(writeBuf), 1, fp), 0) << "> fread errno = " << errno;
    EXPECT_STREQ(writeBuf, readBuf) << "> writeBuf = " << writeBuf << "\n> readBuf = " << readBuf;
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_STDIO_0200
 * @tc.name     basic function test : Use the feof function to determine the end identifier of the file
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFeof, Function | MediumTest | Level3)
{
    FILE *fp = nullptr;
    int fd = 0;
    char writeBuf[] = "this is a file";
    char readBuf[20] = {0};

    // write
    fd = creat(FILE0, 0777);
    EXPECT_NE(fd, -1) << "> creat faild errno = " << errno;
    WriteCloseTest(fd);

    // read
    fp = fopen(FILE0, "r+");
    ASSERT_NE(fp, nullptr) << "> fopen errno = " << errno;

    EXPECT_EQ(fseeko(fp, 0, SEEK_SET), 0) << "> fseeko errno = " << errno;
    fgetc(fp);
    EXPECT_EQ(feof(fp), 0) << "> file should not be end!";      // check end
    EXPECT_EQ(fseeko(fp, 0, SEEK_SET), 0) << "> fseeko errno = " << errno;

    EXPECT_NE(fread(readBuf, sizeof(writeBuf), 1, fp), 0) << "> fread errno = " << errno;
    EXPECT_STREQ(writeBuf, readBuf) << "> writeBuf = " << writeBuf << "\n> readBuf = " << readBuf;
    fgetc(fp);
    EXPECT_NE(feof(fp), 0) << "> file should be end!";          // check end
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_STDIO_0300
 * @tc.name     basic function test : Use fseek to set the stream pointer position with SEEK_SET
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFseek, Function | MediumTest | Level3)
{
    FILE *fp = nullptr;
    int fd = 0;
    char writeBuf[] = "this is a file";
    char readBuf[20] = {0};

    // write
    fd = creat(FILE0, 0777);
    EXPECT_NE(fd, -1) << "> creat faild errno = " << errno;
    WriteCloseTest(fd);

    // read
    fp = fopen(FILE0, "r+");
    ASSERT_NE(fp, nullptr) << "> fopen errno = " << errno;

    fpos_t offset;
    EXPECT_EQ(fgetpos(fp, &offset), 0) << "> fgetpos errno = " << errno;

    EXPECT_EQ(fseek(fp, 2, SEEK_SET), 0) << "> fseek errno = " << errno;
    EXPECT_EQ(ftell(fp), 2) << " errno = " << errno;

    EXPECT_EQ(fsetpos(fp, &offset), 0) << "> fsetpos errno = " << errno;
    EXPECT_NE(fread(readBuf, sizeof(writeBuf), 1, fp), 0) << "> fread errno = " << errno;
    EXPECT_STREQ(writeBuf, readBuf) << "> writeBuf = " << writeBuf << "\n> readBuf = " << readBuf;
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_STDIO_0310
 * @tc.name     basic function test : Use fseek to set the stream pointer position with SEEK_CUR
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFseekSeekCur, Function | MediumTest | Level3)
{
    FILE *fp = nullptr;
    int fd = 0;
    char writeBuf[] = "this is a file";
    char readBuf[20] = {0};

    // write
    fd = creat(FILE0, 0777);
    EXPECT_NE(fd, -1) << "> creat faild errno = " << errno;
    WriteCloseTest(fd);

    // read
    fp = fopen(FILE0, "r+");
    ASSERT_NE(fp, nullptr) << "> fopen errno = " << errno;

    fpos_t offset;
    EXPECT_EQ(fgetpos(fp, &offset), 0) << "> fgetpos errno = " << errno;

    EXPECT_EQ(fseek(fp, 2, SEEK_CUR), 0) << "> fseek errno = " << errno;
    EXPECT_EQ(ftell(fp), 2) << " errno = " << errno;

    EXPECT_EQ(fsetpos(fp, &offset), 0) << "> fsetpos errno = " << errno;
    EXPECT_NE(fread(readBuf, sizeof(writeBuf), 1, fp), 0) << "> fread errno = " << errno;
    EXPECT_STREQ(writeBuf, readBuf) << "> writeBuf = " << writeBuf << "\n> readBuf = " << readBuf;
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_STDIO_0320
 * @tc.name     basic function test : Use fseek to set the stream pointer position with SEEK_END
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFseekSeekEnd, Function | MediumTest | Level3)
{
    FILE *fp = nullptr;
    int fd = 0;
    char writeBuf[] = "this is a file";
    char readBuf[20] = {0};

    // write
    fd = creat(FILE0, 0777);
    EXPECT_NE(fd, -1) << "> creat faild errno = " << errno;
    WriteCloseTest(fd);

    // read
    fp = fopen(FILE0, "r+");
    ASSERT_NE(fp, nullptr) << "> fopen errno = " << errno;

    fpos_t offset;
    EXPECT_EQ(fgetpos(fp, &offset), 0) << "> fgetpos errno = " << errno;

    EXPECT_EQ(fseek(fp, 0, SEEK_END), 0) << "> fseek errno = " << errno;
    EXPECT_EQ(ftell(fp), sizeof(writeBuf)) << " errno = " << errno;

    EXPECT_EQ(fsetpos(fp, &offset), 0) << "> fsetpos errno = " << errno;
    EXPECT_NE(fread(readBuf, sizeof(writeBuf), 1, fp), 0) << "> fread errno = " << errno;
    EXPECT_STREQ(writeBuf, readBuf) << "> writeBuf = " << writeBuf << "\n> readBuf = " << readBuf;
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_STDIO_0400
 * @tc.name     basic function test : Use fseeko to set the stream pointer position with SEEK_SET
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFseeko, Function | MediumTest | Level3)
{
    FILE *fp = nullptr;
    int fd = 0;
    char writeBuf[] = "this is a file";
    char readBuf[20] = {0};

    // write
    fd = creat(FILE0, 0777);
    EXPECT_NE(fd, -1) << "> creat faild errno = " << errno;
    WriteCloseTest(fd);

    // read
    fp = fopen(FILE0, "r+");
    ASSERT_NE(fp, nullptr) << "> fopen errno = " << errno;

    EXPECT_EQ(fseeko(fp, 2, SEEK_SET), 0) << "> fseeko errno = " << errno;
    EXPECT_EQ(ftello(fp), 2) << " errno = " << errno;

    EXPECT_EQ(fseeko(fp, 0, SEEK_SET), 0) << "> fseeko errno = " << errno;
    EXPECT_EQ(ftello(fp), 0) << " errno = " << errno;
    EXPECT_NE(fread(readBuf, sizeof(writeBuf), 1, fp), 0) << "> fread errno = " << errno;
    EXPECT_STREQ(writeBuf, readBuf) << "> writeBuf = " << writeBuf << "\n> readBuf = " << readBuf;
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_STDIO_0410
 * @tc.name     basic function test : Use fseeko to set the stream pointer position with SEEK_CUR
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFseekoSeekCur, Function | MediumTest | Level3)
{
    FILE *fp = nullptr;
    int fd = 0;
    char writeBuf[] = "this is a file";
    char readBuf[20] = {0};

    // write
    fd = creat(FILE0, 0777);
    EXPECT_NE(fd, -1) << "> creat faild errno = " << errno;
    WriteCloseTest(fd);

    // read
    fp = fopen(FILE0, "r+");
    ASSERT_NE(fp, nullptr) << "> fopen errno = " << errno;

    EXPECT_EQ(fseeko(fp, 2, SEEK_CUR), 0) << "> fseeko errno = " << errno;
    EXPECT_EQ(ftello(fp), 2) << " errno = " << errno;

    EXPECT_EQ(fseeko(fp, 0, SEEK_SET), 0) << "> fseeko errno = " << errno;
    EXPECT_EQ(ftello(fp), 0) << " errno = " << errno;
    EXPECT_NE(fread(readBuf, sizeof(writeBuf), 1, fp), 0) << "> fread errno = " << errno;
    EXPECT_STREQ(writeBuf, readBuf) << "> writeBuf = " << writeBuf << "\n> readBuf = " << readBuf;
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_STDIO_0420
 * @tc.name     basic function test : Use fseeko to set the stream pointer position with SEEK_END
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFseekoSeekEnd, Function | MediumTest | Level3)
{
    FILE *fp = nullptr;
    int fd = 0;
    char writeBuf[] = "this is a file";
    char readBuf[20] = {0};

    // write
    fd = creat(FILE0, 0777);
    EXPECT_NE(fd, -1) << "> creat faild errno = " << errno;
    WriteCloseTest(fd);

    // read
    fp = fopen(FILE0, "r+");
    ASSERT_NE(fp, nullptr) << "> fopen errno = " << errno;

    EXPECT_EQ(fseeko(fp, 0, SEEK_END), 0) << "> fseeko errno = " << errno;
    EXPECT_EQ(ftello(fp), sizeof(writeBuf)) << " errno = " << errno;

    EXPECT_EQ(fseeko(fp, 0, SEEK_SET), 0) << "> fseeko errno = " << errno;
    EXPECT_EQ(ftello(fp), 0) << " errno = " << errno;
    EXPECT_NE(fread(readBuf, sizeof(writeBuf), 1, fp), 0) << "> fread errno = " << errno;
    EXPECT_STREQ(writeBuf, readBuf) << "> writeBuf = " << writeBuf << "\n> readBuf = " << readBuf;
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" << errno;
}

void *ChildWrite(void *p)
{
    FILE *fp = (FILE*)p;
    flockfile(fp);
    size_t re0 = fwrite("this is ", sizeof("this is"), 1, fp);
    EXPECT_EQ(re0, 1) << "fwrite errno = " << errno;
    Msleep(FOURTY_MS);
    size_t re1 = fwrite("a file", sizeof("a file"), 1, fp);
    EXPECT_EQ(re1, 1) << "fwrite errno = " << errno;
    funlockfile(fp);
    LOG("> ChildWrite over");
    return nullptr;
}

/**
 * @tc.number   SUB_KERNEL_FS_STDIO_0500
 * @tc.name     basic function test : Use the funlockfile function to unlock the file
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFunlockfile, Function | MediumTest | Level3)
{
    const char filePath[] = TOP_DIR "/" DIR0 "/" DIR0_FILE0;
    char readBuf[20];
    char writeBuf[] = "this is a file";

    CreateTestFolder();
    FILE *fp = fopen(filePath, "r+");
    ASSERT_NE(fp, nullptr) << "> fopen errno = " << errno;

    pthread_t tid;
    int reInt = pthread_create(&tid, nullptr, ChildWrite, (void*)fp);
    ASSERT_EQ(reInt, 0) << "> pthread_create errno, reInt = " << reInt;

    Msleep(20);
    flockfile(fp);
    LOG("> childRead over");
    EXPECT_EQ(fseek(fp, 0, SEEK_SET), 0) << "> fseek errno = " << errno;
    EXPECT_NE(fread(readBuf, sizeof(writeBuf), 1, fp), 0) << "> fread error";
    EXPECT_STREQ(writeBuf, readBuf);
    funlockfile(fp);

    int ret = pthread_join(tid, nullptr);
    EXPECT_EQ(ret, 0) << "pthread_join failed, errno=" << ret;
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_STDIO_0600
 * @tc.name     basic function test : Use the fileno function to return the file descriptor of the stream, test write
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFileno, Function | MediumTest | Level2)
{
    const char filePath[] = TOP_DIR "/" FILE0;
    FILE *fp = nullptr;
    int fd = 0;

    // write
    fd = creat(filePath, 0777);
    ASSERT_NE(fd, -1) << "> creat errno = " << errno;
    ASSERT_NE(close(fd), -1) << "> close errno = " << errno;

    fp = fopen(filePath, "w+");
    ASSERT_NE(fp, nullptr) << "> fopen errno = " << errno;
    fd = fileno(fp);
    EXPECT_NE(fd, -1) << "> fileno errno = " << errno;
    WriteCloseTest(fd);

    // read
    fd = open(filePath, O_RDWR);
    EXPECT_NE(fd, -1) << "> open errno = " << errno;
    ReadCloseTest(fd);
}

/**
 * @tc.number   SUB_KERNEL_FS_STDIO_0610
 * @tc.name     basic function test : Use the fileno function to return the file descriptor of the stream, test read
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFileno1, Function | MediumTest | Level2)
{
    const char filePath[] = TOP_DIR "/" FILE0;
    FILE *fp = nullptr;
    int fd = 0;

    // write
    fd = creat(filePath, 0777);
    EXPECT_NE(fd, -1) << "> creat faild errno = " << errno;
    WriteCloseTest(fd);

    // read
    fp = fopen(filePath, "r+");
    ASSERT_NE(fp, nullptr) << "> fopen errno = " << errno;
    fd = fileno(fp);
    EXPECT_NE(fd, -1) << "> fileno errno = " << errno;

    ReadCloseTest(fd);
}
#endif
/**
 * @tc.number   SUB_KERNEL_FS_STDIO_0700
 * @tc.name     basic function test : Use the rename function to rename files.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testRename, Function | MediumTest | Level3)
{
    int fd = 0;
    const char *newFileName = "FILE_NEW";
    fd = creat(FILE0, 0777);
    EXPECT_NE(fd, -1) << "> creat faild errno = " << errno;
    EXPECT_NE(close(fd), -1) << "> close errno = " << errno;

    EXPECT_NE(rename(FILE0, newFileName), -1) << "> rename errno = " << errno;
    EXPECT_NE(unlink(newFileName), -1) << "> unlink errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_FS_STDIO_0710
 * @tc.name     basic function test : Use the rename function to rename directories.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testRenameDir, Function | MediumTest | Level3)
{
    const char *newDirName = "DIR_NEW";
    EXPECT_NE(mkdir(DIR0, 0777), -1) << "> mkdir errno = " << errno;

    EXPECT_NE(rename(DIR0, newDirName), -1) << "> rename errno = " << errno;
    EXPECT_NE(rmdir(newDirName), -1) << "> rmdir errno = " << errno;
}
#if 0
/**
 * @tc.number   SUB_KERNEL_FS_STDIO_0800
 * @tc.name     basic function test : Use the fflush function to refresh stream
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FileSystemTest, testFflush, Function | MediumTest | Level3)
{
    const char filePath[] = TOP_DIR "/" DIR0 "/" DIR0_FILE0;
    char writeBuf[] = "0123456789ABCDE";
    char readBuf[50] = {0};
    CreateTestFolder();
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent: fork errno = " << errno;
    if (pid == 0) {
        int exitCode = 0;
        FILE *fp = fopen(filePath, "a+");
        if (fp == nullptr) {
            LOG("fp is null");
            exit(1);
        }
        setvbuf(fp, nullptr, _IOFBF, 1024);
        int reInt = fwrite(writeBuf, sizeof(writeBuf), 1, fp);
        if (reInt != 1) {
            LOG("fwrite return reInt = %d, errno = %d", reInt, errno);
            exitCode = 1;
        }
        Msleep(100);
        if (fflush(fp) == -1){
            LOG("fflush errno = %d", errno);
            exitCode = 1;
        }
        Msleep(100);
        if (fclose(fp) == -1){
            LOG("fclose errno = %d", errno);
            exitCode = 1;
        }
        exit(exitCode);
    }
    Msleep(50);
    FILE *fp = fopen(filePath, "r");
    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(fseek(fp, 0, SEEK_END), 0) << "> fseek errno = " << errno;
    EXPECT_EQ(ftell(fp), 0);
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" << errno;

    Msleep(100);
    fp = fopen(filePath, "r");
    ASSERT_NE(fp, nullptr);
    EXPECT_NE(fread(readBuf, sizeof(writeBuf), 1, fp), 0) << "> fread errno = " << errno;
    EXPECT_STREQ(readBuf, writeBuf);
    LOG("> readBuf  = %s", readBuf);
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" << errno;

    Msleep(100);
    AssertProcExitedOK(pid);
}
#endif
