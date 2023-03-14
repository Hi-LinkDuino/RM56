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

#include "base/geometry/dimension.h"
#include "core/components/button/button_component.h"
#include "core/components/button/button_theme.h"
#include "core/components/checkable/checkable_component.h"
#include "core/components/checkable/checkable_theme.h"
#include "core/components/common/layout/constants.h"
#include "core/components/text_field/text_field_component.h"
#include "core/components/text_field/textfield_theme.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/common/dom/dom_input.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const bool INPUT_CHECKED_VALUE_DEFAULT = false;

#ifndef WEARABLE_PRODUCT
constexpr uint32_t INPUT_MAXLENGTH_VALUE_DEFAULT = std::numeric_limits<uint32_t>::max();
const FontWeight INPUT_FONT_WEIGHT_VALUE_DEFAULT = FontWeight::W400;
const std::string INPUT_FONT_FAMILY_VALUE_DEFAULT = "sans-serif";

const std::string INPUT_PLACEHOLDER_VALUE = "please input";
const std::string INPUT_PLACEHOLDER_COLOR_STR_VALUE = "#12345678";
const std::string INPUT_CURCOR_COLOR_STR_VALUE = "#12345678";
constexpr uint32_t INPUT_MAXLENGTH_VALUE = 100;
const TextInputAction INPUT_TEXTINPUTACTION_VALUE = TextInputAction::NEXT;
const std::string INPUT_HEADER_ICON = "test.png";
const std::string INPUT_COLOR_STR_VALUE = "#12345678";
const Dimension INPUT_FONT_SIZE_VALUE = Dimension(30.0, DimensionUnit::PX);
const FontWeight INPUT_FONT_WEIGHT_VALUE = FontWeight::W600;
const std::string INPUT_FONT_FAMILY_VALUE = "serif";
#endif

const bool INPUT_CHECKED_VALUE = true;
const std::string INPUT_NAME_VALUE = "input name";
const std::string INPUT_VALUE_VALUE = "input value";
const Dimension INPUT_WIDTH_VALUE = Dimension(100, DimensionUnit::PX);
const Dimension INPUT_HEIGHT_VALUE = Dimension(50, DimensionUnit::PX);
const Dimension BUTTON_WIDTH_INIT = Dimension(-1.0, DimensionUnit::PX);
const RefPtr<ThemeManager> THEME_MANAGER = AceType::MakeRefPtr<ThemeManager>();

} // namespace

class DomInputTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomInputTest::SetUpTestCase() {}
void DomInputTest::TearDownTestCase() {}
void DomInputTest::SetUp() {}
void DomInputTest::TearDown() {}

/**
 * @tc.name: DomInputCreatorTest001
 * @tc.desc: Test textfield component are created as default.
 * @tc.type: FUNC
 */
