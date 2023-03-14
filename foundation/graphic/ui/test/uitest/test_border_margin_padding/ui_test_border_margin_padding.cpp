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

#include "ui_test_border_margin_padding.h"
#include "common/screen.h"
#include "components/text_adapter.h"
#include "components/ui_arc_label.h"
#include "components/ui_box_progress.h"
#include "components/ui_canvas.h"
#include "components/ui_circle_progress.h"
#include "components/ui_image_animator.h"
#include "components/ui_label_button.h"
#include "components/ui_list.h"
#include "components/ui_qrcode.h"
#include "components/ui_radio_button.h"
#include "components/ui_slider.h"
#include "components/ui_swipe_view.h"
#include "test_resource_config.h"

namespace OHOS {
static ImageAnimatorInfo g_imageAnimatorInfo[4] = {
    {IMAGE_ANIMATOR_0_PATH, {84, 108}, 116, 116, IMG_SRC_FILE_PATH},
    {IMAGE_ANIMATOR_1_PATH, {84, 108}, 116, 116, IMG_SRC_FILE_PATH},
    {IMAGE_ANIMATOR_2_PATH, {84, 108}, 116, 116, IMG_SRC_FILE_PATH},
    {IMAGE_ANIMATOR_3_PATH, {84, 108}, 116, 116, IMG_SRC_FILE_PATH},
};

class MarginListener : public UICheckBox::OnChangeListener {
public:
    explicit MarginListener(UITestBorderMarginPadding* view)
    {
        view_ = view;
    }

    virtual ~MarginListener() {}

    bool OnChange(UICheckBox::UICheckBoxState state) override
    {
        if (state == UICheckBox::SELECTED) {
            view_->style_.SetStyle(STYLE_MARGIN_LEFT, MARGIN_SIZE);
            view_->style_.SetStyle(STYLE_MARGIN_TOP, MARGIN_SIZE);
            view_->style_.SetStyle(STYLE_MARGIN_RIGHT, MARGIN_SIZE);
            view_->style_.SetStyle(STYLE_MARGIN_BOTTOM, MARGIN_SIZE);
        } else {
            view_->style_.SetStyle(STYLE_MARGIN_LEFT, 0);
            view_->style_.SetStyle(STYLE_MARGIN_TOP, 0);
            view_->style_.SetStyle(STYLE_MARGIN_RIGHT, 0);
            view_->style_.SetStyle(STYLE_MARGIN_BOTTOM, 0);
        }
        view_->ReloadTest();
        return true;
    }

private:
    static constexpr int16_t MARGIN_SIZE = 30;
    UITestBorderMarginPadding* view_ = nullptr;
};

class BorderListener : public UICheckBox::OnChangeListener {
public:
    explicit BorderListener(UITestBorderMarginPadding* view)
    {
        view_ = view;
    }

    virtual ~BorderListener() {}

    bool OnChange(UICheckBox::UICheckBoxState state) override
    {
        if (state == UICheckBox::SELECTED) {
            view_->style_.SetStyle(STYLE_BORDER_WIDTH, 20); // 20: border width
            view_->style_.SetStyle(STYLE_BORDER_OPA, OPA_OPAQUE);
            view_->style_.SetStyle(STYLE_BORDER_COLOR, Color::Blue().full);
        } else {
            view_->style_.SetStyle(STYLE_BORDER_WIDTH, 0);
        }
        view_->ReloadTest();
        return true;
    }

private:
    UITestBorderMarginPadding* view_ = nullptr;
};

class PaddingListener : public UICheckBox::OnChangeListener {
public:
    explicit PaddingListener(UITestBorderMarginPadding* view)
    {
        view_ = view;
    }

    virtual ~PaddingListener() {}

