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

#include "frameworks/bridge/common/dom/dom_svg_text.h"
#include "frameworks/bridge/common/dom/dom_svg_text_path.h"
#include "frameworks/bridge/common/dom/dom_svg_tspan.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"
#include "frameworks/core/components/svg/svg_tspan_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const std::string TEXT_DATA = "test1234@?!";
const std::string SVG_LENGTH_ADJUST_VALUE = "spacingAndGlyphs";
const std::string SVG_PATH_VALUE = "M40,1160 Q360,1160 360,980 Q360,840 200,840 Q40,"
                                    "840 40,960 Q40,1080 180,1080 Q280,1080 300,1000";
const Dimension SVG_TEXT_FONT_SIZE = Dimension(50.0);
const Dimension SVG_OFFSET_X = Dimension(10.0);
const Dimension SVG_OFFSET_Y = Dimension(20.0);
const Dimension SVG_OFFSET_DX = Dimension(30.0);
const Dimension SVG_OFFSET_DY = Dimension(40.0);
const Dimension SVG_START_OFFSET_VALUE = Dimension(50.0);
const Dimension SVG_STROKE_WIDTH_VALUE = Dimension(2.0);
const Color SVG_FILL_COLOR = Color::FromString("#0x0000ff");
const Color SVG_STROKE_COLOR = Color::FromString("#0x0000ee");
constexpr double SVG_ROTATE_VALUE = 10.0;
constexpr double SVG_FILL_OPACITY_VALUE = 0.5;
constexpr double SVG_STROKE_OPACITY_VALUE = 0.6;

const std::string JSON_SVG_TEXT_TAG = ""
                                      "{                                         "
                                      "  \"tag\": \"svg-text\",                      ";
const std::string JSON_SVG_TSPAN_TAG = ""
                                       "{                                         "
                                       "  \"tag\": \"tspan\",                      ";
const std::string JSON_SVG_TEXT_STR = "  \"attr\": [{                           "
                                    "             \"value\":  \"test1234@?!\"  "
                                    "          },"
                                    "          { "
                                    "           \"fontSize\":\"50.0\"          "
                                    "          },                              "
                                    "          {"
                                    "           \"x\":\"10\"         "
                                    "           },"
                                    "          {"
                                    "           \"y\":\"20\"         "
                                    "           },"
                                    "          {"
                                    "           \"dx\":\"30\"         "
                                    "           },"
                                    "          {"
                                    "           \"dy\":\"40\"         "
                                    "           },"
                                    "          { "
                                    "           \"rotate\":\"10.0\"          "
                                    "          },                            "
                                    "          { "
                                    "           \"TextLength\":\"100.0\"     "
                                    "          },"
                                    "          { "
                                    "           \"LengthAdjust\":\"spacingAndGlyphs\"     "
                                    "          },"
                                    "          {"
                                    "           \"fill\":\"#0x0000ff\"        "
                                    "           },"
                                    "          {"
                                    "           \"fillOpacity\":\"0.5\"        "
                                    "           },"
                                    "          {"
                                    "           \"stroke\":\"0x0000ee\"       "
                                    "           },"
                                    "          {"
                                    "           \"strokeWidth\":\"2\"         "
                                    "           },"
                                    "           { "
                                    "           \"strokeOpacity\":\"0.6\"       "
                                    "            }]"
                                    "}";

const std::string JSON_SVG_TEXT_PATH_STR = ""
                                    "{                                         "
                                    "  \"tag\": \"textpath\",                      "
                                    "  \"attr\": [{                           "
                                    "             \"value\":  \"test1234@?!\"  "
                                    "          },"
                                    "          { "
                                    "           \"fontSize\":\"50.0\"          "
                                    "          },                              "
                                    "          { "
                                    "           \"startOffset\":\"50.0\"          "
                                    "          },                              "
                                    "          { "
                                    "           \"path\":\"M40,1160 Q360,1160 360,980 Q360,"
                                    "840 200,840 Q40,840 40,960 Q40,1080 180,1080 Q280,1080 300,1000\""
                                    "          },                            "
                                    "          { "
                                    "           \"TextLength\":\"100.0\"     "
                                    "          },"
                                    "          { "
                                    "           \"LengthAdjust\":\"spacingAndGlyphs\"     "
                                    "          },"
                                    "          {"
                                    "           \"fill\":\"#0x0000ff\"        "
                                    "           },"
                                    "          {"
                                    "           \"fillOpacity\":\"0.5\"        "
                                    "           },"
                                    "          {"
                                    "           \"stroke\":\"0x0000ee\"       "
                                    "           },"
                                    "          {"
                                    "           \"strokeWidth\":\"2\"         "
                                    "           },"
                                    "           { "
                                    "           \"strokeOpacity\":\"0.6\"       "
                                    "            }]"
                                    "}";
} // namespace

class DomSvgTextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomSvgTextTest::SetUpTestCase() {}
void DomSvgTextTest::TearDownTestCase() {}
void DomSvgTextTest::SetUp() {}
void DomSvgTextTest::TearDown() {}

/**
 * @tc.name: DomSvgTextTest001
 * @tc.desc: Verify that DomSvgText can be created.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTextTest, DomSvgTextTest001, TestSize.Level1)
{
    const std::string jsonTextStr = ""
                                    "{                       "
                                    "  \"tag\": \"svg-text\" "
                                    "}";

    /**
     * @tc.steps: step1. call JsonUtil interface and create DomSvgText.
     */
    auto domNode = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonTextStr);
    RefPtr<DOMSvgText> svgText = AceType::DynamicCast<DOMSvgText>(domNode);

    /**
     * @tc.steps: step2. Verify whether the DomSvgText.
     * @tc.expected: step2. DomSvgText is not null.
     */
    ASSERT_TRUE(svgText != nullptr);
}

/**
 * @tc.name: DomSvgTextTest002
 * @tc.desc: Verify that DomSvgText can be set styles.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTextTest, DomSvgTextTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of DomSvgText with all attributes.
     */
    const std::string& jsonTextStr = JSON_SVG_TEXT_TAG + JSON_SVG_TEXT_STR;

    /**
     * @tc.steps: step2. call JsonUtil interface, create DomSvgText and set its attr.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonTextStr);
    ASSERT_TRUE(domNodeRoot != nullptr);
    RefPtr<SvgTextComponent> svgText =
            AceType::DynamicCast<SvgTextComponent>(domNodeRoot->GetSpecializedComponent());
    ASSERT_TRUE(svgText != nullptr);
    /**
     * @tc.steps: step3. Check all the attributes matched.
     * @tc.expected: step3. All the attributes are matched.
     */
    EXPECT_EQ(svgText->GetTextData(), TEXT_DATA);
    EXPECT_EQ(svgText->GetX().Value(), SVG_OFFSET_X.Value());
    EXPECT_EQ(svgText->GetY().Value(), SVG_OFFSET_Y.Value());
    EXPECT_EQ(svgText->GetDx().Value(), SVG_OFFSET_DX.Value());
    EXPECT_EQ(svgText->GetDy().Value(), SVG_OFFSET_DY.Value());
    EXPECT_EQ(svgText->GetRotate(), SVG_ROTATE_VALUE);
    EXPECT_EQ(svgText->GetDeclaration()->GetSvgTextStyle().GetFontSize().Value(), SVG_TEXT_FONT_SIZE.Value());
    EXPECT_EQ(svgText->GetDeclaration()->GetFillState().GetColor().GetValue(), SVG_FILL_COLOR.GetValue());
    EXPECT_EQ(svgText->GetDeclaration()->GetFillState().GetOpacity().GetValue(), SVG_FILL_OPACITY_VALUE);
    EXPECT_EQ(svgText->GetDeclaration()->GetStrokeState().GetColor().GetValue(), SVG_STROKE_COLOR.GetValue());
    EXPECT_EQ(svgText->GetDeclaration()->GetStrokeState().GetOpacity().GetValue(), SVG_STROKE_OPACITY_VALUE);
    EXPECT_EQ(svgText->GetDeclaration()->GetStrokeState().GetLineWidth().Value(), SVG_STROKE_WIDTH_VALUE.Value());
}

/**
 * @tc.name: DomSvgTspanTest001
 * @tc.desc: Verify that DomSvgTspan can be created.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTextTest, DomSvgTspanTest001, TestSize.Level1)
{
    const std::string jsonTextStr = ""
                                    "{                       "
                                    "  \"tag\": \"tspan\" "
                                    "}";

    /**
     * @tc.steps: step1. call JsonUtil interface and create DomSvgTspan.
     */
    auto domNode = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonTextStr);
    RefPtr<DOMSvgTspan> svgTspan = AceType::DynamicCast<DOMSvgTspan>(domNode);

    /**
     * @tc.steps: step2. Verify whether the DomSvgTspan.
     * @tc.expected: step2. DomSvgTspan is not null.
     */
    ASSERT_TRUE(svgTspan != nullptr);
}

