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
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/shadow.h"
#include "core/components/common/properties/text_style.h"
#include "core/components/test/json/component_factory.h"
#include "core/components/text_field/text_field_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr double TEXT_FIELD_STYLE_FONT_SIZE = 10.0;
constexpr double TEXT_FIELD_STYLE_FONT_SIZE_DEFAULT = 14.0;
constexpr int32_t TEXT_FIELD_STYLE_FONT_WEIGHT = 1;
constexpr int32_t TEXT_FIELD_STYLE_FONT_WEIGHT_DEFAULT = 10;
constexpr int32_t TEXT_FIELD_STYLE_FONT_STYLE = 1;
constexpr int32_t TEXT_FIELD_STYLE_FONT_STYLE_DEFAULT = 0;
constexpr int32_t TEXT_FIELD_STYLE_TEXT_BASE_LINE = 1;
constexpr int32_t TEXT_FIELD_STYLE_TEXT_BASE_LINE_DEFAULT = 0;
constexpr int32_t TEXT_FIELD_STYLE_TEXT_DECORATION = 1;
constexpr int32_t TEXT_FIELD_STYLE_TEXT_DECORATION_DEFAULT = 0;
constexpr uint32_t TEXT_FIELD_STYLE_COLOR_VALUE_DEFAULT = 4278190080;
constexpr Dimension TEXT_FIELD_STYLE_WORD_SPACING = 10.0_px;
constexpr Dimension TEXT_FIELD_STYLE_WORD_SPACING_DEFAULT = 0.0_px;
constexpr Dimension TEXT_FIELD_STYLE_LETTER_SPACING = 10.0_px;
constexpr Dimension TEXT_FIELD_STYLE_LETTER_SPACING_DEFAULT = 0.0_px;
constexpr double TEXT_FIELD_STYLE_LINE_HEIGHT = 12.0;
constexpr double TEXT_FIELD_STYLE_LINE_HEIGHT_DEFAULT = 0.0;
constexpr uint32_t TEXT_FIELD_STYLE_TEXT_COLOR_VALUE = 255;
constexpr uint32_t TEXT_FIELD_STYLE_TEXT_COLOR_ALPHA = 100;
constexpr uint32_t TEXT_FIELD_STYLE_TEXT_COLOR_RED = 100;
constexpr uint32_t TEXT_FIELD_STYLE_TEXT_COLOR_GREEN = 100;
constexpr uint32_t TEXT_FIELD_STYLE_TEXT_COLOR_BLUE = 100;
constexpr double TEXT_FIELD_STYLE_OFFSET_DX = 10.0;
constexpr double TEXT_FIELD_STYLE_OFFSET_DY = 10.0;
constexpr double TEXT_FIELD_STYLE_BLURRADIUS = 10.0;
const std::vector<std::string> TEXT_FIELD_STYLE_FONT_FAMILIES = { "normal", "sans-serif", "serif", "monospace" };

} // namespace

class TextFieldStyleCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void TextFieldStyleCreatorTest::SetUpTestCase() {}
void TextFieldStyleCreatorTest::TearDownTestCase() {}
void TextFieldStyleCreatorTest::SetUp() {}
void TextFieldStyleCreatorTest::TearDown() {}

/**
 * @tc.name: TextFieldStyleCreator001
 * @tc.desc: Test all the properties of text filed style.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7U
 * @tc.author: hushilong
 */
