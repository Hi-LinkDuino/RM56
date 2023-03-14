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

#include "ui_test_focus_manager.h"

#if ENABLE_FOCUS_MANAGER
namespace OHOS {
namespace {
const uint16_t LABEL_BUTTON_DEFAULT_FONT_SIZE = 15;
const uint16_t LABEL_BUTTON_DEFAULT_WIDTH = 60;
const uint16_t LABEL_BUTTON_DEFAULT_HEIGHT = 40;
} // namespace

class TestOnFocusListener : public UIView::OnFocusListener {
public:
    TestOnFocusListener() {}

    ~TestOnFocusListener() {}

    bool OnFocus(UIView& view) override
    {
        if (view.IsViewGroup()) {
            view.SetStyle(STYLE_BORDER_COLOR, Color::Red().full);
        } else {
            view.SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
        }
        view.Invalidate();
        return true;
    }

    bool OnBlur(UIView& view) override
    {
        if (view.IsViewGroup()) {
            view.SetStyle(STYLE_BORDER_COLOR, Color::White().full);
        } else {
            /* 0: red, 125: green, 255: blue */
            view.SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0, 125, 255).full);
        }
        view.Invalidate();
        return true;
    }
};

class RequestFocusByDirectionOnClickListener : public UIView::OnClickListener {
public:
    explicit RequestFocusByDirectionOnClickListener(uint8_t direction) : direction_(direction) {}

    ~RequestFocusByDirectionOnClickListener() {}

    bool OnClick(UIView& view, const ClickEvent& event)
    {
        FocusManager::GetInstance()->RequestFocusByDirection(direction_);
        return true;
    }
private:
    uint8_t direction_ = 0;
};

class ResetFocusOnClickListener : public UIView::OnClickListener {
public:
    explicit ResetFocusOnClickListener(UIView* focus) : focus_(focus) {}

    ~ResetFocusOnClickListener() {}

    bool OnClick(UIView& view, const ClickEvent& event)
    {
        FocusManager::GetInstance()->RequestFocus(focus_);
        return true;
    }

private:
    UIView* focus_ = nullptr;
};

class SetGroupInterceptOnClickListener : public UIView::OnClickListener {
public:
    SetGroupInterceptOnClickListener(UIViewGroup* viewGroup, bool intercept)
        : viewGroup_(viewGroup), intercept_(intercept) {}

    ~SetGroupInterceptOnClickListener() {}

    bool OnClick(UIView& view, const ClickEvent& event)
    {
        if (viewGroup_ != nullptr) {
            viewGroup_->SetInterceptFocus(intercept_);
            return true;
        }
        return false;
    }
private:
    UIViewGroup* viewGroup_ = nullptr;
    bool intercept_;
};

class SetGroupInterceptFalseOnClickListener : public UIView::OnClickListener {
public:
    explicit SetGroupInterceptFalseOnClickListener(UIViewGroup* viewGroup) : viewGroup_(viewGroup) {}

    ~SetGroupInterceptFalseOnClickListener() {}

    bool OnClick(UIView& view, const ClickEvent& event)
    {
        viewGroup_->SetInterceptFocus(false);
        return true;
    }
private:
    UIViewGroup* viewGroup_ = nullptr;
};

class SetFocusableOnClickListener : public UIView::OnClickListener {
public:
    SetFocusableOnClickListener(UIView* view, bool enable) : view_(view), enable_(enable) {}

    ~SetFocusableOnClickListener() {}

    bool OnClick(UIView& view, const ClickEvent& event)
    {
        view_->SetFocusable(enable_);
        return true;
    }
private:
    UIView* view_ = nullptr;
    bool enable_ = false;
};

class RequestFocusOnClickListener : public UIView::OnClickListener {
public:
    explicit RequestFocusOnClickListener(UIView* view) : view_(view) {}

    ~RequestFocusOnClickListener() {}

    bool OnClick(UIView& view, const ClickEvent& event)
    {
        FocusManager::GetInstance()->RequestFocus(view_);
        return true;
    }
private:
    UIView* view_ = nullptr;
};

