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

#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"
#include "frameworks/bridge/common/dom/dom_slider.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

constexpr double VALUE_DEFAULT = 0.0;
constexpr double MIN_DEFAULT = 0.0;
constexpr double MAX_DEFAULT = 100.0;
constexpr double STEP_DEFAULT = 1.0;
constexpr double PADDING_DEFAULT = 32;
constexpr double TEST_PADDING_LEFT = 20.0;
constexpr double TEST_PADDING_RIGHT = 20.0;
constexpr double TEST_MIN_VALUE = 1.0;
constexpr double TEST_MAX_VALUE = 50.0;
constexpr double TEST_STEP_VALUE = 2.0;
constexpr double TEST_VALUE_VALUE = 10.0;
const std::string TEST_COLOR_VALUE = "#12345678";
constexpr double TEST_MIN_VALUE_004 = -1.0;
constexpr double TEST_MAX_VALUE_004 = 50.0;
constexpr double TEST_STEP_VALUE_004 = 2.0;
constexpr double TEST_VALUE_VALUE_004 = 10.0;
constexpr double TEST_MIN_VALUE_005 = 50.0;
constexpr double TEST_MAX_VALUE_005 = 60.0;
constexpr double TEST_STEP_VALUE_005 = 2.0;
constexpr double TEST_VALUE_VALUE_005 = 10.0;
constexpr double TEST_MIN_VALUE_006 = 10.0;
constexpr double TEST_MAX_VALUE_006 = 50.0;
constexpr double TEST_STEP_VALUE_006 = 2.0;
constexpr double TEST_VALUE_VALUE_006 = 1.0;
constexpr double TEST_MIN_VALUE_007 = 10.0;
constexpr double TEST_MAX_VALUE_007 = 50.0;
constexpr double TEST_STEP_VALUE_007 = 2.0;
constexpr double TEST_VALUE_VALUE_007 = 60.0;
constexpr double TEST_MIN_VALUE_008 = 10.0;
constexpr double TEST_MAX_VALUE_008 = 50.0;
constexpr double TEST_STEP_VALUE_008 = 1.0;
constexpr double TEST_VALUE_VALUE_008 = 20.0;
constexpr double TEST_MIN_VALUE_009 = 10.0;
constexpr double TEST_MAX_VALUE_009 = 50.0;
constexpr double TEST_STEP_VALUE_009 = 40.0;
constexpr double TEST_VALUE_VALUE_009 = 20.0;
const RefPtr<ThemeManager> THEME_MANAGER = AceType::MakeRefPtr<ThemeManager>();

} // namespace

class DomSliderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DomSliderTest::SetUpTestCase() {}
void DomSliderTest::TearDownTestCase() {}
void DomSliderTest::SetUp() {}
void DomSliderTest::TearDown() {}

/**
 * @tc.name: DomSliderCreator001
 * @tc.desc: Test slider node and child slider component are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomSliderTest, DomSliderCreatorTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create slider node with it.
     * @tc.expected: step1. Slider node and child slider component are created successfully.
     */
    const std::string sliderTestStr = "{                                                 "
                                      "  \"tag\": \"slider\",                            "
                                      "  \"attr\": [{                                    "
                                      "               \"min\" : \"1.0\"                  "
                                      "             },                                   "
                                      "             {"
                                      "               \"max\" : \"50.0\"                 "
                                      "             },                                   "
                                      "             {"
                                      "               \"value\" : \"10.0\"               "
                                      "             },                                   "
                                      "             {"
                                      "               \"step\" : \"2.0\"                 "
                                      "             }],                                  "
                                      "  \"style\": [{"
                                      "               \"color\" : \"#12345678\"          "
                                      "             },                                   "
                                      "             {"
                                      "               \"selectedColor\" : \"#12345678\"  "
                                      "             },                                   "
                                      "             {"
                                      "               \"paddingLeft\" : \"20\"           "
                                      "             },                                   "
                                      "             {"
                                      "               \"paddingRight\" : \"20\"          "
                                      "             }]                                   "
                                      "}";
    auto slider = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(sliderTestStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(slider);
    ASSERT_TRUE(boxChild);
    auto padding = AceType::DynamicCast<PaddingComponent>(boxChild->GetChild());
    ASSERT_TRUE(padding);
    RefPtr<SliderComponent> sliderComponent = AceType::DynamicCast<SliderComponent>(padding->GetChild());
    ASSERT_TRUE(sliderComponent);

    /**
     * @tc.steps: step2. check all property matched
     * @tc.expected: step2. all property match
     */
    auto track = sliderComponent->GetTrack();
    auto block = sliderComponent->GetBlock();
    ASSERT_TRUE(track);
    ASSERT_TRUE(block);
    EXPECT_TRUE(NearEqual(padding->GetPadding().Left().Value(), TEST_PADDING_LEFT));
    EXPECT_TRUE(NearEqual(padding->GetPadding().Right().Value(), TEST_PADDING_RIGHT));
    EXPECT_TRUE(NearEqual(sliderComponent->GetMinValue(), TEST_MIN_VALUE));
    EXPECT_TRUE(NearEqual(sliderComponent->GetMaxValue(), TEST_MAX_VALUE));
    EXPECT_TRUE(NearEqual(sliderComponent->GetStep(), TEST_STEP_VALUE));
    EXPECT_TRUE(NearEqual(sliderComponent->GetValue(), TEST_VALUE_VALUE));
    EXPECT_EQ(track->GetBackgroundColor(), Color::FromString(TEST_COLOR_VALUE));
    EXPECT_EQ(track->GetSelectColor(), Color::FromString(TEST_COLOR_VALUE));
}

