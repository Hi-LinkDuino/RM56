/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "ui_test_chart_polyline.h"

#include "common/screen.h"

namespace OHOS {
namespace {
static int16_t g_blank = 20;
static int32_t g_curSerialCount = 0;
static int32_t g_curArrayCount = 0;
static bool g_secondScreenFlag = false;
static bool g_addPointsFlag = false;
static bool g_hidePointsFlag = false;
} // namespace

void UITestChartPolyline::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetHorizontalScrollState(false);
        container_->SetThrowDrag(true);
    }

    dataSerial_[0] = new UIChartDataSerial();
    dataSerial_[0]->SetMaxDataCount(5); // 5: number of data points
    Point pointArray[5] = {{0, 2478}, {1, 2600}, {2, 3000}, {3, 3200}, {4, 3500}};
    dataSerial_[0]->AddPoints(pointArray, 5); // 5: number of data points
    dataSerial_[0]->SetLineColor(Color::Red());
    dataSerial_[0]->SetFillColor(Color::Red());
    dataSerial_[0]->EnableGradient(true);

    dataSerial_[1] = new UIChartDataSerial();
    dataSerial_[1]->SetMaxDataCount(5); // 5: number of data points
    Point pointArray1[5] = {{0, 2000}, {1, 0}, {2, 800}, {3, 700}, {4, 433}};
    dataSerial_[1]->AddPoints(pointArray1, 5); // 5: number of data points
    dataSerial_[1]->SetLineColor(Color::Green());
    dataSerial_[1]->SetFillColor(Color::Green());
    dataSerial_[1]->EnableGradient(true);

    dataSerial_[2] = new UIChartDataSerial(); // 2: array index
    dataSerial_[2]->SetMaxDataCount(5);       // 2: array index, 5: number of data points
    Point pointArray2[5] = {{0, 100}, {1, 200}, {2, 300}, {3, 400}, {4, 500}};
    dataSerial_[2]->AddPoints(pointArray2, 5);   // 2: array index, 5: number of data points
    dataSerial_[2]->SetLineColor(Color::Blue()); // 2: array index
    curDataIndex_ = 0;
}

void UITestChartPolyline::InnerDeleteChildren(UIView* view) const
{
    if (view == nullptr) {
        return;
    }
    while (view != nullptr) {
        UIView* tempView = view;
        view = view->GetNextSibling();
        if (tempView->IsViewGroup()) {
            InnerDeleteChildren(static_cast<UIViewGroup*>(tempView)->GetChildrenHead());
        }
        if (tempView->GetViewType() == UI_AXIS) {
            return;
        }
        if (tempView->GetParent()) {
            static_cast<UIViewGroup*>(tempView->GetParent())->Remove(tempView);
        }
        delete tempView;
    }
}

void UITestChartPolyline::TearDown()
{
    ECGAnimator_->Stop();
    delete ECGAnimator_;
    ECGAnimator_ = nullptr;
    chart_->ClearDataSerial();
    for (uint8_t i = 0; i < DATA_NUM; i++) {
        delete dataSerial_[i];
        dataSerial_[i] = nullptr;
    }
    InnerDeleteChildren(container_);
    container_ = nullptr;
    lastX_ = 0;
    lastY_ = 0;
    positionX_ = 0;
    positionY_ = 0;
    g_curSerialCount = 0;
    g_curArrayCount = 0;
    g_secondScreenFlag = false;
    g_addPointsFlag = false;
    g_hidePointsFlag = false;
}

const UIView* UITestChartPolyline::GetTestView()
{
    UIKit_ChartPolyline_Test_AddDataSerial_001();
    UIKit_ChartPolyline_Test_EnableReverse_002();
    UIKit_ChartPolyline_Test_SetGradientBottom_003();
    UIKit_ChartPolyline_Test_AddPoints_004();

    return container_;
}

