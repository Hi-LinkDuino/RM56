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

#include <string>

#include "gtest/gtest.h"

#include "adapter/aosp/entrance/java/jni/ace_application_info.h"
#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/common/properties/color.h"
#include "core/components/slider/block_component.h"
#include "core/components/slider/slider_theme.h"
#include "core/components/test/json/slider_creator.h"
#include "core/components/theme/theme_manager.h"
#include "core/components/track/track_component.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/constants.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr double JSON_VALUE = 0.0;
constexpr double JSON_MIN_VALUE = 0.0;
constexpr double JSON_MAX_VALUE = 10.0;
constexpr double JSON_STEP = 1.0;
// This color value is easy to compare
const Color BAR_SELECTED_COLOR = Color(123456789);
const Color BAR_BACKGROUND_COLOR = Color(987654321);
const Color BLOCK_COLOR = Color(567891234);
const std::string JSON_ON_MOVE_ID = "123456";

} // namespace

Platform::JniEnvironment::JniEnvironment() {}

Platform::JniEnvironment::~JniEnvironment() = default;

std::shared_ptr<JNIEnv> Platform::JniEnvironment::GetJniEnv(JNIEnv* jniEnv) const
{
    return nullptr;
}

Platform::JniEnvironment& Platform::JniEnvironment::GetInstance()
{
    static Platform::JniEnvironment jniEnvironment;
    return jniEnvironment;
}

class SliderCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    RefPtr<SliderComponent> CreateComponent(const std::string& testJson);
    void CompareAllProperty(const RefPtr<SliderComponent>& slider);
    void CompareAllDefault(const RefPtr<SliderComponent>& slider);
    void CompareAllEssential(const RefPtr<SliderComponent>& slider);
};

void SliderCreatorTest::SetUpTestCase() {}

void SliderCreatorTest::TearDownTestCase() {}

void SliderCreatorTest::SetUp() {}

void SliderCreatorTest::TearDown() {}

RefPtr<SliderComponent> SliderCreatorTest::CreateComponent(const std::string& testJson)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(testJson.c_str());
    size_t dslLen = testJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    if (!component) {
        return nullptr;
    }
    return AceType::DynamicCast<SliderComponent>(component);
}

void SliderCreatorTest::CompareAllProperty(const RefPtr<SliderComponent>& slider)
{
    CompareAllEssential(slider);

    RefPtr<TrackComponent> linearBar = slider->GetTrack();
    Color barBackgroundColor = linearBar->GetBackgroundColor();
    EXPECT_TRUE(barBackgroundColor == BAR_BACKGROUND_COLOR);
    Color selectedColor = linearBar->GetSelectColor();
    EXPECT_TRUE(selectedColor == BAR_SELECTED_COLOR);

    RefPtr<BlockComponent> block = slider->GetBlock();
    Color blockColor = block->GetBlockColor();
    EXPECT_TRUE(blockColor == BLOCK_COLOR);
    const auto& onMoveEndId = slider->GetOnMoveEndEventId();
    EXPECT_TRUE(onMoveEndId == JSON_ON_MOVE_ID);
}

void SliderCreatorTest::CompareAllDefault(const RefPtr<SliderComponent>& slider)
{
    CompareAllEssential(slider);
    RefPtr<TrackComponent> linearBar = slider->GetTrack();
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    auto theme = themeManager->GetTheme<SliderTheme>();
    ASSERT_TRUE(theme);
    Color barBackgroundColor = linearBar->GetBackgroundColor();
    Color selectedColor = linearBar->GetSelectColor();
    EXPECT_TRUE(selectedColor == theme->GetTrackSelectedColor());
    EXPECT_TRUE(barBackgroundColor == theme->GetTrackBgColor());
    RefPtr<BlockComponent> block = slider->GetBlock();
    Color blockColor = block->GetBlockColor();
    EXPECT_TRUE(blockColor == theme->GetBlockColor());
    const auto& onMoveEndId = slider->GetOnMoveEndEventId();
    EXPECT_TRUE(onMoveEndId == "");
}

