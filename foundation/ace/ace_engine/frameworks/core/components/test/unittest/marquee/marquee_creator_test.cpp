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

#include "core/components/marquee/marquee_component.h"
#include "core/components/test/json/marquee_creator.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const std::string DSL_VALUE = "testValue";
const Color DSL_TEXT_COLOR = Color(0xff000000);
const Dimension DSL_FONT_SIZE = Dimension(30.0, DimensionUnit::PX);
const FontWeight DSL_FONT_WEIGHT = FontWeight::W100;
const std::vector<std::string> DSL_FONT_FAMILIES = { "normal", "sans-serif", "serif", "monospace" };
constexpr double DSL_SCROLL_AMOUNT = 10.0;
constexpr int32_t DSL_LOOP = 3;
const MarqueeDirection DSL_DIRECTION = MarqueeDirection::RIGHT;
const std::string DSL_BOUNCE = "onBounceEventId";
const std::string DSL_FINISH = "onFinishEventId";
const std::string DSL_START = "onStartEventId";
const std::vector<std::string> DEFAULT_FONT_FAMILIES = {};
const Dimension DEFAULT_MARQUEE_FONT_SIZE = Dimension(37.5, DimensionUnit::PX);

RefPtr<MarqueeComponent> CreateMarqueeComponent(const std::string& json)
{
    const auto buffer = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t length = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(buffer, length);
    return AceType::DynamicCast<MarqueeComponent>(component);
}

void CheckMarqueeDefaultValue(const RefPtr<MarqueeComponent>& marquee)
{
    ASSERT_TRUE(marquee);
    EXPECT_TRUE(marquee->GetValue().empty());

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

} // namespace

class MarqueeCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void MarqueeCreatorTest::SetUpTestCase() {}
void MarqueeCreatorTest::TearDownTestCase() {}
void MarqueeCreatorTest::SetUp() {}
void MarqueeCreatorTest::TearDown() {}

/**
 * @tc.name: MarqueeCreator001
 * @tc.desc: Test parse marquee component successfully from dsl that contains all attributes.
 * @tc.type: FUNC
 * @tc.require: AR000DAVDJ AR000DAVDK AR000DAVDL AR000DAVDM AR000DAVDN
 * @tc.author: liruokun
 */
HWTEST_F(MarqueeCreatorTest, MarqueeCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with each attribute, create marquee component with the json.
     * @tc.expected: step1. Marquee component successfully created, each attribute equals with the value in json.
     */
    std::string dslJson = "{                                            "
                          "  \"className\": \"Marquee\",                "
                          "  \"value\": \"testValue\",                  "
                          "  \"scrollAmount\": 10,                      "
                          "  \"loop\": 3,                               "
                          "  \"direction\": \"right\",                  "
                          "  \"bounce\": \"onBounceEventId\",           "
                          "  \"finish\": \"onFinishEventId\",           "
                          "  \"start\": \"onStartEventId\",             "
                          "  \"textStyle\": {                           "
                          "      \"className\": \"TextStyle\",          "
                          "      \"fontSize\": 30,                      "
                          "      \"fontWeight\": 0,                     "
                          "      \"fontFamilies\": [                    "
                          "          \"normal\",                        "
                          "          \"sans-serif\",                    "
                          "          \"serif\",                         "
                          "          \"monospace\"                      "
                          "        ],                                   "
                          "      \"color\": {                           "
                          "          \"className\": \"Color\",          "
                          "          \"value\": 4278190080              "
                          "       }                                     "
                          "   }                                         "
                          "}                                            ";

    RefPtr<MarqueeComponent> marquee = CreateMarqueeComponent(dslJson);
    ASSERT_TRUE(marquee);
    EXPECT_EQ(marquee->GetValue(), DSL_VALUE);

    const auto& textStyle = marquee->GetTextStyle();
    EXPECT_EQ(textStyle.GetTextColor().GetValue(), DSL_TEXT_COLOR.GetValue());
    EXPECT_EQ(textStyle.GetFontSize().Unit(), DSL_FONT_SIZE.Unit());
    EXPECT_TRUE(NearEqual(textStyle.GetFontSize().Value(), DSL_FONT_SIZE.Value()));
    EXPECT_EQ(textStyle.GetFontWeight(), DSL_FONT_WEIGHT);
    EXPECT_EQ(textStyle.GetFontFamilies(), DSL_FONT_FAMILIES);

    EXPECT_TRUE(NearEqual(marquee->GetScrollAmount(), DSL_SCROLL_AMOUNT));
    EXPECT_EQ(marquee->GetLoop(), DSL_LOOP);
    EXPECT_EQ(marquee->GetDirection(), DSL_DIRECTION);
    EXPECT_EQ(marquee->GetBounceEventId(), DSL_BOUNCE);
    EXPECT_EQ(marquee->GetFinishEventId(), DSL_FINISH);
    EXPECT_EQ(marquee->GetStartEventId(), DSL_START);
}

/**
 * @tc.name: MarqueeCreator002
 * @tc.desc: Test parse marquee successfully from dsl json with no attributes defined.
 * @tc.type: FUNC
 * @tc.require: AR000DAVDJ AR000DAVDK AR000DAVDL AR000DAVDM AR000DAVDN
 * @tc.author: liruokun
 */
HWTEST_F(MarqueeCreatorTest, MarqueeCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct json string with no attributes, then create marquee component with the json.
     * @tc.expected: step1. Marquee component successfully created, each attribute equals default value.
     */
    std::string dslJson = "{ \"className\": \"Marquee\" }";
    RefPtr<MarqueeComponent> marquee = CreateMarqueeComponent(dslJson);
    CheckMarqueeDefaultValue(marquee);
}

/**
 * @tc.name: MarqueeCreator003
 * @tc.desc: Test parse marquee from dsl json with illegal attributes.
 * @tc.type: FUNC
 * @tc.require: AR000DAVDJ AR000DAVDK AR000DAVDL AR000DAVDM AR000DAVDN
 * @tc.author: liruokun
 */
HWTEST_F(MarqueeCreatorTest, MarqueeCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct json with illegal value, then create marquee component with the json.
     * @tc.expected: step1. Marquee component successfully created, each attribute equals default value.
     */
    std::string dslJson = "{                                            "
                          "  \"className\": \"Marquee\",                "
                          "  \"value\": 10,                             "
                          "  \"scrollAmount\": \"scrollAmount\",        "
                          "  \"loop\": \"loop\",                        "
                          "  \"direction\": 10,                         "
                          "  \"bounce\": 10,                            "
                          "  \"finish\": 10,                            "
                          "  \"start\": 10,                             "
                          "  \"textStyle\": \"textStyle\"               "
                          "}                                            ";
    RefPtr<MarqueeComponent> marquee = CreateMarqueeComponent(dslJson);
    CheckMarqueeDefaultValue(marquee);
}

} // namespace OHOS::Ace