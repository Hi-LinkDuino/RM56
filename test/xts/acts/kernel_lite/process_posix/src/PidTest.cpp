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

// tested api in this file: getpid getppid getpgrp setpgrp  getpgid setpgid

#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <gtest/gtest.h>
#include "log.h"
#include "utils.h"
#include "KernelConstants.h"

using namespace testing::ext;

class PidTest : public testing::Test {
protected:
    const char* mChildELF = RES_DIR_KERNEL "process/executor";
};

/**
 * @tc.number SUB_KERNEL_PM_PID_Getppid_0100
 * @tc.name   getpid and getppid test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(PidTest, testGetPpid, Function | MediumTest | Level1)
{
    pid_t parentPid = getpid();
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        pid_t pPid = getppid();
        if (pPid != parentPid) {
            LOG("getppid fail, expect:%d, but get:%d", parentPid, pPid);
            exit(1);
        }
        exit(0);
    } else { // parent
        Msleep(20);
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_PM_PID_Getppid_0200
 * @tc.name   test that a orphaned process will inherite by init.
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(PidTest, testOrphanProcess, Function | MediumTest | Level1)
{
    const int retPass = 1;
    const int retFail = 2;
    const int memSize = 32;
    int shmID = shmget(IPC_PRIVATE, memSize, 0666 | IPC_CREAT);
    ASSERT_NE(shmID, -1) << "get share mem fail, errno = " << errno;
    int *shared = (int*)(shmat(shmID, nullptr, 0));
    ASSERT_NE(shared, reinterpret_cast<int*>(-1)) << "shmat fail, errno = " << errno;
    shared[0] = retPass;
    shared[1] = 0;
 
    LOG("parent process id:%d", getpid());
    pid_t pid = fork();
    EXPECT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        LOG("sub parent process id:%d", getpid());
        pid_t pid2 = fork();
        if (pid2 < 0) {
            LOG("======== Fork Error! =========");
            exit(1);
        }

        if (pid2 == 0) { // child
            LOG("orphane process id:%d", getpid());
            int *shmAddr = (int*)(shmat(shmID, nullptr, 0));
            LOG("before while child child %d", shmAddr[1]);
            while (shmAddr[1] != 1) {
                Msleep(50);
            }
            LOG("after while child child %d", shmAddr[1]);
            pid_t pPid = getppid();
            if (pPid != 1) {
                LOG("getppid orphaned process fail, expect:1, but get:%d", pPid);
                // transfer result to main process
                shmAddr[0] = retFail;
            }
            shmAddr[1] =2;
            LOG("child child exit %d", shmAddr[1]);
            shmdt(shmAddr);
            exit(0);
        } else { // sub parent
            exit(0);
        }
    }
    // parent
    WaitProcExitedOK(pid);
    shared[1] = 1;
    Msleep(200);
    EXPECT_EQ(shared[0], retPass);
    LOG("before while paret %d", shared[1]);
    while (shared[1] != 2) {
        Msleep(50);
    }
    shmdt(shared);
    shmctl(shmID, IPC_RMID, nullptr);
}

/**
 * @tc.number SUB_KERNEL_PM_PID_SetGetPgrp_0100
 * @tc.name   setpgrp and getpgrp test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(PidTest, testSetGetPgrp, Function | MediumTest | Level1)
{
    pid_t parentPid = getpid();
    int rt = setpgrp();
    ASSERT_EQ(rt, 0);

    pid_t pgid = getpgrp();
    ASSERT_EQ(pgid, parentPid);

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        pid_t childPgid = getpgrp();
        if (childPgid != pgid) {
            LOG("child getpgrp fail, expect:%d, but get:%d", pgid, childPgid);
            exit(1);
        }
        exit(0);
    } else { // parent
        Msleep(20);
        WaitProcExitedOK(pid);
    }
}

/**
 * @tc.number SUB_KERNEL_PM_PID_SetGetPgid_0100
 * @tc.name   setpgid and getpgid basic test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(PidTest, testSetGetPgid, Function | MediumTest | Level1)
{
    pid_t parentPid = getpid();
    int rt = setpgid(0, parentPid);
    ASSERT_EQ(rt, 0);
    rt = setpgid(0, 0);
    ASSERT_EQ(rt, 0);

    pid_t pgid = getpgid(0);
    ASSERT_EQ(pgid, parentPid);

    pid_t pid1 = fork();
    ASSERT_TRUE(pid1 >= 0) << "======== Fork Error! =========";
    if (pid1 == 0) { // child
        pid_t childPgid = getpgid(0);
        if (childPgid != pgid) {
            LOG("child getpgid fail, expect:%d, but get:%d", pgid, childPgid);
            exit(1);
        }
        Msleep(40);
        childPgid = getpgid(0);
        pid_t childPid = getpid();
        if (childPgid != childPid) {
            LOG("child new pgid check fail, pid=%d, pgid:%d", childPid, childPgid);
            exit(1);
        }
        exit(0);
    } else { // parent
        Msleep(30);
        rt = setpgid(pid1, pid1);
        ASSERT_EQ(rt, 0);
        pid_t pgid2 = getpgid(pid1);
        ASSERT_EQ(pgid2, pid1);
        Msleep(50);
        WaitProcExitedOK(pid1);
    }
}
/**
 * @tc.number SUB_KERNEL_PM_PID_SetGetPgid_0200
 * @tc.name   setpgid and getpgid test for sibling process
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(PidTest, testSetGetSiblingPgid, Function | MediumTest | Level1)
{
    pid_t parentPid = getpid();
    int rt = setpgid(0, parentPid);
    ASSERT_EQ(rt, 0);
    rt = setpgid(0, 0);
    ASSERT_EQ(rt, 0);
    pid_t pgid = getpgid(0);
    ASSERT_EQ(pgid, parentPid);

    pid_t pid1 = fork();
    ASSERT_TRUE(pid1 >= 0) << "======== Fork Error! =========";
    if (pid1 == 0) { // child1
        Msleep(20);
        exit(0);
    } else { // parent
        pid_t pid2 = fork();
        ASSERT_TRUE(pid2 >= 0) << "======== Fork Error! =========";
        if (pid2 == 0) { // child2
            int exitCode = 0;
            pid_t siblingPgid = getpgid(pid1);
            if (siblingPgid != parentPid) {
                LOG("child2: get sibling pgid fail, rt=%d, errno=%d", siblingPgid, errno);
                exitCode = 1;
            }
            rt = setpgid(pid1, pid1);
            if (rt != -1) {
                LOG("child2: setpgid for sibling should fail");
                exitCode = 1;
            }
            if (errno != ESRCH) {
                LOG("child2: setpgid errno fail, expected %d, but get %d", ESRCH, errno);
                exitCode = 1;
            }
            exit(exitCode);
        }
        // parent
        Msleep(50);
        WaitProcExitedOK(pid1);
        WaitProcExitedOK(pid2);
    }
}
/**
 * @tc.number SUB_KERNEL_PM_PID_SetGetPgid_0300
 * @tc.name   getpgid fail test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(PidTest, testGetpgidFail, Function | MediumTest | Level3)
{
    pid_t pgid = getpgid(-1);
    EXPECT_EQ(pgid, -1);
    EXPECT_EQ(errno, EINVAL);

    pgid = getpgid(-1000001);
    EXPECT_EQ(pgid, -1);
    EXPECT_EQ(errno, EINVAL);

    pid_t nonExitPid = GetNonExistPid(); // valid but not exist pid
    if (nonExitPid != -1) {
        pgid = getpgid(nonExitPid);
        EXPECT_EQ(pgid, -1);
        EXPECT_EQ(errno, ESRCH);
    }

    pgid = getpgid(MAX_PROCESS_NUMBER + 1);
    EXPECT_EQ(pgid, -1);
    EXPECT_EQ(errno, EINVAL);

    pgid = getpgid(100000);
    EXPECT_EQ(pgid, -1);
    EXPECT_EQ(errno, EINVAL);

    pgid = getpgid(1);
    EXPECT_EQ(pgid, 1) << "get init process-groups-ID fail\n";
}

/**
 * @tc.number SUB_KERNEL_PM_PID_SetGetPgid_0400
 * @tc.name   setpgid fail test
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(PidTest, testSetpgidFail, Function | MediumTest | Level3)
{
    pid_t pid = getpid();
    int rt = setpgrp();
    ASSERT_EQ(rt, 0);

    LOG("invalid pid test...");
    rt = setpgid(-1, 0);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);

    rt = setpgid(-1000001, 0);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);

    pid_t nonExitPid = GetNonExistPid(); // valid but not exist pid
    if (nonExitPid != -1) {
        rt = setpgid(nonExitPid, 0);
        EXPECT_EQ(rt, -1);
        EXPECT_EQ(errno, ESRCH);    
    }

    rt = setpgid(MAX_PROCESS_NUMBER + 1, 0);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);

    rt = setpgid(100000, 0);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);

    rt = setpgid(1, pid); // init
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EPERM);

    rt = setpgid(2, pid); // kProcess
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EPERM);

    LOG("invalid pgid test...");
    rt = setpgid(0, -1);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);

    nonExitPid = GetNonExistPid(); // valid but not exist pid
    if (nonExitPid != -1) {
        rt = setpgid(0, nonExitPid);
        EXPECT_EQ(rt, -1);
        EXPECT_EQ(errno, EPERM);        
    }

    rt = setpgid(0, 10001);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);

    rt = setpgid(0, 1); // init
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EPERM);

    rt = setpgid(0, 2); // kProcess
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EPERM);
}

/**
 * @tc.number SUB_KERNEL_PM_PID_SetGetPgid_0500
 * @tc.name   setpgid fail test: EACCES
 * @tc.desc   [C- SOFTWARE -0200]
 */
HWTEST_F(PidTest, testSetpgidFailEACCES, Function | MediumTest | Level3)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int rt = execve(mChildELF, NULL, NULL);
        if (rt == -1) {
            LOG("ERROR: execve return -1, errno=%d, err=%s\n", errno, strerror(errno));
        }
        exit(1);
    } else { // parent
        sleep(2);
        AssertProcAlive(pid);
        int rt = setpgid(pid, pid);
        EXPECT_EQ(rt, -1) << "setpgid should fail after child execve.";
        EXPECT_EQ(errno, EACCES) << "set errno fail.";

        // recycle child
        int status;
        waitpid(pid, &status, 0);
    }
}
