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
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/common/dom/dom_svg.h"
#include "frameworks/bridge/common/dom/dom_svg_animate.h"
#include "frameworks/bridge/common/dom/dom_svg_animate_motion.h"
#include "frameworks/bridge/common/dom/dom_svg_circle.h"
#include "frameworks/bridge/common/dom/dom_svg_ellipse.h"
#include "frameworks/bridge/common/dom/dom_svg_line.h"
#include "frameworks/bridge/common/dom/dom_svg_path.h"
#include "frameworks/bridge/common/dom/dom_svg_polygon.h"
#include "frameworks/bridge/common/dom/dom_svg_polyline.h"
#include "frameworks/bridge/common/dom/dom_svg_rect.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {

class DomSvgTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void CheckPresentationAttrs(const RefPtr<SvgBaseDeclaration>& baseDeclaration);
};

void DomSvgTest::SetUpTestCase() {}
void DomSvgTest::TearDownTestCase() {}
void DomSvgTest::SetUp() {}
void DomSvgTest::TearDown() {}

void DomSvgTest::CheckPresentationAttrs(const RefPtr<SvgBaseDeclaration>& baseDeclaration)
{
    /**
     * @tc.steps: step1. Check styles and attributes of created node.
     * @tc.expected: step1. The styles and attributes are as expected.
     */
    EXPECT_NEAR(baseDeclaration->GetOpacity(), 0.8, FLT_EPSILON);
    ASSERT_TRUE(baseDeclaration->GetFillState().GetColor() == Color::RED);
    EXPECT_NEAR(baseDeclaration->GetFillState().GetOpacity().GetValue(), 0.5, FLT_EPSILON);
    ASSERT_TRUE(baseDeclaration->GetStrokeState().GetColor() == Color::BLUE);
    EXPECT_NEAR(baseDeclaration->GetStrokeState().GetOpacity().GetValue(), 0.3, FLT_EPSILON);
    EXPECT_NEAR(baseDeclaration->GetStrokeState().GetLineDash().dashOffset, 2.0, FLT_EPSILON);
    ASSERT_TRUE(baseDeclaration->GetStrokeState().GetLineDash().lineDash.size() == 2);
    EXPECT_NEAR(baseDeclaration->GetStrokeState().GetLineDash().lineDash.at(0), 8.0, FLT_EPSILON);
    EXPECT_NEAR(baseDeclaration->GetStrokeState().GetLineDash().lineDash.at(1), 4.0, FLT_EPSILON);
    ASSERT_TRUE(baseDeclaration->GetStrokeState().GetLineCap() == LineCapStyle::BUTT);
    ASSERT_TRUE(baseDeclaration->GetStrokeState().GetLineJoin() == LineJoinStyle::MITER);
    EXPECT_NEAR(baseDeclaration->GetStrokeState().GetMiterLimit(), 2.0, FLT_EPSILON);
}

