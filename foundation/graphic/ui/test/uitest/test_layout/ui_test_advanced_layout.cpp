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

#include "ui_test_advanced_layout.h"
#include "common/screen.h"

namespace OHOS {
namespace {
const int16_t TEXT_H = 20;
const int16_t TEXT_W = 250;
const int16_t TEST_VIEW_H = 40;
const int16_t TEST_VIEW_W = 40;
const int16_t GAP = 5;
const int16_t TEST_VIEW_GAP = 80;
} // namespace

void UITestAdvancedLayout::SetUp()
{
    container_ = new UIScrollView();
    flexController_ = new GridLayout();
    fTarget_ = new FlexLayout();
    resetfBtn_ = new UILabelButton();
    marginfBtn_ = new UILabelButton();

    horfBtn_ = new UILabelButton();
    horfRBtn_ = new UILabelButton();
    verfBtn_ = new UILabelButton();
    verfRBtn_ = new UILabelButton();
    wrapBtn_ = new UILabelButton();
    addElefBtn_ = new UILabelButton();
    addTextElefBtn_ = new UILabelButton();
    majorStartBtn_ = new UILabelButton();
    majorEndBtn_ = new UILabelButton();
    majorCenterBtn_ = new UILabelButton();
    majorEvenBtn_ = new UILabelButton();
    majorAroundBtn_ = new UILabelButton();
    majorBetBtn_ = new UILabelButton();
    secStartBtn_ = new UILabelButton();
    secEndBtn_ = new UILabelButton();
    secCenterBtn_ = new UILabelButton();
    secInvalidBtn_ = new UILabelButton();
    layoutChildrenfBtn_ = new UILabelButton();

    gridController_ = new GridLayout();
    gTarget_ = new GridLayout();
    horgBtn_ = new UILabelButton();
    horgRBtn_ = new UILabelButton();
    vergBtn_ = new UILabelButton();
    vergRBtn_ = new UILabelButton();
    incRowsBtn_ = new UILabelButton();
    decRowsBtn_ = new UILabelButton();
    incColsBtn_ = new UILabelButton();
    decColsBtn_ = new UILabelButton();
    addElegBtn_ = new UILabelButton();
    resetgBtn_ = new UILabelButton();
    margingBtn_ = new UILabelButton();
    layoutChildrengBtn_ = new UILabelButton();
    container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
    positionX_ = 20; // 20: position x
    positionY_ = 5;  // 5: position y
}

void UITestAdvancedLayout::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestAdvancedLayout::GetTestView()
{
    UIKit_Layout_Test_FlexLayout_001();
    UIKit_Layout_Test_GridLayout_001();
    return container_;
}

void UITestAdvancedLayout::UIKit_Layout_Test_FlexLayout_001()
{
    if (container_ != nullptr) {
        UIViewGroup* uiViewGroup = new UIViewGroup();
        uiViewGroup->SetPosition(0, 0, Screen::GetInstance().GetWidth(), 420); // 420: height
        container_->Add(uiViewGroup);
        UILabel* label = new UILabel();
        uiViewGroup->Add(label);
        // 288: width; 48: height
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, 288, 48);
        label->SetText("Flex布局效果");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        UIViewGroup* uiViewGroupFrame = new UIViewGroup();
        uiViewGroup->Add(uiViewGroupFrame);
        uiViewGroupFrame->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, VIEW_DISTANCE_TO_TOP_SIDE,
                                      Screen::GetInstance().GetWidth() - VIEW_DISTANCE_TO_LEFT_SIDE,
                                      100); // 100: height
        uiViewGroupFrame->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
        uiViewGroupFrame->SetStyle(STYLE_BORDER_OPA, HALF_OPA_OPAQUE);
        uiViewGroupFrame->SetStyle(STYLE_BORDER_WIDTH, VIEW_STYLE_BORDER_WIDTH);
        uiViewGroupFrame->SetStyle(STYLE_BORDER_RADIUS, VIEW_STYLE_BORDER_RADIUS);
        uiViewGroupFrame->SetStyle(STYLE_BACKGROUND_OPA, 0);
        uiViewGroupFrame->Add(fTarget_);

        fTarget_->SetPosition(0, 0, Screen::GetInstance().GetWidth() - VIEW_DISTANCE_TO_LEFT_SIDE, 100); // 100: height
        fTarget_->SetStyle(STYLE_BACKGROUND_OPA, 100);                                                   // 100: opacity

