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

#include "base/utils/system_properties.h"
#include "core/common/ime/text_selection.h"
#include "core/components/common/layout/constants.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#define private public
#include "frameworks/bridge/common/dom/dom_search.h"
#undef private
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const std::string TEST_ICON_SRC = "Common/email.png";
const std::string TEST_HINT = "this is hint";
const std::string TEST_VALUE = "this is value";
const std::string EMPTY_STRING = "";
const std::string TEST_TEXT_COLOR = "#FF0000";
const std::string TEST_PLACEHOLDER_COLOR = "#0000FF";
const std::string TEST_BACKGROUND_COLOR = "#00ff00";
const std::string TEST_TEXT_FONT_FAMILY = "sans-serif";
constexpr FontWeight TEST_TEXT_FONT_WEIGHT = FontWeight::W400;
constexpr double TEST_TEXT_FONT_SIZE = 30.0;
const std::string DEFAULT_COLOR = "#000000";
const std::string DEFAULT_BACKGROUND_COLOR = "#0C000000";
const std::string DEFAULT_TEXT_COLOR = "#E5000000";
const std::string DEFAULT_PLACEHOLDER_COLOR = "#99000000";
constexpr FontWeight DEFAULT_TEXT_FONT_WEIGHT = FontWeight::W400;
constexpr FontWeight DEFAULT_FONT_WEIGHT_VALUE = FontWeight::NORMAL;
constexpr double DEFAULT_TEXT_FONT_SIZE = 16.0;
constexpr double INVALID_TEXT_FONT_SIZE = 0.0;
constexpr double TEST_PADDING_LEFT_VALUE = 10.0;
constexpr double TEST_PADDING_TOP_VALUE = 20.0;
constexpr double TEST_PADDING_RIGHT_VALUE = 70.0;
constexpr double TEST_PADDING_BOTTOM_VALUE = 40.0;
constexpr double DEFAULT_PADDING_LEFT_VALUE = 4.0;
constexpr double DEFAULT_PADDING_TOP_VALUE = 0.0;
constexpr double DEFAULT_PADDING_RIGHT_VALUE = 40.0;
constexpr double DEFAULT_PADDING_BOTTOM_VALUE = 0.0;
constexpr double ERROR_PADDING_LEFT_VALUE = 0.0;
const TextSelection TEST_SELECTION = TextSelection(2, 5);
const TextSelection DEFAULT_SELECTION = TextSelection(-1, -1);

const std::string SEARCH_JSON = "{                                                          "
                                "  \"tag\": \"search\",                                     "
                                "  \"attr\": [{                                             "
                                "                \"icon\" : \"Common/email.png\"            "
                                "              },                                           "
                                "              {                                            "
                                "                \"hint\" : \"this is hint\"                "
                                "              },                                           "
                                "              {                                            "
                                "                \"selectedstart\": \"2\"                   "
                                "              },                                           "
                                "              {                                            "
                                "                \"selectedend\": \"5\"                     "
                                "              },                                           "
                                "              {                                            "
                                "                \"softkeyboardenabled\": \"false\"         "
                                "              },                                           "
                                "              {                                            "
                                "                \"value\" : \"this is value\"              "
                                "              }],                                          "
                                "  \"style\": [{                                            "
                                "           \"fontSize\":\"30.0\"                           "
                                "          },                                               "
                                "          {                                                "
                                "           \"fontWeight\":\"400\"                          "
                                "           },                                              "
                                "          {                                                "
                                "           \"color\":\"#ff0000\"                           "
                                "           },                                              "
                                "          {                                                "
                                "           \"placeholderColor\":\"#0000ff\"                "
                                "           },                                              "
                                "          {                                                "
                                "           \"caretColor\":\"#0000ff\"                      "
                                "           },                                              "
                                "           {                                               "
                                "           \"fontFamily\":\"sans-serif\"                   "
                                "           },                                              "
                                "           {                                               "
                                "           \"backgroundColor\":\"#00ff00\"                 "
                                "           },                                              "
                                "           {                                               "
                                "           \"paddingLeft\":\"10px\"                        "
                                "           },                                              "
                                "          {                                                "
                                "           \"paddingTop\":\"20px\"                         "
                                "           },                                              "
                                "          {                                                "
                                "           \"paddingRight\":\"30px\"                       "
                                "           },                                              "
                                "          {                                                "
                                "           \"paddingBottom\":\"40px\"                      "
                                "            }]                                             "
                                "}                                                          ";
