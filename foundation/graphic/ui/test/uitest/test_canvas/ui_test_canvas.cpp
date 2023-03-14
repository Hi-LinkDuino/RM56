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

#include "ui_test_canvas.h"

#include "common/screen.h"
#include "components/ui_label.h"
#include "test_resource_config.h"
namespace OHOS {
void UITestCanvas::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(),
                           Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetHorizontalScrollState(false);
        container_->SetThrowDrag(true);
    }
    positionY_ = 0;
}

void UITestCanvas::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestCanvas::GetTestView()
{
    UIKitCanvasTestDrawImage002();
    RM008UIKitCanvasTest001();
    RM008UIKitCanvasTest002();
    RM008UIKitCanvasTest003();
    RM008UIKitCanvasTest004();
    RM008UIKitCanvasTest005();
    RM008UIKitCanvasTest006();
    RM008UIKitCanvasTest007();
    RM008UIKitCanvasShadowTest008();
    RM009LineCapDrawPath();
    RM009LineJoinDrawPath();
    RM009LineDashDrawPath();
    RM009StrokeAndClearRectDrawPath();
    RM011StrokeText001();
    RM011CanvasScale001();
    RM011CanvasRotate001();
    RM012globalAlpha001();
    RM012SaveOrRestore002();
    RM012GlobalCompositeOperationSourceOver();
    RM012GlobalCompositeOperationSourceAtop();
    RM012GlobalCompositeOperationSourceIn();
    RM012GlobalCompositeOperationSourceOut();
    RM012GlobalCompositeOperationDestinationOver();
    RM012GlobalCompositeOperationDestinationAtop();
    RM012GlobalCompositeOperationDestinationIn();
    RM012GlobalCompositeOperationDestinationOut();
    RM012GlobalCompositeOperationLIGHTER();
    RM012GlobalCompositeOperationCopy();
    RM012GlobalCompositeOperationXOR();
    UIKitCanvasTestDrawLine001();
    UIKitCanvasTestDrawLine002();
    UIKitCanvasTestDrawCurve001();
    UIKitCanvasTestDrawCurve002();
    UIKitCanvasTestDrawRect001();
    UIKitCanvasTestDrawRect002();
    UIKitCanvasTestDrawRect003();
    UIKitCanvasTestDrawCircle001();
    UIKitCanvasTestDrawCircle002();
    UIKitCanvasTestDrawCircle003();
    UIKitCanvasTestDrawArc001();
    UIKitCanvasTestDrawImage001();
    UIKitCanvasTestDrawLabel001();
    UIKitCanvasTestDrawSector001();
    UIKitCanvasTestClear001();
    UIKitCanvasTestDrawPath001();
    UIKitCanvasTestDrawPath002();
    UIKitCanvasTestDrawPath003();
    UIKitCanvasTestDrawPath004();
    UIKitCanvasTestDrawPath005();
    UIKitCanvasTestDrawPath006();
    UIKitCanvasTestDrawPath007();
    UIKitCanvasTestDrawPath008();
    UIKitCanvasTestDrawPath009();
    UIKitCanvasTestDrawPath010();
    UIKitCanvasTestDrawPath011();
    UIKitCanvasTestDrawPath012();
    UIKitCanvasTestDrawPath013();
    UIKitCanvasTestDrawPath014();
    UIKitCanvasTestDrawPath015();
    UIKitCanvasTestDrawPath016();
    UIKitCanvasTestDrawPath017();
    UIKitCanvasTestDrawPath018();
    UIKitCanvasTestDrawPath019();
    UIKitCanvasTestDrawPath020();
    UIKitCanvasTestDrawPath021();
    UIKitCanvasTestDrawPath022();
    UIKitCanvasTestDrawPath023();
    UIKitCanvasTestDrawPath024();
    UIKitCanvasTestDrawPath025();
    UIKitCanvasTestDrawPath026();
    UIKitCanvasTestDrawPath027();
    UIKitCanvasTestDrawPath028();
    UIKitCanvasTestDrawPath029();
    UIKitCanvasTestDrawPath030();
    UIKitCanvasTestDrawPath031();
    UIKitCanvasTestDrawPath032();
    UIKitCanvasTestDrawPath033();
    UIKitCanvasTestDrawPath034();
    return container_;
}

void UITestCanvas::CreateTitleLabel(const char* title)
{
    UILabel* titleLabel = new UILabel();
    titleLabel->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_, Screen::GetInstance().GetWidth(), TITLE_HEIGHT);
    titleLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    titleLabel->SetText(title);
    container_->Add(titleLabel);
    positionY_ += TITLE_HEIGHT + HEIGHT_GAP; // 8: gap
}

UICanvas* UITestCanvas::CreateCanvas()
{
    UICanvas* canvas = new UICanvas();
    canvas->SetHeight(CANVAS_HEIGHT);
    canvas->SetWidth(CANVAS_WIDTH);
    canvas->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, positionY_);
    canvas->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    container_->Add(canvas);
    positionY_ += CANVAS_HEIGHT + GAP;
    return canvas;
}

void UITestCanvas::UIKitCanvasTestDrawLine001()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("绘制直线");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    // {0, 10}: Start point coordinates x, y; {50, 10}: end point coordinates x, y
    canvas->DrawLine({0, START1_X}, {LINE4_X, START1_X}, paint);
}

void UITestCanvas::UIKitCanvasTestDrawLine002()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("绘制直线");
    UICanvas* canvas = CreateCanvas();

    Paint paint;

    paint.SetStrokeWidth(LINE_WIDTH5);            // 5: line width
    canvas->SetStartPosition({START_X, START_Y}); // {50, 10}: Start point coordinates x, y;
    canvas->DrawLine({LINE2_X, RECT_Y}, paint);   // {100, 50}: end point coordinates x, y
}

void UITestCanvas::UIKitCanvasTestDrawCurve001()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("绘制曲线");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    paint.SetStrokeColor(Color::Red());
    canvas->DrawCurve({LINE2_X, RECT_Y}, {LINE12_X, RECT_Y}, {LINE12_X, RECT_Y},
                      {LINE12_X, LINE1_Y}, paint);
}

void UITestCanvas::UIKitCanvasTestDrawCurve002()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("绘制曲线");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    paint.SetStrokeColor(Color::Red());
    canvas->DrawCurve({LINE2_X, LINE_CURVE_Y}, {LINE12_X, LINE_CURVE_Y},
                      {LINE12_X, LINE1_Y}, paint);
}

void UITestCanvas::UIKitCanvasTestDrawRect001()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("矩形填充");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    paint.SetStyle(Paint::PaintStyle::FILL_STYLE);
    paint.SetFillColor(Color::Yellow());
    paint.SetStrokeWidth(LINE_WIDTH30); // 30: line width
    // {100, 10}: left corner coordinates point, 50: width, 50: rectangle style
    canvas->DrawRect({LINE1_Y, START1_Y}, LINE4_X, TRANSLATE_Y, paint);
}