        positionY_ += 160; // 160: position y add
        uiViewGroup->Add(flexController_);
        flexController_->SetPosition(0, positionY_, Screen::GetInstance().GetWidth(), 250); // 250: height
        flexController_->SetLayoutDirection(LAYOUT_HOR);
        flexController_->SetRows(5); // 5: row
        flexController_->SetCols(4); // 4: col

        SetUpButton(flexController_, resetfBtn_, "reset");
        SetUpButton(flexController_, marginfBtn_, "margin");
        SetUpButton(flexController_, horfBtn_, "hor");
        SetUpButton(flexController_, horfRBtn_, "hor_r");
        SetUpButton(flexController_, verfBtn_, "ver");
        SetUpButton(flexController_, verfRBtn_, "ver_r");
        SetUpButton(flexController_, wrapBtn_, "wrap");
        SetUpButton(flexController_, addElefBtn_, "add");
        SetUpButton(flexController_, addTextElefBtn_, "addLabel");
        SetUpButton(flexController_, majorStartBtn_, "M_start");
        SetUpButton(flexController_, majorEndBtn_, "M_end");
        SetUpButton(flexController_, majorCenterBtn_, "M_center");
        SetUpButton(flexController_, majorEvenBtn_, "M_even");
        SetUpButton(flexController_, majorAroundBtn_, "M_around");
        SetUpButton(flexController_, majorBetBtn_, "M_between");
        SetUpButton(flexController_, secStartBtn_, "S_start");
        SetUpButton(flexController_, secEndBtn_, "S_end");
        SetUpButton(flexController_, secCenterBtn_, "S_center");
        SetUpButton(flexController_, secInvalidBtn_, "S_invalid");
        SetUpButton(flexController_, layoutChildrenfBtn_, "布局");
        flexController_->LayoutChildren();
        positionY_ += 250; // 250: position y add
    }
}

void UITestAdvancedLayout::UIKit_Layout_Test_GridLayout_001()
{
    if (container_ != nullptr) {
        UIViewGroup* uiViewGroup = new UIViewGroup();
        uiViewGroup->SetPosition(0, positionY_, Screen::GetInstance().GetWidth(), 370); // 370: height
        container_->Add(uiViewGroup);
        UILabel* label = new UILabel();
        uiViewGroup->Add(label);
        // 288: widht; 48: height
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, 288, 48);
        label->SetText("Grid布局测试");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        UIViewGroup* uiViewGroupFrame = new UIViewGroup();
        uiViewGroup->Add(uiViewGroupFrame);
        uiViewGroupFrame->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, VIEW_DISTANCE_TO_TOP_SIDE,
                                      Screen::GetInstance().GetWidth() - VIEW_DISTANCE_TO_LEFT_SIDE,
                                      100); // 100: height
        uiViewGroupFrame->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
        uiViewGroupFrame->SetStyle(STYLE_BORDER_OPA, HALF_OPA_OPAQUE);
        uiViewGroupFrame->SetStyle(STYLE_BORDER_WIDTH, VIEW_STYLE_BORDER_WIDTH);
        uiViewGroupFrame->SetStyle(STYLE_BORDER_RADIUS, VIEW_STYLE_BORDER_RADIUS);
        uiViewGroupFrame->SetStyle(STYLE_BACKGROUND_OPA, 0);
        uiViewGroupFrame->Add(gTarget_);

        gTarget_->SetPosition(0, 0, Screen::GetInstance().GetWidth() - VIEW_DISTANCE_TO_LEFT_SIDE, 100); // 100: height
        uiViewGroup->Add(gridController_);
        gridController_->SetPosition(0, 160, Screen::GetInstance().GetWidth(), 150); // 160: y-coordinate; 150: height
        gridController_->SetLayoutDirection(LAYOUT_HOR);
        gridController_->SetRows(3); // 3: row
        gridController_->SetCols(4); // 4: col

        SetUpButton(gridController_, resetgBtn_, "reset");
        SetUpButton(gridController_, margingBtn_, "matgin");
        SetUpButton(gridController_, horgBtn_, "hor");
        SetUpButton(gridController_, horgRBtn_, "hor_r");
        SetUpButton(gridController_, vergBtn_, "ver");
        SetUpButton(gridController_, vergRBtn_, "ver_r");
        SetUpButton(gridController_, addElegBtn_, "add");
        SetUpButton(gridController_, incRowsBtn_, "incrows");
        SetUpButton(gridController_, decRowsBtn_, "decrows");
        SetUpButton(gridController_, incColsBtn_, "inccols");
        SetUpButton(gridController_, decColsBtn_, "deccols");
        SetUpButton(gridController_, layoutChildrengBtn_, "布局");
        gridController_->LayoutChildren();
    }
}

