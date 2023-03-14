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

#include "ui_test_label.h"
#include "common/screen.h"
#include "components/ui_checkbox.h"
#include "components/ui_label.h"
#include "font/ui_font.h"
#if ENABLE_VECTOR_FONT
#include "common/spannable_string.h"
#endif
namespace OHOS {
void UITestLabel::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        positionX_ = 0;
        positionY_ = 0;
    }
}

void UITestLabel::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    uiLabel = nullptr;
    labelFontSizeBtn1_ = nullptr;
    labelFontSizeBtn2_ = nullptr;
    labelFontSizeBtn3_ = nullptr;
    labelHorAlignBtn1_ = nullptr;
    labelHorAlignBtn2_ = nullptr;
    labelHorAlignBtn3_ = nullptr;
    labelColorBtn1_ = nullptr;
    labelColorBtn2_ = nullptr;
    labelColorBtn3_ = nullptr;
    labelBeyondBtn1_ = nullptr;
    labelBeyondBtn2_ = nullptr;
    labelBeyondBtn3_ = nullptr;
    labelLineBtn1_ = nullptr;
    labelLineBtn2_ = nullptr;
    labelVerAlignlBtn1_ = nullptr;
    labelVerAlignlBtn2_ = nullptr;
    labelVerAlignlBtn3_ = nullptr;
    labelDirectionBtn1_ = nullptr;
    labelDirectionBtn2_ = nullptr;
    labelSizeBtn1_ = nullptr;
    labelSizeBtn2_ = nullptr;
}

const UIView* UITestLabel::GetTestView()
{
    UIKit_UILabel_Test_Display_001();
    UIKit_UILabel_Test_Display_002();
    UIKit_UILabel_Test_Display_003();
    return container_;
}

void UITestLabel::UIKit_UILabel_Test_Display_001()
{
    if (container_ != nullptr) {
        UIViewGroup* uiViewGroup = new UIViewGroup();
        // 320: width; 390: height
        uiViewGroup->SetPosition(positionX_, positionY_, 320, 390);
        container_->Add(uiViewGroup);
        UILabel* label = new UILabel();
        uiViewGroup->Add(label);
        // 288: width; 48: height
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, 288, 48);
        label->SetText("UILabel效果");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        UIViewGroup* uiViewGroupFrame = new UIViewGroup();
        uiViewGroup->Add(uiViewGroupFrame);
        // 288: width; 336: height
        uiViewGroupFrame->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, VIEW_DISTANCE_TO_TOP_SIDE, 288, 336);
        uiViewGroupFrame->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
        uiViewGroupFrame->SetStyle(STYLE_BORDER_OPA, HALF_OPA_OPAQUE);
        uiViewGroupFrame->SetStyle(STYLE_BORDER_WIDTH, VIEW_STYLE_BORDER_WIDTH);
        uiViewGroupFrame->SetStyle(STYLE_BORDER_RADIUS, VIEW_STYLE_BORDER_RADIUS);
        uiViewGroupFrame->SetStyle(STYLE_BACKGROUND_OPA, 0);
        uiLabel = new UILabel();
        uiLabel->SetText("图形子系统图形子系统图形子系统图形子系统图形子系统图形子系统图形子系统图形子系统图形子系统");
        uiLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 26); // 26: font size
        uiLabel->SetPosition(0, 141, 280, 336);             // 141: y-coordinate; 280: width; 336: height
        uiViewGroupFrame->Add(uiLabel);
        positionX_ += 336; // 336: x-coordinate

        UIViewGroup* uiViewGroup2 = new UIViewGroup();
        uiViewGroup2->SetPosition(positionX_, 0, 300, 384); // 300: width; 384: height
        container_->Add(uiViewGroup2);
        CreateLabels(uiViewGroup2);
    }
}

