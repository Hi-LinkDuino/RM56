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

#include "base/json/json_util.h"
#include "base/utils/utils.h"
#include "core/components/display/display_component.h"
#include "core/components/test/json/component_factory.h"
#include "core/components/text/text_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const double OPACITY_TRANSPARENT = 0.0;
const double OPACITY_TRANSLUCENT = 0.5;
const double OPACITY_OPAQUE = 1.0;
const std::string TEXT_VALUE = "HiAce";

} // namespace

class DisplayCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DisplayCreatorTest::SetUpTestCase() {}
void DisplayCreatorTest::TearDownTestCase() {}
void DisplayCreatorTest::SetUp() {}
void DisplayCreatorTest::TearDown() {}

/**
 * @tc.name: DisplayCreator001
 * @tc.desc: Verify DisplayCreator can create display component with child.
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCreatorTest, DisplayCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of display component with child.
     */
    const std::string jsonStr = ""
                                "{                              "
                                "  \"className\": \"Display\",  "
                                "  \"child\": {                 "
                                "    \"className\": \"Text\",   "
                                "    \"value\": \"HiAce\"      "
                                "  }                            "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get display component.
     * @tc.expected: step2. Set child correctly, and set visible and opacity as default.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonStr.c_str());
    size_t dslLen = jsonStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<DisplayComponent> display = AceType::DynamicCast<DisplayComponent>(component);
    ASSERT_TRUE(display != nullptr);
    ASSERT_TRUE(display->GetVisible() == VisibleType::VISIBLE);
    ASSERT_TRUE(display->GetOpacity() == OPACITY_OPAQUE);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(display->GetChild());
    ASSERT_TRUE(text != nullptr);
    ASSERT_TRUE(text->GetData() == TEXT_VALUE);
}

/**
 * @tc.name: DisplayCreator002
 * @tc.desc: Verify DisplayCreator can handle exception of no child.
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCreatorTest, DisplayCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of display component with no child.
     */
    const std::string jsonStr = ""
                                "{                              "
                                "  \"className\": \"Display\"   "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get display component.
     * @tc.expected: step2. display is null.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonStr.c_str());
    size_t dslLen = jsonStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component == nullptr);
}

/**
 * @tc.name: DisplayCreator003
 * @tc.desc: Verify DisplayCreator can create display component with visible and child.
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCreatorTest, DisplayCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of display component with visible and child.
     */
    const std::string jsonStr = ""
                                "{                              "
                                "  \"className\": \"Display\",  "
                                "  \"visible\": \"visible\",    "
                                "  \"child\": {                 "
                                "    \"className\": \"Text\",   "
                                "    \"value\": \"HiAce\"       "
                                "  }                            "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get display component.
     * @tc.expected: step2. Set visible correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonStr.c_str());
    size_t dslLen = jsonStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<DisplayComponent> display = AceType::DynamicCast<DisplayComponent>(component);
    ASSERT_TRUE(display != nullptr);
    ASSERT_TRUE(display->GetVisible() == VisibleType::VISIBLE);
}

/**
 * @tc.name: DisplayCreator004
 * @tc.desc: Verify DisplayCreator can create display component with visible and child.
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCreatorTest, DisplayCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of display component with visible and child.
     */
    const std::string jsonStr = ""
                                "{                              "
                                "  \"className\": \"Display\",  "
                                "  \"visible\": \"invisible\",  "
                                "  \"child\": {                 "
                                "    \"className\": \"Text\",   "
                                "    \"value\": \"HiAce\"       "
                                "  }                            "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get display component.
     * @tc.expected: step2. Set visible correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonStr.c_str());
    size_t dslLen = jsonStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<DisplayComponent> display = AceType::DynamicCast<DisplayComponent>(component);
    ASSERT_TRUE(display != nullptr);
    ASSERT_TRUE(display->GetVisible() == VisibleType::INVISIBLE);
}

