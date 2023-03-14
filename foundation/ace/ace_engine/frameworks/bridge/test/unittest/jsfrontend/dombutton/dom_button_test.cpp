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
#include "core/components/button/button_theme.h"
#include "core/components/progress/loading_progress_component.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/dom/dom_button.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

constexpr uint32_t BG_COLOR_CAPSULE = 0xffff0000;
constexpr uint32_t BG_COLOR_TEXT = 0x00000000;
#ifndef WEARABLE_PRODUCT
constexpr uint32_t PROGRESS_COLOR_TEST = 0xff00ff00;
#endif
constexpr uint32_t CLICKED_COLOR_TEST = 0xff00aa00;
constexpr uint32_t FOCUS_COLOR_TEST = 0xff00bb00;
constexpr uint32_t TEXT_COLOR_TEST = 0xff00cc00;
constexpr uint32_t BORDER_COLOR_TEST = 0xff00dd00;

constexpr double CIRCLE_RADIUS = 50.0;
#ifndef WEARABLE_PRODUCT
constexpr double PROGRESS_DIAMETER_TEST = 50.0;
#endif
constexpr double WIDTH_TEST = 200.0;
constexpr double HEIGHT_TEST = 100.0;
constexpr double BORDER_WIDTH_TEST = 3.0;
constexpr double FONT_SIZE_TEST = 40.0;

const std::string ICON_SRC_TEST = "pages/index/ic_test.png";

} // namespace

class DomButtonTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DomButtonTest::SetUpTestCase() {}
void DomButtonTest::TearDownTestCase() {}
void DomButtonTest::SetUp() {}
void DomButtonTest::TearDown() {}

/**
 * @tc.name: SetAttributesAndStyles001
 * @tc.desc: All attribute and style values are equal to the default from theme when there are no user-defined
 *           attributes or styles.
 * @tc.type: FUNC
 */
