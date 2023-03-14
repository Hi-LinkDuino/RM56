/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "components/root_view.h"
#include "components/ui_canvas.h"
#include "components/ui_view_group.h"
#include "components/ui_view_group.h"

#include <climits>
#include <gtest/gtest.h>
#include "test_resource_config.h"
using namespace testing::ext;
namespace OHOS {
namespace {
    const int16_t POS_X = 50;
    const int16_t POS_Y = 100;
    const uint16_t WIDTH = 100;
    const uint16_t HEIGHT = 100;
    const int16_t START1_X = 10;
    const int16_t START1_Y = 20;
    const int16_t LINE1_X = 40;
    const int16_t LINE1_Y = 100;
    const int16_t LINE2_X = 50;
    const int16_t LINE2_Y = 120;
    const int16_t CENTER_X = 150;
    const int16_t CENTER_Y = 150;
    const int16_t RADIUS = 50;
    const int16_t START_ANGLE = 30;
    const int16_t END_ANGLE = 250;
    const int16_t RECT_X = 250;
    const int16_t RECT_Y = 50;
    const int16_t RECT_WIDTH = 100;
    const int16_t RECT_HEIGHT = 50;
    const float LINE_OFFSET1 = 10.0;
    const float LINE_OFFSET2 = 50.0;
    const float LINE_OFFSET3 = 150.0;
    const float MITERLIMIT1 = 11.05;
    const float MITERLIMIT2 = 4.6;
    const float MITERLIMIT3 = -5.6;
    const int16_t IMAGE_X = 20;
    const int16_t IMAGE_Y = 10;
    const uint8_t FONT_SIZE = 15;
    const int16_t LETTER_SPACE = 2;
    const float SCALE_X = 0.5;
    const float SCALE_Y = 0.5;
    const float SHEAR_X = -0.5;
    const float SHEAR_Y = 0.5;
    const float TRANSLATE_X = 30;
    const float TRANSLATE_Y = 10;
    const float GLOBALALPHA = 0.5;
    const int16_t DOUBLENUMBER = 2;
    const double START_X = 180.0;
    const double START_Y = 140.0;
    const double START_R = 10.0;
    const double END_X = 150.0;
    const double END_Y = 100.0;
    const double END_R = 80.0;
    const double COLOR_STOP_0 = 0.0;
    const double COLOR_STOP_1 = 0.3;
    const double COLOR_STOP_2 = 0.6;
    const double COLOR_STOP_3 = 1.0;
    const float ROTATE = 10;
    const int16_t LINE_X = 30;
    const int16_t LINE_Y = 70;
}

class TestUICanvas : public UICanvas {
public:
    TestUICanvas() {}
    virtual ~TestUICanvas() {}

    const UICanvasVertices* GetPath() const
    {
        return vertices_;
    }

    Point GetStartPos() const
    {
        float x;
        float y;
        if (GetPath() != nullptr) {
            vertices_->Rewind(0);
            vertices_->GenerateVertex(&x, &y);
            return { (int16_t)x, (int16_t)y };
        } else {
            return { COORD_MIN, COORD_MIN };
        }
    }

    Point GetEndPos() const
    {
        float x;
        float y;
        if (GetPath() != nullptr) {
            vertices_->LastVertex(&x, &y);
            return { (int16_t)x, (int16_t)y };
        } else {
            return { COORD_MIN, COORD_MIN };
        }
    }
};

class UICanvasTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown();
    static Paint* paint_;
    static TestUICanvas* canvas_;
};

Paint* UICanvasTest::paint_ = nullptr;
TestUICanvas* UICanvasTest::canvas_ = nullptr;
void UICanvasTest::SetUp()
{
    if (paint_ == nullptr) {
        paint_ = new Paint();
    }
    if (canvas_ == nullptr) {
        canvas_ = new TestUICanvas();
    }
}

void UICanvasTest::TearDown()
{
    if (paint_ != nullptr) {
        delete paint_;
        paint_ = nullptr;
    }
    if (canvas_ != nullptr) {
        delete canvas_;
        canvas_ = nullptr;
    }
}
/**
 * @tc.name: UICanvasSetPaintStyle_001
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UICanvasTest, UICanvasSetPaintStyle_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Paint::PaintStyle paintStyle = Paint::PaintStyle::FILL_STYLE;

    paint_->SetStyle(Paint::PaintStyle::FILL_STYLE);
    EXPECT_EQ(paint_->GetStyle(), paintStyle);
}

/**
 * @tc.name: UICanvasSetStrokeWidth_001
 * @tc.desc: Verify SetStrokeWidth function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UICanvasTest, UICanvasSetStrokeWidth_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetStrokeWidth(WIDTH);
    EXPECT_EQ(paint_->GetStrokeWidth(), WIDTH);
}

/**
 * @tc.name: UICanvasSetStrokeColor_001
 * @tc.desc: Verify SetStrokeColor function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UICanvasTest, UICanvasSetStrokeColor_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    ColorType color = Color::Red();

    paint_->SetStrokeColor(color);
    EXPECT_EQ(paint_->GetStrokeColor().full, color.full);
}

/**
 * @tc.name: UICanvasSetFillColor_001
 * @tc.desc: Verify SetFillColor function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UICanvasTest, UICanvasSetFillColor_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    ColorType color = Color::Red();

    paint_->SetFillColor(color);
    EXPECT_EQ(paint_->GetFillColor().full, color.full);
}

/**
 * @tc.name: UICanvasSetOpacity_001
 * @tc.desc: Verify SetOpacity function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UICanvasTest, UICanvasSetOpacity_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetOpacity(OPA_OPAQUE);
    EXPECT_EQ(paint_->GetOpacity(), OPA_OPAQUE);
}

/**
 * @tc.name: UICanvasSetStartPosition_001
 * @tc.desc: Verify SetStartPosition function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UICanvasTest, UICanvasSetStartPosition_001, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    canvas_->SetStartPosition({POS_X, POS_Y});
    EXPECT_EQ(canvas_->GetStartPosition().x, POS_X);
    EXPECT_EQ(canvas_->GetStartPosition().y, POS_Y);
}

/**
 * @tc.name: UICanvasDrawLine_001
 * @tc.desc: Verify DrawLine function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UICanvasTest, UICanvasDrawLine_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    RootView* rootView = RootView::GetInstance();
    UIViewGroup* viewGroup = static_cast<UIViewGroup*>(rootView);
    Paint paint;
    Point startPoint = { 0, 50 };
    Point endPoint = { 50, 100 };

    viewGroup->SetPosition(0, 0);
    viewGroup->SetWidth(WIDTH);
    viewGroup->SetHeight(HEIGHT);
    canvas_->DrawLine(startPoint, endPoint, paint);
    viewGroup->Add(canvas_);
    EXPECT_EQ(canvas_->GetStartPosition().x, endPoint.x);
    EXPECT_EQ(canvas_->GetStartPosition().y, endPoint.y);
    viewGroup->Remove(canvas_);
}

/**
 * @tc.name: UICanvasDrawCurve_001
 * @tc.desc: Verify DrawCurve function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPV
 */
HWTEST_F(UICanvasTest, UICanvasDrawCurve_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    RootView* rootView = RootView::GetInstance();
    UIViewGroup* viewGroup = static_cast<UIViewGroup*>(rootView);
    Paint paint;
    Point startPoint = { 100, 100 };
    Point endPoint = { 150, 200 };
    Point control1Point = { 150, 100 };
    Point control2Point = { 150, 100 };

    viewGroup->SetPosition(0, 0);
    viewGroup->SetWidth(WIDTH);
    viewGroup->SetHeight(HEIGHT);
    canvas_->DrawCurve(startPoint, control1Point, control2Point, endPoint, paint);
    viewGroup->Add(canvas_);
    EXPECT_EQ(canvas_->GetStartPosition().x, endPoint.x);
    EXPECT_EQ(canvas_->GetStartPosition().y, endPoint.y);
    viewGroup->Remove(canvas_);
}