/**
 * @tc.name: DisplayCreator005
 * @tc.desc: Verify DisplayCreator can create display component with visible and child.
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCreatorTest, DisplayCreator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of display component with visible and child.
     */
    const std::string jsonStr = ""
                                "{                              "
                                "  \"className\": \"Display\",  "
                                "  \"visible\": \"gone\",    "
                                "  \"child\": {                 "
                                "    \"className\": \"Text\",   "
                                "    \"value\": \"HiAce\"       "
                                "  }                            "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get display component.
     * @tc.expected: step2. Set visible correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonStr.c_str());
    size_t dslLen = jsonStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<DisplayComponent> display = AceType::DynamicCast<DisplayComponent>(component);
    ASSERT_TRUE(display != nullptr);
    ASSERT_TRUE(display->GetVisible() == VisibleType::GONE);
}

/**
 * @tc.name: DisplayCreator006
 * @tc.desc: Verify DisplayCreator can create display component with child and wrong visible value.
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCreatorTest, DisplayCreator006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of display component with child and wrong visible value.
     */
    const std::string jsonStr = ""
                                "{                              "
                                "  \"className\": \"Display\",  "
                                "  \"visible\": \"default\",    "
                                "  \"child\": {                 "
                                "    \"className\": \"Text\",   "
                                "    \"value\": \"HiAce\"       "
                                "  }                            "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get display component.
     * @tc.expected: step2. Set visible as default.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonStr.c_str());
    size_t dslLen = jsonStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<DisplayComponent> display = AceType::DynamicCast<DisplayComponent>(component);
    ASSERT_TRUE(display != nullptr);
    ASSERT_TRUE(display->GetVisible() == VisibleType::VISIBLE);
}

/**
 * @tc.name: DisplayCreator007
 * @tc.desc: Verify DisplayCreator can create display component with child and wrong visible value.
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCreatorTest, DisplayCreator007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of display component with child and wrong visible value.
     */
    const std::string jsonStr = ""
                                "{                              "
                                "  \"className\": \"Display\",  "
                                "  \"visible\": 1.0,            "
                                "  \"child\": {                 "
                                "    \"className\": \"Text\",   "
                                "    \"value\": \"HiAce\"       "
                                "  }                            "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get display component.
     * @tc.expected: step2. Set visible as default.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonStr.c_str());
    size_t dslLen = jsonStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<DisplayComponent> display = AceType::DynamicCast<DisplayComponent>(component);
    ASSERT_TRUE(display != nullptr);
    ASSERT_TRUE(display->GetVisible() == VisibleType::VISIBLE);
}

/**
 * @tc.name: DisplayCreator008
 * @tc.desc: Verify DisplayCreator can create display component with child and opacity.
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCreatorTest, DisplayCreator008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of display component with child and opacity.
     */
    const std::string jsonStr = ""
                                "{                              "
                                "  \"className\": \"Display\",  "
                                "  \"opacity\": 0.0,            "
                                "  \"child\": {                 "
                                "    \"className\": \"Text\",   "
                                "    \"value\": \"HiAce\"       "
                                "  }                            "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get display component.
     * @tc.expected: step2. Set opacity correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonStr.c_str());
    size_t dslLen = jsonStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<DisplayComponent> display = AceType::DynamicCast<DisplayComponent>(component);
    ASSERT_TRUE(display != nullptr);
    ASSERT_TRUE(display->GetOpacity() == OPACITY_TRANSPARENT);
}

/**
 * @tc.name: DisplayCreator009
 * @tc.desc: Verify DisplayCreator can create display component with child and opacity.
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCreatorTest, DisplayCreator009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of display component with child and opacity.
     */
    const std::string jsonStr = ""
                                "{                              "
                                "  \"className\": \"Display\",  "
                                "  \"opacity\": 1.0,            "
                                "  \"child\": {                 "
                                "    \"className\": \"Text\",   "
                                "    \"value\": \"HiAce\"       "
                                "  }                            "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get display component.
     * @tc.expected: step2. Set opacity correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonStr.c_str());
    size_t dslLen = jsonStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<DisplayComponent> display = AceType::DynamicCast<DisplayComponent>(component);
    ASSERT_TRUE(display != nullptr);
    ASSERT_TRUE(display->GetOpacity() == OPACITY_OPAQUE);
}