const std::string INVALID_SEARCH_JSON = "{                                                          "
                                        "  \"tag\": \"search\",                                     "
                                        "  \"style\": [{                                            "
                                        "           \"fontSize\":\"abcd\"                           "
                                        "          },                                               "
                                        "          {                                                "
                                        "           \"fontWeight\":\"abcd\"                         "
                                        "           },                                              "
                                        "          {                                                "
                                        "           \"color\":\"abcd\"                              "
                                        "           },                                              "
                                        "          {                                                "
                                        "           \"placeholderColor\":\"abcd\"                   "
                                        "           },                                              "
                                        "          {                                                "
                                        "           \"caretColor\":\"abcd\"                         "
                                        "           },                                              "
                                        "           {                                               "
                                        "           \"paddingLeft\":\"abcd\"                        "
                                        "           },                                              "
                                        "          {                                                "
                                        "           \"paddingTop\":\"abcd\"                         "
                                        "           },                                              "
                                        "          {                                                "
                                        "           \"paddingRight\":\"abcd\"                       "
                                        "           },                                              "
                                        "          {                                                "
                                        "           \"paddingBottom\":\"abcd\"                      "
                                        "           }]                                              "
                                        "}                                                          ";

const std::string PADDING_SEARCH_JSON = "{                                                          "
                                        "  \"tag\": \"search\",                                     "
                                        "  \"style\": [{                                            "
                                        "           \"paddingStart\":\"10px\"                       "
                                        "           },                                              "
                                        "          {                                                "
                                        "           \"paddingEnd\":\"30px\"                         "
                                        "            }]                                             "
                                        "}                                                          ";
} // namespace

class DomSearchTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomSearchTest::SetUpTestCase() {}
void DomSearchTest::TearDownTestCase() {}
void DomSearchTest::SetUp() {}
void DomSearchTest::TearDown() {}

bool IsTv()
{
    return SystemProperties::GetDeviceType() == DeviceType::TV;
}

/**
 * @tc.name: CreateSearchFromDsl001
 * @tc.desc: Test search node and search component created successfully with all attributes set.
 * @tc.type: FUNC
 */
HWTEST_F(DomSearchTest, CreateSearchFromDsl001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct json string with attributes set, then create search node with it .
     * @tc.expected: step1. Search node and child search component are created with correct attributes and styles.
     */
    auto domSearch = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(SEARCH_JSON);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domSearch);
    RefPtr<SearchComponent> searchComponent = AceType::DynamicCast<SearchComponent>(boxChild->GetChild());
    EXPECT_TRUE(searchComponent);
    RefPtr<TextFieldComponent> textFieldComponent =
        AceType::DynamicCast<TextFieldComponent>(searchComponent->GetChild());
    EXPECT_TRUE(textFieldComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created search node.
     * @tc.expected: step2. Search node's attrs and styles are as expected.
     */
    const auto& textStyle = AceType::DynamicCast<DOMSearch>(domSearch)->textStyle_;
    EXPECT_EQ(textStyle.GetFontSize().Value(), TEST_TEXT_FONT_SIZE);
    EXPECT_TRUE(textStyle.GetFontWeight() == TEST_TEXT_FONT_WEIGHT);
    EXPECT_EQ(textStyle.GetFontFamilies()[0], TEST_TEXT_FONT_FAMILY);
    EXPECT_EQ(textFieldComponent->GetFocusTextColor(), Color::FromString(TEST_TEXT_COLOR));
    EXPECT_EQ(textFieldComponent->GetPlaceholderColor(), Color::FromString(TEST_PLACEHOLDER_COLOR));
    EXPECT_EQ(textFieldComponent->GetCursorColor(), Color::FromString(TEST_PLACEHOLDER_COLOR));
    EXPECT_EQ(textFieldComponent->GetBgColor(), Color::FromString(TEST_BACKGROUND_COLOR));
    EXPECT_EQ(textFieldComponent->GetIconImage(), (IsTv() ? "" : TEST_ICON_SRC));
    EXPECT_EQ(textFieldComponent->GetPlaceholder(), TEST_HINT);
    EXPECT_EQ(textFieldComponent->GetValue(), TEST_VALUE);
    EXPECT_EQ(textFieldComponent->GetDecoration()->GetPadding().Left().Value(), TEST_PADDING_LEFT_VALUE);
    EXPECT_EQ(textFieldComponent->GetDecoration()->GetPadding().Top().Value(), TEST_PADDING_TOP_VALUE);
    EXPECT_EQ(textFieldComponent->GetDecoration()->GetPadding().Right().Value(), TEST_PADDING_RIGHT_VALUE);
    EXPECT_EQ(textFieldComponent->GetDecoration()->GetPadding().Bottom().Value(), TEST_PADDING_BOTTOM_VALUE);
    EXPECT_EQ(textFieldComponent->GetSelection(), TEST_SELECTION);
    EXPECT_TRUE(!textFieldComponent->IsSoftKeyboardEnabled());
}

