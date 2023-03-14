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

#include "base/i18n/localization.h"
#include "core/components/common/layout/constants.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/common/dom/dom_switch.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const std::string TEST_TEXT_ON = "Onn";
const std::string TEST_TEXT_OFF = "Offf";
const std::string TEST_TEXT_COLOR_ON = "#00FF00";
const std::string TEST_TEXT_COLOR_OFF = "#0000FF";
const std::string TEST_TEXT_FONT_FAMILY = "serif";
constexpr FontWeight TEST_TEXT_FONT_WEIGHT = FontWeight::W400;
constexpr FontStyle TEST_TEXT_FONT_STYLE = FontStyle::NORMAL;
constexpr TextDecoration TEST_TEXT_DECORATION = TextDecoration::NONE;
const double TEST_TEXT_PADDING = 12;
constexpr double TEST_TEXT_FONT_SIZE = 30.0;
constexpr Dimension TEST_TEXT_LETTER_SPACING = 10.0_px;
const std::string DEFAULT_TEXT_ON = "On";
const std::string DEFAULT_TEXT_OFF = "Off";
const std::string DEFAULT_TEXT_COLOR_ON = "#000000";
const std::string DEFAULT_TEXT_COLOR_OFF = "#000000";
constexpr FontWeight DEFAULT_TEXT_FONT_WEIGHT = FontWeight::NORMAL;
constexpr FontStyle DEFAULT_TEXT_FONT_STYLE = FontStyle::NORMAL;
constexpr TextDecoration DEFAULT_TEXT_DECORATION = TextDecoration::NONE;
const double DEFAULT_TEXT_PADDING = 0.0;
constexpr double DEFAULT_TEXT_FONT_SIZE = 14.0;
constexpr double INVALID_TEXT_FONT_SIZE = 0.0;
constexpr Dimension DEFAULT_TEXT_LETTER_SPACING = 0.0_px;
const std::string TEXT_SWITCH_JSON = "{                                                          "
                                     "  \"tag\": \"switch\",                                     "
                                     "  \"attr\": [{                                             "
                                     "                \"texton\" : \"Onn\"                       "
                                     "              },                                           "
                                     "              {                                            "
                                     "                \"textoff\" : \"Offf\"                     "
                                     "              },                                           "
                                     "              {                                            "
                                     "                \"showtext\" : \"true\"                    "
                                     "              }],                                          "
                                     "  \"style\": [{                                            "
                                     "           \"fontSize\":\"30.0\"                           "
                                     "          },                                               "
                                     "          {                                                "
                                     "           \"textPadding\":\"12.0\"                        "
                                     "          },                                               "
                                     "          {                                                "
                                     "           \"fontWeight\":\"400\"                          "
                                     "           },                                              "
                                     "          {                                                "
                                     "           \"textonColor\":\"#00ff00\"                     "
                                     "           },                                              "
                                     "          {                                                "
                                     "           \"textoffColor\":\"#0000ff\"                    "
                                     "           },                                              "
                                     "          {                                                "
                                     "           \"fontStyle\":\"normal\"                        "
                                     "           },                                              "
                                     "          {                                                "
                                     "           \"textDecoration\":\"none\"                     "
                                     "           },                                              "
                                     "          {                                                "
                                     "           \"letterSpacing\":\"10.0\"                      "
                                     "           },                                              "
                                     "           {                                               "
                                     "           \"fontFamily\":\"serif\"                        "
                                     "            }]                                             "
                                     "}                                                          ";
