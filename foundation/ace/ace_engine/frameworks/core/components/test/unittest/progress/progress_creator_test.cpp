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
#include "adapter/aosp/entrance/java/jni/jni_environment.h"
#include "base/utils/utils.h"
#include "core/components/common/properties/color.h"
#include "core/components/progress/loading_progress_component.h"
#include "core/components/progress/progress_theme.h"
#include "core/components/test/json/loading_progress_creator.h"
#include "core/components/test/json/progress_creator.h"
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
constexpr double JSON_CACHED_VALUE = 0.0;
constexpr double JSON_THICKNESS = 10.0;
// This color value is easy to compare
const Color TEST_TRACK_SELECTED_COLOR = Color(123456789);
const Color TEST_TRACK_BACKGROUND_COLOR = Color(987654321);
const Color TEST_TRACK_CACHED_COLOR = Color(567891234);

constexpr double JSON_DIAMETER = 50.0;
constexpr double DEFAULT_DIAMETER = 100.0;

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

class ProgressCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    RefPtr<ProgressComponent> CreateComponent(const std::string& testJson);
    void CompareAllProperty(const RefPtr<ProgressComponent>& progress);
    void CompareAllDefault(const RefPtr<ProgressComponent>& progress);
};

void ProgressCreatorTest::SetUpTestCase() {}

void ProgressCreatorTest::TearDownTestCase() {}

void ProgressCreatorTest::SetUp() {}

void ProgressCreatorTest::TearDown() {}

RefPtr<ProgressComponent> ProgressCreatorTest::CreateComponent(const std::string& testJson)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(testJson.c_str());
    size_t dslLen = testJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    if (component == nullptr) {
        return nullptr;
    }
    return AceType::DynamicCast<ProgressComponent>(component);
}

void ProgressCreatorTest::CompareAllProperty(const RefPtr<ProgressComponent>& progress)
{
    double min = progress->GetMinValue();
    double max = progress->GetMaxValue();
    double cachedValue = progress->GetCachedValue();
    double value = progress->GetValue();
    EXPECT_TRUE(NearEqual(min, JSON_MIN_VALUE));
    EXPECT_TRUE(NearEqual(max, JSON_MAX_VALUE));
    EXPECT_TRUE(NearEqual(cachedValue, JSON_CACHED_VALUE));
    EXPECT_TRUE(NearEqual(value, JSON_VALUE));
    RefPtr<TrackComponent> track = progress->GetTrack();
    Color trackBackgroundColor = track->GetBackgroundColor();
    EXPECT_TRUE(trackBackgroundColor == TEST_TRACK_BACKGROUND_COLOR);
    Color selectedColor = track->GetSelectColor();
    EXPECT_TRUE(selectedColor == TEST_TRACK_SELECTED_COLOR);
    Color cachedColor = track->GetCachedColor();
    EXPECT_TRUE(cachedColor == TEST_TRACK_CACHED_COLOR);
    double thickness = track->GetTrackThickness().Value();
    EXPECT_TRUE(NearEqual(thickness, JSON_THICKNESS));
}

void ProgressCreatorTest::CompareAllDefault(const RefPtr<ProgressComponent>& progress)
{
    double min = progress->GetMinValue();
    double max = progress->GetMaxValue();
    double cachedValue = progress->GetCachedValue();
    double value = progress->GetValue();
    EXPECT_TRUE(NearEqual(min, JSON_MIN_VALUE));
    EXPECT_TRUE(NearEqual(max, JSON_MAX_VALUE));
    EXPECT_TRUE(NearEqual(cachedValue, JSON_CACHED_VALUE));
    EXPECT_TRUE(NearEqual(value, JSON_VALUE));
    auto type = progress->GetType();
    EXPECT_EQ(type, ProgressType::LINEAR);
    RefPtr<TrackComponent> track = progress->GetTrack();
    Color trackBackgroundColor = track->GetBackgroundColor();
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    auto theme = themeManager->GetTheme<ProgressTheme>();
    ASSERT_TRUE(theme);
    EXPECT_TRUE(trackBackgroundColor == theme->GetTrackBgColor());
    Color selectedColor = track->GetSelectColor();
    EXPECT_TRUE(selectedColor == theme->GetTrackSelectedColor());
    Color cachedColor = track->GetCachedColor();
    EXPECT_TRUE(cachedColor == theme->GetTrackCachedColor());
    auto thickness = track->GetTrackThickness();
    EXPECT_TRUE(thickness == theme->GetTrackThickness());
}