void UITestLabel::CreateLabels(UIViewGroup* uiViewGroup)
{
    if (uiViewGroup == nullptr) {
        return;
    }
    UILabel* label1 = new UILabel();
    uiViewGroup->Add(label1);
    // 288: width; 48: height
    label1->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, TEXT_DISTANCE_TO_TOP_SIDE, 288, 48);
    label1->SetText("字号");
    label1->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    labelFontSizeBtn1_ = SetUpButton("18 ", 24, 48, uiViewGroup); // 24: x-coordinate; 48: y-coordinate
    labelFontSizeBtn2_ = SetUpButton("26", 116, 48, uiViewGroup); // 116: x-coordinate; 48: y-coordinate
    labelFontSizeBtn3_ = SetUpButton("30", 208, 48, uiViewGroup); // 208: x-coordinate; 48: y-coordinate

    UILabel* label2 = new UILabel();
    uiViewGroup->Add(label2);
    label2->SetPosition(24, 107, 288, 48); // 24: x-coordinate, 107: y-coordinate, 288: width; 48: height
    label2->SetText("水平对齐方式");
    label2->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    labelHorAlignBtn1_ = SetUpButton("左对齐 ", 24, 144, uiViewGroup);  // 24: x-coordinate; 144: y-coordinate
    labelHorAlignBtn2_ = SetUpButton("中对齐 ", 116, 144, uiViewGroup); // 116: x-coordinate; 144: y-coordinate
    labelHorAlignBtn3_ = SetUpButton("右对齐 ", 208, 144, uiViewGroup); // 208: x-coordinate; 144: y-coordinate

    UILabel* label3 = new UILabel();
    uiViewGroup->Add(label3);
    label3->SetPosition(24, 203, 288, 48); // 24: x-coordinate, 203: y-coordinate, 288: width; 48: height
    label3->SetText("颜色");
    label3->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    labelColorBtn1_ = SetUpButton("白色 ", 24, 240, uiViewGroup);    // 24: x-coordinate; 240: y-coordinate
    labelColorBtn2_ = SetUpButton("变色 ", 116, 240, uiViewGroup);   // 116: x-coordinate; 240: y-coordinate
    labelColorBtn3_ = SetUpButton("半透明 ", 208, 240, uiViewGroup); // 208: x-coordinate; 240: y-coordinate

    UILabel* label4 = new UILabel();
    uiViewGroup->Add(label4);
    label4->SetPosition(24, 299, 288, 48); // 24: x-coordinate, 299: y-coordinate, 288: width; 48: height
    label4->SetText("超出处理");
    label4->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    labelBeyondBtn1_ = SetUpButton("截断 ", 24, 336, uiViewGroup);  // 24: x-coordinate; 336: y-coordinate
    labelBeyondBtn2_ = SetUpButton("省略 ", 116, 336, uiViewGroup); // 116: x-coordinate; 336: y-coordinate
    labelBeyondBtn3_ = SetUpButton("滚动 ", 208, 336, uiViewGroup); // 208: x-coordinate; 336: y-coordinate
    positionX_ += 312;                                              // 312: increase y-coordinate
}

void UITestLabel::UIKit_UILabel_Test_Display_002()
{
    if (container_ != nullptr) {
        UIViewGroup* uiViewGroup = new UIViewGroup();
        uiViewGroup->SetPosition(positionX_, positionY_, 320, 390); // 320: width; 390: height
        container_->Add(uiViewGroup);
        UILabel* label = new UILabel();
        uiViewGroup->Add(label);
        // 288: width; 48: height
        label->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, TEXT_DISTANCE_TO_TOP_SIDE, 288, 48);
        label->SetText("行数");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        labelLineBtn1_ = SetUpButton("单行 ", 24, 48, uiViewGroup);  // 24: x-coordinate; 48: y-coordinate
        labelLineBtn2_ = SetUpButton("多行 ", 116, 48, uiViewGroup); // 116: x-coordinate; 48: y-coordinate
        UILabel* label2 = new UILabel();
        uiViewGroup->Add(label2);
        label2->SetPosition(24, 107, 288, 48); // 24: x-coordinate, 107: y-coordinate, 288: width; 48: height
        label2->SetText("竖直对齐方式");
        label2->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        labelVerAlignlBtn1_ = SetUpButton("上对齐 ", 24, 144, uiViewGroup);  // 24: x-coordinate; 144: y-coordinate
        labelVerAlignlBtn2_ = SetUpButton("中对齐 ", 116, 144, uiViewGroup); // 116: x-coordinate; 144: y-coordinate
        labelVerAlignlBtn3_ = SetUpButton("下对齐 ", 208, 144, uiViewGroup); // 208: x-coordinate; 144: y-coordinate
        UILabel* label3 = new UILabel();
        uiViewGroup->Add(label3);
        label3->SetPosition(24, 203, 288, 48); // 24: x-coordinate, 203: y-coordinate, 288: width; 48: height
        label3->SetText("方向");
        label3->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        labelDirectionBtn1_ = SetUpButton("左往右 ", 24, 240, uiViewGroup);  // 24: x-coordinate; 240: y-coordinate
        labelDirectionBtn2_ = SetUpButton("右往左 ", 116, 240, uiViewGroup); // 116: x-coordinate; 240: y-coordinate
        positionY_ += 384;                                                   // 384: increase x-coordinate
        positionX_ -= 312;                                                   // 312: increase y-coordinate
    }
}

