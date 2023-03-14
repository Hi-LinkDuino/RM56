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
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/test/json/component_factory.h"
#include "core/components/text_field/text_field_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr uint32_t TEXT_FIELD_ATTR_MAX_LINES_DEFAULT = 1;
constexpr uint32_t TEXT_FIELD_ATTR_MAX_LENGTH_DEFAULT = std::numeric_limits<uint32_t>::max();
const TextAlign TEXT_FIELD_ATTR_ALIGN_DEFAULT = TextAlign::START;
const TextDirection TEXT_FIELD_ATTR_DIRECTION_DEFAULT = TextDirection::INHERIT;
const TextInputType TEXT_FIELD_ATTR_INPUT_TYPE_DEFAULT = TextInputType::TEXT;
const TextInputAction TEXT_FIELD_ATTR_ACTION_DEFAULT = TextInputAction::UNSPECIFIED;
const std::string TEXT_FIELD_ATTR_ACTION_LABEL_DEFAULT;
const std::string TEXT_FIELD_ATTR_PLACEHOLDER_DEFAULT;
const uint32_t TEXT_FIELD_ATTR_COLOR_DEFAULT = 4278190080;
constexpr uint32_t TEXT_FIELD_ATTR_MAX_LINES = 3;
constexpr uint32_t TEXT_FIELD_ATTR_MAX_LENGTH = 50;
constexpr uint32_t TEXT_FIELD_ATTR_ALIGN = 2;
constexpr uint32_t TEXT_FIELD_ATTR_DIRECTION = 2;
constexpr uint32_t TEXT_FIELD_ATTR_INPUT_TYPE = 0;
constexpr uint32_t TEXT_FIELD_ATTR_ACTION = 0;
const std::string TEXT_FIELD_ATTR_ACTION_LABEL = "TextField";
const std::string TEXT_FIELD_ATTR_PLACEHOLDER = "please input";
constexpr uint32_t TEXT_FIELD_ATTR_COLOR_ALPHA = 100;
constexpr uint32_t TEXT_FIELD_ATTR_COLOR_RED = 100;
constexpr uint32_t TEXT_FIELD_ATTR_COLOR_GREEN = 100;
constexpr uint32_t TEXT_FIELD_ATTR_COLOR_BLUE = 100;
const std::string TEXT_FIELD_ATTR_ICON_IMAGE = "test.png";

} // namespace

class TextFieldCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void TextFieldCreatorTest::SetUpTestCase() {}
void TextFieldCreatorTest::TearDownTestCase() {}
void TextFieldCreatorTest::SetUp() {}
void TextFieldCreatorTest::TearDown() {}

/**
 * @tc.name: TextFieldComponentCreator001
 * @tc.desc: Test all the properties of text_field.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7U AR000DQ1Q1 AR000DQ1Q2
 * @tc.author: hushilong
 */