    bool OnChange(UICheckBox::UICheckBoxState state) override
    {
        if (state == UICheckBox::SELECTED) {
            view_->style_.SetStyle(STYLE_PADDING_LEFT, PADDING_SIZE);
            view_->style_.SetStyle(STYLE_PADDING_TOP, PADDING_SIZE);
            view_->style_.SetStyle(STYLE_PADDING_RIGHT, PADDING_SIZE);
            view_->style_.SetStyle(STYLE_PADDING_BOTTOM, PADDING_SIZE);
        } else {
            view_->style_.SetStyle(STYLE_PADDING_LEFT, 0);
            view_->style_.SetStyle(STYLE_PADDING_TOP, 0);
            view_->style_.SetStyle(STYLE_PADDING_RIGHT, 0);
            view_->style_.SetStyle(STYLE_PADDING_BOTTOM, 0);
        }
        view_->ReloadTest();
        return true;
    }

private:
    static constexpr int16_t PADDING_SIZE = 30;
    UITestBorderMarginPadding* view_ = nullptr;
};

void UITestBorderMarginPadding::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIViewGroup();
    }
    if (scroll_ == nullptr) {
        scroll_ = new UIScrollView();
    }
    if (layoutButton_ == nullptr) {
        layoutButton_ = new GridLayout();
    }
    if (listScroll_ == nullptr) {
        listScroll_ = new ListLayout();
    }

    UIToggleButton* marginBtn = new UIToggleButton();
    UIToggleButton* borderBtn = new UIToggleButton();
    UIToggleButton* paddingBtn = new UIToggleButton();
    UILabel* labelMargin = new UILabel();
    UILabel* labelBorder = new UILabel();
    UILabel* labelPadding = new UILabel();

    container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(),
                            Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);

    scroll_->SetPosition(0, 0, Screen::GetInstance().GetWidth() - BUTTON_GROUP_WIDTH,
                         Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
    scroll_->SetThrowDrag(true);
    scroll_->SetHorizontalScrollState(false);
    scroll_->SetScrollBlankSize(100); // 100 ： blank size

    layoutButton_->SetPosition(Screen::GetInstance().GetWidth() - BUTTON_GROUP_WIDTH, 0, BUTTON_GROUP_WIDTH,
                               Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
    layoutButton_->SetRows(3); // 3: rows
    layoutButton_->SetCols(2); // 2: cols

    listScroll_ = new ListLayout(ListLayout::VERTICAL);
    listScroll_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    listScroll_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Olive().full);
    listScroll_->SetPosition(0, 0, Screen::GetInstance().GetWidth() - BUTTON_GROUP_WIDTH,
                             Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
    scroll_->Add(listScroll_);

    container_->Add(scroll_);
    container_->Add(layoutButton_);
    marginBtn->Resize(64, 64);  // 64 : button size
    borderBtn->Resize(64, 64);  // 64 : button size
    paddingBtn->Resize(64, 64); // 64 : button size

    labelMargin->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    labelMargin->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 15); // 15: font size
    labelMargin->SetText("margin ON/OFF");
    labelMargin->ReMeasure();

    labelBorder->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    labelBorder->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 15); // 15: font size
    labelBorder->SetText("border ON/OFF");
    labelBorder->ReMeasure();

    labelPadding->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    labelPadding->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 15); // 15: font size
    labelPadding->SetText("padding ON/OFF");
    labelPadding->ReMeasure();

    layoutButton_->Add(labelMargin);
    layoutButton_->Add(marginBtn);
    layoutButton_->Add(labelBorder);
    layoutButton_->Add(borderBtn);
    layoutButton_->Add(labelPadding);
    layoutButton_->Add(paddingBtn);
    layoutButton_->LayoutChildren();

    if (borderListener_ == nullptr) {
        borderListener_ = new BorderListener(this);
    }
    borderBtn->SetOnChangeListener(borderListener_);
    if (marginListener_ == nullptr) {
        marginListener_ = new MarginListener(this);
    }
    marginBtn->SetOnChangeListener(marginListener_);
    if (paddingListener_ == nullptr) {
        paddingListener_ = new PaddingListener(this);
    }
    paddingBtn->SetOnChangeListener(paddingListener_);

    if (adapterData_ == nullptr) {
        adapterData_ = new List<const char*>();
        adapterData_->PushBack("000");
        adapterData_->PushBack("111");
        adapterData_->PushBack("222");
        adapterData_->PushBack("333");
        adapterData_->PushBack("444");
        adapterData_->PushBack("555");
        adapterData_->PushBack("666");
        adapterData_->PushBack("777");
        adapterData_->PushBack("888");
        adapterData_->PushBack("999");
        adapterData_->PushBack("AAA");
        adapterData_->PushBack("BBB");
        adapterData_->PushBack("CCC");
    }
}