void SliderCreatorTest::CompareAllEssential(const RefPtr<SliderComponent>& slider)
{
    double value = slider->GetValue();
    EXPECT_TRUE(NearEqual(value, JSON_VALUE));
    double minValue = slider->GetMinValue();
    EXPECT_TRUE(NearEqual(minValue, JSON_MIN_VALUE));
    double maxValue = slider->GetMaxValue();
    EXPECT_TRUE(NearEqual(maxValue, JSON_MAX_VALUE));
    double step = slider->GetStep();
    EXPECT_TRUE(NearEqual(step, JSON_STEP));
}

/**
 * @tc.name: SliderComponentCreator001
 * @tc.desc: Test all the property created properly
 * @tc.type: FUNC
 * @tc.require: AR000DAQV7 AR000DAQV8 AR000DAQV9
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with all property.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"value\": 0.0,                 "
                           "  \"min\": 0.0,                   "
                           "  \"max\": 10.0,                  "
                           "  \"step\": 1.0,                  "
                           "  \"onMoveEndId\": \"123456\",    "
                           "  \"barSelectedColor\": {         "
                           "  \"className\": \"Color\",       "
                           "  \"value\":  123456789           "
                           "                         },       "
                           "  \"barBackgroundColor\": {       "
                           "  \"className\": \"Color\",       "
                           "  \"value\":  987654321           "
                           "                         },       "
                           "  \"blockColor\": {               "
                           "  \"className\": \"Color\",       "
                           "  \"value\":  567891234           "
                           "                         }        "
                           "}";
    /**
     * @tc.steps: step2. check all property matched
     * @tc.expected: step2. all property match
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    ASSERT_TRUE(slider != nullptr);
    CompareAllProperty(slider);
}

/**
 * @tc.name: SliderComponentCreator002
 * @tc.desc: Test the SliderComponent initialized by default value
 * @tc.type: FUNC
 * @tc.require: AR000DAQV7 AR000DAQV8 AR000DAQV9
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with value only.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"value\": 0.0,                 "
                           "  \"min\": 0.0,                   "
                           "  \"max\": 10.0,                  "
                           "  \"step\": 1                     "
                           "}";
    /**
     * @tc.steps: step2. check all property matched
     * @tc.expected: step2. all property match
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    ASSERT_TRUE(slider != nullptr);
    CompareAllDefault(slider);
}

/**
 * @tc.name: SliderComponentCreator003
 * @tc.desc: Test value validation. If the value is invalid, set a default value
 * @tc.type: FUNC
 * @tc.require: AR000DAQVA
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with value, minValue and maxValue. And choose min > value
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"value\": 0.0,                 "
                           "  \"min\": 1.0,                   "
                           "  \"max\": 3.0,                   "
                           "  \"step\": 0.5                   "
                           "}";
    /**
     * @tc.steps: step2. check component is null or not
     * @tc.expected: step2. component is nullptr
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    EXPECT_TRUE(slider == nullptr);
    /**
     * @tc.steps: step3. create a new json string with value, minValue and maxValue. And choose max < value
     */
    testJson = "{                                 "
               "  \"className\": \"Slider\",      "
               "  \"value\": 4.0,                 "
               "  \"min\": 1.0,                   "
               "  \"max\": 3.0,                   "
               "  \"step\": 0.5                   "
               "}";
    /**
     * @tc.steps: step4. check component is null or not
     * @tc.expected: step4. component is nullptr
     */
    slider = CreateComponent(testJson);
    EXPECT_TRUE(slider == nullptr);
}