const std::string INVALID_TEXT_ATTR_SWITCH_JSON = "{                                                          "
                                                  "  \"tag\": \"switch\",                                     "
                                                  "  \"attr\": [{                                             "
                                                  "                \"showtext\" : \"1111\"                    "
                                                  "              }],                                          "
                                                  "  \"style\": [{                                            "
                                                  "           \"fontSize\":\"abcd\"                           "
                                                  "          },                                               "
                                                  "          {                                                "
                                                  "           \"textPadding\":\"abcd\"                        "
                                                  "          },                                               "
                                                  "          {                                                "
                                                  "           \"fontWeight\":\"abc\"                          "
                                                  "           },                                              "
                                                  "          {                                                "
                                                  "           \"textonColor\":\"wxyz\"                        "
                                                  "           },                                              "
                                                  "          {                                                "
                                                  "           \"textoffColor\":\"#wxyz\"                      "
                                                  "           },                                              "
                                                  "          {                                                "
                                                  "           \"fontStyle\":\"abcd\"                          "
                                                  "           },                                              "
                                                  "          {                                                "
                                                  "           \"textDecoration\":\"abcd\"                     "
                                                  "           },                                              "
                                                  "           {                                               "
                                                  "           \"letterSpacing\":\"abcd\"                      "
                                                  "            }]                                             "
                                                  "}                                                          ";
} // namespace

class DomSwitchTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomSwitchTest::SetUpTestCase() {}
void DomSwitchTest::TearDownTestCase() {}
void DomSwitchTest::SetUp() {}
void DomSwitchTest::TearDown() {}

/**
 * @tc.name: DomSwitchCreatorTest001
 * @tc.desc: Test switch node and child switch component are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomSwitchTest, DomSwitchCreatorTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create switch node with it.
     * @tc.expected: step1. Switch node and switch component are created successfully.
     */
    std::string switchDsl = "{                                                          "
                            "  \"tag\": \"switch\",                                     "
                            "  \"attr\": [{                                             "
                            "                \"checked\" : \"true\"                     "
                            "              },                                           "
                            "              {                                            "
                            "                \"disabled\" : \"true\"                    "
                            "              }],                                          "
                            "  \"event\": [ \"change\" ]                                "
                            "}                                                          ";
    Localization::GetInstance()->SetLocale("en", "US", "", "", "en-US");
    auto domSwitch = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(switchDsl);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domSwitch);
    RefPtr<SwitchComponent> switchComponent = AceType::DynamicCast<SwitchComponent>(boxChild->GetChild());
    EXPECT_TRUE(switchComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created switch node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_TRUE(switchComponent->GetValue());
    EXPECT_TRUE(switchComponent->IsDisabled());
    EXPECT_TRUE(switchComponent->GetChangeEvent() == std::to_string(domSwitch->GetNodeId()));
}

/**
 * @tc.name: DomSwitchCreatorTest002
 * @tc.desc: Test switch node and switch component created successfully with empty input of style and attribute.
 * @tc.type: FUNC
 */
HWTEST_F(DomSwitchTest, DomSwitchCreatorTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string without style and attribute, then create switch node with it .
     * @tc.expected: step1. Switch node and child switch component are created successfully.
     */
    std::string switchDsl = "{  \"tag\": \"switch\" }";
    Localization::GetInstance()->SetLocale("en", "US", "", "", "en-US");
    auto domSwitch = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(switchDsl);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domSwitch);
    RefPtr<SwitchComponent> switchComponent = AceType::DynamicCast<SwitchComponent>(boxChild->GetChild());
    EXPECT_TRUE(switchComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created switch node.
     * @tc.expected: step2. Switch node's attrs and styles are initialized with default value.
     */
    EXPECT_FALSE(switchComponent->GetValue());
    EXPECT_FALSE(switchComponent->IsDisabled());
}

/**
 * @tc.name: CreateTextSwitchFromDsl001
 * @tc.desc: Test switch node and switch component created successfully with text styles and attributes set.
 * @tc.type: FUNC
 */