/**
 * @tc.name: DisplayCreator010
 * @tc.desc: Verify DisplayCreator can create display component with child and wrong opacity value.
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCreatorTest, DisplayCreator010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of display component with child and wrong opacity value.
     */
    const std::string jsonStr = ""
                                "{                              "
                                "  \"className\": \"Display\",  "
                                "  \"opacity\": -1.0,            "
                                "  \"child\": {                 "
                                "    \"className\": \"Text\",   "
                                "    \"value\": \"HiAce\"       "
                                "  }                            "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get display component.
     * @tc.expected: step2. Set opacity as transparent.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonStr.c_str());
    size_t dslLen = jsonStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<DisplayComponent> display = AceType::DynamicCast<DisplayComponent>(component);
    ASSERT_TRUE(display != nullptr);
    ASSERT_TRUE(display->GetOpacity() == OPACITY_TRANSPARENT);
}

/**
 * @tc.name: DisplayCreator011
 * @tc.desc: Verify DisplayCreator can create display component with child and wrong opacity value.
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCreatorTest, DisplayCreator011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of display component with child and wrong opacity value.
     */
    const std::string jsonStr = ""
                                "{                              "
                                "  \"className\": \"Display\",  "
                                "  \"opacity\": 2.0,            "
                                "  \"child\": {                 "
                                "    \"className\": \"Text\",   "
                                "    \"value\": \"HiAce\"       "
                                "  }                            "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get display component.
     * @tc.expected: step2. Set opacity as opaque.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonStr.c_str());
    size_t dslLen = jsonStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<DisplayComponent> display = AceType::DynamicCast<DisplayComponent>(component);
    ASSERT_TRUE(display != nullptr);
    ASSERT_TRUE(display->GetOpacity() == OPACITY_OPAQUE);
}

/**
 * @tc.name: DisplayCreator012
 * @tc.desc: Verify DisplayCreator can create display component with child and opacity.
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCreatorTest, DisplayCreator012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of display component with child and opacity.
     */
    const std::string jsonStr = ""
                                "{                              "
                                "  \"className\": \"Display\",  "
                                "  \"opacity\": 0.5,            "
                                "  \"child\": {                 "
                                "    \"className\": \"Text\",   "
                                "    \"value\": \"HiAce\"       "
                                "  }                            "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get display component.
     * @tc.expected: step2. Set opacity correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonStr.c_str());
    size_t dslLen = jsonStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<DisplayComponent> display = AceType::DynamicCast<DisplayComponent>(component);
    ASSERT_TRUE(display != nullptr);
    ASSERT_TRUE(display->GetOpacity() == OPACITY_TRANSLUCENT);
}

/**
 * @tc.name: DisplayCreator013
 * @tc.desc: Verify DisplayCreator can create display component with child and wrong opacity value.
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCreatorTest, DisplayCreator013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of display component with child and wrong opacity value.
     */
    const std::string jsonStr = ""
                                "{                              "
                                "  \"className\": \"Display\",  "
                                "  \"opacity\": \"opacity\",    "
                                "  \"child\": {                 "
                                "    \"className\": \"Text\",   "
                                "    \"value\": \"HiAce\"       "
                                "  }                            "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get display component.
     * @tc.expected: step2. Set opacity as default.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonStr.c_str());
    size_t dslLen = jsonStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<DisplayComponent> display = AceType::DynamicCast<DisplayComponent>(component);
    ASSERT_TRUE(display != nullptr);
    ASSERT_TRUE(display->GetOpacity() == OPACITY_OPAQUE);
}

} // namespace OHOS::Ace