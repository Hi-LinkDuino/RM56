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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>
#include <gtest/gtest.h>
#include "log.h"
#include "utils.h"
#include "KernelConstants.h"
#include <securec.h>

using namespace testing::ext;

#define FIFO_PATH "/dev/xtsTestFifo"

class FifoTest : public::testing::Test {
protected:
    void SetUp()
    {
        remove(FIFO_PATH);
    }
    void TearDown()
    {
        remove(FIFO_PATH);
    }
};

/**
 * @tc.number   SUB_KERNEL_IPC_FIFO_0100
 * @tc.name     basic function test : hello world
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FifoTest, testHelloWorld, Function | MediumTest | Level0)
{
    char buffer[80];
    int fd;
    char sentence[] = "Hello World";

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent: fork : error";
    if (pid == 0) {
        Msleep(20);
        fd = open(FIFO_PATH, O_WRONLY, S_IRUSR|S_IWUSR);
        write(fd, sentence, sizeof(sentence));
        LOG("> child: write: %s", sentence);
        close(fd);
        exit(0);
    }
    // parent
    int ret = mkfifo(FIFO_PATH, 0666);
    EXPECT_EQ(ret, 0) << "> parent: mkfifo errno = " << errno;
    fd = open(FIFO_PATH, O_RDONLY, S_IRUSR|S_IWUSR);
    EXPECT_NE(fd, -1) << "> open faild errno = " << errno;
    EXPECT_NE(read(fd, buffer, sizeof(buffer)), -1) << "> read errno = " << errno;
    EXPECT_STREQ(sentence, buffer) << "> parent: read = " << buffer;
    LOG("> parent: read: %s", buffer);
    close(fd);

    Msleep(20);
    WaitProcExitedOK(pid);
}

/**
 * @tc.number   SUB_KERNEL_IPC_FIFO_0700
 * @tc.name     mkfifo Duplicate creation and delete
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FifoTest, testFifoAddDelete, Function | MediumTest | Level0)
{
    char fifoPath[50];
    const int loopNum = 32;

    for (int i = 0; i < loopNum; i++) {
        sprintf(fifoPath, "/dev/xtsTestFifo_%d", i);
        remove(fifoPath);

        LOG("> Create fifo %d", i);
        EXPECT_EQ(mkfifo(fifoPath, 0666), 0) << "> mkfifo errno = " << errno;

        LOG("> Delete fifo %d", i);
        EXPECT_NE(remove(fifoPath), -1) << "> remove errno = " << errno;
    }
}

/**
 * @tc.number   SUB_KERNEL_IPC_FIFO_0800
 * @tc.name     test O_NONBLOCK FIFO
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FifoTest, testFifoNonblack, Function | MediumTest | Level1)
{
    const int arrSize = MAX_PIPE_BUFFER + 10;
    int fd = -1;
    int tmpInt;
    int memRet = -1;
    char testBuffer[arrSize];
    memRet = memset_s(testBuffer, sizeof(testBuffer), '1', sizeof(testBuffer));
    EXPECT_EQ(0, memRet);

    int ret = mkfifo(FIFO_PATH, 0666);
    EXPECT_EQ(ret, 0) << "> parent: mkfifo errno = " << errno;

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent : fork : error";
    if (pid == 0) {
        char readBuffer[arrSize];
        memRet = memset_s(readBuffer, sizeof(readBuffer), 0, sizeof(readBuffer));
        EXPECT_EQ(0, memRet);
        fd = open(FIFO_PATH, O_RDONLY, S_IRUSR|S_IWUSR);
        if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
            LOG("> fcntl errno = %d", errno);
        }

        Msleep(150);
        LOG("> child read");
        tmpInt = read(fd, readBuffer, arrSize);
        if (tmpInt != MAX_PIPE_BUFFER) {
            LOG("> child: tmpInt = %d", tmpInt);
            LOG("> child: errno = %d", errno);
            close(fd);
            exit(1);
        }
        if (strncmp(testBuffer, readBuffer, MAX_PIPE_BUFFER) != 0) {
            close(fd);
            exit(1);
        }
        close(fd);
        exit(0);
    }
    // parent
    char writeBuffer[arrSize];
    memRet = memset_s(writeBuffer, sizeof(writeBuffer), '1', sizeof(writeBuffer));
    EXPECT_EQ(0, memRet);
    fd = open(FIFO_PATH, O_WRONLY, S_IRUSR|S_IWUSR);
    EXPECT_NE(fd, -1) << "> open faild errno = " << errno;
    EXPECT_NE(fcntl(fd, F_SETFL, O_NONBLOCK), -1) << "> fcntl errno = " << errno;

    Msleep(50);
    LOG("> parent write");
    tmpInt = write(fd, writeBuffer, arrSize);
    EXPECT_EQ(tmpInt, MAX_PIPE_BUFFER) << "> parent: errno = " <<errno;
    LOG("> parent: write num = %d", tmpInt);
    close(fd);

    Msleep(50);
    WaitProcExitedOK(pid);
}

/**
 * @tc.number   SUB_KERNEL_IPC_FIFO_0900
 * @tc.name     test BLOCK FIFO
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FifoTest, testFifoBlock, Function | MediumTest | Level1)
{
    const int arrSize = MAX_PIPE_BUFFER + 1000;
    int fd = -1;
    int tmpInt;
    char testBuffer[arrSize];
    int memRet = -1;
    memRet = memset_s(testBuffer, sizeof(testBuffer), '1', sizeof(testBuffer));
    EXPECT_EQ(0, memRet);

    int ret = mkfifo(FIFO_PATH, 0666);
    EXPECT_EQ(ret, 0) << "> parent: mkfifo errno = " << errno;

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent : fork : error";
    if (pid == 0) {
        char readBuffer[arrSize];
        memRet = memset_s(readBuffer, sizeof(readBuffer), 0, sizeof(readBuffer));
        EXPECT_EQ(0, memRet);
        fd = open(FIFO_PATH, O_RDONLY);

        Msleep(60);
        tmpInt = read(fd, readBuffer, arrSize);
        if (tmpInt != arrSize) {
            LOG("> child: error : read %d bytes", tmpInt);
            LOG("> child: errno = %d", errno);
            close(fd);
            exit(1);
        }
        if (strncmp(testBuffer, readBuffer, MAX_PIPE_BUFFER) != 0) {
            close(fd);
            exit(1);
        }
        close(fd);
        exit(0);
    }
    // parent
    char writeBuffer[arrSize];
    memRet = memset_s(writeBuffer, sizeof(writeBuffer), '1', sizeof(writeBuffer));
    EXPECT_EQ(0, memRet);
    fd = open(FIFO_PATH, O_WRONLY, S_IRUSR|S_IWUSR);
    EXPECT_NE(fd, -1) << "> open faild errno = " << errno;

    Msleep(30);
    tmpInt = write(fd, writeBuffer, arrSize);
    EXPECT_EQ(tmpInt, arrSize) << "> parent: errno = " <<errno;
    LOG("> parent: write : = %d bytes", tmpInt);
    close(fd);

    Msleep(50);
    WaitProcExitedOK(pid);
}