HWTEST_F(DomSwitchTest, CreateTextSwitchFromDsl001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string text styles and attributes set, then create switch node with it .
     * @tc.expected: step1. Switch node and child switch component are created with correct text attributes and styles.
     */
    Localization::GetInstance()->SetLocale("en", "US", "", "", "en-US");
    auto domSwitch = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(TEXT_SWITCH_JSON);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domSwitch);
    RefPtr<SwitchComponent> switchComponent = AceType::DynamicCast<SwitchComponent>(boxChild->GetChild());
    EXPECT_TRUE(switchComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created switch node.
     * @tc.expected: step2. Switch node's attrs and styles are as expected.
     */
    EXPECT_TRUE(switchComponent->GetShowText());
    EXPECT_EQ(switchComponent->GetTextOn(), TEST_TEXT_ON);
    EXPECT_EQ(switchComponent->GetTextOff(), TEST_TEXT_OFF);
    EXPECT_EQ(switchComponent->GetTextPadding().Value(), TEST_TEXT_PADDING);
    EXPECT_TRUE(switchComponent->GetTextStyle().GetFontWeight() == TEST_TEXT_FONT_WEIGHT);
    EXPECT_TRUE(switchComponent->GetTextStyle().GetFontStyle() == TEST_TEXT_FONT_STYLE);
    EXPECT_TRUE(switchComponent->GetTextStyle().GetTextDecoration() == TEST_TEXT_DECORATION);
    EXPECT_EQ(switchComponent->GetTextStyle().GetFontSize().Value(), TEST_TEXT_FONT_SIZE);
    EXPECT_EQ(switchComponent->GetTextColorOn(), Color::FromString(TEST_TEXT_COLOR_ON));
    EXPECT_EQ(switchComponent->GetTextColorOff(), Color::FromString(TEST_TEXT_COLOR_OFF));
    EXPECT_TRUE(
        NearEqual(switchComponent->GetTextStyle().GetLetterSpacing().Value(), TEST_TEXT_LETTER_SPACING.Value()));
    EXPECT_EQ(switchComponent->GetTextStyle().GetFontFamilies()[0], TEST_TEXT_FONT_FAMILY);
}

/**
 * @tc.name: CreateTextSwitchFromDsl002
 * @tc.desc: Test switch node and switch component created correctly with no text styles and attributes set.
 * @tc.type: FUNC
 */
HWTEST_F(DomSwitchTest, CreateTextSwitchFromDsl002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with no text styles and attributes set, then create switch node with it .
     * @tc.expected: step1. Switch node and child switch component are created with default text attributes and styles.
     */
    std::string defaultTextSwitch = "{  \"tag\": \"switch\" }";
    Localization::GetInstance()->SetLocale("en", "US", "", "", "en-US");
    auto domSwitch = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(defaultTextSwitch);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domSwitch);
    RefPtr<SwitchComponent> switchComponent = AceType::DynamicCast<SwitchComponent>(boxChild->GetChild());
    EXPECT_TRUE(switchComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created switch node.
     * @tc.expected: step2. Switch node's attrs and styles are as default.
     */
    EXPECT_TRUE(!switchComponent->GetShowText());
    EXPECT_EQ(switchComponent->GetTextOn(), DEFAULT_TEXT_ON);
    EXPECT_EQ(switchComponent->GetTextOff(), DEFAULT_TEXT_OFF);
    EXPECT_EQ(switchComponent->GetTextPadding().Value(), DEFAULT_TEXT_PADDING);
    EXPECT_TRUE(switchComponent->GetTextStyle().GetFontWeight() == DEFAULT_TEXT_FONT_WEIGHT);
    EXPECT_TRUE(switchComponent->GetTextStyle().GetFontStyle() == DEFAULT_TEXT_FONT_STYLE);
    EXPECT_TRUE(switchComponent->GetTextStyle().GetTextDecoration() == DEFAULT_TEXT_DECORATION);
    EXPECT_EQ(switchComponent->GetTextStyle().GetFontSize().Value(), DEFAULT_TEXT_FONT_SIZE);
    EXPECT_EQ(switchComponent->GetTextColorOn(), Color::FromString(DEFAULT_TEXT_COLOR_ON));
    EXPECT_EQ(switchComponent->GetTextColorOff(), Color::FromString(DEFAULT_TEXT_COLOR_OFF));
    EXPECT_TRUE(
        NearEqual(switchComponent->GetTextStyle().GetLetterSpacing().Value(), DEFAULT_TEXT_LETTER_SPACING.Value()));
    EXPECT_TRUE(switchComponent->GetTextStyle().GetFontFamilies().empty());
}

