/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "../../../interfaces/innerkits/abilitymgr_lite/slite/ability_manager_inner.h"

using namespace testing::ext;

using int (*StartCheckFunc)(const char *bundleName);

namespace OHOS {
    class AbilityManagerInnerTest : public testing::Test {
    public:
        void SetUp() override
        {
            printf("AbilityManagerInnerTest setup");
        }

        void TearDown() override
        {
            printf("AbilityManagerInnerTest TearDown");
        }

        void AbilityManagerInnerTest001();
        void AbilityManagerInnerTest002();
        void AbilityManagerInnerTest003();
        void AbilityManagerInnerTest004();
        void AbilityManagerInnerTest005();
        void AbilityManagerInnerTest006();
        void AbilityManagerInnerTest007();
        void AbilityManagerInnerTest008();
        void RunTests();
    };

    void AbilityManagerInnerTest::AbilityManagerInnerTest001()
    {
        printf("AbilityManagerInnerTest setup");
        SetUp();
        StartCheckFunc startChecktCallback = getAbilityCallback();
        auto ret = RegAbilityCallback(startChecktCallback);
        EXPECT_TRUE(ret == 0);
        TearDown();
        printf("--------AbilityManagerInnerTest001 RegAbilityCallback end--------");
    }

    void AbilityManagerInnerTest::AbilityManagerInnerTest002()
    {
        printf("--------AbilityManagerInnerTest002 SchedulerLifecycleDone begin--------");
        SetUp();
        int state = 2;
        auto token = 0;
        auto ret = SchedulerLifecycleDone(token, state);
        EXPECT_TRUE(ret == 0);
        TearDown();
        printf("--------AbilityManagerInnerTest002 SchedulerLifecycleDone end--------");
    }

    void AbilityManagerInnerTest::AbilityManagerInnerTest003()
    {
        printf("--------AbilityManagerInnerTest003 ForceStopBundle begin--------");
        SetUp();
        auto token = 0;
        auto ret = ForceStopBundle(token);
        EXPECT_TRUE(ret == 0);
        TearDown();
        printf("--------AbilityManagerInnerTest003 ForceStopBundle end--------");
    }

    void AbilityManagerInnerTest::AbilityManagerInnerTest004()
    {
        printf("--------AbilityManagerInnerTest004 GetTopAbility begin--------");
        SetUp();
        auto ret = GetTopAbility();
        EXPECT_TRUE(ret == nullptr);
        TearDown();
        printf("--------AbilityManagerInnerTest004 GetTopAbility end--------");
    }

    void AbilityManagerInnerTest::AbilityManagerInnerTest005()
    {
        printf("--------AbilityManagerInnerTest005 ForceStop begin--------");
        SetUp();
        char *bundlename = "bundletest";
        auto ret = ForceStop(bundlename);
        EXPECT_TRUE(ret == 0);
        TearDown();
        printf("--------AbilityManagerInnerTest005 ForceStop end--------");
    }

    void AbilityManagerInnerTest::AbilityManagerInnerTest006()
    {
        printf("--------AbilityManagerInnerTest006 getAbilityCallback begin--------");
        SetUp();
        auto ret = getAbilityCallback();
        EXPECT_TRUE(ret == nullptr);
        TearDown();
        printf("--------AbilityManagerInnerTest006 getAbilityCallback end--------");
    }

    void AbilityManagerInnerTest::AbilityManagerInnerTest007()
    {
        printf("--------AbilityManagerInnerTest007 setCleanAbilityDataFlag begin--------");
        SetUp();
        bool cleanFlag = true;
        setCleanAbilityDataFlag(cleanFlag);
        TearDown();
        printf("--------AbilityManagerInnerTest007 setCleanAbilityDataFlag end--------");
    }

    void AbilityManagerInnerTest::AbilityManagerInnerTest008()
    {
        printf("--------AbilityManagerInnerTest008 getCleanAbilityDataFlag begin--------");
        SetUp();
        auto ret = getCleanAbilityDataFlag();
        EXPECT_TRUE(ret == 0);
        TearDown();
        printf("--------AbilityManagerInnerTest008 getCleanAbilityDataFlag end--------");
    }

    void AbilityManagerInnerTest::RunTests()
    {
        void AbilityManagerInnerTest001();
        void AbilityManagerInnerTest002();
        void AbilityManagerInnerTest003();
        void AbilityManagerInnerTest004();
        void AbilityManagerInnerTest005();
        void AbilityManagerInnerTest006();
        void AbilityManagerInnerTest007();
        void AbilityManagerInnerTest008();
    }
}