void UITestChartPolyline::UIKit_ChartPolyline_Test_AddDataSerial_001()
{
    UILabel* label = new UILabel();
    container_->Add(label);
    lastY_ = TEXT_DISTANCE_TO_TOP_SIDE;
    // 29: label height
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, lastY_, Screen::GetInstance().GetWidth(), 29);
    label->SetText("chart添加、删除数据串 ");
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

    chart_ = new UIChartPolyline();
    chart_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    chart_->SetWidth(454);  // 454: width
    chart_->SetHeight(250); // 250: height

    UIXAxis& xAxis = chart_->GetXAxis();
    UIYAxis& yAxis = chart_->GetYAxis();
    xAxis.SetMarkNum(5);         // 5: number of scales
    xAxis.SetDataRange(0, 5);    // 0: minimum value, 5: maximum value
    yAxis.SetDataRange(0, 5000); // 0: minimum value, 5000: maximum value

    chart_->SetGradientOpacity(25, 127); // 25: min opacity, 127: max opacity
    chart_->AddDataSerial(dataSerial_[0]);
    curDataIndex_++;
    container_->Add(chart_);
    SetLastPos(chart_);

    addDataSerialBtn_ = new UILabelButton();
    deleteDataSerialBtn_ = new UILabelButton();
    clearDataSerialBtn_ = new UILabelButton();
    topPointBtn_ = new UILabelButton();
    bottomPointBtn_ = new UILabelButton();
    headPointBtn_ = new UILabelButton();

    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE;
    positionY_ = lastY_ + 10; // 10: increase y-coordinate
    SetUpButton(addDataSerialBtn_, "添加数据 ");
    positionX_ = addDataSerialBtn_->GetX() + addDataSerialBtn_->GetWidth() + g_blank;
    positionY_ = addDataSerialBtn_->GetY();
    SetUpButton(deleteDataSerialBtn_, "删除数据 ");
    positionX_ = deleteDataSerialBtn_->GetX() + deleteDataSerialBtn_->GetWidth() + g_blank;
    positionY_ = deleteDataSerialBtn_->GetY();
    SetUpButton(clearDataSerialBtn_, "清空数据 ");

    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE;
    SetUpButton(topPointBtn_, "最高点 ");
    positionX_ = topPointBtn_->GetX() + topPointBtn_->GetWidth() + g_blank;
    positionY_ = topPointBtn_->GetY();
    SetUpButton(bottomPointBtn_, "最低点 ");
    positionX_ = bottomPointBtn_->GetX() + bottomPointBtn_->GetWidth() + g_blank;
    positionY_ = bottomPointBtn_->GetY();
    SetUpButton(headPointBtn_, "最新点 ");
}

void UITestChartPolyline::UIKit_ChartPolyline_Test_EnableReverse_002()
{
    reverseBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE;
    SetUpButton(reverseBtn_, "翻转 ");
    SetLastPos(reverseBtn_);
}

void UITestChartPolyline::UIKit_ChartPolyline_Test_SetGradientBottom_003()
{
    gradientBottomBtn_ = new UILabelButton();
    positionX_ = reverseBtn_->GetX() + reverseBtn_->GetWidth() + g_blank;
    positionY_ = reverseBtn_->GetY();
    SetUpButton(gradientBottomBtn_, "填充底部位置 ");
    SetLastPos(gradientBottomBtn_);
}
namespace {
const int16_t DATA_COUNT = 480;
}
/* ECG test data */
static int16_t g_ECGData[DATA_COUNT] = {
    68, 70, 73, 83, 95,  107, 118, 127, 118, 103, 90, 77, 66, 61, 57,  58,  60,  61,  62,  62, 63, 64, 64, 65, 67, 69,
    70, 71, 73, 75, 76,  78,  78,  79,  80,  80,  80, 80, 80, 80, 79,  78,  77,  76,  75,  73, 72, 71, 70, 70, 70, 70,
    70, 70, 70, 70, 71,  71,  71,  71,  71,  71,  72, 72, 72, 72, 73,  73,  73,  73,  74,  74, 74, 74, 75, 75, 75, 75,
    75, 75, 75, 75, 74,  74,  74,  74,  74,  74,  74, 73, 73, 73, 73,  73,  73,  73,  73,  73, 73, 73, 73, 73, 72, 71,
    71, 71, 70, 70, 70,  70,  71,  73,  75,  78,  80, 81, 82, 82, 82,  80,  78,  76,  73,  71, 69, 69, 68, 68, 68, 68,
    68, 70, 76, 88, 100, 111, 122, 126, 112, 98,  85, 73, 61, 58, 57,  59,  60,  61,  62,  62, 63, 64, 65, 66, 68, 69,
    70, 72, 74, 76, 77,  78,  79,  79,  80,  80,  80, 80, 79, 79, 79,  78,  77,  76,  74,  73, 72, 71, 70, 70, 70, 70,
    70, 70, 70, 71, 71,  71,  71,  71,  71,  72,  72, 72, 72, 73, 73,  73,  73,  74,  74,  74, 74, 74, 75, 75, 75, 75,
    75, 75, 75, 75, 74,  74,  74,  74,  74,  74,  73, 73, 73, 73, 73,  73,  73,  73,  73,  73, 73, 73, 73, 72, 72, 71,
    71, 70, 70, 69, 70,  70,  72,  74,  76,  78,  80, 82, 82, 82, 81,  79,  77,  75,  72,  70, 69, 68, 68, 68, 68, 68,
    69, 72, 80, 93, 104, 115, 126, 121, 106, 93,  80, 68, 59, 57, 58,  60,  61,  62,  62,  63, 63, 64, 65, 67, 68, 70,
    71, 73, 75, 76, 77,  78,  79,  80,  80,  80,  80, 80, 79, 79, 78,  78,  76,  75,  74,  72, 71, 70, 70, 70, 70, 70,
    70, 70, 70, 71, 71,  71,  71,  71,  71,  72,  72, 72, 73, 73, 73,  73,  74,  74,  74,  74, 74, 75, 75, 75, 75, 75,
    75, 75, 75, 75, 74,  74,  74,  74,  74,  73,  73, 73, 73, 73, 73,  73,  73,  73,  73,  73, 73, 73, 73, 72, 72, 71,
    71, 70, 70, 69, 70,  71,  72,  75,  77,  79,  81, 82, 82, 82, 80,  79,  76,  74,  71,  70, 69, 68, 68, 68, 68, 69,
    70, 75, 85, 97, 109, 120, 127, 115, 101, 88,  75, 64, 57, 56, 58,  60,  61,  62,  62,  63, 64, 65, 66, 67, 69, 71,
    72, 74, 75, 77, 78,  79,  79,  80,  80,  80,  80, 80, 79, 79, 78,  77,  76,  74,  73,  72, 71, 70, 70, 70, 70, 70,
    70, 70, 71, 71, 71,  71,  71,  71,  72,  72,  72, 72, 80, 93, 104, 115, 126, 121, 106, 93, 80, 68, 59, 57, 58, 60,
    61, 62, 62, 63, 63,  64,  65,  67,  68,  70,  71, 73};

