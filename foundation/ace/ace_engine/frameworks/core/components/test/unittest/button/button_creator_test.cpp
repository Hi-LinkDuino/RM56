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

#include "core/components/button/button_component.h"
#include "core/components/test/json/button_creator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr double BUTTON_BORDER_WIDTH_TEST = 3.0;
constexpr int BUTTON_DISABLED_STATE_TEST = 1;
constexpr int BUTTON_AUTOFOCUS_STATE_TEST = 1;
constexpr uint32_t COLOR_VALUE_TEST = 255;
const std::string BUTTON_EVENT_ID_TEST = "ButtonClickEvent";
const Dimension BUTTON_WIDTH_TEST = Dimension(400.0);
const Dimension BUTTON_HEIGHT_TEST = Dimension(100.0);
const Dimension BUTTON_RADIUS_TEST = Dimension(10.0);

/**
 * Create component from dsl.
 *
 * @param testJsonStr The json string which will be parsed.
 * @return RefPtr of Component.
 */
RefPtr<Component> CreateComponent(const std::string& testJsonStr)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(testJsonStr.c_str());
    size_t dslLen = testJsonStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    return component;
}

} // namespace

class ButtonCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ButtonCreatorTest::SetUpTestCase() {}
void ButtonCreatorTest::TearDownTestCase() {}
void ButtonCreatorTest::SetUp() {}
void ButtonCreatorTest::TearDown() {}

/**
 * @tc.name: ParseProperties001
 * @tc.desc: All parsed correctly when all properties are configured.
 * @tc.type: FUNC
 */
HWTEST_F(ButtonCreatorTest, ParseProperties001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string for button contains all properties.
     */
    const std::string jsonStr = ""
                                "{                                                                            "
                                "  \"className\": \"Button\", \"width\": 400, \"height\": 100, \"type\": 3,   "
                                "  \"rrectRadius\": 10, \"disabledState\": 1, \"autoFocus\": 1,               "
                                "  \"backgroundColor\": { \"className\": \"Color\", \"value\": 255 },         "
                                "  \"clickedColor\": { \"className\": \"Color\", \"value\": 255 },            "
                                "  \"disabledColor\": { \"className\": \"Color\", \"value\": 255 },           "
                                "  \"focusColor\": { \"className\": \"Color\", \"value\": 255 },              "
                                "  \"buttonBorder\": { \"className\": \"BorderEdge\", \"width\": 3.0 },       "
                                "  \"buttonClickedEventId\": \"ButtonClickEvent\"                             "
                                "}";

    /**
     * @tc.steps: step2. Parse the json string and compare the result with configured value.
     * @tc.expected: step2. All properties parsed correctly and the result equal with configured value.
     */
    RefPtr<Component> component = CreateComponent(jsonStr);
    ASSERT_TRUE(component != nullptr);
    RefPtr<ButtonComponent> button = AceType::DynamicCast<ButtonComponent>(component);
    ASSERT_TRUE(button != nullptr);
    EXPECT_TRUE(button->GetWidth() == BUTTON_WIDTH_TEST);
    EXPECT_TRUE(button->GetHeight() == BUTTON_HEIGHT_TEST);
    EXPECT_TRUE(button->GetType() == ButtonType::TEXT);
    EXPECT_TRUE(button->GetRectRadius() == BUTTON_RADIUS_TEST);
    EXPECT_TRUE(button->GetDisabledState() == BUTTON_DISABLED_STATE_TEST);
    EXPECT_TRUE(button->GetAutoFocusState() == BUTTON_AUTOFOCUS_STATE_TEST);
    EXPECT_TRUE(button->GetBackgroundColor().GetValue() == COLOR_VALUE_TEST);
    EXPECT_TRUE(button->GetClickedColor().GetValue() == COLOR_VALUE_TEST);
    EXPECT_TRUE(button->GetDisabledColor().GetValue() == COLOR_VALUE_TEST);
    EXPECT_TRUE(button->GetFocusColor().GetValue() == COLOR_VALUE_TEST);
    EXPECT_TRUE(button->GetBorderEdge().GetWidth().Value() == BUTTON_BORDER_WIDTH_TEST);
    EXPECT_TRUE(button->GetClickedEventId() == BUTTON_EVENT_ID_TEST);
}

/**
 * @tc.name: ParseProperties002
 * @tc.desc: All properties have the same value as the default when no property is configured.
 * @tc.type: FUNC
 */
HWTEST_F(ButtonCreatorTest, ParseProperties002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string for button does not contain any property.
     */
    const std::string jsonStr = ""
                                "{                             "
                                "  \"className\": \"Button\"   "
                                "}";

    /**
     * @tc.steps: step2. Parse the json string and compare the result with configured value.
     * @tc.expected: step2. All properties have the same value as the default.
     */
    RefPtr<Component> component = CreateComponent(jsonStr);
    ASSERT_TRUE(component != nullptr);
    RefPtr<ButtonComponent> button = AceType::DynamicCast<ButtonComponent>(component);
    ASSERT_TRUE(button != nullptr);
    EXPECT_TRUE(button->GetWidth() == Dimension(-1.0));
    EXPECT_TRUE(button->GetHeight() == Dimension(-1.0));
    EXPECT_TRUE(button->GetType() == ButtonType::NORMAL);
    EXPECT_TRUE(button->GetRectRadius() == Dimension(0.0));
    EXPECT_TRUE(button->GetDisabledState() == 0);
    EXPECT_TRUE(button->GetAutoFocusState() == 0);
    EXPECT_TRUE(button->GetClickedEventId() == "");
}

} // namespace OHOS::Ace