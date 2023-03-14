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

#include "system_info_tdd_test.h"
#include "product_adapter.h"
#include "securec.h"
#include "system_info.h"
#include "test_common.h"

namespace OHOS {
namespace ACELite {
constexpr static char DEVICE_TYPE_NAME_SMART_VISION[] = "smartVision";

/**
 * @tc.name: ACELite_System_Info_001
 * @tc.desc: test the default device value
 */
HWTEST_F(SystemInfoTddTest, SystemInfoTest001, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1.init the device info
     */
    SystemInfo::GetInstance().Initialize();

    /**
     * @tc.steps: step2. get the device width, height and aspect-ratio
     * @tc.expected: step2. the device width is 454, height is 454, aspect-ratio is 1
     */
    const uint16_t screenHeight = 454;
    const uint16_t screenWidth = 454;
    EXPECT_TRUE(SystemInfo::GetInstance().GetScreenHeight() == screenHeight);
    EXPECT_TRUE(SystemInfo::GetInstance().GetScreenWidth() == screenWidth);
    EXPECT_EQ(SystemInfo::GetInstance().GetAspectRatio(), 1);

    /**
     * @tc.steps: step3. get the device type and the round screen
     * @tc.expected: step3. the device type is liteWearable, the round screen is true
     */
    const char *deviceType = SystemInfo::GetInstance().GetDeviceType();
    EXPECT_NE(deviceType, nullptr);
    if (deviceType != nullptr) {
        EXPECT_EQ(strcmp(deviceType, "smartVision"), 0);
    }
    EXPECT_FALSE(SystemInfo::GetInstance().IsRoundScreen());
    TDD_CASE_END();
}

/**
 * @tc.name ACELite_System_info_002
 * @tc.desc test width/height/aspect-ratio of device is saved or not
 */
HWTEST_F(SystemInfoTddTest, SystemInfoTest002, TestSize.Level0)
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set the device width 960px, height 480px
     */
    const uint16_t screenWidth = 960;
    const uint16_t screenHeight = 480;
    ProductAdapter::SetScreenSize(screenWidth, screenHeight);
    SystemInfo::GetInstance().Initialize();

    /**
     * @tc.steps: step2. get the device width
     * @tc.expected: step2. the device width is 960px
     */
    uint16_t width = SystemInfo::GetInstance().GetScreenWidth();
    EXPECT_TRUE(width == screenWidth);

    /**
     * @tc.steps: step3. get the device height
     * @tc.expected: step3. the device height is 480px
     */
    uint16_t height = SystemInfo::GetInstance().GetScreenHeight();
    EXPECT_TRUE(height == screenHeight);

    /**
     * @tc.steps: step4. get the aspect-ratio value
     * @tc.expected: step4. the aspect-ratio value is 2
     */
    const float aspectRatioValue = 2.0;
    float aspectRatio = SystemInfo::GetInstance().GetAspectRatio();
    EXPECT_EQ(aspectRatio, aspectRatioValue);

    TDD_CASE_END();
}

/**
 * @tc.name: ACELite_System_Info_003
 * @tc.desc: test the device type and the round screen type
 */
HWTEST_F(SystemInfoTddTest, SystemInfoTest003, TestSize.Level0)
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set the device type smartVision, and the round screen false
     */
    ProductAdapter::InitDeviceInfo(DEVICE_TYPE_NAME_SMART_VISION);
    SystemInfo::GetInstance().Initialize();

    /**
     * @tc.steps: step2. get the screen type
     * @tc.expected: step2. the screen type is false
     */
    EXPECT_FALSE(SystemInfo::GetInstance().IsRoundScreen());

    /**
     * @tc.steps: step3. get the device type
     * @tc.expected: step3. the device type is smartVision
     */
    const char *deviceType = SystemInfo::GetInstance().GetDeviceType();
    EXPECT_NE(deviceType, nullptr);
    if (deviceType != nullptr) {
        EXPECT_EQ(strcmp(deviceType, "smartVision"), 0);
    }

    TDD_CASE_END();
}

/**
 * @tc.name: ACELite_System_Info_004
 * @tc.desc: test the width or height is 0
 */
HWTEST_F(SystemInfoTddTest, SystemInfoTest004, TestSize.Level0)
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set the device width 0, device height 100
     */
    uint16_t screenWidth = 0;
    uint16_t screenHeight = 100;
    ProductAdapter::SetScreenSize(screenWidth, screenHeight);
    SystemInfo::GetInstance().Initialize();

    /**
     * @tc.steps: step2. get the device width, height and aspect-ratio
     * @tc.expected: step2. the device width is 0, the device height is 100, the aspect-ratio is 0
     */
    uint16_t width = SystemInfo::GetInstance().GetScreenWidth();
    EXPECT_TRUE(width == screenWidth);
    uint16_t height = SystemInfo::GetInstance().GetScreenHeight();
    EXPECT_TRUE(height == screenHeight);
    float aspectRatio = SystemInfo::GetInstance().GetAspectRatio();
    EXPECT_EQ(aspectRatio, 0);

    /**
     * @tc.steps: step3. set the device width 100, height 0
     */
    screenWidth = 100;
    screenHeight = 0;
    ProductAdapter::SetScreenSize(screenWidth, screenHeight);
    SystemInfo::GetInstance().Initialize();

    /**
     * @tc.steps: step4. get the device width, hright and aspect-ratio
     * @tc.expected: step4. the device width is 100, height is 0, aspect-ratio is 0
     */
    width = SystemInfo::GetInstance().GetScreenWidth();
    EXPECT_TRUE(width == screenWidth);
    height = SystemInfo::GetInstance().GetScreenHeight();
    EXPECT_TRUE(height == screenHeight);
    aspectRatio = SystemInfo::GetInstance().GetAspectRatio();
    EXPECT_EQ(aspectRatio, 0);

    TDD_CASE_END();
}

#ifndef TDD_ASSERTIONS
void SystemInfoTddTest::RunTests()
{
    SystemInfoTest001();
    SystemInfoTest002();
    SystemInfoTest003();
    SystemInfoTest004();
}
#endif
} // namespace ACELite
} // namespace OHOS
