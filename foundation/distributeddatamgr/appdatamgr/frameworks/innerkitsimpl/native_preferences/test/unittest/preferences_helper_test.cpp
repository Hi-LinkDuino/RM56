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

#include "preferences_helper.h"

#include <gtest/gtest.h>

#include <cctype>
#include <iostream>
#include <string>

#include "logger.h"
#include "preferences.h"
#include "preferences_errno.h"
#include "preferences_observer.h"

using namespace testing::ext;
using namespace OHOS::NativePreferences;

class PreferencesHelperTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void PreferencesHelperTest::SetUpTestCase(void)
{
}

void PreferencesHelperTest::TearDownTestCase(void)
{
}

void PreferencesHelperTest::SetUp(void)
{
}

void PreferencesHelperTest::TearDown(void)
{
}

/**
 * @tc.name: NativePreferencesHelperTest_001
 * @tc.desc: normal testcase of DeletePreferences
 * @tc.type: FUNC
 * @tc.require: SR000CU2BL
 * @tc.author: xiuhongju
 */
HWTEST_F(PreferencesHelperTest, NativePreferencesHelperTest_001, TestSize.Level1)
{
    int errCode = E_OK;
    std::shared_ptr<Preferences> pref = PreferencesHelper::GetPreferences("/data/test/test_helper", errCode);
    EXPECT_EQ(errCode, E_OK);

    pref->PutInt("key1", 2);
    pref->PutString("key2", "test");
    int ret = pref->FlushSync();
    EXPECT_EQ(ret, E_OK);

    pref = nullptr;

    ret = PreferencesHelper::RemovePreferencesFromCache("/data/test/test_helper");
    EXPECT_EQ(ret, E_OK);

    ret = PreferencesHelper::DeletePreferences("/data/test/test_helper");
    EXPECT_EQ(ret, E_OK);
}