/**
 * @tc.name: ProgressComponentCreator001
 * @tc.desc: Test all the property created properly
 * @tc.type: FUNC
 * @tc.require: AR000DAR1T AR000DAR1V
 * @tc.author: YaoYuchi
 */
HWTEST_F(ProgressCreatorTest, ProgressComponentCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with all property.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Progress\",    "
                           "  \"value\": 0.0,                 "
                           "  \"min\": 0.0,                   "
                           "  \"max\": 10.0,                  "
                           "  \"cachedValue\": 0.0,           "
                           "  \"thickness\": 10.0,            "
                           "  \"selectColor\": {              "
                           "  \"className\": \"Color\",       "
                           "  \"value\":  123456789           "
                           "                         },       "
                           "  \"trackColor\": {          "
                           "  \"className\": \"Color\",       "
                           "  \"value\":  987654321           "
                           "                         },       "
                           "  \"cachedColor\": {              "
                           "  \"className\": \"Color\",       "
                           "  \"value\":  567891234           "
                           "                         }        "
                           "}";
    /**
     * @tc.steps: step2. check all property matched
     * @tc.expected: step2. all property match
     */
    RefPtr<ProgressComponent> progress = CreateComponent(testJson);
    ASSERT_TRUE(progress != nullptr);
    CompareAllProperty(progress);
    GTEST_LOG_(INFO) << progress->GetMaxValue() << " " << progress->GetMinValue();
    auto type = progress->GetType();
    EXPECT_EQ(type, ProgressType::LINEAR);
}

/**
 * @tc.name: ProgressComponentCreator002
 * @tc.desc: Test default property. And all essential properly.
 * @tc.type: FUNC
 * @tc.require: AR000DAR1T AR000DAR1V
 * @tc.author: YaoYuchi
 */
HWTEST_F(ProgressCreatorTest, ProgressComponentCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with all property.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Progress\",    "
                           "  \"value\": 0.0,                 "
                           "  \"min\": 0.0,                   "
                           "  \"max\": 10.0,                  "
                           "  \"cachedValue\": 0.0            "
                           "}";
    /**
     * @tc.steps: step2. check all property including default property matched
     * @tc.expected: step2. all property match
     */
    RefPtr<ProgressComponent> progress = CreateComponent(testJson);
    ASSERT_TRUE(progress != nullptr);
    CompareAllDefault(progress);
}

/**
 * @tc.name: ProgressComponentCreator003
 * @tc.desc: Missing min property. Fail to create component.
 * @tc.type: FUNC
 * @tc.require: AR000DAR1T AR000DAR1V
 * @tc.author: YaoYuchi
 */
HWTEST_F(ProgressCreatorTest, ProgressComponentCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with missing min.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Progress\",    "
                           "  \"value\": 0.0,                 "
                           "  \"max\": 10.0,                  "
                           "  \"cachedValue\": 0.0            "
                           "}";
    /**
     * @tc.steps: step2. check component is null or not
     * @tc.expected: step2. component is null
     */
    RefPtr<ProgressComponent> progress = CreateComponent(testJson);
    EXPECT_TRUE(progress == nullptr);
}

/**
 * @tc.name: ProgressComponentCreator004
 * @tc.desc: Missing max property. Fail to create component.
 * @tc.type: FUNC
 * @tc.require: AR000DAR1T AR000DAR1V
 * @tc.author: YaoYuchi
 */
