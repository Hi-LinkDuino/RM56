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

#include "gtest/gtest.h"

#include "core/components/common/layout/constants.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/common/dom/dom_progress.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

constexpr double PERCENT = 20.0;
constexpr double PERCENT_MAX = 100.0;
constexpr double PERCENT_MIN = 0.0;

} // namespace

class DomProgressTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DomProgressTest::SetUpTestCase() {}
void DomProgressTest::TearDownTestCase() {}
void DomProgressTest::SetUp() {}
void DomProgressTest::TearDown() {}

/**
 * @tc.name: DomProgressCreatorTest001
 * @tc.desc: Test create progress node successfully and progress component create as desire.
 * @tc.type: FUNC
 */
HWTEST_F(DomProgressTest, DomProgressCreatorTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string of DomProgress with two attributes and a style:
     * type, percent, color.
     */
    const std::string progressTest001 = ""
                                        "{                                                  "
                                        "  \"tag\": \"progress\",                           "
                                        "  \"attr\": [{                                     "
                                        "                \"type\" : \"horizontal\"          "
                                        "              },                                   "
                                        "              {"
                                        "                \"percent\" : \"20\"               "
                                        "              },"
                                        "              {"
                                        "                \"secondaryPercent\" : \"20\"      "
                                        "              }],                                  "
                                        "  \"style\":  [{ "
                                        "                \"color\" : \"#12345678\"          "
                                        "              }]"
                                        "}";
    /**
     * @tc.steps: step2. Create progress node with the string.
     * @tc.expected: step2. Progress node and child progress component are created successfully.
     */
    auto progress = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(progressTest001);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(progress);
    RefPtr<ProgressComponent> progressComponent = AceType::DynamicCast<ProgressComponent>(boxChild->GetChild());
    ASSERT_TRUE(progressComponent != nullptr);

    /**
     * @tc.steps: step3. Check the style and attributes of created progress node.
     * @tc.expected: step3. The style and attributes are as expected.
     */
    EXPECT_TRUE(progressComponent->GetType() == ProgressType::LINEAR);
    double percent = progressComponent->GetValue();
    double cachedPercent = progressComponent->GetCachedValue();
    EXPECT_TRUE(NearEqual(percent, PERCENT));
    EXPECT_TRUE(NearEqual(cachedPercent, PERCENT));
    auto progressTrack = progressComponent->GetTrack();
    ASSERT_TRUE(progressTrack != nullptr);
    Color selectedColor = progressTrack->GetSelectColor();
    EXPECT_EQ(selectedColor, Color::FromString("#12345678"));
}

/**
 * @tc.name: DomProgressCreatorTest002
 * @tc.desc: Set percent,secondaryPercent = 100 when percent > 100 secondaryPercent > 100.
 * @tc.type: FUNC
 */
HWTEST_F(DomProgressTest, DomProgressCreatorTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string of DomProgress with percent > 100 and secondarypercent > 100
     */
    const std::string progressTest002 = ""
                                        "{                                                  "
                                        "  \"tag\": \"progress\",                           "
                                        "  \"attr\": [{                                     "
                                        "                \"type\" : \"horizontal\"          "
                                        "              },                                   "
                                        "              {"
                                        "                \"percent\" : \"200\"              "
                                        "              },                                   "
                                        "              {"
                                        "                \"secondaryPercent\" : \"200\"     "
                                        "              }],                                  "
                                        "  \"style\":  [{ "
                                        "                \"color\" : \"#12345678\"          "
                                        "              }]"
                                        "}";
    /**
     * @tc.steps: step2. Create progress node with the string.
     * @tc.expected: step2. Progress node and child progress component are created successfully.
     */
    auto progress = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(progressTest002);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(progress);
    RefPtr<ProgressComponent> progressComponent = AceType::DynamicCast<ProgressComponent>(boxChild->GetChild());
    ASSERT_TRUE(progressComponent != nullptr);

    /**
     * @tc.steps: step3. Check the percent of created progress node.
     * @tc.expected: step3. The percent = 100.
     */
    double percent = progressComponent->GetValue();
    double secondaryPercent = progressComponent->GetCachedValue();
    EXPECT_TRUE(NearEqual(percent, PERCENT_MAX));
    EXPECT_TRUE(NearEqual(secondaryPercent, PERCENT_MAX));
}