class ClearFocusOnClickListener : public UIView::OnClickListener {
public:
    ClearFocusOnClickListener() {}

    ~ClearFocusOnClickListener() {}

    bool OnClick(UIView& view, const ClickEvent& event)
    {
        FocusManager::GetInstance()->ClearFocus();
        return true;
    }
};

void UITestFocusManager::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetHorizontalScrollState(false);
    }
}

void UITestFocusManager::TearDown()
{
    FocusManager::GetInstance()->ClearFocus();
    if (testOnFocusListener_ != nullptr) {
        delete testOnFocusListener_;
        testOnFocusListener_ = nullptr;
    }
    if (requestFocusByDirectionLeftListener_ != nullptr) {
        delete requestFocusByDirectionLeftListener_;
        requestFocusByDirectionLeftListener_ = nullptr;
    }
    if (requestFocusByDirectionRightListener_ != nullptr) {
        delete requestFocusByDirectionRightListener_;
        requestFocusByDirectionRightListener_ = nullptr;
    }
    if (requestFocusByDirectionUpListener_ != nullptr) {
        delete requestFocusByDirectionUpListener_;
        requestFocusByDirectionUpListener_ = nullptr;
    }
    if (requestFocusByDirectionLeftListener_ != nullptr) {
        delete requestFocusByDirectionLeftListener_;
        requestFocusByDirectionLeftListener_ = nullptr;
    }
    if (requestFocusByDirectionDownListener_ != nullptr) {
        delete requestFocusByDirectionDownListener_;
        requestFocusByDirectionDownListener_ = nullptr;
    }
    if (setFocusableViewListener_ != nullptr) {
        delete setFocusableViewListener_;
        setFocusableViewListener_ = nullptr;
    }
    if (setFocusableViewListener1_ != nullptr) {
        delete setFocusableViewListener1_;
        setFocusableViewListener1_ = nullptr;
    }
    if (setGroupInterceptListener_ != nullptr) {
        delete setGroupInterceptListener_;
        setGroupInterceptListener_ = nullptr;
    }
    if (setGroupInterceptListener1_ != nullptr) {
        delete setGroupInterceptListener1_;
        setGroupInterceptListener1_ = nullptr;
    }
    if (resetFocusListener_ != nullptr) {
        delete resetFocusListener_;
        resetFocusListener_ = nullptr;
    }
    if (clearFocusListener_ != nullptr) {
        delete clearFocusListener_;
        clearFocusListener_ = nullptr;
    }
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestFocusManager::GetTestView()
{
    UIKit_Focus_Manager_Test_001();
    return container_;
}

UIView* UITestFocusManager::CreateTestUILabel(UIViewGroup* parent, int16_t x, int16_t y,
    const char* text, bool focusable)
{
    UILabel* label = new UILabel();
    parent->Add(label);
    label->SetPosition(x, y, LABEL_BUTTON_DEFAULT_WIDTH, LABEL_BUTTON_DEFAULT_HEIGHT);
    label->SetText(text);
    label->SetViewId(text);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_BUTTON_DEFAULT_FONT_SIZE);
    label->SetFocusable(focusable);
    /* 0: red, 125: green, 255: blue */
    label->SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0, 125, 255).full);
    label->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    label->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    if (testOnFocusListener_ == nullptr) {
        testOnFocusListener_ = static_cast<UIView::OnFocusListener*>(new TestOnFocusListener());
    }
    label->SetOnFocusListener(testOnFocusListener_);
    return label;
}

UIViewGroup* UITestFocusManager::CreateTestUIViewGroup(UIViewGroup* parent, bool focusable, bool interceptFocus)
{
    UIViewGroup* viewGroup = new UIViewGroup();
    parent->Add(viewGroup);
    viewGroup->SetFocusable(focusable);
    viewGroup->SetInterceptFocus(interceptFocus);
    viewGroup->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
    viewGroup->SetStyle(STYLE_BORDER_OPA, HALF_OPA_OPAQUE);
    viewGroup->SetStyle(STYLE_BORDER_WIDTH, 1);
    viewGroup->SetStyle(STYLE_BORDER_RADIUS, VIEW_STYLE_BORDER_RADIUS);
    if (testOnFocusListener_ == nullptr) {
        testOnFocusListener_ = static_cast<UIView::OnFocusListener*>(new TestOnFocusListener());
    }
    viewGroup->SetOnFocusListener(testOnFocusListener_);
    return viewGroup;
}

