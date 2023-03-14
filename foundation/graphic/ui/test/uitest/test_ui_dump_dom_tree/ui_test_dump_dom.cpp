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

#include "graphic_config.h"

#if ENABLE_DEBUG
#include "ui_test_dump_dom.h"

#include "common/screen.h"
#include "components/ui_arc_label.h"
#include "components/ui_box_progress.h"
#include "components/ui_checkbox.h"
#include "components/ui_circle_progress.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_radio_button.h"
#include "components/ui_slider.h"
#include "components/ui_toggle_button.h"
#include "dfx/ui_dump_dom_tree.h"
#include "imgdecode/cache_manager.h"
#include "test_resource_config.h"
#include "gfx_utils/graphic_log.h"

namespace OHOS {
void UITestDumpDomTree::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetViewId("dump_scroll_view");
    }
}

void UITestDumpDomTree::TearDown()
{
    if (clickDumpDomListener1_ != nullptr) {
        delete clickDumpDomListener1_;
        clickDumpDomListener1_ = nullptr;
    }
    if (clickDumpDomListener2_ != nullptr) {
        delete clickDumpDomListener2_;
        clickDumpDomListener2_ = nullptr;
    }
    if (clickDumpDomListener3_ != nullptr) {
        delete clickDumpDomListener3_;
        clickDumpDomListener3_ = nullptr;
    }
    if (clickDumpDomListener4_ != nullptr) {
        delete clickDumpDomListener4_;
        clickDumpDomListener4_ = nullptr;
    }
    if (clickDumpDomListener5_ != nullptr) {
        delete clickDumpDomListener5_;
        clickDumpDomListener5_ = nullptr;
    }
    if (clickDumpDomListener6_ != nullptr) {
        delete clickDumpDomListener6_;
        clickDumpDomListener6_ = nullptr;
    }
    if (clickDumpDomListener7_ != nullptr) {
        delete clickDumpDomListener7_;
        clickDumpDomListener7_ = nullptr;
    }
    if (clickDumpDomListener8_ != nullptr) {
        delete clickDumpDomListener8_;
        clickDumpDomListener8_ = nullptr;
    }
    if (clickDumpDomListener9_ != nullptr) {
        delete clickDumpDomListener9_;
        clickDumpDomListener9_ = nullptr;
    }
    if (clickDumpDomListener10_ != nullptr) {
        delete clickDumpDomListener10_;
        clickDumpDomListener10_ = nullptr;
    }
    if (clickDumpDomListener11_ != nullptr) {
        delete clickDumpDomListener11_;
        clickDumpDomListener11_ = nullptr;
    }
    if (clickDumpDomListener12_ != nullptr) {
        delete clickDumpDomListener12_;
        clickDumpDomListener12_ = nullptr;
    }
    positionY_ = 0;
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestDumpDomTree::GetTestView()
{
    positionY_ = 0;
    UIKit_TestDumpDom_001();
    UIKit_TestDumpDom_002();
    UIKit_TestDumpDom_003();
    UIKit_TestDumpDom_004();
    UIKit_TestDumpDom_005();
    UIKit_TestDumpDom_006();
    return container_;
}

class TestBtnOnClickDumpDomListener : public UIView::OnClickListener {
public:
    TestBtnOnClickDumpDomListener(UIView* uiView, const char* id) : uiView_(uiView), id_(id) {}
    ~TestBtnOnClickDumpDomListener() {}
    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        char* tmpPtr = UIDumpDomTree::GetInstance()->DumpDomNode(id_);
        if (tmpPtr != nullptr) {
            cJSON_free(tmpPtr);
        }
        uiView_->Invalidate();
        return true;
    }

private:
    UIView* uiView_;
    const char* id_;
};

class TestBtnOnClickDumpDomTreeListener : public UIView::OnClickListener {
public:
    TestBtnOnClickDumpDomTreeListener(UIView* uiView, const char* id) : uiView_(uiView), id_(id) {}
    ~TestBtnOnClickDumpDomTreeListener() {}
    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        if (UIDumpDomTree::GetInstance()->DumpDomTree(id_)) {
            if (id_ == nullptr) {
                GRAPHIC_LOGI("dump tree from rootView success\n");
            } else {
                GRAPHIC_LOGI("dump tree from id success\n");
            }
        } else {
            GRAPHIC_LOGI("dump tree failed\n");
        }
        uiView_->Invalidate();
        return true;
    }

