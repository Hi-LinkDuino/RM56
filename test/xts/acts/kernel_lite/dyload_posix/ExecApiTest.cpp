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

class ExecApiTest : public testing::Test {
};

/**
 * @tc.number   SUB_KERNEL_DL_API_EXECL_0100
 * @tc.name     execl api test
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ExecApiTest, testExecl, Function | MediumTest | Level1)
{
    char* resELF = RES_DIR_DYLOAD "executor2";

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        int rt = execl(resELF, "1", "!@#$%^&*()_+", NULL);
        if (rt == -1) {
            PANIC("execl failed, errno=%d\n", errno);
        }
        exit(0);
    }
    // parent
    WaitProcExitedOK(pid);
}

/**
 * @tc.number   SUB_KERNEL_DL_API_EXECLP_0100
 * @tc.name     execlp api test
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ExecApiTest, testExeclp, Function | MediumTest | Level1)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        putenv("PATH=" RES_DIR_DYLOAD);
        int rt = execlp("executor2", "1", "!@#$%^&*()_+", NULL);
        if (rt == -1) {
            PANIC("execlp failed, errno=%d\n", errno);
        }
        LOG("Error, should never get here.");
        exit(1);
    }
    // parent
    WaitProcExitedOK(pid);
}

/**
 * @tc.number   SUB_KERNEL_DL_API_EXECLE_0100
 * @tc.name     execle api test
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ExecApiTest, testExecle, Function | MediumTest | Level1)
{
    char* resELF = RES_DIR_DYLOAD "executor1";
    putenv("NAME=Alice");

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        char *env[] = {"NAME=Bob", NULL};
        int rt = execle(resELF, "executor1", "-n", "NAME", "-v", "Bob", NULL, env);
        if (rt == -1) {
            PANIC("execle failed, errno=%d\n", errno);
        }
        LOG("Error, should never get here.");
        exit(1);
    }
    // parent
    WaitProcExitedOK(pid);
}

/**
 * @tc.number   SUB_KERNEL_DL_API_EXECV_0100
 * @tc.name     execv api test
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ExecApiTest, testExecv, Function | MediumTest | Level1)
{
    char* resELF = RES_DIR_DYLOAD "executor1";

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        putenv("NAME=Alice");
        char *arg[] = {"executor1", "-n", "NAME", "-v", "Alice", NULL};
        int rt = execv(resELF, arg);
        if (rt == -1) {
            PANIC("execv failed, errno=%d\n", errno);
        }
        LOG("Error, should never get here.");
        exit(1);
    }
    // parent
    WaitProcExitedOK(pid);
}

/**
 * @tc.number   SUB_KERNEL_DL_API_EXECVP_0100
 * @tc.name     execvp api test
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ExecApiTest, testExecvp, Function | MediumTest | Level1)
{
    putenv("NAME=Alice");

    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "======== Fork Error! =========";
    if (pid == 0) { // child
        putenv("PATH=" RES_DIR_DYLOAD);

        char *arg[] = {"executor1", "-n", "NAME", "-v", "Alice", NULL};
        int rt = execvp("executor1", arg);
        if (rt == -1) {
            PANIC("execvp failed, errno=%d\n", errno);
        }
        LOG("Error, should never get here.");
        exit(1);
    }
    // parent
    WaitProcExitedOK(pid);
}