/**
 * @tc.name: DomSliderCreator002
 * @tc.desc: Test slider node and child slider component are created defaultly.
 * @tc.type: FUNC
 */
HWTEST_F(DomSliderTest, DomSliderCreatorTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create slider node with it.
     * @tc.expected: step1. Slider node and child slider component are created successfully.
     */
    const std::string sliderTestStr = "{                      "
                                      "  \"tag\": \"slider\"  "
                                      "}";
    auto slider = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(sliderTestStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(slider);
    ASSERT_TRUE(boxChild);
    auto padding = AceType::DynamicCast<PaddingComponent>(boxChild->GetChild());
    ASSERT_TRUE(padding);
    RefPtr<SliderComponent> sliderComponent = AceType::DynamicCast<SliderComponent>(padding->GetChild());
    ASSERT_TRUE(sliderComponent);

    /**
     * @tc.steps: step2. check all property matched
     * @tc.expected: step2. all property match
     */
    auto track = sliderComponent->GetTrack();
    auto block = sliderComponent->GetBlock();
    ASSERT_TRUE(track);
    ASSERT_TRUE(block);
    EXPECT_TRUE(NearEqual(padding->GetPadding().Left().Value(), PADDING_DEFAULT));
    EXPECT_TRUE(NearEqual(padding->GetPadding().Right().Value(), PADDING_DEFAULT));
    EXPECT_TRUE(NearEqual(sliderComponent->GetMinValue(), MIN_DEFAULT));
    EXPECT_TRUE(NearEqual(sliderComponent->GetMaxValue(), MAX_DEFAULT));
    EXPECT_TRUE(NearEqual(sliderComponent->GetStep(), STEP_DEFAULT));
    EXPECT_TRUE(NearEqual(sliderComponent->GetValue(), VALUE_DEFAULT));
    auto themeConstants = THEME_MANAGER->GetThemeConstants();
    ASSERT_NE(themeConstants, nullptr);
    EXPECT_EQ(track->GetBackgroundColor(), themeConstants->GetColor(THEME_SLIDER_TRACK_BG));
    EXPECT_EQ(track->GetSelectColor(), themeConstants->GetColor(THEME_SLIDER_TRACK_SELECTED));
}

/**
 * @tc.name: DomSliderCreator003
 * @tc.desc: Test add event to slider component successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomSliderTest, DomSliderCreatorTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create slider node with it.
     * @tc.expected: step1. Slider node and child slider component are created successfully.
     */
    const std::string sliderTestStr = "{                                    "
                                      "  \"tag\": \"slider\",               "
                                      "  \"event\": [ \"change\" ]          "
                                      "}";
    auto slider = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(sliderTestStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(slider);
    ASSERT_TRUE(boxChild);
    auto padding = AceType::DynamicCast<PaddingComponent>(boxChild->GetChild());
    ASSERT_TRUE(padding);
    RefPtr<SliderComponent> sliderComponent = AceType::DynamicCast<SliderComponent>(padding->GetChild());
    ASSERT_TRUE(sliderComponent);

    /**
     * @tc.steps: step2. Check eventId of created slider component.
     * @tc.expected: step2. The eventId value of slider component is as expected.
     */
    EXPECT_TRUE(sliderComponent->GetOnMovingEventId() == std::to_string(slider->GetNodeId()));
}

/**
 * @tc.name: DomSliderCreator004
 * @tc.desc: Test slider node and child slider component are created successfully when min < 0.
 * @tc.type: FUNC
 */
HWTEST_F(DomSliderTest, DomSliderCreatorTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create slider node with it.
     * @tc.expected: step1. Slider node and child slider component are created successfully.
     */
    const std::string sliderTestStr = "{                                      "
                                      "  \"tag\": \"slider\",                 "
                                      "  \"attr\": [{"
                                      "               \"min\" : \"-1\"        "
                                      "             },                        "
                                      "             {"
                                      "               \"max\" : \"50.0\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"value\" : \"10.0\"    "
                                      "             },                        "
                                      "             {"
                                      "               \"step\" : \"2.0\"      "
                                      "             }]                        "
                                      "}";
    auto slider = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(sliderTestStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(slider);
    ASSERT_TRUE(boxChild);
    auto padding = AceType::DynamicCast<PaddingComponent>(boxChild->GetChild());
    ASSERT_TRUE(padding);
    RefPtr<SliderComponent> sliderComponent = AceType::DynamicCast<SliderComponent>(padding->GetChild());
    ASSERT_TRUE(sliderComponent);

    /**
     * @tc.steps: step2. check all property matched
     * @tc.expected: step2. all property match
     */
    auto track = sliderComponent->GetTrack();
    auto block = sliderComponent->GetBlock();
    ASSERT_TRUE(track);
    ASSERT_TRUE(block);
    EXPECT_TRUE(NearEqual(sliderComponent->GetMinValue(), TEST_MIN_VALUE_004));
    EXPECT_TRUE(NearEqual(sliderComponent->GetMaxValue(), TEST_MAX_VALUE_004));
    EXPECT_TRUE(NearEqual(sliderComponent->GetStep(), TEST_STEP_VALUE_004));
    EXPECT_TRUE(NearEqual(sliderComponent->GetValue(), TEST_VALUE_VALUE_004));
}

/**
 * @tc.name: DomSliderCreator005
 * @tc.desc: Test slider node and child slider component are created successfully when min > max.
 * @tc.type: FUNC
 */
HWTEST_F(DomSliderTest, DomSliderCreatorTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create slider node with it.
     * @tc.expected: step1. Slider node and child slider component are created successfully.
     */
    const std::string sliderTestStr = "{                                      "
                                      "  \"tag\": \"slider\",                 "
                                      "  \"attr\": [{"
                                      "               \"min\" : \"60.0\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"max\" : \"50.0\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"value\" : \"10.0\"    "
                                      "             },                        "
                                      "             {"
                                      "               \"step\" : \"2.0\"      "
                                      "             }]                        "
                                      "}";
    auto slider = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(sliderTestStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(slider);
    ASSERT_TRUE(boxChild);
    auto padding = AceType::DynamicCast<PaddingComponent>(boxChild->GetChild());
    ASSERT_TRUE(padding);
    RefPtr<SliderComponent> sliderComponent = AceType::DynamicCast<SliderComponent>(padding->GetChild());
    ASSERT_TRUE(sliderComponent);

    /**
     * @tc.steps: step2. check all property matched
     * @tc.expected: step2. all property match
     */
    auto track = sliderComponent->GetTrack();
    auto block = sliderComponent->GetBlock();
    ASSERT_TRUE(track);
    ASSERT_TRUE(block);
    EXPECT_TRUE(NearEqual(sliderComponent->GetMinValue(), TEST_MIN_VALUE_005));
    EXPECT_TRUE(NearEqual(sliderComponent->GetMaxValue(), TEST_MAX_VALUE_005));
    EXPECT_TRUE(NearEqual(sliderComponent->GetStep(), TEST_STEP_VALUE_005));
    EXPECT_TRUE(NearEqual(sliderComponent->GetValue(), TEST_VALUE_VALUE_005));
}

/**
 * @tc.name: DomSliderCreator006
 * @tc.desc: Test slider node and child slider component are created successfully when min > value.
 * @tc.type: FUNC
 */
HWTEST_F(DomSliderTest, DomSliderCreatorTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create slider node with it.
     * @tc.expected: step1. Slider node and child slider component are created successfully.
     */
    const std::string sliderTestStr = "{                                      "
                                      "  \"tag\": \"slider\",                 "
                                      "  \"attr\": [{"
                                      "               \"min\" : \"10.0\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"max\" : \"50.0\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"value\" : \"1.0\"     "
                                      "             },                        "
                                      "             {"
                                      "               \"step\" : \"2.0\"      "
                                      "             }]                        "
                                      "}";
    auto slider = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(sliderTestStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(slider);
    ASSERT_TRUE(boxChild);
    auto padding = AceType::DynamicCast<PaddingComponent>(boxChild->GetChild());
    ASSERT_TRUE(padding);
    RefPtr<SliderComponent> sliderComponent = AceType::DynamicCast<SliderComponent>(padding->GetChild());
    ASSERT_TRUE(sliderComponent);

    /**
     * @tc.steps: step2. check all property matched
     * @tc.expected: step2. all property match
     */
    auto track = sliderComponent->GetTrack();
    auto block = sliderComponent->GetBlock();
    ASSERT_TRUE(track);
    ASSERT_TRUE(block);
    EXPECT_TRUE(NearEqual(sliderComponent->GetMinValue(), TEST_MIN_VALUE_006));
    EXPECT_TRUE(NearEqual(sliderComponent->GetMaxValue(), TEST_MAX_VALUE_006));
    EXPECT_TRUE(NearEqual(sliderComponent->GetStep(), TEST_STEP_VALUE_006));
    EXPECT_TRUE(NearEqual(sliderComponent->GetValue(), TEST_VALUE_VALUE_006));
}

/**
 * @tc.name: DomSliderCreator007
 * @tc.desc: Test slider node and child slider component are created successfully when max < value.
 * @tc.type: FUNC
 */
HWTEST_F(DomSliderTest, DomSliderCreatorTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create slider node with it.
     * @tc.expected: step1. Slider node and child slider component are created successfully.
     */
    const std::string sliderTestStr = "{                                      "
                                      "  \"tag\": \"slider\",                 "
                                      "  \"attr\": [{"
                                      "               \"min\" : \"10.0\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"max\" : \"50.0\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"value\" : \"60.0\"    "
                                      "             },                        "
                                      "             {"
                                      "               \"step\" : \"2.0\"      "
                                      "             }]                        "
                                      "}";
    auto slider = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(sliderTestStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(slider);
    ASSERT_TRUE(boxChild);
    auto padding = AceType::DynamicCast<PaddingComponent>(boxChild->GetChild());
    ASSERT_TRUE(padding);
    RefPtr<SliderComponent> sliderComponent = AceType::DynamicCast<SliderComponent>(padding->GetChild());
    ASSERT_TRUE(sliderComponent);

    /**
     * @tc.steps: step2. check all property matched
     * @tc.expected: step2. all property match
     */
    auto track = sliderComponent->GetTrack();
    auto block = sliderComponent->GetBlock();
    ASSERT_TRUE(track);
    ASSERT_TRUE(block);
    EXPECT_TRUE(NearEqual(sliderComponent->GetMinValue(), TEST_MIN_VALUE_007));
    EXPECT_TRUE(NearEqual(sliderComponent->GetMaxValue(), TEST_MAX_VALUE_007));
    EXPECT_TRUE(NearEqual(sliderComponent->GetStep(), TEST_STEP_VALUE_007));
    EXPECT_TRUE(NearEqual(sliderComponent->GetValue(), TEST_VALUE_VALUE_007));
}

/**
 * @tc.name: DomSliderCreator008
 * @tc.desc: Test slider node and child slider component are created successfully when step < 0.
 * @tc.type: FUNC
 */
HWTEST_F(DomSliderTest, DomSliderCreatorTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create slider node with it.
     * @tc.expected: step1. Slider node and child slider component are created successfully.
     */
    const std::string sliderTestStr = "{                                      "
                                      "  \"tag\": \"slider\",                 "
                                      "  \"attr\": [{"
                                      "               \"min\" : \"10.0\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"max\" : \"50.0\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"value\" : \"20.0\"    "
                                      "             },                        "
                                      "             {"
                                      "               \"step\" : \"-2.0\"     "
                                      "             }]                        "
                                      "}";
    auto slider = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(sliderTestStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(slider);
    ASSERT_TRUE(boxChild);
    auto padding = AceType::DynamicCast<PaddingComponent>(boxChild->GetChild());
    ASSERT_TRUE(padding);
    RefPtr<SliderComponent> sliderComponent = AceType::DynamicCast<SliderComponent>(padding->GetChild());
    ASSERT_TRUE(sliderComponent);

    /**
     * @tc.steps: step2. check all property matched
     * @tc.expected: step2. all property match
     */
    auto track = sliderComponent->GetTrack();
    auto block = sliderComponent->GetBlock();
    ASSERT_TRUE(track);
    ASSERT_TRUE(block);
    EXPECT_TRUE(NearEqual(sliderComponent->GetMinValue(), TEST_MIN_VALUE_008));
    EXPECT_TRUE(NearEqual(sliderComponent->GetMaxValue(), TEST_MAX_VALUE_008));
    EXPECT_TRUE(NearEqual(sliderComponent->GetStep(), TEST_STEP_VALUE_008));
    EXPECT_TRUE(NearEqual(sliderComponent->GetValue(), TEST_VALUE_VALUE_008));
}

/**
 * @tc.name: DomSliderCreator009
 * @tc.desc: Test slider node and child slider component are created successfully when step > max - min.
 * @tc.type: FUNC
 */
HWTEST_F(DomSliderTest, DomSliderCreatorTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create slider node with it.
     * @tc.expected: step1. Slider node and child slider component are created successfully.
     */
    const std::string sliderTestStr = "{                                      "
                                      "  \"tag\": \"slider\",                 "
                                      "  \"attr\": [{"
                                      "               \"min\" : \"10.0\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"max\" : \"50.0\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"value\" : \"20.0\"    "
                                      "             },                        "
                                      "             {"
                                      "               \"step\" : \"50.0\"     "
                                      "             }]                        "
                                      "}";
    auto slider = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(sliderTestStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(slider);
    ASSERT_TRUE(boxChild);
    auto padding = AceType::DynamicCast<PaddingComponent>(boxChild->GetChild());
    ASSERT_TRUE(padding);
    RefPtr<SliderComponent> sliderComponent = AceType::DynamicCast<SliderComponent>(padding->GetChild());
    ASSERT_TRUE(sliderComponent);

    /**
     * @tc.steps: step2. check all property matched
     * @tc.expected: step2. all property match
     */
    auto track = sliderComponent->GetTrack();
    auto block = sliderComponent->GetBlock();
    ASSERT_TRUE(track);
    ASSERT_TRUE(block);
    EXPECT_TRUE(NearEqual(sliderComponent->GetMinValue(), TEST_MIN_VALUE_009));
    EXPECT_TRUE(NearEqual(sliderComponent->GetMaxValue(), TEST_MAX_VALUE_009));
    EXPECT_TRUE(NearEqual(sliderComponent->GetStep(), TEST_STEP_VALUE_009));
    EXPECT_TRUE(NearEqual(sliderComponent->GetValue(), TEST_VALUE_VALUE_009));
}

/**
 * @tc.name: DomSliderCreator010
 * @tc.desc: Test slider node and child slider component are created successfully when step > max - min.
 * @tc.type: FUNC
 */
HWTEST_F(DomSliderTest, DomSliderCreatorTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create slider node with it.
     * @tc.expected: step1. Slider node and child slider component are created successfully.
     */
    const std::string sliderTestStr = "{                                      "
                                      "  \"tag\": \"slider\",                 "
                                      "  \"attr\": [{"
                                      "               \"min\" : \"10.0\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"max\" : \"50.0\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"value\" : \"20.0\"    "
                                      "             },                        "
                                      "             {"
                                      "               \"step\" : \"50.0\"     "
                                      "             }]                        "
                                      "}";
    auto slider = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(sliderTestStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(slider);
    ASSERT_TRUE(boxChild);
    auto padding = AceType::DynamicCast<PaddingComponent>(boxChild->GetChild());
    ASSERT_TRUE(padding);
    RefPtr<SliderComponent> sliderComponent = AceType::DynamicCast<SliderComponent>(padding->GetChild());
    ASSERT_TRUE(sliderComponent);

    /**
     * @tc.steps: step2. check all property matched
     * @tc.expected: step2. all property match
     */
    auto track = sliderComponent->GetTrack();
    auto block = sliderComponent->GetBlock();
    ASSERT_TRUE(track);
    ASSERT_TRUE(block);
    EXPECT_FALSE(sliderComponent->NeedShowSteps());
    EXPECT_FALSE(sliderComponent->NeedShowTips());
    EXPECT_FALSE(sliderComponent->GetSliderMode() == SliderMode::INSET);
}

/**
 * @tc.name: DomSliderCreator011
 * @tc.desc: Test slider node and child slider component are created successfully when step > max - min.
 * @tc.type: FUNC
 */
HWTEST_F(DomSliderTest, DomSliderCreatorTest011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create slider node with it.
     * @tc.expected: step1. Slider node and child slider component are created successfully.
     */
    const std::string sliderTestStr = "{                                      "
                                      "  \"tag\": \"slider\",                 "
                                      "  \"attr\": [{"
                                      "               \"mode\" : \"inset\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"step\" : \"20\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"showtips\" : \"true\"    "
                                      "             },                        "
                                      "             {"
                                      "               \"showsteps\" : \"true\"     "
                                      "             }]                        "
                                      "}";
    auto slider = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(sliderTestStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(slider);
    ASSERT_TRUE(boxChild);
    auto padding = AceType::DynamicCast<PaddingComponent>(boxChild->GetChild());
    ASSERT_TRUE(padding);
    RefPtr<SliderComponent> sliderComponent = AceType::DynamicCast<SliderComponent>(padding->GetChild());
    ASSERT_TRUE(sliderComponent);

    /**
     * @tc.steps: step2. check all property matched
     * @tc.expected: step2. all property match
     */
    auto track = sliderComponent->GetTrack();
    auto block = sliderComponent->GetBlock();
    ASSERT_TRUE(track);
    ASSERT_TRUE(block);
    EXPECT_TRUE(sliderComponent->NeedShowSteps());
    EXPECT_TRUE(sliderComponent->NeedShowTips());
    EXPECT_TRUE(sliderComponent->GetSliderMode() == SliderMode::INSET);
}

/**
 * @tc.name: DomSliderCreator012
 * @tc.desc: Test slider node and child slider component are created successfully when step > max - min.
 * @tc.type: FUNC
 */
HWTEST_F(DomSliderTest, DomSliderCreatorTest012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create slider node with it.
     * @tc.expected: step1. Slider node and child slider component are created successfully.
     */
    const std::string sliderTestStr = "{                                      "
                                      "  \"tag\": \"slider\",                 "
                                      "  \"attr\": [{"
                                      "               \"mode\" : \"inset\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"step\" : \"120.0\"      "
                                      "             },                        "
                                      "             {"
                                      "               \"max\" : \"0\"    "
                                      "             },                        "
                                      "             {"
                                      "               \"min\" : \"100\"     "
                                      "             }]                        "
                                      "}";
    auto slider = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(sliderTestStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(slider);
    ASSERT_TRUE(boxChild);
    auto padding = AceType::DynamicCast<PaddingComponent>(boxChild->GetChild());
    ASSERT_TRUE(padding);
    RefPtr<SliderComponent> sliderComponent = AceType::DynamicCast<SliderComponent>(padding->GetChild());
    ASSERT_TRUE(sliderComponent);

    /**
     * @tc.steps: step2. check all property matched
     * @tc.expected: step2. all property match
     */
    auto track = sliderComponent->GetTrack();
    auto block = sliderComponent->GetBlock();
    ASSERT_TRUE(track);
    ASSERT_TRUE(block);
    EXPECT_EQ(sliderComponent->GetStep(), 100.0);
    EXPECT_EQ(sliderComponent->GetMaxValue(), 100.0);
    EXPECT_EQ(sliderComponent->GetMinValue(), 0.0);
}

} // namespace OHOS::Ace::Framework
