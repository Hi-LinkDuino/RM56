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
#include "core/components/common/properties/text_style.h"
#include "core/components/test/json/component_factory.h"
#include "core/components/text/text_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr double FONT_SIZE = 10.0;
constexpr double FONT_SIZE_DEFAULT = 14.0;
constexpr int32_t FONT_WEIGHT = 1;
constexpr int32_t FONT_WEIGHT_DEFAULT = 10;
constexpr int32_t FONT_STYLE = 1;
constexpr int32_t FONT_STYLE_DEFAULT = 0;
constexpr int32_t TEXT_BASE_LINE_JSON = 1;
constexpr int32_t TEXT_BASE_LINE_DEFAULT = 0;
constexpr int32_t TEXT_DECORATION_JSON = 1;
constexpr int32_t TEXT_DECORATION_DEFAULT = 0;
constexpr uint32_t COLOR_VALUE_DEFAULT = 4278190080;
constexpr Dimension WORD_SPACING = 10.0_px;
constexpr Dimension WORD_SPACING_DEFAULT = 0.0_px;
constexpr Dimension LETTER_SPACING = 10.0_px;
constexpr Dimension LETTER_SPACING_DEFAULT = 0.0_px;
constexpr double LINE_HEIGHT = 10.0;
constexpr double LINE_HEIGHT_DEFAULT = 0.0;
constexpr uint32_t TEXT_COLOR_VALUE = 255;
constexpr uint32_t TEXT_COLOR_ALPHA = 100;
constexpr uint32_t TEXT_COLOR_RED = 100;
constexpr uint32_t TEXT_COLOR_GREEN = 100;
constexpr uint32_t TEXT_COLOR_BLUE = 100;
constexpr double OFFSET_DX = 10.0;
constexpr double OFFSET_DY = 10.0;
constexpr double OFFSET_DX_DEFAULT = 0.0;
constexpr double OFFSET_DY_DEFAULT = 0.0;
constexpr double SHADOW_BLUR_RADIUS = 10.0;
constexpr double BLUR_RADIUS_DEFAULT = 0.0;
const std::vector<std::string> FONT_FAMILIES = { "normal", "sans-serif", "serif", "monospace" };
const std::string JSON_TEXT_STR = "{                                    "
                                  "  \"className\": \"Text\",           "
                                  "  \"textStyle\":                     "
                                  "{                                    "
                                  "  \"className\": \"TextStyle\",      "
                                  "  \"fontSize\": 10.0,                "
                                  "  \"fontWeight\": 1,                 "
                                  "  \"fontStyle\": 1,                  "
                                  "  \"textBaseline\": 1,               "
                                  "  \"textDecoration\": 1,             "
                                  "  \"wordSpacing\": 10.0,             "
                                  "  \"letterSpacing\": 10.0,           "
                                  "  \"lineHeight\": 10.0,              "
                                  "  \"color\":                         "
                                  "{                                    "
                                  "  \"className\": \"Color\",          "
                                  "  \"value\": 255                     "
                                  "},                                   "
                                  "  \"shadow\":                        "
                                  "{                                    "
                                  "  \"className\": \"Shadow\",         "
                                  "  \"blurRadius\": 10.0,              "
                                  "  \"offset\":                        "
                                  "{                                    "
                                  "  \"className\": \"Offset\",         "
                                  "\"dx\": 10.0,                        "
                                  "\"dy\": 10.0                         "
                                  "},                                   "
                                  "  \"color\":                         "
                                  "{                                    "
                                  "  \"className\": \"Color\",          "
                                  "  \"alpha\": 100,                    "
                                  "  \"red\": 100,                      "
                                  "  \"green\": 100,                    "
                                  "  \"blue\": 100                      "
                                  "}                                    "
                                  "},                                   "
                                  "  \"lineHeight\": 10.0,              "
                                  "  \"fontFamilies\":                  "
                                  "[                                    "
                                  "  \"normal\",                        "
                                  "  \"sans-serif\",                    "
                                  "  \"serif\",                         "
                                  "  \"monospace\"                      "
                                  "]                                    "
                                  "}"
                                  "}";

} // namespace

class TextStyleCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    RefPtr<TextComponent> CreateComponent(const std::string& textJson);
};

void TextStyleCreatorTest::SetUpTestCase() {}
void TextStyleCreatorTest::TearDownTestCase() {}
void TextStyleCreatorTest::SetUp() {}
void TextStyleCreatorTest::TearDown() {}