/**
 * @tc.name: UICanvasBeginPath_001
 * @tc.desc: Verify BeginPath function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EVN8V
 */
HWTEST_F(UICanvasTest, UICanvasBeginPath_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    EXPECT_EQ(canvas_->GetPath(), nullptr);
    canvas_->BeginPath();
    EXPECT_NE(canvas_->GetPath(), nullptr);
}

/**
 * @tc.name: UICanvasMoveTo_001
 * @tc.desc: Verify MoveTo function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EVN8V
 */
HWTEST_F(UICanvasTest, UICanvasMoveTo_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    canvas_->BeginPath();
    canvas_->MoveTo({START1_X, START1_Y});
    Point start = canvas_->GetStartPos();
    EXPECT_EQ(start.x, START1_X);
    EXPECT_EQ(start.y, START1_Y);

    Point end = canvas_->GetEndPos();
    EXPECT_EQ(end.x, START1_X);
    EXPECT_EQ(end.y, START1_Y);
}

/**
 * @tc.name: UICanvasLineTo_001
 * @tc.desc: Verify LineTo function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EVN8V
 */
HWTEST_F(UICanvasTest, UICanvasLineTo_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    canvas_->BeginPath();
    canvas_->LineTo({LINE1_X, LINE1_Y});
    Point start = canvas_->GetStartPos();
    EXPECT_EQ(start.x, LINE1_X);
    EXPECT_EQ(start.y, LINE1_Y);

    Point end = canvas_->GetEndPos();
    EXPECT_EQ(end.x, LINE1_X);
    EXPECT_EQ(end.y, LINE1_Y);
}

/**
 * @tc.name: UICanvasLineTo_002
 * @tc.desc: Verify LineTo function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EVN8V
 */
HWTEST_F(UICanvasTest, UICanvasLineTo_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    canvas_->BeginPath();
    canvas_->MoveTo({START1_X, START1_Y});
    canvas_->LineTo({LINE1_X, LINE1_Y});
    Point start = canvas_->GetStartPos();
    EXPECT_EQ(start.x, START1_X);
    EXPECT_EQ(start.y, START1_Y);

    Point end = canvas_->GetEndPos();
    EXPECT_EQ(end.x, LINE1_X);
    EXPECT_EQ(end.y, LINE1_Y);

    canvas_->LineTo({LINE2_X, LINE2_Y});
    end = canvas_->GetEndPos();
    EXPECT_EQ(end.x, LINE2_X);
    EXPECT_EQ(end.y, LINE2_Y);
}

#if GRAPHIC_ENABLE_ARC_FLAG
/**
 * @tc.name: UICanvasArcTo_001
 * @tc.desc: Verify ArcTo function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EVN8V
 */
HWTEST_F(UICanvasTest, UICanvasArcTo_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    canvas_->BeginPath();
    canvas_->ArcTo({CENTER_X, CENTER_Y}, RADIUS, START_ANGLE, END_ANGLE);

    float sinma = RADIUS * Sin(START_ANGLE);
    float cosma = RADIUS * Sin(QUARTER_IN_DEGREE - START_ANGLE);
    Point start = canvas_->GetStartPos();
    EXPECT_EQ(start.x, int16_t(CENTER_X + sinma));
    EXPECT_EQ(start.y, int16_t(CENTER_Y - cosma));

    sinma = RADIUS * Sin(END_ANGLE);
    cosma = RADIUS * Sin(QUARTER_IN_DEGREE - END_ANGLE);
    Point end = canvas_->GetEndPos();
    EXPECT_EQ(end.x, int16_t(CENTER_X + sinma));
    EXPECT_EQ(end.y, int16_t(CENTER_Y - cosma));
}

/**
 * @tc.name: UICanvasArcTo_002
 * @tc.desc: Verify ArcTo function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EVN8V
 */
HWTEST_F(UICanvasTest, UICanvasArcTo_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    canvas_->BeginPath();
    canvas_->ArcTo({CENTER_X, CENTER_Y}, RADIUS, START_ANGLE,
                   START_ANGLE + CIRCLE_IN_DEGREE + QUARTER_IN_DEGREE);

    float sinma = RADIUS * Sin(START_ANGLE);
    float cosma = RADIUS * Sin(QUARTER_IN_DEGREE - START_ANGLE);
    Point start = canvas_->GetStartPos();
    EXPECT_EQ(start.x, int16_t(CENTER_X + sinma));
    EXPECT_EQ(start.y, int16_t(CENTER_Y - cosma));

    sinma = RADIUS * Sin(END_ANGLE);
    cosma = RADIUS * Sin(QUARTER_IN_DEGREE - END_ANGLE);
    Point end = canvas_->GetEndPos();
    EXPECT_EQ(end.x, start.x);
    EXPECT_EQ(end.y, start.y);
}

/**
 * @tc.name: UICanvasArcTo_003
 * @tc.desc: Verify ArcTo function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EVN8V
 */
HWTEST_F(UICanvasTest, UICanvasArcTo_003, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    canvas_->BeginPath();
    canvas_->MoveTo({START1_X, START1_Y});
    canvas_->ArcTo({CENTER_X, CENTER_Y}, RADIUS, START_ANGLE, END_ANGLE);

    Point start = canvas_->GetStartPos();
    EXPECT_EQ(start.x, START1_X);
    EXPECT_EQ(start.y, START1_Y);

    float sinma = RADIUS * Sin(END_ANGLE);
    float cosma = RADIUS * Sin(QUARTER_IN_DEGREE - END_ANGLE);
    Point end = canvas_->GetEndPos();
    EXPECT_EQ(end.x, int16_t(CENTER_X + sinma));
    EXPECT_EQ(end.y, int16_t(CENTER_Y - cosma));
}
#endif

/**
 * @tc.name: UICanvasAddRect_001
 * @tc.desc: Verify AddRect function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EVN8V
 */
HWTEST_F(UICanvasTest, UICanvasAddRect_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    canvas_->BeginPath();
    canvas_->AddRect({RECT_X, RECT_Y}, RECT_HEIGHT, RECT_WIDTH);

    Point start = canvas_->GetStartPos();
    EXPECT_EQ(start.x, RECT_X);
    EXPECT_EQ(start.y, RECT_Y);

    Point end = canvas_->GetEndPos();
    EXPECT_EQ(end.x, 0);
    EXPECT_EQ(end.y, 0);
}

/**
 * @tc.name: UICanvasAddRect_002
 * @tc.desc: Verify AddRect function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EVN8V
 */
HWTEST_F(UICanvasTest, UICanvasAddRect_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    canvas_->BeginPath();
    canvas_->MoveTo({START1_X, START1_Y});
    canvas_->LineTo({LINE1_X, LINE1_Y});
    canvas_->AddRect({RECT_X, RECT_Y}, RECT_HEIGHT, RECT_WIDTH);

    Point start = canvas_->GetStartPos();
    EXPECT_EQ(start.x, START1_X);
    EXPECT_EQ(start.y, START1_Y);

    Point end = canvas_->GetEndPos();
    EXPECT_EQ(end.x, 0);
    EXPECT_EQ(end.y, 0);
}

/**
 * @tc.name: UICanvasClosePath_001
 * @tc.desc: Verify ClosePath function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EVN8V
 */
HWTEST_F(UICanvasTest, UICanvasClosePath_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    canvas_->BeginPath();
    canvas_->MoveTo({START1_X, START1_Y});
    canvas_->LineTo({LINE1_X, LINE1_Y});
    canvas_->ClosePath();

    Point start = canvas_->GetStartPos();
    EXPECT_EQ(start.x, START1_X);
    EXPECT_EQ(start.y, START1_Y);

    Point end = canvas_->GetEndPos();
    EXPECT_EQ(end.x, 0);
    EXPECT_EQ(end.y, 0);
}

