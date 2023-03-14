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

#include "ui_test_basic_layout.h"
#include "common/screen.h"

namespace OHOS {
namespace {
const int16_t ITEM_H = 50;
} // namespace

void UITestBasicLayout::SetUp()
{
    container_ = new UIScrollView();
    layout_ = new GridLayout();
    uiViewGroupFrame_ = new UIViewGroup();
    testTarget_ = new UIViewGroup();
    testTargetSmall1_ = new UIView();
    testTargetSmall2_ = new UIView();
    refTarget_ = new UIView();
    resetBtn_ = new UILabelButton();
    marginBtn_ = new UILabelButton();
    padBtn_ = new UILabelButton();

    lCenterOfParentBtn_ = new UILabelButton();
    lLeftOfParentBtn_ = new UILabelButton();
    lRightOfParentBtn_ = new UILabelButton();
    lTopOfParentBtn_ = new UILabelButton();
    lBottomOfParentBtn_ = new UILabelButton();

    aLeftToSiblingBtn_ = new UILabelButton();
    aRightToSiblingBtn_ = new UILabelButton();
    aTopToSiblingBtn_ = new UILabelButton();
    aBottomToSiblingBtn_ = new UILabelButton();
    aHorCenterToSiblingBtn_ = new UILabelButton();
    aVerCenterToSiblingBtn_ = new UILabelButton();

    lLeftToSiblingBtn_ = new UILabelButton();
    lRightToSiblingBtn_ = new UILabelButton();
    lTopToSiblingBtn_ = new UILabelButton();
    lBottomToSiblingBtn_ = new UILabelButton();
    lInvalidLayoutBtn_ = new UILabelButton();
    container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
}

void UITestBasicLayout::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    layout_ = nullptr;
    testTarget_ = nullptr;
    uiViewGroupFrame_ = nullptr;
    testTargetSmall1_ = nullptr;
    testTargetSmall2_ = nullptr;
    refTarget_ = nullptr;
    resetBtn_ = nullptr;
    lCenterOfParentBtn_ = nullptr;
    lLeftOfParentBtn_ = nullptr;
    lRightOfParentBtn_ = nullptr;
    lTopOfParentBtn_ = nullptr;
    lBottomOfParentBtn_ = nullptr;
    aLeftToSiblingBtn_ = nullptr;
    aRightToSiblingBtn_ = nullptr;
    aTopToSiblingBtn_ = nullptr;
    aBottomToSiblingBtn_ = nullptr;
    aHorCenterToSiblingBtn_ = nullptr;
    aVerCenterToSiblingBtn_ = nullptr;
    lLeftToSiblingBtn_ = nullptr;
    lRightToSiblingBtn_ = nullptr;
    lTopToSiblingBtn_ = nullptr;
    lBottomToSiblingBtn_ = nullptr;
    lInvalidLayoutBtn_ = nullptr;
    marginBtn_ = nullptr;
    padBtn_ = nullptr;
    setStyle_ = nullptr;
    positionX_ = 20; // 20: position x
    positionY_ = 5;  // 5 position y
}

const UIView* UITestBasicLayout::GetTestView()
{
    UIKitLayoutTestBasicLayout001();
    return container_;
}