private:
    UIView* uiView_;
    const char* id_;
};

void UITestDumpDomTree::UIKit_TestDumpDom_001()
{
    if (container_ != nullptr) {
        UILabel* label = GetTitleLabel("dump checkbox");
        container_->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE);
        positionY_ += label->GetHeight();

        GridLayout* layout = new GridLayout();
        layout->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE,
        // 2: half of screen width; 88: decrease screen width; 180: height
            Screen::GetInstance().GetWidth() / 2 - 88, 180);
        layout->SetViewId("dump_group");
        container_->Add(layout);
        positionY_ += layout->GetHeight();
        layout->SetRows(3); // 3: row
        layout->SetCols(3); // 3: col
        CreateButtons(layout);
        layout->LayoutChildren();
    }
}

void UITestDumpDomTree::CreateButtons(GridLayout* layout)
{
    if (layout == nullptr) {
        return;
    }
    UICheckBox* checkBox = new UICheckBox();
    layout->Add(checkBox);
    checkBox->SetState(UICheckBox::SELECTED);
    checkBox->SetViewId("dump_checkbox");

    UIRadioButton* radioButton = new UIRadioButton("aaa");
    layout->Add(radioButton);
    radioButton->SetState(UICheckBox::UNSELECTED);
    radioButton->SetViewId("dump_radiobutton");

    UIToggleButton* toggleButton = new UIToggleButton();
    layout->Add(toggleButton);
    toggleButton->SetStyle(STYLE_BACKGROUND_OPA, 0);
    toggleButton->SetViewId("dump_togglebutton");

    UILabelButton* dumpButton1 = GetLabelButton("dump");
    layout->Add(dumpButton1);
    /* dump node here */
    if (clickDumpDomListener1_ == nullptr) {
        clickDumpDomListener1_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickDumpDomListener(static_cast<UIView*>(dumpButton1), "dump_checkbox"));
    }
    dumpButton1->SetOnClickListener(clickDumpDomListener1_);

    UILabelButton* dumpButton2 = GetLabelButton("dump");
    layout->Add(dumpButton2);
    /* dump node here */
    if (clickDumpDomListener2_ == nullptr) {
        clickDumpDomListener2_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickDumpDomListener(static_cast<UIView*>(dumpButton2), "dump_radiobutton"));
    }
    dumpButton2->SetOnClickListener(clickDumpDomListener2_);

    UILabelButton* dumpButton3 = GetLabelButton("dump");
    layout->Add(dumpButton3);
    /* dump node here */
    if (clickDumpDomListener3_ == nullptr) {
        clickDumpDomListener3_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickDumpDomListener(static_cast<UIView*>(dumpButton3), "dump_togglebutton"));
    }
    dumpButton3->SetOnClickListener(clickDumpDomListener3_);

    UILabelButton* dumpButton4 = GetLabelButton("dump_all ");
    layout->Add(dumpButton4);

    if (clickDumpDomListener4_ == nullptr) {
        clickDumpDomListener4_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickDumpDomTreeListener(static_cast<UIView*>(dumpButton4), nullptr));
    }
    dumpButton4->SetOnClickListener(clickDumpDomListener4_);

    UILabelButton* dumpButton5 = GetLabelButton("dump_group");
    layout->Add(dumpButton5);
    if (clickDumpDomListener5_ == nullptr) {
        clickDumpDomListener5_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickDumpDomTreeListener(static_cast<UIView*>(dumpButton5), "dump_group"));
    }
    dumpButton5->SetOnClickListener(clickDumpDomListener5_);
}