HWTEST_F(ProgressCreatorTest, ProgressComponentCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with missing max.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Progress\",    "
                           "  \"value\": 10.0,                "
                           "  \"min\": 10.0,                  "
                           "  \"cachedValue\": 0.0            "
                           "}";
    /**
     * @tc.steps: step2. check component is null or not
     * @tc.expected: step2. component is null
     */
    RefPtr<ProgressComponent> progress = CreateComponent(testJson);
    EXPECT_TRUE(progress == nullptr);
}

/**
 * @tc.name: ProgressComponentCreator005
 * @tc.desc: Missing value property. Fail to create component.
 * @tc.type: FUNC
 * @tc.require: AR000DAR1T AR000DAR1V
 * @tc.author: YaoYuchi
 */
HWTEST_F(ProgressCreatorTest, ProgressComponentCreator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with missing value.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Progress\",    "
                           "  \"min\": 0.0,                 "
                           "  \"max\": 10.0,                  "
                           "  \"cachedValue\": 0.0            "
                           "}";
    /**
     * @tc.steps: step2. check component is null or not
     * @tc.expected: step2. component is null
     */
    RefPtr<ProgressComponent> progress = CreateComponent(testJson);
    EXPECT_TRUE(progress == nullptr);
}

/**
 * @tc.name: ProgressComponentCreator006
 * @tc.desc: Missing cachedValue property. Fail to create component.
 * @tc.type: FUNC
 * @tc.require: AR000DAR1T AR000DAR1V
 * @tc.author: YaoYuchi
 */
HWTEST_F(ProgressCreatorTest, ProgressComponentCreator006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with missing value.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Progress\",    "
                           "  \"min\": 0.0,                 "
                           "  \"max\": 10.0,                  "
                           "  \"value\": 0.0            "
                           "}";
    /**
     * @tc.steps: step2. check component is null or not
     * @tc.expected: step2. component is null
     */
    RefPtr<ProgressComponent> progress = CreateComponent(testJson);
    EXPECT_TRUE(progress == nullptr);
}

/**
 * @tc.name: ProgressComponentCreator007
 * @tc.desc: Max and min value is illegal.
 * @tc.type: FUNC
 * @tc.require: AR000DAR1T AR000DAR1V
 * @tc.author: YaoYuchi
 */
HWTEST_F(ProgressCreatorTest, ProgressComponentCreator007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with min = max.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Progress\",    "
                           "  \"min\": 10.0,                  "
                           "  \"max\": 10.0,                  "
                           "  \"value\": 10.0,                "
                           "  \"cachedValue\": 10.0           "
                           "}";
    /**
     * @tc.steps: step2. check component is null or not
     * @tc.expected: step2. component is null
     */
    RefPtr<ProgressComponent> progress = CreateComponent(testJson);
    EXPECT_TRUE(progress == nullptr);
}

/**
 * @tc.name: ProgressComponentCreator008
 * @tc.desc: Value is illegal.
 * @tc.type: FUNC
 * @tc.require: AR000DAR1T AR000DAR1V
 * @tc.author: YaoYuchi
 */
HWTEST_F(ProgressCreatorTest, ProgressComponentCreator008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with value < min.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Progress\",    "
                           "  \"min\": 8.0,                   "
                           "  \"max\": 10.0,                  "
                           "  \"value\": 0.0,                 "
                           "  \"cachedValue\": 10.0           "
                           "}";
    /**
     * @tc.steps: step2. check component is null or not
     * @tc.expected: step2. component is null
     */
    RefPtr<ProgressComponent> progress = CreateComponent(testJson);
    EXPECT_TRUE(progress == nullptr);
}

/**
 * @tc.name: ProgressComponentCreator009
 * @tc.desc: CachedValue is illegal.
 * @tc.type: FUNC
 * @tc.require: AR000DAR1T AR000DAR1V
 * @tc.author: YaoYuchi
 */