/**
 * @tc.name: CreateTextSwitchFromDsl003
 * @tc.desc: Test switch node and switch component created correctly with invalid text styles and attributes set.
 * @tc.type: FUNC
 */
HWTEST_F(DomSwitchTest, CreateTextSwitchFromDsl003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with invalid text styles and attributes set, then create switch node with it .
     * @tc.expected: step1. Switch node and child switch component are created with default text attributes and styles.
     */
    Localization::GetInstance()->SetLocale("en", "US", "", "", "en-US");
    auto domSwitch = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(INVALID_TEXT_ATTR_SWITCH_JSON);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domSwitch);
    RefPtr<SwitchComponent> switchComponent = AceType::DynamicCast<SwitchComponent>(boxChild->GetChild());
    EXPECT_TRUE(switchComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created switch node.
     * @tc.expected: step2. Switch node's attrs and styles are as default.
     */
    EXPECT_TRUE(!switchComponent->GetShowText());
    EXPECT_EQ(switchComponent->GetTextOn(), DEFAULT_TEXT_ON);
    EXPECT_EQ(switchComponent->GetTextOff(), DEFAULT_TEXT_OFF);
    EXPECT_EQ(switchComponent->GetTextPadding().Value(), DEFAULT_TEXT_PADDING);
    EXPECT_TRUE(switchComponent->GetTextStyle().GetFontWeight() == DEFAULT_TEXT_FONT_WEIGHT);
    EXPECT_TRUE(switchComponent->GetTextStyle().GetFontStyle() == DEFAULT_TEXT_FONT_STYLE);
    EXPECT_TRUE(switchComponent->GetTextStyle().GetTextDecoration() == DEFAULT_TEXT_DECORATION);
    EXPECT_EQ(switchComponent->GetTextStyle().GetFontSize().Value(), INVALID_TEXT_FONT_SIZE);
    EXPECT_EQ(switchComponent->GetTextColorOn(), Color::FromString(DEFAULT_TEXT_COLOR_ON));
    EXPECT_EQ(switchComponent->GetTextColorOff(), Color::FromString(DEFAULT_TEXT_COLOR_OFF));
    EXPECT_TRUE(
        NearEqual(switchComponent->GetTextStyle().GetLetterSpacing().Value(), DEFAULT_TEXT_LETTER_SPACING.Value()));
}

/**
 * @tc.name: CreateTextSwitchFromDsl004
 * @tc.desc: Test switch set [showtext] without [texton]/[textoff] can display default texts..
 * @tc.type: FUNC
 */
HWTEST_F(DomSwitchTest, CreateTextSwitchFromDsl004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create switch node with it.
     * @tc.expected: step1. Switch node and switch component are created successfully.
     */
    std::string switchDsl = "{                                                          "
                            "  \"tag\": \"switch\",                                     "
                            "  \"attr\": [{                                             "
                            "                \"showtext\" : \"true\"                    "
                            "              }]                                           "
                            "}                                                          ";
    Localization::GetInstance()->SetLocale("en", "US", "", "", "en-US");
    auto domSwitch = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(switchDsl);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domSwitch);
    RefPtr<SwitchComponent> switchComponent = AceType::DynamicCast<SwitchComponent>(boxChild->GetChild());
    EXPECT_TRUE(switchComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created switch node.
     * @tc.expected: step2. Value of [texton]/[textoff] is as expected.
     */
    EXPECT_TRUE(switchComponent->GetShowText());
    EXPECT_EQ(switchComponent->GetTextOn(), DEFAULT_TEXT_ON);
    EXPECT_EQ(switchComponent->GetTextOff(), DEFAULT_TEXT_OFF);
}

} // namespace OHOS::Ace::Framework