class ImageAnimatorCallbackDemo : public OHOS::AnimatorCallback {
public:
    explicit ImageAnimatorCallbackDemo(UIChartDataSerial* serial) : times_(0), serial_(serial) {}

    ~ImageAnimatorCallbackDemo() {}

    virtual void Callback(UIView* view)
    {
        if (view == nullptr) {
            return;
        }
        if (!g_addPointsFlag) {
            return; /* Control the addition of points by button, and automate if commented out */
        }
        if (times_++ != 6) { /* Add 10 points for every 6 ticks */
            return;
        }
        times_ = 0;
        if (g_curSerialCount == 0) {
            view->Invalidate();
        }
        Point pointArray1[10];
        for (uint16_t i = 0; i < 10; i++) { // 10: array max index
            pointArray1[i].x = g_curSerialCount;
            pointArray1[i].y = g_ECGData[g_curArrayCount];
            g_curSerialCount++;
            g_curArrayCount++;
            if (g_curArrayCount == DATA_COUNT) {
                g_curArrayCount = 0;
            }
            if (!g_secondScreenFlag) {
                serial_->AddPoints(&pointArray1[i], 1);
            } else {
                serial_->ModifyPoint(g_curSerialCount, pointArray1[i]);
                if (g_hidePointsFlag) {
                    serial_->HidePoint(g_curSerialCount, 30); // 30: the number of points
                }
            }
        }
        if (g_curSerialCount > 454) { // 454: max serial count
            g_curSerialCount = 0;
            g_secondScreenFlag = true;
        }

        UIChart* chart = static_cast<UIChart*>(view);
        chart->RefreshChart();
        g_addPointsFlag = false;
    }

protected:
    int16_t times_;
    UIChartDataSerial* serial_;
};

