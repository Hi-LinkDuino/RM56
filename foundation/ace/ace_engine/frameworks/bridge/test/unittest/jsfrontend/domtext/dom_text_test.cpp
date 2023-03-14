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
#include "core/components/common/layout/constants.h"
#include "core/components/text/text_theme.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/dom/dom_div.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/common/dom/dom_text.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const std::string TEXT_DATA = "test1234@?!";
constexpr int32_t TEST_OVER_FLOW = 1;
constexpr double TEST_FONT_SIZE = 50.0;
constexpr int32_t TEST_FONT_WEIGHT = 1;
const std::string TEST_COLOR_VALUE = "#0x0000ff";
constexpr double TEST_LINE_HEIGHT = 11.0;
constexpr int32_t TEST_FONT_STYLE = 1;
constexpr int32_t TEST_TEXT_DECORATION = 2;
constexpr uint32_t TEST_MAX_LINE = 10;
constexpr Dimension TEST_LETTER_SPACING = 11.0_px;
constexpr int32_t TEST_TEXT_ALIGN = 1;
const std::string TEST_FONT_FAMILY = "serif";
constexpr int32_t DEFAULT_OVER_FLOW = 0;
constexpr int32_t DEFAULT_TEXT_ALIGN = 4;
constexpr int32_t DEFAULT_TEXT_ALIGN_INVALIDVALUE = 2;
const Dimension TEST_MIN_FONT_SIZE = Dimension(10.0);
const Dimension TEST_MAX_FONT_SIZE = Dimension(20.0);
const Dimension TEST_FONT_SIZE_STEP = Dimension(2.0);
constexpr uint32_t TEST_PREFER_FONT_SIZES_SIZE = 3;
constexpr uint32_t TEST_FONT_FEATURES_SIZE = 6;
constexpr uint32_t TEST_FONT_FEATURES_NONE_SIZE = 5;
const RefPtr<ThemeManager> THEME_MANAGER = AceType::MakeRefPtr<ThemeManager>();
const std::string JSON_TEXT_STR = ""
                                  "{                                                       "
                                  "  \"tag\": \"text\",                                    "
                                  "  \"attr\" : [{                                         "
                                  "           \"value\" : \"test1234@?!\"                "
                                  "            }],                                         "
                                  "  \"style\": [{                                         "
                                  "           \"textOverflow\":\"ellipsis\"                "
                                  "          },                                            "
                                  "          {                                             "
                                  "           \"fontSize\":\"50.0\"                        "
                                  "          },                                            "
                                  "          {                                             "
                                  "           \"fontWeight\":\"200\"                       "
                                  "           },                                           "
                                  "          {                                             "
                                  "           \"color\":\"#0x0000ff\"                      "
                                  "           },                                           "
                                  "          {                                             "
                                  "           \"lineHeight\":\"11.0\"                      "
                                  "           },                                           "
                                  "          {                                             "
                                  "           \"fontStyle\":\"italic\"                     "
                                  "           },                                           "
                                  "          {                                             "
                                  "           \"textDecoration\":\"overline\"              "
                                  "           },                                           "
                                  "           {                                            "
                                  "           \"maxLines\":\"10\"                          "
                                  "           },                                           "
                                  "          {                                             "
                                  "           \"letterSpacing\":\"11.0\"                   "
                                  "           },                                           "
                                  "          {                                             "
                                  "           \"minFontSize\":\"10.0px\"                   "
                                  "           },                                           "
                                  "          {                                             "
                                  "           \"maxFontSize\":\"20.0px\"                   "
                                  "           },                                           "
                                  "          {                                             "
                                  "           \"fontSizeStep\":\"2px\"                     "
                                  "           },                                           "
                                  "          {                                             "
                                  "           \"preferFontSizes\":\"20px, 18px, 16px\"     "
                                  "           },                                           "
                                  "           {                                            "
                                  "           \"textAlign\":\"right\"                      "
                                  "            },                                          "
                                  "           {                                                                  "
                                  "           \"fontVariant\":\"small-caps slashed-zero common-ligatures ruby    "
                                  "stylistic(2) \"                                                               "
                                  "            },                                          "
                                  "           {                                            "
                                  "           \"fontFamily\":\"serif\"                     "
                                  "            }]                                          "
                                  "}";

} // namespace

class DomTextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomTextTest::SetUpTestCase() {}
void DomTextTest::TearDownTestCase() {}
void DomTextTest::SetUp() {}
void DomTextTest::TearDown() {}

/**
 * @tc.name: DomTextTest001
 * @tc.desc: Verify that DomText can be created.
 * @tc.type: FUNC
 */
HWTEST_F(DomTextTest, DomTextTest001, TestSize.Level1)
{
    const std::string jsonTextStr = ""
                                    "{                                 "
                                    "  \"tag\": \"text\"               "
                                    "}";

    /**
     * @tc.steps: step1. call JsonUtil interface and create DomText.
     */
    auto domNodeText = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonTextStr);

    /**
     * @tc.steps: step2. Verify whether the DomText.
     * @tc.expected: step2. DomText is not null.
     */
    ASSERT_TRUE(domNodeText != nullptr);
}