HWTEST_F(TextFieldCreatorTest, TextFieldComponentCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with all property.
     */
    const std::string jsonTextFieldStr = ""
                                         "{                                     "
                                         "  \"className\": \"TextField\",       "
                                         "  \"obscure\": false,                 "
                                         "  \"enabled\": true,                  "
                                         "  \"autoFocus\": false,               "
                                         "  \"maxLines\": 3,                    "
                                         "  \"maxLength\": 50,                  "
                                         "  \"action\": 0,                      "
                                         "  \"actionLabel\": \"TextField\",     "
                                         "  \"textOverflow\": 2,                "
                                         "  \"placeholder\": \"please input\",  "
                                         "  \"placeholderColor\":               "
                                         "    {"
                                         "      \"className\": \"Color\",       "
                                         "      \"alpha\": 100,                 "
                                         "      \"red\": 100,                   "
                                         "      \"green\": 100,                 "
                                         "      \"blue\": 100                   "
                                         "    },"
                                         "  \"cursorColor\":                    "
                                         "    {"
                                         "      \"className\": \"Color\",       "
                                         "      \"alpha\": 100,                 "
                                         "      \"red\": 100,                   "
                                         "      \"green\": 100,                 "
                                         "      \"blue\": 100                   "
                                         "    },"
                                         "  \"keyboardType\": 0,                "
                                         "  \"textAlign\": 2,                   "
                                         "  \"extend\": true,                   "
                                         "  \"iconImage\": \"test.png\"         "
                                         "}";
    /**
     * @tc.steps: step2. Check all the properties matched.
     * @tc.expected: step2. All the properties are matched.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonTextFieldStr.c_str());
    size_t dslLen = jsonTextFieldStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    RefPtr<TextFieldComponent> textFieldComponent = AceType::DynamicCast<TextFieldComponent>(component);
    EXPECT_TRUE(textFieldComponent != nullptr);
    EXPECT_TRUE(!textFieldComponent->NeedObscure());
    EXPECT_TRUE(textFieldComponent->IsEnabled());
    EXPECT_TRUE(!textFieldComponent->GetAutoFocus());
    EXPECT_TRUE(textFieldComponent->IsExtend());
    EXPECT_EQ(static_cast<uint32_t>(textFieldComponent->GetTextMaxLines()), TEXT_FIELD_ATTR_MAX_LINES);
    EXPECT_EQ(static_cast<uint32_t>(textFieldComponent->GetMaxLength()), TEXT_FIELD_ATTR_MAX_LENGTH);
    EXPECT_EQ(static_cast<uint32_t>(textFieldComponent->GetAction()), TEXT_FIELD_ATTR_ACTION);
    EXPECT_EQ(textFieldComponent->GetActionLabel(), TEXT_FIELD_ATTR_ACTION_LABEL);
    EXPECT_EQ(static_cast<uint32_t>(textFieldComponent->GetTextDirection()), TEXT_FIELD_ATTR_DIRECTION);
    EXPECT_EQ(static_cast<uint32_t>(textFieldComponent->GetTextInputType()), TEXT_FIELD_ATTR_INPUT_TYPE);
    EXPECT_EQ(static_cast<uint32_t>(textFieldComponent->GetTextAlign()), TEXT_FIELD_ATTR_ALIGN);
    EXPECT_EQ(textFieldComponent->GetIconImage(), TEXT_FIELD_ATTR_ICON_IMAGE);
    // placeholder
    EXPECT_EQ(textFieldComponent->GetPlaceholder(), TEXT_FIELD_ATTR_PLACEHOLDER);
    const auto& placeholderColor = textFieldComponent->GetPlaceholderColor();
    EXPECT_EQ(placeholderColor.GetAlpha(), TEXT_FIELD_ATTR_COLOR_ALPHA);
    EXPECT_EQ(placeholderColor.GetRed(), TEXT_FIELD_ATTR_COLOR_RED);
    EXPECT_EQ(placeholderColor.GetGreen(), TEXT_FIELD_ATTR_COLOR_GREEN);
    EXPECT_EQ(placeholderColor.GetBlue(), TEXT_FIELD_ATTR_COLOR_BLUE);
    // cursor color
    const auto& cursorColor = textFieldComponent->GetCursorColor();
    EXPECT_EQ(cursorColor.GetAlpha(), TEXT_FIELD_ATTR_COLOR_ALPHA);
    EXPECT_EQ(cursorColor.GetRed(), TEXT_FIELD_ATTR_COLOR_RED);
    EXPECT_EQ(cursorColor.GetGreen(), TEXT_FIELD_ATTR_COLOR_GREEN);
    EXPECT_EQ(cursorColor.GetBlue(), TEXT_FIELD_ATTR_COLOR_BLUE);
}

/**
 * @tc.name: TextFieldComponentCreator002
 * @tc.desc: Test all the properties when initialized by default value.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7U
 * @tc.author: hushilong
 */
HWTEST_F(TextFieldCreatorTest, TextFieldComponentCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with default.
     */
    const std::string jsonTextFieldStr = ""
                                         "{                                     "
                                         "  \"className\": \"TextField\"        "
                                         "}";
    /**
     * @tc.steps: step2. Check all the properties matched.
     * @tc.expected: step2. All the properties are matched.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonTextFieldStr.c_str());
    size_t dslLen = jsonTextFieldStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    RefPtr<TextFieldComponent> textFieldComponent = AceType::DynamicCast<TextFieldComponent>(component);
    EXPECT_TRUE(textFieldComponent != nullptr);
    EXPECT_TRUE(!textFieldComponent->NeedObscure());
    EXPECT_TRUE(textFieldComponent->IsEnabled());
    EXPECT_TRUE(!textFieldComponent->GetAutoFocus());
    EXPECT_EQ(static_cast<uint32_t>(textFieldComponent->GetTextMaxLines()), TEXT_FIELD_ATTR_MAX_LINES_DEFAULT);
    EXPECT_EQ(static_cast<uint32_t>(textFieldComponent->GetMaxLength()), TEXT_FIELD_ATTR_MAX_LENGTH_DEFAULT);
    EXPECT_EQ(textFieldComponent->GetAction(), TEXT_FIELD_ATTR_ACTION_DEFAULT);
    EXPECT_EQ(textFieldComponent->GetActionLabel(), TEXT_FIELD_ATTR_ACTION_LABEL_DEFAULT);
    EXPECT_EQ(textFieldComponent->GetTextDirection(), TEXT_FIELD_ATTR_DIRECTION_DEFAULT);
    EXPECT_EQ(textFieldComponent->GetTextInputType(), TEXT_FIELD_ATTR_INPUT_TYPE_DEFAULT);
    EXPECT_EQ(textFieldComponent->GetTextAlign(), TEXT_FIELD_ATTR_ALIGN_DEFAULT);
    // placeholder
    EXPECT_EQ(textFieldComponent->GetPlaceholder(), TEXT_FIELD_ATTR_PLACEHOLDER_DEFAULT);
    EXPECT_EQ(textFieldComponent->GetPlaceholderColor().GetValue(), TEXT_FIELD_ATTR_COLOR_DEFAULT);
    // cursor color
    EXPECT_EQ(textFieldComponent->GetCursorColor().GetValue(), TEXT_FIELD_ATTR_COLOR_DEFAULT);
}

} // namespace OHOS::Ace