void UITestBorderMarginPadding::TearDown()
{
    if (borderListener_ != nullptr) {
        delete borderListener_;
        borderListener_ = nullptr;
    }
    if (marginListener_ != nullptr) {
        delete marginListener_;
        marginListener_ = nullptr;
    }
    if (paddingListener_ != nullptr) {
        delete paddingListener_;
        paddingListener_ = nullptr;
    }
    if (adapterData_ != nullptr) {
        adapterData_->Clear();
        delete adapterData_;
        adapterData_ = nullptr;
    }
    if (picker_ != nullptr) {
        listScroll_->Remove(picker_);
        delete picker_;
        picker_ = nullptr;
    }
    if (chart_ != nullptr) {
        listScroll_->Remove(chart_);
        chart_->ClearDataSerial();
        delete dataSerial_;
        dataSerial_ = nullptr;
        delete chart_;
        chart_ = nullptr;
    }
    DeleteChildren(container_);
    container_ = nullptr;
    scroll_ = nullptr;
    layoutButton_ = nullptr;
    listScroll_ = nullptr;
}

void UITestBorderMarginPadding::ReloadTest()
{
    int16_t heightBefor = listScroll_->GetHeight();
    int16_t yBefor = listScroll_->GetY();

    scroll_->RemoveAll();
    if (chart_ != nullptr) {
        listScroll_->Remove(chart_);
        chart_->ClearDataSerial();
        delete dataSerial_;
        dataSerial_ = nullptr;
        delete chart_;
        chart_ = nullptr;
    }

    if (picker_ != nullptr) {
        listScroll_->Remove(picker_);
        delete picker_;
        picker_ = nullptr;
    }

    DeleteChildren(listScroll_);
    listScroll_ = new ListLayout(ListLayout::VERTICAL);
    listScroll_->SetPosition(0, 0, Screen::GetInstance().GetWidth() - BUTTON_GROUP_WIDTH,
                             Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
    listScroll_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    listScroll_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Olive().full);
    scroll_->Add(listScroll_);
    GetTestView();

    int16_t yAfter = listScroll_->GetHeight() * yBefor / heightBefor;
    scroll_->ScrollBy(0, yAfter);
    container_->Invalidate();
}

const UIView* UITestBorderMarginPadding::GetTestView()
{
    UIKit_UITestBorderMarginPadding_Test_001();
    UIKit_UITestBorderMarginPadding_Test_002();
    UIKit_UITestBorderMarginPadding_Test_003();
    UIKit_UITestBorderMarginPadding_Test_004();
    UIKit_UITestBorderMarginPadding_Test_005();
    UIKit_UITestBorderMarginPadding_Test_006();
    UIKit_UITestBorderMarginPadding_Test_007();
    UIKit_UITestBorderMarginPadding_Test_008();
    UIKit_UITestBorderMarginPadding_Test_009();
    UIKit_UITestBorderMarginPadding_Test_011();
    UIKit_UITestBorderMarginPadding_Test_012();
    UIKit_UITestBorderMarginPadding_Test_013();
    UIKit_UITestBorderMarginPadding_Test_014();
    UIKit_UITestBorderMarginPadding_Test_015();
    UIKit_UITestBorderMarginPadding_Test_016();
    UIKit_UITestBorderMarginPadding_Test_017();
    UIKit_UITestBorderMarginPadding_Test_018();
    return container_;
}

