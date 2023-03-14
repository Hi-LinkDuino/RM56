/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "../../utils/ability_test_helper.h"

using namespace testing::ext;

namespace OHOS {
    constexpr char BUNDLE_NAME[] = "com.huawei.setting";
    constexpr char ABILITY_NAME[] = "SettingMainAbility";

    static Want g_want = {};
    static ElementName g_element = {};

    class PageAbilityTest : public testing::Test {
    public:
        static void SetUpTestCase()
        {
            AbilityTestHelper::Initialize();
        }

        static void TearDownTestCase()
        {
            AbilityTestHelper::UnInitialize();
        }

        void SetUp() override
        {
            SetElementBundleName(&g_element, BUNDLE_NAME);
            SetElementAbilityName(&g_element, ABILITY_NAME);
            SetWantElement(&g_want, g_element);
        }

        void TearDown() override
        {
            ASSERT_TRUE(AbilityTestHelper::TestTerminateApp(g_element.bundleName));
            ASSERT_NE(STATE_ACTIVE, AbilityTestHelper::GetAbilityState(g_element));
            ClearElement(&g_element);
            ClearWant(&g_want);
        }
    };

    /**
     * @tc.name: StartAbilityTest001
     * @tc.desc: test StartAbility.
     * @tc.require: AR000DSMBM 
     * @tc.type: FUNC
     */
    HWTEST_F(PageAbilityTest, startAbility001, TestSize.Level0)
    {
        /**
         * @tc.steps: step1. Start ability.
         * @tc.expected: step1. Start ability success.
         */
        ASSERT_TRUE(AbilityTestHelper::TestStartAbility(g_want));
    }
} // namespace OHOS