RefPtr<TextComponent> TextStyleCreatorTest::CreateComponent(const std::string& textJson)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(textJson.c_str());
    size_t dslLen = textJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    if (!component) {
        return nullptr;
    }
    return AceType::DynamicCast<TextComponent>(component);
}

/**
 * @tc.name: TextStyleCreator001
 * @tc.desc: Test all the properties of text style.
 * @tc.type: FUNC
 * @tc.require: AR000DAR00 AR000DAR01 AR000DAR02 AR000DAR03 AR000DAR04 AR000DAR05 AR000DAR06 AR000DBK1A AR000DBK1B
 *              AR000DBK1C
 * @tc.author: shanshurong
 */
HWTEST_F(TextStyleCreatorTest, TextStyleCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create component based on json string.
     */
    RefPtr<TextComponent> text = CreateComponent(JSON_TEXT_STR);
    /**
     * @tc.steps: step2. Check all the properties matched.
     * @tc.expected: step2. All the properties are matched.
     */
    EXPECT_TRUE(text != nullptr);
    const auto& textStyle = text->GetTextStyle();
    EXPECT_TRUE(NearEqual(textStyle.GetFontSize().Value(), FONT_SIZE));
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetFontWeight()), FONT_WEIGHT);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetFontStyle()), FONT_STYLE);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetTextBaseline()), TEXT_BASE_LINE_JSON);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetTextDecoration()), TEXT_DECORATION_JSON);
    EXPECT_EQ(textStyle.GetWordSpacing().Unit(), WORD_SPACING.Unit());
    EXPECT_TRUE(NearEqual(textStyle.GetWordSpacing().Value(), WORD_SPACING.Value()));
    EXPECT_EQ(textStyle.GetLetterSpacing().Unit(), LETTER_SPACING.Unit());
    EXPECT_TRUE(NearEqual(textStyle.GetLetterSpacing().Value(), LETTER_SPACING.Value()));
    EXPECT_TRUE(NearEqual(textStyle.GetLineHeight().Value(), LINE_HEIGHT));
    EXPECT_EQ(textStyle.GetTextColor().GetValue(), TEXT_COLOR_VALUE);

    const auto& textShadow = textStyle.GetShadow();
    EXPECT_TRUE(NearEqual(textShadow.GetOffset().GetX(), OFFSET_DX));
    EXPECT_TRUE(NearEqual(textShadow.GetOffset().GetY(), OFFSET_DY));
    EXPECT_TRUE(NearEqual(textShadow.GetBlurRadius(), SHADOW_BLUR_RADIUS));
    const auto& textShadowColor = textStyle.GetShadow().GetColor();
    EXPECT_EQ(textShadowColor.GetAlpha(), TEXT_COLOR_ALPHA);
    EXPECT_EQ(textShadowColor.GetRed(), TEXT_COLOR_RED);
    EXPECT_EQ(textShadowColor.GetGreen(), TEXT_COLOR_GREEN);
    EXPECT_EQ(textShadowColor.GetBlue(), TEXT_COLOR_BLUE);
    EXPECT_TRUE(textStyle.GetFontFamilies() == FONT_FAMILIES);
}

/**
 * @tc.name: TextStyleCreator002
 * @tc.desc: Test all the properties set by default value when value is invalid.
 * @tc.type: FUNC
 * @tc.require: AR000DAR01 AR000DAR02 AR000DAR03 AR000DAR04 AR000DAR05 AR000DAR06 AR000DBK1B AR000DBK1C
 * @tc.author: shanshurong
 */