void UITestBorderMarginPadding::AddTitle(const char* text)
{
    UILabel* titleLabel = new UILabel();
    titleLabel->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    titleLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    titleLabel->SetText(text);
    listScroll_->Add(titleLabel);
}

void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_001()
{
    AddTitle("UIView");
    UIView* view = new UIView();
    view->SetStyle(style_);
    view->Resize(200, 100); // 200:size 100: size
    listScroll_->Add(view);
}

void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_002()
{
    AddTitle("UILabel");
    UILabel* view = new UILabel();
    view->SetStyle(style_);
    view->SetLineBreakMode(UILabel::LINE_BREAK_CLIP);
    view->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    view->Resize(200, 100); // 200:size 100: size
    view->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    view->SetText("TEST UILABEL");
    listScroll_->Add(view);
}

void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_003()
{
    AddTitle("ArcLabel");
    UIArcLabel* view = new UIArcLabel();
    view->SetStyle(style_);
    view->SetArcTextCenter(180, 180);                // 180: text center
    view->SetArcTextRadius(150);                     // 150: text radius
    view->SetArcTextAngle(0, 270);                   // 0: start angle 270: end angle
    view->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
    view->SetText("012345678ABCDEF0123456789ABCDE");
    listScroll_->Add(view);
}

void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_004()
{
    AddTitle("BoxProgress");
    UIBoxProgress* boxProgress = new UIBoxProgress();
    boxProgress->SetStyle(style_);
    boxProgress->SetPosition(10, 10, 100, 10); // 10: x 10: y 100: width 10: height
    boxProgress->SetValue(20);                 // 20: value
    boxProgress->SetBackgroundStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    boxProgress->SetForegroundStyle(STYLE_BACKGROUND_COLOR, Color::Green().full);
    listScroll_->Add(boxProgress);
}

void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_005()
{
    AddTitle("UIViewGroup");
    UIViewGroup* group = new UIViewGroup();
    group->SetStyle(style_);
    group->Resize(200, 100); // 200: size 100: size
    listScroll_->Add(group);

    UIButton* button = new UIButton();
    button->SetPosition(0, 0, 100, 50); // 100: size 50: size
    group->Add(button);
}

void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_006()
{
    AddTitle("UICanvas");
    UICanvas* canvas = new UICanvas();
    canvas->SetStyle(style_);
    canvas->Resize(400, 400); // 400: size
    Paint paint;
    paint.SetStrokeWidth(10); // 10: line width
    // {10, 10}: Start point coordinates x, y; {300, 10}: end point coordinates x, y
    canvas->DrawLine({10, 10}, {300, 10}, paint);

    paint.SetStrokeColor(Color::Yellow());
    canvas->DrawCurve({100, 50}, {150, 50}, {150, 50}, {150, 100}, paint);

    paint.SetStyle(Paint::PaintStyle::FILL_STYLE);
    paint.SetFillColor(Color::Yellow());
    paint.SetStrokeWidth(30); // 30: line width
    // {10, 200}: left corner coordinates point, 50: width, 50: rectangle style
    canvas->DrawRect({10, 200}, 50, 50, paint);

    paint.SetStyle(Paint::PaintStyle::STROKE_FILL_STYLE);
    paint.SetFillColor(Color::Yellow());
    paint.SetStrokeColor(Color::Blue());
    paint.SetStrokeWidth(10); // 10: line width
    paint.SetOpacity(127);    // 127: opacity
    // {300, 100}: circle center coordinates, 30: circle radius
    canvas->DrawCircle({300, 100}, 30, paint);
    listScroll_->Add(canvas);
}

