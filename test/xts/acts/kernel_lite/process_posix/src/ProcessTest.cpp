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
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/shm.h>
#include <assert.h>
#include <unistd.h>
#include <gtest/gtest.h>
#include "utils.h"
#include "mt_utils.h"
#include "log.h"
#include "KernelConstants.h"

using namespace testing::ext;

class ProcessTest : public::testing::Test {
};

/**
 * @tc.number   SUB_KERNEL_PROCESS_LINE_BIGEXIT_0100
 * @tc.name     Basic test about _Exit
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessTest, testLineBigExit, Function | MediumTest | Level2)
{
    int exitCode;
    pid_t pid;
    int reInt[4] = {0, 1, 100, 255};

    for (int i = 0; i < sizeof(reInt) / sizeof(int); i++) {
        pid = fork();
        ASSERT_TRUE(pid >= 0) << "> parent: fork errno = " << errno;
        if (pid == 0) {
            _Exit(reInt[i]);
        }
        Msleep(50);
        exitCode = 0;
        ASSERT_EQ(CheckProcStatus(pid, &exitCode, 0), 1);
        ASSERT_EQ(exitCode, reInt[i]);
    }
}

/**
 * @tc.number   SUB_KERNEL_PROCESS_LINE_BIGEXIT_0200
 * @tc.name     Test _Exit about IO flush
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessTest, testLineBigExitFlush, Function | MediumTest | Level3)
{
    const char* testFile = "TEST_FILE.txt";
    char writeBuf[] = "this is a file";
    char readBuf[20] = {0};
    pid_t pid;

    pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent: fork errno = " << errno;
    if (pid == 0) {

        // write
        FILE *fp = fopen(testFile, "w+");
        if (fp == nullptr) {
            LOG("> child fopen errno = %d", errno);
            _Exit(1);
        }
        fwrite(writeBuf, sizeof(writeBuf), 1, fp);
        _Exit(0);
    }

    WaitProcExitedOK(pid);

    // read
    FILE *fp = fopen(testFile, "r+");
    ASSERT_NE(fp, nullptr) << "> fopen errno = " << errno;
    EXPECT_EQ(fread(readBuf, sizeof(writeBuf), 1, fp), 0);
    EXPECT_STRNE(writeBuf, readBuf) << "> writeBuf = " << writeBuf\
                                    << "\n> readBuf = " << readBuf;
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" <<errno;
    remove(testFile);
}

int g_shmid0;

void AtexitCallback0(void)
{
    LOG("> AtexitCallback0");
    pid_t *shared = (int*)shmat(g_shmid0, nullptr, 0);
    if (shared == (void *)-1) {
        LOG("> AtexitCallback0 shmat errno = %d", errno);
        _Exit(1);
    }
    *shared = getppid();
    if ((shmdt(shared)) == -1) {
        LOG("> AtexitCallback0 shmdt errno = %d", errno);
        _Exit(1);
    }
    LOG("> 333");
}

/**
 * @tc.number   SUB_KERNEL_PROCESS_LINE_BIGEXIT_0300
 * @tc.name     Test _Exit about call functions registered with atexit
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessTest, testLineBigExitAtexit, Function | MediumTest | Level3)
{
    const int memSize = 1024;
    g_shmid0 = shmget(IPC_PRIVATE, memSize, 0666 | IPC_CREAT);
    ASSERT_NE(g_shmid0, -1) << "> parent: shmid errno = " << errno;
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent: fork errno = " << errno;
    if (pid == 0) {
        Msleep(20);
        if (atexit(AtexitCallback0) != 0) {
            _Exit(1);
        }
        LOG("> 222");
        _Exit(0);
    }
    pid_t *shared = (int*)shmat(g_shmid0, nullptr, 0);
    LOG("> 111");
    WaitProcExitedOK(pid);
    LOG("> 444");
    ASSERT_NE(shared, (void*)-1) << "> shmat errno = " << errno;
    EXPECT_NE(*shared, getpid());
    ASSERT_NE(shmdt(shared), -1) << "> parent: shmdt errno = " << errno;
    ASSERT_NE(shmctl(g_shmid0, IPC_RMID, nullptr), -1) << "> shmctl : IPC_RMID : errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_PROCESS_LINE_EXIT_0100
 * @tc.name     Basic test about _exit
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessTest, testLineExit, Function | MediumTest | Level2)
{
    int exitCode;
    pid_t pid;
    int reInt[4] = {0, 1, 100, 255};

    for (int i = 0; i < sizeof(reInt) / sizeof(int); i++) {
        pid = fork();
        ASSERT_TRUE(pid >= 0) << "> parent: fork errno = " << errno;
        if (pid == 0) {
            _exit(reInt[i]);
        }
        Msleep(50);
        exitCode = 0;
        ASSERT_EQ(CheckProcStatus(pid, &exitCode, 0), 1);
        ASSERT_EQ(exitCode, reInt[i]);
    }
}

/**
 * @tc.number   SUB_KERNEL_PROCESS_LINE_EXIT_0200
 * @tc.name     Test _exit about IO flush
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessTest, testLineExitFlush, Function | MediumTest | Level3)
{
    const char* testFile = "TEST_FILE.txt";
    char writeBuf[] = "this is a file";
    char readBuf[20] = {0};
    pid_t pid;

    pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent: fork errno = " << errno;
    if (pid == 0) {

        // write
        FILE *fp = fopen(testFile, "w+");
        if (fp == nullptr) {
            LOG("> child fopen errno = %d", errno);
            _exit(1);
        }
        fwrite(writeBuf, sizeof(writeBuf), 1, fp);
        _exit(0);
    }

    WaitProcExitedOK(pid);

    // read
    FILE *fp = fopen(testFile, "r+");
    ASSERT_NE(fp, nullptr) << "> fopen errno = " << errno;
    EXPECT_EQ(fread(readBuf, sizeof(writeBuf), 1, fp), 0);
    EXPECT_STRNE(writeBuf, readBuf) << "> writeBuf = " << writeBuf\
                                    << "\n> readBuf = " << readBuf;
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" <<errno;
    remove(testFile);
}

int g_shmid1;

void AtexitCallback1(void)
{
    LOG("> AtexitCallback0");
    pid_t *shared = (int*)shmat(g_shmid1, nullptr, 0);
    if (shared == (void *)-1) {
        LOG("> AtexitCallback0 shmat errno = %d", errno);
        _exit(1);
    }
    *shared = getppid();
    if ((shmdt(shared)) == -1) {
        LOG("> AtexitCallback0 shmdt errno = %d", errno);
        _exit(1);
    }
    LOG("> 333");
}

/**
 * @tc.number   SUB_KERNEL_PROCESS_LINE_EXIT_0300
 * @tc.name     Test _exit about call functions registered with atexit
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessTest, testLineExitAtexit, Function | MediumTest | Level3)
{
    const int memSize = 1024;
    g_shmid1 = shmget(IPC_PRIVATE, memSize, 0666 | IPC_CREAT);
    ASSERT_NE(g_shmid1, -1) << "> parent: shmid errno = " << errno;
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent: fork errno = " << errno;
    if (pid == 0) {
        Msleep(20);
        if (atexit(AtexitCallback1) != 0) {
            _exit(1);
        }
        LOG("> 222");
        _exit(0);
    }
    pid_t *shared = (int*)shmat(g_shmid1, nullptr, 0);
    LOG("> 111");
    WaitProcExitedOK(pid);
    LOG("> 444");
    ASSERT_NE(shared, (void*)-1) << "> shmat errno = " << errno;
    EXPECT_NE(*shared, getpid());
    ASSERT_NE(shmdt(shared), -1) << "> parent: shmdt errno = " << errno;
    ASSERT_NE(shmctl(g_shmid1, IPC_RMID, nullptr), -1) << "> shmctl : IPC_RMID : errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_PROCESS_EXIT_0100
 * @tc.name     Basic test about exit
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessTest, testExit, Function | MediumTest | Level2)
{
    int exitCode;
    pid_t pid;
    int reInt[4] = {0, 1, 100, 255};

    for (int i = 0; i < sizeof(reInt) / sizeof(int); i++) {
        pid = fork();
        ASSERT_TRUE(pid >= 0) << "> parent: fork errno = " << errno;
        if (pid == 0) {
            exit(reInt[i]);
        }
        Msleep(50);
        exitCode = 0;
        ASSERT_EQ(CheckProcStatus(pid, &exitCode, 0), 1);
        ASSERT_EQ(exitCode, reInt[i]);
    }
}
#if 0
/**
 * @tc.number   SUB_KERNEL_PROCESS_EXIT_0200
 * @tc.name     Test exit about IO flush
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessTest, testExitFlush, Function | MediumTest | Level3)
{
    const char* testFile = "TEST_FILE.txt";
    char writeBuf[] = "this is a file";
    char readBuf[20] = {0};
    pid_t pid;

    pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent: fork errno = " << errno;
    if (pid == 0) {

        // write
        FILE *fp = fopen(testFile, "w+");
        if (fp == nullptr) {
            LOG("> child fopen errno = %d", errno);
            exit(1);
        }
        fwrite(writeBuf, sizeof(writeBuf), 1, fp);
        exit(0);
    }

    WaitProcExitedOK(pid);

    // read
    FILE *fp = fopen(testFile, "r+");
    ASSERT_NE(fp, nullptr) << "> fopen errno = " << errno;
    EXPECT_NE(fread(readBuf, sizeof(writeBuf), 1, fp), 0);
    EXPECT_STREQ(writeBuf, readBuf) << "> writeBuf = " << writeBuf\
                                    << "\n> readBuf = " << readBuf;
    EXPECT_NE(fclose(fp), -1) << "> fclose errno =" <<errno;
    remove(testFile);
}
#endif
/**
 * @tc.number   SUB_KERNEL_PROCESS_ASSERT_0100
 * @tc.name     Basic test about assert true
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessTest, testAssertTrue, Function | MediumTest | Level3)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent: fork errno = " << errno;
    if (pid == 0) {
        assert(true);
        exit(0);
    }
    Msleep(50);
    WaitProcExitedOK(pid);
}

int FunctionAssertFalse(void)
{
    pid_t pid = fork();
    if (pid < 0) {
        LOG("> fork errno = %d", errno);
        return 0;
    } else if (pid == 0) {
        assert(false);
        LOG("> child not stop");
        exit(0);
    }
    Msleep(50);
    int exitCode = 0;
    int reInt = CheckProcStatus(pid, &exitCode, 0);
    if ((reInt == 2) && (exitCode == SIGABRT)) {
        LOG("> Success");
        return 1;
    }
    LOG("> Fail");
    return 0;
}

/**
 * @tc.number   SUB_KERNEL_PROCESS_ASSERT_0200
 * @tc.name     Basic test about assert false
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessTest, testAssertFalse, Function | MediumTest | Level3)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent: fork errno = " << errno;
    if (pid == 0) {
        assert(false);
        LOG("> child not stop");
        exit(0);
    }
    Msleep(50);
    int exitCode = 0;
    ASSERT_EQ(CheckProcStatus(pid, &exitCode, 0), 2);
    ASSERT_EQ(exitCode, SIGABRT);
}

int g_shmid;

void AtexitCallback(void)
{
    LOG("> AtexitCallback");
    pid_t *shared = (int*)shmat(g_shmid, nullptr, 0);
    if (shared == (void *)-1) {
        LOG("> AtexitCallback shmat errno = %d", errno);
        exit(1);
    }
    *shared = getppid();
    if ((shmdt(shared)) == -1) {
        LOG("> AtexitCallback shmdt errno = %d", errno);
        exit(1);
    }
}

/**
 * @tc.number   SUB_KERNEL_PROCESS_ATEXIT_0100
 * @tc.name     Basic test about atexit
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessTest, testAtexit, Function | MediumTest | Level3)
{
    const int memSize = 1024;
    g_shmid = shmget(IPC_PRIVATE, memSize, 0666 | IPC_CREAT);
    ASSERT_NE(g_shmid, -1) << "> parent: shmid errno = " << errno;
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent: fork errno = " << errno;
    if (pid == 0) {
        Msleep(20);
        if (atexit(AtexitCallback) != 0) {
            LOG("> atexit errno = %d", errno);
            exit(1);
        }
        exit(0);
    }
    pid_t *shared = (int*)shmat(g_shmid, nullptr, 0);

    WaitProcExitedOK(pid);
    ASSERT_NE(shared, (void*)-1) << "> shmat errno = " << errno;
    EXPECT_EQ(*shared, getpid());
    ASSERT_NE(shmdt(shared), -1) << "> parent: shmdt errno = " << errno;
    ASSERT_NE(shmctl(g_shmid, IPC_RMID, nullptr), -1) << "> shmctl : IPC_RMID : errno = " << errno;
}

/**
 * @tc.number     SUB_KERNEL_PROCESS_WAIT_0100
 * @tc.name       test wait return
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessTest, testWaitReturn, Function | MediumTest | Level3)
{
    InitGlobalVariable();
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> fork errno = " << errno;
    if (pid == 0) {
        int myPid = (int)getpid();
        LOG("> child pid = %d", myPid);
        SetGlobalVariable(myPid);
        exit(0);
    }
    Msleep(20);
    int pidChild = (int)wait(nullptr);
    LOG("> pidChild = %d", pidChild);
    int expectPid = GetGlobalVariable();
    EXPECT_EQ(pidChild, expectPid);
    DeleteGlobalVariable();
}


/**
 * @tc.number     SUB_KERNEL_PROCESS_WAIT_0200
 * @tc.name       test wait parameter
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessTest, testWaitTest, Function | MediumTest | Level3)
{
    int childReturn = GetRandom(256) - 1;
    LOG("> childReturn = %d", childReturn);

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> fork errno = " << errno;
    if (pid == 0) {
        exit(childReturn);
    }
    Msleep(20);
    int status = 0;
    int pidChild = (int)wait(&status);
    EXPECT_NE(pidChild, -1);
    EXPECT_TRUE(WIFEXITED(status));
    EXPECT_EQ(WEXITSTATUS(status), childReturn);
}

/**
 * @tc.number     SUB_KERNEL_PROCESS_WAIT_0300
 * @tc.name       test wait killed by SIGABRT
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessTest, testWaitKilled, Function | MediumTest | Level3)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> fork errno = " << errno;
    if (pid == 0) {
        Msleep(40);
        exit(0);
    }
    Msleep(20);
    kill(pid, SIGABRT);
    int status = 0;
    int pidChild = (int)wait(&status);
    EXPECT_NE(pidChild, -1);
    EXPECT_TRUE(WIFSIGNALED(status));
    EXPECT_EQ(WTERMSIG(status), SIGABRT);
}

/**
 * @tc.number     SUB_KERNEL_PROCESS_WAITPID_0100
 * @tc.name       waitpid basic test
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessTest, testWaitPidBasic, Function | MediumTest | Level3)
{
    int childReturn = GetRandom(128) - 1;
    LOG("> childReturn = %d", childReturn);

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> fork errno = " << errno;
    if (pid == 0) {
        exit(childReturn);
    }
    Msleep(20);
    int status = 0;
    int pidChild = (int)waitpid(pid, &status, 0);
    EXPECT_NE(pidChild, -1);
    EXPECT_TRUE(WIFEXITED(status));
    EXPECT_EQ(WEXITSTATUS(status), childReturn);
}

/**
 * @tc.number     SUB_KERNEL_PROCESS_WAITPID_0200
 * @tc.name       test waitpid with WNOHANG
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessTest, testWaitPidTest, Function | MediumTest | Level3)
{
    InitGlobalVariable();
    SetGlobalVariable(1);
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> fork errno = " << errno;
    if (pid == 0) {
        Msleep(50);
        SetGlobalVariable(2);
        exit(0);
    }
    Msleep(20);
    int status = 0;
    int pidChild = (int)waitpid(pid, &status, WNOHANG);
    int expectPid = GetGlobalVariable();
    EXPECT_EQ(expectPid, 1);
    EXPECT_EQ(pidChild, 0);
    Msleep(50);
    WaitProcExitedOK(pid);
    DeleteGlobalVariable();
}
