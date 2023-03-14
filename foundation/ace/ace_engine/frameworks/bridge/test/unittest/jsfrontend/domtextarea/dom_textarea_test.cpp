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

#include "core/common/ime/text_selection.h"
#include "core/components/common/layout/constants.h"
#include "core/components/text_field/textfield_theme.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/common/dom/dom_textarea.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

constexpr uint32_t TEXTAREA_MAXLENGTH_VALUE_DEFAULT = std::numeric_limits<uint32_t>::max();
const FontWeight TEXTAREA_FONT_WEIGHT_VALUE_DEFAULT = FontWeight::W400;
const std::string TEXTAREA_FONT_FAMILY_VALUE_DEFAULT = "sans-serif";

const std::string TEXTAREA_DATA_VALUE = "test dom textarea";
const std::string TEXTAREA_PLACEHOLDER_VALUE = "please input";
constexpr uint32_t TEXTAREA_MAXLENGTH_VALUE = 100;
const std::string TEXTAREA_COLOR_STR_VALUE = "#12345678";
const std::string TEXTAREA_PLACEHOLDER_COLOR_STR_VALUE = "#12345678";
constexpr double TEXTAREA_FONT_SIZE_VALUE = 30.0;
const FontWeight TEXTAREA_FONT_WEIGHT_VALUE = FontWeight::W600;
const std::string TEXTAREA_FONT_FAMILY_VALUE = "serif";
const std::string TEXTAREA_HEADER_ICON = "test.png";
const TextSelection TEXTAREA_SELECTION = TextSelection(2, 5);
const TextSelection TEXTAREA_SELECTION_DEFAULT = TextSelection(-1, -1);

} // namespace

class DomTextareaTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomTextareaTest::SetUpTestCase() {}
void DomTextareaTest::TearDownTestCase() {}
void DomTextareaTest::SetUp() {}
void DomTextareaTest::TearDown() {}

/**
 * @tc.name: DomTextareaCreatorTest001
 * @tc.desc: Test textarea component are created as default.
 * @tc.type: FUNC
 */
HWTEST_F(DomTextareaTest, DomTextareaCreatorTest001, TestSize.Level1)
{
    const std::string textareaTestJsonStr = "{"
                                    "  \"tag\": \"textarea\"                                "
                                    "}";
    /**
     * @tc.steps: step1. Construct string without style and attribute, then create textarea node with it
     * @tc.expected: step1. textarea component are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(textareaTestJsonStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    ASSERT_TRUE(boxChild);
    RefPtr<TextFieldComponent> textareaComponent = AceType::DynamicCast<TextFieldComponent>(boxChild->GetChild());
    ASSERT_TRUE(textareaComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created textarea node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    RefPtr<TextFieldTheme> theme = themeManager->GetTheme<TextFieldTheme>();
    ASSERT_TRUE(theme);

    EXPECT_EQ(textareaComponent->GetPlaceholderColor(), theme->GetPlaceholderColor());
    EXPECT_EQ(textareaComponent->GetCursorColor(), theme->GetCursorColor());
    EXPECT_EQ(textareaComponent->GetMaxLength(), TEXTAREA_MAXLENGTH_VALUE_DEFAULT);
    EXPECT_EQ(textareaComponent->GetSelection(), TEXTAREA_SELECTION_DEFAULT);
    EXPECT_TRUE(textareaComponent->IsSoftKeyboardEnabled());

    auto style = textareaComponent->GetTextStyle();
    EXPECT_EQ(style.GetTextColor(), theme->GetTextColor());
    EXPECT_EQ(style.GetFontSize(), theme->GetFontSize());
    EXPECT_EQ(style.GetFontWeight(), TEXTAREA_FONT_WEIGHT_VALUE_DEFAULT);
    EXPECT_EQ(style.GetFontFamilies()[0], TEXTAREA_FONT_FAMILY_VALUE_DEFAULT);
}

/**
 * @tc.name: DomTextareaCreatorTest002
 * @tc.desc: Test textarea component are created with all style and attribute.
 * @tc.type: FUNC
 */
HWTEST_F(DomTextareaTest, DomTextareaCreatorTest002, TestSize.Level1)
{
    const std::string textareaTestJsonStr = "{"
                                    "  \"tag\": \"textarea\",                               "
                                    "  \"attr\": [{                                         "
                                    "                \"value\": \"test dom textarea\"       "
                                    "              },                                       "
                                    "              {                                        "
                                    "                \"placeholder\": \"please input\"      "
                                    "              },                                       "
                                    "              {                                        "
                                    "                \"maxlength\": \"100\"                 "
                                    "              },                                       "
                                    "              {                                        "
                                    "                \"extend\": \"true\"                   "
                                    "              },                                       "
                                    "              {                                        "
                                    "                \"softkeyboardenabled\": \"false\"     "
                                    "              },                                       "
                                    "              {                                        "
                                    "                \"selectedstart\": \"2\"               "
                                    "              },                                       "
                                    "              {                                        "
                                    "                \"selectedend\": \"5\"                 "
                                    "              },                                       "
                                    "              {                                        "
                                    "                \"headerIcon\": \"test.png\"           "
                                    "              }],                                      "
                                    "  \"style\":  [{                                       "
                                    "                \"color\" : \"#12345678\"              "
                                    "              },                                       "
                                    "              {                                        "
                                    "                \"placeholderColor\": \"#12345678\"    "
                                    "              },                                       "
                                    "              {                                        "
                                    "                \"caretColor\": \"#12345678\"          "
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
     * @tc.steps: step1. Construct string with all style and attribute, then create textarea node with it
     * @tc.expected: step1. textarea component are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(textareaTestJsonStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    ASSERT_TRUE(boxChild);
    RefPtr<TextFieldComponent> textareaComponent = AceType::DynamicCast<TextFieldComponent>(boxChild->GetChild());
    ASSERT_TRUE(textareaComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created textarea node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(textareaComponent->GetPlaceholder(), TEXTAREA_PLACEHOLDER_VALUE);
    EXPECT_EQ(textareaComponent->GetPlaceholderColor(), Color::FromString(TEXTAREA_COLOR_STR_VALUE));
    EXPECT_EQ(textareaComponent->GetCursorColor(), Color::FromString(TEXTAREA_COLOR_STR_VALUE));
    EXPECT_EQ(textareaComponent->GetMaxLength(), TEXTAREA_MAXLENGTH_VALUE);
    EXPECT_TRUE(textareaComponent->IsExtend());
    EXPECT_TRUE(!textareaComponent->IsSoftKeyboardEnabled());
    EXPECT_EQ(textareaComponent->GetIconImage(), TEXTAREA_HEADER_ICON);
    EXPECT_EQ(textareaComponent->GetSelection(), TEXTAREA_SELECTION);

    auto style = textareaComponent->GetTextStyle();
    EXPECT_EQ(style.GetTextColor(), Color::FromString(TEXTAREA_PLACEHOLDER_COLOR_STR_VALUE));
    EXPECT_EQ(style.GetFontSize().Value(), TEXTAREA_FONT_SIZE_VALUE);
    EXPECT_EQ(style.GetFontWeight(), TEXTAREA_FONT_WEIGHT_VALUE);
    EXPECT_EQ(style.GetFontFamilies()[0], TEXTAREA_FONT_FAMILY_VALUE);
}

} // namespace OHOS::Ace::Framework