void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_007()
{
    AddTitle("UIChart");
    chart_ = new UIChartPolyline();
    chart_->SetStyle(style_);
    chart_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    chart_->SetWidth(454);  // 454: width
    chart_->SetHeight(250); // 250: height

    dataSerial_ = new UIChartDataSerial();
    dataSerial_->SetMaxDataCount(5); // 5: number of data points
    Point pointArray[5] = {{0, 2478}, {1, 2600}, {2, 3000}, {3, 3200}, {4, 3500}};
    dataSerial_->AddPoints(pointArray, 5); // 5: number of data points
    dataSerial_->SetLineColor(Color::Red());
    dataSerial_->SetFillColor(Color::Red());
    dataSerial_->EnableGradient(true);

    UIXAxis& xAxis = chart_->GetXAxis();
    UIYAxis& yAxis = chart_->GetYAxis();
    xAxis.SetMarkNum(5);         // 5: number of scales
    xAxis.SetDataRange(0, 5);    // 0: minimum value, 5: maximum value
    yAxis.SetDataRange(0, 5000); // 0: minimum value, 5000: maximum value

    chart_->SetGradientOpacity(25, 127); // 25: min opacity, 127: max opacity
    chart_->AddDataSerial(dataSerial_);

    listScroll_->Add(chart_);
}
void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_008()
{
    AddTitle("Button");
    UIViewGroup* group = new UIViewGroup();
    group->Resize(Screen::GetInstance().GetWidth() - BUTTON_GROUP_WIDTH, 200); // 200: button UIViewGroup height

    UIButton* button = new UIButton();
    button->SetStyleForState(STYLE_MARGIN_LEFT, style_.marginLeft_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_MARGIN_LEFT, style_.marginLeft_, UIButton::PRESSED);
    button->SetStyleForState(STYLE_MARGIN_TOP, style_.marginTop_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_MARGIN_TOP, style_.marginTop_, UIButton::PRESSED);
    button->SetStyleForState(STYLE_MARGIN_RIGHT, style_.marginRight_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_MARGIN_RIGHT, style_.marginRight_, UIButton::PRESSED);
    button->SetStyleForState(STYLE_MARGIN_BOTTOM, style_.marginBottom_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_MARGIN_BOTTOM, style_.marginBottom_, UIButton::PRESSED);

    button->SetStyleForState(STYLE_BORDER_WIDTH, style_.borderWidth_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_BORDER_WIDTH, style_.borderWidth_, UIButton::PRESSED);
    button->SetStyleForState(STYLE_BORDER_OPA, style_.borderOpa_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_BORDER_OPA, style_.borderOpa_, UIButton::PRESSED);
    button->SetStyleForState(STYLE_BORDER_COLOR, style_.borderColor_.full, UIButton::RELEASED);
    button->SetStyleForState(STYLE_BORDER_COLOR, style_.borderColor_.full, UIButton::PRESSED);

    button->SetStyleForState(STYLE_PADDING_LEFT, style_.paddingLeft_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_PADDING_LEFT, style_.paddingLeft_, UIButton::PRESSED);
    button->SetStyleForState(STYLE_PADDING_TOP, style_.paddingTop_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_PADDING_TOP, style_.paddingTop_, UIButton::PRESSED);
    button->SetStyleForState(STYLE_PADDING_RIGHT, style_.paddingRight_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_PADDING_RIGHT, style_.paddingRight_, UIButton::PRESSED);
    button->SetStyleForState(STYLE_PADDING_BOTTOM, style_.paddingBottom_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_PADDING_BOTTOM, style_.paddingBottom_, UIButton::PRESSED);

    button->SetPosition(50, 30, 70, 50); // 50: x 30: y 70: width 50: height
    group->Add(button);

    UICheckBox* checkbox = new UICheckBox();
    checkbox->SetStyle(style_);
    checkbox->SetPosition(200, 30, 50, 50); // 200: x-coordinate, 30: y-coordinate, 50: size
    group->Add(checkbox);

    UIRadioButton* radioButton = new UIRadioButton();
    radioButton->SetStyle(style_);
    radioButton->SetPosition(350, 30, 50, 50); // 350: x-coordinate, 30: y-coordinate, 50: size
    group->Add(radioButton);

    UIToggleButton* toggleButton = new UIToggleButton();
    toggleButton->SetStyle(style_);
    toggleButton->SetPosition(500, 30, 50, 50); // 500: x-coordinate, 30: y-coordinate, 50: size
    group->Add(toggleButton);

    listScroll_->Add(group);
}

