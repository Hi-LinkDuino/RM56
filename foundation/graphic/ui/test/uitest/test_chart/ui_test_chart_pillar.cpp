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

#include "ui_test_chart_pillar.h"
#include "common/screen.h"

namespace OHOS {
namespace {
    static int16_t g_blank = 20;
}

void UITestChartPillar::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetThrowDrag(true);
    }

    dataSerial_[0] = new UIChartDataSerial();
    dataSerial_[0]->SetMaxDataCount(5); // 5: number of data points
    Point pointArray[5] = {{0, 2478}, {1, 2600}, {2, 3000}, {3, 3200}, {4, 3500}};
    dataSerial_[0]->AddPoints(pointArray, 5); // 5: number of data points
    dataSerial_[0]->SetFillColor(Color::Red());

    dataSerial_[1] = new UIChartDataSerial();
    dataSerial_[1]->SetMaxDataCount(5); // 5: number of data points
    Point pointArray1[5] = {{0, 2000}, {1, 0}, {2, 800}, {3, 700}, {4, 433}};
    dataSerial_[1]->AddPoints(pointArray1, 5); // 5: number of data points
    dataSerial_[1]->SetFillColor(Color::Green());

    dataSerial_[2] = new UIChartDataSerial(); // 2 array index
    dataSerial_[2]->SetMaxDataCount(5); // 2 array index, 5: number of data points
    Point pointArray2[5] = {{0, 100}, {1, 200}, {2, 300}, {3, 400}, {4, 500}};
    dataSerial_[2]->AddPoints(pointArray2, 5); // 2 array index, 5: number of data points
    dataSerial_[2]->SetFillColor(Color::Blue()); // 2 array index
    curDataIndex_ = 0;
}

void UITestChartPillar::InnerDeleteChildren(UIView* view) const
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

void UITestChartPillar::TearDown()
{
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
}

const UIView* UITestChartPillar::GetTestView()
{
    UIKit_ChartPillar_Test_AddDataSerial_001();
    UIKit_ChartPillar_Test_EnableReverse_002();
    UIKit_ChartPillar_Test_SetAxisLineColor_003();
    UIKit_ChartPillar_Test_SetAxisLineVisible_004();
    return container_;
}

void UITestChartPillar::UIKit_ChartPillar_Test_AddDataSerial_001()
{
    UILabel* label = GetTitleLabel("UIChartPillar效果 ");
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE);
    container_->Add(label);

    chart_ = new UIChartPillar();
    chart_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    chart_->SetWidth(454); // 454: width
    chart_->SetHeight(250); // 250: height

    UIXAxis& xAxis = chart_->GetXAxis();
    UIYAxis& yAxis = chart_->GetYAxis();
    xAxis.SetMarkNum(5); // 5: number of scales
    yAxis.SetDataRange(0, 5000); // 0: minimum value, 5000: maximum value

    chart_->AddDataSerial(dataSerial_[0]);
    curDataIndex_++;
    container_->Add(chart_);
    SetLastPos(chart_);

    addDataSerialBtn_ = new UILabelButton();
    deleteDataSerialBtn_ = new UILabelButton();
    clearDataSerialBtn_ = new UILabelButton();

    positionY_ = lastY_ + 10; // 10: increase y-coordinate
    positionX_ = 48; // 48: x-coordinate
    SetUpButton(addDataSerialBtn_, "添加数据 ");

    positionX_ = addDataSerialBtn_->GetX() + addDataSerialBtn_->GetWidth() + g_blank;
    positionY_ = addDataSerialBtn_->GetY();
    SetUpButton(deleteDataSerialBtn_, "删除数据 ");

    positionX_ = deleteDataSerialBtn_->GetX() + deleteDataSerialBtn_->GetWidth() + g_blank;
    positionY_ = deleteDataSerialBtn_->GetY();
    SetUpButton(clearDataSerialBtn_, "清空数据 ");
}

void UITestChartPillar::UIKit_ChartPillar_Test_EnableReverse_002()
{
    reverseBtn_ = new UILabelButton();
    positionX_ = 48; // 48: x-coordinate
    SetUpButton(reverseBtn_, "翻转 ");
}

void UITestChartPillar::UIKit_ChartPillar_Test_SetAxisLineColor_003()
{
    setAxisColorBtn_ = new UILabelButton();
    positionX_ = reverseBtn_->GetX() + reverseBtn_->GetWidth() + g_blank;
    positionY_ = reverseBtn_->GetY();
    SetUpButton(setAxisColorBtn_, "坐标轴颜色 ");
}

void UITestChartPillar::UIKit_ChartPillar_Test_SetAxisLineVisible_004()
{
    setAxisVisibleBtn_ = new UILabelButton();
    positionX_ = setAxisColorBtn_->GetX() + setAxisColorBtn_->GetWidth() + g_blank;
    positionY_ = setAxisColorBtn_->GetY();
    SetUpButton(setAxisVisibleBtn_, "坐标轴不可见");
}

bool UITestChartPillar::OnClick(UIView& view, const ClickEvent& event)
{
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
    } else if (&view == reverseBtn_) {
        chart_->EnableReverse(true);
        chart_->Invalidate();
    } else if (&view == setAxisColorBtn_) {
        UIXAxis& xAxis = chart_->GetXAxis();
        UIYAxis& yAxis = chart_->GetYAxis();
        xAxis.SetLineColor(Color::Red());
        yAxis.SetLineColor(Color::Red());
        chart_->Invalidate();
    } else if (&view == setAxisVisibleBtn_) {
        UIXAxis& xAxis = chart_->GetXAxis();
        UIYAxis& yAxis = chart_->GetYAxis();
        xAxis.SetVisible(false);
        yAxis.SetVisible(false);
        chart_->Invalidate();
    }
    return true;
}

void UITestChartPillar::SetUpButton(UILabelButton* btn, const char* title)
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

void UITestChartPillar::SetLastPos(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    lastX_ = view->GetX();
    lastY_ = view->GetY() + view->GetHeight();
}
} // namespace OHOS
