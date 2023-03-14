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

#include <gtest/gtest.h>
#include "string_ex.h"
#include "securec.h"
using namespace testing::ext;
using namespace OHOS;

class UtilsSecurecTest : public testing::Test
{
public :
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void UtilsSecurecTest::SetUpTestCase(void)
{
    // step 2: input testsuit setup step
}

void UtilsSecurecTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void UtilsSecurecTest::SetUp(void)
{
    // step 3: input testcase setup step
}

void UtilsSecurecTest::TearDown(void)
{
    // step 3: input testcase teardown step
}

HWTEST_F(UtilsSecurecTest, test_memset_s_01, TestSize.Level0)
{
    char cBase[20];
    errno_t result = memset_s((void*)cBase, sizeof(char) * 20, 1, sizeof(char) * 20);
    EXPECT_EQ(result, 0);
}

HWTEST_F(UtilsSecurecTest, test_memset_s_02, TestSize.Level0)
{
    char cBase[20];
    errno_t result = memset_s((void*)cBase, sizeof(char) * 20, 1, sizeof(char) * 21);
    EXPECT_NE(result, 0);
}

HWTEST_F(UtilsSecurecTest, test_memcpy_s_01, TestSize.Level0)
{
    char cBase[20] = "memcpy_s";
    char cTemp[20];
    errno_t result = memcpy_s((void*)cTemp, sizeof(char) * 20, cBase, sizeof(cBase));
    EXPECT_EQ(result, 0);
    EXPECT_EQ(0, strcmp(cTemp, cBase));
}

HWTEST_F(UtilsSecurecTest, test_memcpy_s_02, TestSize.Level0)
{
    char cBase[20] = "memcpy_s";
    char cTemp[5];
    errno_t result = memcpy_s((void*)cTemp, sizeof(char) * 5, cBase, sizeof(cBase));
    EXPECT_NE(result, 0);
}

HWTEST_F(UtilsSecurecTest, test_strcpy_s_01, TestSize.Level0)
{
    const char cBase[] = "strcpy_base";
    char cTemp[20];
    errno_t result = strcpy_s(cTemp, sizeof(cTemp), cBase);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(0, strcmp(cTemp, cBase));
}

HWTEST_F(UtilsSecurecTest, test_strcpy_s_02, TestSize.Level0)
{
    const char cBase[] = "strcpy_base";
    char cTemp[10];
    errno_t result = strcpy_s(cTemp, sizeof(cTemp), cBase);
    EXPECT_NE(result, 0);
}

HWTEST_F(UtilsSecurecTest, test_strncpy_s_01, TestSize.Level0)
{
    const char cBase[] = "strcpy_base";
    char cTemp[20];
    errno_t result = strncpy_s(cTemp, sizeof(cTemp), cBase, sizeof(cBase));
    EXPECT_EQ(result, 0);
    EXPECT_EQ(0, strcmp(cTemp, cBase));
}

HWTEST_F(UtilsSecurecTest, test_strncpy_s_02, TestSize.Level0)
{
    const char cBase[] = "strcpy_base";
    char cTemp[10];
    errno_t result = strncpy_s(cTemp, sizeof(cTemp), cBase, (sizeof(cTemp) -1));
    EXPECT_EQ(result, 0);
    result = strncpy_s(cTemp, sizeof(cTemp), cBase, sizeof(cBase));
    EXPECT_NE(result, 0);
}

HWTEST_F(UtilsSecurecTest, test_strcat_s_01, TestSize.Level0)
{
    const char cBase[] = "strcpy_base";
    char cTemp[20] = "temp ";
    errno_t result = strcat_s(cTemp, sizeof(cTemp), cBase);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(0, strcmp(cTemp, "temp strcpy_base"));
}

HWTEST_F(UtilsSecurecTest, test_strcat_s_02, TestSize.Level0)
{
    const char cBase[] = "strcpy_base";
    char cTemp[10];
    errno_t result = strcat_s(cTemp, sizeof(cTemp), cBase);
    EXPECT_NE(result, 0);
}

HWTEST_F(UtilsSecurecTest, test_sprintf_s_01, TestSize.Level0)
{
    char cBase[64] = { 0 };
    errno_t result = sprintf_s(cBase, sizeof(cBase), "%d", 12345);
    EXPECT_EQ(string(cBase), "12345");
    EXPECT_NE(result, 0);
}