HWTEST_F(TextFieldStyleCreatorTest, TextFieldStyleCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with all property.
     */
    const std::string jsonTextFieldStr = "{"
                                         "  \"className\": \"TextField\",        "
                                         "  \"textStyle\":                       "
                                         "  {                                    "
                                         "    \"className\": \"TextStyle\",      "
                                         "    \"fontSize\": 10.0,                "
                                         "    \"fontWeight\": 1,                 "
                                         "    \"fontStyle\": 1,                  "
                                         "    \"textBaseline\": 1,               "
                                         "    \"textDecoration\": 1,             "
                                         "    \"wordSpacing\": 10.0,             "
                                         "    \"letterSpacing\": 10.0,           "
                                         "    \"lineHeight\": 12.0,              "
                                         "    \"color\":                         "
                                         "    {                                  "
                                         "      \"className\": \"Color\",        "
                                         "      \"value\": 255                   "
                                         "    },                                 "
                                         "    \"shadow\":                        "
                                         "    {                                  "
                                         "      \"className\": \"Shadow\",       "
                                         "      \"blurRadius\": 10.0,            "
                                         "      \"offset\":                      "
                                         "      {                                "
                                         "        \"className\": \"Offset\",     "
                                         "        \"dx\": 10.0,                  "
                                         "        \"dy\": 10.0                   "
                                         "      },                               "
                                         "      \"color\":                       "
                                         "      {                                "
                                         "        \"className\": \"Color\",      "
                                         "        \"alpha\": 100,                "
                                         "        \"red\": 100,                  "
                                         "        \"green\": 100,                "
                                         "        \"blue\": 100                  "
                                         "      }                                "
                                         "    },                                 "
                                         "    \"fontFamilies\":                  "
                                         "    [                                  "
                                         "      \"normal\",                      "
                                         "      \"sans-serif\",                  "
                                         "      \"serif\",                       "
                                         "      \"monospace\"                    "
                                         "    ]                                  "
                                         "  }"
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
    const auto& textStyle = textFieldComponent->GetTextStyle();
    EXPECT_TRUE(NearEqual(textStyle.GetFontSize().Value(), TEXT_FIELD_STYLE_FONT_SIZE));
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetFontWeight()), TEXT_FIELD_STYLE_FONT_WEIGHT);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetFontStyle()), TEXT_FIELD_STYLE_FONT_STYLE);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetTextBaseline()), TEXT_FIELD_STYLE_TEXT_BASE_LINE);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetTextDecoration()), TEXT_FIELD_STYLE_TEXT_DECORATION);
    EXPECT_EQ(textStyle.GetWordSpacing().Unit(), TEXT_FIELD_STYLE_WORD_SPACING.Unit());
    EXPECT_TRUE(NearEqual(textStyle.GetWordSpacing().Value(), TEXT_FIELD_STYLE_WORD_SPACING.Value()));
    EXPECT_EQ(textStyle.GetLetterSpacing().Unit(), TEXT_FIELD_STYLE_LETTER_SPACING.Unit());
    EXPECT_TRUE(NearEqual(textStyle.GetLetterSpacing().Value(), TEXT_FIELD_STYLE_LETTER_SPACING.Value()));
    EXPECT_TRUE(NearEqual(textStyle.GetLineHeight().Value(), TEXT_FIELD_STYLE_LINE_HEIGHT));
    EXPECT_EQ(textStyle.GetTextColor().GetValue(), TEXT_FIELD_STYLE_TEXT_COLOR_VALUE);
    EXPECT_EQ(textStyle.GetFontFamilies(), TEXT_FIELD_STYLE_FONT_FAMILIES);
    // shadow
    const auto& textShadow = textStyle.GetShadow();
    EXPECT_TRUE(NearEqual(textShadow.GetOffset().GetX(), TEXT_FIELD_STYLE_OFFSET_DX));
    EXPECT_TRUE(NearEqual(textShadow.GetOffset().GetY(), TEXT_FIELD_STYLE_OFFSET_DY));
    EXPECT_TRUE(NearEqual(textShadow.GetBlurRadius(), TEXT_FIELD_STYLE_BLURRADIUS));
    // color
    const auto& textShadowColor = textStyle.GetShadow().GetColor();
    EXPECT_EQ(textShadowColor.GetAlpha(), TEXT_FIELD_STYLE_TEXT_COLOR_ALPHA);
    EXPECT_EQ(textShadowColor.GetRed(), TEXT_FIELD_STYLE_TEXT_COLOR_RED);
    EXPECT_EQ(textShadowColor.GetGreen(), TEXT_FIELD_STYLE_TEXT_COLOR_GREEN);
    EXPECT_EQ(textShadowColor.GetBlue(), TEXT_FIELD_STYLE_TEXT_COLOR_BLUE);
}

/**
 * @tc.name: TextFieldStyleCreator002
 * @tc.desc: Test all the properties when initialized by default value.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7U
 * @tc.author: hushilong
 */
HWTEST_F(TextFieldStyleCreatorTest, TextFieldStyleCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with default.
     */
    const std::string jsonTextFieldStr = "{"
                                         "  \"className\": \"TextField\",   "
                                         "  \"textStyle\":                  "
                                         "  {                               "
                                         "    \"className\": \"TextStyle\"  "
                                         "  }"
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
    const auto& textStyle = textFieldComponent->GetTextStyle();
    EXPECT_TRUE(NearEqual(textStyle.GetFontSize().Value(), TEXT_FIELD_STYLE_FONT_SIZE_DEFAULT));
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetFontWeight()), TEXT_FIELD_STYLE_FONT_WEIGHT_DEFAULT);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetFontStyle()), TEXT_FIELD_STYLE_FONT_STYLE_DEFAULT);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetTextBaseline()), TEXT_FIELD_STYLE_TEXT_BASE_LINE_DEFAULT);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetTextDecoration()), TEXT_FIELD_STYLE_TEXT_DECORATION_DEFAULT);
    EXPECT_EQ(textStyle.GetWordSpacing().Unit(), TEXT_FIELD_STYLE_WORD_SPACING_DEFAULT.Unit());
    EXPECT_TRUE(NearEqual(textStyle.GetWordSpacing().Value(), TEXT_FIELD_STYLE_WORD_SPACING_DEFAULT.Value()));
    EXPECT_EQ(textStyle.GetLetterSpacing().Unit(), TEXT_FIELD_STYLE_LETTER_SPACING_DEFAULT.Unit());
    EXPECT_TRUE(NearEqual(textStyle.GetLetterSpacing().Value(), TEXT_FIELD_STYLE_LETTER_SPACING_DEFAULT.Value()));
    EXPECT_TRUE(NearEqual(textStyle.GetLineHeight().Value(), TEXT_FIELD_STYLE_LINE_HEIGHT_DEFAULT));
    const auto& textShadowColor = textStyle.GetShadow().GetColor().GetValue();
    EXPECT_EQ(static_cast<uint32_t>(textShadowColor), TEXT_FIELD_STYLE_COLOR_VALUE_DEFAULT);
}

} // namespace OHOS::Ace