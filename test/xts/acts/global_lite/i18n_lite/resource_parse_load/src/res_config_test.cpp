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
#include "res_config_test.h"

#include <climits>
#include <cstring>
#include <gtest/gtest.h>

#include "hap_resource.h"
#include "res_config.h"
#include "resource_manager_impl.h"
#include "test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

class ResConfigTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void ResConfigTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void ResConfigTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ResConfigTest::SetUp()
{
}

void ResConfigTest::TearDown()
{
}

/*
 * @tc.name: ResConfigFuncTest001
 * @tc.desc: Test Config function, non file case.
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigTest, ResConfigFuncTest001, TestSize.Level1)
{
    ResConfigImpl *rc = new ResConfigImpl;
    rc->SetLocaleInfo("en", nullptr, "AU");
    ResConfigImpl *current = new ResConfigImpl;
    current->SetLocaleInfo("en", nullptr, "GB");
    ResConfigImpl *target = new ResConfigImpl;
    target->SetLocaleInfo("zh", nullptr, "CN");
    EXPECT_TRUE(rc->Match(current));
    EXPECT_TRUE(rc->Match(target) == false);
    delete target;
    delete current;
    delete rc;
};