void UITestChartPolyline::UIKit_ChartPolyline_Test_AddPoints_004()
{
    UIViewGroup* uiViewGroup = new UIViewGroup();
    // 2: x-coordinate, half of screen width; 2: half of screen width; 470: screen height
    uiViewGroup->SetPosition(Screen::GetInstance().GetWidth() / 2, 0, Screen::GetInstance().GetWidth() / 2, 470);
    container_->Add(uiViewGroup);

    UILabel* label = new UILabel();
    uiViewGroup->Add(label);
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE,
                       Screen::GetInstance().GetWidth() / 2 - TEXT_DISTANCE_TO_LEFT_SIDE, // 2: half of screen width;
                       TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetText("chart追加点、修改点、平滑化");
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

    ECGChart_ = new UIChartPolyline();
    // 454: new width, 250: new height
    ECGChart_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE, 454, 250);
    uiViewGroup->Add(ECGChart_);
    SetLastPos(ECGChart_);
    ECGChart_->SetStyle(STYLE_LINE_WIDTH, 5); // 5: line width

    UIXAxis& xAxis = ECGChart_->GetXAxis();
    UIYAxis& yAxis = ECGChart_->GetYAxis();
    xAxis.SetDataRange(0, 454); // 454: maximum value
    xAxis.SetMarkNum(10);       // 10: number of scales
    yAxis.SetDataRange(0, 200); // 200: maximum value

    ECGDataSerial_ = new UIChartDataSerial();
    ECGDataSerial_->SetMaxDataCount(454); // 454: number of data points
    ECGDataSerial_->SetLineColor(Color::Red());
    ECGDataSerial_->EnableHeadPoint(true);
    ECGChart_->AddDataSerial(ECGDataSerial_);
    ImageAnimatorCallbackDemo* imageAnimCallback = new ImageAnimatorCallbackDemo(ECGDataSerial_);
    ECGAnimator_ = new OHOS::Animator(imageAnimCallback, ECGChart_, 0, true);
    ECGAnimator_->Start();

    addPointsBtn_ = new UILabelButton();
    smoothBtn_ = new UILabelButton();
    hidePointsBtn_ = new UILabelButton();

    // 2: half of screen width
    positionX_ = Screen::GetInstance().GetWidth() / 2 + VIEW_DISTANCE_TO_LEFT_SIDE;
    positionY_ = lastY_ + 10; // 10: increase y-coordinate
    SetUpButton(addPointsBtn_, "添加点 ");

    positionX_ = addPointsBtn_->GetX() + addPointsBtn_->GetWidth() + g_blank;
    positionY_ = addPointsBtn_->GetY();
    SetUpButton(smoothBtn_, "平滑化 ");

    positionX_ = smoothBtn_->GetX() + smoothBtn_->GetWidth() + g_blank;
    positionY_ = smoothBtn_->GetY();
    SetUpButton(hidePointsBtn_, "隐藏点 ");
}

bool UITestChartPolyline::OnClick(UIView& view, const ClickEvent& event)
{
    UIChartDataSerial::PointStyle pointStyle;
    pointStyle.fillColor = Color::White();
    pointStyle.radius = 5; // 5: Inner radius
    pointStyle.strokeColor = Color::Red();
    pointStyle.strokeWidth = 2; // 2: border width

    if (&view == addDataSerialBtn_) {
        if (curDataIndex_ >= DATA_NUM) {
            return true;
        }
        chart_->AddDataSerial(dataSerial_[curDataIndex_]);
        curDataIndex_++;
        chart_->Invalidate();
    } else if (&view == deleteDataSerialBtn_) {
        if (curDataIndex_ <= 0) {
            return true;
        }
        chart_->DeleteDataSerial(dataSerial_[curDataIndex_ - 1]);
        curDataIndex_--;
        chart_->Invalidate();
    } else if (&view == clearDataSerialBtn_) {
        chart_->ClearDataSerial();
        curDataIndex_ = 0;
        chart_->Invalidate();
    } else if (&view == topPointBtn_) {
        dataSerial_[0]->EnableTopPoint(true);
        pointStyle.strokeColor = Color::Red();
        dataSerial_[0]->SetTopPointStyle(pointStyle);
        chart_->Invalidate();
    } else if (&view == bottomPointBtn_) {
        dataSerial_[0]->EnableBottomPoint(true);
        pointStyle.strokeColor = Color::Blue();
        dataSerial_[0]->SetBottomPointStyle(pointStyle);
        chart_->Invalidate();
    } else if (&view == headPointBtn_) {
        dataSerial_[0]->EnableHeadPoint(true);
        pointStyle.strokeColor = Color::Yellow();
        dataSerial_[0]->SetHeadPointStyle(pointStyle);
        chart_->Invalidate();
    } else if (&view == reverseBtn_) {
        chart_->EnableReverse(true);
        chart_->Invalidate();
    } else {
        ClickExpand(view, pointStyle);
    }
    return true;
}

bool UITestChartPolyline::ClickExpand(UIView& view, UIChartDataSerial::PointStyle pointStyle)
{
    if (&view == gradientBottomBtn_) {
        chart_->SetGradientBottom(50); // 50: bottom of the filling range
        chart_->Invalidate();
    } else if (&view == addPointsBtn_) {
        g_addPointsFlag = true;
    } else if (&view == smoothBtn_) {
        ECGDataSerial_->EnableSmooth(true);
        ECGChart_->Invalidate();
    } else if (&view == hidePointsBtn_) {
        g_hidePointsFlag = true;
    }
    return true;
}

void UITestChartPolyline::SetUpButton(UILabelButton* btn, const char* title)
{
    if (btn == nullptr) {
        return;
    }
    container_->Add(btn);
    btn->SetPosition(positionX_, positionY_, BUTTON_WIDHT2, BUTTON_HEIGHT2);
    positionY_ += btn->GetHeight() + 10; // 10: increase height
    btn->SetText(title);
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    btn->SetOnClickListener(this);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    container_->Invalidate();
}

void UITestChartPolyline::SetLastPos(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    lastX_ = view->GetX();
    lastY_ = view->GetY() + view->GetHeight();
}
} // namespace OHOS
