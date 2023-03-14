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

#include "condition_arbitrator_test.h"
#include "condition_arbitrator.h"
#include "product_adapter.h"
#include "system_info.h"
#include "test_common.h"

namespace OHOS {
namespace ACELite {
void ConditionArbitratorTest::SetUp()
{
    // set the device with 960px, height 480px
    const uint16_t screenHeight = 480;
    const uint16_t screenWidth = 960;
    ProductAdapter::SetScreenSize(screenWidth, screenHeight);
    ProductAdapter::InitDeviceInfo("smartVision");
    SystemInfo::GetInstance().Initialize();
}

/**
 * @tc.name: MaxWidthTest001
 * @tc.desc: test the condition max-width
 */
HWTEST_F(ConditionArbitratorTest, MaxWidthTest001, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. test the condition max-width is 980
     * @tc.expected: step1. the condition check result is positive
     */
    ConditionArbitrator conditionArbitrator;
    EXPECT_TRUE(conditionArbitrator.Decide("(max-width : 980)"));

    EXPECT_TRUE(conditionArbitrator.Decide("(  max-width : 980px)"));

    /**
     * @tc.steps: step2. the condition max-width is 900
     * @tc.expected: step2. the condition check result is nagtive
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(max-width : 900)"));

    /**
     * @tc.steps: step3. the condition max-width is test
     * @tc.expected: step3. the condition check result is nagtive
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(max-width : test"));
    TDD_CASE_END();
}

/**
 * @tc.desc:MinWidthTest002
 * @tc.expected: test the min-width condition
 */
HWTEST_F(ConditionArbitratorTest, MinWidthTest002, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. test the condition min-width is 980
     * @tc.expected: step2. the condition check result is nagtive
     */
    ConditionArbitrator conditionArbitrator;
    EXPECT_FALSE(conditionArbitrator.Decide("(min-width : 980)"));

    /**
     * @tc.steps: step1. the condition min-width is 900
     * @tc.expected: step1. the condition check result is positive
     */
    EXPECT_TRUE(conditionArbitrator.Decide("(min-width: 900)"));

    /**
     * @tc.steps: step2. the condition min-width is test
     * @tc.expected: step2. the condition check result is nagtive
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(min-width: test)"));

    TDD_CASE_END();
}

/**
 * @tc.name: WidthTest003
 * @tc.desc: test the condition width
 */
HWTEST_F(ConditionArbitratorTest, WidthTest003, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. set the condition width is 980
     * @tc.expected: step2. the condition is false
     */
    ConditionArbitrator conditionArbitrator;
    EXPECT_FALSE(conditionArbitrator.Decide("(width: 980)"));

    /**
     * @tc.steps: step2. set the condition width is 900
     * @tc.steps: step2. the condition is false
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(width: 900)"));

    /**
     * @tc.steps: step3. set the condition width is 960
     * @tc.expected: step3. the condition is true
     */
    EXPECT_TRUE(conditionArbitrator.Decide("(width: 960)"));

    /**
     * @tc.steps: step4. set the condition is test
     * @tc.expected: step4. the condition is false
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(width: test)"));

    TDD_CASE_END();
}

/**
 * @tc.name: maxHeightTest004
 * @tc.desc: test the condition max-width
 */
HWTEST_F(ConditionArbitratorTest, MaxHeightTest004, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. test the condition max-height is 500
     * @tc.expected: step1. the condition check result is positive
     */
    ConditionArbitrator conditionArbitrator;
    EXPECT_TRUE(conditionArbitrator.Decide("(max-height : 500)"));

    /**
     * @tc.steps: step2. the condition max-width is 400
     * @tc.expected: step2. the condition check result is nagtive
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(max-height : 400)"));

    /**
     * @tc.steps: step3. the condition max-height is test
     * @tc.expected: step3. the condition check result is nagtive
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(max-height : test"));
    TDD_CASE_END();
}

/**
 * @tc.desc:MinHeightTest005
 * @tc.expected: test the min-height condition
 */
HWTEST_F(ConditionArbitratorTest, MinHeightTest005, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. test the condition min-height is 480
     * @tc.expected: step2. the condition check result is nagtive
     */
    ConditionArbitrator conditionArbitrator;
    EXPECT_TRUE(conditionArbitrator.Decide("(min-height : 480)"));

    /**
     * @tc.steps: step1. the condition min-height is 400
     * @tc.expected: step1. the condition check result is positive
     */
    EXPECT_TRUE(conditionArbitrator.Decide("(min-height: 400)"));

    /**
     * @tc.steps: step2. the condition min-height is test
     * @tc.expected: step2. the condition check result is nagtive
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(min-height: test)"));

    TDD_CASE_END();
}

/**
 * @tc.name: HeightTest006
 * @tc.desc: test the condition height
 */
HWTEST_F(ConditionArbitratorTest, HeightTest006, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. set the condition height is 500
     * @tc.expected: step2. the condition is false
     */
    ConditionArbitrator conditionArbitrator;
    EXPECT_FALSE(conditionArbitrator.Decide("(height: 500)"));

    /**
     * @tc.steps: step2. set the condition height is 400
     * @tc.steps: step2. the condition is false
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(height: 400)"));

    /**
     * @tc.steps: step3. set the condition height is 480
     * @tc.expected: step3. the condition is true
     */
    EXPECT_TRUE(conditionArbitrator.Decide("(height: 480)"));

    /**
     * @tc.steps: step4. set the condition height is test
     * @tc.expected: step4. the condition is false
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(height: test)"));

    TDD_CASE_END();
}

/**
 * @tc.name: MaxAspectRatioTest007
 * @tc.desc: test the condition max-aspect-ratio
 */
HWTEST_F(ConditionArbitratorTest, MaxAspectRatioTest007, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. test the condition max-aspect-ratio is 3
     * @tc.expected: step1. the condition check result is positive
     */
    ConditionArbitrator conditionArbitrator;
    EXPECT_TRUE(conditionArbitrator.Decide("(max-aspect-ratio : 3)"));

    /**
     * @tc.steps: step2. the condition max-aspect-ratio is 1
     * @tc.expected: step2. the condition check result is nagtive
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(max-aspect-ratio : 1)"));

    /**
     * @tc.steps: step3. the condition max-aspect-ratio is test
     * @tc.expected: step3. the condition check result is nagtive
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(max-aspect-ratio : test"));
    TDD_CASE_END();
}

/**
 * @tc.desc:MinAspectRatioTest008
 * @tc.expected: test the min-aspect-ratio condition
 */
HWTEST_F(ConditionArbitratorTest, MinAspectRatioTest008, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. test the condition min-aspect-ratio is 980
     * @tc.expected: step2. the condition check result is nagtive
     */
    ConditionArbitrator conditionArbitrator;
    EXPECT_FALSE(conditionArbitrator.Decide("(min-aspect-ratio : 3)"));

    /**
     * @tc.steps: step1. the condition min-aspect-ratio is 1
     * @tc.expected: step1. the condition check result is positive
     */
    EXPECT_TRUE(conditionArbitrator.Decide("(min-aspect-ratio: 1)"));

    /**
     * @tc.steps: step2. the condition min-aspect-ratio is test
     * @tc.expected: step2. the condition check result is nagtive
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(min-aspect-ratio: test)"));

    TDD_CASE_END();
}

/**
 * @tc.name: AspectRatioTest009
 * @tc.desc: test the condition width
 */
HWTEST_F(ConditionArbitratorTest, AspectRatioTest009, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. set the condition aspect-ratio is 3
     * @tc.expected: step2. the condition is false
     */
    ConditionArbitrator conditionArbitrator;
    EXPECT_FALSE(conditionArbitrator.Decide("(aspect-ratio: 3)"));

    /**
     * @tc.steps: step2. set the condition aspect-ratio is 1
     * @tc.steps: step2. the condition is false
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(aspect-ratio: 1)"));

    /**
     * @tc.steps: step3. set the condition aspect-ratio is 2
     * @tc.expected: step3. the condition is true
     */
    EXPECT_TRUE(conditionArbitrator.Decide("(aspect-ratio : 2)"));

    /**
     * @tc.steps: step4. set the condition aspect-ratio is test
     * @tc.expected: step4. the condition is false
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(aspect-ratio: test)"));

    TDD_CASE_END();
}

/**
 * @tc.desc:AspectRatioTest010
 * @tc.expected: test the aspect-ratio condition checking base on float values
 */
HWTEST_F(ConditionArbitratorTest, AspectRatioTest010, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. test the condition min-aspect-ratio is 980
     * @tc.expected: step2. the condition check result is nagtive
     */
    ConditionArbitrator conditionArbitrator;
    // prepare one float screen aspect ratio, 0.6140350877192982
    uint16_t screenWidth = 280;
    uint16_t screenHeight = 456;
    ProductAdapter::SetScreenSize(screenWidth, screenHeight);
    SystemInfo::GetInstance().Initialize();
    EXPECT_FALSE(conditionArbitrator.Decide("(min-aspect-ratio : 0.615)"));

    /**
     * @tc.steps: step1. the condition min-aspect-ratio is 1
     * @tc.expected: step1. the condition check result is positive
     */
    screenWidth = 456;
    screenHeight = 280;
    ProductAdapter::SetScreenSize(screenWidth, screenHeight);
    SystemInfo::GetInstance().Initialize(); // aspect ratio: 1.628571428571429
    EXPECT_TRUE(conditionArbitrator.Decide("(max-aspect-ratio: 1.628566)"));

    /**
     * @tc.steps: step2. the condition min-aspect-ratio is test
     * @tc.expected: step2. the condition check result is nagtive
     */
    screenWidth = 300;
    screenHeight = 466;
    ProductAdapter::SetScreenSize(screenWidth, screenHeight);
    SystemInfo::GetInstance().Initialize(); // aspect ratio: 0.6437768240343348
    EXPECT_FALSE(conditionArbitrator.Decide("(aspect-ratio: 0.6)"));

    TDD_CASE_END();
}

/**
 * @tc.name: DeviceTypeTest010
 * @tc.desc: test the device type
 */
HWTEST_F(ConditionArbitratorTest, DeviceTypeTest011, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. test the device type value smartVision
     * @tc.expected: step1. the condition is success
     */
    ConditionArbitrator conditionArbitrator;
    EXPECT_TRUE(conditionArbitrator.Decide("(device-type: smartVision)"));

    /**
     * @tc.steps: step2. set the condition device-type is liteWearable
     * @tc.expected: step2. the condition is false
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(device-type: liteWearable)"));

    /**
     * @tc.steps: step3. set the condition device-type is testDevice
     * @tc.expected: step3. the condition is false
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(device-type: testDevice)"));

    TDD_CASE_END();
}

/**
 * @tc.name: RoundScreenTest011
 * @tc.desc: test the round screen condition
 */
HWTEST_F(ConditionArbitratorTest, RoundScreenTest012, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. set the condition round screen true
     * @tc.expected: step1. the condition is false
     */
    ConditionArbitrator conditionArbitrator;
    EXPECT_FALSE(conditionArbitrator.Decide("(round-screen: true)"));

    /**
     * @tc.steps: step2. set the condition round screen false
     * @tc.expected: step2. the condition is true
     */
    EXPECT_TRUE(conditionArbitrator.Decide("(round-screen: false)"));

    /**
     * @tc.steps: step3. set the condition round screen true
     * @tc.expected: step3. the condition is false
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(round-screen: TRUE)"));

    /**
     * @tc.steps: step4. set the condition round screen false
     * @tc.expected: step4. the condition is true
     */
    EXPECT_TRUE(conditionArbitrator.Decide("(round-screen: FALSE)"));

    /**
     * @tc.steps: step5. set the condition round screen false
     * @tc.expected: step5. the condition is true
     */
    EXPECT_TRUE(conditionArbitrator.Decide("(round-screen: 0)"));

    /**
     * @tc.steps: step6. set the condition round screen true
     * @tc.expected: step6. the condition is false
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(round-screen: 1)"));

    /**
     * @tc.steps: step7. the condition round screen testDevice
     * @tc.expected: step7. the condition is false
     */
    EXPECT_FALSE(conditionArbitrator.Decide("(round-screen: testDevice)"));

    TDD_CASE_END();
}

/**
 * @tc.name: CombineConditionTest012
 * @tc.desc:test the combine condition
 */
HWTEST_F(ConditionArbitratorTest, CombineConditionTest013, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. set the condition is screen and (width:960) and (min-height:480)
     * @tc.expected: step1. the condition is true
     */
    ConditionArbitrator conditionArbitrator;
    EXPECT_TRUE(conditionArbitrator.Decide("screen and (width: 960) and (min-height: 480)"));

    /**
     * @tc.steps: step2. set the condition is screen and (max-width : 1028) and (height:480)
     * @tc.expected: step2. the condition is true
     */
    EXPECT_TRUE(conditionArbitrator.Decide(" (max-width: 1028) and (height: 480)"));

    /**
     * @tc.steps: step3. set the condition is screen and (min-width : 480) and (max-height : 320)
     * @tc.expected: step3. the condition is false
     */
    EXPECT_FALSE(conditionArbitrator.Decide("and (min-width: 480) and (max-height: 320)"));

    TDD_CASE_END();
}

/**
 * @tc.name:CombineConditionTest013
 * @tc.desc:test the combine condition
 */
HWTEST_F(ConditionArbitratorTest, CombineConditionTest014, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. set the condition screen and min-aspect-ratio 1 and round screen true
     * and device-type liteWerable
     * @tc.expected: step1. the condition is false
     */
    ConditionArbitrator conditionArbitrator;
    EXPECT_FALSE(conditionArbitrator.Decide(
        "screen and (min-aspect-ratio: 1) and (round-screen: true) and (device-type: liteWearable)"));

    /**
     * @tc.steps: step2. set the condition screen and aspect-ratio 2 and round-screen false
     * and the device-type smartVision
     * @tc.expected: step2. the condition is flase
     */
    EXPECT_FALSE(conditionArbitrator.Decide(
        "screen and (min-aspect-ratio: 1) and (round-screen: true) and (device-type: smartVision)"));

    /**
     * @tc.steps: step3. set the condition screen and max-aspect-ratio 3 and round-screen false
     * and device-type liteWearable
     * @tc.expected: the condition is true
     */
    EXPECT_TRUE(conditionArbitrator.Decide(
        "screen and (max-aspect-ratio: 3) and (device-type: smartVision) and (round-screen: false)"));
    TDD_CASE_END();
}

#ifndef TDD_ASSERTIONS
void ConditionArbitratorTest::RunTests()
{
    MaxWidthTest001();
    MinWidthTest002();
    WidthTest003();
    MaxHeightTest004();
    MinHeightTest005();
    HeightTest006();
    MaxAspectRatioTest007();
    MinAspectRatioTest008();
    AspectRatioTest009();
    AspectRatioTest010();
    DeviceTypeTest011();
    RoundScreenTest012();
    CombineConditionTest013();
    CombineConditionTest014();
}
#endif
}
}