/**
 * @tc.name: UICanvasClosePath_002
 * @tc.desc: Verify ClosePath function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000EVN8R
 */
HWTEST_F(UICanvasTest, UICanvasClosePath_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    canvas_->BeginPath();
    canvas_->MoveTo({START1_X, START1_Y});
    canvas_->LineTo({LINE1_X, LINE1_Y});
    canvas_->ClosePath();
    canvas_->DrawPath(*paint_);

    Point start = canvas_->GetStartPos();
    EXPECT_EQ(start.x, START1_X);
    EXPECT_EQ(start.y, START1_Y);

    Point end = canvas_->GetEndPos();
    EXPECT_EQ(end.x, 0);
    EXPECT_EQ(end.y, 0);

    canvas_->BeginPath();
    canvas_->LineTo({LINE2_X, LINE2_Y});
    canvas_->ClosePath();

    start = canvas_->GetStartPos();
    EXPECT_EQ(start.x, LINE2_X);
    EXPECT_EQ(start.y, LINE2_Y);

    end = canvas_->GetEndPos();
    EXPECT_EQ(end.x, 0);
    EXPECT_EQ(end.y, 0);
}

#if GRAPHIC_ENABLE_LINECAP_FLAG
/**
 * @tc.name: UICanvasSetLineCap_001
 * @tc.desc: Verify SetLineCap function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetLineCap_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    paint_->SetLineCap(LineCap::BUTT_CAP);
    EXPECT_EQ(paint_->GetLineCap(), LineCap::BUTT_CAP);
    
    paint_->SetLineCap(LineCap::SQUARE_CAP);
    EXPECT_EQ(paint_->GetLineCap(), LineCap::SQUARE_CAP);

    paint_->SetLineCap(LineCap::ROUND_CAP);
    EXPECT_EQ(paint_->GetLineCap(), LineCap::ROUND_CAP);
}

/**
 * @tc.name: UICanvasSetLineCap_002
 * @tc.desc: Verify SetLineCap function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetLineCap_002, TestSize.Level0)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetLineCap(LineCap::ROUND_CAP);
    EXPECT_EQ(paint_->GetLineCap(), LineCap::ROUND_CAP);

    paint_->SetLineCap(LineCap::BUTT_CAP);
    EXPECT_EQ(paint_->GetLineCap(), LineCap::BUTT_CAP);

    paint_->SetLineCap(LineCap::SQUARE_CAP);
    EXPECT_EQ(paint_->GetLineCap(), LineCap::SQUARE_CAP);
}
#endif

#if GRAPHIC_ENABLE_LINEJOIN_FLAG
/**
 * @tc.name: UICanvasSetLineJoin_001
 * @tc.desc: Verify SetLineJoin function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetLineJoin_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetLineJoin(LineJoin::MITER_JOIN);
    EXPECT_EQ(paint_->GetLineJoin(), LineJoin::MITER_JOIN);

    paint_->SetLineJoin(LineJoin::ROUND_JOIN);
    EXPECT_EQ(paint_->GetLineJoin(), LineJoin::ROUND_JOIN);

    paint_->SetLineJoin(LineJoin::BEVEL_JOIN);
    EXPECT_EQ(paint_->GetLineJoin(), LineJoin::BEVEL_JOIN);
}

/**
 * @tc.name: UICanvasSetLineJoin_002
 * @tc.desc: Verify SetLineJoin function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetLineJoin_002, TestSize.Level0)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetLineJoin(LineJoin::MITER_JOIN);
    EXPECT_EQ(paint_->GetLineJoin(), LineJoin::MITER_JOIN);

    paint_->SetLineJoin(LineJoin::ROUND_JOIN);
    EXPECT_EQ(paint_->GetLineJoin(), LineJoin::ROUND_JOIN);

    paint_->SetLineJoin(LineJoin::BEVEL_JOIN);
    EXPECT_EQ(paint_->GetLineJoin(), LineJoin::BEVEL_JOIN);
}
#endif

#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
/**
 * @tc.name: UICanvasSetLineDashOffset_001
 * @tc.desc: Verify SetLineDashOffset function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetLineDashOffset_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetLineDashOffset(LINE_OFFSET1);
    EXPECT_EQ(paint_->GetLineDashOffset(), LINE_OFFSET1);

    paint_->SetLineDashOffset(LINE_OFFSET2);
    EXPECT_EQ(paint_->GetLineDashOffset(), LINE_OFFSET2);

    paint_->SetLineDashOffset(LINE_OFFSET3);
    EXPECT_EQ(paint_->GetLineDashOffset(), LINE_OFFSET3);
}

/**
 * @tc.name: UICanvasSetLineDashOffset_002
 * @tc.desc: Verify SetLineDashOffset function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetLineDashOffset_002, TestSize.Level0)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetLineDashOffset(LINE_OFFSET1);
    EXPECT_EQ(paint_->GetLineDashOffset(), LINE_OFFSET1);

    paint_->SetLineDashOffset(LINE_OFFSET2);
    EXPECT_EQ(paint_->GetLineDashOffset(), LINE_OFFSET2);

    paint_->SetLineDashOffset(LINE_OFFSET3);
    EXPECT_EQ(paint_->GetLineDashOffset(), LINE_OFFSET3);
}
#endif

#if GRAPHIC_ENABLE_LINEJOIN_FLAG
/**
 * @tc.name: UICanvasMiterLimit_001
 * @tc.desc: Verify MiterLimit function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasMiterLimit_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetMiterLimit(MITERLIMIT1);
    EXPECT_EQ(paint_->GetMiterLimit(), MITERLIMIT1);

    paint_->SetMiterLimit(MITERLIMIT2);
    EXPECT_EQ(paint_->GetMiterLimit(), MITERLIMIT2);

    paint_->SetMiterLimit(MITERLIMIT3);
    EXPECT_EQ(paint_->GetMiterLimit(), MITERLIMIT3);
}

/**
 * @tc.name: UICanvasMiterLimit_002
 * @tc.desc: Verify MiterLimit function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasMiterLimit_002, TestSize.Level0)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetMiterLimit(MITERLIMIT1);
    EXPECT_EQ(paint_->GetMiterLimit(), MITERLIMIT1);

    paint_->SetMiterLimit(MITERLIMIT2);
    EXPECT_EQ(paint_->GetMiterLimit(), MITERLIMIT2);

    paint_->SetMiterLimit(MITERLIMIT3);
    EXPECT_EQ(paint_->GetMiterLimit(), MITERLIMIT3);
}

/**
 * @tc.name: UICanvasMiterLimit_003
 * @tc.desc: Verify MiterLimit function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasMiterLimit_003, TestSize.Level0)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetMiterLimit(MITERLIMIT1);
    EXPECT_EQ(paint_->GetMiterLimit(), MITERLIMIT1);

    paint_->SetMiterLimit(MITERLIMIT2 + MITERLIMIT1);
    EXPECT_EQ(paint_->GetMiterLimit(), MITERLIMIT2 + MITERLIMIT1);

    paint_->SetMiterLimit(MITERLIMIT3 + MITERLIMIT2);
    EXPECT_EQ(paint_->GetMiterLimit(), MITERLIMIT3 + MITERLIMIT2);
}
#endif

#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
/**
 * @tc.name: UICanvasSetLineDash_001
 * @tc.desc: Verify SetLineDash function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetLineDash_001, TestSize.Level0)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    const int32_t dashCount = 4;
    float dash[dashCount] = {1, 1.5, 2, 2.5};
    paint_->SetLineDash(dash, dashCount);
    EXPECT_EQ(paint_->GetLineDashCount(), dashCount);

    paint_->ClearLineDash();
    EXPECT_EQ(paint_->GetLineDashCount(), 0);
    
    paint_->SetLineDash(dash, dashCount);
    int32_t dashLen = paint_->GetLineDashCount();
    float* dashArr = paint_->GetLineDash();
    for (int32_t i = 0; i < dashLen; i++) {
        EXPECT_EQ(dashArr[i], dash[i]);
    }

    EXPECT_EQ(paint_->IsLineDash(), 1);

    paint_->ClearLineDash();
    EXPECT_EQ(paint_->GetLineDashCount(), 0);

    EXPECT_EQ(paint_->IsLineDash(), 0);
}
#endif
/**
 * @tc.name: UICanvasStrokeRect_001
 * @tc.desc: Verify StrokeRect function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasStrokeRect_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    RootView* rootView = RootView::GetInstance();
    UIViewGroup* viewGroup = static_cast<UIViewGroup*>(rootView);
    Paint paint;

    viewGroup->SetPosition(0, 0);
    viewGroup->SetWidth(WIDTH);
    viewGroup->SetHeight(HEIGHT);
    canvas_->StrokeRect({RECT_X, RECT_Y}, RECT_HEIGHT, RECT_WIDTH, paint);
    viewGroup->Add(canvas_);
    EXPECT_EQ(canvas_->GetStartPosition().x, RECT_X);
    EXPECT_EQ(canvas_->GetStartPosition().y, RECT_Y);
    viewGroup->Remove(canvas_);

    viewGroup->SetPosition(10, 20);
    viewGroup->SetWidth(WIDTH);
    viewGroup->SetHeight(HEIGHT);
    canvas_->StrokeRect({0, 0}, RECT_HEIGHT, RECT_WIDTH, paint);
    viewGroup->Add(canvas_);
    EXPECT_EQ(canvas_->GetStartPosition().x, 0);
    EXPECT_EQ(canvas_->GetStartPosition().y, 0);
    viewGroup->Remove(canvas_);
}

/**
 * @tc.name: UICanvasCleanRect_001
 * @tc.desc: Verify CleanRect function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasCleanRect_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    RootView* rootView = RootView::GetInstance();
    UIViewGroup* viewGroup = static_cast<UIViewGroup*>(rootView);
    Paint paint;

    viewGroup->SetPosition(0, 0);
    viewGroup->SetWidth(WIDTH);
    viewGroup->SetHeight(HEIGHT);

    paint.SetStyle(Paint::PaintStyle::FILL_STYLE);
    paint.SetFillColor(Color::Red());

    canvas_->DrawRect({RECT_X, RECT_Y}, RECT_HEIGHT, RECT_WIDTH, paint);
    viewGroup->Add(canvas_);
    
    canvas_->ClearRect({RECT_X + RECT_X / 2, RECT_Y + RECT_Y / 2},
                      RECT_HEIGHT / 2, RECT_WIDTH / 2);
    EXPECT_EQ(canvas_->GetStartPos().x, RECT_X + RECT_X / 2);
    EXPECT_EQ(canvas_->GetStartPos().y, RECT_Y + RECT_Y / 2);

    viewGroup->Remove(canvas_);

    viewGroup->SetPosition(0, 0);
    viewGroup->SetWidth(WIDTH);
    viewGroup->SetHeight(HEIGHT);
    paint.SetStyle(Paint::PaintStyle::FILL_STYLE);
    paint.SetFillColor(Color::Blue());
    canvas_->StrokeRect({0, 0}, RECT_HEIGHT, RECT_WIDTH, paint);
    viewGroup->Add(canvas_);
    EXPECT_EQ(canvas_->GetStartPosition().x, 0);
    EXPECT_EQ(canvas_->GetStartPosition().y, 0);

    canvas_->ClearRect({RECT_X - 10, RECT_Y - 10},
                       RECT_HEIGHT + 10, RECT_WIDTH + 10);
    EXPECT_EQ(canvas_->GetStartPos().x, RECT_X - 10);
    EXPECT_EQ(canvas_->GetStartPos().y, RECT_Y - 10);
    viewGroup->Remove(canvas_);
}
#if GRAPHIC_ENABLE_DRAW_IMAGE_FLAG
/**
 * @tc.name: UICanvasDrawImage_001
 * @tc.desc: Verify DrawImage function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasDrawImage_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    RootView* rootView = RootView::GetInstance();
    UIViewGroup* viewGroup = static_cast<UIViewGroup*>(rootView);
    Paint paint;

    viewGroup->SetPosition(0, 0);
    viewGroup->SetWidth(WIDTH);
    viewGroup->SetHeight(HEIGHT);

    canvas_->DrawImage({0, 0}, JPEG_IMAGE_PATH, paint);
    viewGroup->Add(canvas_);
    EXPECT_EQ(canvas_->GetStartPosition().x, 0);
    EXPECT_EQ(canvas_->GetStartPosition().y, 0);
    
    canvas_->ClearRect({RECT_X + RECT_X / DOUBLENUMBER, RECT_Y + RECT_Y / DOUBLENUMBER},
                       RECT_HEIGHT / DOUBLENUMBER, RECT_WIDTH / DOUBLENUMBER);
    EXPECT_EQ(canvas_->GetStartPos().x, RECT_X + RECT_X / DOUBLENUMBER);
    EXPECT_EQ(canvas_->GetStartPos().y, RECT_Y + RECT_Y / DOUBLENUMBER);

    viewGroup->Remove(canvas_);

    viewGroup->SetPosition(0, 0);
    viewGroup->SetWidth(WIDTH);
    viewGroup->SetHeight(HEIGHT);
    
    canvas_->DrawImage({IMAGE_X, IMAGE_Y}, JPEG_IMAGE_PATH, paint);
    viewGroup->Add(canvas_);
    EXPECT_EQ(canvas_->GetStartPosition().x, IMAGE_X);
    EXPECT_EQ(canvas_->GetStartPosition().y, IMAGE_Y);

    canvas_->DrawImage({IMAGE_X, IMAGE_Y}, JPEG_IMAGE_PATH, paint);
    EXPECT_EQ(canvas_->GetStartPosition().x, IMAGE_X);
    EXPECT_EQ(canvas_->GetStartPosition().y, IMAGE_Y);
    viewGroup->Remove(canvas_);
}

/**
 * @tc.name: UICanvasDrawImage_002
 * @tc.desc: Verify DrawImage function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasDrawImage_002, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    RootView* rootView = RootView::GetInstance();
    UIViewGroup* viewGroup = static_cast<UIViewGroup*>(rootView);
    Paint paint;

    viewGroup->SetPosition(0, 0);
    viewGroup->SetWidth(WIDTH);
    viewGroup->SetHeight(HEIGHT);

    canvas_->DrawImage({0, 0}, JPEG_IMAGE_PATH, paint);
    viewGroup->Add(canvas_);
    EXPECT_EQ(canvas_->GetStartPosition().x, 0);
    EXPECT_EQ(canvas_->GetStartPosition().y, 0);

    canvas_->ClearRect({RECT_X + RECT_X / DOUBLENUMBER, RECT_Y + RECT_Y / DOUBLENUMBER},
                       RECT_HEIGHT / DOUBLENUMBER, RECT_WIDTH / DOUBLENUMBER);
    EXPECT_EQ(canvas_->GetStartPos().x, RECT_X + RECT_X / DOUBLENUMBER);
    EXPECT_EQ(canvas_->GetStartPos().y, RECT_Y + RECT_Y / DOUBLENUMBER);

    viewGroup->Remove(canvas_);

    viewGroup->SetPosition(0, 0);
    viewGroup->SetWidth(WIDTH);
    viewGroup->SetHeight(HEIGHT);

    canvas_->DrawImage({IMAGE_X, IMAGE_Y}, PNG_1_PALETTE_IMAGE_PATH, paint);
    viewGroup->Add(canvas_);
    EXPECT_EQ(canvas_->GetStartPosition().x, IMAGE_X);
    EXPECT_EQ(canvas_->GetStartPosition().y, IMAGE_Y);

    canvas_->DrawImage({IMAGE_X, IMAGE_Y}, PNG_1_PALETTE_IMAGE_PATH, paint);
    EXPECT_EQ(canvas_->GetStartPosition().x, IMAGE_X);
    EXPECT_EQ(canvas_->GetStartPosition().y, IMAGE_Y);
    viewGroup->Remove(canvas_);
}

/**
 * @tc.name: UICanvasDrawImage_003
 * @tc.desc: Verify DrawImage function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasDrawImage_003, TestSize.Level1)
{
    // ͼ�β���
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    RootView* rootView = RootView::GetInstance();
    UIViewGroup* viewGroup = static_cast<UIViewGroup*>(rootView);
    Paint paint;

    viewGroup->SetPosition(0, 0);
    viewGroup->SetWidth(WIDTH);
    viewGroup->SetHeight(HEIGHT);

    canvas_->DrawImage({0, 0}, JPEG_IMAGE_PATH, paint);
    viewGroup->Add(canvas_);
    EXPECT_EQ(canvas_->GetStartPosition().x, 0);
    EXPECT_EQ(canvas_->GetStartPosition().y, 0);

    canvas_->ClearRect({RECT_X + RECT_X / DOUBLENUMBER, RECT_Y + RECT_Y / DOUBLENUMBER},
                       RECT_HEIGHT / DOUBLENUMBER, RECT_WIDTH / DOUBLENUMBER);
    EXPECT_EQ(canvas_->GetStartPos().x, RECT_X + RECT_X / DOUBLENUMBER);
    EXPECT_EQ(canvas_->GetStartPos().y, RECT_Y + RECT_Y / DOUBLENUMBER);

    viewGroup->Remove(canvas_);

    viewGroup->SetPosition(0, 0);
    viewGroup->SetWidth(WIDTH);
    viewGroup->SetHeight(HEIGHT);

    canvas_->DrawImage({IMAGE_X, IMAGE_Y}, GIF_IMAGE_PATH, paint);
    viewGroup->Add(canvas_);
    EXPECT_EQ(canvas_->GetStartPosition().x, IMAGE_X);
    EXPECT_EQ(canvas_->GetStartPosition().y, IMAGE_Y);

    canvas_->DrawImage({IMAGE_X, IMAGE_Y}, GIF_IMAGE_PATH, paint);
    EXPECT_EQ(canvas_->GetStartPosition().x, IMAGE_X);
    EXPECT_EQ(canvas_->GetStartPosition().y, IMAGE_Y);
    viewGroup->Remove(canvas_);
}
#endif

#if GRAPHIC_ENABLE_DRAW_TEXT_FLAG
/**
 * @tc.name: UICanvasStrokeText_001
 * @tc.desc: Verify StrokeText function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasStrokeText_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_CENTER;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE;
    fontStyle.letterSpace = LETTER_SPACE;

    EXPECT_EQ(fontStyle.align, TEXT_ALIGNMENT_CENTER);
    EXPECT_EQ(fontStyle.direct, TEXT_DIRECT_LTR);
    EXPECT_EQ(fontStyle.fontName, DEFAULT_VECTOR_FONT_FILENAME);
    EXPECT_EQ(fontStyle.fontSize, FONT_SIZE);
    EXPECT_EQ(fontStyle.letterSpace, LETTER_SPACE);
}

/**
 * @tc.name: UICanvasStrokeText_002
 * @tc.desc: Verify StrokeText function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasStrokeText_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_CENTER;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE;
    fontStyle.letterSpace = LETTER_SPACE;
    canvas_->StrokeText("hello world", {POS_X, POS_Y}, fontStyle, *paint_);

    EXPECT_EQ(fontStyle.align, TEXT_ALIGNMENT_CENTER);
    EXPECT_EQ(fontStyle.direct, TEXT_DIRECT_LTR);
    EXPECT_EQ(fontStyle.fontName, DEFAULT_VECTOR_FONT_FILENAME);
    EXPECT_EQ(fontStyle.fontSize, FONT_SIZE);
    EXPECT_EQ(fontStyle.letterSpace, LETTER_SPACE);
    EXPECT_EQ(canvas_->GetStartPosition().x, POS_X);
    EXPECT_EQ(canvas_->GetStartPosition().y, POS_Y);
}
#endif

/**
 * @tc.name: UICanvasMeasureText_001
 * @tc.desc: Verify MeasureText function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasMeasureText_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_CENTER;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE;
    fontStyle.letterSpace = LETTER_SPACE;

    EXPECT_EQ(fontStyle.align, TEXT_ALIGNMENT_CENTER);
    EXPECT_EQ(fontStyle.direct, TEXT_DIRECT_LTR);
    EXPECT_EQ(fontStyle.fontName, DEFAULT_VECTOR_FONT_FILENAME);
    EXPECT_EQ(fontStyle.fontSize, FONT_SIZE);
    EXPECT_EQ(fontStyle.letterSpace, LETTER_SPACE);
}

/**
 * @tc.name: UICanvasMeasureText_001
 * @tc.desc: Verify MeasureText function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasMeasureText_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_CENTER;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE;
    fontStyle.letterSpace = LETTER_SPACE;
    canvas_->MeasureText("hello world", fontStyle);

    EXPECT_EQ(fontStyle.align, TEXT_ALIGNMENT_CENTER);
    EXPECT_EQ(fontStyle.direct, TEXT_DIRECT_LTR);
    EXPECT_EQ(fontStyle.fontName, DEFAULT_VECTOR_FONT_FILENAME);
    EXPECT_EQ(fontStyle.fontSize, FONT_SIZE);
    EXPECT_EQ(fontStyle.letterSpace, LETTER_SPACE);
}

/**
 * @tc.name: UICanvasSetScale_001
 * @tc.desc: Verify SetScale function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetScale_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_CENTER;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE;
    fontStyle.letterSpace = LETTER_SPACE;

    paint_->Scale(SCALE_X, SCALE_Y);

    EXPECT_EQ(fontStyle.align, TEXT_ALIGNMENT_CENTER);
    EXPECT_EQ(fontStyle.direct, TEXT_DIRECT_LTR);
    EXPECT_EQ(fontStyle.fontName, DEFAULT_VECTOR_FONT_FILENAME);
    EXPECT_EQ(fontStyle.fontSize, FONT_SIZE);
    EXPECT_EQ(fontStyle.letterSpace, LETTER_SPACE);
    EXPECT_EQ(paint_->GetScaleX(), SCALE_X);
    EXPECT_EQ(paint_->GetScaleY(), SCALE_Y);
}

/**
 * @tc.name: UICanvasSetScale_002
 * @tc.desc: Verify SetScale function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetScale_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_CENTER;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE;
    fontStyle.letterSpace = LETTER_SPACE;

    paint_->Scale(SCALE_X, SCALE_Y);
#if GRAPHIC_ENABLE_DRAW_TEXT_FLAG
    canvas_->StrokeText("hello world", {POS_X, POS_Y}, fontStyle, *paint_);
#endif

    EXPECT_EQ(fontStyle.align, TEXT_ALIGNMENT_CENTER);
    EXPECT_EQ(fontStyle.direct, TEXT_DIRECT_LTR);
    EXPECT_EQ(fontStyle.fontName, DEFAULT_VECTOR_FONT_FILENAME);
    EXPECT_EQ(fontStyle.fontSize, FONT_SIZE);
    EXPECT_EQ(fontStyle.letterSpace, LETTER_SPACE);
    EXPECT_EQ(paint_->GetScaleX(), SCALE_X);
    EXPECT_EQ(paint_->GetScaleY(), SCALE_Y);
}

/**
 * @tc.name: UICanvasSetTranslate_001
 * @tc.desc: Verify SetTranslate function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetTranslate_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_CENTER;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE;
    fontStyle.letterSpace = LETTER_SPACE;

    paint_->Translate(TRANSLATE_X, TRANSLATE_Y);

    EXPECT_EQ(fontStyle.align, TEXT_ALIGNMENT_CENTER);
    EXPECT_EQ(fontStyle.direct, TEXT_DIRECT_LTR);
    EXPECT_EQ(fontStyle.fontName, DEFAULT_VECTOR_FONT_FILENAME);
    EXPECT_EQ(fontStyle.fontSize, FONT_SIZE);
    EXPECT_EQ(fontStyle.letterSpace, LETTER_SPACE);
    EXPECT_EQ(paint_->GetTranslateX(), TRANSLATE_X);
    EXPECT_EQ(paint_->GetTranslateY(), TRANSLATE_Y);
}

/**
 * @tc.name: UICanvasSetTranslate_002
 * @tc.desc: Verify SetTranslate function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetTranslate_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_CENTER;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE;
    fontStyle.letterSpace = LETTER_SPACE;

    paint_->Translate(TRANSLATE_X, TRANSLATE_Y);
#if GRAPHIC_ENABLE_DRAW_TEXT_FLAG
    canvas_->StrokeText("hello world", {POS_X, POS_Y}, fontStyle, *paint_);
#endif

    EXPECT_EQ(fontStyle.align, TEXT_ALIGNMENT_CENTER);
    EXPECT_EQ(fontStyle.direct, TEXT_DIRECT_LTR);
    EXPECT_EQ(fontStyle.fontName, DEFAULT_VECTOR_FONT_FILENAME);
    EXPECT_EQ(fontStyle.fontSize, FONT_SIZE);
    EXPECT_EQ(fontStyle.letterSpace, LETTER_SPACE);
    EXPECT_EQ(paint_->GetTranslateX(), TRANSLATE_X);
    EXPECT_EQ(paint_->GetTranslateY(), TRANSLATE_Y);
}

/**
 * @tc.name: UICanvasTransform_001
 * @tc.desc: Verify Transform function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasTransform_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_CENTER;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE;
    fontStyle.letterSpace = LETTER_SPACE;

    paint_->Transform(SCALE_X, SHEAR_X, SHEAR_Y, SCALE_Y, TRANSLATE_X, TRANSLATE_Y);

    EXPECT_EQ(fontStyle.align, TEXT_ALIGNMENT_CENTER);
    EXPECT_EQ(fontStyle.direct, TEXT_DIRECT_LTR);
    EXPECT_EQ(fontStyle.fontName, DEFAULT_VECTOR_FONT_FILENAME);
    EXPECT_EQ(fontStyle.fontSize, FONT_SIZE);
    EXPECT_EQ(fontStyle.letterSpace, LETTER_SPACE);
    EXPECT_EQ(paint_->GetScaleX(), SCALE_X);
    EXPECT_EQ(paint_->GetScaleY(), SCALE_Y);
    EXPECT_EQ(paint_->GetTranslateX(), TRANSLATE_X);
    EXPECT_EQ(paint_->GetTranslateY(), TRANSLATE_Y);
}

/**
 * @tc.name: UICanvasTransform_002
 * @tc.desc: Verify Transform function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasTransform_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_CENTER;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE;
    fontStyle.letterSpace = LETTER_SPACE;

    paint_->Transform(SCALE_X, SHEAR_X, SHEAR_Y, SCALE_Y, TRANSLATE_X, TRANSLATE_Y);
#if GRAPHIC_ENABLE_DRAW_TEXT_FLAG
    canvas_->StrokeText("hello world", {POS_X, POS_Y}, fontStyle, *paint_);
#endif

    EXPECT_EQ(fontStyle.align, TEXT_ALIGNMENT_CENTER);
    EXPECT_EQ(fontStyle.direct, TEXT_DIRECT_LTR);
    EXPECT_EQ(fontStyle.fontName, DEFAULT_VECTOR_FONT_FILENAME);
    EXPECT_EQ(fontStyle.fontSize, FONT_SIZE);
    EXPECT_EQ(fontStyle.letterSpace, LETTER_SPACE);
    EXPECT_EQ(paint_->GetScaleX(), SCALE_X);
    EXPECT_EQ(paint_->GetScaleY(), SCALE_Y);
    EXPECT_EQ(paint_->GetTranslateX(), TRANSLATE_X);
    EXPECT_EQ(paint_->GetTranslateY(), TRANSLATE_Y);
}

/**
 * @tc.name: UICanvasSetTransform_001
 * @tc.desc: Verify SetTransform function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetTransform_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_CENTER;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE;
    fontStyle.letterSpace = LETTER_SPACE;

    paint_->SetTransform(SCALE_X, SHEAR_X, SHEAR_Y, SCALE_Y, TRANSLATE_X, TRANSLATE_Y);

    EXPECT_EQ(fontStyle.align, TEXT_ALIGNMENT_CENTER);
    EXPECT_EQ(fontStyle.direct, TEXT_DIRECT_LTR);
    EXPECT_EQ(fontStyle.fontName, DEFAULT_VECTOR_FONT_FILENAME);
    EXPECT_EQ(fontStyle.fontSize, FONT_SIZE);
    EXPECT_EQ(fontStyle.letterSpace, LETTER_SPACE);
    EXPECT_EQ(paint_->GetScaleX(), SCALE_X);
    EXPECT_EQ(paint_->GetScaleY(), SCALE_Y);
    EXPECT_EQ(paint_->GetTranslateX(), TRANSLATE_X);
    EXPECT_EQ(paint_->GetTranslateY(), TRANSLATE_Y);
}

/**
 * @tc.name: UICanvasSetTransform_002
 * @tc.desc: Verify SetTransform function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetTransform_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_CENTER;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE;
    fontStyle.letterSpace = LETTER_SPACE;

    paint_->SetTransform(SCALE_X, SHEAR_X, SHEAR_Y, SCALE_Y, TRANSLATE_X, TRANSLATE_Y);
#if GRAPHIC_ENABLE_DRAW_TEXT_FLAG
    canvas_->StrokeText("hello world", {POS_X, POS_Y}, fontStyle, *paint_);
#endif

    EXPECT_EQ(fontStyle.align, TEXT_ALIGNMENT_CENTER);
    EXPECT_EQ(fontStyle.direct, TEXT_DIRECT_LTR);
    EXPECT_EQ(fontStyle.fontName, DEFAULT_VECTOR_FONT_FILENAME);
    EXPECT_EQ(fontStyle.fontSize, FONT_SIZE);
    EXPECT_EQ(fontStyle.letterSpace, LETTER_SPACE);
    EXPECT_EQ(paint_->GetScaleX(), SCALE_X);
    EXPECT_EQ(paint_->GetScaleY(), SCALE_Y);
    EXPECT_EQ(paint_->GetTranslateX(), TRANSLATE_X);
    EXPECT_EQ(paint_->GetTranslateY(), TRANSLATE_Y);
}

/**
 * @tc.name: UICanvasGlobalAlpha_001
 * @tc.desc: Verify GlobalAlpha function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasGlobalAlpha_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    paint_->SetGlobalAlpha(GLOBALALPHA);
    EXPECT_EQ(paint_->GetGlobalAlpha(), GLOBALALPHA);
}

/**
 * @tc.name: UICanvasGlobalAlpha_002
 * @tc.desc: Verify GlobalAlpha function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasGlobalAlpha_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    ColorType color = Color::Red();
    paint_->SetStyle(Paint::PaintStyle::STROKE_FILL_STYLE);
    paint_->SetFillColor(color);
    canvas_->DrawRect({RECT_X, RECT_Y}, RECT_WIDTH, RECT_HEIGHT, *paint_);
    paint_->SetStyle(Paint::PaintStyle::FILL_STYLE);
    color = Color::Green();
    paint_->SetFillColor(color);
    paint_->SetGlobalAlpha(GLOBALALPHA);
    canvas_->DrawRect({POS_X, POS_Y}, WIDTH, HEIGHT, *paint_);

    EXPECT_EQ(paint_->GetGlobalAlpha(), GLOBALALPHA);
}

/**
 * @tc.name: UICanvasGetGlobalAlpha_001
 * @tc.desc: Verify GetGlobalAlpha function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasGetGlobalAlpha_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    paint_->SetGlobalAlpha(GLOBALALPHA);
    EXPECT_EQ(paint_->GetGlobalAlpha(), GLOBALALPHA);
}

/**
 * @tc.name: UICanvasGetGlobalAlpha_002
 * @tc.desc: Verify GetGlobalAlpha function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasGetGlobalAlpha_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    ColorType color = Color::Green();
    paint_->SetStyle(Paint::PaintStyle::FILL_STYLE);
    paint_->SetFillColor(color);
    paint_->SetGlobalAlpha(GLOBALALPHA);
    canvas_->DrawRect({POS_X, POS_Y}, WIDTH, HEIGHT, *paint_);

    EXPECT_EQ(paint_->GetGlobalAlpha(), GLOBALALPHA);
}

/**
 * @tc.name: UICanvasSetGlobalCompositeOperation_001
 * @tc.desc: Verify SetGlobalCompositeOperation function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetGlobalCompositeOperation_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    paint_->SetGlobalCompositeOperation(OHOS::COPY);
    EXPECT_EQ(paint_->GetGlobalCompositeOperation(), OHOS::COPY);
}

/**
 * @tc.name: UICanvasSetGlobalCompositeOperation_002
 * @tc.desc: Verify SetGlobalCompositeOperation function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetGlobalCompositeOperation_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    paint_->SetFillColor(Color::Blue());
    canvas_->BeginPath();
    canvas_->MoveTo({START1_X, START1_Y});
    canvas_->LineTo({START1_X, POS_Y});
    canvas_->LineTo({POS_X, POS_Y});
    canvas_->LineTo({POS_X, START1_Y});
    canvas_->ClosePath();
    canvas_->FillPath(*paint_);
    paint_->SetGlobalCompositeOperation(OHOS::COPY);

    paint_->SetFillColor(Color::Green());
    canvas_->BeginPath();
    canvas_->MoveTo({LINE_X, LINE_X});
    canvas_->LineTo({LINE_X, LINE_Y});
    canvas_->LineTo({LINE_Y, LINE_Y});
    canvas_->LineTo({LINE_Y, LINE_X});
    canvas_->ClosePath();
    canvas_->FillPath(*paint_);

    EXPECT_EQ(paint_->GetGlobalCompositeOperation(), OHOS::COPY);
}

/**
 * @tc.name: UICanvasGetGlobalCompositeOperation_001
 * @tc.desc: Verify GetGlobalCompositeOperation function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasGetGlobalCompositeOperation_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    paint_->SetGlobalCompositeOperation(OHOS::SOURCE_OVER);
    EXPECT_EQ(paint_->GetGlobalCompositeOperation(), OHOS::SOURCE_OVER);
}

/**
 * @tc.name: UICanvasGetGlobalCompositeOperation_002
 * @tc.desc: Verify GetGlobalCompositeOperation function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasGetGlobalCompositeOperation_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    
    paint_->SetFillColor(Color::Blue());
    canvas_->BeginPath();
    canvas_->MoveTo({LINE_X, LINE_X});
    canvas_->LineTo({LINE_X, LINE_Y});
    canvas_->LineTo({LINE_Y, LINE_Y});
    canvas_->LineTo({LINE_Y, LINE_X});
    canvas_->ClosePath();
    canvas_->FillPath(*paint_);
    paint_->SetGlobalCompositeOperation(OHOS::SOURCE_OVER);
    
    paint_->SetFillColor(Color::Green());
    canvas_->BeginPath();
    canvas_->MoveTo({START1_X, START1_Y});
    canvas_->LineTo({START1_X, POS_Y});
    canvas_->LineTo({POS_X, POS_Y});
    canvas_->LineTo({POS_X, START1_Y});
    canvas_->ClosePath();
    canvas_->FillPath(*paint_);
    EXPECT_EQ(paint_->GetGlobalCompositeOperation(), OHOS::SOURCE_OVER);
}

/**
 * @tc.name: UICanvasSave_001
 * @tc.desc: Verify Save function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSave_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    ColorType color = Color::Red();
    paint_->SetFillColor(color);
    canvas_->Save(*paint_);
    EXPECT_EQ(paint_->GetFillColor().full, color.full);
}

/**
 * @tc.name: UICanvasSave_002
 * @tc.desc: Verify Save function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSave_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    ColorType color = Color::Red();
    paint_->SetFillColor(color);
    canvas_->Save(*paint_);
    canvas_->DrawRect({RECT_X, RECT_Y}, RECT_WIDTH, RECT_HEIGHT, *paint_);
    *paint_ = canvas_->Restore();
    canvas_->DrawRect({POS_X, POS_Y}, WIDTH, HEIGHT, *paint_);

    EXPECT_EQ(paint_->GetFillColor().full, color.full);
}

/**
 * @tc.name: UICanvasRestore_001
 * @tc.desc: Verify Restore function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasRestore_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    ColorType color = Color::Red();
    paint_->SetFillColor(color);
    canvas_->Save(*paint_);
    *paint_ = canvas_->Restore();

    EXPECT_EQ(paint_->GetFillColor().full, color.full);
}

/**
 * @tc.name: UICanvasRestore_002
 * @tc.desc: Verify Restore function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasRestore_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    ColorType color = Color::Red();
    paint_->SetFillColor(color);
    canvas_->Save(*paint_);
    canvas_->DrawRect({POS_X, POS_Y}, RECT_WIDTH, RECT_HEIGHT, *paint_);
    *paint_ = canvas_->Restore();
    canvas_->DrawRect({RECT_X, RECT_Y}, WIDTH, HEIGHT, *paint_);

    EXPECT_EQ(paint_->GetFillColor().full, color.full);
}
#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
HWTEST_F(UICanvasTest, UICanvasShadowColor_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    ColorType color = Color::Red();
    paint_->SetShadowColor(color);
    EXPECT_EQ(paint_->GetShadowColor().full, color.full);
}
HWTEST_F(UICanvasTest, UICanvasShadowColor_002, TestSize.Level0)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    ColorType color = Color::Red();
    paint_->SetShadowColor(color);
    EXPECT_EQ(paint_->GetShadowColor().full, color.full);
}
#endif

#if GRAPHIC_ENABLE_BLUR_EFFECT_FLAG
HWTEST_F(UICanvasTest, UICanvasShadowBlur_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetShadowBlur(RADIUS);
    EXPECT_EQ(paint_->GetShadowBlur(), RADIUS);
}

HWTEST_F(UICanvasTest, UICanvasShadowBlur_002, TestSize.Level0)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetShadowBlur(RADIUS);
    EXPECT_EQ(paint_->GetShadowBlur(), RADIUS);
}
#endif

#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
HWTEST_F(UICanvasTest, UICanvasShadowOffsetX_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetShadowOffsetX(RADIUS);
    EXPECT_EQ(paint_->GetShadowOffsetX(), RADIUS);
}

HWTEST_F(UICanvasTest, UICanvasShadowOffsetX_002, TestSize.Level0)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetShadowOffsetX(RADIUS);
    EXPECT_EQ(paint_->GetShadowOffsetX(), RADIUS);
}

HWTEST_F(UICanvasTest, UICanvasShadowOffsetY_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetShadowOffsetY(RADIUS);
    EXPECT_EQ(paint_->GetShadowOffsetY(), RADIUS);
}

HWTEST_F(UICanvasTest, UICanvasShadowOffsetY_002, TestSize.Level0)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetShadowOffsetY(RADIUS);
    EXPECT_EQ(paint_->GetShadowOffsetY(), RADIUS);
}
#endif

#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
HWTEST_F(UICanvasTest, UICanvasCreateLinearGradient_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->createLinearGradient(START_X, START_Y, END_X, END_Y);
    paint_->SetStyle(Paint::GRADIENT);
    EXPECT_EQ(paint_->GetLinearGradientPoint().x0, START_X);
    EXPECT_EQ(paint_->GetLinearGradientPoint().y0, START_Y);
    EXPECT_EQ(paint_->GetLinearGradientPoint().x1, END_X);
    EXPECT_EQ(paint_->GetLinearGradientPoint().y1, END_Y);
}

HWTEST_F(UICanvasTest, UICanvasCreateLinearGradient_002, TestSize.Level0)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->createLinearGradient(START_X, START_Y, END_X, END_Y);
    paint_->SetStyle(Paint::GRADIENT);
    EXPECT_EQ(paint_->GetLinearGradientPoint().x0, START_X);
    EXPECT_EQ(paint_->GetLinearGradientPoint().y0, START_Y);
    EXPECT_EQ(paint_->GetLinearGradientPoint().x1, END_X);
    EXPECT_EQ(paint_->GetLinearGradientPoint().y1, END_Y);
}

HWTEST_F(UICanvasTest, UICanvasCreateRadialGradient_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->createRadialGradient(START_X, START_Y, START_R, END_X, END_Y, END_R);
    paint_->SetStyle(Paint::GRADIENT);
    EXPECT_EQ(paint_->GetRadialGradientPoint().x0, START_X);
    EXPECT_EQ(paint_->GetRadialGradientPoint().y0, START_Y);
    EXPECT_EQ(paint_->GetRadialGradientPoint().r0, START_R);
    EXPECT_EQ(paint_->GetRadialGradientPoint().x1, END_X);
    EXPECT_EQ(paint_->GetRadialGradientPoint().y1, END_Y);
    EXPECT_EQ(paint_->GetRadialGradientPoint().r1, END_R);
}

HWTEST_F(UICanvasTest, UICanvasCreateRadialGradient_002, TestSize.Level0)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->createRadialGradient(START_X, START_Y, START_R, END_X, END_Y, END_R);
    paint_->SetStyle(Paint::GRADIENT);
    EXPECT_EQ(paint_->GetRadialGradientPoint().x0, START_X);
    EXPECT_EQ(paint_->GetRadialGradientPoint().y0, START_Y);
    EXPECT_EQ(paint_->GetRadialGradientPoint().r0, START_R);
    EXPECT_EQ(paint_->GetRadialGradientPoint().x1, END_X);
    EXPECT_EQ(paint_->GetRadialGradientPoint().y1, END_Y);
    EXPECT_EQ(paint_->GetRadialGradientPoint().r1, END_R);
}
#endif

#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
HWTEST_F(UICanvasTest, UICanvasCreatePattern_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->CreatePattern(RED_IMAGE_PATH, REPEAT);
    EXPECT_EQ(paint_->GetPatternImage(), RED_IMAGE_PATH);
    EXPECT_EQ(paint_->GetPatternRepeatMode(), REPEAT);
}

HWTEST_F(UICanvasTest, UICanvasCreatePattern_002, TestSize.Level0)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->CreatePattern(RED_IMAGE_PATH, REPEAT);
    EXPECT_EQ(paint_->GetPatternImage(), RED_IMAGE_PATH);
    EXPECT_EQ(paint_->GetPatternRepeatMode(), REPEAT);
}
#endif

#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
HWTEST_F(UICanvasTest, UICanvasAddColorStop_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    ColorType colorStop0 = Color::Yellow();
    ColorType colorStop1 = Color::White();
    ColorType colorStop2 = Color::Green();
    ColorType colorStop3 = Color::Blue();
    paint_->createLinearGradient(START_X, START_Y, END_X, END_Y);
    paint_->addColorStop(COLOR_STOP_0, colorStop0);
    paint_->addColorStop(COLOR_STOP_1, colorStop1);
    paint_->addColorStop(COLOR_STOP_2, colorStop2);
    paint_->addColorStop(COLOR_STOP_3, colorStop3);
    List<Paint::StopAndColor> stopAndColor = paint_->getStopAndColor();
    EXPECT_EQ(stopAndColor.Head()->data_.stop, COLOR_STOP_0);
    EXPECT_EQ(stopAndColor.Head()->data_.color.full, colorStop0.full);
    stopAndColor.PopFront();
    EXPECT_EQ(stopAndColor.Head()->data_.stop, COLOR_STOP_1);
    EXPECT_EQ(stopAndColor.Head()->data_.color.full, colorStop1.full);
    stopAndColor.PopFront();
    EXPECT_EQ(stopAndColor.Head()->data_.stop, COLOR_STOP_2);
    EXPECT_EQ(stopAndColor.Head()->data_.color.full, colorStop2.full);
    stopAndColor.PopFront();
    EXPECT_EQ(stopAndColor.Head()->data_.stop, COLOR_STOP_3);
    EXPECT_EQ(stopAndColor.Head()->data_.color.full, colorStop3.full);
    stopAndColor.PopFront();
}

HWTEST_F(UICanvasTest, UICanvasFillStyle_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetStyle(Paint::PaintStyle::GRADIENT);
    EXPECT_EQ(paint_->GetStyle(), Paint::PaintStyle::GRADIENT);
    paint_->SetStyle(Paint::PaintStyle::PATTERN);
    EXPECT_EQ(paint_->GetStyle(), Paint::PaintStyle::PATTERN);
}

HWTEST_F(UICanvasTest, UICanvasStrokeStyle_001, TestSize.Level1)
{
    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    paint_->SetStyle(Paint::PaintStyle::GRADIENT);
    EXPECT_EQ(paint_->GetStyle(), Paint::PaintStyle::GRADIENT);
    paint_->SetStyle(Paint::PaintStyle::PATTERN);
    EXPECT_EQ(paint_->GetStyle(), Paint::PaintStyle::PATTERN);
}
#endif

/**
 * @tc.name: UICanvasSetRotate_001
 * @tc.desc: Verify SetRotate function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetRotate_001, TestSize.Level0)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_CENTER;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE;
    fontStyle.letterSpace = LETTER_SPACE;

    paint_->Rotate(ROTATE);

    EXPECT_EQ(fontStyle.align, TEXT_ALIGNMENT_CENTER);
    EXPECT_EQ(fontStyle.direct, TEXT_DIRECT_LTR);
    EXPECT_EQ(fontStyle.fontName, DEFAULT_VECTOR_FONT_FILENAME);
    EXPECT_EQ(fontStyle.fontSize, FONT_SIZE);
    EXPECT_EQ(fontStyle.letterSpace, LETTER_SPACE);
    EXPECT_EQ(paint_->GetRotateAngle(), ROTATE);
}


/**
 * @tc.name: UICanvasSetRotate_002
 * @tc.desc: Verify SetRotate function, equal.
 * @tc.type: FUNC
 */
HWTEST_F(UICanvasTest, UICanvasSetRotate_002, TestSize.Level1)
{
    if (canvas_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    if (paint_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }

    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_CENTER;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE;
    fontStyle.letterSpace = LETTER_SPACE;

    paint_->Rotate(ROTATE);
#if GRAPHIC_ENABLE_DRAW_TEXT_FLAG
    canvas_->StrokeText("hello world", {POS_X, POS_Y}, fontStyle, *paint_);
#endif

    EXPECT_EQ(fontStyle.align, TEXT_ALIGNMENT_CENTER);
    EXPECT_EQ(fontStyle.direct, TEXT_DIRECT_LTR);
    EXPECT_EQ(fontStyle.fontName, DEFAULT_VECTOR_FONT_FILENAME);
    EXPECT_EQ(fontStyle.fontSize, FONT_SIZE);
    EXPECT_EQ(fontStyle.letterSpace, LETTER_SPACE);
    EXPECT_EQ(paint_->GetRotateAngle(), ROTATE);
}
} // namespace OHOS