void UITestBasicLayout::UIKitLayoutTestBasicLayout001()
{
    if (container_ != nullptr) {
        UIViewGroup* uiViewGroup = new UIViewGroup();
        uiViewGroup->SetPosition(0, 0, 320, 390); // 320: width; 390: height
        container_->Add(uiViewGroup);
        UILabel* label = new UILabel();
        uiViewGroup->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, 288, 48); // 288: width; 48: height
        label->SetText("布局效果");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        uiViewGroup->Add(uiViewGroupFrame_);
        // 288: width; 336: height
        uiViewGroupFrame_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, VIEW_DISTANCE_TO_TOP_SIDE, 288, 336);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_OPA, HALF_OPA_OPAQUE);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_WIDTH, VIEW_STYLE_BORDER_WIDTH);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_RADIUS, VIEW_STYLE_BORDER_RADIUS);
        uiViewGroupFrame_->SetStyle(STYLE_BACKGROUND_OPA, 0);
        uiViewGroupFrame_->Add(testTarget_);
        testTarget_->SetPosition(0, 0, 96, 96); // 96: width 96:height
        testTargetSmall1_->Resize(40, 40);      // 40:width 40:height
        testTargetSmall2_->Resize(20, 20);      // 20:width 20:height
        testTargetSmall1_->LayoutCenterOfParent();
        testTargetSmall2_->LayoutCenterOfParent();
        testTargetSmall1_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Purple().full);
        testTargetSmall2_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Cyan().full);
        testTarget_->SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0, 182, 191).full); // 182, 191: color
        uiViewGroupFrame_->Add(refTarget_);
        refTarget_->Resize(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
        refTarget_->SetViewId("refView");
        refTarget_->LayoutCenterOfParent();
        refTarget_->SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(255, 152, 0).full); // 255, 152: color

        UIViewGroup* uiViewGroup2 = new UIViewGroup();
        uiViewGroup2->SetPosition(336, 0, 600, 384); // 336: x-coordinate; 600: width; 384: height
        container_->Add(uiViewGroup2);
        UILabel* label2 = new UILabel();
        uiViewGroup2->Add(label2);
        // 288: width; 48: height
        label2->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, TEXT_DISTANCE_TO_TOP_SIDE, 288, 48);
        label2->SetText("布局效果");
        label2->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        uiViewGroup2->Add(layout_);
        layout_->SetPosition(0, VIEW_DISTANCE_TO_TOP_SIDE, 516, 296); // 516: width; 296: height
        layout_->SetLayoutDirection(LAYOUT_HOR);
        layout_->SetRows(5); // 5: row
        layout_->SetCols(4); // 4: col
        InitSetUpButton();
        layout_->LayoutChildren();
        positionY_ += ITEM_H;
    }
}

void UITestBasicLayout::InitSetUpButton()
{
    SetUpButton(resetBtn_, "重置", UI_TEST_BASIC_LAYOUT_RESET_BTN_ID);
    SetUpButton(marginBtn_, "margin", UI_TEST_BASIC_LAYOUT_MARGIN_BTN_ID);
    SetUpButton(padBtn_, "padding", UI_TEST_BASIC_LAYOUT_PADDING_BTN_ID);
    SetUpButton(lCenterOfParentBtn_, "居中", UI_TEST_BASIC_LAYOUT_CENTEROFPARENT_BTN_ID);
    SetUpButton(lLeftOfParentBtn_, "居左", UI_TEST_BASIC_LAYOUT_LEFTOFPARENT_BTN_ID);
    SetUpButton(lRightOfParentBtn_, "居右", UI_TEST_BASIC_LAYOUT_RIGHTOFPARENT_BTN_ID);
    SetUpButton(lTopOfParentBtn_, "居上", UI_TEST_BASIC_LAYOUT_TOPOFPARENT_BTN_ID);
    SetUpButton(lBottomOfParentBtn_, "居下", UI_TEST_BASIC_LAYOUT_BOTTOMOFPARENT_BTN_ID);
    SetUpButton(aLeftToSiblingBtn_, "左侧对齐兄弟", UI_TEST_BASIC_LAYOUT_LEFTOFSIBLING_BTN_ID);
    SetUpButton(aRightToSiblingBtn_, "右侧对齐兄弟", UI_TEST_BASIC_LAYOUT_RIGHTOFSIBLING_BTN_ID);
    SetUpButton(aTopToSiblingBtn_, "上侧对齐兄弟", UI_TEST_BASIC_LAYOUT_TOPOFSIBLING_BTN_ID);
    SetUpButton(aBottomToSiblingBtn_, "下侧对齐兄弟", UI_TEST_BASIC_LAYOUT_BOTTOMOFSIBLING_BTN_ID);
    SetUpButton(aHorCenterToSiblingBtn_, "水平居中对齐", UI_TEST_BASIC_LAYOUT_HORCENTEROFSIBLING_BTN_ID);
    SetUpButton(aVerCenterToSiblingBtn_, "竖直居中对齐", UI_TEST_BASIC_LAYOUT_VERCENTEROFSIBLING_BTN_ID);
    SetUpButton(lLeftToSiblingBtn_, "放至兄弟左侧", UI_TEST_BASIC_LAYOUT_LEFTTOSIBLING_BTN_ID);
    SetUpButton(lRightToSiblingBtn_, "放至兄弟右侧", UI_TEST_BASIC_LAYOUT_RIGHTTOSIBLING_BTN_ID);
    SetUpButton(lTopToSiblingBtn_, "放至兄弟上侧", UI_TEST_BASIC_LAYOUT_TOPTOSIBLING_BTN_ID);
    SetUpButton(lBottomToSiblingBtn_, "放至兄弟下侧", UI_TEST_BASIC_LAYOUT_BOTTOMTOSIBLING_BTN_ID);
    SetUpButton(lInvalidLayoutBtn_, "非法节点布局", UI_TEST_BASIC_LAYOUT_INVALIDLAYOUT_BTN_ID);
}