/**
 * @tc.name: DomTextTest002
 * @tc.desc: Verify that DomText can be set styles.
 * @tc.type: FUNC
 */
HWTEST_F(DomTextTest, DomTextTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of DomText with all attributes.
     */
    const std::string& jsonTextStr = JSON_TEXT_STR;

    /**
     * @tc.steps: step2. call JsonUtil interface, create DomText and set its style.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonTextStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<TextComponent> textChild = AceType::DynamicCast<TextComponent>(boxChild->GetChild());
    const auto textStyle = textChild->GetTextStyle();

    /**
     * @tc.steps: step3. Check all the attributes matched.
     * @tc.expected: step3. All the attributes are matched.
     */
    EXPECT_EQ(textChild->GetData(), TEXT_DATA);
    EXPECT_TRUE(static_cast<int32_t>(textChild->GetTextStyle().GetTextOverflow()) == TEST_OVER_FLOW);
    EXPECT_TRUE(NearEqual(textStyle.GetFontSize().Value(), TEST_FONT_SIZE));
    EXPECT_EQ(static_cast<int32_t>(textStyle.GetFontWeight()), TEST_FONT_WEIGHT);
    EXPECT_EQ(textStyle.GetTextColor(), Color::FromString(TEST_COLOR_VALUE));
    EXPECT_TRUE(NearEqual(textStyle.GetLineHeight().Value(), TEST_LINE_HEIGHT));
    EXPECT_TRUE(NearEqual(static_cast<int32_t>(textStyle.GetFontStyle()), TEST_FONT_STYLE));
    EXPECT_TRUE(NearEqual(static_cast<int32_t>(textStyle.GetTextDecoration()), TEST_TEXT_DECORATION));
    EXPECT_TRUE(NearEqual(textChild->GetTextStyle().GetMaxLines(), TEST_MAX_LINE));
    EXPECT_EQ(textStyle.GetLetterSpacing().Unit(), TEST_LETTER_SPACING.Unit());
    EXPECT_TRUE(NearEqual(textStyle.GetLetterSpacing().Value(), TEST_LETTER_SPACING.Value()));
    EXPECT_EQ(static_cast<int32_t>(textChild->GetTextStyle().GetTextAlign()), TEST_TEXT_ALIGN);
    EXPECT_EQ(textStyle.GetFontFamilies()[0], TEST_FONT_FAMILY);
    EXPECT_EQ(textStyle.GetMaxLines(), TEST_MAX_LINE);
    EXPECT_EQ(textStyle.GetAdaptMinFontSize(), TEST_MIN_FONT_SIZE);
    EXPECT_EQ(textStyle.GetAdaptMaxFontSize(), TEST_MAX_FONT_SIZE);
    EXPECT_EQ(textStyle.GetAdaptFontSizeStep(), TEST_FONT_SIZE_STEP);
    EXPECT_EQ(textStyle.GetPreferFontSizes().size(), TEST_PREFER_FONT_SIZES_SIZE);
    EXPECT_EQ(textStyle.GetFontFeatures().size(), TEST_FONT_FEATURES_SIZE);
}

/**
 * @tc.name: DomTextTest003
 * @tc.desc: Verify that DomText can be set styles to the default values when styles' attributes are invalid.
 * @tc.type: FUNC
 */
HWTEST_F(DomTextTest, DomTextTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of DomText with invalid attributes.
     */
    const std::string jsonTextStr = ""
                                    "{                                                 "
                                    "  \"tag\": \"text\",                              "
                                    "  \"style\": [{                                   "
                                    "           \"textOverflow\":\"invalidValue\"      "
                                    "          },                                      "
                                    "          {                                       "
                                    "           \"fontWeight\":\"invalidValue\"        "
                                    "           },                                     "
                                    "          {                                       "
                                    "           \"fontStyle\":\"invalidValue\"         "
                                    "           },                                     "
                                    "          {                                       "
                                    "           \"textDecoration\":\"invalidValue\"    "
                                    "           },                                     "
                                    "          {                                       "
                                    "           \"fontVariant\":\"invalidValue\"       "
                                    "           },                                     "
                                    "           {                                      "
                                    "           \"textAlign\":\"invalidValue\"         "
                                    "            }]                                    "
                                    "}";

    /**
     * @tc.steps: step2. call JsonUtil interface, create DomText and set attributes' value.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonTextStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<TextComponent> textChild = AceType::DynamicCast<TextComponent>(boxChild->GetChild());
    const auto textStyle = textChild->GetTextStyle();

    /**
     * @tc.steps: step3. Check all the attributes matched with default values.
     * @tc.expected: step3. All the attributes are matched.
     */
    RefPtr<TextTheme> theme = THEME_MANAGER->GetTheme<TextTheme>();
    ASSERT_TRUE(theme);
    EXPECT_EQ(static_cast<int32_t>(textChild->GetTextStyle().GetTextOverflow()), DEFAULT_OVER_FLOW);
    EXPECT_EQ(textStyle.GetFontWeight(), theme->GetTextStyle().GetFontWeight());
    EXPECT_EQ(textStyle.GetFontStyle(), theme->GetTextStyle().GetFontStyle());
    EXPECT_EQ(static_cast<int32_t>(textChild->GetTextStyle().GetTextAlign()), DEFAULT_TEXT_ALIGN_INVALIDVALUE);
    EXPECT_EQ(textStyle.GetTextDecoration(), theme->GetTextStyle().GetTextDecoration());
    EXPECT_TRUE(textChild->GetTextStyle().GetFontFeatures().empty());
}