void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_009()
{
    AddTitle("UICircleProgress");
    UICircleProgress* circleProgress = new UICircleProgress();
    circleProgress->SetStyle(style_);
    circleProgress->Resize(150, 150);          // 150: width 150: height
    circleProgress->SetCenterPosition(75, 75); // 75: position x 75: position y
    circleProgress->SetRadius(50);             // 50: radius
    circleProgress->SetValue(20);              // 20: value
    listScroll_->Add(circleProgress);
}

void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_010()
{
    AddTitle("UIImageAnimatorView");
    UIImageAnimatorView* imageAnimator = new UIImageAnimatorView();
    imageAnimator->SetStyle(style_);
    imageAnimator->SetPosition(50, 50, 200, 200);                    // 50 : offset 50 : offset 200 : offset 200: offset
    imageAnimator->SetImageAnimatorSrc(g_imageAnimatorInfo, 4, 100); // 4: the number of images, 100: updating time
    imageAnimator->Start();
    listScroll_->Add(imageAnimator);
}

void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_011()
{
    AddTitle("UIImageView");
    UIImageView* imageView = new UIImageView();
    imageView->SetStyle(style_);
    imageView->SetWidth(50);  // 50 : size
    imageView->SetHeight(50); // 50 : size
    imageView->SetSrc(BLUE_RGB888_IMAGE_PATH);
    listScroll_->Add(imageView);
}

void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_012()
{
    AddTitle("UILabelButton");
    UILabelButton* button = new UILabelButton();
    button->SetStyleForState(STYLE_MARGIN_LEFT, style_.marginLeft_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_MARGIN_LEFT, style_.marginLeft_, UIButton::PRESSED);
    button->SetStyleForState(STYLE_MARGIN_TOP, style_.marginTop_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_MARGIN_TOP, style_.marginTop_, UIButton::PRESSED);
    button->SetStyleForState(STYLE_MARGIN_RIGHT, style_.marginRight_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_MARGIN_RIGHT, style_.marginRight_, UIButton::PRESSED);
    button->SetStyleForState(STYLE_MARGIN_BOTTOM, style_.marginBottom_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_MARGIN_BOTTOM, style_.marginBottom_, UIButton::PRESSED);

    button->SetStyleForState(STYLE_BORDER_WIDTH, style_.borderWidth_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_BORDER_WIDTH, style_.borderWidth_, UIButton::PRESSED);
    button->SetStyleForState(STYLE_BORDER_OPA, style_.borderOpa_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_BORDER_OPA, style_.borderOpa_, UIButton::PRESSED);
    button->SetStyleForState(STYLE_BORDER_COLOR, style_.borderColor_.full, UIButton::RELEASED);
    button->SetStyleForState(STYLE_BORDER_COLOR, style_.borderColor_.full, UIButton::PRESSED);

    button->SetStyleForState(STYLE_PADDING_LEFT, style_.paddingLeft_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_PADDING_LEFT, style_.paddingLeft_, UIButton::PRESSED);
    button->SetStyleForState(STYLE_PADDING_TOP, style_.paddingTop_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_PADDING_TOP, style_.paddingTop_, UIButton::PRESSED);
    button->SetStyleForState(STYLE_PADDING_RIGHT, style_.paddingRight_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_PADDING_RIGHT, style_.paddingRight_, UIButton::PRESSED);
    button->SetStyleForState(STYLE_PADDING_BOTTOM, style_.paddingBottom_, UIButton::RELEASED);
    button->SetStyleForState(STYLE_PADDING_BOTTOM, style_.paddingBottom_, UIButton::PRESSED);
    button->SetWidth(200); // 200 : size
    button->SetHeight(50); // 50 : size
    button->SetText("Test UILabelButton");
    listScroll_->Add(button);
}