void UITestBasicLayout::OnClickButton(UIView& view)
{
    if (&view == aLeftToSiblingBtn_) {
        testTarget_->AlignLeftToSibling("refView");
    } else if (&view == aRightToSiblingBtn_) {
        testTarget_->AlignRightToSibling("refView");
    } else if (&view == aTopToSiblingBtn_) {
        testTarget_->AlignTopToSibling("refView");
    } else if (&view == aBottomToSiblingBtn_) {
        testTarget_->AlignBottomToSibling("refView");
    } else if (&view == aHorCenterToSiblingBtn_) {
        testTarget_->AlignHorCenterToSibling("refView");
    } else if (&view == aVerCenterToSiblingBtn_) {
        testTarget_->AlignVerCenterToSibling("refView");
    } else if (&view == lLeftToSiblingBtn_) {
        testTarget_->LayoutLeftToSibling("refView");
    } else if (&view == lRightToSiblingBtn_) {
        testTarget_->LayoutRightToSibling("refView");
    } else if (&view == lTopToSiblingBtn_) {
        testTarget_->LayoutTopToSibling("refView");
    } else if (&view == lBottomToSiblingBtn_) {
        testTarget_->LayoutBottomToSibling("refView");
    } else if (&view == lInvalidLayoutBtn_) {
        testTarget_->AlignLeftToSibling("invalid");
    } else {
    }
}

bool UITestBasicLayout::OnClick(UIView& view, const ClickEvent& event)
{
    uiViewGroupFrame_->Invalidate();
    if (&view == resetBtn_) {
        testTarget_->SetPosition(0, 0);
        testTarget_->SetStyle(STYLE_MARGIN_LEFT, 0);
        testTarget_->SetStyle(STYLE_MARGIN_RIGHT, 0);
        testTarget_->SetStyle(STYLE_MARGIN_TOP, 0);
        testTarget_->SetStyle(STYLE_MARGIN_BOTTOM, 0);
        uiViewGroupFrame_->SetStyle(STYLE_PADDING_LEFT, 0);
        uiViewGroupFrame_->SetStyle(STYLE_PADDING_RIGHT, 0);
        uiViewGroupFrame_->SetStyle(STYLE_PADDING_TOP, 0);
        uiViewGroupFrame_->SetStyle(STYLE_PADDING_BOTTOM, 0);
    } else if (&view == marginBtn_) {
        testTarget_->SetStyle(STYLE_MARGIN_LEFT, 10);   // 10: margin left
        testTarget_->SetStyle(STYLE_MARGIN_RIGHT, 10);  // 10: margin right
        testTarget_->SetStyle(STYLE_MARGIN_TOP, 10);    // 10: margin top
        testTarget_->SetStyle(STYLE_MARGIN_BOTTOM, 10); // 10: margin bottom
    } else if (&view == padBtn_) {
        uiViewGroupFrame_->SetStyle(STYLE_PADDING_LEFT, 15);   // 15: padding left
        uiViewGroupFrame_->SetStyle(STYLE_PADDING_RIGHT, 15);  // 15: padding right
        uiViewGroupFrame_->SetStyle(STYLE_PADDING_TOP, 15);    // 15: padding top
        uiViewGroupFrame_->SetStyle(STYLE_PADDING_BOTTOM, 15); // 15: padding bottom
        refTarget_->LayoutCenterOfParent();
    } else if (&view == lCenterOfParentBtn_) {
        testTarget_->LayoutCenterOfParent();
    } else if (&view == lLeftOfParentBtn_) {
        testTarget_->LayoutLeftOfParent();
    } else if (&view == lRightOfParentBtn_) {
        testTarget_->LayoutRightOfParent();
    } else if (&view == lTopOfParentBtn_) {
        testTarget_->LayoutTopOfParent();
    } else if (&view == lBottomOfParentBtn_) {
        testTarget_->LayoutBottomOfParent();
    } else {
        OnClickButton(view);
    }
    uiViewGroupFrame_->Invalidate();
    return true;
}
} // namespace OHOS
