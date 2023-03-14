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

#include "global_test.h"

#include <gtest/gtest.h>

#include "test_common.h"
#include "utils/errors.h"
#include "utils/string_utils.h"

#define private public

#include "global.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

class GlobalTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void GlobalTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void GlobalTest::TearDownTestCase()
{
    // step 2: input testsuit teardown step
}

void GlobalTest::SetUp()
{
    // step 3: input testcase setup step
}

void GlobalTest::TearDown()
{
    // step 4: input testcase teardown step
}

int GetResId(const std::string &name, ResType resType)
{
    auto rc = new ResConfigImpl;
    rc->SetLocaleInfo("en", nullptr, "US");
    std::string resPath = FormatFullPath(g_resFilePath);
    const char *path = resPath.c_str();

    const HapResource *pResource = HapResource::LoadFromIndex(path, rc);

    int ret = pResource->GetIdByName(name.c_str(), resType);
    delete pResource;
    delete rc;
    return ret;
}

/*
 * @tc.name: GlobalFuncTest001
 * @tc.desc: Test GLOBAL_ConfigLanguage function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(GlobalTest, GlobalFuncTest001, TestSize.Level1)
{
    char lan[10];
    char region[10];

    GLOBAL_ConfigLanguage("en-Latn-US");
    int32_t ret = GLOBAL_GetLanguage(lan, 10);
    ASSERT_EQ(OK, ret);
    ASSERT_EQ(std::string("en"), lan);
    ret = GLOBAL_GetRegion(region, 10);
    ASSERT_EQ(OK, ret);
    ASSERT_EQ(std::string("US"), region);

    GLOBAL_ConfigLanguage("en_Latn_US");
    ret = GLOBAL_GetLanguage(lan, 10);
    ASSERT_EQ(OK, ret);
    ASSERT_EQ(std::string("en"), lan);
    ret = GLOBAL_GetRegion(region, 10);
    ASSERT_EQ(OK, ret);
    ASSERT_EQ(std::string("US"), region);

    GLOBAL_ConfigLanguage("en-US");
    ret = GLOBAL_GetLanguage(lan, 10);
    ASSERT_EQ(OK, ret);
    ASSERT_EQ(std::string("en"), lan);
    ret = GLOBAL_GetRegion(region, 10);
    ASSERT_EQ(OK, ret);
    ASSERT_EQ(std::string("US"), region);

    GLOBAL_ConfigLanguage("en_US");
    ret = GLOBAL_GetLanguage(lan, 10);
    ASSERT_EQ(OK, ret);
    ASSERT_EQ(std::string("en"), lan);
    ret = GLOBAL_GetRegion(region, 10);
    ASSERT_EQ(OK, ret);
    ASSERT_EQ(std::string("US"), region);

    GLOBAL_ConfigLanguage("en");
    ret = GLOBAL_GetLanguage(lan, 10);
    ASSERT_EQ(OK, ret);
    ASSERT_EQ(std::string("en"), lan);
}

/*
 * @tc.name: GlobalFuncTest002
 * @tc.desc: Test GLOBAL_GetValueByName function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(GlobalTest, GlobalFuncTest002, TestSize.Level1)
{
    GLOBAL_ConfigLanguage("en_Latn_US");
    char *values = nullptr;
    int32_t ret = GLOBAL_GetValueByName("app_name", FormatFullPath(g_resFilePath).c_str(), &values);
    // when ret != OK , values has been freed.
    ASSERT_EQ(OK, ret);
    EXPECT_EQ(std::string("App Name"), values);
    free(values);
}

/*
 * @tc.name: GlobalFuncTest003
 * @tc.desc: Test GLOBAL_GetValueById function, file case.
 * @tc.type: FUNC
 */
HWTEST_F(GlobalTest, GlobalFuncTest003, TestSize.Level1)
{
    GLOBAL_ConfigLanguage("en_Latn_US");
    char *values = nullptr;
    int id = GetResId("app_name", ResType::STRING);
    ASSERT_TRUE(id > 0);
    int32_t ret = GLOBAL_GetValueById(static_cast<uint32_t>(id), FormatFullPath(g_resFilePath).c_str(), &values);
    // when ret != OK , values has been freed.
    ASSERT_EQ(OK, ret);
    EXPECT_EQ(std::string("App Name"), values);
    free(values);
}
