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

#include "ui_test_rotate_input.h"

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "cstdio"
#include "dock/focus_manager.h"
#include "dock/vibrator_manager.h"
#include "engines/gfx/gfx_engine_manager.h"

#if ENABLE_ROTATE_INPUT
namespace OHOS {
static int16_t g_blank = 20;
static int16_t g_listW = 200;
static int16_t g_listH = 400;
static int16_t g_scrollH = 200;
static int16_t g_scrollW = 400;
static int16_t g_buttonH = 500;
static int16_t g_buttonW = 500;
static int16_t g_smallButtonH = 80;
static int16_t g_smallButtonW = 200;
static int16_t g_swipeH = 200;
static int16_t g_swipeW = 400;
static int16_t g_sliderW = 40;
static int16_t g_sliderH = 300;
static int16_t g_testButtonW = 80;
static int16_t g_testButtonH = 40;
static const char* g_pickerRange[] = {"A0", "B1", "C2", "D3", "E4", "F5", "G6", "H7", "I8", "J9", "K10", "L11"};

#if ENABLE_VIBRATOR
void Print(VibratorType vibratorType)
{
    switch (vibratorType) {
        case VibratorType::VIBRATOR_TYPE_ONE:
            printf("VIBRATOR_TYPE_ONE\n");
            break;
        case VibratorType::VIBRATOR_TYPE_TWO:
            printf("VIBRATOR_TYPE_TWO\n");
            break;
        case VibratorType::VIBRATOR_TYPE_THREE:
            printf("VIBRATOR_TYPE_THREE\n");
            break;
        default:
            break;
    }
}
#endif

class TestSetRotateFactorListener : public UIView::OnClickListener {
public:
    TestSetRotateFactorListener(UIView* view, float factor) : view_(view), factor_(factor) {}
    ~TestSetRotateFactorListener() {}

    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        switch (view_->GetViewType()) {
            case UI_SWIPE_VIEW:
                static_cast<UISwipeView*>(view_)->SetRotateFactor(factor_);
                break;
            case UI_SCROLL_VIEW:
                static_cast<UIScrollView*>(view_)->SetRotateFactor(factor_);
                break;
            default:
                break;
        }
        return true;
    }

private:
    UIView* view_ = nullptr;
    float factor_ = 0;
};

void UITestRotateInput::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetOnTouchListener(this);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        container_->SetThrowDrag(true);
        container_->SetRotateFactor(20); // 20 : rotate factor for container
        container_->RequestFocus();
        container_->SetReboundSize(50); // 50 : rebound size
    }
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
        adapterData_->PushBack("///");
        adapterData_->PushBack("***");
        adapterData_->PushBack("+++");
        adapterData_->PushBack("---");
        adapterData_->PushBack("...");
        adapterData_->PushBack("@@@");
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
        adapterData_->PushBack("///");
        adapterData_->PushBack("***");
        adapterData_->PushBack("+++");
        adapterData_->PushBack("---");
        adapterData_->PushBack("...");
        adapterData_->PushBack("@@@");
    }
#if ENABLE_VIBRATOR
    VibratorManager::GetInstance()->RegisterVibratorFunc(Print);
#endif
}

void UITestRotateInput::TearDown()
{
    if (listener1_ != nullptr) {
        delete listener1_;
        listener1_ = nullptr;
    }
    if (listener2_ != nullptr) {
        delete listener2_;
        listener2_ = nullptr;
    }
    if (listener3_ != nullptr) {
        delete listener3_;
        listener3_ = nullptr;
    }
    if (listener4_ != nullptr) {
        delete listener4_;
        listener4_ = nullptr;
    }
    if (listener5_ != nullptr) {
        delete listener5_;
        listener5_ = nullptr;
    }
    container_->ClearFocus();
    container_->Remove(timePicker_);
    container_->Remove(picker_);
    DeleteChildren(container_);
    container_ = nullptr;
    if (adapterData_ != nullptr) {
        adapterData_->Clear();
        delete adapterData_;
        adapterData_ = nullptr;
    }
    if (adapter_ != nullptr) {
        delete adapter_;
        adapter_ = nullptr;
    }
    lastX_ = 0;
    lastY_ = 0;
    list_ = nullptr;
    scroll_ = nullptr;
    swipe_ = nullptr;
    timePicker_ = nullptr;
    slider_ = nullptr;
    picker_ = nullptr;
}

const UIView* UITestRotateInput::GetTestView()
{
    UIKit_Rotate_Event_List_001();
    UIKit_Rotate_Event_Scroll_View_002();
    UIKit_Rotate_Event_Swipe_View_003();
    UIKit_Rotate_Event_Slider_004();
    UIKit_Rotate_Event_Time_Picker_005();
    UIKit_Rotate_Event_Picker_006();
    return container_;
}