HWTEST_F(DomInputTest, DomInputCreatorTest001, TestSize.Level1)
{
    const std::string inputTestJsonStr = "{"
                                         "  \"tag\": \"input\",                                  "
                                         "  \"attr\": [{                                         "
                                         "                \"type\": \"text\"                     "
                                         "            }]                                         "
                                         "}";
    /**
     * @tc.steps: step1. Construct string without style and attribute, then create input node with it
     * @tc.expected: step1. input component are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(inputTestJsonStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    ASSERT_TRUE(boxChild);

#ifndef WEARABLE_PRODUCT
    RefPtr<TextFieldComponent> textareaComponent = AceType::DynamicCast<TextFieldComponent>(boxChild->GetChild());
    ASSERT_TRUE(textareaComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created input node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    RefPtr<TextFieldTheme> theme = THEME_MANAGER->GetTheme<TextFieldTheme>();
    ASSERT_TRUE(theme);

    EXPECT_EQ(textareaComponent->GetPlaceholderColor(), theme->GetPlaceholderColor());
    EXPECT_EQ(textareaComponent->GetMaxLength(), INPUT_MAXLENGTH_VALUE_DEFAULT);

    auto style = textareaComponent->GetTextStyle();
    EXPECT_EQ(style.GetTextColor(), theme->GetTextColor());
    EXPECT_EQ(style.GetFontSize(), theme->GetFontSize());
    EXPECT_EQ(style.GetFontWeight(), INPUT_FONT_WEIGHT_VALUE_DEFAULT);
    EXPECT_EQ(style.GetFontFamilies()[0], INPUT_FONT_FAMILY_VALUE_DEFAULT);

    RefPtr<Decoration> decoration = textareaComponent->GetDecoration();
    EXPECT_EQ(decoration->GetBackgroundColor(), theme->GetBgColor());
#endif
}

/**
 * @tc.name: DomInputCreatorTest002
 * @tc.desc: Test textfield component are created with all style and attribute.
 * @tc.type: FUNC
 */
HWTEST_F(DomInputTest, DomInputCreatorTest002, TestSize.Level1)
{
    const std::string inputTestJsonStr = "{"
                                         "  \"tag\": \"input\",                                  "
                                         "  \"attr\": [{                                         "
                                         "                \"type\": \"text\"                     "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"placeholder\": \"please input\"      "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"maxlength\": \"100\"                 "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"enterkeytype\": \"next\"             "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"disabled\": \"true\"                 "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"headerIcon\": \"test.png\"           "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"obscure\": \"true\"                  "
                                         "              }],                                      "
                                         "  \"style\":  [{                                       "
                                         "                \"color\" : \"#12345678\"              "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"cursorColor\": \"#12345678\"         "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"placeholderColor\": \"#12345678\"    "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"fontSize\": \"30.0\"                 "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"fontWeight\": \"600\"                "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"fontFamily\": \"serif\"              "
                                         "              }]                                       "
                                         "}";
    /**
     * @tc.steps: step1. Construct string with all style and attribute, then create input node with it
     * @tc.expected: step1. input component are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(inputTestJsonStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    ASSERT_TRUE(boxChild);

#ifndef WEARABLE_PRODUCT
    RefPtr<TextFieldComponent> textareaComponent = AceType::DynamicCast<TextFieldComponent>(boxChild->GetChild());
    ASSERT_TRUE(textareaComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created input node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(textareaComponent->GetPlaceholder(), INPUT_PLACEHOLDER_VALUE);
    EXPECT_EQ(textareaComponent->GetPlaceholderColor(), Color::FromString(INPUT_PLACEHOLDER_COLOR_STR_VALUE));
    EXPECT_EQ(textareaComponent->GetCursorColor(), Color::FromString(INPUT_CURCOR_COLOR_STR_VALUE));
    EXPECT_EQ(textareaComponent->GetMaxLength(), INPUT_MAXLENGTH_VALUE);
    EXPECT_EQ(textareaComponent->GetAction(), INPUT_TEXTINPUTACTION_VALUE);
    EXPECT_EQ(textareaComponent->IsEnabled(), false);
    EXPECT_EQ(textareaComponent->NeedObscure(), true);
    EXPECT_EQ(textareaComponent->GetIconImage(), INPUT_HEADER_ICON);

    auto style = textareaComponent->GetTextStyle();
    EXPECT_EQ(style.GetTextColor(), Color::FromString(INPUT_COLOR_STR_VALUE));
    EXPECT_EQ(style.GetFontSize(), INPUT_FONT_SIZE_VALUE);
    EXPECT_EQ(style.GetFontWeight(), INPUT_FONT_WEIGHT_VALUE);
    EXPECT_EQ(style.GetFontFamilies()[0], INPUT_FONT_FAMILY_VALUE);
#endif
}

/**
 * @tc.name: DomInputCreatorTest003
 * @tc.desc: Test input component are created with event.
 * @tc.type: FUNC
 */
HWTEST_F(DomInputTest, DomInputCreatorTest003, TestSize.Level1)
{
    const std::string inputTestJsonStr = "{"
                                         "  \"tag\": \"input\",                     "
                                         "  \"attr\": [{                            "
                                         "                \"type\": \"text\"        "
                                         "              }],                         "
                                         "  \"event\": [                            "
                                         "               \"change\",                "
                                         "               \"enterkeyclick\"          "
                                         "             ]                            "
                                         "}";
    /**
     * @tc.steps: step1. Construct string with event, then create input node with it
     * @tc.expected: step1. input component are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(inputTestJsonStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    ASSERT_TRUE(boxChild);

#ifndef WEARABLE_PRODUCT
    RefPtr<TextFieldComponent> textareaComponent = AceType::DynamicCast<TextFieldComponent>(boxChild->GetChild());
    ASSERT_TRUE(textareaComponent);

    /**
     * @tc.steps: step2. Check event of created input node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(textareaComponent->GetOnTextChange(), std::to_string(domNodeRoot->GetNodeId()));
    EXPECT_EQ(textareaComponent->GetOnFinishInput(), std::to_string(domNodeRoot->GetNodeId()));
#endif
}

/**
 * @tc.name: DomInputCreatorTest004
 * @tc.desc: Test button component are created as default.
 * @tc.type: FUNC
 */
HWTEST_F(DomInputTest, DomInputCreatorTest004, TestSize.Level1)
{
    const std::string inputTestJsonStr = "{"
                                         "  \"tag\": \"input\",                                  "
                                         "  \"attr\": [{                                         "
                                         "                \"type\": \"button\"                   "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"name\": \"input name\"               "
                                         "              }]                                       "
                                         "}";
    /**
     * @tc.steps: step1. Construct string without style and attribute, then create input node with it
     * @tc.expected: step1. input component are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(inputTestJsonStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    ASSERT_TRUE(boxChild);
    RefPtr<ButtonComponent> buttonComponent = AceType::DynamicCast<ButtonComponent>(boxChild->GetChild());
    ASSERT_TRUE(buttonComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created input node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    RefPtr<ButtonTheme> theme = THEME_MANAGER->GetTheme<ButtonTheme>();
    ASSERT_TRUE(theme);

    EXPECT_EQ(buttonComponent->GetType(), ButtonType::NORMAL);
    EXPECT_EQ(buttonComponent->GetWidth(), BUTTON_WIDTH_INIT);
}

/**
 * @tc.name: DomInputCreatorTest005
 * @tc.desc: Test button component are created with all style and attribute.
 * @tc.type: FUNC
 */
HWTEST_F(DomInputTest, DomInputCreatorTest005, TestSize.Level1)
{
    const std::string inputTestJsonStr = "{"
                                         "  \"tag\": \"input\",                                  "
                                         "  \"attr\": [{                                         "
                                         "                \"type\": \"button\"                   "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"name\": \"input name\"               "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"value\": \"input value\"             "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"disabled\": \"true\"                 "
                                         "              }],                                      "
                                         "  \"style\":  [{                                       "
                                         "                \"color\" : \"#12345678\"              "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"fontSize\": \"30.0\"                 "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"fontWeight\": \"600\"                "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"width\": \"100\"                     "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"height\": \"50\"                     "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"fontFamily\": \"serif\"              "
                                         "              }]                                       "
                                         "}";
    /**
     * @tc.steps: step1. Construct string without style and attribute, then create input node with it
     * @tc.expected: step1. input component are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(inputTestJsonStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    ASSERT_TRUE(boxChild);
    RefPtr<ButtonComponent> buttonComponent = AceType::DynamicCast<ButtonComponent>(boxChild->GetChild());
    ASSERT_TRUE(buttonComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created input node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(buttonComponent->GetType(), ButtonType::NORMAL);
    EXPECT_EQ(buttonComponent->GetDisabledState(), true);
    EXPECT_EQ(boxChild->GetWidthDimension().Value(), INPUT_WIDTH_VALUE.Value());
    EXPECT_EQ(boxChild->GetHeightDimension().Value(), INPUT_HEIGHT_VALUE.Value());
}

/**
 * @tc.name: DomInputCreatorTest006
 * @tc.desc: Test input component are created with event.
 * @tc.type: FUNC
 */
HWTEST_F(DomInputTest, DomInputCreatorTest006, TestSize.Level1)
{
    const std::string inputTestJsonStr = "{"
                                         "  \"tag\": \"input\",                  "
                                         "  \"attr\": [{                         "
                                         "                \"type\": \"button\"   "
                                         "              }],                      "
                                         "  \"event\": [                         "
                                         "               \"click\"               "
                                         "             ]                         "
                                         "}";
    /**
     * @tc.steps: step1. Construct string with event, then create input node with it
     * @tc.expected: step1. input component are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(inputTestJsonStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    ASSERT_TRUE(boxChild);
    RefPtr<ButtonComponent> buttonComponent = AceType::DynamicCast<ButtonComponent>(boxChild->GetChild());
    ASSERT_TRUE(buttonComponent);

    /**
     * @tc.steps: step2. Check event of created input node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(buttonComponent->GetClickedEventId(), std::to_string(domNodeRoot->GetNodeId()));
}

/**
 * @tc.name: DomInputCreatorTest007
 * @tc.desc: Test checkbox component are created as default.
 * @tc.type: FUNC
 */
HWTEST_F(DomInputTest, DomInputCreatorTest007, TestSize.Level1)
{
    const std::string inputTestJsonStr = "{"
                                         "  \"tag\": \"input\",                                  "
                                         "  \"attr\": [{                                         "
                                         "                \"type\": \"checkbox\"                 "
                                         "              }]                                       "
                                         "}";
    /**
     * @tc.steps: step1. Construct string without style and attribute, then create input node with it
     * @tc.expected: step1. input component are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(inputTestJsonStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    ASSERT_TRUE(boxChild);
    RefPtr<CheckboxComponent> checkboxComponent = AceType::DynamicCast<CheckboxComponent>(boxChild->GetChild());
    ASSERT_TRUE(checkboxComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created input node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    RefPtr<CheckboxTheme> theme = THEME_MANAGER->GetTheme<CheckboxTheme>();
    ASSERT_TRUE(theme);

    EXPECT_EQ(checkboxComponent->GetValue(), INPUT_CHECKED_VALUE_DEFAULT);
    EXPECT_EQ(checkboxComponent->GetWidth(), theme->GetWidth());
    EXPECT_EQ(checkboxComponent->GetHeight(), theme->GetHeight());
}

/**
 * @tc.name: DomInputCreatorTest008
 * @tc.desc: Test checkbox component are created with all style and attribute.
 * @tc.type: FUNC
 */
HWTEST_F(DomInputTest, DomInputCreatorTest008, TestSize.Level1)
{
    const std::string inputTestJsonStr = "{"
                                         "  \"tag\": \"input\",                                  "
                                         "  \"attr\": [{                                         "
                                         "                \"type\": \"checkbox\"                 "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"checked\": \"true\"                  "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"name\": \"input name\"               "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"value\": \"input value\"             "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"disabled\": \"true\"                 "
                                         "              }],                                      "
                                         "  \"style\":  [{                                       "
                                         "                \"width\": \"100\"                     "
                                         "              },                                       "
                                         "              {                                        "
                                         "                \"height\": \"50\"                     "
                                         "              }]                                       "
                                         "}";
    /**
     * @tc.steps: step1. Construct string without style and attribute, then create input node with it
     * @tc.expected: step1. input component are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(inputTestJsonStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    ASSERT_TRUE(boxChild);
    RefPtr<CheckboxComponent> checkboxComponent = AceType::DynamicCast<CheckboxComponent>(boxChild->GetChild());
    ASSERT_TRUE(checkboxComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created input node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(checkboxComponent->IsDisabled(), true);
    EXPECT_EQ(checkboxComponent->GetValue(), INPUT_CHECKED_VALUE);
    EXPECT_EQ(boxChild->GetWidthDimension().Value(), INPUT_WIDTH_VALUE.Value());
    EXPECT_EQ(boxChild->GetHeightDimension().Value(), INPUT_HEIGHT_VALUE.Value());
}

/**
 * @tc.name: DomInputCreatorTest009
 * @tc.desc: Test input component are created with event.
 * @tc.type: FUNC
 */
HWTEST_F(DomInputTest, DomInputCreatorTest009, TestSize.Level1)
{
    const std::string inputTestJsonStr = "{"
                                         "  \"tag\": \"input\",                  "
                                         "  \"attr\": [{                         "
                                         "                \"type\": \"checkbox\" "
                                         "              }],                      "
                                         "  \"event\": [                         "
                                         "               \"change\"              "
                                         "             ]                         "
                                         "}";
    /**
     * @tc.steps: step1. Construct string with event, then create input node with it
     * @tc.expected: step1. input component are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(inputTestJsonStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    ASSERT_TRUE(boxChild);
    RefPtr<CheckboxComponent> checkboxComponent = AceType::DynamicCast<CheckboxComponent>(boxChild->GetChild());
    ASSERT_TRUE(checkboxComponent);

    /**
     * @tc.steps: step2. Check event of created input node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(checkboxComponent->GetChangeEvent(), std::to_string(domNodeRoot->GetNodeId()));
}

/**
 * @tc.name: DomInputCreatorTest010
 * @tc.desc: Test radio component are created as default.
 * @tc.type: FUNC
 */
HWTEST_F(DomInputTest, DomInputCreatorTest010, TestSize.Level1)
{
    const std::string inputTestJsonStr = "{"
                                         "  \"tag\": \"input\",                                  "
                                         "  \"attr\": [{                                         "
                                         "                \"type\": \"radio\"                    "
                                         "              }]                                       "
                                         "}";
    /**
     * @tc.steps: step1. Construct string without style and attribute, then create input node with it
     * @tc.expected: step1. input component are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(inputTestJsonStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    ASSERT_TRUE(boxChild);
    RefPtr<RadioComponent<std::string>> radioComponent =
        AceType::DynamicCast<RadioComponent<std::string>>(boxChild->GetChild());
    ASSERT_TRUE(radioComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created input node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    RefPtr<CheckboxTheme> theme = THEME_MANAGER->GetTheme<CheckboxTheme>();
    ASSERT_TRUE(theme);

    EXPECT_EQ(radioComponent->GetWidth(), theme->GetWidth());
    EXPECT_EQ(radioComponent->GetHeight(), theme->GetHeight());
}

/**
 * @tc.name: DomInputCreatorTest012
 * @tc.desc: Test input component are created with event.
 * @tc.type: FUNC
 */
HWTEST_F(DomInputTest, DomInputCreatorTest012, TestSize.Level1)
{
    const std::string inputTestJsonStr = "{"
                                         "  \"tag\": \"input\",                  "
                                         "  \"attr\": [{                         "
                                         "                \"type\": \"radio\"    "
                                         "              }],                      "
                                         "  \"event\": [ \"change\" ]            "
                                         "}";
    /**
     * @tc.steps: step1. Construct string with event, then create input node with it
     * @tc.expected: step1. input component are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(inputTestJsonStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    ASSERT_TRUE(boxChild);
    RefPtr<RadioComponent<std::string>> radioComponent =
        AceType::DynamicCast<RadioComponent<std::string>>(boxChild->GetChild());
    ASSERT_TRUE(radioComponent);

    /**
     * @tc.steps: step2. Check event of created input node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(radioComponent->GetChangeEvent(), std::to_string(domNodeRoot->GetNodeId()));
}

} // namespace OHOS::Ace::Framework