void UITestCanvas::UIKitCanvasTestDrawRect002()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("矩形描边");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    paint.SetStyle(Paint::PaintStyle::STROKE_STYLE);
    paint.SetStrokeColor(Color::Blue());
    // {200, 10}: left corner coordinates point, 50: width, 50: rectangle style
    canvas->DrawRect({LINE28_X, START2_Y}, LINE4_X, LINE_RECT_Y50, paint);
}

void UITestCanvas::UIKitCanvasTestDrawRect003()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("矩形填充 + 描边");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    paint.SetStyle(Paint::PaintStyle::STROKE_FILL_STYLE);
    paint.SetFillColor(Color::Yellow());
    paint.SetStrokeColor(Color::Blue());
    // {300, 10}: left corner coordinates point, 50: width, 50: rectangle style
    canvas->DrawRect({LINE_RECT_X300, LINE_RECT_Y10}, LINE_RECT_WIDTH50, LINE_RECT_HEIGHT50, paint);
}

void UITestCanvas::UIKitCanvasTestDrawCircle001()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("圆形填充");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    paint.SetStyle(Paint::PaintStyle::FILL_STYLE);
    paint.SetStrokeColor(Color::Yellow());
    paint.SetFillColor(Color::Yellow());
    paint.SetStrokeWidth(LINE_WIDTH10); // 10: line width
    paint.SetOpacity(OPARCITY_127);     // 127: opacity
    // {100, 100}: circle center coordinates, 30: circle radius
    canvas->DrawCircle({LINE_CURVE_X100, LINE_CURVE_Y100}, LINE_CURVE_R30, paint);
}

void UITestCanvas::UIKitCanvasTestDrawCircle002()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("圆形描边");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    paint.SetStyle(Paint::PaintStyle::STROKE_STYLE);
    paint.SetStrokeColor(Color::Blue());
    paint.SetStrokeWidth(LINE_WIDTH10); // 10: line width
    paint.SetOpacity(OPARCITY_127);     // 127: opacity
    // {200, 100}: circle center coordinates, 30: circle radius
    canvas->DrawCircle({LINE_CURVE_X200, LINE_CURVE_Y100}, LINE_CURVE_R30, paint);
}

void UITestCanvas::UIKitCanvasTestDrawCircle003()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("圆形填充 + 描边");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    paint.SetStyle(Paint::PaintStyle::STROKE_FILL_STYLE);
    paint.SetFillColor(Color::Yellow());
    paint.SetStrokeColor(Color::Blue());
    paint.SetStrokeWidth(LINE_WIDTH10); // 10: line width
    paint.SetOpacity(OPARCITY_127);     // 127: opacity
    // {300, 100}: circle center coordinates, 30: circle radius
    canvas->DrawCircle({LINE_CURVE_X300, LINE_CURVE_Y100}, LINE_CURVE_R30, paint);
}

void UITestCanvas::UIKitCanvasTestDrawArc001()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("绘制弧线");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    paint.SetStyle(Paint::PaintStyle::STROKE_STYLE);
    paint.SetStrokeColor(Color::Red());
    paint.SetStrokeWidth(LINE_WIDTH10); // 10: line width
    canvas->BeginPath();

    canvas->MoveTo({LINE11_X, LINE13_Y});
    // {100, 150}: arc's center coordinates, 50: arc radius, 135: start angle, 270: end angle
    canvas->DrawArc({ARC_X100, ARC_Y150}, ARC_R50, ARC_ANGLE135, ARC_ANGLE270, paint);
}

void UITestCanvas::UIKitCanvasTestDrawImage001()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("绘制图片");
    // {200, 50}: start point coordinates
#if GRAPHIC_ENABLE_DRAW_IMAGE_FLAG
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    paint.SetOpacity(OPARCITY_127); // 127: opacity

    canvas->DrawImage({0, IMAGE_START60}, RED_IMAGE_PATH, paint);
    paint.SetOpacity(OPA_OPAQUE);
    paint.Rotate(ROTATE_20);
    canvas->DrawImage({IMAGE_START100, IMAGE_START20}, JPEG_IMAGE_PATH, paint);
    canvas->DrawImage({0, 0}, JPEG_IMAGE_PATH, paint, IMAGE_WIDTH100, IMAGE_HEIGHT100);
#endif
}
void UITestCanvas::UIKitCanvasTestDrawImage002()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("绘制图片");
    // {200, 50}: start point coordinates
#if GRAPHIC_ENABLE_DRAW_IMAGE_FLAG
    UICanvas* canvas = CreateCanvas();
    const float scale = 1.5f;
    const int16_t trx = 50;
    Paint paint;
    paint.Rotate(ROTATE20);
    paint.Scale(scale, scale);
    paint.Translate(trx, 0);
    paint.SetOpacity(OPARCITY_127); // 127: opacity
    canvas->DrawImage({ 200, 50 }, GREEN_IMAGE_PATH, paint);
#if GRAPHIC_ENABLE_DRAW_TEXT_FLAG
    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_CENTER;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE15;
    fontStyle.letterSpace = FONT_LETTERSPACE2;
    paint.SetFillColor(Color::Green());
    canvas->StrokeText("CANVAS绘制图片", {0, 0}, fontStyle, paint);
#endif
    paint.Translate(trx, 0);
    canvas->DrawImage({0, 0}, GIF_IMAGE_PATH, paint);
    canvas->DrawImage({0, 0}, GIF_IMAGE_PATH, paint, DRAWIMAGE_WIDTH, DRAWIMAGE_HEIGHT);
#endif
}

void UITestCanvas::UIKitCanvasTestDrawLabel001()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("绘制文字");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    paint.SetFillColor(Color::Blue());
    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_RIGHT;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE30;         // 30: font size
    fontStyle.letterSpace = FONT_LETTERSPACE; // 10 letter space
    // {50, 50}: start point coordinates, 100: max width
    canvas->DrawLabel({LABEL_START50, LABEL_START50}, "canvas绘制字体", LABEL_WIDTH100, fontStyle, paint);
}

void UITestCanvas::UIKitCanvasTestDrawSector001()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("扇形填充");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    paint.SetStyle(Paint::PaintStyle::FILL_STYLE);
    paint.SetFillColor(Color::Yellow());
    // {350, 150}: sector's center coordinates, 100: sector radius, 0: start angle, 30: end angle
    canvas->DrawSector({START_X350, START_Y150}, ARC_R100, 0, END_ANGLE30, paint);
}