void UITestFocusManager::UIKit_Focus_Manager_Test_001()
{
    UILabel* label = new UILabel();
    container_->Add(label);
    /* 288: width */
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, 288, TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetText("焦点管理效果 ");
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    UIViewGroup* viewGroup = CreateTestUIViewGroup(container_, true, false);
    /* 600: width, 350: height */
    viewGroup->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, VIEW_DISTANCE_TO_TOP_SIDE, 600, 350);

    UIViewGroup* viewGroup1 = CreateTestUIViewGroup(viewGroup, true, false);
    UIViewGroup* viewGroup2 = CreateTestUIViewGroup(viewGroup1, true, false);
    UIViewGroup* viewGroup3 = CreateTestUIViewGroup(viewGroup, true, false);
    UIViewGroup* viewGroup4 = CreateTestUIViewGroup(viewGroup3, true, false);
    UIViewGroup* viewGroup5 = CreateTestUIViewGroup(viewGroup4, true, false);
    UIViewGroup* viewGroup6 = CreateTestUIViewGroup(viewGroup, true, false);

    viewGroup1->SetPosition(5, 5, 270, 60);      /* 5: x,   5: y;   270: width, 60: height */
    viewGroup2->SetPosition(100, 5, 80, 50);     /* 100: x, 5: y;   80: width,  50: height */
    viewGroup3->SetPosition(290, 5, 280, 115);   /* 290: x, 5: y;   280: width, 115: height */
    viewGroup4->SetPosition(5, 5, 260, 60);      /* 5: x,   5: y;   260: width, 60: height */
    viewGroup5->SetPosition(5, 5, 250, 50);      /* 5: x,   5: y;   250: width, 50: height */
    viewGroup6->SetPosition(100, 160, 400, 160); /* 100: x, 160: y; 400: width, 160: height */

    CreateTestUILabel(viewGroup1, 0, 5, "1", true);                   /* 0: x,   5: y */
    UIView* view2 = CreateTestUILabel(viewGroup1, 195, 5, "2", true); /* 195: x, 5: y */
    CreateTestUILabel(viewGroup2, 5, 5, "3", true);                   /* 5: x,   5: y */
    CreateTestUILabel(viewGroup5, 5, 5, "4", true);                   /* 5: x,   5: y */
    CreateTestUILabel(viewGroup5, 180, 5, "5", true);                 /* 180: x, 5: y */
    CreateTestUILabel(viewGroup3, 150, 70, "6", true);                /* 150: x, 70: y */

    CreateTestUILabel(viewGroup, 80, 100, "7", true);                 /* 80: x,  100: y */
    CreateTestUILabel(viewGroup6, 5, 5, "8", true);                   /* 5: x,   5: y */
    CreateTestUILabel(viewGroup6, 100, 80, "9", true);                /* 100: x, 80: y */
    CreateTestUILabel(viewGroup6, 300, 5, "10", true);                /* 300: x, 5: y */

    UIViewGroup* btnViewGroup = new UIViewGroup();
    /* 650: x, 300: width, 400: height */
    btnViewGroup->SetPosition(650, VIEW_DISTANCE_TO_TOP_SIDE, 300, 400);
    container_->Add(btnViewGroup);

    if (requestFocusByDirectionLeftListener_ == nullptr) {
        requestFocusByDirectionLeftListener_ = static_cast<UIView::OnClickListener*>(
            new RequestFocusByDirectionOnClickListener(FOCUS_DIRECTION_LEFT));
    }
    if (requestFocusByDirectionRightListener_ == nullptr) {
        requestFocusByDirectionRightListener_ = static_cast<UIView::OnClickListener*>(
            new RequestFocusByDirectionOnClickListener(FOCUS_DIRECTION_RIGHT));
    }
    if (requestFocusByDirectionUpListener_ == nullptr) {
        requestFocusByDirectionUpListener_ = static_cast<UIView::OnClickListener*>(
            new RequestFocusByDirectionOnClickListener(FOCUS_DIRECTION_UP));
    }
    if (requestFocusByDirectionDownListener_ == nullptr) {
        requestFocusByDirectionDownListener_ = static_cast<UIView::OnClickListener*>(
            new RequestFocusByDirectionOnClickListener(FOCUS_DIRECTION_DOWN));
    }
    if (setFocusableViewListener_ == nullptr) {
        setFocusableViewListener_ = static_cast<UIView::OnClickListener*>(
            new SetFocusableOnClickListener(view2, true));
    }
    if (setFocusableViewListener1_ == nullptr) {
        setFocusableViewListener1_ = static_cast<UIView::OnClickListener*>(
            new SetFocusableOnClickListener(view2, false));
    }
    if (setGroupInterceptListener_ == nullptr) {
        setGroupInterceptListener_ = static_cast<UIView::OnClickListener*>(
            new SetGroupInterceptOnClickListener(viewGroup5, true));
    }
    if (setGroupInterceptListener1_ == nullptr) {
        setGroupInterceptListener1_ = static_cast<UIView::OnClickListener*>(
            new SetGroupInterceptOnClickListener(viewGroup5, false));
    }
    if (resetFocusListener_ == nullptr) {
        resetFocusListener_ = static_cast<UIView::OnClickListener*>(
            new ResetFocusOnClickListener(viewGroup1->GetChildById("1")));
    }
    if (clearFocusListener_ == nullptr) {
        clearFocusListener_ = static_cast<UIView::OnClickListener*>(new ClearFocusOnClickListener());
    }
    /* 10: x, 10: y */
    SetUpButton("向左 ", 10, 10, btnViewGroup, requestFocusByDirectionLeftListener_);
    /* 150: x, 10: y */
    SetUpButton("向右 ", 150, 10, btnViewGroup, requestFocusByDirectionRightListener_);
    /* 10: x, 60: y */
    SetUpButton("向上 ", 10, 60, btnViewGroup, requestFocusByDirectionUpListener_);
    /* 150: x, 60: y */
    SetUpButton("向下 ", 150, 60, btnViewGroup, requestFocusByDirectionDownListener_);
    /* 10: x, 110: y */
    SetUpButton("2可获焦 ", 10, 110, btnViewGroup, setFocusableViewListener_);
    /* 150: x, 110: y */
    SetUpButton("2不可获焦 ", 150, 110, btnViewGroup, setFocusableViewListener1_);
    /* 10: x, 160: y */
    SetUpButton("设置4容器拦截 ", 10, 160, btnViewGroup, setGroupInterceptListener_);
    /* 150: x, 160: y */
    SetUpButton("取消4容器拦截 ", 150, 160, btnViewGroup, setGroupInterceptListener1_);
    /* 10: x, 210: y */
    SetUpButton("重置焦点 ", 10, 210, btnViewGroup, resetFocusListener_);
    /* 150: x, 210: y */
    SetUpButton("清除焦点 ", 150, 210, btnViewGroup, clearFocusListener_);

    FocusManager::GetInstance()->RequestFocus(viewGroup1->GetChildById("1"));
}

UILabelButton* UITestFocusManager::SetUpButton(const char* title, int16_t x, int16_t y, UIViewGroup* viewGroup,
    UIView::OnClickListener* listener)
{
    UILabelButton* btn = new UILabelButton();
    btn->SetPosition(x, y, BUTTON_WIDHT2, BUTTON_HEIGHT1);
    btn->SetText(title);
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    btn->SetOnClickListener(listener);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    viewGroup->Add(btn);
    return btn;
}
} // namespace OHOS
#endif
