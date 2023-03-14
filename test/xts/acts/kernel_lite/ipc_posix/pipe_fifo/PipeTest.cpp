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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <gtest/gtest.h>
#include "utils.h"
#include "log.h"
#include "KernelConstants.h"
#include <securec.h>

using namespace testing::ext;

class PipeTest : public::testing::Test {
};

/**
 * @tc.number   SUB_KERNEL_IPC_PIPE_0100
 * @tc.name     basic function test
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PipeTest, testHelloWorld, Function | MediumTest | Level0)
{
    int fd[2];
    char readBuffer[100];
    int ret;

    ret = pipe(fd);
    ASSERT_EQ(ret, 0) << "> parent: pipe errno = " << errno;

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent: fork errno = " << errno;
    if (pid == 0) {
        close(fd[1]);
        if (read(fd[0], readBuffer, 20) == -1) {
            close(fd[0]);
            exit(1);
        }
        if (strncmp(readBuffer, "hello world", sizeof("hello world")) != 0) {
            close(fd[0]);
            exit(1);
        }
        close(fd[0]);
        exit(0);
    }

    // parent
    close(fd[0]);
    ret = write(fd[1], "hello world", sizeof("hello world"));
    EXPECT_NE(ret, -1) << "> write errno = " << errno;
    close(fd[1]);

    Msleep(100);
    WaitProcExitedOK(pid);
}

/**
 * @tc.number   SUB_KERNEL_IPC_PIPE_0200
 * @tc.name     O_NONBLOCK read test
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PipeTest, tesPipeNonblack, Function | MediumTest | Level1)
{
    int fd[2];
    int ret = pipe(fd);
    ASSERT_EQ(ret, 0) << "> parent: pipe error ret = %d" << ret;
    printf("fd %d, %d\n", fd[0], fd[1]);
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent: fork errno = " << errno;
    if (pid == 0) {     // child --- O_NONBLOCK R test, should be failed
        char readBuffer[100];
        close(fd[1]);
        if (fcntl(fd[0], F_SETFL, O_NONBLOCK) == -1) {
            LOG("> child fcntl errno = %d", errno);
        }
        ret = read(fd[0], readBuffer, 20);
        if ((ret != -1) || (errno != EAGAIN)) {
            LOG("> child : O_NONBLOCK R testfaild");
            close(fd[0]);
            exit(1);
        }
        Msleep(100);
        close(fd[0]);
        printf("1234 \n");
        exit(0);
    }
    // parent --- use O_NONBLOCK W test
    close(fd[0]);
    Msleep(50);
    close(fd[1]);

    Msleep(150);
    WaitProcExitedOK(pid);
    printf("5678 \n");
}

/**
 * @tc.number   SUB_KERNEL_IPC_PIPE_0300
 * @tc.name     pipe buffer teset
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PipeTest, testPipeBuf, Function | MediumTest | Level3)
{
    const int arrSize = MAX_PIPE_BUFFER + 1000;
    int fd[2];
    int tmpInt;
    int memRet = -1;
    char testBuffer[arrSize];
    memRet = memset_s(testBuffer, sizeof(testBuffer), '1', sizeof(testBuffer));
    EXPECT_EQ(0, memRet);
    printf("abcd \n");
    tmpInt = pipe(fd);
    ASSERT_EQ(tmpInt, 0) << "> parent: Create Pipe Error! ";
    printf("fd %d, %d\n", fd[0], fd[1]);

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent: error : fork";
    if (pid == 0) {
        char readBuffer[arrSize];
        memRet = memset_s(readBuffer, sizeof(readBuffer), 0, sizeof(readBuffer));
        EXPECT_EQ(0, memRet);
        close(fd[1]);

        Msleep(60);
        if (fcntl(fd[0], F_SETFL, O_NONBLOCK) == -1) {
            LOG("> child fcntl errno = %d", errno);
        }
        tmpInt = read(fd[0], readBuffer, arrSize);
        if (tmpInt != MAX_PIPE_BUFFER) {
            LOG("> child: error : read MAX_PIPE_BUFFER = %d, not %d", tmpInt, MAX_PIPE_BUFFER);
            close(fd[0]);
            exit(1);
        }
        if (strncmp(testBuffer, readBuffer, MAX_PIPE_BUFFER) != 0) {
            close(fd[0]);
            exit(1);
        }
        close(fd[0]);
        exit(0);
    }

    // parent
    char writeBuffer[arrSize];
    memRet = memset_s(writeBuffer, sizeof(writeBuffer), '1', sizeof(writeBuffer));
    EXPECT_EQ(0, memRet);
    close(fd[0]);

    Msleep(30);
    EXPECT_NE(fcntl(fd[1], F_SETFL, O_NONBLOCK), -1) << "> fcntl errno = " << errno;
    printf("efg \n");
    tmpInt = write(fd[1], writeBuffer, arrSize);
    LOG("> parent: write num = %d", tmpInt);
    EXPECT_NE(tmpInt, -1) << "> parent: error : write num = "<< tmpInt;
    close(fd[1]);

    Msleep(100);
    WaitProcExitedOK(pid);
}

/**
 * @tc.number   SUB_KERNEL_IPC_PIPE_0400
 * @tc.name     block pipe teset: MAX_PIPE_BUFFER
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PipeTest, testPipeBlock, Function | MediumTest | Level2)
{
    const int arrSize = MAX_PIPE_BUFFER + 1000;
    int fd[2];
    int ret;
    char testBuffer[arrSize];
    int memRet = -1;
    memRet = memset_s(testBuffer, sizeof(testBuffer), '1', sizeof(testBuffer));
    EXPECT_EQ(0, memRet);
    ret = pipe(fd);
    ASSERT_EQ(ret, 0) << "> parent: Create Pipe Error! ";

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent: error : fork";
    if (pid == 0) {
        char readBuffer[arrSize];
        memRet = memset_s(readBuffer, sizeof(readBuffer), 0, sizeof(readBuffer));
        EXPECT_EQ(0, memRet);
        close(fd[1]);

        Msleep(60);
        ret = read(fd[0], readBuffer, arrSize);
        if (ret != arrSize) {
            LOG("> child: error : read bytes = %d, not %d", ret, arrSize);
            close(fd[0]);
            exit(1);
        }
        if (strncmp(testBuffer, readBuffer, arrSize) != 0) {
            close(fd[0]);
            exit(1);
        }
        close(fd[0]);
        exit(0);
    }

    // parent
    char writeBuffer[arrSize];
    memRet = memset_s(writeBuffer, sizeof(writeBuffer), '1', sizeof(writeBuffer));
    EXPECT_EQ(0, memRet);
    close(fd[0]);

    Msleep(30);
    ret = write(fd[1], writeBuffer, arrSize);
    LOG("> parent: write num = %d", ret);
    EXPECT_NE(ret, -1) << "> parent: error : write num = "<< ret;
    close(fd[1]);

    Msleep(100);
    WaitProcExitedOK(pid);
}

/**
 * @tc.number   SUB_KERNEL_IPC_PIPE_0600
 * @tc.name     test pipe-ipc between brother
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PipeTest, testBrotherHelloWorld, Function | MediumTest | Level1)
{
    int fd[2];
    char readBuffer[100];
    int ret;

    ret = pipe(fd);
    ASSERT_EQ(ret, 0) << "> parent: pipe errno = " << errno;

    pid_t pidChild1 = fork();
    ASSERT_TRUE(pidChild1 >= 0) << "> parent: fork errno = " << errno;
    if (pidChild1 == 0) {
        close(fd[1]);
        if (read(fd[0], readBuffer, 20) == -1) {
            close(fd[0]);
            exit(1);
        }
        if (strncmp(readBuffer, "hello world", sizeof("hello world")) != 0) {
            close(fd[0]);
            exit(1);
        }
        close(fd[0]);
        exit(0);
    }

    pid_t pidChild2 = fork();
    ASSERT_TRUE(pidChild2 >= 0) << "> parent: fork errno = " << errno;
    if (pidChild2 == 0) {
        close(fd[0]);
        ret = write(fd[1], "hello world", sizeof("hello world"));
        if (ret == -1) {
            LOG("> child write errno = %d", errno);
        }
        close(fd[1]);
        exit(0);
    }

    close(fd[0]);
    close(fd[1]);

    Msleep(100);
    WaitProcExitedOK(pidChild1);
    WaitProcExitedOK(pidChild2);
}