void UITestRotateInput::UIKit_Rotate_Event_List_001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UIList Test");
    container_->Add(label);
    label->SetPosition(0, lastY_ + g_blank);
    if (adapter_ == nullptr) {
        adapter_ = new TextAdapter();
    }
    adapter_->SetData(adapterData_);
    if (list_ == nullptr) {
        list_ = new UIList(UIList::VERTICAL);
    }
    list_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    list_->SetPosition(0, label->GetY() + g_blank, g_listW, g_listH - 200); // 200: mean list reduce height
    list_->SetStartIndex(6);                                                // 6 : list start index
    list_->SetAdapter(adapter_);
    list_->SetOnTouchListener(this);
    list_->SetThrowDrag(true);
    list_->SetRotateFactor(10); // 10 : rotate factor
    list_->SetReboundSize(50);  // 50 : rebound size
    list_->SetIntercept(true);
    list_->SetLoopState(true);
    container_->Add(list_);
    SetLastPos(list_);
}

void UITestRotateInput::UIKit_Rotate_Event_Scroll_View_002()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UIScrollView Test");
    container_->Add(label);
    label->SetPosition(0, lastY_ + g_blank);
    if (scroll_ == nullptr) {
        scroll_ = new UIScrollView();
    }
    scroll_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    scroll_->SetPosition(0, label->GetY() + g_blank, g_scrollW, g_scrollH);
    if (Screen::GetInstance().GetScreenShape() == ScreenShape::RECTANGLE) {
        scroll_->SetXScrollBarVisible(true);
    }
    scroll_->SetYScrollBarVisible(true);
    scroll_->SetRotateFactor(10); // 10 : rotateFactor
    UIButton* btn = new UIButton();
    btn->SetPosition(0, 0, g_buttonW, g_buttonH);
    scroll_->Add(btn);
    scroll_->SetThrowDrag(true);
    scroll_->SetOnTouchListener(this);
    scroll_->SetReboundSize(50); // 50 : rebound size
    scroll_->SetIntercept(true);
    container_->Add(scroll_);
    SetTestLabelButton(label, scroll_);
    SetLastPos(scroll_);
}

void UITestRotateInput::UIKit_Rotate_Event_Swipe_View_003()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UISwipeView Test");
    container_->Add(label);
    label->SetPosition(0, lastY_ + g_blank);
    if (swipe_ == nullptr) {
        swipe_ = new UISwipeView(UISwipeView::HORIZONTAL);
    }
    swipe_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    swipe_->SetPosition(0, label->GetY() + g_blank, g_swipeW, g_swipeH);
    swipe_->SetBlankSize(100); // 100 is blank size
    UILabelButton* btn1 = new UILabelButton();
    UILabelButton* btn2 = new UILabelButton();
    UILabelButton* btn3 = new UILabelButton();
    UILabelButton* btn4 = new UILabelButton();
    UILabelButton* btn5 = new UILabelButton();
    UILabelButton* btn6 = new UILabelButton();
    UILabelButton* btn7 = new UILabelButton();
    btn1->SetPosition(0, 0, g_smallButtonW, g_smallButtonH);
    btn2->SetPosition(0, 0, g_smallButtonW, g_smallButtonH);
    btn3->SetPosition(0, 0, g_smallButtonW, g_smallButtonH);
    btn4->SetPosition(0, 0, g_smallButtonW, g_smallButtonH);
    btn5->SetPosition(0, 0, g_smallButtonW, g_smallButtonH);
    btn6->SetPosition(0, 0, g_smallButtonW, g_smallButtonH);
    btn7->SetPosition(0, 0, g_smallButtonW, g_smallButtonH);
    btn1->SetText("Button 1");
    btn2->SetText("Button 2");
    btn3->SetText("Button 3");
    btn4->SetText("Button 4");
    btn5->SetText("Button 5");
    btn6->SetText("Button 6");
    btn7->SetText("Button 7");
    swipe_->Add(btn1);
    swipe_->Add(btn2);
    swipe_->Add(btn3);
    swipe_->Add(btn4);
    swipe_->Add(btn5);
    swipe_->Add(btn6);
    swipe_->Add(btn7);
    swipe_->SetOnTouchListener(this);
    swipe_->SetRotateFactor(30); // 30 : rotate factor
    swipe_->SetIntercept(true);
    container_->Add(swipe_);
    SetTestLabelButton(label, swipe_);
    SetLastPos(swipe_);
}

void UITestRotateInput::UIKit_Rotate_Event_Slider_004()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UISlider Test");
    container_->Add(label);
    label->SetPosition(0, lastY_ + g_blank);
    if (slider_ == nullptr) {
        slider_ = new UISlider();
    }
    slider_->SetPosition(50, label->GetY() + g_blank, g_sliderW, g_sliderH); // 50: position x
    slider_->SetValidHeight(200);                                            // 200: valid height
    slider_->SetValue(0);                                                    // 0: progress bar current value
    slider_->SetDirection(UISlider::Direction::DIR_BOTTOM_TO_TOP);
    slider_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    slider_->SetOnTouchListener(this);
    container_->Add(slider_);
    SetLastPos(slider_);
}