/**
 * @tc.name: DomSvgTest001
 * @tc.desc: Verify that DomSvg can be created and set attrs correctly.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTest, DomSvgTest001, TestSize.Level1)
{
    const std::string jsonStr = ""
                                "{                                             "
                                "  \"tag\": \"svg\",                           "
                                "  \"attr\": [{                                "
                                "                \"x\" : \"10\"                "
                                "             },                               "
                                "             {                                "
                                "                \"y\" : \"20\"                "
                                "             },                               "
                                "             {                                "
                                "                \"width\" : \"100\"           "
                                "             },                               "
                                "             {                                "
                                "                \"height\" : \"50\"           "
                                "             },                               "
                                "             {                                "
                                "                \"viewbox\" : \"0 0 100 50\"  "
                                "             }]                               "
                                "}";

    /**
     * @tc.steps: step1. Construct string with right fields, then create svg node with it.
     * @tc.expected: step1. svg node are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonStr);
    ASSERT_TRUE(domNodeRoot != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<SvgComponent> svgComponent = AceType::DynamicCast<SvgComponent>(boxChild->GetChild());
    ASSERT_TRUE(svgComponent != nullptr);
    /**
     * @tc.steps: step2. Check styles and attributes of created node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_NEAR(svgComponent->GetX().Value(), 10.0, FLT_EPSILON);
    EXPECT_NEAR(svgComponent->GetY().Value(), 20.0, FLT_EPSILON);
    EXPECT_NEAR(svgComponent->GetWidth().Value(), 100.0, FLT_EPSILON);
    EXPECT_NEAR(svgComponent->GetHeight().Value(), 50.0, FLT_EPSILON);
    ASSERT_TRUE(svgComponent->GetViewBox() == Rect(0.0, 0.0, 100.0, 50.0));
}

/**
 * @tc.name: DomSvgTest002
 * @tc.desc: Verify that DomSvg can be created and set common attrs correctly.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTest, DomSvgTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create svg node with it.
     * @tc.expected: step1. svg node are created successfully.
     */
    const std::string jsonStr = ""
                                "{                                             "
                                "  \"tag\": \"svg\",                           "
                                "  \"attr\": [{                                "
                                "                \"fill\" : \"red\"            "
                                "             },                               "
                                "             {                                "
                                "                \"fillOpacity\" : \"0.5\"     "
                                "             },                               "
                                "             {                                "
                                "                \"stroke\" : \"blue\"         "
                                "             },                               "
                                "             {                                "
                                "                \"strokeWidth\" : \"5\"       "
                                "             },                               "
                                "             {                                "
                                "                \"strokeDasharray\" : \"8 4\" "
                                "             },                               "
                                "             {                                "
                                "                \"strokeDashoffset\" : \"2\"  "
                                "             },                               "
                                "             {                                "
                                "                \"strokeLinecap\" : \"butt\"  "
                                "             },                               "
                                "             {                                "
                                "                \"strokeLinejoin\" : \"miter\""
                                "             },                               "
                                "             {                                "
                                "                \"strokeMiterlimit\" : \"2\"  "
                                "             },                               "
                                "             {                                "
                                "                \"strokeOpacity\" : \"0.3\"   "
                                "             },                               "
                                "             {                                "
                                "                \"opacity\" : \"0.8\"         "
                                "             }]                               "
                                "}";
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonStr);
    ASSERT_TRUE(domNodeRoot != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<SvgComponent> svgComponent = AceType::DynamicCast<SvgComponent>(boxChild->GetChild());
    ASSERT_TRUE(svgComponent != nullptr);
    /**
     * @tc.steps: step2. Check styles and attributes of created svg node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    CheckPresentationAttrs(svgComponent->GetDeclaration());
}

/**
 * @tc.name: DomSvgTest003
 * @tc.desc: Verify that DomSvgRect can be created and set attrs correctly.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTest, DomSvgTest003, TestSize.Level1)
{
    const std::string jsonStr = ""
                                "{                                             "
                                "  \"tag\": \"rect\",                          "
                                "  \"attr\": [{                                "
                                "                \"x\" : \"10\"                "
                                "             },                               "
                                "             {                                "
                                "                \"y\" : \"20\"                "
                                "             },                               "
                                "             {                                "
                                "                \"width\" : \"100\"           "
                                "             },                               "
                                "             {                                "
                                "                \"height\" : \"50\"           "
                                "             },                               "
                                "             {                                "
                                "                \"rx\" : \"5\"                "
                                "             },                               "
                                "             {                                "
                                "                \"ry\" : \"3\"                "
                                "             }]                               "
                                "}";

    /**
     * @tc.steps: step1. Construct string with right fields, then create svg rect node with it.
     * @tc.expected: step1. svg rect node are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonStr);
    ASSERT_TRUE(domNodeRoot != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<SvgRectComponent> component = AceType::DynamicCast<SvgRectComponent>(boxChild->GetChild());
    ASSERT_TRUE(component != nullptr);
    /**
     * @tc.steps: step2. Check styles and attributes of created rect node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_NEAR(component->GetX().Value(), 10.0, FLT_EPSILON);
    EXPECT_NEAR(component->GetY().Value(), 20.0, FLT_EPSILON);
    EXPECT_NEAR(component->GetWidth().Value(), 100.0, FLT_EPSILON);
    EXPECT_NEAR(component->GetHeight().Value(), 50.0, FLT_EPSILON);
    EXPECT_NEAR(component->GetRx().Value(), 5.0, FLT_EPSILON);
    EXPECT_NEAR(component->GetRy().Value(), 3.0, FLT_EPSILON);
}

/**
 * @tc.name: DomSvgTest004
 * @tc.desc: Verify that DomSvgCircle can be created and set attrs correctly.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTest, DomSvgTest004, TestSize.Level1)
{
    const std::string jsonStr = ""
                                "{                                             "
                                "  \"tag\": \"circle\",                        "
                                "  \"attr\": [{                                "
                                "                \"cx\" : \"20\"               "
                                "             },                               "
                                "             {                                "
                                "                \"cy\" : \"30\"               "
                                "             },                               "
                                "             {                                "
                                "                \"r\" : \"50\"                "
                                "             }]                               "
                                "}";

    /**
     * @tc.steps: step1. Construct string with right fields, then create svg circle node with it.
     * @tc.expected: step1. svg circle node are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonStr);
    ASSERT_TRUE(domNodeRoot != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<SvgCircleComponent> component = AceType::DynamicCast<SvgCircleComponent>(boxChild->GetChild());
    ASSERT_TRUE(component != nullptr);
    /**
     * @tc.steps: step2. Check styles and attributes of created circle node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_NEAR(component->GetCx().Value(), 20.0, FLT_EPSILON);
    EXPECT_NEAR(component->GetCy().Value(), 30.0, FLT_EPSILON);
    EXPECT_NEAR(component->GetR().Value(), 50.0, FLT_EPSILON);
}

/**
 * @tc.name: DomSvgTest005
 * @tc.desc: Verify that DomSvgEllipse can be created and set attrs correctly.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTest, DomSvgTest005, TestSize.Level1)
{
    const std::string jsonStr = ""
                                "{                                             "
                                "  \"tag\": \"ellipse\",                       "
                                "  \"attr\": [{                                "
                                "                \"cx\" : \"20\"               "
                                "             },                               "
                                "             {                                "
                                "                \"cy\" : \"30\"               "
                                "             },                               "
                                "             {                                "
                                "                \"rx\" : \"50\"               "
                                "             },                               "
                                "             {                                "
                                "                \"ry\" : \"40\"               "
                                "             }]                               "
                                "}";

    /**
     * @tc.steps: step1. Construct string with right fields, then create svg ellipse node with it.
     * @tc.expected: step1. svg ellipse node are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonStr);
    ASSERT_TRUE(domNodeRoot != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<SvgEllipseComponent> component = AceType::DynamicCast<SvgEllipseComponent>(boxChild->GetChild());
    ASSERT_TRUE(component != nullptr);
    /**
     * @tc.steps: step2. Check styles and attributes of created ellipse node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_NEAR(component->GetCx().Value(), 20.0, FLT_EPSILON);
    EXPECT_NEAR(component->GetCy().Value(), 30.0, FLT_EPSILON);
    EXPECT_NEAR(component->GetRx().Value(), 50.0, FLT_EPSILON);
    EXPECT_NEAR(component->GetRy().Value(), 40.0, FLT_EPSILON);
}

/**
 * @tc.name: DomSvgTest006
 * @tc.desc: Verify that DomSvgPath can be created and set attrs correctly.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTest, DomSvgTest006, TestSize.Level1)
{
    const std::string jsonStr = ""
                                "{                                                                                "
                                "  \"tag\": \"path\",                                                             "
                                "  \"attr\": [{                                                                   "
                                "                \"d\" : \"M20,50 C20,-50 180,150 180,50 C180,-50 20,150 20,50 z\""
                                "             }]                                                                  "
                                "}";

    /**
     * @tc.steps: step1. Construct string with right fields, then create svg path node with it.
     * @tc.expected: step1. svg path node are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonStr);
    ASSERT_TRUE(domNodeRoot != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<SvgPathComponent> component = AceType::DynamicCast<SvgPathComponent>(boxChild->GetChild());
    ASSERT_TRUE(component != nullptr);
    /**
     * @tc.steps: step2. Check styles and attributes of created path node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    ASSERT_TRUE(component->GetD() == "M20,50 C20,-50 180,150 180,50 C180,-50 20,150 20,50 z");
}

/**
 * @tc.name: DomSvgTest007
 * @tc.desc: Verify that DomSvgLine can be created and set attrs correctly.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTest, DomSvgTest007, TestSize.Level1)
{
    const std::string jsonStr = ""
                                "{                                             "
                                "  \"tag\": \"line\",                          "
                                "  \"attr\": [{                                "
                                "                \"x1\" : \"10\"               "
                                "             },                               "
                                "             {                                "
                                "                \"y1\" : \"20\"               "
                                "             },                               "
                                "             {                                "
                                "                \"x2\" : \"60\"               "
                                "             },                               "
                                "             {                                "
                                "                \"y2\" : \"50\"               "
                                "             }]                               "
                                "}";

    /**
     * @tc.steps: step1. Construct string with right fields, then create svg line node with it.
     * @tc.expected: step1. svg line node are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonStr);
    ASSERT_TRUE(domNodeRoot != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<SvgLineComponent> component = AceType::DynamicCast<SvgLineComponent>(boxChild->GetChild());
    ASSERT_TRUE(component != nullptr);
    /**
     * @tc.steps: step2. Check styles and attributes of created line node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_NEAR(component->GetX1().Value(), 10.0, FLT_EPSILON);
    EXPECT_NEAR(component->GetY1().Value(), 20.0, FLT_EPSILON);
    EXPECT_NEAR(component->GetX2().Value(), 60.0, FLT_EPSILON);
    EXPECT_NEAR(component->GetY2().Value(), 50.0, FLT_EPSILON);
}

/**
 * @tc.name: DomSvgTest008
 * @tc.desc: Verify that DomSvgPolyline can be created and set attrs correctly.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTest, DomSvgTest008, TestSize.Level1)
{
    const std::string jsonStr = ""
                                "{                                                           "
                                "  \"tag\": \"polyline\",                                        "
                                "  \"attr\": [{                                              "
                                "                \"points\" : \"0,400 60,325 70,375 100,300\""
                                "             }]                                             "
                                "}";

    /**
     * @tc.steps: step1. Construct string with right fields, then create svg polyline node with it.
     * @tc.expected: step1. svg polyline node are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonStr);
    ASSERT_TRUE(domNodeRoot != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<SvgPolygonComponent> component = AceType::DynamicCast<SvgPolygonComponent>(boxChild->GetChild());
    ASSERT_TRUE(component != nullptr);
    /**
     * @tc.steps: step2. Check styles and attributes of created polyline node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    ASSERT_TRUE(component->GetPoints() == "0,400 60,325 70,375 100,300");
}

/**
 * @tc.name: DomSvgTest009
 * @tc.desc: Verify that DomSvgPolygon can be created and set attrs correctly.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTest, DomSvgTest009, TestSize.Level1)
{
    const std::string jsonStr = ""
                                "{                                                           "
                                "  \"tag\": \"polygon\",                                     "
                                "  \"attr\": [{                                              "
                                "                \"points\" : \"0,400 60,325 70,375 100,300\""
                                "             }]                                             "
                                "}";

    /**
     * @tc.steps: step1. Construct string with right fields, then create svg polygon node with it.
     * @tc.expected: step1. svg polygon node are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonStr);
    ASSERT_TRUE(domNodeRoot != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<SvgPolygonComponent> component = AceType::DynamicCast<SvgPolygonComponent>(boxChild->GetChild());
    ASSERT_TRUE(component != nullptr);
    /**
     * @tc.steps: step2. Check styles and attributes of created polygon node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    ASSERT_TRUE(component->GetPoints() == "0,400 60,325 70,375 100,300");
}

/**
 * @tc.name: DomSvgTest010
 * @tc.desc: Verify that DomSvgAnimate can be created and set attrs correctly.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTest, DomSvgTest010, TestSize.Level1)
{
    const std::string jsonStr = ""
                                "{                                             "
                                "  \"tag\": \"animate\",                       "
                                "  \"attr\": [{                                "
                                "                \"begin\" : \"200\"           "
                                "             },                               "
                                "             {                                "
                                "                \"dur\" : \"2000\"            "
                                "             },                               "
                                "             {                                "
                                "                \"repeatcount\" : \"3\"       "
                                "             },                               "
                                "             {                                "
                                "                \"fill\" : \"freeze\"         "
                                "             },                               "
                                "             {                                "
                                "                \"calcmode\" : \"linear\"     "
                                "             },                               "
                                "             {                                "
                                "                \"from\" : \"100\"            "
                                "             },                               "
                                "             {                                "
                                "                \"to\" : \"300\"              "
                                "             },                               "
                                "             {                                "
                                "                \"attributename\" : \"width\" "
                                "             }]                               "
                                "}";

    /**
     * @tc.steps: step1. Construct string with right fields, then create svg animate node with it.
     * @tc.expected: step1. svg animate node are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonStr);
    ASSERT_TRUE(domNodeRoot != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<SvgAnimateComponent> component = AceType::DynamicCast<SvgAnimateComponent>(boxChild->GetChild());
    ASSERT_TRUE(component != nullptr);
    /**
     * @tc.steps: step2. Check styles and attributes of created animate node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    ASSERT_TRUE(component->GetBegin() == 200);
    ASSERT_TRUE(component->GetDur() == 2000);
    ASSERT_TRUE(component->GetRepeatCount() == 3);
    ASSERT_TRUE(component->GetFillMode() == FillMode::FORWARDS);
    ASSERT_TRUE(component->GetCalcMode() == CalcMode::LINEAR);
    ASSERT_TRUE(component->GetAttributeName() == "width");
    ASSERT_TRUE(component->GetFrom() == "100");
    ASSERT_TRUE(component->GetTo() == "300");
}

/**
 * @tc.name: DomSvgTest011
 * @tc.desc: Verify that DomSvgAnimateMotion can be created and set attrs correctly.
 * @tc.type: FUNC
 */
HWTEST_F(DomSvgTest, DomSvgTest011, TestSize.Level1)
{
    const std::string jsonStr = ""
                                "{                                                                                   "
                                "  \"tag\": \"animatemotion\",                                                       "
                                "  \"attr\": [{                                                                      "
                                "                \"begin\" : \"200\"                                                 "
                                "             },                                                                     "
                                "             {                                                                      "
                                "                \"dur\" : \"2000\"                                                  "
                                "             },                                                                     "
                                "             {                                                                      "
                                "                \"repeatcount\" : \"3\"                                             "
                                "             },                                                                     "
                                "             {                                                                      "
                                "                \"fill\" : \"freeze\"                                               "
                                "             },                                                                     "
                                "             {                                                                      "
                                "                \"calcmode\" : \"spline\"                                           "
                                "             },                                                                     "
                                "             {                                                                      "
                                "                \"keypoints\" : \"0;0.2;0.5;1\"                                     "
                                "             },                                                                     "
                                "             {                                                                      "
                                "                \"keytimes\" : \"0;0.1;0.8;1\"                                      "
                                "             },                                                                     "
                                "             {                                                                      "
                                "                \"path\" : \"M20,50 C20,-50 180,150 180,50 C180,-50 20,150 20,50 z\""
                                "             },                                                                     "
                                "             {                                                                      "
                                "                \"rotate\" : \"auto\"                                               "
                                "             },                                                                     "
                                "             {                                                                      "
                                "                \"keysplines\" : \"0.5 0 0.5 1; 0.5 0 0.5 1; 0.5 0 0.5 1\"          "
                                "             }]                                                                     "
                                "}";

    /**
     * @tc.steps: step1. Construct string with right fields, then create svg animate motion node with it.
     * @tc.expected: step1. svg animate motion node are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonStr);
    ASSERT_TRUE(domNodeRoot != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<SvgAnimateComponent> component = AceType::DynamicCast<SvgAnimateComponent>(boxChild->GetChild());
    ASSERT_TRUE(component != nullptr);
    /**
     * @tc.steps: step2. Check styles and attributes of created animate motion node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    ASSERT_TRUE(component->GetBegin() == 200);
    ASSERT_TRUE(component->GetDur() == 2000);
    ASSERT_TRUE(component->GetRepeatCount() == 3);
    ASSERT_TRUE(component->GetFillMode() == FillMode::FORWARDS);
    ASSERT_TRUE(component->GetCalcMode() == CalcMode::SPLINE);
    ASSERT_TRUE(component->GetKeyTimes().size() == 4);
    EXPECT_NEAR(component->GetKeyTimes().at(0), 0.0, FLT_EPSILON);
    EXPECT_NEAR(component->GetKeyTimes().at(1), 0.1, FLT_EPSILON);
    EXPECT_NEAR(component->GetKeyTimes().at(2), 0.8, FLT_EPSILON);
    EXPECT_NEAR(component->GetKeyTimes().at(3), 1.0, FLT_EPSILON);
    ASSERT_TRUE(component->GetKeyPoints().size() == 4);
    ASSERT_TRUE(component->GetKeyPoints().at(0) == "0");
    ASSERT_TRUE(component->GetKeyPoints().at(1) == "0.2");
    ASSERT_TRUE(component->GetKeyPoints().at(2) == "0.5");
    ASSERT_TRUE(component->GetKeyPoints().at(3) == "1");
    ASSERT_TRUE(component->GetKeySplines().size() == 3);
    ASSERT_TRUE(component->GetKeySplines().at(0) == "0.5 0 0.5 1");
    ASSERT_TRUE(component->GetKeySplines().at(1) == "0.5 0 0.5 1");
    ASSERT_TRUE(component->GetKeySplines().at(2) == "0.5 0 0.5 1");
    ASSERT_TRUE(component->GetPath() == "M20,50 C20,-50 180,150 180,50 C180,-50 20,150 20,50 z");
    ASSERT_TRUE(component->GetRotate() == "auto");
}

} // namespace OHOS::Ace::Framework