/**
 * @tc.name: DomProgressCreatorTest003
 * @tc.desc: Test create progress node successfully and stroke-width create as desire.
 * @tc.type: FUNC
 */
HWTEST_F(DomProgressTest, DomProgressCreatorTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string of DomProgress with two attributes and a style:
     * type, percent, strokeWidth.
     */
    const std::string progressTest003 = ""
                                        "{                                                  "
                                        "  \"tag\": \"progress\",                           "
                                        "  \"attr\": [{                                     "
                                        "                \"type\" : \"horizontal\"          "
                                        "              },                                   "
                                        "              {"
                                        "                \"percent\" : \"20\"               "
                                        "              }],                                  "
                                        "  \"style\":  [{ "
                                        "                \"strokeWidth\" : \"32\"           "
                                        "              }]"
                                        "}";
    /**
     * @tc.steps: step2. Create progress node with the string.
     * @tc.expected: step2. Progress node and child progress component are created successfully.
     */
    auto progress = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(progressTest003);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(progress);
    RefPtr<ProgressComponent> progressComponent = AceType::DynamicCast<ProgressComponent>(boxChild->GetChild());
    ASSERT_TRUE(progressComponent != nullptr);

    /**
     * @tc.steps: step3. Check the stroke-width of created progress node.
     * @tc.expected: step3. The stroke-width is as expected.
     */
    auto progressTrack = progressComponent->GetTrack();
    ASSERT_TRUE(progressTrack != nullptr);
    Dimension defaultThickness = Dimension(32.0, DimensionUnit::VP);
    Dimension thickness = progressTrack->GetTrackThickness();
    EXPECT_TRUE(NearEqual(thickness.Value(), defaultThickness.Value()));
}

/**
 * @tc.name: DomProgressCreatorTest004
 * @tc.desc: Set percent, secondaryPercent = 0 when percent < 0, secondaryPercent < 0.
 * @tc.type: FUNC
 */
HWTEST_F(DomProgressTest, DomProgressCreatorTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string of DomProgress with percent < 0 and secondarypercent < 0:
     */
    const std::string progressTest004 = ""
                                        "{                                                  "
                                        "  \"tag\": \"progress\",                           "
                                        "  \"attr\": [{                                     "
                                        "                \"type\" : \"horizontal\"          "
                                        "              },                                   "
                                        "              {"
                                        "                \"percent\" : \"-10\"              "
                                        "              },                                   "
                                        "              {"
                                        "                \"secondaryPercent\" : \"-10\"     "
                                        "              }],                                  "
                                        "  \"style\":  [{ "
                                        "                \"color\" : \"#12345678\"          "
                                        "              }]"
                                        "}";
    /**
     * @tc.steps: step2. Create progress node with the string.
     * @tc.expected: step2. Progress node and child progress component are created successfully.
     */
    auto progress = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(progressTest004);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(progress);
    RefPtr<ProgressComponent> progressComponent = AceType::DynamicCast<ProgressComponent>(boxChild->GetChild());
    ASSERT_TRUE(progressComponent != nullptr);

    /**
     * @tc.steps: step3. Check the percent of created progress node.
     * @tc.expected: step3. The percent = 0 and secondarypercent = 0.
     */
    double percent = progressComponent->GetValue();
    double secondaryPercent = progressComponent->GetCachedValue();
    EXPECT_TRUE(NearEqual(percent, PERCENT_MIN));
    EXPECT_TRUE(NearEqual(secondaryPercent, PERCENT_MIN));
}

/**
 * @tc.name: DomProgressCreatorTest005
 * @tc.desc: Set style color and effect to the ring and comet.
 * @tc.type: FUNC
 */
HWTEST_F(DomProgressTest, DomProgressCreatorTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string of DomProgress with type circular:
     */
    const std::string progressTest005 = ""
                                        "{                                                  "
                                        "  \"tag\": \"progress\",                           "
                                        "  \"attr\": [{                                     "
                                        "                \"type\" : \"circular\"            "
                                        "              }],                                  "
                                        "  \"style\":  [{                                   "
                                        "                \"color\" : \"#11111111\"          "
                                        "              }]"
                                        "}";
    /**
     * @tc.steps: step2. Create progress node with the string.
     * @tc.expected: step2. Progress node and child progress component are created successfully.
     */
    auto progress = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(progressTest005);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(progress);
    auto loading005 = AceType::DynamicCast<LoadingProgressComponent>(boxChild->GetChild());
    ASSERT_TRUE(loading005 != nullptr);

    /**
     * @tc.steps: step3. Check the percent of created progress node.
     * @tc.expected: step3. The ring color and comet color are correct.
     */
    Color progressColor = loading005->GetProgressColor();
    EXPECT_TRUE(progressColor == Color(0x11111111));
}

