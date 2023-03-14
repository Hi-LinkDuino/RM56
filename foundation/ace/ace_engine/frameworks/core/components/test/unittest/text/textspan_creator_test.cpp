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
#include "core/components/text_span/text_span_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const std::string SPAN_DATA = "test1234@?!";
constexpr double FONT_SIZE = 10.0;
constexpr int32_t FONT_WEIGHT = 1;
constexpr int32_t FONT_STYLE = 1;
constexpr int32_t TEXT_BASE_LINE_JSON = 1;
constexpr int32_t TEXT_DECORATION_JSON = 1;
constexpr double WORD_SPACING = 10.0;
constexpr double LETTER_SPACING = 10.0;
constexpr double LINE_HEIGHT = 10.0;
constexpr uint32_t TEXT_COLOR_VALUE = 255;
constexpr uint32_t TEXT_COLOR_ALPHA = 100;
constexpr uint32_t TEXT_COLOR_RED = 100;
constexpr uint32_t TEXT_COLOR_GREEN = 100;
constexpr uint32_t TEXT_COLOR_BLUE = 100;
constexpr double OFFSET_DX = 10.0;
constexpr double OFFSET_DY = 10.0;
constexpr double BLURRADIUS = 10.0;
const std::vector<std::string> FONT_FAMILIES = { "normal", "sans-serif", "serif", "monospace" };
const std::string JSON_SPAN_STR = "{                                    "
                                  "  \"className\": \"TextSpan\",       "
                                  "  \"value\": \"test1234@?!\",      "
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
const std::string JSON_NESTED_SPAN_STR = "{                                    "
                                         "  \"className\": \"TextSpan\",       "
                                         "  \"child\":                         "
                                         "{                                    "
                                         "  \"className\": \"TextSpan\",       "
                                         "  \"value\": \"test1234@?!\",      "
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
                                         "}                                    "
                                         "}                                    "
                                         "}";

} // namespace

class TextSpanCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    RefPtr<TextSpanComponent> CreateComponent(const std::string& spanJson);
};

void TextSpanCreatorTest::SetUpTestCase() {}
void TextSpanCreatorTest::TearDownTestCase() {}
void TextSpanCreatorTest::SetUp() {}
void TextSpanCreatorTest::TearDown() {}

RefPtr<TextSpanComponent> TextSpanCreatorTest::CreateComponent(const std::string& spanJson)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(spanJson.c_str());
    size_t dslLen = spanJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    if (!component) {
        return nullptr;
    }
    return AceType::DynamicCast<TextSpanComponent>(component);
}

/**
 * @tc.name: TextSpanCreator001
 * @tc.desc: Test all the properties of text span.
 * @tc.type: FUNC
 * @tc.require: AR000DBAOK AR000DAR00 AR000DAR01 AR000DAR02 AR000DAR03 AR000DAR04 AR000DAR05 AR000DAR06 AR000DBK1A
 *              AR000DBK1B AR000DBK1C AR000DAQVR
 * @tc.author: shanshurong
 */
HWTEST_F(TextSpanCreatorTest, TextSpanCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create component based on json string.
     */
    RefPtr<TextSpanComponent> span = CreateComponent(JSON_SPAN_STR);
    /**
     * @tc.steps: step2. Check all the properties matched.
     * @tc.expected: step2. All the properties are matched.
     */
    EXPECT_TRUE(span != nullptr);
    const auto& textStyle = span->GetTextStyle();
    EXPECT_TRUE(NearEqual(textStyle.GetFontSize().Value(), FONT_SIZE));
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetFontWeight()), FONT_WEIGHT);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetFontStyle()), FONT_STYLE);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetTextBaseline()), TEXT_BASE_LINE_JSON);
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetTextDecoration()), TEXT_DECORATION_JSON);
    EXPECT_TRUE(NearEqual(textStyle.GetWordSpacing().Value(), WORD_SPACING));
    EXPECT_TRUE(NearEqual(textStyle.GetLetterSpacing().Value(), LETTER_SPACING));
    EXPECT_TRUE(NearEqual(textStyle.GetLineHeight().Value(), LINE_HEIGHT));
    EXPECT_EQ(textStyle.GetTextColor().GetValue(), TEXT_COLOR_VALUE);

    const auto& spanShadow = textStyle.GetShadow();
    EXPECT_TRUE(NearEqual(spanShadow.GetOffset().GetX(), OFFSET_DX));
    EXPECT_TRUE(NearEqual(spanShadow.GetOffset().GetY(), OFFSET_DY));
    EXPECT_TRUE(NearEqual(spanShadow.GetBlurRadius(), BLURRADIUS));
    const auto& spanShadowColor = textStyle.GetShadow().GetColor();
    EXPECT_EQ(spanShadowColor.GetAlpha(), TEXT_COLOR_ALPHA);
    EXPECT_EQ(spanShadowColor.GetRed(), TEXT_COLOR_RED);
    EXPECT_EQ(spanShadowColor.GetGreen(), TEXT_COLOR_GREEN);
    EXPECT_EQ(spanShadowColor.GetBlue(), TEXT_COLOR_BLUE);
    EXPECT_TRUE(textStyle.GetFontFamilies() == FONT_FAMILIES);
}