HWTEST_F(ProgressCreatorTest, ProgressComponentCreator009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with cachedValue < min.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Progress\",    "
                           "  \"min\": 8.0,                   "
                           "  \"max\": 10.0,                  "
                           "  \"value\": 9.0,                 "
                           "  \"cachedValue\": 0.0            "
                           "}";
    /**
     * @tc.steps: step2. check component is null or not
     * @tc.expected: step2. component is null
     */
    RefPtr<ProgressComponent> progress = CreateComponent(testJson);
    EXPECT_TRUE(progress == nullptr);
}

/**
 * @tc.name: ProgressComponentCreator010
 * @tc.desc: Create circular progress with all parameter
 * @tc.type: FUNC
 * @tc.require: AR000DAR1U AR000DAR1V
 * @tc.author: YaoYuchi
 */
HWTEST_F(ProgressCreatorTest, ProgressComponentCreator010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with all parameter.
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Progress\",    "
                           "  \"value\": 0.0,                 "
                           "  \"min\": 0.0,                   "
                           "  \"max\": 10.0,                  "
                           "  \"cachedValue\": 0.0,           "
                           "  \"thickness\": 10.0,            "
                           "  \"type\": 4,                    "
                           "  \"selectColor\": {              "
                           "  \"className\": \"Color\",       "
                           "  \"value\":  123456789           "
                           "                         },       "
                           "  \"trackColor\": {               "
                           "  \"className\": \"Color\",       "
                           "  \"value\":  987654321           "
                           "                         },       "
                           "  \"cachedColor\": {              "
                           "  \"className\": \"Color\",       "
                           "  \"value\":  567891234           "
                           "                         }        "
                           "}";
    /**
     * @tc.steps: step2. check component is null or not and compare all parameter.
     * @tc.expected: step2. component is null
     */
    RefPtr<ProgressComponent> progress = CreateComponent(testJson);
    ASSERT_TRUE(progress != nullptr);
    CompareAllProperty(progress);
    auto type = progress->GetType();
    EXPECT_EQ(type, ProgressType::CIRCLE);
}

/**
 * @tc.name: LoadingProgressCreator001
 * @tc.desc: Input correct loading progress json. The loading progress can be generated correctly.
 * @tc.type: FUNC
 * @tc.require: AR000DAUN8
 * @tc.author: YaoYuchi
 */
HWTEST_F(ProgressCreatorTest, LoadingProgressCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with diameter = 50.0
     */
    std::string testJson = "{                                    "
                           "  \"className\": \"LoadingProgress\","
                           "  \"diameter\": 50.0                 "
                           "}";
    /**
     * @tc.steps: step2. check the diameter is 50.0
     * @tc.expected: step2. diameter matches
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(testJson.c_str());
    size_t dslLen = testJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    EXPECT_TRUE(component != nullptr);
    RefPtr<LoadingProgressComponent> loadingProgress = AceType::DynamicCast<LoadingProgressComponent>(component);
    EXPECT_TRUE(loadingProgress != nullptr);
    EXPECT_TRUE(NearEqual(loadingProgress->GetDiameter().Value(), JSON_DIAMETER));
}

/**
 * @tc.name: LoadingProgressCreator002
 * @tc.desc: Input correct loading progress json. The loading progress can be generated correctly. And the diameter
 * is default value.
 * @tc.type: FUNC
 * @tc.require: AR000DAUN8
 * @tc.author: YaoYuchi
 */
HWTEST_F(ProgressCreatorTest, LoadingProgressCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with no diameter = 50.0
     */
    std::string testJson = "{                                    "
                           "  \"className\": \"LoadingProgress\" "
                           "}";
    /**
     * @tc.steps: step2. check the diameter is 100.0
     * @tc.expected: step2. diameter matches
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(testJson.c_str());
    size_t dslLen = testJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<LoadingProgressComponent> loadingProgress = AceType::DynamicCast<LoadingProgressComponent>(component);
    ASSERT_TRUE(loadingProgress != nullptr);
    EXPECT_TRUE(NearEqual(loadingProgress->GetDiameter().Value(), DEFAULT_DIAMETER));
}

} // namespace OHOS::Ace