void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_013()
{
    AddTitle("UIList");
    if (adapter_ == nullptr) {
        adapter_ = new TextAdapter();
    }
    adapter_->SetData(adapterData_);
    adapter_->GetStyle() = style_;

    UIList* list = new UIList(UIList::VERTICAL);
    list->SetIntercept(true);
    list->SetStyle(style_);
    list->Resize(300, 400); // 300 : size, 400 : size
    list->SetAdapter(adapter_);
    listScroll_->Add(list);
}

void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_014()
{
    AddTitle("UIPicker");
    picker_ = new UIPicker();
    picker_->SetIntercept(true);
    picker_->SetStyle(style_);
    picker_->SetFontId(16, 18); // 16:back font id 18:high light font id
    picker_->SetItemHeight(50); // 50: height
    picker_->SetTextColor(Color::White(), Color::Red());
    picker_->SetValues(-5, 20); // -5: start 20:end
    picker_->Resize(300, 400);  // 300: picker size 400: picker size
    listScroll_->Add(picker_);
}

void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_015()
{
    AddTitle("UIQrcode");
    UIQrcode* qrcode = new UIQrcode();
    qrcode->SetStyle(style_);
    qrcode->Resize(60, 60); // 60: height
    qrcode->SetQrcodeInfo("Hello\n Test of GUI");
    listScroll_->Add(qrcode);
}

void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_016()
{
    AddTitle("UIScroll");
    UIScrollView* scroll = new UIScrollView();
    scroll->SetIntercept(true);
    scroll->SetStyle(style_);
    scroll->Resize(200, 200); // 200: size

    UILabelButton* button = new UILabelButton();
    button->SetPosition(0, 0, 300, 300); // 300: size
    button->SetText("button1");
    scroll->Add(button);
    listScroll_->Add(scroll);
}
void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_017()
{
    AddTitle("UISlider");
    UISlider* slider = new UISlider();
    slider->SetIntercept(true);
    slider->SetStyle(style_);
    slider->Resize(50, 300);     // 50: width; 300: height
    slider->SetValidHeight(250); // 250: valid height;
    slider->SetValue(20);        // 20:  progress bar current value
    slider->SetDirection(UISlider::Direction::DIR_BOTTOM_TO_TOP);
    listScroll_->Add(slider);
}

void UITestBorderMarginPadding::UIKit_UITestBorderMarginPadding_Test_018()
{
    AddTitle("UISwipe");
    UISwipeView* swipe = new UISwipeView(UISwipeView::HORIZONTAL);
    swipe->SetIntercept(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    swipe->Resize(200, 200);  // 200: size
    swipe->SetBlankSize(100); // 100: is blank size
    UILabel* view1 = new UILabel();
    view1->SetStyle(style_);
    view1->SetLineBreakMode(UILabel::LINE_BREAK_CLIP);
    view1->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    view1->SetPosition(0, 0, 150, 150); // 150: size
    view1->SetText("label1");
    swipe->Add(view1);
    UILabel* view2 = new UILabel();
    view2->SetStyle(style_);
    view2->SetLineBreakMode(UILabel::LINE_BREAK_CLIP);
    view2->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    view2->SetPosition(0, 0, 150, 150); // 150: size
    view2->SetText("label2");
    swipe->Add(view2);
    UILabel* view3 = new UILabel();
    view3->SetStyle(style_);
    view3->SetLineBreakMode(UILabel::LINE_BREAK_CLIP);
    view3->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    view3->SetPosition(0, 0, 150, 150); // 150: size
    view3->SetText("label3");
    swipe->Add(view3);

    listScroll_->Add(swipe);
}
} // namespace OHOS
