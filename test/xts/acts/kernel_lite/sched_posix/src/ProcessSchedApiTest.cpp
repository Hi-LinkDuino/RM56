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


#include <unistd.h>
#include <sys/resource.h>
#include <sys/shm.h>
#include <gtest/gtest.h>
#include "log.h"
#include "utils.h"
#include "KernelConstants.h"
#include "mt_utils.h"

using namespace testing::ext;

class ProcessSchedApiTest : public testing::Test {
};

/**
 * @tc.number   SUB_KERNEL_SCHED_API_GETPRIORITY_0100
 * @tc.name     getpriority api test.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testGetPriority, Function | MediumTest | Level1)
{
    int priority = getpriority(PRIO_PROCESS, INIT_PROCESS_ID);
    EXPECT_EQ(priority, DEFAULT_INIT_PROCESS_PRIORITY) << "check 'init' priority failed.";
    priority = getpriority(PRIO_PROCESS, KERNEL_PROCESS_ID);
    EXPECT_EQ(priority, DEFAULT_KERNEL_PROCESS_PRIORITY) << "check 'KProcess' priority failed.";
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_GETPRIORITY_0200
 * @tc.name     getpriority error test with unsupport parameter 1.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testGetPriorityError0200, Function | MediumTest | Level3)
{
    LOG("invalid 'which' test:");
    errno = 0;
    int priority = getpriority(PRIO_PGRP, 0);
    EXPECT_EQ(priority, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    priority = getpriority(PRIO_USER, 0);
    EXPECT_EQ(priority, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_GETPRIORITY_0300
 * @tc.name     getpriority error test with invalid parameter 1.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testGetPriorityError0300, Function | MediumTest | Level3)
{
    int priority;

    LOG("invalid 'which' test:");
    errno = 0;
    priority = getpriority(PRIO_USER + GetRandom(1000), 0);
    EXPECT_EQ(priority, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    priority = getpriority(-GetRandom(1000), 0);
    EXPECT_EQ(priority, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_GETPRIORITY_0400
 * @tc.name     getpriority error test with invalid parameter 2.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testGetPriorityError0400, Function | MediumTest | Level3)
{
    int priority;
    LOG("invalid 'who' test:");
    errno = 0;
    priority = getpriority(PRIO_PROCESS, -1);
    EXPECT_EQ(priority, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    priority = getpriority(PRIO_PROCESS, MAX_PROCESS_NUMBER + 1);
    EXPECT_EQ(priority, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    priority = getpriority(PRIO_PROCESS, MAX_PROCESS_NUMBER + GetRandom(100000));
    EXPECT_EQ(priority, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_GETPRIORITY_0500
 * @tc.name     getpriority error test with not exist parameter 2.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testGetPriorityError0500, Function | MediumTest | Level3)
{
    int priority;
    LOG("invalid 'who' test:");
    LOG("not exist pid test:");
    pid_t nonExitPid = GetNonExistPid(); // valid but not exist pid
    if (nonExitPid != -1) {
        priority = getpriority(PRIO_PROCESS, nonExitPid);
        EXPECT_EQ(priority, -1);
        EXPECT_EQ(errno, ESRCH);
    }
}

void SetPriorityAllTest()
{
    int rt, newPrio;
    struct sched_param param;
    LOG("test pid '0' and cover all supported priority:");
    for (int prio = HIGHEST_USER_PROCESS_PRIORITY; prio <= LOWEST_USER_PROCESS_PRIORITY; prio++) {
        rt = setpriority(PRIO_PROCESS, 0, prio);
        EXPECT_EQ(rt, 0) << "setpriority failed for prio=" << prio << ", errno=" << errno;
        newPrio = getpriority(PRIO_PROCESS, 0);
        EXPECT_EQ(newPrio, prio);
        rt = sched_getparam(getpid(), &param);
        EXPECT_EQ(rt, 0);
        newPrio = param.sched_priority;
        EXPECT_EQ(newPrio, prio);
    }
    LOG("set back to default value:");
    rt = setpriority(PRIO_PROCESS, getpid(), DEFAULT_SHELL_PROCESS_PRIORITY);
    EXPECT_EQ(rt, 0);
    newPrio = getpriority(PRIO_PROCESS, 0);
    EXPECT_EQ(newPrio, DEFAULT_SHELL_PROCESS_PRIORITY);
}
/**
* @tc.number     SUB_KERNEL_SCHED_API_SETPRIORITY_0100
* @tc.name       test setpriority for all supported priority.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSetPriority, Function | MediumTest | Level1)
{
    SetPriorityAllTest();
}
/**
 * @tc.number   SUB_KERNEL_SCHED_API_SETPRIORITY_0200
 * @tc.name     test setpriority for all supported priority, in RR mode
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSetPriorityFiFo, Function | MediumTest | Level3)
{
    LOG("change sched policy");
    struct sched_param param;
    int rt = sched_getparam(0, &param);
    ASSERT_EQ(rt, 0) << "get sched param failed, errno=" << errno;

    SetPriorityAllTest();

    LOG("set back to RR");
    rt = sched_setscheduler(0, SCHED_RR, &param);
    ASSERT_EQ(rt, 0) << "set SCHED_RR failed, errno=" << errno;
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SETPRIORITY_0300
 * @tc.name     setpriority error test with unsupport parameter 1.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSetPriorityError0300, Function | MediumTest | Level3)
{
    const int testPrio = 18;
    LOG("invalid 'which' test:");
    errno = 0;
    int rt = setpriority(PRIO_PGRP, 0, testPrio);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = setpriority(PRIO_USER, 0, testPrio);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SETPRIORITY_0400
 * @tc.name     setpriority error test with invlid parameter 1.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSetPriorityError0400, Function | MediumTest | Level3)
{
    const int testPrio = 18;
    int rt;
    LOG("invalid 'which' test:");
    errno = 0;
    rt = setpriority(PRIO_USER + GetRandom(1000), 0, testPrio);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = setpriority(-GetRandom(1000), 0, testPrio);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SETPRIORITY_0500
 * @tc.name     setpriority error test with invalid parameter 2.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSetPriorityError0500, Function | MediumTest | Level3)
{
    int rt;
    const int testPrio = 18;
    LOG("invalid 'pid' test:");
    errno = 0;
    rt = setpriority(PRIO_PROCESS, -1, testPrio);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = setpriority(PRIO_PROCESS, MAX_PROCESS_NUMBER + 1, testPrio);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = setpriority(PRIO_PROCESS, MAX_PROCESS_NUMBER + GetRandom(1000), testPrio);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SETPRIORITY_0600
 * @tc.name     setpriority error test with not exist parameter 2.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSetPriorityError0600, Function | MediumTest | Level3)
{
    const int testPrio = 18;
    int rt;
    LOG("not exist pid test:");
    pid_t nonExitPid = GetNonExistPid(); // valid but not exist pid
    if (nonExitPid != -1) {
        rt = setpriority(PRIO_PROCESS, nonExitPid, testPrio);
        EXPECT_EQ(rt, -1);
        EXPECT_EQ(errno, ESRCH);
    }
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SETPRIORITY_0700
 * @tc.name     setpriority error test with invalid parameter 3.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSetPriorityError0700, Function | MediumTest | Level3)
{
    int rt;
    LOG("invalid 'priority' test:");
    errno = 0;
    rt = setpriority(PRIO_PROCESS, 0, -1);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = setpriority(PRIO_PROCESS, 0, -GetRandom(1000));
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = setpriority(PRIO_PROCESS, 0, 0);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = setpriority(PRIO_PROCESS, 0, HIGHEST_USER_PROCESS_PRIORITY - 1);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    LOG("check the default priority not changed:");
    int newPrio = getpriority(PRIO_PROCESS, 0);
    if (newPrio != DEFAULT_SHELL_PROCESS_PRIORITY) {
        rt = setpriority(PRIO_PROCESS, 0, DEFAULT_SHELL_PROCESS_PRIORITY);
        EXPECT_EQ(rt, 0);
        FAIL() << "setpriority failed but priority has changed";
    }
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SETPRIORITY_0800
 * @tc.name     setpriority error test with no permission.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSetPriorityError0800, Function | MediumTest | Level3)
{
    const int testPrio = 18;
    int rt;
    LOG("no permission test:");
    errno = 0;
    rt = setpriority(PRIO_PROCESS, INIT_PROCESS_ID, testPrio);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EPERM);
    errno = 0;
    rt = setpriority(PRIO_PROCESS, KERNEL_PROCESS_ID, testPrio);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EPERM);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SCHED_GETPARAM_0100
 * @tc.name     sched_getparam api test.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedGetParam, Function | MediumTest | Level1)
{
    struct sched_param param;
    int rt = sched_getparam(0, &param);
    EXPECT_EQ(rt, 0) << "sched_getparam failed, errno=" << errno;
    EXPECT_EQ(param.sched_priority, DEFAULT_SHELL_PROCESS_PRIORITY);

    rt = sched_getparam(getpid(), &param);
    EXPECT_EQ(rt, 0) << "sched_getparam failed, errno=" << errno;
    EXPECT_EQ(param.sched_priority, DEFAULT_SHELL_PROCESS_PRIORITY);

    rt = sched_getparam(INIT_PROCESS_ID, &param);
    EXPECT_EQ(rt, 0) << "sched_getparam failed, errno=" << errno;
    EXPECT_EQ(param.sched_priority, DEFAULT_INIT_PROCESS_PRIORITY);

    rt = sched_getparam(KERNEL_PROCESS_ID, &param);
    EXPECT_EQ(rt, 0) << "sched_getparam failed, errno=" << errno;
    EXPECT_EQ(param.sched_priority, DEFAULT_KERNEL_PROCESS_PRIORITY);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SCHED_GETPARAM_0200
 * @tc.name     sched_getparam error test with invalid parameter 1.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedGetParamError0200, Function | MediumTest | Level3)
{
    struct sched_param param;
    LOG("invalid 'pid' test:");
    errno = 0;
    int rt = sched_getparam(-1, &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = sched_getparam(MAX_PROCESS_NUMBER + 1, &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = sched_getparam(MAX_PROCESS_NUMBER + GetRandom(100000), &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SCHED_GETPARAM_0300
 * @tc.name     sched_getparam error test with not exist pid.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedGetParamError0300, Function | MediumTest | Level3)
{
    struct sched_param param;
    int rt;
    LOG("not exist pid test:");
    pid_t nonExitPid = GetNonExistPid(); // valid but not exist pid
    if (nonExitPid != -1) {
        rt = sched_getparam(nonExitPid, &param);
        EXPECT_EQ(rt, -1);
        EXPECT_EQ(errno, ESRCH);
    }
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SCHED_GETPARAM_0400
 * @tc.name     sched_getparam error test with invalid parameter 2.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedGetParamError0400, Function | MediumTest | Level3)
{
    int rt;
    LOG("invalid 'param' test:");
    rt = sched_getparam(0, NULL);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
}

void SchedSetParamAllTest()
{
    int rt, newPrio;
    struct sched_param param;
    LOG("test all supported priority:");
    for (int prio = HIGHEST_USER_PROCESS_PRIORITY; prio <= LOWEST_USER_PROCESS_PRIORITY; prio++) {
        param.sched_priority = prio;
        rt = sched_setparam(0, &param);
        EXPECT_EQ(rt, 0) << "sched_setparam failed for prio=" << prio << ", errno=" << errno;
        newPrio = getpriority(PRIO_PROCESS, getpid());
        EXPECT_EQ(prio, newPrio);
        rt = sched_getparam(0, &param);
        EXPECT_EQ(rt, 0);
        EXPECT_EQ(prio, param.sched_priority);
    }
    LOG("set back to default value:");
    param.sched_priority = DEFAULT_SHELL_PROCESS_PRIORITY;
    rt = sched_setparam(getpid(), &param);
    EXPECT_EQ(rt, 0);
    newPrio = getpriority(PRIO_PROCESS, 0);
    EXPECT_EQ(newPrio, DEFAULT_SHELL_PROCESS_PRIORITY);
}
/**
 * @tc.number   SUB_KERNEL_SCHED_API_SCHED_SETPARAM_0100
 * @tc.name     test sched_setparam for all supported priority.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedSetParam, Function | MediumTest | Level1)
{
    SchedSetParamAllTest();
}
/**
 * @tc.number   SUB_KERNEL_SCHED_API_SCHED_SETPARAM_0200
 * @tc.name     test sched_setparam for all supported priority, in FIFO mode
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedSetParamFiFo, Function | MediumTest | Level3)
{
    struct sched_param param;
    int rt = sched_getparam(0, &param);
    ASSERT_EQ(rt, 0) << "get sched param failed, errno=" << errno;

    SchedSetParamAllTest();

    LOG("set to RR");
    rt = sched_setscheduler(0, SCHED_RR, &param);
    ASSERT_EQ(rt, 0) << "set SCHED_RR failed, errno=" << errno;
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SCHED_SETPARAM_0300
 * @tc.name     sched_setparam error test with invalid pid.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedSetParamError0300, Function | MediumTest | Level3)
{
    int rt;
    struct sched_param param;
    param.sched_priority = 18;

    LOG("invalid 'pid' test:");
    errno = 0;
    rt = sched_setparam(-1, &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = sched_setparam(MAX_PROCESS_NUMBER + 1, &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = sched_setparam(MAX_PROCESS_NUMBER + GetRandom(100000), &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SCHED_SETPARAM_0400
 * @tc.name     sched_setparam error test with not exist pid.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedSetParamError0400, Function | MediumTest | Level3)
{
    int rt;
    struct sched_param param;
    param.sched_priority = 18;

    LOG("not exist pid test:");
    pid_t nonExitPid = GetNonExistPid(); // valid but not exist pid
    if (nonExitPid != -1) {
        rt = sched_setparam(nonExitPid, &param);
        EXPECT_EQ(rt, -1);
        EXPECT_EQ(errno, ESRCH);
    }
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SCHED_SETPARAM_0500
 * @tc.name     sched_setparam error test with invalid parameter 2.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedSetParamError0500, Function | MediumTest | Level3)
{
    int rt;
    LOG("invalid 'param' test:");
    rt = sched_setparam(0, NULL);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SCHED_SETPARAM_0600
 * @tc.name     sched_setparam error test with invalid priority.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedSetParamError0600, Function | MediumTest | Level3)
{
    int rt;
    struct sched_param param;
    param.sched_priority = 18;

    LOG("invalid 'priority' test:");
    errno = 0;
    param.sched_priority = -1;
    rt = sched_setparam(0, &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    param.sched_priority = -GetRandom(1000);
    rt = sched_setparam(0, &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    param.sched_priority = 0;
    rt = sched_setparam(0, &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    param.sched_priority = HIGHEST_USER_PROCESS_PRIORITY - 1;
    rt = sched_setparam(0, &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);

    LOG("check the default priority not changed:");
    int newPrio = getpriority(PRIO_PROCESS, 0);
    if (newPrio != DEFAULT_SHELL_PROCESS_PRIORITY) {
        rt = setpriority(PRIO_PROCESS, 0, DEFAULT_SHELL_PROCESS_PRIORITY);
        EXPECT_EQ(rt, 0);
        FAIL() << "sched_setparam failed but priority has changed";
    }
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SCHED_SETPARAM_0700
 * @tc.name     sched_setparam error test with no permission.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedSetParamError0700, Function | MediumTest | Level3)
{
    int rt;
    struct sched_param param;
    param.sched_priority = 18;

    LOG("no permission test:");
    errno = 0;
    rt = sched_setparam(INIT_PROCESS_ID, &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EPERM);
    errno = 0;
    rt = sched_setparam(KERNEL_PROCESS_ID, &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EPERM);

    LOG("check the default priority not changed:");
    int newPrio = getpriority(PRIO_PROCESS, 0);
    if (newPrio != DEFAULT_SHELL_PROCESS_PRIORITY) {
        rt = setpriority(PRIO_PROCESS, 0, DEFAULT_SHELL_PROCESS_PRIORITY);
        EXPECT_EQ(rt, 0);
        FAIL() << "sched_setparam failed but priority has changed";
    }
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_GET_PRIORITY_MAX_0100
 * @tc.name     sched_get_priority_max api test.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedGetPriorityMax, Function | MediumTest | Level3)
{
    int prio = sched_get_priority_max(SCHED_RR);
    EXPECT_EQ(prio, LOWEST_USER_PROCESS_PRIORITY);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_GET_PRIORITY_MAX_0200
 * @tc.name     sched_get_priority_max api error test with unsupport policy.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedGetPriorityMaxError0200, Function | MediumTest | Level3)
{
    // SCHED_RR is 2
    int invalidPolicy[] = {SCHED_FIFO, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_DEADLINE, SCHED_RESET_ON_FORK};
    int testLoop = sizeof(invalidPolicy)/sizeof(int);
    for (int i = 0; i < testLoop; i++) {
        errno = 0;
        int prio = sched_get_priority_max(invalidPolicy[i]);
        EXPECT_EQ(prio, -1) << "sched_get_priority_max ok for policy=" << invalidPolicy[i];
        EXPECT_EQ(errno, EINVAL) << "errno check fail for policy=" << invalidPolicy[i];
    }
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_GET_PRIORITY_MAX_0300
 * @tc.name     sched_get_priority_max api error test with invalid policy.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedGetPriorityMaxError0300, Function | MediumTest | Level3)
{
    // SCHED_RR is 2
    int invalidPolicyVal;
    int prio;

    invalidPolicyVal = -GetRandom(10000);
    errno = 0;
    prio = sched_get_priority_max(invalidPolicyVal);
    EXPECT_EQ(prio, -1) << "sched_get_priority_max ok for policy=" << invalidPolicyVal;
    EXPECT_EQ(errno, EINVAL) << "errno check fail for policy=" << invalidPolicyVal;

    invalidPolicyVal = GetRandom(10000) + SCHED_DEADLINE;
    errno = 0;
    prio = sched_get_priority_max(invalidPolicyVal);
    EXPECT_EQ(prio, -1) << "sched_get_priority_max ok for policy=" << invalidPolicyVal;
    EXPECT_EQ(errno, EINVAL) << "errno check fail for policy=" << invalidPolicyVal;
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_GET_PRIORITY_MIN_0100
 * @tc.name     sched_get_priority_min api test.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedGetPriorityMin, Function | MediumTest | Level3)
{
    int prio = sched_get_priority_min(SCHED_RR);
    EXPECT_EQ(prio, HIGHEST_USER_PROCESS_PRIORITY);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_GET_PRIORITY_MIN_0200
 * @tc.name     sched_get_priority_min api error test with unsupport policy.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedGetPriorityMinError0200, Function | MediumTest | Level3)
{
    //  SCHED_RR is 2
    int invalidPolicy[] = {SCHED_FIFO, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_DEADLINE, SCHED_RESET_ON_FORK};
    int testLoop = sizeof(invalidPolicy)/sizeof(int);
    for (int i = 0; i < testLoop; i++) {
        errno = 0;
        int prio = sched_get_priority_min(invalidPolicy[i]);
        EXPECT_EQ(prio, -1) << "sched_get_priority_min ok for policy=" << invalidPolicy[i];
        EXPECT_EQ(errno, EINVAL) << "errno check fail for policy=" << invalidPolicy[i];
    }
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_GET_PRIORITY_MIN_0300
 * @tc.name     sched_get_priority_min api error test with invalid policy.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedGetPriorityMinError0300, Function | MediumTest | Level3)
{
    int invalidPolicyVal;
    int prio;
    invalidPolicyVal = -GetRandom(10000);
    errno = 0;
    prio = sched_get_priority_min(invalidPolicyVal);
    EXPECT_EQ(prio, -1) << "sched_get_priority_min ok for policy=" << invalidPolicyVal;
    EXPECT_EQ(errno, EINVAL) << "errno check fail for policy=" << invalidPolicyVal;

    invalidPolicyVal = GetRandom(10000) + SCHED_DEADLINE;
    errno = 0;
    prio = sched_get_priority_min(invalidPolicyVal);
    EXPECT_EQ(prio, -1) << "sched_get_priority_min ok for policy=" << invalidPolicyVal;
    EXPECT_EQ(errno, EINVAL) << "errno check fail for policy=" << invalidPolicyVal;
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_GETSCHEDULER_0100
 * @tc.name     sched_getscheduler api test.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedGetScheduler, Function | MediumTest | Level1)
{
    int policy = sched_getscheduler(0);
    EXPECT_EQ(policy, SCHED_RR);
    policy = sched_getscheduler(getpid());
    EXPECT_EQ(policy, SCHED_RR) << "check 'shell' policy failed.";
    policy = sched_getscheduler(INIT_PROCESS_ID);
    EXPECT_EQ(policy, SCHED_RR) << "check 'init' policy failed.";
    policy = sched_getscheduler(KERNEL_PROCESS_ID);
    EXPECT_EQ(policy, SCHED_RR) << "check 'KProcess' policy failed.";
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_GETSCHEDULER_0200
 * @tc.name     sched_getscheduler api error test with invalid pid.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedGetSchedulerError0200, Function | MediumTest | Level1)
{
    LOG("invalid pid test:");
    errno = 0;
    int rt = sched_getscheduler(-1);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = sched_getscheduler(MAX_PROCESS_NUMBER + 1);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = sched_getscheduler(MAX_PROCESS_NUMBER + GetRandom(1000));
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_GETSCHEDULER_0300
 * @tc.name     sched_getscheduler api error test with not exist pid.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedGetSchedulerError0300, Function | MediumTest | Level1)
{
    int rt;
    errno = 0;
    LOG("not exist pid test:");
    pid_t nonExitPid = GetNonExistPid(); // valid but not exist pid
    if (nonExitPid != -1) {
        rt = sched_getscheduler(nonExitPid);
        EXPECT_EQ(rt, -1);
        EXPECT_EQ(errno, ESRCH);
    }
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SETSCHEDULER_0100
 * @tc.name     sched_setscheduler api test.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedSetScheduler, Function | MediumTest | Level1)
{
    struct sched_param param;
    int rt = sched_getparam(0, &param);
    ASSERT_EQ(rt, 0) << "get sched param failed, errno=" << errno;

    LOG("change policy and priority:");
    rt = sched_setscheduler(0, SCHED_RR, &param);
    ASSERT_EQ(rt, 0) << "set RR failed, errno=" << errno;
    int policy = sched_getscheduler(0);
    EXPECT_EQ(policy, SCHED_RR);
    rt = sched_setscheduler(getpid(), SCHED_RR, &param);
    EXPECT_EQ(rt, 0) << "set RR again failed, errno=" << errno;
    policy = sched_getscheduler(0);
    EXPECT_EQ(policy, SCHED_RR);

    param.sched_priority = LOWEST_USER_PROCESS_PRIORITY;
    rt = sched_setscheduler(getpid(), SCHED_RR, &param);
    EXPECT_EQ(rt, 0) << "set RR again failed, errno=" << errno;
    policy = sched_getscheduler(0);
    EXPECT_EQ(policy, SCHED_RR);
    int newPrio = getpriority(PRIO_PROCESS, 0);
    EXPECT_EQ(newPrio, LOWEST_USER_PROCESS_PRIORITY);

    LOG("set back to SCHED_RR and default priority:");
    param.sched_priority = DEFAULT_SHELL_PROCESS_PRIORITY;
    rt = sched_setscheduler(getpid(), SCHED_RR, &param);
    EXPECT_EQ(rt, 0) << "set back to SCHED_RR failed, errno=" << errno;
    policy = sched_getscheduler(0);
    EXPECT_EQ(policy, SCHED_RR);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SETSCHEDULER_0200
 * @tc.name     sched_setscheduler api error test with invalid pid.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedSetSchedulerError0200, Function | MediumTest | Level1)
{
    struct sched_param param;
    int rt = sched_getparam(0, &param);
    ASSERT_EQ(rt, 0) << "get sched param failed, errno=" << errno;

    LOG("invalid pid test:");
    errno = 0;
    rt = sched_setscheduler(-1, SCHED_RR, &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = sched_setscheduler(MAX_PROCESS_NUMBER + 1, SCHED_RR, &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = sched_setscheduler(MAX_PROCESS_NUMBER + GetRandom(1000), SCHED_RR, &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SETSCHEDULER_0300
 * @tc.name     sched_setscheduler api error test with not exist pid.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedSetSchedulerError0300, Function | MediumTest | Level1)
{
    struct sched_param param;
    int rt = sched_getparam(0, &param);
    ASSERT_EQ(rt, 0) << "get sched param failed, errno=" << errno;

    LOG("not exist pid test:");
    pid_t nonExitPid = GetNonExistPid(); // valid but not exist pid
    if (nonExitPid != -1) {
        rt = sched_setscheduler(nonExitPid, SCHED_RR, &param);
        EXPECT_EQ(rt, -1);
        EXPECT_EQ(errno, ESRCH);
    }
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SETSCHEDULER_0400
 * @tc.name     sched_setscheduler api error test with unsupport policy.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedSetSchedulerError0400, Function | MediumTest | Level1)
{
    struct sched_param param;
    int rt = sched_getparam(0, &param);
    ASSERT_EQ(rt, 0) << "get sched param failed, errno=" << errno;

    LOG("invalid policy test:");
    //and SCHED_RR is 2
    int invalidPolicy[] = {SCHED_FIFO, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_DEADLINE, SCHED_RESET_ON_FORK};
    int testLoop = sizeof(invalidPolicy)/sizeof(int);
    for (int i = 0; i < testLoop; i++) {
        errno = 0;
        rt = sched_setscheduler(0, invalidPolicy[i], &param);
        EXPECT_EQ(rt, -1) << "sched_get_priority_min ok for policy=" << invalidPolicy[i];
        EXPECT_EQ(errno, EINVAL) << "errno check fail for policy=" << invalidPolicy[i];
    }

    LOG("check the default priority not changed:");
    int policy = sched_getscheduler(0);
    if (policy != SCHED_RR) {
        rt = sched_setscheduler(0, SCHED_RR, &param);
        EXPECT_EQ(rt, 0);
        FAIL() << "sched_setscheduler failed but policy has changed";
    }
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SETSCHEDULER_0500
 * @tc.name     sched_setscheduler api error test with invalid policy.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedSetSchedulerError0500, Function | MediumTest | Level1)
{
    struct sched_param param;
    int rt = sched_getparam(0, &param);
    ASSERT_EQ(rt, 0) << "get sched param failed, errno=" << errno;

    LOG("invalid policy test:");
    int invalidPolicy = -GetRandom(10000);
    errno = 0;
    rt = sched_setscheduler(0, invalidPolicy, &param);
    EXPECT_EQ(rt, -1) << "sched_get_priority_min ok for policy=" << invalidPolicy;
    EXPECT_EQ(errno, EINVAL) << "errno check fail for policy=" << invalidPolicy;
    invalidPolicy = GetRandom(10000) + SCHED_DEADLINE;
    errno = 0;
    rt = sched_setscheduler(0, invalidPolicy, &param);
    EXPECT_EQ(rt, -1) << "sched_get_priority_min ok for policy=" << invalidPolicy;
    EXPECT_EQ(errno, EINVAL) << "errno check fail for policy=" << invalidPolicy;
    
    LOG("check the default priority not changed:");
    int policy = sched_getscheduler(0);
    if (policy != SCHED_RR) {
        rt = sched_setscheduler(0, SCHED_RR, &param);
        EXPECT_EQ(rt, 0);
        FAIL() << "sched_setscheduler failed but policy has changed";
    }
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SETSCHEDULER_0600
 * @tc.name     sched_setscheduler api error test with invalid parameter.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedSetSchedulerError0600, Function | MediumTest | Level1)
{
    struct sched_param param;
    int rt = sched_getparam(0, &param);
    ASSERT_EQ(rt, 0) << "get sched param failed, errno=" << errno;

    LOG("invalid param test:");
    rt = sched_setscheduler(0, SCHED_RR, NULL);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_SETSCHEDULER_0700
 * @tc.name     sched_setscheduler api error test with no permission.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedSetSchedulerError0700, Function | MediumTest | Level1)
{
    struct sched_param param;
    int rt = sched_getparam(0, &param);
    ASSERT_EQ(rt, 0) << "get sched param failed, errno=" << errno;

    LOG("no permission test:");
    errno = 0;
    rt = sched_setscheduler(INIT_PROCESS_ID, SCHED_RR, &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EPERM);
    errno = 0;
    rt = sched_setscheduler(KERNEL_PROCESS_ID, SCHED_RR, &param);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EPERM);
}

/**
* @tc.number     SUB_KERNEL_SCHED_API_GETRRINTERVAL_0100
* @tc.name       sched_rr_get_interval api test
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ProcessSchedApiTest, testSchedRRGetInterval, Function | MediumTest | Level1)
{
    struct timespec time1 = {1, 0};
    int rt = sched_rr_get_interval(0, &time1);
    printf("%llu, %u\n", time1.tv_sec, time1.tv_nsec);
    EXPECT_EQ(rt, 0);
    EXPECT_EQ(time1.tv_sec, 0);
    EXPECT_GE (time1.tv_nsec, DEFAULT_RR_SCHED_INTERVAL);

    time1.tv_sec = 1;
    time1.tv_nsec = 0;
    rt = sched_rr_get_interval(getpid(), &time1);
    printf("%llu, %u\n", time1.tv_sec, time1.tv_nsec);
    EXPECT_EQ(rt, 0);
    EXPECT_EQ(time1.tv_sec, 0);
    EXPECT_GE(time1.tv_nsec, DEFAULT_RR_SCHED_INTERVAL);
}

/**
* @tc.number     SUB_KERNEL_SCHED_API_GETRRINTERVAL_0200
* @tc.name       sched_rr_get_interval error test with invalid pid
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ProcessSchedApiTest, testSchedRRGetIntervalError0200, Function | MediumTest | Level1)
{
    struct timespec time1 = {0, 0};
    LOG("invalid 'pid' test:");
    errno = 0;
    int rt = sched_rr_get_interval(-1, &time1);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = sched_rr_get_interval(MAX_PROCESS_NUMBER + 1, &time1);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
    errno = 0;
    rt = sched_rr_get_interval(MAX_PROCESS_NUMBER + GetRandom(1000), &time1);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
* @tc.number     SUB_KERNEL_SCHED_API_GETRRINTERVAL_0300
* @tc.name       sched_rr_get_interval error test with not exist pid
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ProcessSchedApiTest, testSchedRRGetIntervalError0300, Function | MediumTest | Level1)
{
    struct timespec time1 = {0, 0};
    int rt;

    LOG("not exist pid test:");
    pid_t nonExitPid = GetNonExistPid(); // valid but not exist pid
    if (nonExitPid != -1) {
        rt = sched_rr_get_interval(nonExitPid, &time1);
        EXPECT_EQ(rt, -1);
        EXPECT_EQ(errno, ESRCH);
    }

    LOG("invalid 'timespec' test:");
    rt = sched_rr_get_interval(0, NULL);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
* @tc.number     SUB_KERNEL_SCHED_API_GETRRINTERVAL_0400
* @tc.name       sched_rr_get_interval error test with invalid timespec
* @tc.desc       [C- SOFTWARE -0200]
*/
HWTEST_F(ProcessSchedApiTest, testSchedRRGetIntervalError0400, Function | MediumTest | Level1)
{
    int rt;

    LOG("invalid 'timespec' test:");
    rt = sched_rr_get_interval(0, NULL);
    EXPECT_EQ(rt, -1);
    EXPECT_EQ(errno, EINVAL);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_YIELD_0100
 * @tc.name     test sched_yield with two process
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ProcessSchedApiTest, testSchedYieldBasic, Function | MediumTest | Level1)
{
    int ret;
    int cpuCnt = 0;
    cpu_set_t oldMask;
    CPU_ZERO(&oldMask);
    cpuCnt = GetCpuCount();
    LOG("GetCpuCount=%d", cpuCnt);
    if (cpuCnt > 1) {
        ret = FixCurProcessToOneCpu(0, &oldMask);
        ASSERT_EQ(ret, 0);
    }

    ASSERT_EQ(InitPipe(), 0) << "> parent: pipe errno = " << errno;
    EXPECT_NE(InitGlobalVariable(), -1);
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent: fork errno = " << errno;
    if (pid == 0) {
        if (cpuCnt > 1) {
            ret = FixCurProcessToOneCpu(0, &oldMask);
            EXPECT_EQ(ret, 0);
        }
        // control step 2
        BlockOnPipe();
        SetGlobalVariable(1);
        LOG("222");
        exit(0);
    }
    Msleep(50);
    // control step 1
    UnBlockPipe();
    LOG("111");
    EXPECT_EQ(sched_yield(), 0) << "> sched_yield error";
    // control step 3
    EXPECT_NE(SetGlobalVariable(2), -1);
    LOG("333");
    // control step 4
    WaitProcExitedOK(pid);
    EXPECT_EQ(GetGlobalVariable(), 2);
    EXPECT_NE(DeleteGlobalVariable(), -1);
    if (cpuCnt > 1) {
        ret = sched_setaffinity(0, sizeof(oldMask), &oldMask);
        ASSERT_EQ(ret, 0);
    }
}
