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

#ifndef UI_TEST_CANVAS_H
#define UI_TEST_CANVAS_H

#include "components/ui_canvas.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "layout/grid_layout.h"
#include "ui_test.h"
namespace OHOS {
class UITestCanvas : public UITest {
public:
    UITestCanvas()
    {}
    ~UITestCanvas() override
    {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void UIKitCanvasTestDrawLine001();
    void UIKitCanvasTestDrawLine002();
    void UIKitCanvasTestDrawCurve001();
    void UIKitCanvasTestDrawCurve002();
    void UIKitCanvasTestDrawRect001();
    void UIKitCanvasTestDrawRect002();
    void UIKitCanvasTestDrawRect003();
    void UIKitCanvasTestDrawCircle001();
    void UIKitCanvasTestDrawCircle002();
    void UIKitCanvasTestDrawCircle003();
    void UIKitCanvasTestDrawArc001();
    void UIKitCanvasTestDrawImage001();
    void UIKitCanvasTestDrawImage002();
    void UIKitCanvasTestDrawLabel001();
    void UIKitCanvasTestDrawSector001();
    void UIKitCanvasTestClear001();
    void UIKitCanvasTestDrawPath001();
    void UIKitCanvasTestDrawPath002();
    void UIKitCanvasTestDrawPath003();
    void UIKitCanvasTestDrawPath004();
    void UIKitCanvasTestDrawPath005();
    void UIKitCanvasTestDrawPath006();
    void UIKitCanvasTestDrawPath007();
    void UIKitCanvasTestDrawPath008();
    void UIKitCanvasTestDrawPath009();
    void UIKitCanvasTestDrawPath010();
    void UIKitCanvasTestDrawPath011();
    void UIKitCanvasTestDrawPath012();
    void UIKitCanvasTestDrawPath013();
    void UIKitCanvasTestDrawPath014();
    void UIKitCanvasTestDrawPath015();
    void UIKitCanvasTestDrawPath016();
    void UIKitCanvasTestDrawPath017();
    void UIKitCanvasTestDrawPath018();
    void UIKitCanvasTestDrawPath019();
    void UIKitCanvasTestDrawPath020();
    void UIKitCanvasTestDrawPath021();
    void UIKitCanvasTestDrawPath022();
    void UIKitCanvasTestDrawPath023();
    void UIKitCanvasTestDrawPath024();
    void UIKitCanvasTestDrawPath025();
    void UIKitCanvasTestDrawPath026();
    void UIKitCanvasTestDrawPath027();
    void UIKitCanvasTestDrawPath028();
    void UIKitCanvasTestDrawPath029();
    void UIKitCanvasTestDrawPath030();
    void UIKitCanvasTestDrawPath031();
    void UIKitCanvasTestDrawPath032();
    void UIKitCanvasTestDrawPath033();
    void UIKitCanvasTestDrawPath034();
    void RM008UIKitCanvasTest001();
    void RM008UIKitCanvasTest002();
    void RM008UIKitCanvasTest003();
    void RM008UIKitCanvasTest004();
    void RM008UIKitCanvasTest005();
    void RM008UIKitCanvasTest006();
    void RM008UIKitCanvasTest007();
    void RM008UIKitCanvasShadowTest008();
    void RM009LineCapDrawPath();
    void RM009LineJoinDrawPath();
    void RM009LineDashDrawPath();
    void RM009StrokeAndClearRectDrawPath();
    void RM011StrokeText001();
    void RM011CanvasScale001();
    void RM011CanvasRotate001();
    void RM012globalAlpha001();
    void RM012GlobalCompositeOperationSourceOver();
    void RM012GlobalCompositeOperationSourceAtop();
    void RM012GlobalCompositeOperationSourceIn();
    void RM012GlobalCompositeOperationSourceOut();
    void RM012GlobalCompositeOperationDestinationOver();
    void RM012GlobalCompositeOperationDestinationAtop();
    void RM012GlobalCompositeOperationDestinationIn();
    void RM012GlobalCompositeOperationDestinationOut();
    void RM012GlobalCompositeOperationLIGHTER();
    void RM012GlobalCompositeOperationCopy();
    void RM012GlobalCompositeOperationXOR();
    void RM012SaveOrRestore002();
private:
    void CreateTitleLabel(const char* title);
    UICanvas* CreateCanvas();

    UIScrollView* container_ = nullptr;

    const int16_t GAP = 10;
    const int16_t TITLE_HEIGHT = 29;
    const uint16_t CANVAS_WIDTH = 454;
    const uint16_t CANVAS_HEIGHT = 200;
    const int16_t START1_X = 10;
    const int16_t START1_Y = 10;
    const int16_t START2_X = 80;
    const int16_t START2_Y = 10;
    const int16_t LINE1_X = 40;
    const int16_t LINE1_Y = 100;
    const int16_t LINE2_X = 100;
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
    const int16_t LINE3_X = 5;
    const int16_t LINE3_Y = 35;
    const int16_t LINE4_X = 50;
    const int16_t LINE4_Y = 5;
    const int16_t LINE6_X = 60;
    const int16_t LINE6_Y = 15;
    const int16_t LINE7_Y = 45;
    const int16_t LINE8_X = 15;
    const int16_t LINE8_Y = 45;
    const int16_t LINE9_X = 230;
    const int16_t LINE10_Y = 60;
    const int16_t LINE11_X = 20;
    const int16_t LINE11_Y = 80;
    const int16_t LINE12_X = 150;
    const int16_t LINE13_Y = 20;
    const int16_t LINE14_X = 180;
    const int16_t LINE14_Y = 160;
    const int16_t LINE15_X = 310;
    const int16_t LINE17_X = 80;
    const int16_t LINE18_X = 240;
    const int16_t LINE19_X = 40;
    const int16_t LINE21_X = 260;
    const int16_t LINE22_X = 60;
    const int16_t LINE23_Y = 140;
    const int16_t LINE24_X = 300;
    const int16_t LINE25_X = 40;
    const int16_t LINE25_Y = 40;
    const int16_t LINE26_X = 120;
    const int16_t LINE26_Y = 180;
    const int16_t LINE27_X = 140;
    const int16_t LINE27_Y = 130;
    const int16_t LINE28_X = 200;
    const int16_t LINE29_X = 220;
    const int16_t LINE30_X = 280;
    const int16_t LINE31_X = 330;
    const int16_t LINE32_X = 270;
    const int16_t LINE33_X = 215;
    const int16_t LINE34_X = 250;
    const int16_t TRANSLATE_X = 50;
    const int16_t TRANSLATE_Y = 50;
    const uint16_t STROKE3_WIDTH = 3;
    const uint16_t STROKE8_WIDTH = 8;
    const uint16_t ARC_RADIUS = 80;
    const int16_t START1_ANGLE = 0;
    const int16_t END1_ANGLE = 180;
    const int16_t HEIGHT_GAP = 8;
    const int16_t LINE_WIDTH5 = 5;
    const int16_t LINE_WIDTH10 = 10;
    const int16_t LINE_WIDTH30 = 30;
    const int16_t START_X = 50;
    const int16_t START_Y = 10;
    const int16_t LINE_CURVE_Y = 50;
    const int16_t LINE_RECT_Y50 = 50;
    const int16_t LINE_RECT_X300 = 300;
    const int16_t LINE_RECT_Y10 = 10;
    const int16_t LINE_RECT_WIDTH50 = 50;
    const int16_t LINE_RECT_HEIGHT50 = 50;
    const int16_t OPARCITY_127 = 127;
    const int16_t LINE_CURVE_X100 = 100;
    const int16_t LINE_CURVE_X200 = 200;
    const int16_t LINE_CURVE_X300 = 300;
    const int16_t LINE_CURVE_Y100 = 100;
    const int16_t LINE_CURVE_R30 = 30;
    const int16_t ARC_X100 = 100;
    const int16_t ARC_Y150 = 1500;
    const int16_t ARC_R50 = 50;
    const int16_t ARC_R100 = 100;
    const int16_t ARC_ANGLE135 = 135;
    const int16_t ARC_ANGLE270 = 270;
    const int16_t ROTATE20 = 20;
    const int16_t ROTATE_20 = -20;
    const int16_t ROTATE_45 = -45;
    const int16_t ROTATE45 = 45;
    const int16_t IMAGE_START60 = 60;
    const int16_t IMAGE_START100 = 100;
    const int16_t IMAGE_START20 = 20;
    const int16_t IMAGE_WIDTH100 = 100;
    const int16_t IMAGE_HEIGHT100 = 100;
    const int16_t FONT_SIZE30 = 30;
    const int16_t FONT_SIZE15 = 15;
    const int16_t FONT_LETTERSPACE = 10;
    const int16_t FONT_LETTERSPACE2 = 2;
    const int16_t LABEL_START50 = 50;
    const int16_t LABEL_WIDTH100 = 100;
    const int16_t END_ANGLE30 = 30;
    const int16_t START_X350 = 350;
    const int16_t START_Y150 = 150;
    const float DASH10 = 10.0f;
    const int16_t DASH_COUNT = 4;
    const int16_t DASH_LINE_WIDTH2 = 2;
    const int16_t MOVETO5 = 5;
    const int16_t MOVETO20 = 20;
    const int16_t MOVETO30 = 30;
    const int16_t MOVETO60 = 60;
    const int16_t LINET020 = 20;
    const int16_t LINET050 = 50;
    const int16_t LINET035 = 35;
    const int16_t LINET080 = 80;
    const int16_t LINET0100 = 100;
    const int16_t LINET0200 = 200;
    const int16_t LINET0120 = 120;
    const int16_t LINET0180 = 180;
    const int16_t LINE_DASH_OFFSET5 = 5;
    const int16_t STROKEWIDTH2 = 2;
    const int16_t STROKEWIDTH3 = 3;
    const int16_t STROKEWIDTH8 = 8;
    const int16_t RECT40 = 40;
    const int16_t RECT50 = 50;
    const int16_t RECT70 = 70;
    const int16_t RECT80 = 80;
    const int16_t RECT100 = 100;
    const int16_t LINEARGRADIENT50 = 50;
    const int16_t LINEARGRADIENT150 = 150;
    const int16_t LINEARGRADIENT140 = 140;
    const int16_t LINEARGRADIENT5 = 5;
    const int16_t LINEARGRADIENT300 = 300;
    const int16_t LINEARGRADIENT100 = 100;
    const int16_t LINEARGRADIENT80 = 80;
    const int16_t LINEARGRADIENT270 = 270;
    const int16_t OFFSETX10 = 10;
    const int16_t OFFSETX20 = 20;
    const int16_t SHADOWBLUR15 = 15;
    const int16_t SHADOWBLUR5 = 5;
    const int16_t HEIGHT_Y20 = 20;
    const int16_t HEIGHT_Y40 = 40;
    const int16_t HEIGHT_Y60 = 60;
    const int16_t HEIGHT_Y80 = 80;
    const int16_t HEIGHT_Y100 = 100;
    const int16_t HEIGHT_Y120 = 120;
    const int16_t HEIGHT_Y140 = 140;
    const int16_t HEIGHT_Y160 = 160;
    const int16_t HEIGHT_Y180 = 180;
    const int16_t HEIGHT_Y200 = 200;
    const int16_t SCALE2 = 2;
    const int16_t SCALE4 = 4;
    const int16_t STROKE_WIDTH12 = 12;
    const int16_t STROKE_WIDTH10 = 10;
    const int16_t STROKE_WIDTH8 = 8;
    const int16_t MITERLIMIT4 = 4;
    const int16_t RADIAL_GRADIENT300 = 300;
    const int16_t RADIAL_GRADIENT140 = 140;
    const int16_t RADIAL_GRADIENT5 = 5;
    const int16_t RADIAL_GRADIENT270 = 270;
    const int16_t RADIAL_GRADIENT100 = 100;
    const int16_t RADIAL_GRADIENT80 = 80;
    const int16_t DRAWIMAGE_WIDTH = 50;
    const int16_t DRAWIMAGE_HEIGHT = 50;
    const float SCALE_X = 0.5f;
    const float SCALE_Y = 0.5f;
    const float ROTATE_ANGLE = 20;
    const float ROTATE45_ANGLE = 45;
    const float SCALE1_X = 1;
    const float SCALE2_X = 2;
    const float SCALE1_Y = 1;
    const float SCALE2_Y = 2;
    const float SCALE4_Y = 4;
    const float SHEAR_X = 0.5f;
    const float SHEAR_Y = 0.5f;
    const float TRANSLATE1_X = 0;
    const float TRANSLATE1_Y = 0;
    const float SHEAR1_X = 0;
    const float SHEAR1_Y = 0;
    const float DASH5 = 5.0f;
    const float DASH2 = 2.0f;
    const float GLOBALALPHA5 = 0.5f;
    const float GLOBALALPHA2 = 0.2f;
    const float COLOR_STOP3 = 0.3f;
    const float COLOR_STOP6 = 0.6f;
};
} // namespace OHOS
#endif // UI_TEST_CANVAS_H
