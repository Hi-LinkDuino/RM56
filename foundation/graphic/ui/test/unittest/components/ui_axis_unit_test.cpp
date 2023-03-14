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

#include "components/ui_axis.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;

namespace OHOS {
namespace {
    const int16_t LARGER_VALUE = 10;
    const int16_t SMALLER_VALUE = 2;
}
class UIAxisTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static UIXAxis* axisX_;
    static UIYAxis* axisY_;
};
UIXAxis* UIAxisTest::axisX_ = nullptr;
UIYAxis* UIAxisTest::axisY_ = nullptr;

void UIAxisTest::SetUpTestCase(void)
{
    if (axisX_ == nullptr) {
        axisX_ = new UIXAxis();
    }
    if (axisY_ == nullptr) {
        axisY_ = new UIYAxis();
    }
}

void UIAxisTest::TearDownTestCase(void)
{
    if (axisX_ != nullptr) {
        delete axisX_;
        axisX_ = nullptr;
    }
    if (axisY_ != nullptr) {
        delete axisY_;
        axisY_ = nullptr;
    }
}
/**
 * @tc.name: UIAxisGetViewType_001
 * @tc.desc: Verify GetViewType function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIAxisTest, UIAxisGetViewType_001, TestSize.Level1)
{
    if (axisX_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(axisX_->GetViewType(), UI_AXIS);
}

/**
 * @tc.name: UIAxisSetLineColor_001
 * @tc.desc: Verify SetLineColor function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIAxisTest, UIAxisSetLineColor_001, TestSize.Level1)
{
    if (axisY_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    axisY_->SetLineColor(Color::Gray());
    EXPECT_EQ(axisY_->GetStyle(STYLE_LINE_COLOR), Color::Gray().full);
}

/**
 * @tc.name: UIXAxisSetDataRange_001
 * @tc.desc: Verify SetDataRange function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIAxisTest, UIXAxisSetDataRange_001, TestSize.Level0)
{
    if (axisX_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(axisX_->SetDataRange(SMALLER_VALUE, LARGER_VALUE), true);
}

/**
 * @tc.name: UIXAxisSetDataRange_002
 * @tc.desc: Verify SetDataRange function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIAxisTest, UIXAxisSetDataRange_002, TestSize.Level1)
{
    if (axisX_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(axisX_->SetDataRange(LARGER_VALUE, SMALLER_VALUE), false);
}

/**
 * @tc.name: UIYAxisSetDataRange_001
 * @tc.desc: Verify SetDataRange function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIAxisTest, UIYAxisSetDataRange_001, TestSize.Level1)
{
    if (axisY_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(axisY_->SetDataRange(SMALLER_VALUE, LARGER_VALUE), true);
}

/**
 * @tc.name: UIYAxisSetDataRange_002
 * @tc.desc: Verify SetDataRange function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIAxisTest, UIYAxisSetDataRange_002, TestSize.Level1)
{
    if (axisY_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(axisY_->SetDataRange(LARGER_VALUE, SMALLER_VALUE), false);
}
} // namespace OHOS