HWTEST_F(TextStyleCreatorTest, TextStyleCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with all property.
     */
    const std::string jsonText = "{                                    "
                                 "  \"className\": \"Text\",           "
                                 "  \"textStyle\":                     "
                                 "{                                    "
                                 "  \"className\": \"TextStyle\",      "
                                 "  \"fontSize\": 10.0,                "
                                 "  \"fontWeight\": 10,                "
                                 "  \"fontStyle\": 10,                 "
                                 "  \"textBaseline\": 10,              "
                                 "  \"textDecoration\": 10,            "
                                 "  \"wordSpacing\": 10.0,             "
                                 "  \"letterSpacing\": 10.0,           "
                                 "  \"lineHeight\": 10.0,              "
                                 "  \"fontFamilies\":                  "
                                 "[                                    "
                                 "  \"normal\",                        "
                                 "  \"sans-serif\",                    "
                                 "  \"serif\",                         "
                                 "  \"monospace\"                      "
                                 "]                                    "
                                 "}"
                                 "}";
    /**
     * @tc.steps: step2. Check all the properties matched.
     * @tc.expected: step2. All the properties are matched.
     */
    RefPtr<TextComponent> text = CreateComponent(jsonText);
    EXPECT_TRUE(text != nullptr);
    const auto& textStyle = text->GetTextStyle();
    EXPECT_TRUE(NearEqual(textStyle.GetFontSize().Value(), FONT_SIZE));
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetFontWeight()), FONT_WEIGHT_DEFAULT);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetFontStyle()), FONT_STYLE_DEFAULT);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetTextBaseline()), TEXT_BASE_LINE_DEFAULT);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetTextDecoration()), TEXT_DECORATION_DEFAULT);
    EXPECT_EQ(textStyle.GetWordSpacing().Unit(), WORD_SPACING.Unit());
    EXPECT_TRUE(NearEqual(textStyle.GetWordSpacing().Value(), WORD_SPACING.Value()));
    EXPECT_EQ(textStyle.GetLetterSpacing().Unit(), LETTER_SPACING.Unit());
    EXPECT_TRUE(NearEqual(textStyle.GetLetterSpacing().Value(), LETTER_SPACING.Value()));
    EXPECT_TRUE(NearEqual(textStyle.GetLineHeight().Value(), LINE_HEIGHT));
    EXPECT_TRUE(textStyle.GetFontFamilies() == FONT_FAMILIES);
}

/**
 * @tc.name: TextStyleCreator003
 * @tc.desc: Test all the properties when initialized by default value.
 * @tc.type: FUNC
 * @tc.require: AR000DAR01 AR000DAR02 AR000DAR03 AR000DAR04 AR000DAR05 AR000DAR06 AR000DBK1A AR000DBK1B AR000DBK1C
 * @tc.author: shanshurong
 */
HWTEST_F(TextStyleCreatorTest, TextStyleCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with all property.
     */
    const std::string jsonText = "{                                "
                                 "  \"className\": \"Text\",      "
                                 "  \"textStyle\":                "
                                 "{                               "
                                 "  \"className\": \"TextStyle\"  "
                                 "}"
                                 "}";
    /**
     * @tc.steps: step2. Check all the properties matched.
     * @tc.expected: step2. All the properties are matched.
     */
    RefPtr<TextComponent> text = CreateComponent(jsonText);
    EXPECT_TRUE(text != nullptr);
    const auto& textStyle = text->GetTextStyle();
    EXPECT_TRUE(NearEqual(textStyle.GetFontSize().Value(), FONT_SIZE_DEFAULT));
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetFontWeight()), FONT_WEIGHT_DEFAULT);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetFontStyle()), FONT_STYLE_DEFAULT);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetTextBaseline()), TEXT_BASE_LINE_DEFAULT);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetTextDecoration()), TEXT_DECORATION_DEFAULT);
    EXPECT_EQ(textStyle.GetWordSpacing().Unit(), WORD_SPACING_DEFAULT.Unit());
    EXPECT_TRUE(NearEqual(textStyle.GetWordSpacing().Value(), WORD_SPACING_DEFAULT.Value()));
    EXPECT_EQ(textStyle.GetLetterSpacing().Unit(), LETTER_SPACING_DEFAULT.Unit());
    EXPECT_TRUE(NearEqual(textStyle.GetLetterSpacing().Value(), LETTER_SPACING_DEFAULT.Value()));
    EXPECT_TRUE(NearEqual(textStyle.GetLineHeight().Value(), LINE_HEIGHT_DEFAULT));

    const auto& textShadow = textStyle.GetShadow();
    const auto& textShadowColor = textShadow.GetColor().GetValue();
    Offset textShadowOffset = textShadow.GetOffset();
    double textShadowBlurRadius = textShadow.GetBlurRadius();
    EXPECT_EQ(static_cast<uint32_t>(textShadowColor), COLOR_VALUE_DEFAULT);
    EXPECT_TRUE(NearEqual(textShadowOffset.GetX(), OFFSET_DX_DEFAULT));
    EXPECT_TRUE(NearEqual(textShadowOffset.GetY(), OFFSET_DY_DEFAULT));
    EXPECT_TRUE(NearEqual(textShadowBlurRadius, BLUR_RADIUS_DEFAULT));
}

} // namespace OHOS::Ace