void UITestLabel::UIKit_UILabel_Test_Display_003()
{
    if (container_ != nullptr) {
        UIViewGroup* uiViewGroup = new UIViewGroup();
        uiViewGroup->SetPosition(positionX_, positionY_, 450, 110); // 450: width; 110: height
        container_->Add(uiViewGroup);
        UILabel* label = new UILabel();
        uiViewGroup->Add(label);
        label->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, TEXT_DISTANCE_TO_TOP_SIDE, 420, 48); // 420: width; 48: height
        label->SetText("Label大小");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        // 24: x-coordinate; 48: y-coordinate; 120: width
        labelSizeBtn1_ = SetUpButton("动态宽度 ", 24, 48, uiViewGroup, 120);
        // 156: x-coordinate; 48: y-coordinate; 120: width
        labelSizeBtn2_ = SetUpButton("动态高度 ", 156, 48, uiViewGroup, 120);
    }
}

bool UITestLabel::OnClick(UIView& view, const ClickEvent& event)
{
    uiLabel->Resize(288, 100);                          // 288: width; 100: height
    uiLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 26); // 26: font size
    if (&view == labelFontSizeBtn1_) {
        uiLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 18); // 18: font size
    } else if (&view == labelFontSizeBtn2_) {
        uiLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 26); // 26: font size
    } else if (&view == labelFontSizeBtn3_) {
        uiLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
    } else if (&view == labelHorAlignBtn1_) {
        uiLabel->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT);
    } else if (&view == labelHorAlignBtn2_) {
        uiLabel->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    } else if (&view == labelHorAlignBtn3_) {
        uiLabel->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_RIGHT);
    } else if (&view == labelColorBtn1_) {
        uiLabel->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
        uiLabel->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    } else if (&view == labelColorBtn2_) {
        uiLabel->SetStyle(STYLE_TEXT_COLOR, Color::Red().full);
        uiLabel->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    } else if (&view == labelColorBtn3_) {
        uiLabel->SetStyle(STYLE_TEXT_OPA, 127); // 127: opaque
    } else if (&view == labelBeyondBtn1_) {
        uiLabel->Resize(288, 35); // 288: width; 35: height
        uiLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        uiLabel->SetText("Test of 图形子系统，Test of 图形子系统 ");
        uiLabel->SetLineBreakMode(UILabel::LINE_BREAK_CLIP);
    } else if (&view == labelBeyondBtn2_) {
        uiLabel->Resize(288, 35); // 288: width; 35: height
        uiLabel->SetText("Test of 图形子系统，Test of 图形子系统 ");
        uiLabel->SetLineBreakMode(UILabel::LINE_BREAK_ELLIPSIS);
    } else if (&view == labelBeyondBtn3_) {
        uiLabel->SetText("Test of 图形子系统，Test of 图形子系统 ");
        uiLabel->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
    } else {
        ExpandClick(view, event);
    }
    return true;
}

void UITestLabel::ExpandClick(UIView& view, const ClickEvent& event) const
{
    if (&view == labelLineBtn1_) {
        uiLabel->Resize(288, 50);                           // 288: width; 50: height
        uiLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30:font size
        uiLabel->SetLineBreakMode(UILabel::LINE_BREAK_ELLIPSIS);
    } else if (&view == labelLineBtn2_) {
        uiLabel->Resize(288, 100);                          // 288: width; 100: height
        uiLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30:font size
        uiLabel->SetLineBreakMode(UILabel::LINE_BREAK_ELLIPSIS);
    } else if (&view == labelVerAlignlBtn1_) {
        uiLabel->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT, UITextLanguageAlignment::TEXT_ALIGNMENT_TOP);
    } else if (&view == labelVerAlignlBtn2_) {
        uiLabel->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT, UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    } else if (&view == labelVerAlignlBtn3_) {
        uiLabel->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT, UITextLanguageAlignment::TEXT_ALIGNMENT_BOTTOM);
    } else if (&view == labelDirectionBtn1_) {
        uiLabel->SetDirect(TEXT_DIRECT_LTR);
    } else if (&view == labelDirectionBtn2_) {
        uiLabel->SetDirect(TEXT_DIRECT_RTL);
    } else if (&view == labelSizeBtn1_) {
        uiLabel->SetText("Test of 图形子系统，Test of 图形子系统 ");
        uiLabel->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    } else if (&view == labelSizeBtn2_) {
        uiLabel->SetText("Test of 图形子系统，Test of 图形子系统 ");
        uiLabel->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    }
}
} // namespace OHOS
