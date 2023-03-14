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

#include <errno.h>
#include <gtest/gtest.h>
#include "log.h"
#include "utils.h"
#include "libfs.h"
#include "KernelConstants.h"

using namespace testing::ext;

#define RES_DIR_DYLOAD RES_DIR_KERNEL "dyload/"

class ExecveTest : public testing::Test {
};

/**
 * @tc.number   SUB_KERNEL_DL_ELF_0100
 * @tc.name     execve fixed address elf, non-pie elf
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ExecveTest, testExecFixedAddressELF, Function | MediumTest | Level1)
{
    char* testELF = RES_DIR_DYLOAD "hello_nopie";
    int rt = RunElf(testELF, NULL, NULL);
    EXPECT_EQ(rt, 0) << "fixed address elf test failed! exitcode=" << rt;
}

/**
 * @tc.number   SUB_KERNEL_DL_ELF_0200
 * @tc.name     execve static link elf
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ExecveTest, testExecStaticELF, Function | MediumTest | Level1)
{
    char* testELF = RES_DIR_DYLOAD "hello_static";
    int rt = RunElf(testELF, NULL, NULL);
    EXPECT_EQ(rt, 0) << "static link elf test failed! exitcode=" << rt;
}


/**
 * @tc.number   SUB_KERNEL_DL_Execve_fname_0100
 * @tc.name     execve parameter1 fname test
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ExecveTest, testParaFilename, Function | MediumTest | Level1)
{
    char* resELF = RES_DIR_DYLOAD "executor1";
    char* testELF1 = resELF;
    char* testELF2 = "/storage/executor2";
    char* testELF3 = "/storage/data/executor3";

    // test SetUp
    ASSERT_EQ(CopyFile(testELF1, testELF2), 0);
    EXPECT_EQ(CopyFile(testELF1, testELF3), 0);
    int rt = chmod(testELF2, 0777);
    if (rt) {
        // chmod may fail if DAC is not enabled, so this failure is not 'record'
        LOG("chmod '%s' failed: rt=%d, errno=%d", testELF2, rt, errno);
    }
    rt = chmod(testELF3, 0777);
    if (rt) {
        LOG("chmod '%s' failed: rt=%d, errno=%d", testELF3, rt, errno);
    }
    char* curPath = GetCurrentPath();
    EXPECT_EQ(chdir("/storage/data"), 0);
    LOG("SetUp ok");

    // test
    rt = RunElf(testELF1, NULL, NULL);
    EXPECT_EQ(rt, 0) << "execve absolute path elf failed! exitcode=" << rt;

    rt = RunElf("./executor3", NULL, NULL);
    EXPECT_EQ(rt, 0) << "execve relative-path1 elf failed! exitcode=" << rt;
    rt = RunElf("../data/executor3", NULL, NULL);
    EXPECT_EQ(rt, 0) << "execve relative-path2 elf failed! exitcode=" << rt;

    rt = RunElf("../executor2", NULL, NULL);
    EXPECT_EQ(rt, 0) << "execve upper-path-file failed! exitcode=" << rt;

    rt = StartExecveError("./executorNoExist", NULL, NULL);
    EXPECT_EQ(rt, ENOENT) << "execve non-exist-elf errno failed! errno=" << rt;

    // test TearDown
    EXPECT_EQ(RemoveFile(testELF2), 0);
    EXPECT_EQ(RemoveFile(testELF3), 0);
    ASSERT_EQ(chdir(curPath), 0);
    LOG("TearDown ok ");
}

/**
 * @tc.number   SUB_KERNEL_DL_Execve_argv_0100
 * @tc.name     execve parameter2 argv test
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ExecveTest, testParaArgv, Function | MediumTest | Level1)
{
    int rt;
    char* resELF = RES_DIR_DYLOAD "executor2";

    char *arg[] = {"1", "!@#$%^&*()_+", NULL};
    rt = RunElf(resELF, arg, NULL);
    EXPECT_FALSE(rt) << "execve elf with normal argv parameter failed !";
    sleep(1);

    char *arg2[] = {"2", "aaaa", "bbbb", NULL};
    rt = RunElf(resELF, arg2, NULL);
    EXPECT_FALSE(rt) << "execve elf with normal argv parameter failed !";
    sleep(1);

    char *arg3[] = {"3", "-f", "/home/result/log.txt", "-d", "!@#$%^&*()_+", NULL};
    rt = RunElf(resELF, arg3, NULL);
    EXPECT_FALSE(rt) << "execve elf with normal argv parameter failed !";
}

/**
 * @tc.number   SUB_KERNEL_DL_Execve_envp_0100
 * @tc.name     execve parameter3 envp test
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ExecveTest, testParaEnvp, Function | MediumTest | Level1)
{
    int rt;
    char* resELF = RES_DIR_DYLOAD "executor2";

    char *arg[] = {"1", "!@#$%^&*()_+", NULL};
    rt = RunElf(resELF, NULL, arg);
    EXPECT_FALSE(rt) << "execve elf with normal envp parameter failed !";
    sleep(1);

    char *arg2[]={"2", "aaaa", "bbbb", NULL};
    rt = RunElf(resELF, NULL, arg2);
    EXPECT_FALSE(rt) << "execve elf with normal envp parameter failed !";
    sleep(1);

    char *arg3[] = {"3", "-f", "/home/result/log.txt", "-d", "!@#$%^&*()_+", NULL};
    rt = RunElf(resELF, NULL, arg3);
    EXPECT_FALSE(rt) << "execve elf with normal envp parameter failed !";
}