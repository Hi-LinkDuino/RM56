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
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/common/dom/dom_span.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const std::string SPAN_DATA = "test1234@?!";
constexpr double TEST_FONT_SIZE = 50.0;
constexpr int32_t TEST_FONT_WEIGHT = 1;
constexpr int32_t TEST_FONT_STYLE = 1;
constexpr int32_t TEST_TEXT_DECORATION = 2;
const std::string TEST_FONT_FAMILY = "serif";

constexpr int32_t DEFAULT_FONT_WEIGHT = 10;
constexpr int32_t DEFAULT_FONT_STYLE = 0;
constexpr int32_t DEFAULT_TEXT_DECORATION = 0;
constexpr uint32_t TEST_FONT_FEATURES_SIZE = 6;
constexpr uint32_t TEST_FONT_FEATURES_NONE_SIZE = 5;

} // namespace

class DomSpanTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DomSpanTest::SetUpTestCase() {}
void DomSpanTest::TearDownTestCase() {}
void DomSpanTest::SetUp() {}
void DomSpanTest::TearDown() {}

/**
 * @tc.name: DomSpanTest001
 * @tc.desc: Verify that DomSpan can be created.
 * @tc.type: FUNC
 */
HWTEST_F(DomSpanTest, DomSpanTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call JsonUtil interface and create DomSpan.
     */
    const std::string jsonSpanStr = ""
                                    "{                                 "
                                    "  \"tag\": \"span\"               "
                                    "}";

    /**
     * @tc.steps: step2. call JsonUtil interface and create DomSpan.
     */
    auto domNodeSpan = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonSpanStr);

    /**
     * @tc.steps: step3. Verify whether the DomSpan.
     * @tc.expected: step3. DomSpan is not null.
     */
    ASSERT_TRUE(domNodeSpan != nullptr);
}

/**
 * @tc.name: DomSpanTest002
 * @tc.desc: Verify that DomSpan can be set styles.
 * @tc.type: FUNC
 */
HWTEST_F(DomSpanTest, DomSpanTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of DomSpan with all attributes.
     */
    const std::string jsonSpanStr = ""
                                    "{                                             "
                                    "  \"tag\": \"span\",                          "
                                    "  \"attr\" : [{                               "
                                    "           \"value\" : \"test1234@?!\"      "
                                    "            }],                               "
                                    "  \"style\": [{                               "
                                    "           \"fontSize\":\"50.0\"              "
                                    "          },                                  "
                                    "          {                                   "
                                    "           \"fontWeight\":\"200\"             "
                                    "           },                                 "
                                    "          {                                   "
                                    "           \"color\":\"#000000ff\"            "
                                    "           },                                 "
                                    "          {                                   "
                                    "           \"fontStyle\":\"italic\"           "
                                    "           },                                 "
                                    "          {                                   "
                                    "           \"textDecoration\":\"overline\"    "
                                    "           },                                 "
                                    "           {                                                                  "
                                    "           \"fontVariant\":\"small-caps slashed-zero common-ligatures ruby    "
                                    "stylistic(2) \"                                                               "
                                    "           },                                 "
                                    "           {                                  "
                                    "           \"fontFamily\":\"serif\"           "
                                    "            }]                                "
                                    "}";

    /**
     * @tc.steps: step2. call JsonUtil interface, create DomSpan and set its style.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonSpanStr);
    RefPtr<TextSpanComponent> spanChild =
        AceType::DynamicCast<TextSpanComponent>(domNodeRoot->GetSpecializedComponent());
    const auto& spanStyle = spanChild->GetTextStyle();

    /**
     * @tc.steps: step3. Check all the attributes matched.
     * @tc.expected: step3. All the attributes are matched.
     */
    EXPECT_EQ(spanChild->GetSpanData(), SPAN_DATA);
    EXPECT_TRUE(NearEqual(spanStyle.GetFontSize().Value(), TEST_FONT_SIZE));
    EXPECT_EQ(static_cast<int32_t>(spanStyle.GetFontWeight()), TEST_FONT_WEIGHT);
    EXPECT_EQ(spanStyle.GetTextColor(), Color::FromString("#000000ff"));
    EXPECT_TRUE(NearEqual(static_cast<int32_t>(spanStyle.GetFontStyle()), TEST_FONT_STYLE));
    EXPECT_TRUE(NearEqual(static_cast<int32_t>(spanStyle.GetTextDecoration()), TEST_TEXT_DECORATION));
    EXPECT_EQ(spanStyle.GetFontFamilies()[0], TEST_FONT_FAMILY);
    EXPECT_EQ(spanStyle.GetFontFeatures().size(), TEST_FONT_FEATURES_SIZE);
}