/**
 * @tc.name: DomSvgTspanTest002
 * @tc.desc: Verify that DomSvgTspan can be set styles.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTextTest, DomSvgTspanTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of DomSvgTspan with all attributes.
     */
    const std::string& jsonTextStr = JSON_SVG_TSPAN_TAG + JSON_SVG_TEXT_STR;

    /**
     * @tc.steps: step2. call JsonUtil interface, create DomSvgTspan and set its attr.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonTextStr);
    ASSERT_TRUE(domNodeRoot != nullptr);

    RefPtr<SvgTspanComponent> svgTspan =
            AceType::DynamicCast<SvgTspanComponent>(domNodeRoot->GetSpecializedComponent());
    ASSERT_TRUE(svgTspan != nullptr);

    /**
     * @tc.steps: step3. Check all the attributes matched.
     * @tc.expected: step3. All the attributes are matched.
     */
    EXPECT_EQ(svgTspan->GetTextData(), TEXT_DATA);
    EXPECT_EQ(svgTspan->GetX().Value(), SVG_OFFSET_X.Value());
    EXPECT_EQ(svgTspan->GetY().Value(), SVG_OFFSET_Y.Value());
    EXPECT_EQ(svgTspan->GetDx().Value(), SVG_OFFSET_DX.Value());
    EXPECT_EQ(svgTspan->GetDy().Value(), SVG_OFFSET_DY.Value());
    EXPECT_EQ(svgTspan->GetRotate(), SVG_ROTATE_VALUE);
    EXPECT_EQ(svgTspan->GetDeclaration()->GetSvgTextStyle().GetFontSize().Value(), SVG_TEXT_FONT_SIZE.Value());
    EXPECT_EQ(svgTspan->GetDeclaration()->GetFillState().GetColor().GetValue(), SVG_FILL_COLOR.GetValue());
    EXPECT_EQ(svgTspan->GetDeclaration()->GetFillState().GetOpacity().GetValue(), SVG_FILL_OPACITY_VALUE);
    EXPECT_EQ(svgTspan->GetDeclaration()->GetStrokeState().GetColor().GetValue(), SVG_STROKE_COLOR.GetValue());
    EXPECT_EQ(svgTspan->GetDeclaration()->GetStrokeState().GetOpacity().GetValue(), SVG_STROKE_OPACITY_VALUE);
    EXPECT_EQ(svgTspan->GetDeclaration()->GetStrokeState().GetLineWidth().Value(), SVG_STROKE_WIDTH_VALUE.Value());
}

/**
 * @tc.name: DomSvgTextPathTest001
 * @tc.desc: Verify that DomSvgTextPath can be created.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTextTest, DomSvgTextPathTest001, TestSize.Level1)
{
    const std::string jsonTextStr = ""
                                    "{                       "
                                    "  \"tag\": \"textpath\" "
                                    "}";

    /**
     * @tc.steps: step1. call JsonUtil interface and create DomSvgTextPath.
     */
    auto domNode = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonTextStr);
    RefPtr<DOMSvgTextPath> svgTextPath = AceType::DynamicCast<DOMSvgTextPath>(domNode);

    /**
     * @tc.steps: step2. Verify whether the DOMSvgTextPath.
     * @tc.expected: step2. DOMSvgTextPath is not null.
     */
    ASSERT_TRUE(svgTextPath != nullptr);
}

/**
 * @tc.name: DomSvgTextPathTest002
 * @tc.desc: Verify that DomSvgTextPath can be set styles.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTextTest, DomSvgTextPathTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of DomSvgTextPath with all attributes.
     */
    const std::string& jsonTextStr = JSON_SVG_TEXT_PATH_STR;

    /**
     * @tc.steps: step2. call JsonUtil interface, create DomSvgTextPath and set its attr.
     */
    auto domNode = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonTextStr);
    ASSERT_TRUE(domNode != nullptr);
    RefPtr<SvgTextPathComponent> svgTextPath =
            AceType::DynamicCast<SvgTextPathComponent>(domNode->GetSpecializedComponent());
    ASSERT_TRUE(svgTextPath != nullptr);

    /**
     * @tc.steps: step3. Check all the attributes matched.
     * @tc.expected: step3. All the attributes are matched.
     */
    EXPECT_EQ(svgTextPath->GetTextData(), TEXT_DATA);
    EXPECT_EQ(svgTextPath->GetPath(), SVG_PATH_VALUE);
    EXPECT_EQ(svgTextPath->GetStartOffset().Value(), SVG_START_OFFSET_VALUE.Value());
    EXPECT_EQ(svgTextPath->GetDeclaration()->GetSvgTextStyle().GetFontSize().Value(), SVG_TEXT_FONT_SIZE.Value());
    EXPECT_EQ(svgTextPath->GetDeclaration()->GetFillState().GetColor().GetValue(), SVG_FILL_COLOR.GetValue());
    EXPECT_EQ(svgTextPath->GetDeclaration()->GetFillState().GetOpacity().GetValue(), SVG_FILL_OPACITY_VALUE);
    EXPECT_EQ(svgTextPath->GetDeclaration()->GetStrokeState().GetColor().GetValue(), SVG_STROKE_COLOR.GetValue());
    EXPECT_EQ(svgTextPath->GetDeclaration()->GetStrokeState().GetOpacity().GetValue(), SVG_STROKE_OPACITY_VALUE);
    EXPECT_EQ(svgTextPath->GetDeclaration()->GetStrokeState().GetLineWidth().Value(), SVG_STROKE_WIDTH_VALUE.Value());
}

} // namespace OHOS::Ace::Framework