void UITestDumpDomTree::UIKit_TestDumpDom_002()
{
    if (container_ != nullptr) {
        UIViewGroup* group2 = new UIViewGroup();
        group2->SetPosition(Screen::GetInstance().GetWidth() / 2, 0); // 2: half of screen width
        group2->SetWidth(Screen::GetInstance().GetWidth() / 2); // 2: half of screen width
        group2->SetHeight(200); // 200: height
        group2->SetViewId("dump_group2");
        container_->Add(group2);
        positionY_ += group2->GetHeight();

        UILabel* label = GetTitleLabel("dump image");
        group2->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE);

        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(VIEW_DISTANCE_TO_TOP_SIDE, TEXT_DISTANCE_TO_LEFT_SIDE);
        imageView->SetWidth(50); // 50: width
        imageView->SetHeight(50); // 50: height
        imageView->SetSrc(BLUE_RGB888_IMAGE_PATH);
        imageView->SetViewId("dump_image");
        group2->Add(imageView);

        UILabelButton* dumpButton1 = GetLabelButton("dump");
        dumpButton1->SetPosition(160, 73); // 160: x-coordinate, 73: y-coordinate
        group2->Add(dumpButton1);
        dumpButton1->SetText("dump");
        /* dump node here */
        if (clickDumpDomListener6_ == nullptr) {
            clickDumpDomListener6_ = static_cast<UIView::OnClickListener*>(
                new TestBtnOnClickDumpDomListener(static_cast<UIView*>(dumpButton1), "dump_image"));
        }
        dumpButton1->SetOnClickListener(clickDumpDomListener6_);
    }
}

void UITestDumpDomTree::UIKit_TestDumpDom_003()
{
    if (container_ != nullptr) {
        UIViewGroup* group5 = new UIViewGroup();
        group5->SetPosition(0, 260); // 260: y-coordinate
        group5->SetWidth(Screen::GetInstance().GetWidth() / 2); // 2: half of screen width
        group5->SetHeight(200); // 200: height
        group5->SetViewId("dump_group5");
        container_->Add(group5);
        positionY_ += group5->GetHeight();

        UILabel* label = GetTitleLabel("dump slider progress");
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE);
        group5->Add(label);

        UISlider* slider = new UISlider();
        // 50: new width, 100: new height
        slider->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE, 50, 100);
        slider->SetValidHeight(250); // 250: valid height
        slider->SetDirection(UISlider::Direction::DIR_BOTTOM_TO_TOP);
        slider->SetRange(100, 0); // 100: max
        slider->SetValue(50);     // 50: value of progress bar
        slider->SetStep(0);
        slider->SetBackgroundStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0, 182, 191).full); // 182, 191: color
        slider->SetForegroundStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(255, 152, 0).full); // 255, 152: color
        slider->SetKnobWidth(50); // 50: knob width
        slider->SetKnobStyle(StyleDefault::GetSliderKnobStyle());
        slider->SetImage(static_cast<ImageInfo*>(nullptr), static_cast<ImageInfo*>(nullptr));
        slider->EnableBackground(true);
        slider->SetViewId("dump_slider");
        group5->Add(slider);

        UILabelButton* dumpButton = GetLabelButton("dump");
        dumpButton->SetPosition(120, 73); // 120: x-coordinate, 73: y-coordinate
        /* dump node here */
        if (clickDumpDomListener7_ == nullptr) {
            clickDumpDomListener7_ = static_cast<UIView::OnClickListener*>(
                new TestBtnOnClickDumpDomListener(static_cast<UIView*>(dumpButton), "dump_slider"));
        }
        dumpButton->SetOnClickListener(clickDumpDomListener7_);
        group5->Add(dumpButton);
    }
}

