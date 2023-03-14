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

#include <dlfcn.h>
#include <gtest/gtest.h>
#include "log.h"
#include "utils.h"
#include "libfs.h"
#include "KernelConstants.h"

using namespace testing::ext;

class DlopenApiTest : public testing::Test {
};

#define RES_DIR_DYLOAD RES_DIR_KERNEL "dyload/"
#define DYLOAD_TEST_DIR "/storage/data/"

/**
 * @tc.number   SUB_KERNEL_DL_API_DLOPEN_0100
 * @tc.name     the dlopen current main elf
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(DlopenApiTest, testDlopenSelf, Function | MediumTest | Level1)
{
    char* testELF = RES_DIR_DYLOAD "api_dlopen_self";

    int rt = RunElf(testELF, NULL, NULL);
    EXPECT_EQ(rt, 0);
}

/**
 * @tc.number   SUB_KERNEL_DL_API_DLOPEN_0200
 * @tc.name     dlopen error test
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(DlopenApiTest, testDlopenError, Function | MediumTest | Level3)
{
    dlerror(); // clear any old error info
    void* h = dlopen(0, 0);
    EXPECT_TRUE(h == NULL) << "dlopen with flag=0 succeed!";

    char* errMsg = dlerror();
    LOG("err_msg:%s", errMsg);
    ASSERT_TRUE(errMsg != NULL) << "dlerror message is empty";

    char *p = strcasestr(errMsg, "invalid mode");
    ASSERT_TRUE(p != NULL) << "dlerror msg check failed";
}

/**
 * @tc.number   SUB_KERNEL_DL_API_DLSYM_0100
 * @tc.name     find self symbol use RTLD_DEFAULT.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(DlopenApiTest, testDlsymSelf, Function | MediumTest | Level1)
{
    char* testELF = RES_DIR_DYLOAD "api_dlsym_self";

    int rt = RunElf(testELF, NULL, NULL);
    EXPECT_EQ(rt, 0);
}

/**
 * @tc.number   SUB_KERNEL_DL_API_DLSYM_0200
 * @tc.name     find symbol in next so use RTLD_NEXT.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(DlopenApiTest, testDlsymNext, Function | MediumTest | Level1)
{
    char* resSO = RES_DIR_DYLOAD "libdso1.so";
    char* newSO = DYLOAD_TEST_DIR "libdso1.so";

    // test SetUp
    ASSERT_EQ(CopyFile(resSO, newSO), 0);
    LOG("SetUp ok");

    char* testELF = RES_DIR_DYLOAD "api_dlsym_next";

    int rt = RunElf(testELF, NULL, NULL);
    EXPECT_EQ(rt, 0);

    // test TearDown
    ASSERT_EQ(RemoveFile(newSO), 0);
    LOG("TearDown ok ");
}