/**
 * @tc.name: DomTextTest004
 * @tc.desc: Verify that DomText's style can be initialized to default values when no styles are set manually.
 * @tc.type: FUNC
 */
HWTEST_F(DomTextTest, DomTextTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of DomText with all attributes.
     */
    const std::string jsonTextStr = ""
                                    "{                                            "
                                    "  \"tag\": \"text\"                          "
                                    "}";

    /**
     * @tc.steps: step2. call JsonUtil interface, create DomText and set its style.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonTextStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<TextComponent> textChild = AceType::DynamicCast<TextComponent>(boxChild->GetChild());
    const auto textStyle = textChild->GetTextStyle();

    /**
     * @tc.steps: step3. Check all the attributes matched with the default values.
     * @tc.expected: step3. All the attributes are matched.
     */
    RefPtr<TextTheme> theme = THEME_MANAGER->GetTheme<TextTheme>();
    ASSERT_TRUE(theme);
    EXPECT_TRUE(static_cast<int32_t>(textChild->GetTextStyle().GetTextOverflow()) == DEFAULT_OVER_FLOW);
    EXPECT_EQ(textStyle.GetFontSize().Unit(), theme->GetTextStyle().GetFontSize().Unit());
    EXPECT_TRUE(NearEqual(textStyle.GetFontSize().Value(), theme->GetTextStyle().GetFontSize().Value()));
    EXPECT_EQ(textStyle.GetFontWeight(), theme->GetTextStyle().GetFontWeight());
    EXPECT_EQ(textStyle.GetTextColor().GetValue(), theme->GetTextStyle().GetTextColor().GetValue());
    EXPECT_EQ(textStyle.GetLineHeight(), theme->GetTextStyle().GetLineHeight());
    EXPECT_EQ(textStyle.GetFontStyle(), theme->GetTextStyle().GetFontStyle());
    EXPECT_EQ(textStyle.GetTextDecoration(), theme->GetTextStyle().GetTextDecoration());
    EXPECT_EQ(textStyle.GetLetterSpacing().Unit(), theme->GetTextStyle().GetLetterSpacing().Unit());
    EXPECT_TRUE(NearEqual(textStyle.GetLetterSpacing().Value(), theme->GetTextStyle().GetLetterSpacing().Value()));
    EXPECT_EQ(static_cast<int32_t>(textChild->GetTextStyle().GetTextAlign()), DEFAULT_TEXT_ALIGN);
    EXPECT_TRUE(textStyle.GetFontFeatures().empty());
}

/**
 * @tc.name: DomTextTest005
 * @tc.desc: Verify that DomText can be created.
 * @tc.type: FUNC
 */
HWTEST_F(DomTextTest, DomTextTest005, TestSize.Level1)
{
    const std::string jsonTextStr = ""
                                    "{                                          "
                                    "  \"tag\": \"text\",                       "
                                    "  \"style\": [{                            "
                                    "               \"fontVariant\":\"normal\"  "
                                    "            }]                             "
                                    "}";

    /**
     * @tc.steps: step1. call JsonUtil interface and create DomText.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonTextStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<TextComponent> textChild = AceType::DynamicCast<TextComponent>(boxChild->GetChild());
    const auto textStyle = textChild->GetTextStyle();

    /**
     * @tc.steps: step3. Check font features is correct.
     * @tc.expected: step3. Font featrues is empty.
     */
    EXPECT_TRUE(textStyle.GetFontFeatures().empty());
}

/**
 * @tc.name: DomTextTest006
 * @tc.desc: Verify that DomText can be created.
 * @tc.type: FUNC
 */
HWTEST_F(DomTextTest, DomTextTest006, TestSize.Level1)
{
    const std::string jsonTextStr = ""
                                    "{                                     "
                                    "  \"tag\": \"text\",                  "
                                    "  \"style\": [{                       "
                                    "           \"fontVariant\":\"none\"   "
                                    "            }]                        "
                                    "}";

    /**
     * @tc.steps: step1. call JsonUtil interface and create DomText.
     */
    auto domNodeText = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonTextStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeText);
    RefPtr<TextComponent> textChild = AceType::DynamicCast<TextComponent>(boxChild->GetChild());
    const auto textStyle = textChild->GetTextStyle();

    /**
     * @tc.steps: step3. Check font features is correct.
     * @tc.expected: step3. Font feature is correct.
     */
    EXPECT_EQ(textStyle.GetFontFeatures().size(), TEST_FONT_FEATURES_NONE_SIZE);
}

} // namespace OHOS::Ace::Framework