/**
 * @tc.name: DomSpanTest003
 * @tc.desc: Verify that DomSpan can be set styles to the default values when styles' attributes are invalid.
 * @tc.type: FUNC
 */
HWTEST_F(DomSpanTest, DomSpanTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of DomSpan with invalid attributes.
     */
    const std::string jsonSpanStr = ""
                                    "{                                                 "
                                    "  \"tag\": \"span\",                              "
                                    "  \"style\": [{                                   "
                                    "           \"fontWeight\":\"invalidValue\"        "
                                    "           },                                     "
                                    "          {                                       "
                                    "           \"fontStyle\":\"invalidValue\"         "
                                    "           },                                     "
                                    "          {                                       "
                                    "           \"fontVariant\":\"invalidValue\"       "
                                    "           },                                     "
                                    "          {                                       "
                                    "           \"textDecoration\":\"invalidValue\"    "
                                    "           }]                                     "
                                    "}";

    /**
     * @tc.steps: step2. call JsonUtil interface, create DomSpan and set attributes' value.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonSpanStr);
    RefPtr<TextSpanComponent> spanChild =
        AceType::DynamicCast<TextSpanComponent>(domNodeRoot->GetSpecializedComponent());
    const auto& spanStyle = spanChild->GetTextStyle();

    /**
     * @tc.steps: step3. Check all the attributes matched with default values.
     * @tc.expected: step3. All the attributes are matched.
     */
    EXPECT_EQ(static_cast<int32_t>(spanStyle.GetFontWeight()), DEFAULT_FONT_WEIGHT);
    EXPECT_TRUE(NearEqual(static_cast<int32_t>(spanStyle.GetFontStyle()), DEFAULT_FONT_STYLE));
    EXPECT_TRUE(NearEqual(static_cast<int32_t>(spanStyle.GetTextDecoration()), DEFAULT_TEXT_DECORATION));
    EXPECT_TRUE(spanStyle.GetFontFeatures().empty());
}

/**
 * @tc.name: DomSpanTest004
 * @tc.desc: Verify that DomSpan can be created.
 * @tc.type: FUNC
 */
HWTEST_F(DomSpanTest, DomSpanTest004, TestSize.Level1)
{
    const std::string jsonSpanStr = ""
                                    "{                                          "
                                    "  \"tag\": \"span\",                       "
                                    "  \"style\": [{                            "
                                    "               \"fontVariant\":\"normal\"  "
                                    "            }]                             "
                                    "}";

    /**
     * @tc.steps: step1. call JsonUtil interface and create DomText.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonSpanStr);
    RefPtr<TextSpanComponent> spanChild =
        AceType::DynamicCast<TextSpanComponent>(domNodeRoot->GetSpecializedComponent());
    const auto textStyle = spanChild->GetTextStyle();

    /**
     * @tc.steps: step3. Check font features is correct.
     * @tc.expected: step3. Font featrues is empty.
     */
    EXPECT_TRUE(textStyle.GetFontFeatures().empty());
}

/**
 * @tc.name: DomSpanTest005
 * @tc.desc: Verify that DomSpan can be created.
 * @tc.type: FUNC
 */
HWTEST_F(DomSpanTest, DomSpanTest005, TestSize.Level1)
{
    const std::string jsonSpanStr = ""
                                    "{                                     "
                                    "  \"tag\": \"span\",                  "
                                    "  \"style\": [{                       "
                                    "           \"fontVariant\":\"none\"   "
                                    "            }]                        "
                                    "}";

    /**
     * @tc.steps: step1. call JsonUtil interface and create DomText.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonSpanStr);
    RefPtr<TextSpanComponent> spanChild =
        AceType::DynamicCast<TextSpanComponent>(domNodeRoot->GetSpecializedComponent());
    const auto textStyle = spanChild->GetTextStyle();

    /**
     * @tc.steps: step3. Check font features is correct.
     * @tc.expected: step3. Font feature is correct.
     */
    EXPECT_EQ(textStyle.GetFontFeatures().size(), TEST_FONT_FEATURES_NONE_SIZE);
}

} // namespace OHOS::Ace::Framework
