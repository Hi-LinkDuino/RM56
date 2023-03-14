/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "file_ex.h"
#include "gtest/gtest.h"

#include "base/json/json_util.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/test/unittest/cardfrontend/card_test_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const std::string CORRECT_PATH = "/data/test/resource/carddomchart/correct.json";
const std::string ERROR_PATH = "/data/test/resource/carddomchart/error.json";
constexpr int32_t DOM_CHART_INDEX = 1;

} // namespace

class DomCardChartTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DomCardChartTest::SetUpTestCase() {}
void DomCardChartTest::TearDownTestCase() {}
void DomCardChartTest::SetUp() {}
void DomCardChartTest::TearDown() {}

/**
 * @tc.name: DomChartDataParse001
 * @tc.desc: Verify that dom chart can be created successfully and the attr is correct.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardChartTest, DomCardChartDataParse001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. read the correct card template file.
     * @tc.expected: step1. read file successfully.
     */
    std::string index;
    bool isSuccess = LoadStringFromFile(CORRECT_PATH, index);
    ASSERT_TRUE(isSuccess);

    /**
     * @tc.steps: step2. use factory to parse template file.
     * @tc.expected: step2. parse successfully and can get the dom chart.
     */
    auto document = AceType::MakeRefPtr<DOMDocument>(0);
    auto page = AceType::MakeRefPtr<JsAcePage>(0, document, "");
    CardTestFactory::GetInstance().ParseCardDsl(page, index);
    auto domChart = AceType::DynamicCast<DOMChart>(document->GetDOMNodeById(DOM_CHART_INDEX));
    ASSERT_TRUE(domChart != nullptr);
    ASSERT_TRUE(domChart->GetTag() == "chart");

    /**
     * @tc.steps: step3. read chart attr.
     * @tc.expected: step3. chart attr is correct.
     */
    auto chartComponent = AceType::DynamicCast<ChartComponent>(domChart->GetSpecializedComponent());
    ASSERT_TRUE(chartComponent != nullptr);
    auto mainCharts = chartComponent->GetMainCharts();
    ASSERT_EQ(mainCharts.size(), 1UL);
    // MainChart Info.
    auto mainChart = mainCharts[0];

    auto stockColor = mainChart.GetStrokeColor();
    ASSERT_EQ(stockColor, Color::FromString("#0081ff"));
    auto fillColor = mainChart.GetFillColor();
    ASSERT_EQ(fillColor, Color::FromString("#cce5ff"));
    auto lineWidth = mainChart.GetLineWidth();
    ASSERT_EQ(lineWidth, 1);
    auto smoothFlag = mainChart.GetSmoothFlag();
    ASSERT_EQ(smothFlag, true);

    auto dataSets = mainChart.GetData();
    ASSERT_EQ(dataSets.size(), 5UL);

    auto lineInfo = dataSets[0];
    // check SegmentInfo
    auto segmentInfo = lineInfo.GetSegmentInfo();
    ASSERT_EQ(segmentInfo.GetLineType(), LineType::DASHED);
    ASSERT_EQ(segmentInfo.GetSolidWidth(), 10);
    ASSERT_EQ(segmentInfo.GetSegmentColor(), Color::FromString("#4cffffff"));

    // check TextInfo
    auto textInfo = lineInfo.GetTextInfo();
    ASSERT_EQ(textInfo.GetTextValue(), -5);
    ASSERT_EQ(textInfo.GetPlacement(), Placement::TOP);
    ASSERT_EQ(textInfo.GetColor(), Color::FromString("#4cffffff"));
    // check PointInfo
    auto pointInfo = lineInfo.GetPointInfo();
    ASSERT_EQ(pointInfo.GetPointShape(), PointShape::CIRCLE);
    ASSERT_EQ(pointInfo.GetPointSize(), 15);
    ASSERT_EQ(pointInfo.GetPointStrokeWidth(), 5);
    ASSERT_EQ(pointInfo.GetFillColor(), Color::FromString("#4cffffff"));
    ASSERT_EQ(pointInfo.GetStrokeColor(), Color::FromString('#00ffffff'));
    ASSERT_EQ(pointInfo.GetDisplay(), true);

    // check xAxis
    auto horizontalOption = chartComponent->GetHorizontalOption();
    ASSERT_EQ(horizontalOption.min, 0);
    ASSERT_EQ(horizontalOption.max, 4);
    ASSERT_EQ(horizontalOption.display, false);

    // check yAxis
    auto verticalOption = chartComponent->GetVerticalOption();
    ASSERT_EQ(verticalOption.min, -10);
    ASSERT_EQ(verticalOption.max, 40);
    ASSERT_EQ(verticalOption.display, false);
}

