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

#include "ui_test_render.h"

#include "common/screen.h"

namespace OHOS {
void UITestRender::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
    }
}

void UITestRender::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestRender::GetTestView()
{
    UIKit_Render_Test_Render_001();
    UIKit_Render_Test_RenderMeasure_001();
    return container_;
}

class TestRenderClickListener : public UIView::OnClickListener {
public:
    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        static uint8_t r = 20;
        static uint8_t g = 120;
        static uint8_t b = 50;
        view.SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(r, g, b).full);
        view.Invalidate();

        r = r + 30; // 30: red add
        g = g + 30; // 30: green add
        b = b + 30; // 30: blue add
        return true;
    }
};

UIViewGroup* UITestRender::CreateTestCaseGroup() const
{
    UIViewGroup* group = new UIViewGroup();
    group->Resize(Screen::GetInstance().GetWidth(), 200); // 200: height
    return group;
}

UILabel* UITestRender::CreateTitleLabel() const
{
    UILabel* label = new UILabel();
    // 216: label width
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, 216, TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    return label;
}

void UITestRender::UIKit_Render_Test_Render_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_Render_Test_Render_001");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    label->SetText("UIKit绘制效果:");

    UIView* view = new UIView();
    view->Resize(200, 50);     // 200: width 50: height
    view->SetPosition(50, 50); // 50: position x 50: position y
    view->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    group->Add(view);
    view->SetTouchable(true);
    view->SetOnClickListener(new TestRenderClickListener());

    UIView* view1 = new UIView();
    view1->Resize(100, 50);      // 100: width 50: height
    view1->SetPosition(50, 120); // 50: position x 120: position y
    view1->SetStyle(STYLE_BACKGROUND_COLOR, Color::Green().full);
    view1->SetTouchable(true);
    view1->SetOnClickListener(new TestRenderClickListener());
    group->Add(view1);
    container_->Add(group);
}

void UITestRender::UIKit_Render_Test_RenderMeasure_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_Render_Test_RenderMeasure_001");
    container_->Add(group);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    // 2: half of screen width
    label->Resize(Screen::GetInstance().GetWidth() / 2, TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetText("UIKit绘制Measure效果:");

    testLabel_ = new UILabel();
    group->Add(testLabel_);
    testLabel_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    testLabel_->Resize(150, 80); // 150: width 80: height
    testLabel_->SetViewId("label_text");
    testLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    testLabel_->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    testLabel_->SetText("示例文字,示例文字");

    labelButton_ = new UILabelButton();
    group->Add(labelButton_);
    labelButton_->Resize(BUTTON_WIDHT3, BUTTON_HEIGHT3);
    labelButton_->SetText("更新label");
    labelButton_->SetViewId(UI_TEST_RENDER_UPDATA_BUTTON_ID_01);
    labelButton_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    labelButton_->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    labelButton_->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    labelButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    labelButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_PRESS, UIButton::PRESSED);
    labelButton_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    labelButton_->SetOnClickListener(this);
    labelButton_->LayoutBottomToSibling("label_text"); // 10: offset

    group->LayoutBottomToSibling("UIKit_Render_Test_Render_001", 10); // 10: offset
}

bool UITestRender::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == labelButton_) {
        testLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 15); // 18: font size
    }
    return true;
}
} // namespace OHOS