void UITestCanvas::UIKitCanvasTestClear001()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("清空画布，无显示");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    // {0, 10}: Start point coordinates x, y; {50, 10}: end point coordinates x, y
    canvas->DrawLine({0, START_Y}, {TRANSLATE_X, START_Y}, paint);
    canvas->Clear();
}

void UITestCanvas::UIKitCanvasTestDrawPath001()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("moveTo，无显示");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath002()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("lineTo，无显示");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->LineTo({LINE1_X, LINE1_Y});
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath003()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("arc");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->ArcTo({CENTER_X, CENTER_Y}, RADIUS, START_ANGLE, END_ANGLE);
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath004()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("rect");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->AddRect({RECT_X, RECT_Y}, RECT_HEIGHT, RECT_WIDTH);
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath005()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("closePath，无显示");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->ClosePath();
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath006()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("moveTo + lineTo");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->LineTo({LINE1_X, LINE1_Y});
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath007()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("moveTo + arc");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->ArcTo({CENTER_X, CENTER_Y}, RADIUS, START_ANGLE, END_ANGLE);
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath008()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("moveTo + rect");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->AddRect({RECT_X, RECT_Y}, RECT_WIDTH, RECT_HEIGHT);
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath009()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("moveTo + closePath");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->LineTo({LINE1_X, LINE1_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath010()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("闭合路径调用closePath");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->LineTo({LINE1_X, LINE1_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->LineTo({START1_X, START1_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath011()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("moveTo + lineTo + moveTo + lineTo");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->LineTo({LINE1_X, LINE1_Y});
    canvas->MoveTo({START2_X, START2_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath012()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("moveTo + lineTo + arc");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->LineTo({LINE1_X, LINE1_Y});
    canvas->ArcTo({CENTER_X, CENTER_Y}, RADIUS, START_ANGLE, END_ANGLE);
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath013()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("moveTo + lineTo + arc + closePath");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->LineTo({LINE1_X, LINE1_Y});
    canvas->ArcTo({CENTER_X, CENTER_Y}, RADIUS, START_ANGLE, END_ANGLE);
    canvas->ClosePath();
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath014()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("moveTo + lineTo + rect");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->LineTo({LINE1_X, LINE1_Y});
    canvas->AddRect({RECT_X, RECT_Y}, RECT_HEIGHT, RECT_WIDTH);
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath015()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("moveTo + lineTo + rect + closePath");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->LineTo({LINE1_X, LINE1_Y});
    canvas->AddRect({RECT_X, RECT_Y}, RECT_HEIGHT, RECT_WIDTH);
    canvas->ClosePath();
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath016()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("rect + lineTo");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->AddRect({RECT_X, RECT_Y}, RECT_HEIGHT, RECT_WIDTH);
    canvas->LineTo({LINE1_X, LINE1_Y});
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath017()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("rect + moveTo + lineTo");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->AddRect({RECT_X, RECT_Y}, RECT_HEIGHT, RECT_WIDTH);
    canvas->MoveTo({START1_X, START1_Y});
    canvas->LineTo({LINE1_X, LINE1_Y});
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath018()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("rect + arc");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->AddRect({RECT_X, RECT_Y}, RECT_HEIGHT, RECT_WIDTH);
    canvas->ArcTo({CENTER_X, CENTER_Y}, RADIUS, START_ANGLE, END_ANGLE);
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath019()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("arc + rect");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->ArcTo({CENTER_X, CENTER_Y}, RADIUS, START_ANGLE, END_ANGLE);
    canvas->AddRect({RECT_X, RECT_Y}, RECT_HEIGHT, RECT_WIDTH);
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath020()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("moveTo + arc + closePath + lineTo");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->ArcTo({CENTER_X, CENTER_Y}, RADIUS, START_ANGLE, END_ANGLE);
    canvas->ClosePath();
    canvas->LineTo({LINE1_X, LINE1_Y});
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath021()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("不调用beginPath，无显示");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->MoveTo({START1_X, START1_Y});
    canvas->ArcTo({CENTER_X, CENTER_Y}, RADIUS, START_ANGLE, END_ANGLE);
    canvas->ClosePath();
    canvas->LineTo({LINE1_X, LINE1_Y});
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath022()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("不调用drawPath，无显示");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->ArcTo({CENTER_X, CENTER_Y}, RADIUS, START_ANGLE, END_ANGLE);
    canvas->ClosePath();
    canvas->LineTo({LINE1_X, LINE1_Y});
}

void UITestCanvas::UIKitCanvasTestDrawPath023()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("moveTo + lineTo + closePath");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->LineTo({LINE1_X, LINE1_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath024()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("moveTo + closePath，无显示");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath025()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("多次drawPath");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->LineTo({LINE1_X, LINE1_Y});
    canvas->DrawPath(paint);
    paint.SetStrokeColor(Color::Blue());
    paint.SetStrokeWidth(1);
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath026()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("arc起止角度互换");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->ArcTo({CENTER_X, CENTER_Y}, RADIUS, START_ANGLE, END_ANGLE);

    canvas->MoveTo({START2_X, START2_Y});
    canvas->ArcTo({int16_t(CENTER_X + CENTER_X), CENTER_Y}, RADIUS, END_ANGLE, START_ANGLE);

    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath027()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("arc扫描范围超过360度 ");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->ArcTo({CENTER_X, CENTER_Y},
                  RADIUS, START_ANGLE, START_ANGLE + CIRCLE_IN_DEGREE + QUARTER_IN_DEGREE);
    canvas->ArcTo({int16_t(CENTER_X + CENTER_X), CENTER_Y}, RADIUS, END_ANGLE,
                  START_ANGLE + CIRCLE_IN_DEGREE);

    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath028()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("创建两条路径，只绘制后一条 ");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->ArcTo({CENTER_X, CENTER_Y}, RADIUS, START_ANGLE, START_ANGLE + CIRCLE_IN_DEGREE);

    canvas->BeginPath();
    canvas->MoveTo({START2_X, START2_Y});
    canvas->ArcTo({int16_t(CENTER_X + CENTER_X), CENTER_Y}, RADIUS, END_ANGLE,
                  START_ANGLE + CIRCLE_IN_DEGREE);

    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath029()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("绘制两条不同样式的路径 ");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->ArcTo({CENTER_X, CENTER_Y}, RADIUS, START_ANGLE, START_ANGLE + CIRCLE_IN_DEGREE);
    canvas->DrawPath(paint);

    canvas->BeginPath();
    canvas->MoveTo({START2_X, START2_Y});
    canvas->ArcTo({int16_t(CENTER_X + CENTER_X), CENTER_Y}, RADIUS, END_ANGLE,
                  START_ANGLE + CIRCLE_IN_DEGREE);
    paint.SetStrokeColor(Color::Blue());
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath030()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("同一条路径绘制100遍 ");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->LineTo({LINE1_X, LINE1_Y});
    canvas->ArcTo({CENTER_X, CENTER_Y}, RADIUS, START_ANGLE, START_ANGLE + CIRCLE_IN_DEGREE);
    canvas->AddRect({RECT_X, RECT_Y}, RECT_HEIGHT, RECT_WIDTH);
    for (uint8_t i = 0; i < 100; i++) { // 100: number of times for drawing loops
        canvas->DrawPath(paint);
    }
}

void UITestCanvas::UIKitCanvasTestDrawPath031()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("绘制直线超出canvas范围");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({START1_X, START1_Y});
    canvas->LineTo({int16_t(LINE1_X + HORIZONTAL_RESOLUTION), LINE1_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath032()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("绘制直线传入临界值 ");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->MoveTo({INT16_MAX, INT16_MAX});
    canvas->LineTo({0, 0});

    canvas->MoveTo({0, 0});
    canvas->LineTo({INT16_MAX, INT16_MAX});

    canvas->MoveTo({INT16_MIN, INT16_MIN});
    canvas->LineTo({0, 0});

    canvas->MoveTo({0, 0});
    canvas->LineTo({INT16_MIN, INT16_MIN});
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath033()
{
    if (container_ == nullptr) {
        return;
    }

    CreateTitleLabel("绘制arc传入临界值 ");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->ArcTo({INT16_MAX, INT16_MAX}, RADIUS, START_ANGLE, END_ANGLE);
    canvas->DrawPath(paint);

    canvas->BeginPath();
    canvas->ArcTo({INT16_MIN, INT16_MIN}, RADIUS, START_ANGLE, END_ANGLE);
    paint.SetStrokeColor(Color::Red());
    canvas->DrawPath(paint);

    canvas->BeginPath();
    canvas->ArcTo({CENTER_X, CENTER_Y}, UINT16_MAX, START_ANGLE, END_ANGLE);
    paint.SetStrokeColor(Color::Yellow());
    canvas->DrawPath(paint);

    canvas->BeginPath();
    canvas->ArcTo({int16_t(CENTER_X + CENTER_X), CENTER_Y}, RADIUS, INT16_MIN, INT16_MAX);
    paint.SetStrokeColor(Color::Blue());
    canvas->DrawPath(paint);
}

void UITestCanvas::UIKitCanvasTestDrawPath034()
{
    if (container_ == nullptr) {
        return;
    }

    CreateTitleLabel("绘制rect传入临界值 ");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    canvas->BeginPath();
    canvas->AddRect({INT16_MAX, INT16_MAX}, RECT_HEIGHT, RECT_WIDTH);
    canvas->DrawPath(paint);

    canvas->BeginPath();
    canvas->AddRect({RECT_X, RECT_Y}, INT16_MAX, INT16_MAX);
    paint.SetStrokeColor(Color::Red());
    canvas->DrawPath(paint);
}

void UITestCanvas::RM009LineCapDrawPath()
{
    if (container_ == nullptr) {
        return;
    }

    CreateTitleLabel("RM009LineCap_平直的边缘_正方形_圆形线帽");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetStrokeWidth(STROKE_WIDTH12);
    paint.SetStrokeColor(Color::Green());
#if GRAPHIC_ENABLE_LINECAP_FLAG
    paint.SetLineCap(LineCap::BUTT_CAP);
#endif
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y}); /* 竖线 */
    canvas->LineTo({LINE11_X, LINE11_Y});
    canvas->DrawPath(paint);
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE1_Y}); /* 横线 */
    canvas->LineTo({LINE17_X, LINE1_Y});
    canvas->DrawPath(paint);
    canvas->BeginPath();
    canvas->MoveTo({LINE26_Y, LINE25_Y}); /* 斜线 */
    canvas->LineTo({LINE18_X, LINE1_Y});
    canvas->DrawPath(paint);
    paint.SetStrokeColor(Color::Red());
#if GRAPHIC_ENABLE_LINECAP_FLAG
    paint.SetLineCap(LineCap::SQUARE_CAP);
#endif
    canvas->BeginPath();
    canvas->MoveTo({LINE1_X, LINE13_Y}); /* 竖线 */
    canvas->LineTo({LINE19_X, LINE11_Y});
    canvas->DrawPath(paint);
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE26_X}); /* 横线 */
    canvas->LineTo({LINE17_X, LINE2_Y});
    canvas->DrawPath(paint);
    canvas->BeginPath();
    canvas->MoveTo({LINE28_X, LINE25_Y}); /* 斜线 */
    canvas->LineTo({LINE21_X, LINE1_Y});
    canvas->DrawPath(paint);
    paint.SetStrokeColor(Color::Blue());
#if GRAPHIC_ENABLE_LINECAP_FLAG
    paint.SetLineCap(LineCap::ROUND_CAP);
#endif
    canvas->BeginPath();
    canvas->MoveTo({LINE22_X, LINE13_Y}); /* 竖线 */
    canvas->LineTo({LINE22_X, LINE11_Y});
    canvas->DrawPath(paint);
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE23_Y}); /* 横线 */
    canvas->LineTo({LINE17_X, LINE23_Y});
    canvas->DrawPath(paint);
    canvas->BeginPath();
    canvas->MoveTo({LINE18_X, LINE25_Y}); /* 斜线 */
    canvas->LineTo({LINE24_X, LINE1_Y});
    canvas->DrawPath(paint);
}
void UITestCanvas::RM009LineJoinDrawPath()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM009LineJoin_圆角_斜角_尖角_限制尖角长度");
    UICanvas* canvas = CreateCanvas();

    Paint paint;
    paint.SetStrokeWidth(STROKE_WIDTH10);
    paint.SetStrokeColor(Color::Green());
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    paint.SetLineJoin(LineJoin::ROUND_JOIN);
#endif
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE25_X, LINE25_X});
    canvas->LineTo({LINE22_X, LINE11_Y});
    canvas->DrawPath(paint);
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    paint.SetLineJoin(LineJoin::BEVEL_JOIN);
#endif
    canvas->BeginPath();
    canvas->MoveTo({LINE1_Y, LINE13_Y});
    canvas->LineTo({LINE2_X, LINE11_Y});
    canvas->LineTo({LINE26_X, LINE25_Y});
    canvas->LineTo({LINE27_X, LINE11_Y});
    canvas->DrawPath(paint);
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    paint.SetLineJoin(LineJoin::MITER_JOIN);
#endif
    canvas->BeginPath();
    canvas->MoveTo({LINE14_X, LINE13_Y});
    canvas->LineTo({LINE14_X, LINE11_Y});
    canvas->LineTo({LINE28_X, LINE25_Y});
    canvas->LineTo({LINE29_X, LINE11_Y});
    canvas->DrawPath(paint);