/**
 * @tc.name: CreateSearchFromDsl002
 * @tc.desc: Test search node and search component created correctly with no styles and attributes set.
 * @tc.type: FUNC
 */
HWTEST_F(DomSearchTest, CreateSearchFromDsl002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct json string with no styles and attributes set, then create search node with it .
     * @tc.expected: step1. Search node and child search component are created with default attributes and styles.
     */
    std::string searchDsl = "{  \"tag\": \"search\" }";
    auto domSearch = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(searchDsl);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domSearch);
    RefPtr<SearchComponent> searchComponent = AceType::DynamicCast<SearchComponent>(boxChild->GetChild());
    EXPECT_TRUE(searchComponent);
    RefPtr<TextFieldComponent> textFieldComponent =
        AceType::DynamicCast<TextFieldComponent>(searchComponent->GetChild());
    EXPECT_TRUE(textFieldComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created search node.
     * @tc.expected: step2. Search node's attrs and styles are as default.
     */
    const auto& textStyle = AceType::DynamicCast<DOMSearch>(domSearch)->textStyle_;
    EXPECT_EQ(textStyle.GetFontSize().Value(), DEFAULT_TEXT_FONT_SIZE);
    EXPECT_TRUE(textStyle.GetFontWeight() == DEFAULT_TEXT_FONT_WEIGHT);
    EXPECT_EQ(textStyle.GetFontFamilies()[0], TEST_TEXT_FONT_FAMILY);
    EXPECT_EQ(textFieldComponent->GetIconImage(), EMPTY_STRING);
    EXPECT_EQ(textFieldComponent->GetPlaceholder(), EMPTY_STRING);
    EXPECT_EQ(textFieldComponent->GetTextEditController()->GetText(), EMPTY_STRING);
    EXPECT_EQ(textFieldComponent->GetFocusTextColor(), Color::FromString(DEFAULT_TEXT_COLOR));
    EXPECT_EQ(textFieldComponent->GetPlaceholderColor(), Color::FromString(DEFAULT_PLACEHOLDER_COLOR));
    EXPECT_EQ(textFieldComponent->GetBgColor(), Color::FromString(DEFAULT_BACKGROUND_COLOR));
    EXPECT_EQ(textFieldComponent->GetDecoration()->GetPadding().Left().Value(), DEFAULT_PADDING_LEFT_VALUE);
    EXPECT_EQ(textFieldComponent->GetDecoration()->GetPadding().Top().Value(), DEFAULT_PADDING_TOP_VALUE);
    EXPECT_EQ(textFieldComponent->GetDecoration()->GetPadding().Right().Value(), DEFAULT_PADDING_RIGHT_VALUE);
    EXPECT_EQ(textFieldComponent->GetDecoration()->GetPadding().Bottom().Value(), DEFAULT_PADDING_BOTTOM_VALUE);
    EXPECT_EQ(textFieldComponent->GetSelection(), DEFAULT_SELECTION);
}

/**
 * @tc.name: CreateSearchFromDsl003
 * @tc.desc: Test search node and search component created correctly with invalid text styles and attributes set.
 * @tc.type: FUNC
 */
