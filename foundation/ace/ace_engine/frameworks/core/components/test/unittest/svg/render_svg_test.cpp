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

#include "base/log/log.h"
#include "base/utils/utils.h"

#include "core/components/svg/render_svg.h"
#include "core/components/svg/render_svg_circle.h"
#include "core/components/svg/render_svg_ellipse.h"
#include "core/components/svg/render_svg_line.h"
#include "core/components/svg/render_svg_path.h"
#include "core/components/svg/render_svg_polygon.h"
#include "core/components/svg/render_svg_rect.h"
#include "core/components/svg/svg_circle_component.h"
#include "core/components/svg/svg_component.h"
#include "core/components/svg/svg_ellipse_component.h"
#include "core/components/svg/svg_line_component.h"
#include "core/components/svg/svg_path_component.h"
#include "core/components/svg/svg_polygon_component.h"
#include "core/components/svg/svg_rect_component.h"
#include "core/components/test/unittest/mock/mock_render_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class RenderSvgTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: RenderSvgTest001
 * @tc.desc: Verify the Update Interface of RenderSvg work correctly with svg component.
 * @tc.type: FUNC
 * @tc.require: AR000FL0TG
 * @tc.author: chenlien
 */
HWTEST_F(RenderSvgTest, RenderSvgTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgComponent and RenderSvg.
     */
    auto svgComponent = AceType::MakeRefPtr<SvgComponent>();
    svgComponent->SetWidth(Dimension(100.0));
    svgComponent->SetHeight(Dimension(50.0));

    auto mockContext = MockRenderCommon::GetMockContext();
    auto renderSvg = AceType::MakeRefPtr<RenderSvg>();
    renderSvg->Attach(mockContext);

    /**
     * @tc.steps: step2. call the Update interface of RenderSvg.
     * @tc.expected: step2. renderSvg is setted as needLayout.
     */
    renderSvg->Update(svgComponent);
    EXPECT_TRUE(renderSvg->NeedLayout());
    renderSvg->PerformLayout();
    EXPECT_TRUE(renderSvg->GetLayoutSize() == Size(100.0, 50.0));
}

/**
 * @tc.name: RenderSvgTest002
 * @tc.desc: Verify the Update Interface of RenderSvgRect work correctly with svg rect component.
 * @tc.type: FUNC
 * @tc.require: AR000FL0TH
 * @tc.author: chenlien
 */
HWTEST_F(RenderSvgTest, RenderSvgTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgRectComponent and RenderSvgRect.
     */
    auto component = AceType::MakeRefPtr<SvgRectComponent>();
    component->SetWidth(Dimension(100.0));
    component->SetHeight(Dimension(50.0));
    component->SetX(Dimension(10.0));
    component->SetY(Dimension(20.0));
    component->SetRx(Dimension(5.0));
    component->SetRy(Dimension(3.0));
    auto mockContext = MockRenderCommon::GetMockContext();
    auto render = AceType::MakeRefPtr<RenderSvgRect>();
    render->Attach(mockContext);

    /**
     * @tc.steps: step2. call the Update interface of RenderSvgRect.
     * @tc.expected: step2. renderSvgRect is setted as needLayout.
     */
    render->Update(component);
    EXPECT_TRUE(render->NeedLayout());
    EXPECT_NEAR(render->GetWidth().Value(), 100.0, FLT_EPSILON);
    EXPECT_NEAR(render->GetHeight().Value(), 50.0, FLT_EPSILON);
    EXPECT_NEAR(render->GetX().Value(), 10.0, FLT_EPSILON);
    EXPECT_NEAR(render->GetY().Value(), 20.0, FLT_EPSILON);
    EXPECT_NEAR(render->GetRx().Value(), 5.0, FLT_EPSILON);
    EXPECT_NEAR(render->GetRy().Value(), 3.0, FLT_EPSILON);
    render->PerformLayout();
}

/**
 * @tc.name: RenderSvgTest003
 * @tc.desc: Verify the Update Interface of RenderSvgCircle work correctly with svg circle component.
 * @tc.type: FUNC
 * @tc.require: AR000FL0TI
 * @tc.author: chenlien
 */
HWTEST_F(RenderSvgTest, RenderSvgTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgCircleComponent and RenderSvgRect.
     */
    auto component = AceType::MakeRefPtr<SvgCircleComponent>();
    component->SetCx(Dimension(20.0));
    component->SetCy(Dimension(30.0));
    component->SetR(Dimension(50.0));
    auto mockContext = MockRenderCommon::GetMockContext();
    auto render = AceType::MakeRefPtr<RenderSvgCircle>();
    render->Attach(mockContext);

    /**
     * @tc.steps: step2. call the Update interface of renderSvgCircle.
     * @tc.expected: step2. renderSvgCircle is setted as needLayout.
     */
    render->Update(component);
    EXPECT_TRUE(render->NeedLayout());
    EXPECT_NEAR(render->GetCx().Value(), 20.0, FLT_EPSILON);
    EXPECT_NEAR(render->GetCy().Value(), 30.0, FLT_EPSILON);
    EXPECT_NEAR(render->GetR().Value(), 50.0, FLT_EPSILON);
    render->PerformLayout();
}

/**
 * @tc.name: RenderSvgTest004
 * @tc.desc: Verify the Update Interface of RenderSvgEllipse work correctly with svg ellipse component.
 * @tc.type: FUNC
 * @tc.require: AR000FL0TJ
 * @tc.author: chenlien
 */