void UITestRotateInput::UIKit_Rotate_Event_Time_Picker_005()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UITimePicker Test");
    container_->Add(label);
    label->SetPosition(0, lastY_ + g_blank);
    if (timePicker_ == nullptr) {
        timePicker_ = new UITimePicker();
    }
    timePicker_->EnableSecond(true);
    timePicker_->SetPosition(0, label->GetY() + g_blank, g_listW, g_listH);
    timePicker_->SetItemHeight(50); // 50 height
    timePicker_->SetIntercept(true);

    UIView* hour = timePicker_->GetChildById("hour");
    if (hour == nullptr) {
        return;
    }
    static_cast<UIList*>(hour)->SetRotateFactor(5); // 5: rotate factor
    hour->SetOnTouchListener(this);

    UIView* minute = timePicker_->GetChildById("minute");
    if (minute == nullptr) {
        return;
    }
    static_cast<UIList*>(minute)->SetRotateFactor(5); // 5: rotate factor
    minute->SetOnTouchListener(this);

    UIView* second = timePicker_->GetChildById("second");
    if (second == nullptr) {
        return;
    }
    static_cast<UIList*>(second)->SetRotateFactor(5); // 5: rotate factor
    second->SetOnTouchListener(this);

    container_->Add(timePicker_);
    SetLastPos(timePicker_);
}

void UITestRotateInput::UIKit_Rotate_Event_Picker_006()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UIPicker Test");
    container_->Add(label);
    label->SetPosition(0, lastY_ + g_blank);
    if (picker_ == nullptr) {
        picker_ = new UIPicker();
    }
    picker_->SetPosition(0, label->GetY() + g_blank, g_listW, g_listH);
    picker_->SetItemHeight(50); // 50: height
    picker_->SetValues(g_pickerRange, sizeof(g_pickerRange) / sizeof(g_pickerRange[0]));
    picker_->SetOnTouchListener(this);
    picker_->SetRotateFactor(5); // 5: rotate factor
    container_->Add(picker_);
    SetLastPos(picker_);
}
bool UITestRotateInput::OnPress(UIView& view, const PressEvent& event)
{
    if (&view == container_) {
        container_->RequestFocus();
        return true;
    } else if (&view == list_) {
        list_->RequestFocus();
        return true;
    } else if (&view == scroll_) {
        scroll_->RequestFocus();
        return true;
    } else if (&view == swipe_) {
        swipe_->RequestFocus();
        return true;
    } else if (&view == slider_) {
        slider_->RequestFocus();
        return true;
    } else if (&view == timePicker_->GetChildById(UITimePicker::HOUR_LIST_NAME)) {
        view.RequestFocus();
        return true;
    } else if (&view == timePicker_->GetChildById(UITimePicker::MIN_LIST_NAME)) {
        view.RequestFocus();
        return true;
    } else if (&view == timePicker_->GetChildById(UITimePicker::SEC_LIST_NAME)) {
        view.RequestFocus();
        return true;
    } else if (&view == picker_) {
        picker_->RequestFocus();
        return true;
    }
    return false;
}
void UITestRotateInput::SetLastPos(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    lastX_ = view->GetX();
    lastY_ = view->GetY() + view->GetHeight();
}

void UITestRotateInput::CreateLabelButton(int16_t x, int16_t y, UIView::OnClickListener* listener, const char* text)
{
    UILabelButton* labelButton = new UILabelButton();
    labelButton->SetPosition(x, y, g_testButtonW, g_testButtonH);
    labelButton->SetOnClickListener(listener);
    labelButton->SetText(text);
    labelButton->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 15);
    container_->Add(labelButton);
    SetLastPos(labelButton);
}

void UITestRotateInput::SetTestLabelButton(UILabel* label, UIView* view)
{
    if (listener1_ == nullptr) {
        listener1_ = static_cast<UIView::OnClickListener*>(new TestSetRotateFactorListener(view, 0));
    }
    CreateLabelButton(g_swipeW + g_blank, label->GetY() + g_blank, listener1_, "factor: 0");
    if (listener2_ == nullptr) {
        listener2_ =
            static_cast<UIView::OnClickListener*>(new TestSetRotateFactorListener(view, 15)); // 15: rotate factor
    }
    CreateLabelButton(g_swipeW + g_blank, lastY_ + g_blank, listener2_, "factor: 15");
    if (listener3_ == nullptr) {
        listener3_ =
            static_cast<UIView::OnClickListener*>(new TestSetRotateFactorListener(view, -15)); // -15: rotate factor
    }
    CreateLabelButton(g_swipeW + g_blank, lastY_ + g_blank, listener3_, "factor: -15");
    if (listener4_ == nullptr) {
        listener4_ =
            static_cast<UIView::OnClickListener*>(new TestSetRotateFactorListener(view, 30)); // 30: rotate factor
    }
    int16_t x = lastX_ + g_testButtonW + g_blank;
    CreateLabelButton(x, label->GetY() + g_blank, listener4_, "factor: 30");
    if (listener5_ == nullptr) {
        listener5_ =
            static_cast<UIView::OnClickListener*>(new TestSetRotateFactorListener(view, -30)); // -30: rotate factor
    }
    CreateLabelButton(x, lastY_ + g_blank, listener5_, "factor: -30");
}
} // namespace OHOS
#endif