void UITestAdvancedLayout::OnClickButton(const UIView& view)
{
    if (&view == resetgBtn_) {
        Clear(gTarget_);
        gTarget_->SetLayoutDirection(LAYOUT_HOR);
        rows_ = 0;
        cols_ = 0;
        gTarget_->SetRows(rows_);
        gTarget_->SetCols(cols_);
        withMargin_ = false;
    } else if (&view == margingBtn_) {
        withMargin_ = true;
    } else if (&view == horgBtn_) {
        gTarget_->SetLayoutDirection(LAYOUT_HOR);
    } else if (&view == horgRBtn_) {
        gTarget_->SetLayoutDirection(LAYOUT_HOR_R);
    } else if (&view == vergBtn_) {
        gTarget_->SetLayoutDirection(LAYOUT_VER);
    } else if (&view == vergRBtn_) {
        gTarget_->SetLayoutDirection(LAYOUT_VER_R);
    } else if (&view == addElegBtn_) {
        AddElement(gTarget_);
    } else if (&view == incRowsBtn_) {
        rows_++;
        gTarget_->SetRows(rows_);
    } else if (&view == decRowsBtn_) {
        rows_--;
        gTarget_->SetRows(rows_);
    } else if (&view == incColsBtn_) {
        cols_++;
        gTarget_->SetCols(cols_);
    } else if (&view == decColsBtn_) {
        cols_--;
        gTarget_->SetCols(cols_);
    } else if (&view == layoutChildrenfBtn_) {
        fTarget_->LayoutChildren();
        fTarget_->Invalidate();
    } else if (&view == layoutChildrengBtn_) {
        gTarget_->LayoutChildren();
        gTarget_->Invalidate();
    } else {
    }
}

bool UITestAdvancedLayout::OnClick(UIView& view, const ClickEvent& event)
{
    fTarget_->Invalidate();
    gTarget_->Invalidate();
    if (&view == resetfBtn_) {
        Clear(fTarget_);
        fTarget_->SetFlexWrap(false);
        fTarget_->SetLayoutDirection(LAYOUT_HOR);
        fTarget_->SetMajorAxisAlign(ALIGN_START);
        fTarget_->SetSecondaryAxisAlign(ALIGN_CENTER);
        withMargin_ = false;
    } else if (&view == marginfBtn_) {
        withMargin_ = true;
    } else if (&view == horfBtn_) {
        fTarget_->SetLayoutDirection(LAYOUT_HOR);
    } else if (&view == horfRBtn_) {
        fTarget_->SetLayoutDirection(LAYOUT_HOR_R);
    } else if (&view == verfBtn_) {
        fTarget_->SetLayoutDirection(LAYOUT_VER);
    } else if (&view == verfRBtn_) {
        fTarget_->SetLayoutDirection(LAYOUT_VER_R);
    } else if (&view == wrapBtn_) {
        fTarget_->SetFlexWrap(true);
    } else if (&view == addElefBtn_) {
        AddElement(fTarget_);
    } else if (&view == addTextElefBtn_) {
        AddTextElement(fTarget_);
    } else if (&view == majorStartBtn_) {
        fTarget_->SetMajorAxisAlign(ALIGN_START);
    } else if (&view == majorEndBtn_) {
        fTarget_->SetMajorAxisAlign(ALIGN_END);
    } else if (&view == majorCenterBtn_) {
        fTarget_->SetMajorAxisAlign(ALIGN_CENTER);
    } else if (&view == majorEvenBtn_) {
        fTarget_->SetMajorAxisAlign(ALIGN_EVENLY);
    } else if (&view == majorAroundBtn_) {
        fTarget_->SetMajorAxisAlign(ALIGN_AROUND);
    } else if (&view == majorBetBtn_) {
        fTarget_->SetMajorAxisAlign(ALIGN_BETWEEN);
    } else if (&view == secStartBtn_) {
        fTarget_->SetSecondaryAxisAlign(ALIGN_START);
    } else if (&view == secEndBtn_) {
        fTarget_->SetSecondaryAxisAlign(ALIGN_END);
    } else if (&view == secCenterBtn_) {
        fTarget_->SetSecondaryAxisAlign(ALIGN_CENTER);
    } else {
        OnClickButton(view);
    }
    return true;
}
} // namespace OHOS