#if GRAPHIC_ENABLE_LINEJOIN_FLAG
    // 当斜接角度超过MiterLimit时应该转化为平角
    paint.SetLineJoin(LineJoin::MITER_JOIN);
    paint.SetMiterLimit(MITERLIMIT4);
#endif

    canvas->BeginPath();
    canvas->MoveTo({LINE21_X, LINE13_Y});
    canvas->LineTo({LINE21_X, LINE11_Y});
    canvas->LineTo({LINE30_X, LINE25_Y});
    canvas->LineTo({LINE31_X, LINE25_Y});
    canvas->DrawPath(paint);
}
void UITestCanvas::RM009LineDashDrawPath()
{
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM009LineDash_虚实线(10,5,5,2)_更改虚实线起点的偏移量(5)");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetStrokeWidth(DASH_LINE_WIDTH2);
    paint.SetStrokeColor(Color::Green());
    float ds2[] = {DASH10, DASH5, DASH5, DASH2};
    paint.SetLineDash(ds2, DASH_COUNT);
    canvas->BeginPath();
    canvas->MoveTo({MOVETO20, MOVETO20});
    canvas->LineTo({LINET0200, MOVETO20});
    canvas->LineTo({LINET0200, LINET080});
    canvas->LineTo({LINET020, LINET080});
    canvas->ClosePath();
    canvas->DrawPath(paint);

    paint.SetLineDashOffset(LINE_DASH_OFFSET5);
    canvas->BeginPath();
    canvas->MoveTo({MOVETO20, LINET0120});
    canvas->LineTo({LINET0200, LINET0120});
    canvas->LineTo({LINET0200, LINET0180});
    canvas->LineTo({LINET020, LINET0180});
    canvas->ClosePath();
    canvas->DrawPath(paint);
#endif
}