HWTEST_F(DomSearchTest, CreateSearchFromDsl003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with invalid text styles and attributes set, then create search node with it.
     * @tc.expected: step1. Search node and child search component are created with default text attributes and styles.
     */
    auto domSearch = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(INVALID_SEARCH_JSON);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domSearch);
    RefPtr<SearchComponent> searchComponent = AceType::DynamicCast<SearchComponent>(boxChild->GetChild());
    EXPECT_TRUE(searchComponent);
    RefPtr<TextFieldComponent> textFieldComponent =
        AceType::DynamicCast<TextFieldComponent>(searchComponent->GetChild());
    EXPECT_TRUE(textFieldComponent);

    /**
     * @tc.steps: step2. Check styles and attributes of created search node.
     * @tc.expected: step2. Search node's attrs and styles are as default.
     */
    const auto& textStyle = AceType::DynamicCast<DOMSearch>(domSearch)->textStyle_;
    EXPECT_EQ(textStyle.GetFontSize().Value(), INVALID_TEXT_FONT_SIZE);
    EXPECT_TRUE(textStyle.GetFontWeight() == DEFAULT_FONT_WEIGHT_VALUE);
    EXPECT_EQ(textStyle.GetFontFamilies()[0], TEST_TEXT_FONT_FAMILY);
    EXPECT_EQ(textFieldComponent->GetFocusTextColor(), Color::FromString(DEFAULT_COLOR));
    EXPECT_EQ(textFieldComponent->GetPlaceholderColor(), Color::FromString(DEFAULT_COLOR));
    EXPECT_EQ(textFieldComponent->GetBgColor(), Color::FromString(DEFAULT_BACKGROUND_COLOR));
    EXPECT_EQ(textFieldComponent->GetDecoration()->GetPadding().Left().Value(), ERROR_PADDING_LEFT_VALUE);
    EXPECT_EQ(textFieldComponent->GetDecoration()->GetPadding().Top().Value(), DEFAULT_PADDING_TOP_VALUE);
    EXPECT_EQ(textFieldComponent->GetDecoration()->GetPadding().Right().Value(), DEFAULT_PADDING_RIGHT_VALUE);
    EXPECT_EQ(textFieldComponent->GetDecoration()->GetPadding().Bottom().Value(), DEFAULT_PADDING_BOTTOM_VALUE);
    EXPECT_EQ(textFieldComponent->GetIconImage(), EMPTY_STRING);
    EXPECT_EQ(textFieldComponent->GetPlaceholder(), EMPTY_STRING);
    EXPECT_EQ(textFieldComponent->GetTextEditController()->GetText(), EMPTY_STRING);
    EXPECT_EQ(textFieldComponent->GetSelection(), DEFAULT_SELECTION);
}

/**
 * @tc.name: CreateSearchFromDsl004
 * @tc.desc: Test search node and search component created correctly with paddingStart and paddingEnd styles.
 * @tc.type: FUNC
 */
HWTEST_F(DomSearchTest, CreateSearchFromDsl004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with paddingStart and paddingEnd set, then create search node with it.
     * @tc.expected: step1. Search node and child search component are created with correct paddingStart and paddingEnd.
     */
    auto domSearch = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(PADDING_SEARCH_JSON);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domSearch);
    RefPtr<SearchComponent> searchComponent = AceType::DynamicCast<SearchComponent>(boxChild->GetChild());
    EXPECT_TRUE(searchComponent);
    RefPtr<TextFieldComponent> textFieldComponent =
        AceType::DynamicCast<TextFieldComponent>(searchComponent->GetChild());
    EXPECT_TRUE(textFieldComponent);

    /**
     * @tc.steps: step2. Check padding values of created search node.
     * @tc.expected: step2. Search node's padding values are as expected.
     */
    EXPECT_EQ(textFieldComponent->GetDecoration()->GetPadding().Left().Value(), TEST_PADDING_LEFT_VALUE);
    EXPECT_EQ(textFieldComponent->GetDecoration()->GetPadding().Right().Value(), TEST_PADDING_RIGHT_VALUE);
}

} // namespace OHOS::Ace::Framework
