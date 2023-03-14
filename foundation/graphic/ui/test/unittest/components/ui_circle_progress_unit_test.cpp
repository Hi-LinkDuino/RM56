/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "components/ui_circle_progress.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    constexpr uint16_t MAX_ANGLE_VALUE = 360;
    constexpr uint16_t MIN_ANGLE_VALUE = 0;
}
class UICircleProgressTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static UICircleProgress* circleProgress_;
};

UICircleProgress* UICircleProgressTest::circleProgress_ = nullptr;

void UICircleProgressTest::SetUpTestCase()
{
    if (circleProgress_ == nullptr) {
        circleProgress_ = new UICircleProgress();
    }
}

void UICircleProgressTest::TearDownTestCase()
{
    if (circleProgress_ != nullptr) {
        delete circleProgress_;
        circleProgress_ = nullptr;
    }
}

/**
 * @tc.name: UICircleProgressGetViewType_001
 * @tc.desc: Verify GetViewType function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UICircleProgressTest, UICircleProgressGetViewType_001, TestSize.Level1)
{
    if (circleProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(circleProgress_->GetViewType(), UI_CIRCLE_PROGRESS);
}

/**
 * @tc.name: UICircleProgressSetRadius_001
 * @tc.desc: Verify SetRadius function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UICircleProgressTest, UICircleProgressSetRadius_001, TestSize.Level1)
{
    if (circleProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const uint16_t radius = 1;

    circleProgress_->SetRadius(radius);
    EXPECT_EQ(circleProgress_->GetRadius(), radius);
}

/**
 * @tc.name: UICircleProgressSetCenterPosition_001
 * @tc.desc: Verify SetCenterPosition function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UICircleProgressTest, UICircleProgressSetCenterPosition_001, TestSize.Level1)
{
    if (circleProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const int16_t presetLength = 100;
    const int16_t centerX = 50;
    const int16_t centerY = 50;

    circleProgress_->Resize(presetLength, presetLength);
    circleProgress_->SetCenterPosition(centerX, centerY);
    EXPECT_EQ(circleProgress_->GetCenterPosition().x, centerX);
    EXPECT_EQ(circleProgress_->GetCenterPosition().y, centerY);
}

/**
 * @tc.name: UICircleProgressSetLineColor_001
 * @tc.desc: Verify SetLineColor function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UICircleProgressTest, UICircleProgressSetLineColor_001, TestSize.Level1)
{
    if (circleProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    circleProgress_->SetLineColor(Color::Red());
    EXPECT_EQ(circleProgress_->GetForegroundStyle(STYLE_LINE_COLOR), Color::Red().full);
}

/**
 * @tc.name: UICircleProgressSetAngle_001
 * @tc.desc: Verify SetAngle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UICircleProgressTest, UICircleProgressSetStartAngle_001, TestSize.Level0)
{
    if (circleProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const int16_t startAngle = 0;
    const int16_t endAngle = 90;

    EXPECT_EQ(circleProgress_->GetStartAngle(), MIN_ANGLE_VALUE);
    circleProgress_->SetStartAngle(startAngle);
    EXPECT_EQ(circleProgress_->GetStartAngle(), startAngle);

    EXPECT_EQ(circleProgress_->GetEndAngle(), MAX_ANGLE_VALUE);
    circleProgress_->SetEndAngle(endAngle);
    EXPECT_EQ(circleProgress_->GetEndAngle(), endAngle);
}
} // namespace OHOS