void UITestDumpDomTree::UIKit_TestDumpDom_004()
{
    if (container_ != nullptr) {
        UIViewGroup* group4 = new UIViewGroup();
        group4->SetPosition(Screen::GetInstance().GetWidth() / 2, 260); // 2: half of screen width; 260: y-coordinate
        group4->SetWidth(Screen::GetInstance().GetWidth() / 2); // 2: half of screen width
        group4->SetHeight(100); // 100: height
        group4->SetViewId("dump_group4");
        container_->Add(group4);
        positionY_ += group4->GetHeight();

        UILabel* label = GetTitleLabel("dump box progress");
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE);
        group4->Add(label);

        UIBoxProgress* boxProgress = new UIBoxProgress();
        // 48: x-coordinate, 68: y-coordinate, 90: new width, 10: new height
        boxProgress->SetPosition(48, 68, 90, 10);
        boxProgress->SetDirection(UIBoxProgress::Direction::DIR_LEFT_TO_RIGHT);
        boxProgress->SetValidHeight(10); // 10: valid height
        boxProgress->SetValidWidth(100); // 100: valid width
        boxProgress->SetRange(100, 0); // 100: max
        boxProgress->SetValue(20); // 20 : value of progress bar
        boxProgress->SetStep(0);
        // 182, 191: color
        boxProgress->SetBackgroundStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0, 182, 191).full);
        // 255, 152: color
        boxProgress->SetForegroundStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(255, 152, 0).full);
        boxProgress->SetImage(static_cast<char*>(nullptr));
        boxProgress->EnableBackground(true);
        boxProgress->SetViewId("dump_boxprogress");
        group4->Add(boxProgress);
        UILabelButton* dumpButton = GetLabelButton("dump");
        dumpButton->SetPosition(160, 48); // 160: x-coordinate, 48: y-coordinate
        /* dump node here */
        if (clickDumpDomListener8_ == nullptr) {
            clickDumpDomListener8_ = static_cast<UIView::OnClickListener*>(
                new TestBtnOnClickDumpDomListener(static_cast<UIView*>(dumpButton), "dump_boxprogress"));
        }
        dumpButton->SetOnClickListener(clickDumpDomListener8_);
        group4->Add(dumpButton);
    }
}

void UITestDumpDomTree::UIKit_TestDumpDom_005()
{
    if (container_ != nullptr) {
        UIViewGroup* group3 = new UIViewGroup();
        group3->SetPosition(0, 450); // 450: y-coordinate
        group3->SetWidth(Screen::GetInstance().GetWidth() / 2 - VIEW_DISTANCE_TO_LEFT_SIDE); // 2: half of screen width
        group3->SetHeight(500); // 500: height
        group3->SetViewId("dump_group3");
        container_->Add(group3);

        UILabel* label = GetTitleLabel("dump label");
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE);
        group3->Add(label);

        UILabel* label2 = GetTitleLabel("UILabel");
        label2->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
        label2->SetPosition(48, 58, 90, 29); // 48: x position, 58: y position, 90: width, 29: height
        label2->SetViewId("dump_label");
        group3->Add(label2);

        UILabelButton* labelButton = GetLabelButton("LabelButton");
        labelButton->SetPosition(48, 108); // 48: x position, 108: y position
        labelButton->SetViewId("dump_label_button");
        group3->Add(labelButton);

        UIArcLabel* label3 = new UIArcLabel();
        label3->SetArcTextCenter(70, 330); // 70: x position, 330: y position
        label3->SetArcTextRadius(150);     // 150: radius
        label3->SetArcTextAngle(0, 270);   // 0: start angle, 270: end angle
        label3->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        label3->SetText("012345678ABCDEF");
        label3->SetStyle(STYLE_LETTER_SPACE, 30); // 30: letter space
        label3->SetViewId("dump_arc_label");
        group3->Add(label3);

        UILabelButton* dumpButton1 = GetLabelButton("dump");
        dumpButton1->SetPosition(230, 48); // 230: x-coordinate, 48: y-coordinate
        /* dump node here */
        if (clickDumpDomListener9_ == nullptr) {
            clickDumpDomListener9_ = static_cast<UIView::OnClickListener*>(
                new TestBtnOnClickDumpDomListener(static_cast<UIView*>(dumpButton1), "dump_label"));
        }
        dumpButton1->SetOnClickListener(clickDumpDomListener9_);
        group3->Add(dumpButton1);

        UILabelButton* dumpButton2 = GetLabelButton("dump");
        dumpButton2->SetPosition(230, 108); // 230: x-coordinate, 108: y-coordinate
        /* dump node here */
        if (clickDumpDomListener10_ == nullptr) {
            clickDumpDomListener10_ = static_cast<UIView::OnClickListener*>(
                new TestBtnOnClickDumpDomListener(static_cast<UIView*>(dumpButton2), "dump_label_button"));
        }
        dumpButton2->SetOnClickListener(clickDumpDomListener10_);
        group3->Add(dumpButton2);

        UILabelButton* dumpButton3 = GetLabelButton("dump");
        dumpButton3->SetPosition(230, 310); // 230: x position, 310: y position
        /* dump node here */
        if (clickDumpDomListener11_ == nullptr) {
            clickDumpDomListener11_ = static_cast<UIView::OnClickListener*>(
                new TestBtnOnClickDumpDomListener(static_cast<UIView*>(dumpButton3), "dump_arc_label"));
        }
        dumpButton3->SetOnClickListener(clickDumpDomListener11_);
        group3->Add(dumpButton3);
    }
}