/**
 * @tc.name: TextSpanCreator002
 * @tc.desc: Test all the properties of nested text span.
 * @tc.type: FUNC
 * @tc.require: AR000DBAOK AR000DAR00 AR000DAR01 AR000DAR02 AR000DAR03 AR000DAR04 AR000DAR05 AR000DAR06 AR000DBK1A
 *              AR000DBK1B AR000DBK1C AR000DAQVR
 * @tc.author: shanshurong
 */
HWTEST_F(TextSpanCreatorTest, TextSpanCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create child component based on json string.
     */
    JsonComponentFactory* factory = static_cast<JsonComponentFactory*>(&(ComponentFactory::GetInstance()));
    const auto& parseJson = JsonUtil::ParseJsonString(JSON_NESTED_SPAN_STR);
    ASSERT_FALSE(!parseJson);
    std::list<RefPtr<Component>> spanChild = factory->CreateChildComponent(*parseJson);
    /**
     * @tc.steps: step2. Check all the properties matched.
     * @tc.expected: step2. All the properties are matched.
     */
    for (const auto& child : spanChild) {
        RefPtr<TextSpanComponent> span = AceType::DynamicCast<TextSpanComponent>(child);
        EXPECT_TRUE(span != nullptr);

        const auto& textStyle = span->GetTextStyle();
        EXPECT_TRUE(NearEqual(textStyle.GetFontSize().Value(), FONT_SIZE));
        EXPECT_EQ(static_cast<int32_t>(textStyle.GetFontWeight()), FONT_WEIGHT);
        EXPECT_EQ(static_cast<int32_t>(textStyle.GetFontStyle()), FONT_STYLE);
        EXPECT_EQ(static_cast<int32_t>(textStyle.GetTextBaseline()), TEXT_BASE_LINE_JSON);
        EXPECT_EQ(static_cast<int32_t>(textStyle.GetTextDecoration()), TEXT_DECORATION_JSON);
        EXPECT_TRUE(NearEqual(textStyle.GetWordSpacing().Value(), WORD_SPACING));
        EXPECT_TRUE(NearEqual(textStyle.GetLetterSpacing().Value(), LETTER_SPACING));
        EXPECT_TRUE(NearEqual(textStyle.GetLineHeight().Value(), LINE_HEIGHT));
        EXPECT_EQ(textStyle.GetTextColor().GetValue(), TEXT_COLOR_VALUE);

        const auto& spanShadow = textStyle.GetShadow();
        EXPECT_TRUE(NearEqual(spanShadow.GetOffset().GetX(), OFFSET_DX));
        EXPECT_TRUE(NearEqual(spanShadow.GetOffset().GetY(), OFFSET_DY));
        EXPECT_TRUE(NearEqual(spanShadow.GetBlurRadius(), BLURRADIUS));
        const auto& spanShadowColor = textStyle.GetShadow().GetColor();
        EXPECT_EQ(spanShadowColor.GetAlpha(), TEXT_COLOR_ALPHA);
        EXPECT_EQ(spanShadowColor.GetRed(), TEXT_COLOR_RED);
        EXPECT_EQ(spanShadowColor.GetGreen(), TEXT_COLOR_GREEN);
        EXPECT_EQ(spanShadowColor.GetBlue(), TEXT_COLOR_BLUE);
        EXPECT_TRUE(textStyle.GetFontFamilies() == FONT_FAMILIES);
    }
}

} // namespace OHOS::Ace