void UITestCanvas::RM009StrokeAndClearRectDrawPath()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM009_StrokeRect_ClearRect");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetStrokeWidth(STROKEWIDTH2);
    paint.SetStrokeColor(Color::Orange());
    canvas->StrokeRect({START1_X, START1_Y}, RECT40, RECT50, paint);

    paint.SetFillColor(Color::Red());
    canvas->BeginPath();
    canvas->MoveTo({MOVETO30, MOVETO60});
    canvas->LineTo({LINET0200, MOVETO60});
    canvas->LineTo({LINET0200, LINET0200});
    canvas->LineTo({MOVETO30, LINET0200});
    canvas->ClosePath();
    canvas->FillPath(paint);

    canvas->ClearRect({RECT50, RECT80}, RECT100, RECT70);
}

void UITestCanvas::RM008UIKitCanvasTest001()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM008_FillStyle_Solid_单色绘制多边形和填充多边形");
    UICanvas* canvas = CreateCanvas();
    Paint paint;

    paint.SetStyle(Paint::STROKE_FILL_STYLE);
    paint.SetStrokeWidth(STROKEWIDTH2);
    paint.SetStrokeColor(Color::Orange());
    paint.SetFillColor(Color::Blue());
    canvas->BeginPath();

    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE22_X, LINE11_Y});
    canvas->LineTo({LINE26_X, LINE13_Y});
    canvas->ArcTo({LINE26_X, LINE1_Y}, ARC_RADIUS, START1_ANGLE, END1_ANGLE);
    canvas->LineTo({LINE26_X, LINE26_Y});
    canvas->LineTo({LINE4_X, LINE2_Y});
    canvas->LineTo({LINE11_X, LINE27_Y});
    canvas->LineTo({LINE3_X, LINE11_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
    canvas->DrawPath(paint);
}

void UITestCanvas::RM008UIKitCanvasTest002()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM008_FillStyle_RM_013_Gradient_渐变填充多边形");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetStyle(Paint::GRADIENT);
    // 线性渐变
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    paint.createLinearGradient(LINEARGRADIENT50, LINEARGRADIENT50, LINEARGRADIENT150, LINEARGRADIENT150);
    paint.addColorStop(0, Color::Yellow());
    paint.addColorStop(COLOR_STOP3, Color::White());
    paint.addColorStop(COLOR_STOP6, Color::Green());
    paint.addColorStop(1, Color::Blue());
#endif
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE22_X, LINE11_Y});
    canvas->LineTo({LINE26_X, LINE13_Y});
    canvas->ArcTo({LINE26_X, LINE1_Y}, ARC_RADIUS, START1_ANGLE, END1_ANGLE);
    canvas->LineTo({LINE26_X, LINE26_Y});
    canvas->LineTo({LINE4_X, LINE2_Y});
    canvas->LineTo({LINE11_X, LINE27_Y});
    canvas->LineTo({LINE3_X, LINE11_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
    // 放射渐变
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    paint.createRadialGradient(RADIAL_GRADIENT300, RADIAL_GRADIENT140, RADIAL_GRADIENT5,
                               RADIAL_GRADIENT270, RADIAL_GRADIENT100, RADIAL_GRADIENT80);
#endif
    canvas->BeginPath();

    canvas->MoveTo({LINE9_X, LINE13_Y});
    canvas->LineTo({LINE32_X, LINE11_Y});
    canvas->LineTo({LINE31_X, LINE13_Y});
    canvas->ArcTo({LINE31_X, LINE1_Y}, ARC_RADIUS, START1_ANGLE, END1_ANGLE);
    canvas->LineTo({LINE31_X, LINE26_Y});
    canvas->LineTo({LINE21_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE27_Y});
    canvas->LineTo({LINE33_X, LINE11_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}

void UITestCanvas::RM008UIKitCanvasTest003()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM008_StrokeStyle_RM_013_Gradient_渐变绘制多边形路径");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetStyle(Paint::GRADIENT);
    paint.SetStrokeWidth(STROKE_WIDTH8);
    // 线性渐变
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    paint.createLinearGradient(LINEARGRADIENT50, LINEARGRADIENT50, LINEARGRADIENT150, LINEARGRADIENT150);
    paint.addColorStop(0, Color::Yellow());
    paint.addColorStop(COLOR_STOP3, Color::White());
    paint.addColorStop(COLOR_STOP6, Color::Green());
    paint.addColorStop(1, Color::Blue());
#endif
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE22_X, LINE11_Y});
    canvas->LineTo({LINE26_X, LINE13_Y});
    canvas->ArcTo({LINE26_X, LINE1_Y}, ARC_RADIUS, START1_ANGLE, END1_ANGLE);
    canvas->LineTo({LINE26_X, LINE26_Y});
    canvas->LineTo({LINE4_X, LINE2_Y});
    canvas->LineTo({LINE11_X, LINE27_Y});
    canvas->LineTo({LINE3_X, LINE11_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
    // 放射渐变
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    paint.createRadialGradient(LINEARGRADIENT300, LINEARGRADIENT140, LINEARGRADIENT5,
                               LINEARGRADIENT270, LINEARGRADIENT100, LINEARGRADIENT80);
#endif
    canvas->BeginPath();
    canvas->MoveTo({LINE9_X, LINE13_Y});
    canvas->LineTo({LINE32_X, LINE11_Y});
    canvas->LineTo({LINE31_X, LINE13_Y});
    canvas->ArcTo({LINE31_X, LINE1_Y}, ARC_RADIUS, START1_ANGLE, END1_ANGLE);
    canvas->LineTo({LINE31_X, LINE26_Y});
    canvas->LineTo({LINE21_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE27_Y});
    canvas->LineTo({LINE33_X, LINE11_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
}

void UITestCanvas::RM008UIKitCanvasTest004()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM008_Pattern_模式REPEAT(图像)绘制多边形路径和填充");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetStyle(Paint::PATTERN);
    paint.SetStrokeWidth(STROKEWIDTH8);
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    paint.CreatePattern(RED_IMAGE_PATH, REPEAT);
#endif
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE22_X, LINE11_Y});
    canvas->LineTo({LINE26_X, LINE13_Y});
    canvas->ArcTo({LINE26_X, LINE1_Y}, ARC_RADIUS, START1_ANGLE, END1_ANGLE);
    canvas->LineTo({LINE26_X, LINE26_Y});
    canvas->LineTo({LINE4_X, LINE2_Y});
    canvas->LineTo({LINE11_X, LINE27_Y});
    canvas->LineTo({LINE3_X, LINE11_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);

    canvas->BeginPath();
    canvas->MoveTo({LINE9_X, LINE13_Y});
    canvas->LineTo({LINE32_X, LINE11_Y});
    canvas->LineTo({LINE31_X, LINE13_Y});
    canvas->ArcTo({LINE31_X, LINE1_Y}, ARC_RADIUS, START1_ANGLE, END1_ANGLE);
    canvas->LineTo({LINE31_X, LINE26_Y});
    canvas->LineTo({LINE21_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE27_Y});
    canvas->LineTo({LINE33_X, LINE11_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}
void UITestCanvas::RM008UIKitCanvasTest005()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM008_Pattern_模式REPEAT_X(图像)绘制多边形路径和填充");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetStyle(Paint::PATTERN);
    paint.SetStrokeWidth(STROKEWIDTH8);
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    paint.CreatePattern(RED_IMAGE_PATH, REPEAT_X);
#endif
    canvas->BeginPath();

    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE22_X, LINE11_Y});
    canvas->LineTo({LINE26_X, LINE13_Y});
    canvas->ArcTo({LINE26_X, LINE1_Y}, ARC_RADIUS, START1_ANGLE, END1_ANGLE);
    canvas->LineTo({LINE26_X, LINE26_Y});
    canvas->LineTo({LINE4_X, LINE2_Y});
    canvas->LineTo({LINE11_X, LINE27_Y});
    canvas->LineTo({LINE3_X, LINE11_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);

    canvas->BeginPath();
    canvas->MoveTo({LINE9_X, LINE13_Y});
    canvas->LineTo({LINE32_X, LINE11_Y});
    canvas->LineTo({LINE31_X, LINE13_Y});
    canvas->ArcTo({LINE31_X, LINE1_Y}, ARC_RADIUS, START1_ANGLE, END1_ANGLE);
    canvas->LineTo({LINE31_X, LINE26_Y});
    canvas->LineTo({LINE21_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE27_Y});
    canvas->LineTo({LINE33_X, LINE11_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}
void UITestCanvas::RM008UIKitCanvasTest006()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM008_Pattern_模式REPEAT_Y(图像)绘制多边形路径和填充");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetStyle(Paint::PATTERN);
    paint.SetStrokeWidth(STROKEWIDTH8);
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    paint.CreatePattern(RED_IMAGE_PATH, REPEAT_Y);
#endif
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE10_Y});
    canvas->LineTo({LINE28_X, LINE10_Y});
    canvas->LineTo({LINE28_X, LINE13_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);

    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE11_X, LINE26_Y});
    canvas->LineTo({LINE28_X, LINE26_Y});
    canvas->LineTo({LINE28_X, LINE11_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}
void UITestCanvas::RM008UIKitCanvasTest007()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM008_Pattern_模式NO_REPEAT(图像)绘制多边形路径和填充");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetStyle(Paint::PATTERN);
    paint.SetStrokeWidth(STROKEWIDTH8);
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    paint.CreatePattern(RED_IMAGE_PATH, NO_REPEAT);
#endif
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE10_Y});
    canvas->LineTo({LINE28_X, LINE10_Y});
    canvas->LineTo({LINE28_X, LINE13_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);

    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE11_X, LINE26_Y});
    canvas->LineTo({LINE28_X, LINE26_Y});
    canvas->LineTo({LINE28_X, LINE11_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}

void UITestCanvas::RM008UIKitCanvasShadowTest008()
{
#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM008_阴影不偏移_偏移_改变模糊级别_改变模糊颜色");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetFillStyle(Color::Orange());
    paint.SetStrokeWidth(STROKEWIDTH8);
    paint.SetShadowBlur(LINEARGRADIENT5);
    paint.SetShadowColor(Color::Black());
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE10_Y});
    canvas->LineTo({LINE2_X, LINE10_Y});
    canvas->LineTo({LINE2_X, LINE13_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
    paint.SetShadowOffsetX(OFFSETX20);
    paint.SetShadowOffsetY(OFFSETX10);
    canvas->BeginPath();
    canvas->MoveTo({LINE26_X, LINE13_Y});
    canvas->LineTo({LINE26_X, LINE10_Y});
    canvas->LineTo({LINE28_X, LINE10_Y});
    canvas->LineTo({LINE28_X, LINE13_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
    paint.SetStrokeStyle(Color::Orange());
    paint.SetShadowOffsetX(0);
    paint.SetShadowOffsetY(0);
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE11_X, LINE2_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->LineTo({LINE2_X, LINE11_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
    paint.SetShadowOffsetX(OFFSETX20);
    paint.SetShadowOffsetY(OFFSETX10);
    canvas->BeginPath();
    canvas->MoveTo({LINE26_X, LINE11_Y});
    canvas->LineTo({LINE26_X, LINE2_Y});
    canvas->LineTo({LINE28_X, LINE2_Y});
    canvas->LineTo({LINE28_X, LINE11_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
    paint.SetFillStyle(Color::Orange());
    paint.SetShadowBlur(SHADOWBLUR15);
    paint.SetShadowOffsetX(OFFSETX20);
    paint.SetShadowOffsetY(OFFSETX10);
    canvas->BeginPath();
    canvas->MoveTo({LINE34_X, LINE13_Y});
    canvas->LineTo({LINE34_X, LINE10_Y});
    canvas->LineTo({LINE31_X, LINE10_Y});
    canvas->LineTo({LINE31_X, LINE13_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
    paint.SetStrokeStyle(Color::Orange());
    paint.SetShadowBlur(SHADOWBLUR5);
    paint.SetShadowColor(Color::Blue());
    paint.SetShadowOffsetX(OFFSETX20);
    paint.SetShadowOffsetY(OFFSETX10);
    canvas->BeginPath();
    canvas->MoveTo({LINE34_X, LINE11_Y});
    canvas->LineTo({LINE34_X, LINE26_X});
    canvas->LineTo({LINE31_X, LINE26_X});
    canvas->LineTo({LINE31_X, LINE11_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
#endif
}

void UITestCanvas::RM011StrokeText001()
{
    CreateTitleLabel("RM011_StrokeText_多国文字加旋转放大");

#if GRAPHIC_ENABLE_DRAW_TEXT_FLAG
    UICanvas* canvas = CreateCanvas();
    UICanvas::FontStyle fontStyle;
    fontStyle.align = TEXT_ALIGNMENT_CENTER;
    fontStyle.direct = TEXT_DIRECT_LTR;
    fontStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
    fontStyle.fontSize = FONT_SIZE15;
    fontStyle.letterSpace = FONT_LETTERSPACE2;
    Paint paint;
    paint.SetFillColor(Color::Blue());
    canvas->StrokeText("葡萄牙语：Hongmeng, teste", {0, HEIGHT_Y20}, fontStyle, paint);
    canvas->StrokeText("西班牙语：Hong Meng, test", {0, HEIGHT_Y40}, fontStyle, paint);
    canvas->StrokeText("法语：HongMeng, test", {0, HEIGHT_Y60}, fontStyle, paint);
    canvas->StrokeText("瓜拉尼语：mitapoañda, pens", {0, HEIGHT_Y80}, fontStyle, paint);
    canvas->StrokeText("荷兰语：Hongmeng, kom op.", {0, HEIGHT_Y100}, fontStyle, paint);
    canvas->StrokeText("樊瓦什语：Кайалла", {0, HEIGHT_Y120}, fontStyle, paint);
    canvas->StrokeText("白俄罗斯语：Прывітанне", {0, HEIGHT_Y140}, fontStyle, paint);
    canvas->StrokeText("希腊语：Γεια.", {0, HEIGHT_Y160}, fontStyle, paint);
    canvas->StrokeText("瑞典语:Hej, Hongmeng.", {0, HEIGHT_Y180}, fontStyle, paint);
    canvas->StrokeText("俄语: Привет, hongmon ", {0, HEIGHT_Y200}, fontStyle, paint);
    paint.Scale(SCALE1_X, SCALE1_Y);
    paint.Rotate(ROTATE45);
    canvas->StrokeText("中国 你好，鸿蒙。。", {HEIGHT_Y20, 0}, fontStyle, paint);
#endif
}

void UITestCanvas::RM011CanvasScale001()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM011放大和缩小Scale");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetStrokeStyle(Color::Red());
    paint.SetStrokeWidth(STROKEWIDTH3);
    canvas->BeginPath();
    canvas->MoveTo({MOVETO5, MOVETO5});
    canvas->LineTo({LINET050, MOVETO5});
    canvas->LineTo({LINET050, LINET035});
    canvas->LineTo({MOVETO5, LINET035});
    canvas->ClosePath();
    canvas->DrawPath(paint);
    paint.SetStrokeStyle(Color::Green());
    paint.SetGlobalAlpha(GLOBALALPHA5);
    paint.Scale(SCALE2, SCALE4);
    canvas->BeginPath();
    canvas->MoveTo({MOVETO5, MOVETO5});
    canvas->LineTo({LINET050, MOVETO5});
    canvas->LineTo({LINET050, LINET035});
    canvas->LineTo({LINE3_X, LINE3_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
    paint.SetStrokeStyle(Color::Blue());
    paint.SetGlobalAlpha(GLOBALALPHA5);
    paint.Scale(SCALE_X, SCALE_Y);
    canvas->BeginPath();
    canvas->MoveTo({LINE3_X, LINE4_Y});
    canvas->LineTo({LINE4_X, LINE4_Y});
    canvas->LineTo({LINE4_X, LINE3_Y});
    canvas->LineTo({LINE3_X, LINE3_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
}

void UITestCanvas::RM011CanvasRotate001()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM011原(红)_旋转(绿)_平移(蓝)_单位矩阵(黄)_重置矩阵后再设置矩阵（橘色）");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetStrokeStyle(Color::Red());
    paint.SetStrokeWidth(STROKE3_WIDTH);
    canvas->BeginPath();
    canvas->MoveTo({LINE8_X, LINE8_X});
    canvas->LineTo({LINE6_X, LINE6_Y});
    canvas->LineTo({LINE6_X, LINE7_Y});
    canvas->LineTo({LINE8_X, LINE8_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
    paint.SetStrokeStyle(Color::Green());
    paint.Rotate(ROTATE_ANGLE);
    canvas->BeginPath();
    canvas->MoveTo({LINE8_X, LINE6_Y});
    canvas->LineTo({LINE6_X, LINE6_Y});
    canvas->LineTo({LINE6_X, LINE7_Y});
    canvas->LineTo({LINE8_X, LINE7_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
    paint.Translate(TRANSLATE_X, TRANSLATE_Y);
    paint.SetStrokeStyle(Color::Blue());
    canvas->BeginPath();
    canvas->MoveTo({LINE8_X, LINE6_Y});
    canvas->LineTo({LINE6_X, LINE6_Y});
    canvas->LineTo({LINE6_X, LINE7_Y});
    canvas->LineTo({LINE8_X, LINE8_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
    paint.Transform(SCALE1_X, SHEAR_X, SHEAR_Y, SCALE2_Y, TRANSLATE1_X, TRANSLATE1_Y);
    paint.SetStrokeStyle(Color::Yellow());
    canvas->BeginPath();
    canvas->MoveTo({LINE8_X, LINE6_Y});
    canvas->LineTo({LINE6_X, LINE6_Y});
    canvas->LineTo({LINE6_X, LINE7_Y});
    canvas->LineTo({LINE8_X, LINE8_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
    paint.SetTransform(SCALE1_X, SHEAR1_X, SHEAR1_Y, SCALE2_Y, TRANSLATE1_X, TRANSLATE1_Y);
    paint.SetStrokeStyle(Color::Orange());
    canvas->BeginPath();
    canvas->MoveTo({LINE8_X, LINE6_Y});
    canvas->LineTo({LINE6_X, LINE6_Y});
    canvas->LineTo({LINE6_X, LINE7_Y});
    canvas->LineTo({LINE8_X, LINE8_Y});
    canvas->ClosePath();
    canvas->DrawPath(paint);
}

void UITestCanvas::RM012globalAlpha001()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM012_设置图像透明度_红不透明_绿蓝_一半透明度");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetFillColor(Color::Red());
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE13_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);

    paint.SetGlobalAlpha(GLOBALALPHA2);
    paint.SetFillColor(Color::Green());
    canvas->BeginPath();
    canvas->MoveTo({LINE1_Y, LINE10_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE10_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);

    paint.SetFillColor(Color::Blue());
    canvas->BeginPath();
    canvas->MoveTo({LINE14_X, LINE1_Y});
    canvas->LineTo({LINE14_X, LINE14_Y});
    canvas->LineTo({LINE15_X, LINE14_Y});
    canvas->LineTo({LINE15_X, LINE1_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}

void UITestCanvas::RM012GlobalCompositeOperationSourceOver()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM012_设置混合_SOURCE_OVER_第二个图源在第一个图源之上");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetFillColor(Color::Red());
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE13_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
    paint.SetGlobalCompositeOperation(SOURCE_OVER);
    paint.SetFillColor(Color::Green());
    canvas->BeginPath();
    canvas->MoveTo({LINE1_Y, LINE10_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE10_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}

void UITestCanvas::RM012GlobalCompositeOperationSourceAtop()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM012_设置混合_SOURCE_ATOP_第一个图源和与第二个图源相交部分");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetFillColor(Color::Red());
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE13_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
    paint.SetGlobalCompositeOperation(SOURCE_ATOP);
    paint.SetFillColor(Color::Green());
    canvas->BeginPath();
    canvas->MoveTo({LINE1_Y, LINE10_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE10_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}
void UITestCanvas::RM012GlobalCompositeOperationSourceIn()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM012_设置混合_SOURCE_IN_只显示两个相交部分显示第二个图源颜色");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetFillColor(Color::Red());
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE13_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
    paint.SetGlobalCompositeOperation(SOURCE_IN);
    paint.SetFillColor(Color::Green());
    canvas->BeginPath();
    canvas->MoveTo({LINE1_Y, LINE10_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE10_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}
void UITestCanvas::RM012GlobalCompositeOperationSourceOut()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM012_设置混合_SOURCE_OUT_只显示第二个图源的第一个不相交部分");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetFillColor(Color::Red());
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE13_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
    paint.SetGlobalCompositeOperation(SOURCE_OUT);
    paint.SetFillColor(Color::Green());
    canvas->BeginPath();
    canvas->MoveTo({LINE1_Y, LINE10_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE10_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}
void UITestCanvas::RM012GlobalCompositeOperationDestinationOver()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM012_设置混合_DESTINATION_OVER_源图像在目标图像之上");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetFillColor(Color::Red());
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE13_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
    paint.SetGlobalCompositeOperation(DESTINATION_OVER);
    paint.SetFillColor(Color::Green());
    canvas->BeginPath();
    canvas->MoveTo({LINE1_Y, LINE10_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE10_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}

void UITestCanvas::RM012GlobalCompositeOperationDestinationAtop()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM012_设置混合_DESTINATION_ATOP_在源图像上显示目标图像");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetFillColor(Color::Red());
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE13_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
    paint.SetGlobalCompositeOperation(DESTINATION_ATOP);
    paint.SetFillColor(Color::Green());
    canvas->BeginPath();
    canvas->MoveTo({LINE1_Y, LINE10_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE10_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}

void UITestCanvas::RM012GlobalCompositeOperationDestinationIn()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM012_设置混合_DESTINATION_IN_在源图像上显示目标图像");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetFillColor(Color::Red());
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE13_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
    paint.SetGlobalCompositeOperation(DESTINATION_IN);
    paint.SetFillColor(Color::Green());
    canvas->BeginPath();
    canvas->MoveTo({LINE1_Y, LINE10_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE10_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}

void UITestCanvas::RM012GlobalCompositeOperationDestinationOut()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM012_设置混合_DESTINATION_OUT_在源图像上显示目标图像");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetFillColor(Color::Red());
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE13_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
    paint.SetGlobalCompositeOperation(DESTINATION_OUT);
    paint.SetFillColor(Color::Green());
    canvas->BeginPath();
    canvas->MoveTo({LINE1_Y, LINE10_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE10_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}

void UITestCanvas::RM012GlobalCompositeOperationLIGHTER()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM012_设置混合_LIGHTER_显示源图像 + 目标图像相交部分混合");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetFillColor(Color::Red());
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE13_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
    paint.SetGlobalCompositeOperation(LIGHTER);
    paint.SetFillColor(Color::Blue());
    canvas->BeginPath();
    canvas->MoveTo({LINE1_Y, LINE10_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE10_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}

void UITestCanvas::RM012GlobalCompositeOperationCopy()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM012_设置混合_COPY_只显示第二个图源绿色");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetFillColor(Color::Red());
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE13_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
    paint.SetGlobalCompositeOperation(COPY);
    paint.SetFillColor(Color::Green());
    canvas->BeginPath();
    canvas->MoveTo({LINE1_Y, LINE10_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE10_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}

void UITestCanvas::RM012GlobalCompositeOperationXOR()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM012_设置混合_XOR_相交部分不显示");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetFillColor(Color::Red());
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE13_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
    paint.SetGlobalCompositeOperation(XOR);
    paint.SetFillColor(Color::Green());
    canvas->BeginPath();
    canvas->MoveTo({LINE1_Y, LINE10_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE10_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);
}

void UITestCanvas::RM012SaveOrRestore002()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("RM012_设置图像透明度_红不透明_绿蓝_一半透明度Save_Restore");
    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetFillColor(Color::Red());
    canvas->BeginPath();
    canvas->MoveTo({LINE11_X, LINE13_Y});
    canvas->LineTo({LINE11_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE11_Y});
    canvas->LineTo({LINE12_X, LINE13_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);

    paint.SetGlobalAlpha(GLOBALALPHA2);
    paint.SetFillColor(Color::Green());
    canvas->BeginPath();
    canvas->MoveTo({LINE1_Y, LINE10_Y});
    canvas->LineTo({LINE2_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE2_Y});
    canvas->LineTo({LINE9_X, LINE10_Y});
    canvas->ClosePath();
    canvas->FillPath(paint);

    canvas->Save(paint);
    paint.SetFillColor(Color::Blue());
    canvas->BeginPath();
    canvas->MoveTo({LINE14_X, LINE1_Y});
    canvas->LineTo({LINE14_X, LINE14_Y});
    canvas->LineTo({LINE15_X, LINE14_Y});
    canvas->LineTo({LINE15_X, LINE1_Y});
    canvas->ClosePath();
    paint = canvas->Restore();
    canvas->FillPath(paint);
}
} // namespace OHOS