HWTEST_F(DomButtonTest, SetAttributesAndStyles001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create json string for testing, just value attribute set.
     */
    const std::string jsonButtonStr = ""
                                      "{                                        "
                                      "  \"tag\": \"button\"                    "
                                      "}";

    /**
     * @tc.steps: step2. Verify whether attributes and styles are equal to default values.
     * @tc.expected: step2. All attribute and style values are equal to the default from theme.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonButtonStr);
    ASSERT_TRUE(domNodeRoot);
    auto buttonComponent = AceType::DynamicCast<ButtonComponent>(domNodeRoot->GetSpecializedComponent());
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    RefPtr<ButtonTheme> buttonTheme = themeManager->GetTheme<ButtonTheme>();
    ASSERT_TRUE(buttonTheme);
    ASSERT_TRUE(buttonComponent);
    ASSERT_TRUE(buttonComponent->GetBackgroundColor() == buttonTheme->GetBgColor());
    ASSERT_TRUE(buttonComponent->GetFocusColor() == buttonTheme->GetBgFocusColor());
    ASSERT_TRUE(buttonComponent->GetFocusAnimationColor() == buttonTheme->GetBgFocusColor());

    auto children = buttonComponent->GetChildren();
    ASSERT_TRUE(children.size() > 0);
    auto paddingChild = AceType::DynamicCast<PaddingComponent>(children.front());
    ASSERT_TRUE(paddingChild);
    auto textChild = AceType::DynamicCast<TextComponent>(paddingChild->GetChild());
    ASSERT_TRUE(textChild);
    ASSERT_TRUE(textChild->GetFocusColor() == buttonTheme->GetTextFocusColor());
    ASSERT_TRUE(textChild->GetTextStyle().GetMaxLines() == uint32_t(buttonTheme->GetTextMaxLines()));
    auto textStyle = textChild->GetTextStyle();
    ASSERT_TRUE(textStyle.GetAdaptTextSize());
    ASSERT_TRUE(textStyle.GetAdaptMinFontSize() == buttonTheme->GetMinFontSize());
}

/**
 * @tc.name: SetAttributesAndStyles002
 * @tc.desc: The focus color is equal to background color when type is capsule and background color defined by user.
 * @tc.type: FUNC
 */
HWTEST_F(DomButtonTest, SetAttributesAndStyles002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create json string for testing, the type is capsule, background color defined.
     */
    const std::string jsonButtonStr = ""
                                      "{                                                         "
                                      "  \"tag\": \"button\",                                    "
                                      "  \"attr\":  [{  \"value\" : \"test\"    },               "
                                      "              {  \"type\" : \"capsule\"  }],              "
                                      "  \"style\": [{  \"backgroundColor\" : \"#ff0000\"  }]    "
                                      "}";

    /**
     * @tc.steps: step2. Verify whether focus color is equal to user-defined background.
     * @tc.expected: step2. The focus color is equal to background color.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonButtonStr);
    ASSERT_TRUE(domNodeRoot);
    auto buttonComponent = AceType::DynamicCast<ButtonComponent>(domNodeRoot->GetSpecializedComponent());
    ASSERT_TRUE(buttonComponent);
    ASSERT_TRUE(buttonComponent->GetBackgroundColor() == Color(BG_COLOR_CAPSULE));
}

/**
 * @tc.name: SetAttributesAndStyles003
 * @tc.desc: The background color is transparent when type is text.
 * @tc.type: FUNC
 */
HWTEST_F(DomButtonTest, SetAttributesAndStyles003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create json string for testing, the type is text.
     */
    const std::string jsonButtonStr = ""
                                      "{                                             "
                                      "  \"tag\": \"button\",                        "
                                      "  \"attr\": [{  \"value\" : \"test\"  },      "
                                      "             {  \"type\" : \"text\"   }]      "
                                      "}";

    /**
     * @tc.steps: step2. Verify whether background color is transparent.
     * @tc.expected: step2. The background color is transparent.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonButtonStr);
    ASSERT_TRUE(domNodeRoot);
    auto buttonComponent = AceType::DynamicCast<ButtonComponent>(domNodeRoot->GetSpecializedComponent());
    ASSERT_TRUE(buttonComponent);
    ASSERT_TRUE(buttonComponent->GetBackgroundColor() == Color(BG_COLOR_TEXT));
}

/**
 * @tc.name: SetAttributesAndStyles004
 * @tc.desc: The width and height are equal to double radius when type is circle and just radius defined.
 * @tc.type: FUNC
 */
HWTEST_F(DomButtonTest, SetAttributesAndStyles004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create json string for testing, the type is circle, radius style defined.
     */
    const std::string jsonButtonStr = ""
                                      "{                                                              "
                                      "  \"tag\": \"button\",                                         "
                                      "  \"attr\":  [{  \"type\" : \"circle\"  },                     "
                                      "              {  \"icon\" : \"pages/index/ic_test.png\"  }],   "
                                      "  \"style\": [{  \"radius\" : \"50\"    }]                     "
                                      "}";

    /**
     * @tc.steps: step2. Verify whether width and height are equal to double radius.
     * @tc.expected: step2. The width and height are equal to double radius.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonButtonStr);
    ASSERT_TRUE(domNodeRoot);
    auto buttonComponent = AceType::DynamicCast<ButtonComponent>(domNodeRoot->GetSpecializedComponent());
    ASSERT_TRUE(buttonComponent);
    ASSERT_TRUE(buttonComponent->GetRectRadius() == Dimension(CIRCLE_RADIUS));

    auto children = buttonComponent->GetChildren();
    ASSERT_TRUE(children.size() > 0);
    auto paddingChild = AceType::DynamicCast<PaddingComponent>(children.front());
    ASSERT_TRUE(paddingChild);
    auto imageChild = AceType::DynamicCast<ImageComponent>(paddingChild->GetChild());
    ASSERT_TRUE(imageChild->GetSrc() == ICON_SRC_TEST);
}

#ifndef WEARABLE_PRODUCT
/**
 * @tc.name: SetAttributesAndStyles005
 * @tc.desc: The diameter and color of progress are equal to user-defined in waiting state.
 * @tc.type: FUNC
 */
HWTEST_F(DomButtonTest, SetAttributesAndStyles005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create json string for testing, the type is circle, the waiting is true,
     *                   diameter and progress color style defined.
     */
    const std::string jsonButtonStr = ""
                                      "{                                                        "
                                      "  \"tag\": \"button\",                                   "
                                      "  \"attr\":  [{  \"type\" : \"circle\"             },    "
                                      "              {  \"waiting\" : \"true\"            }],   "
                                      "  \"style\": [{  \"radius\" : \"50\"               },    "
                                      "              {  \"progressColor\" : \"#00ff00\"   }]    "
                                      "}";

    /**
     * @tc.steps: step2. Verify whether diameter and color of progress are equal to user-defined.
     * @tc.expected: step2. The diameter and color of progress are equal to user-defined.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonButtonStr);
    ASSERT_TRUE(domNodeRoot);
    auto buttonComponent = AceType::DynamicCast<ButtonComponent>(domNodeRoot->GetSpecializedComponent());
    ASSERT_TRUE(buttonComponent);
    auto children = buttonComponent->GetChildren();
    ASSERT_TRUE(children.size() > 0);
    auto paddingChild = AceType::DynamicCast<PaddingComponent>(children.front());
    ASSERT_TRUE(paddingChild);
    auto progressChild = AceType::DynamicCast<LoadingProgressComponent>(paddingChild->GetChild());
    ASSERT_TRUE(progressChild);
    ASSERT_TRUE(NearEqual(progressChild->GetDiameter().Value(), PROGRESS_DIAMETER_TEST));
    ASSERT_TRUE(progressChild->GetProgressColor() == Color(PROGRESS_COLOR_TEST));
}
#endif

/**
 * @tc.name: SetAttributesAndStyles006
 * @tc.desc: The attributes and styles are equal to user-defined.
 * @tc.type: FUNC
 */
HWTEST_F(DomButtonTest, SetAttributesAndStyles006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create json string for testing.
     */
    const std::string jsonButtonStr = ""
                                      "{                                                     "
                                      "  \"tag\": \"button\",                                "
                                      "  \"attr\":  [{  \"value\" : \"test\"           },    "
                                      "              {  \"autofocus\" : \"true\"       }],   "
                                      "  \"style\": [{  \"width\" : \"200\"            },    "
                                      "              {  \"height\" : \"100\"           },    "
                                      "              {  \"clickedColor\" : \"#00aa00\" },    "
                                      "              {  \"focusColor\" : \"#00bb00\"   },    "
                                      "              {  \"textColor\" : \"#00cc00\"    },    "
                                      "              {  \"fontSize\" : \"40\"          },    "
                                      "              {  \"fontWeight\" : \"bolder\"    },    "
                                      "              {  \"borderWidth\" : \"3\"        },    "
                                      "              {  \"borderColor\" : \"#00dd00\"  }]    "
                                      "}";

    /**
     * @tc.steps: step2. Verify whether attributes and styles are equal to default values.
     * @tc.expected: step2. All attribute and style values are equal to the default from theme.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonButtonStr);
    ASSERT_TRUE(domNodeRoot);
    auto buttonComponent = AceType::DynamicCast<ButtonComponent>(domNodeRoot->GetSpecializedComponent());
    ASSERT_TRUE(buttonComponent);
    ASSERT_TRUE(buttonComponent->GetAutoFocusState());
    ASSERT_TRUE(buttonComponent->GetWidth() == Dimension(WIDTH_TEST));
    ASSERT_TRUE(buttonComponent->GetHeight() == Dimension(HEIGHT_TEST));
    ASSERT_TRUE(buttonComponent->GetClickedColor() == Color(CLICKED_COLOR_TEST));
    ASSERT_TRUE(buttonComponent->GetFocusColor() == Color(FOCUS_COLOR_TEST));
    ASSERT_TRUE(buttonComponent->GetBorderEdge().GetColor() == Color(BORDER_COLOR_TEST));
    ASSERT_TRUE(NearEqual(buttonComponent->GetBorderEdge().GetWidth().Value(), BORDER_WIDTH_TEST));

    auto children = buttonComponent->GetChildren();
    ASSERT_TRUE(children.size() > 0);
    auto paddingChild = AceType::DynamicCast<PaddingComponent>(children.front());
    ASSERT_TRUE(paddingChild);
    auto textChild = AceType::DynamicCast<TextComponent>(paddingChild->GetChild());
    ASSERT_TRUE(textChild);
    auto textStyle = textChild->GetTextStyle();
    ASSERT_TRUE(textStyle.GetTextColor() == Color(TEXT_COLOR_TEST));
    ASSERT_TRUE(textStyle.GetFontSize() == Dimension(FONT_SIZE_TEST));
    ASSERT_TRUE(textStyle.GetFontWeight() == FontWeight::BOLDER);
}

} // namespace OHOS::Ace::Framework