/**
 * @tc.name: DomChartDataParse002
 * @tc.desc: Verify that dom chart can be created successfully and the attr is error.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardChartTest, DomCardChartDataParse002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. read the correct card template file.
     * @tc.expected: step1. read file successfully.
     */
    std::string index;
    bool isSuccess = LoadStringFromFile(ERROR_PATH, index);
    ASSERT_TRUE(isSuccess);

    /**
     * @tc.steps: step2. use factory to parse template file.
     * @tc.expected: step2. parse successfully and can get the dom chart.
     */
    auto document = AceType::MakeRefPtr<DOMDocument>(0);
    auto page = AceType::MakeRefPtr<JsAcePage>(0, document, "");
    CardTestFactory::GetInstance().ParseCardDsl(page, index);
    auto domChart = AceType::DynamicCast<DOMChart>(document->GetDOMNodeById(DOM_CHART_INDEX));
    ASSERT_TRUE(domChart != nullptr);
    ASSERT_TRUE(domChart->GetTag() == "chart");

    /**
     * @tc.steps: step3. read chart attr.
     * @tc.expected: step3. chart attr is correct except for the lineOps
     */
    auto chartComponent = AceType::DynamicCast<ChartComponent>(domChart->GetSpecializedComponent());
    ASSERT_TRUE(chartComponent != nullptr);
    auto mainCharts = chartComponent->GetMainCharts();
    ASSERT_EQ(mainCharts.size(), 1UL);
    // MainChart Info.
    auto mainChart = mainCharts[0];

    auto stockColor = mainChart.GetStrokeColor();
    ASSERT_EQ(stockColor, Color::FromString("#0081ff"));
    auto fillColor = mainChart.GetFillColor();
    ASSERT_EQ(fillColor, Color::FromString("#cce5ff"));
    auto lineWidth = mainChart.GetLineWidth();
    ASSERT_EQ(lineWidth, 3);
    auto smoothFlag = mainChart.GetSmoothFlag();
    ASSERT_EQ(smothFlag, true);

    auto dataSets = mainChart.GetData();
    ASSERT_EQ(dataSets.size(), 5UL);

    auto lineInfo = dataSets[0];
    // check SegmentInfo
    auto segmentInfo = lineInfo.GetSegmentInfo();
    ASSERT_EQ(segmentInfo.GetLineType(), LineType::DASHED);
    ASSERT_EQ(segmentInfo.GetSolidWidth(), 10);
    ASSERT_EQ(segmentInfo.GetSegmentColor(), Color::FromString("#4cffffff"));

    // check TextInfo
    auto textInfo = lineInfo.GetTextInfo();
    ASSERT_EQ(textInfo.GetTextValue(), -5);
    ASSERT_EQ(textInfo.GetPlacement(), Placement::TOP);
    ASSERT_EQ(textInfo.GetColor(), Color::FromString("#4cffffff"));
    // check PointInfo
    auto pointInfo = lineInfo.GetPointInfo();
    ASSERT_EQ(pointInfo.GetPointShape(), PointShape::CIRCLE);
    ASSERT_EQ(pointInfo.GetPointSize(), 15);
    ASSERT_EQ(pointInfo.GetPointStrokeWidth(), 5);
    ASSERT_EQ(pointInfo.GetFillColor(), Color::FromString("#4cffffff"));
    ASSERT_EQ(pointInfo.GetStrokeColor(), Color::FromString('#00ffffff'));
    ASSERT_EQ(pointInfo.GetDisplay(), true);

    // check xAxis which should be default
    auto horizontalOption = chartComponent->GetHorizontalOption();
    ASSERT_EQ(horizontalOption.min, 0);
    ASSERT_EQ(horizontalOption.max, 100);
    ASSERT_EQ(horizontalOption.display, false);

    // check yAxis which should be default
    auto verticalOption = chartComponent->GetVerticalOption();
    ASSERT_EQ(verticalOption.min, 0);
    ASSERT_EQ(verticalOption.max, 100);
    ASSERT_EQ(verticalOption.display, false);
}

/**
 * @tc.name: DomChartDataParse003
 * @tc.desc: Verify that dom chart can be created successfully and updated successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardChartTest, DomCardChartDataParse003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. read the correct card template file.
     * @tc.expected: step1. read file successfully.
     */
    std::string index;
    bool isSuccess = LoadStringFromFile(CORRECT_PATH, index);
    ASSERT_TRUE(isSuccess);

    /**
     * @tc.steps: step2. use factory to parse template file.
     * @tc.expected: step2. parse successfully and can get the dom chart.
     */
    auto document = AceType::MakeRefPtr<DOMDocument>(0);
    auto page = AceType::MakeRefPtr<JsAcePage>(0, document, "");
    auto params = "{\n"
                  "  \"lineOps\": {\n"
                  "    \"xAxis\": {\n"
                  "      \"min\": 2,\n"
                  "      \"max\": 6,\n"
                  "      \"display\": true\n"
                  "    },\n"
                  "    \"yAxis\": {\n"
                  "      \"min\": -20,\n"
                  "      \"max\": 80,\n"
                  "      \"display\": true\n"
                  "    },\n"
                  "    \"series\": {\n"
                  "      \"lineStyle\": {\n"
                  "        \"width\": \"3px\",\n"
                  "        \"smooth\": true\n"
                  "      }\n"
                  "    }\n"
                  "  }\n"
                  "}";
    CardTestFactory::GetInstance().ParseCardDslWithParams(page, index, params);
    auto domChart = AceType::DynamicCast<DOMChart>(document->GetDOMNodeById(DOM_CHART_INDEX));
    ASSERT_TRUE(domChart != nullptr);
    ASSERT_TRUE(domChart->GetTag() == "chart");
    /**
     * @tc.steps: step3. read chart attr.
     * @tc.expected: step3. chart attr is correct.
     */
    auto chartComponent = AceType::DynamicCast<ChartComponent>(domChart->GetSpecializedComponent());
    ASSERT_TRUE(chartComponent != nullptr);

    // check xAxis which should be updated
    auto horizontalOption = chartComponent->GetHorizontalOption();
    ASSERT_EQ(horizontalOption.min, 2);
    ASSERT_EQ(horizontalOption.max, 6);
    ASSERT_EQ(horizontalOption.display, true);

    // check yAxis which should be updated
    auto verticalOption = chartComponent->GetVerticalOption();
    ASSERT_EQ(verticalOption.min, -20);
    ASSERT_EQ(verticalOption.max, 80);
    ASSERT_EQ(verticalOption.display, true);
}
} // namespace OHOS::Ace::Framework
