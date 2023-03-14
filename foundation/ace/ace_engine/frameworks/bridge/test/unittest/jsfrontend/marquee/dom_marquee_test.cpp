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

#include "core/components/common/layout/constants.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/common/dom/dom_marquee.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const std::string CUSTOM_VALUE = "testValue";
const Color CUSTOM_TEXT_COLOR = Color(0xffff8888);
const Dimension CUSTOM_FONT_SIZE = Dimension(50.0, DimensionUnit::PX);
const FontWeight CUSTOM_FONT_WEIGHT = FontWeight::BOLDER;
const std::vector<std::string> CUSTOM_FONT_FAMILIES = { "serif" };
constexpr double CUSTOM_SCROLL_AMOUNT = 30.0;
constexpr int32_t CUSTOM_LOOP = 3;
const MarqueeDirection CUSTOM_DIRECTION = MarqueeDirection::RIGHT;
const std::vector<std::string> DEFAULT_FONT_FAMILIES = {};
const Dimension DEFAULT_MARQUEE_FONT_SIZE = Dimension(37.5, DimensionUnit::PX);

const std::string MARQUEE_JS_CUSTOM = ""
                                      "{                                                               "
                                      "  \"tag\": \"marquee\",                                         "
                                      "  \"attr\": [{                                                  "
                                      "                \"scrollamount\" : \"30px\"                     "
                                      "              },                                                "
                                      "              {"
                                      "                \"direction\" : \"right\"                       "
                                      "              },                                                "
                                      "              {"
                                      "                \"loop\" : \"3\"                                "
                                      "              },                                                "
                                      "              {"
                                      "                \"value\" : \"testValue\"                       "
                                      "              }],                                               "
                                      "  \"style\":  [{ "
                                      "                \"fontSize\" : \"50px\"                         "
                                      "              },                                                "
                                      "              { "
                                      "                \"color\" : \"#ff8888\" "
                                      "              },                                                "
                                      "              {"
                                      "                \"fontWeight\" : \"bolder\"                     "
                                      "              },                                                "
                                      "              {"
                                      "                \"fontFamily\" : \"serif\"                      "
                                      "              }],"
                                      "  \"event\":  [ \"bounce\", \"finish\", \"start\" ]             "
                                      "}";

const std::string MARQUEE_JS_DEFAULT = ""
                                       "{                                 "
                                       "  \"tag\": \"marquee\"            "
                                       "}";

} // namespace

class DomMarqueeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomMarqueeTest::SetUpTestCase() {}
void DomMarqueeTest::TearDownTestCase() {}
void DomMarqueeTest::SetUp() {}
void DomMarqueeTest::TearDown() {}

/**
 * @tc.name: DomMarqueeCreateFromDsl001
 * @tc.desc: Test DOMMarquee can be created with custom attributes.
 * @tc.type: FUNC
 */
HWTEST_F(DomMarqueeTest, DomMarqueeCreateFromDsl001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with custom fields, create marquee node with it.
     * @tc.expected: step1. Marquee node and marquee component are created successfully.
     */
    auto domNode = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(MARQUEE_JS_CUSTOM);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNode);
    RefPtr<MarqueeComponent> marquee = AceType::DynamicCast<MarqueeComponent>(boxChild->GetChild());
    ASSERT_TRUE(marquee);

    /**
     * @tc.steps: step2. Check styles and attributes of MarqueeComponent.
     * @tc.expected: step2. The styles and attributes are same as defined in js dsl.
     */
    EXPECT_EQ(marquee->GetValue(), CUSTOM_VALUE);

    const auto& textStyle = marquee->GetTextStyle();
    EXPECT_EQ(textStyle.GetTextColor().GetValue(), CUSTOM_TEXT_COLOR.GetValue());
    EXPECT_EQ(textStyle.GetFontSize().Unit(), CUSTOM_FONT_SIZE.Unit());
    EXPECT_TRUE(NearEqual(textStyle.GetFontSize().Value(), CUSTOM_FONT_SIZE.Value()));
    EXPECT_EQ(textStyle.GetFontWeight(), CUSTOM_FONT_WEIGHT);
    EXPECT_EQ(textStyle.GetFontFamilies(), CUSTOM_FONT_FAMILIES);

    EXPECT_TRUE(NearEqual(marquee->GetScrollAmount(), CUSTOM_SCROLL_AMOUNT));
    EXPECT_EQ(marquee->GetLoop(), CUSTOM_LOOP);
    EXPECT_EQ(marquee->GetDirection(), CUSTOM_DIRECTION);
    EXPECT_EQ(marquee->GetBounceEventId(), std::to_string(domNode->GetNodeId()));
    EXPECT_EQ(marquee->GetFinishEventId(), std::to_string(domNode->GetNodeId()));
    EXPECT_EQ(marquee->GetStartEventId(), std::to_string(domNode->GetNodeId()));
}

/**
 * @tc.name: DomMarqueeCreateFromDsl002
 * @tc.desc: Test DOMMarquee can be created with default attributes.
 * @tc.type: FUNC
 */
HWTEST_F(DomMarqueeTest, DomMarqueeCreateFromDsl002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with default fields, create marquee node with it.
     * @tc.expected: step1. Marquee node and marquee component are created successfully.
     */
    auto domNode = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(MARQUEE_JS_DEFAULT);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNode);
    RefPtr<MarqueeComponent> marquee = AceType::DynamicCast<MarqueeComponent>(boxChild->GetChild());
    ASSERT_TRUE(marquee);

    /**
     * @tc.steps: step2. Check styles and attributes of MarqueeComponent.
     * @tc.expected: step2. The styles and attributes are same as defined in js dsl.
     */
    EXPECT_EQ(marquee->GetValue(), "");

    const auto& textStyle = marquee->GetTextStyle();
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    auto themeConstants = themeManager->GetThemeConstants();
    ASSERT_NE(themeConstants, nullptr);
    EXPECT_EQ(textStyle.GetTextColor().GetValue(), themeConstants->GetColor(THEME_OHOS_COLOR_TEXT_PRIMARY).GetValue());
    EXPECT_EQ(textStyle.GetFontSize().Unit(), DEFAULT_MARQUEE_FONT_SIZE.Unit());
    EXPECT_TRUE(NearEqual(textStyle.GetFontSize().Value(), DEFAULT_MARQUEE_FONT_SIZE.Value()));
    EXPECT_EQ(textStyle.GetFontWeight(), FontWeight::NORMAL);
    EXPECT_EQ(textStyle.GetFontFamilies(), DEFAULT_FONT_FAMILIES);

    EXPECT_TRUE(NearEqual(marquee->GetScrollAmount(), DEFAULT_MARQUEE_SCROLL_AMOUNT));
    EXPECT_EQ(marquee->GetLoop(), DEFAULT_MARQUEE_LOOP);
    EXPECT_EQ(marquee->GetDirection(), MarqueeDirection::LEFT);
    EXPECT_TRUE(marquee->GetBounceEventId().IsEmpty());
    EXPECT_TRUE(marquee->GetFinishEventId().IsEmpty());
    EXPECT_TRUE(marquee->GetStartEventId().IsEmpty());
}

} // namespace OHOS::Ace::Framework