HWTEST_F(RenderSvgTest, RenderSvgTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgEllipseComponent and RenderSvgEllipse.
     */
    auto component = AceType::MakeRefPtr<SvgEllipseComponent>();
    component->SetCx(Dimension(20.0));
    component->SetCy(Dimension(30.0));
    component->SetRx(Dimension(50.0));
    component->SetRy(Dimension(40.0));
    auto mockContext = MockRenderCommon::GetMockContext();
    auto render = AceType::MakeRefPtr<RenderSvgEllipse>();
    render->Attach(mockContext);

    /**
     * @tc.steps: step2. call the Update interface of renderSvgEllipse.
     * @tc.expected: step2. renderSvgEllipse is setted as needLayout.
     */
    render->Update(component);
    EXPECT_TRUE(render->NeedLayout());
    EXPECT_NEAR(render->GetCx().Value(), 20.0, FLT_EPSILON);
    EXPECT_NEAR(render->GetCy().Value(), 30.0, FLT_EPSILON);
    EXPECT_NEAR(render->GetRx().Value(), 50.0, FLT_EPSILON);
    EXPECT_NEAR(render->GetRy().Value(), 40.0, FLT_EPSILON);
    render->PerformLayout();
}

/**
 * @tc.name: RenderSvgTest005
 * @tc.desc: Verify the Update Interface of RenderSvgPath work correctly with svg path component.
 * @tc.type: FUNC
 * @tc.require: AR000FL0TK
 * @tc.author: chenlien
 */
HWTEST_F(RenderSvgTest, RenderSvgTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgPathComponent and RenderSvgPath.
     */
    auto component = AceType::MakeRefPtr<SvgPathComponent>();
    component->SetD("M20,50 C20,-50 180,150 180,50 C180,-50 20,150 20,50 z");
    auto mockContext = MockRenderCommon::GetMockContext();
    auto render = AceType::MakeRefPtr<RenderSvgPath>();
    render->Attach(mockContext);

    /**
     * @tc.steps: step2. call the Update interface of renderSvgPath.
     * @tc.expected: step2. renderSvgPath is setted as needLayout.
     */
    render->Update(component);
    EXPECT_TRUE(render->NeedLayout());
    ASSERT_TRUE(render->GetD() == "M20,50 C20,-50 180,150 180,50 C180,-50 20,150 20,50 z");
    render->PerformLayout();
}

/**
 * @tc.name: RenderSvgTest006
 * @tc.desc: Verify the Update Interface of RenderSvgLine work correctly with svg line component.
 * @tc.type: FUNC
 * @tc.require: AR000FL0TL
 * @tc.author: chenlien
 */
HWTEST_F(RenderSvgTest, RenderSvgTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgLineComponent and RenderSvgLine.
     */
    auto component = AceType::MakeRefPtr<SvgLineComponent>();
    component->SetX1(Dimension(10.0));
    component->SetY1(Dimension(20.0));
    component->SetX2(Dimension(60.0));
    component->SetY2(Dimension(50.0));
    auto mockContext = MockRenderCommon::GetMockContext();
    auto render = AceType::MakeRefPtr<RenderSvgLine>();
    render->Attach(mockContext);

    /**
     * @tc.steps: step2. call the Update interface of renderSvgLine.
     * @tc.expected: step2. renderSvgLine is setted as needLayout.
     */
    render->Update(component);
    EXPECT_TRUE(render->NeedLayout());
    EXPECT_NEAR(render->GetX1().Value(), 10.0, FLT_EPSILON);
    EXPECT_NEAR(render->GetY1().Value(), 20.0, FLT_EPSILON);
    EXPECT_NEAR(render->GetX2().Value(), 60.0, FLT_EPSILON);
    EXPECT_NEAR(render->GetY2().Value(), 50.0, FLT_EPSILON);
    render->PerformLayout();
}

/**
 * @tc.name: RenderSvgTest007
 * @tc.desc: Verify the Update Interface of RenderSvgPolyline work correctly with svg polyline component.
 * @tc.type: FUNC
 * @tc.require: AR000FL0TM
 * @tc.author: chenlien
 */
HWTEST_F(RenderSvgTest, RenderSvgTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgPolygonComponent and RenderSvgPolygon.
     */
    auto component = AceType::MakeRefPtr<SvgPolygonComponent>(false);
    component->SetPoints("0,400 60,325 70,375 100,300");
    auto mockContext = MockRenderCommon::GetMockContext();
    auto render = AceType::MakeRefPtr<RenderSvgPolygon>();
    render->Attach(mockContext);

    /**
     * @tc.steps: step2. call the Update interface of renderSvgPolygon.
     * @tc.expected: step2. renderSvgPolygon is setted as needLayout.
     */
    render->Update(component);
    EXPECT_TRUE(render->NeedLayout());
    ASSERT_TRUE(render->GetPoints() == "0,400 60,325 70,375 100,300");
    render->PerformLayout();
}

/**
 * @tc.name: RenderSvgTest008
 * @tc.desc: Verify the Update Interface of RenderSvgPolygon work correctly with svg polygon component.
 * @tc.type: FUNC
 * @tc.require: AR000FL0TN
 * @tc.author: chenlien
 */
HWTEST_F(RenderSvgTest, RenderSvgTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct SvgPolygonComponent and RenderSvgPolygon.
     */
    auto component = AceType::MakeRefPtr<SvgPolygonComponent>();
    component->SetPoints("0,400 60,325 70,375 100,300");
    auto mockContext = MockRenderCommon::GetMockContext();
    auto render = AceType::MakeRefPtr<RenderSvgPolygon>();
    render->Attach(mockContext);

    /**
     * @tc.steps: step2. call the Update interface of renderSvgPolygon.
     * @tc.expected: step2. renderSvgPolygon is setted as needLayout.
     */
    render->Update(component);
    EXPECT_TRUE(render->NeedLayout());
    ASSERT_TRUE(render->GetPoints() == "0,400 60,325 70,375 100,300");
    render->PerformLayout();
}

} // namespace OHOS::Ace