/**
 * @tc.name: DomProgressCreatorTest006
 * @tc.desc: Set style color & cometColor and only effect color.
 * @tc.type: FUNC
 */
HWTEST_F(DomProgressTest, DomProgressCreatorTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string of DomProgress with type circular:
     */
    const std::string progressTest006 = ""
                                        "{                                                  "
                                        "  \"tag\": \"progress\",                           "
                                        "  \"attr\": [{                                     "
                                        "                \"type\" : \"circular\"            "
                                        "              }],                                  "
                                        "  \"style\":  [{                                   "
                                        "                \"color\" : \"#22222222\"          "
                                        "              }]"
                                        "}";
    /**
     * @tc.steps: step2. Create progress node with the string.
     * @tc.expected: step2. Progress node and child progress component are created successfully.
     */
    auto progress = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(progressTest006);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(progress);
    auto loading006 = AceType::DynamicCast<LoadingProgressComponent>(boxChild->GetChild());
    ASSERT_TRUE(loading006 != nullptr);

    /**
     * @tc.steps: step3. Check the percent of created progress node.
     * @tc.expected: step3. The ring color and comet color are correct.
     */
    Color progressColor = loading006->GetProgressColor();
    EXPECT_TRUE(progressColor == Color(0x22222222));
}

/**
 * @tc.name: DomProgressCreatorTest007
 * @tc.desc: Set style ringColor & cometColor and only effect to ring and comet.
 * @tc.type: FUNC
 */
HWTEST_F(DomProgressTest, DomProgressCreatorTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string of DomProgress with type circular:
     */
    const std::string progressTest007 = ""
                                        "{                                                  "
                                        "  \"tag\": \"progress\",                           "
                                        "  \"attr\": [{                                     "
                                        "                \"type\" : \"circular\"            "
                                        "              }],                                  "
                                        "  \"style\":  [{                                   "
                                        "                \"color\" : \"#88888888\"          "
                                        "              }]"
                                        "}";
    /**
     * @tc.steps: step2. Create progress node with the string.
     * @tc.expected: step2. Progress node and child progress component are created successfully.
     */
    auto progress = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(progressTest007);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(progress);
    auto loading007 = AceType::DynamicCast<LoadingProgressComponent>(boxChild->GetChild());
    ASSERT_TRUE(loading007 != nullptr);

    /**
     * @tc.steps: step3. Check the percent of created progress node.
     * @tc.expected: step3. The ring color and comet color are correct.
     */
    Color progressColor = loading007->GetProgressColor();
    EXPECT_TRUE(progressColor == Color(0x88888888));
}

/**
 * @tc.name: DomProgressCreatorTest008
 * @tc.desc: Set none color style and effect the default color.
 * @tc.type: FUNC
 */
HWTEST_F(DomProgressTest, DomProgressCreatorTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string of DomProgress with type circular:
     */
    const std::string progressTest008 = ""
                                        "{                                                  "
                                        "  \"tag\": \"progress\",                           "
                                        "  \"attr\": [{                                     "
                                        "                \"type\" : \"circular\"            "
                                        "              }]"
                                        "}";
    /**
     * @tc.steps: step2. Create progress node with the string.
     * @tc.expected: step2. Progress node and child progress component are created successfully.
     */
    auto progress = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(progressTest008);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(progress);
    auto loading008 = AceType::DynamicCast<LoadingProgressComponent>(boxChild->GetChild());
    ASSERT_TRUE(loading008 != nullptr);

    /**
     * @tc.steps: step3. Check the percent of created progress node.
     * @tc.expected: step3. The ring color and comet color are correct.
     */
    Color progressColor = loading008->GetProgressColor();
    EXPECT_TRUE(progressColor == Color(0xff666666) || progressColor == Color(0x99ffffff));
}

} // namespace OHOS::Ace::Framework