void UITestDumpDomTree::UIKit_TestDumpDom_006()
{
    if (container_ != nullptr) {
        UIViewGroup* group6 = new UIViewGroup();
        // 2: half of screen width; 450: screen height
        group6->SetPosition(Screen::GetInstance().GetWidth() / 2, 450);
        group6->SetWidth(Screen::GetInstance().GetWidth() / 2); // 2: half of screen width
        group6->SetHeight(250); // 250: height
        group6->SetViewId("dump_group6");
        container_->Add(group6);
        positionY_ += group6->GetHeight();

        UILabel* label = GetTitleLabel("dump circle progress");
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE);
        group6->Add(label);

        UICircleProgress* circleProgress = new UICircleProgress();
        // 40: y-coordinate, 48: y-coordinate, 150: new width, 150: new height
        circleProgress->SetPosition(40, 48, 150, 150);
        circleProgress->SetCenterPosition(50, 50); // 50: x-coordinate, 50: y-coordinate
        circleProgress->SetRadius(50); // 50: radius
        circleProgress->SetValue(20); // 20: value of progress bar
        circleProgress->SetStyle(STYLE_PADDING_LEFT, 10); // 10: padding left
        circleProgress->SetStyle(STYLE_PADDING_TOP, 5); // 5: padding top
        circleProgress->SetRange(100, 0); // 100: max
        circleProgress->SetValue(50); // 50: value of progress bar
        circleProgress->SetStartAngle(0);
        circleProgress->SetEndAngle(360); // 360: end angle
        circleProgress->SetStep(0);
        circleProgress->SetBackgroundStyle(StyleDefault::GetProgressBackgroundStyle());
        circleProgress->SetForegroundStyle(StyleDefault::GetProgressForegroundStyle());
        circleProgress->SetLineColor(Color::White());
        circleProgress->EnableBackground(true);
        circleProgress->SetViewId("dump_circle_progress");
        group6->Add(circleProgress);

        UILabelButton* dumpButton = GetLabelButton("dump");
        dumpButton->SetPosition(170, 73); // 170: x-coordinate, 73: y-coordinate
        /* dump node here */
        if (clickDumpDomListener12_ == nullptr) {
            clickDumpDomListener12_ = static_cast<UIView::OnClickListener*>(
                new TestBtnOnClickDumpDomListener(static_cast<UIView*>(dumpButton), "dump_circle_progress"));
        }
        dumpButton->SetOnClickListener(clickDumpDomListener12_);
        group6->Add(dumpButton);
    }
}

UILabelButton* UITestDumpDomTree::GetLabelButton(const char* buttonname)
{
    const int16_t labelButtonWidth = 120;
    const int16_t labelButtonHeight = 40;
    if (buttonname == nullptr) {
        return nullptr;
    }
    UILabelButton* labelButton = new UILabelButton();
    if (labelButton == nullptr) {
        return nullptr;
    }
    labelButton->SetPosition(0, 0);
    labelButton->Resize(labelButtonWidth, labelButtonHeight);
    labelButton->SetText(buttonname);
    labelButton->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    labelButton->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    labelButton->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    labelButton->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    labelButton->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    labelButton->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    labelButton->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    return labelButton;
}
} // namespace OHOS
#endif // ENABLE_DEBUG
