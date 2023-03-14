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

#include "components/ui_chart.h"
#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;

namespace OHOS {
namespace {
    const uint16_t ARRAY_SIZE = 5;
    const uint16_t RADIUS = 5;
    const uint16_t WIDTH = 10;
    const Point POINT_ARRAY[ARRAY_SIZE] = { {0, 2478}, {1, 2600}, {2, 3000}, {3, 3200}, {4, 3500} };
}

class UIChartDataSerialTest : public testing::Test {
public:
    UIChartDataSerialTest() : chartDataSerial_(nullptr), chart_(nullptr) {}
    ~UIChartDataSerialTest() {}
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp(void);
    void TearDowm(void);
    UIChartDataSerial* chartDataSerial_;
    UIChartPillar* chart_;
};

void UIChartDataSerialTest::SetUp(void)
{
    if (chartDataSerial_ == nullptr) {
        chartDataSerial_ = new UIChartDataSerial();
        chartDataSerial_->SetMaxDataCount(ARRAY_SIZE);
    }
    if (chart_ == nullptr) {
        chart_ = new UIChartPillar();
    }
}

void UIChartDataSerialTest::TearDowm(void)
{
    if (chartDataSerial_ != nullptr) {
        delete chartDataSerial_;
        chartDataSerial_ = nullptr;
    }
    if (chart_ != nullptr) {
        delete chart_;
        chart_ = nullptr;
    }
}

void InitPointStyle(UIChartDataSerial::PointStyle& pointStyle)
{
    pointStyle.fillColor = Color::Red();
    pointStyle.radius = RADIUS;
    pointStyle.strokeColor = Color::White();
    pointStyle.strokeWidth = WIDTH;
}

/**
 * @tc.name: UIChartDataSerialSetMaxDataCount_001
 * @tc.desc: Verify SetMaxDataCount function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialSetMaxDataCount_001, TestSize.Level1)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(chartDataSerial_->SetMaxDataCount(1), true);
}

/**
 * @tc.name: UIChartDataSerialAddPoints_001
 * @tc.desc: Verify AddPoints function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialAddPoints_001, TestSize.Level1)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    bool ret = chartDataSerial_->AddPoints(nullptr, 0);
    EXPECT_EQ(ret, false);
    ret = chartDataSerial_->AddPoints(POINT_ARRAY, ARRAY_SIZE);
    EXPECT_EQ(ret, true);
    ret = chartDataSerial_->AddPoints(POINT_ARRAY, 0);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: UIChartDataSerialGetPoint_001
 * @tc.desc: Verify GetPoint function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialGetPoint_001, TestSize.Level1)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Point point;

    bool ret = chartDataSerial_->AddPoints(POINT_ARRAY, ARRAY_SIZE);
    EXPECT_EQ(ret, true);
    ret = chartDataSerial_->GetPoint(1, point);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(point.x, POINT_ARRAY[1].x);
    EXPECT_EQ(point.y, POINT_ARRAY[1].y);
}

/**
 * @tc.name: UIChartDataSerialModifyPoint_001
 * @tc.desc: Verify ModifyPoint function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialModifyPoint_001, TestSize.Level0)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Point point = { 1, 200 }; // 200 : y posetion
    Point point1;

    bool ret = chartDataSerial_->AddPoints(POINT_ARRAY, ARRAY_SIZE);
    EXPECT_EQ(ret, true);
    ret = chartDataSerial_->ModifyPoint(ARRAY_SIZE + 1, point);
    EXPECT_EQ(ret, false);
    ret = chartDataSerial_->ModifyPoint(1, point);
    EXPECT_EQ(ret, true);
    ret = chartDataSerial_->GetPoint(1, point1);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(point1.x, point.x);
    EXPECT_EQ(point1.y, point.y);
}

/**
 * @tc.name: UIChartDataSerialClearData_001
 * @tc.desc: Verify ClearData function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialClearData_001, TestSize.Level1)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Point point;

    bool ret = chartDataSerial_->AddPoints(POINT_ARRAY, ARRAY_SIZE);
    EXPECT_EQ(ret, true);
    chartDataSerial_->ClearData();
    ret = chartDataSerial_->GetPoint(1, point);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: UIChartDataSerialGetDataCount_001
 * @tc.desc: Verify GetDataCount function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialGetDataCount_001, TestSize.Level1)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    bool ret = chartDataSerial_->AddPoints(POINT_ARRAY, ARRAY_SIZE);
    EXPECT_EQ(ret, true);
    uint16_t count = chartDataSerial_->GetDataCount();
    EXPECT_EQ(count, ARRAY_SIZE);
    chartDataSerial_->ClearData();
    count = chartDataSerial_->GetDataCount();
    EXPECT_EQ(count, 0);
}

/**
 * @tc.name: UIChartDataSerialEnableGradient_001
 * @tc.desc: Verify EnableGradient function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialEnableGradient_001, TestSize.Level1)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    chartDataSerial_->EnableGradient(true);
    EXPECT_EQ(chartDataSerial_->IsGradient(), true);
}

/**
 * @tc.name: UIChartDataSerialEnableSmooth_001
 * @tc.desc: Verify EnableSmooth function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialEnableSmooth_001, TestSize.Level1)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    chartDataSerial_->EnableSmooth(true);
    bool ret = chartDataSerial_->IsSmooth();
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: UIChartDataSerialGetPeakIndex_001
 * @tc.desc: Verify GetPeakIndex function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialGetPeakIndex_001, TestSize.Level1)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    bool ret = chartDataSerial_->AddPoints(POINT_ARRAY, ARRAY_SIZE);
    EXPECT_EQ(ret, true);
    uint16_t index = chartDataSerial_->GetPeakIndex();
    EXPECT_EQ(index, 4);
    int16_t value = chartDataSerial_->GetPeakData();
    EXPECT_EQ(value, 3500);
}

/**
 * @tc.name: UIChartDataSerialGetValleyIndex_001
 * @tc.desc: Verify GetValleyIndex function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialGetValleyIndex_001, TestSize.Level1)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    bool ret = chartDataSerial_->AddPoints(POINT_ARRAY, ARRAY_SIZE);
    EXPECT_EQ(ret, true);
    uint16_t index = chartDataSerial_->GetValleyIndex();
    EXPECT_EQ(index, 0);
}

/**
 * @tc.name: UIChartDataSerialGetLatestIndex_001
 * @tc.desc: Verify GetLatestIndex function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialGetLatestIndex_001, TestSize.Level1)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    bool ret = chartDataSerial_->AddPoints(POINT_ARRAY, ARRAY_SIZE);
    EXPECT_EQ(ret, true);
    uint16_t index = chartDataSerial_->GetLatestIndex();
    EXPECT_EQ(index, ARRAY_SIZE - 1);
}

/**
 * @tc.name: UIChartDataSerialSetLastPointIndex_001
 * @tc.desc: Verify SetLastPointIndex function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialSetLastPointIndex_001, TestSize.Level1)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    bool ret = chartDataSerial_->AddPoints(POINT_ARRAY, ARRAY_SIZE);
    EXPECT_EQ(ret, true);
    uint16_t index = chartDataSerial_->GetLastPointIndex();
    EXPECT_EQ(index, 0);
    const uint16_t lastPointIndex = 4;
    chartDataSerial_->SetLastPointIndex(lastPointIndex);
    EXPECT_EQ(chartDataSerial_->GetLastPointIndex(), lastPointIndex);
}

/**
 * @tc.name: UIChartDataSerialSetLineColor_001
 * @tc.desc: Verify SetLineColor function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialSetLineColor_001, TestSize.Level1)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    ColorType color = Color::Red();
    chartDataSerial_->SetLineColor(color);
    EXPECT_EQ(chartDataSerial_->GetLineColor().full, color.full);
}

/**
 * @tc.name: UIChartDataSerialSetFillColor_001
 * @tc.desc: Verify SetFillColor function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialSetFillColor_001, TestSize.Level1)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    ColorType color = Color::Red();
    chartDataSerial_->SetFillColor(color);
    EXPECT_EQ(chartDataSerial_->GetFillColor().full, color.full);
}

/**
 * @tc.name: UIChartDataSerialHidePoint_001
 * @tc.desc: Verify HidePoint function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialHidePoint_001, TestSize.Level1)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const uint16_t index = 3;
    const uint16_t count = 5;
    chartDataSerial_->HidePoint(index, count);
    EXPECT_EQ(chartDataSerial_->GetHideIndex(), index);
    EXPECT_EQ(chartDataSerial_->GetHideCount(), count);
}

/**
 * @tc.name: UIChartDataSerialSetHeadPointStyle_001
 * @tc.desc: Verify SetHeadPointStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialSetHeadPointStyle_001, TestSize.Level0)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    UIChartDataSerial::PointStyle pointStyle;
    InitPointStyle(pointStyle);

    chartDataSerial_->SetHeadPointStyle(pointStyle);
    UIChartDataSerial::PointStyle pointStyle1 = chartDataSerial_->GetHeadPointStyle();
    EXPECT_EQ(pointStyle1.fillColor.full, pointStyle.fillColor.full);
    EXPECT_EQ(pointStyle1.strokeColor.full, pointStyle.strokeColor.full);
    EXPECT_EQ(pointStyle1.strokeWidth, pointStyle.strokeWidth);
    EXPECT_EQ(pointStyle1.radius, pointStyle.radius);
}

/**
 * @tc.name: UIChartDataSerialSetTopPointStyle_001
 * @tc.desc: Verify SetTopPointStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialSetTopPointStyle_001, TestSize.Level0)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    UIChartDataSerial::PointStyle pointStyle;
    InitPointStyle(pointStyle);

    chartDataSerial_->SetTopPointStyle(pointStyle);
    UIChartDataSerial::PointStyle pointStyle1 = chartDataSerial_->GetTopPointStyle();
    EXPECT_EQ(pointStyle1.fillColor.full, pointStyle.fillColor.full);
    EXPECT_EQ(pointStyle1.strokeColor.full, pointStyle.strokeColor.full);
    EXPECT_EQ(pointStyle1.strokeWidth, pointStyle.strokeWidth);
    EXPECT_EQ(pointStyle1.radius, pointStyle.radius);
}

/**
 * @tc.name: UIChartDataSerialSetBottomPointStyle_001
 * @tc.desc: Verify SetBottomPointStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDataSerialSetBottomPointStyle_001, TestSize.Level0)
{
    if (chartDataSerial_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    UIChartDataSerial::PointStyle pointStyle;
    InitPointStyle(pointStyle);

    chartDataSerial_->SetBottomPointStyle(pointStyle);
    UIChartDataSerial::PointStyle pointStyle1 = chartDataSerial_->GetBottomPointStyle();
    EXPECT_EQ(pointStyle1.fillColor.full, pointStyle.fillColor.full);
    EXPECT_EQ(pointStyle1.strokeColor.full, pointStyle.strokeColor.full);
    EXPECT_EQ(pointStyle1.strokeWidth, pointStyle.strokeWidth);
    EXPECT_EQ(pointStyle1.radius, pointStyle.radius);
}

/**
 * @tc.name: UIChartAddDataSerial_001
 * @tc.desc: Verify AddDataSerial function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartAddDataSerial_001, TestSize.Level1)
{
    if (chart_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    bool ret = chart_->AddDataSerial(nullptr);
    EXPECT_EQ(ret, false);
    ret = chart_->AddDataSerial(chartDataSerial_);
    EXPECT_EQ(ret, true);
    ret = chart_->AddDataSerial(chartDataSerial_);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: UIChartDeleteDataSerial_001
 * @tc.desc: Verify DeleteDataSerial function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartDeleteDataSerial_001, TestSize.Level1)
{
    if (chart_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    bool ret = chart_->DeleteDataSerial(nullptr);
    EXPECT_EQ(ret, false);
    ret = chart_->DeleteDataSerial(chartDataSerial_);
    EXPECT_EQ(ret, false);
    ret = chart_->AddDataSerial(chartDataSerial_);
    EXPECT_EQ(ret, true);
    ret = chart_->DeleteDataSerial(chartDataSerial_);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: UIChartClearDataSerial_001
 * @tc.desc: Verify ClearDataSerial function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UIChartDataSerialTest, UIChartClearDataSerial_001, TestSize.Level1)
{
    if (chart_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    bool ret = chart_->AddDataSerial(chartDataSerial_);
    EXPECT_EQ(ret, true);
    chart_->ClearDataSerial();
    ret = chart_->DeleteDataSerial(chartDataSerial_);
    EXPECT_EQ(ret, false);
}
}
