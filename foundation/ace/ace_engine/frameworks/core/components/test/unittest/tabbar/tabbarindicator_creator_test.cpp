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

#include "gtest/gtest.h"

#include "base/json/json_util.h"
#include "core/components/tab_bar/tab_bar_component.h"
#include "core/components/test/json/tab_bar_creator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const Dimension INDICATOR_WIDTH = Dimension(10.0, DimensionUnit::PX);
const Color INDICATOR_COLOR = Color::FromARGB(255, 100, 100, 100);
const Edge INDICATOR_PADDING = Edge(123.0);

RefPtr<TabBarIndicatorComponent> CreateTabBarIndicatorComponent(const std::string& testJson)
{
    const char* errMsg = nullptr;
    std::unique_ptr<JsonValue> componentJson = JsonUtil::ParseJsonString(testJson.c_str(), &errMsg);
    if (componentJson == nullptr) {
        return nullptr;
    }

    RefPtr<BoxComponent> component = TabBarCreator::CreateIndicatorFromJson(*componentJson);
    if (!component) {
        return nullptr;
    }

    RefPtr<TabBarIndicatorComponent> indicatorComponent = AceType::DynamicCast<TabBarIndicatorComponent>(component);
    return indicatorComponent;
}

} // namespace

class TabBarIndicatorCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void TabBarIndicatorCreatorTest::SetUpTestCase() {}

void TabBarIndicatorCreatorTest::TearDownTestCase() {}

void TabBarIndicatorCreatorTest::SetUp() {}

void TabBarIndicatorCreatorTest::TearDown() {}

/**
 * @tc.name: TabBarIndicatorCreator001
 * @tc.desc: Verify BoxCreator can create Indicator with empty json string.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarIndicatorCreatorTest, TabBarIndicator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of Indicator component with empty json string.
     */
    std::string dslJson = "{}";

    /**
     * @tc.steps: step2. call CreateIndicatorFromJson interface and get Indicator component.
     * @tc.expected: step2. Indicator's all properties are default .
     */
    RefPtr<TabBarIndicatorComponent> indicator = CreateTabBarIndicatorComponent(dslJson);
    EXPECT_TRUE(indicator);
    EXPECT_EQ(indicator->GetIndicatorStyle(), TabBarIndicatorStyle::DEFAULT);
    EXPECT_EQ(indicator->GetIndicatorColor(), DEFAULT_INDICATOR_COLOR);
    EXPECT_EQ(indicator->GetIndicatorWidth(), DEFAULT_INDICATOR_WIDTH);
    EXPECT_EQ(indicator->GetIndicatorPadding(), DEFAULT_INDICATOR_PADDING);

    const auto& backDecoration = indicator->GetBackDecoration();
    EXPECT_EQ(backDecoration->GetBackgroundColor(), DEFAULT_INDICATOR_COLOR);
    EXPECT_TRUE(NearEqual(indicator->GetHeightDimension().Value(), DEFAULT_INDICATOR_WIDTH.Value()));
    EXPECT_EQ(indicator->GetMargin(), DEFAULT_INDICATOR_PADDING);
}

/**
 * @tc.name: TabBarIndicatorCreator002
 * @tc.desc: Verify BoxCreator can create Indicator with default style properties.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarIndicatorCreatorTest, TabBarIndicator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of Indicator component with default style properties.
     */
    std::string dslJson = R"(
        {
            "indicatorWidth": 10,
            "indicatorPadding": {
                "className": "Edge",
                "value": 123.0
            },
            "indicatorColor": {
                "className": "Color",
                "alpha": 255,
                "red": 0,
                "green": 0,
                "blue": 255
            }
        })";

    /**
     * @tc.steps: step2. call CreateIndicatorFromJson interface and get Indicator component.
     * @tc.expected: step2. default style properties are correct set .
     */
    RefPtr<TabBarIndicatorComponent> indicator = CreateTabBarIndicatorComponent(dslJson);
    EXPECT_TRUE(indicator);
    EXPECT_EQ(indicator->GetIndicatorStyle(), TabBarIndicatorStyle::DEFAULT);
    EXPECT_EQ(indicator->GetIndicatorColor(), Color::BLUE);
    EXPECT_EQ(indicator->GetIndicatorWidth(), INDICATOR_WIDTH);
    EXPECT_EQ(indicator->GetIndicatorPadding(), INDICATOR_PADDING);

    const auto& backDecoration = indicator->GetBackDecoration();
    EXPECT_EQ(backDecoration->GetBackgroundColor(), Color::BLUE);
    EXPECT_TRUE(NearEqual(indicator->GetHeightDimension().Value(), INDICATOR_WIDTH.Value()));
}

/**
 * @tc.name: TabBarIndicatorCreator003
 * @tc.desc: Verify BoxCreator can create Indicator with custom style properties.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarIndicatorCreatorTest, TabBarIndicator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of Indicator component with custom style properties.
     */
    std::string dslJson = R"(
        {
            "indicator": {
                "className": "Decoration",
                "border": {
                    "className": "Border",
                    "top": {
                        "className": "BorderEdge",
                        "color": {
                            "className": "Color",
                            "alpha": 255,
                            "red": 100,
                            "green": 100,
                            "blue": 100
                        },
                        "width": 10.0,
                        "style": "solid"
                    }
                }
            },
            "indicatorWidth": 10,
            "indicatorPadding": {
                "className": "Edge",
                "value": 0
            },
            "indicatorColor": {
                "className": "Color",
                "alpha": 255,
                "red": 0,
                "green": 0,
                "blue": 255
            }
        })";

    /**
     * @tc.steps: step2. call CreateIndicatorFromJson interface and get Indicator component.
     * @tc.expected: step2. custom style are correct set and default style properties are not set .
     */
    RefPtr<TabBarIndicatorComponent> indicator = CreateTabBarIndicatorComponent(dslJson);
    EXPECT_TRUE(indicator);
    EXPECT_EQ(indicator->GetIndicatorColor(), DEFAULT_INDICATOR_COLOR);
    EXPECT_EQ(indicator->GetIndicatorWidth(), DEFAULT_INDICATOR_WIDTH);
    EXPECT_EQ(indicator->GetIndicatorPadding(), DEFAULT_INDICATOR_PADDING);
    EXPECT_EQ(indicator->GetIndicatorStyle(), TabBarIndicatorStyle::CUSTOM);
    const Border& border = indicator->GetBackDecoration()->GetBorder();
    EXPECT_EQ(border.Top().GetColor(), INDICATOR_COLOR);
    EXPECT_TRUE(NearEqual(border.Top().GetWidth().Value(), INDICATOR_WIDTH.Value()));
    EXPECT_EQ(border.Top().GetBorderStyle(), BorderStyle::SOLID);
}

} // namespace OHOS::Ace