/**
 * @tc.name: SliderComponentCreator004
 * @tc.desc: Test max and min validation.
 * @tc.type: FUNC
 * @tc.require: AR000DAQVA
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with value, minValue and maxValue. And choose min >= max
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"value\": 0.0,                 "
                           "  \"min\": 0.0,                   "
                           "  \"max\": 0.0,                   "
                           "  \"step\": 0.0                   "
                           "}";
    /**
     * @tc.steps: step2. check component is null or not
     * @tc.expected: step2. component is nullptr
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    EXPECT_TRUE(slider == nullptr);
}

/**
 * @tc.name: SliderComponentCreator005
 * @tc.desc: Test the step validation or not
 * @tc.type: FUNC
 * @tc.require: AR000DAQVA
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with value, minValue, maxValue and step. And choose step > max - min
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"value\": 0.0,                 "
                           "  \"min\": 0.0,                   "
                           "  \"max\": 1.0,                   "
                           "  \"step\": 3.0                   "
                           "}";
    /**
     * @tc.steps: step2. check component nullptr or not
     * @tc.expected: step2. component is nullptr
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    EXPECT_TRUE(slider == nullptr);
}

/**
 * @tc.name: SliderComponentCreator006
 * @tc.desc: check step: 0, component is created successfully
 * @tc.type: FUNC
 * @tc.require: AR000DAQVA
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with value, minValue, maxValue and step. And choose step > max - min
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"value\": 0.0,                 "
                           "  \"min\": 0.0,                   "
                           "  \"max\": 10.0,                  "
                           "  \"step\": 0.0                   "
                           "}";
    /**
     * @tc.steps: step2. check component nullptr or not
     * @tc.expected: step2. component is not nullptr
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    EXPECT_TRUE(slider != nullptr);
    double value = slider->GetValue();
    EXPECT_TRUE(NearEqual(value, JSON_VALUE));
    double minValue = slider->GetMinValue();
    EXPECT_TRUE(NearEqual(minValue, JSON_MIN_VALUE));
    double maxValue = slider->GetMaxValue();
    EXPECT_TRUE(NearEqual(maxValue, JSON_MAX_VALUE));
    double step = slider->GetStep();
    EXPECT_TRUE(NearEqual(step, 0.0));
}

/**
 * @tc.name: SliderComponentCreator007
 * @tc.desc: check step: -1, component is created - failed
 * @tc.type: FUNC
 * @tc.require: AR000DAQVA
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with value, minValue, maxValue and step. step: -1
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"value\": 0.0,                 "
                           "  \"min\": 0.0,                   "
                           "  \"max\": 10.0,                  "
                           "  \"step\": -1.0                  "
                           "}";
    /**
     * @tc.steps: step2. check component nullptr or not
     * @tc.expected: step2. component is nullptr
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    EXPECT_TRUE(slider == nullptr);
}

/**
 * @tc.name: SliderComponentCreator008
 * @tc.desc: missing essential param [value]
 * @tc.type: FUNC
 * @tc.require: AR000DAQVA
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with minValue, maxValue and step.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"min\": 0.0,                   "
                           "  \"max\": 10.0,                  "
                           "  \"step\": -1.0                  "
                           "}";
    /**
     * @tc.steps: step2. check component nullptr or not
     * @tc.expected: step2. component is nullptr
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    EXPECT_TRUE(slider == nullptr);
}

/**
 * @tc.name: SliderComponentCreator009
 * @tc.desc: missing essential param [max]
 * @tc.type: FUNC
 * @tc.require: AR000DAQVA
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with value, minValue, and step.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"min\": 0.0,                   "
                           "  \"value\": 0.0,                 "
                           "  \"step\": -1.0                  "
                           "}";
    /**
     * @tc.steps: step2. check component nullptr or not
     * @tc.expected: step2. component is nullptr
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    EXPECT_TRUE(slider == nullptr);
}

/**
 * @tc.name: SliderComponentCreator010
 * @tc.desc: missing essential param [step]
 * @tc.type: FUNC
 * @tc.require: AR000DAQVA
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with value, minValue, maxValue.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"min\": 0.0,                   "
                           "  \"value\": 0.0,                 "
                           "  \"max\": 10.0                   "
                           "}";
    /**
     * @tc.steps: step2. check component nullptr or not
     * @tc.expected: step2. component is nullptr
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    EXPECT_TRUE(slider == nullptr);
}

/**
 * @tc.name: SliderComponentCreator011
 * @tc.desc: missing essential param [min]
 * @tc.type: FUNC
 * @tc.require: AR000DAQVA
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with value, step, maxValue.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"step\": 0.0,                   "
                           "  \"value\": 0.0,                 "
                           "  \"max\": 10.0                   "
                           "}";
    /**
     * @tc.steps: step2. check component nullptr or not
     * @tc.expected: step2. component is nullptr
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    EXPECT_TRUE(slider == nullptr);
}

/**
 * @tc.name: SliderComponentCreator012
 * @tc.desc: type error essential param [value]
 * @tc.type: FUNC
 * @tc.require: AR000DAQVA
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with step, min, max. value type is string
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"step\": 0.0,                  "
                           "  \"min\": 0.0,                   "
                           "  \"max\": 10.0,                  "
                           "  \"value\": \"5.0\"              "
                           "}";
    /**
     * @tc.steps: step2. check component nullptr or not
     * @tc.expected: step2. component is nullptr
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    EXPECT_TRUE(slider == nullptr);
}

/**
 * @tc.name: SliderComponentCreator013
 * @tc.desc: type error essential param [max]
 * @tc.type: FUNC
 * @tc.require: AR000DAQVA
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with step, min, value. max type is string
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"step\": 0.0,                  "
                           "  \"min\": 0.0,                   "
                           "  \"value\": 3.0,                 "
                           "  \"max\": \"5.0\"                "
                           "}";
    /**
     * @tc.steps: step2. check component nullptr or not
     * @tc.expected: step2. component is nullptr
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    EXPECT_TRUE(slider == nullptr);
}

/**
 * @tc.name: SliderComponentCreator014
 * @tc.desc: type error essential param [min]
 * @tc.type: FUNC
 * @tc.require: AR000DAQVA
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with step, max, value. min type is string
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"step\": 0.0,                  "
                           "  \"max\": 10.0,                  "
                           "  \"value\": 3.0,                 "
                           "  \"min\": \"0.0\"                "
                           "}";
    /**
     * @tc.steps: step2. check component nullptr or not
     * @tc.expected: step2. component is nullptr
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    EXPECT_TRUE(slider == nullptr);
}

/**
 * @tc.name: SliderComponentCreator015
 * @tc.desc: type error essential param [step]
 * @tc.type: FUNC
 * @tc.require: AR000DAQVA
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with min, max, value. step type is string
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"min\": 0.0,                   "
                           "  \"max\": 10.0,                  "
                           "  \"value\": 3.0,                 "
                           "  \"step\": \"1.0\"               "
                           "}";
    /**
     * @tc.steps: step2. check component nullptr or not
     * @tc.expected: step2. component is nullptr
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    EXPECT_TRUE(slider == nullptr);
}

/**
 * @tc.name: SliderComponentCreator016
 * @tc.desc: type error: all non-essential param
 * @tc.type: FUNC
 * @tc.require: AR000DAQVA
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with min, max, value, step. Other param is type error
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"value\": 0.0,                 "
                           "  \"min\": 0.0,                   "
                           "  \"max\": 10.0,                  "
                           "  \"step\": 1.0,                  "
                           "  \"onMoveEndId\": 123456,        "
                           "  \"barSelectedColor\": 123456,   "
                           "  \"barBackgroundColor\": 654321, "
                           "  \"blockColor\": 456123          "
                           "}";
    /**
     * @tc.steps: step2. check component nullptr or not
     * @tc.expected: step2. component is nullptr
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    EXPECT_TRUE(slider != nullptr);
    CompareAllDefault(slider);
}

/**
 * @tc.name: SliderComponentCreator017
 * @tc.desc: when value, min, max are negative value, test creator performance
 * @tc.type: FUNC
 * @tc.require: AR000DAQVA
 * @tc.author: YaoYuchi
 */
HWTEST_F(SliderCreatorTest, SliderComponentCreator017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with min, max, value, step.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Slider\",      "
                           "  \"value\": -1.0,                "
                           "  \"min\": -10,                   "
                           "  \"max\": -1,                    "
                           "  \"step\": 1.0                   "
                           "}";
    /**
     * @tc.steps: step2. check component nullptr or not
     * @tc.expected: step2. component is not nullptr
     */
    RefPtr<SliderComponent> slider = CreateComponent(testJson);
    EXPECT_TRUE(slider != nullptr);
    double value = slider->GetValue();
    EXPECT_TRUE(NearEqual(value, -1.0));
    double minValue = slider->GetMinValue();
    EXPECT_TRUE(NearEqual(minValue, -10));
    double maxValue = slider->GetMaxValue();
    EXPECT_TRUE(NearEqual(maxValue, -1));
    double step = slider->GetStep();
    EXPECT_TRUE(NearEqual(step, 1.0));
}

} // namespace OHOS::Ace