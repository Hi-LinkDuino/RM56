/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
#include "animator/easing_equation.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "core/render_manager.h"
#include "gfx_utils/sys_info.h"
#include "graphic_config.h"
#include "hal_tick.h"
#include "hilog/log.h"
#include "ui_test.h"
#include <stdio.h>
#include <stdlib.h>

#undef LOG_TAG
#define LOG_TAG "AnimatorDemo"

using namespace OHOS;

namespace
{
static constexpr uint16_t TITLE_LABEL_DEFAULT_HEIGHT = 29;
static constexpr uint16_t FONT_DEFAULT_SIZE = 20;
static constexpr uint16_t VIEW_DISTANCE_TO_LEFT_SIDE = 48;
static constexpr uint16_t VIEW_DISTANCE_TO_TOP_SIDE = 48;
static constexpr uint16_t VIEW_DISTANCE_TO_LEFT_SIDE2 = 24;
static constexpr uint16_t TEXT_DISTANCE_TO_LEFT_SIDE = 48;
static constexpr uint16_t TEXT_DISTANCE_TO_TOP_SIDE = 20;
static constexpr uint16_t HALF_OPA_OPAQUE = OPA_OPAQUE / 2;
static constexpr uint16_t VIEW_STYLE_BORDER_WIDTH = 2;
static constexpr uint16_t VIEW_STYLE_BORDER_RADIUS = 8;
static constexpr uint16_t BUTTON_LABEL_SIZE = 16;
static constexpr uint8_t BUTTON_STYLE_BORDER_RADIUS_VALUE = 20;
static constexpr uint32_t BUTTON_STYLE_BACKGROUND_COLOR_VALUE = 0xFF333333;
static constexpr uint32_t BUTTON_STYLE_BACKGROUND_COLOR_PRESS = 0xFF2D2D2D;
static constexpr int16_t BACK_BUTTON_HEIGHT = 64;
static constexpr uint16_t BUTTON_WIDHT1 = 80;
static constexpr uint16_t BUTTON_HEIGHT1 = 40;
static constexpr uint16_t BUTTON_WIDHT2 = 120;
static constexpr uint16_t BUTTON_HEIGHT2 = 40;
static constexpr uint16_t BUTTON_WIDHT3 = 150;
static constexpr uint16_t BUTTON_HEIGHT3 = 60;
const int16_t LABEL_HEIGHT = 29;
const int16_t BUTTON_WIDTH = BUTTON_WIDHT3;
const int16_t BUTTON_HEIGHT = BUTTON_HEIGHT3;
const int16_t DELTA_X_COORDINATE = 5;
const int16_t DELTA_Y_COORDINATE = 12;
} // namespace

static void DeleteChildren(UIView *view)
{
    if (view == nullptr) {
        return;
    }
    while (view) {
        UIView *tempView = view;
        view = view->GetNextSibling();
        if (tempView->IsViewGroup()) {
            DeleteChildren(static_cast<UIViewGroup *>(tempView)->GetChildrenHead());
        }
        if (tempView->GetParent()) {
            static_cast<UIViewGroup *>(tempView->GetParent())->Remove(tempView);
        }
        delete tempView;
    }
}

class AnimatorDemoCallback : public AnimatorCallback
{
public:
    AnimatorDemoCallback(UIView *uiView, int16_t startPos, int16_t endPos)
        : startPos_(startPos),
          endPos_(endPos),
          easingFunc_(EasingEquation::LinearEaseNone),
          animator_(new Animator(this, uiView, 1000, false)) // duration = 1000ms, repeat = false
    {
    }

    virtual ~AnimatorDemoCallback()
    {
        delete animator_;
    }

    Animator *GetAnimator() const
    {
        return animator_;
    }

    void SetEasingFunc(EasingFunc easingFunc)
    {
        easingFunc_ = easingFunc;
    }

    virtual void Callback(UIView *view)
    {
        if (view == nullptr) {
            return;
        }
        int16_t pos = easingFunc_(startPos_, endPos_, animator_->GetRunTime(), animator_->GetTime());
        Rect invalidatedArea = view->GetRect();
        view->SetPosition(pos, view->GetY());
        invalidatedArea.Join(invalidatedArea, view->GetRect());
        view->InvalidateRect(invalidatedArea);
    }

protected:
    int16_t startPos_;
    int16_t endPos_;
    EasingFunc easingFunc_;
    Animator *animator_;
};

class AnimatorDemo : public UIView::OnClickListener, public SysInfo::OnFPSChangedListener
{
public:
    static AnimatorDemo *GetInstance()
    {
        static AnimatorDemo instance;
        return &instance;
    }

    void SetUp();
    UIView *GetTestView();

private:
    AnimatorDemo() {}
    ~AnimatorDemo()
    {
        DeleteChildren(container_);
        container_ = nullptr;
        label_ = nullptr;
        button_ = nullptr;
        scroll_ = nullptr;
        delete callback_;
        callback_ = nullptr;
        uiViewGroupFrame_ = nullptr;
    };

    void SetUpButton(UILabelButton *btn, const char *title, int16_t x, int16_t y);
    void SetUpLabel(const char *title, int16_t x, int16_t y);

    bool OnClick(UIView &view, const ClickEvent &event) override;
    void OnFPSChanged(float newFPS) override;

    void UIKit_Animator_Test_BackEasing_001();

    UILabel *GetTitleLabel(const char *titleName)
    {
        if (titleName == nullptr) {
            return nullptr;
        }
        UILabel *label = new UILabel();
        if (label == nullptr) {
            return nullptr;
        }
        // 2: half of screen width
        label->SetPosition(0, 0, Screen::GetInstance().GetWidth() / 2, TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText(titleName);
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        return label;
    }

    UIScrollView *container_ = nullptr;
    UIScrollView *scroll_ = nullptr;
    UILabel *label_ = nullptr;
    UIButton *button_ = nullptr;
    AnimatorDemoCallback *callback_ = nullptr;
    Animator *animator_ = nullptr;
    UIViewGroup *uiViewGroupFrame_ = nullptr;

    UILabelButton *backOvershootBtn_ = nullptr;
    UILabelButton *backEaseInBtn_ = nullptr;
    UILabelButton *backEaseOutBtn_ = nullptr;
    UILabelButton *backEaseInOutBtn_ = nullptr;

    int16_t positionX_ = 0;
    int16_t positionY_ = 0;
};

void AnimatorDemo::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetPosition(25, 25, Screen::GetInstance().GetWidth() - 50, Screen::GetInstance().GetHeight() - 50);
        uiViewGroupFrame_ = new UIViewGroup();
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_OPA, HALF_OPA_OPAQUE);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_WIDTH, VIEW_STYLE_BORDER_WIDTH);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_RADIUS, VIEW_STYLE_BORDER_RADIUS);
        uiViewGroupFrame_->SetStyle(STYLE_BACKGROUND_OPA, 0);
        uiViewGroupFrame_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, VIEW_DISTANCE_TO_TOP_SIDE);
        container_->Add(uiViewGroupFrame_);
        label_ = new UILabel();
        container_->Add(label_);
        label_->SetPosition(100, 20, 264, 48);
        label_->SetText("动画效果");
        label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        button_ = new UIButton();
        button_->SetPosition(0, 0, 100, 100);
        button_->SetStyle(STYLE_BORDER_RADIUS, 90);
        callback_ = new AnimatorDemoCallback(button_, 0, 320);
        animator_ = callback_->GetAnimator();
        uiViewGroupFrame_->Add(button_);
        uiViewGroupFrame_->Resize(container_->GetWidth() - VIEW_DISTANCE_TO_LEFT_SIDE,
                                  button_->GetHeight());
    }
    if (scroll_ == nullptr) {
        scroll_ = new UIScrollView();
        // 20: increase y-coordinate
        uint16_t y = uiViewGroupFrame_->GetY() + uiViewGroupFrame_->GetHeight() + 20;
        scroll_->SetPosition(0, y, Screen::GetInstance().GetWidth(),
                             Screen::GetInstance().GetHeight() - y - 64); // 64: decrease y-coordinate
        container_->Add(scroll_);
        positionX_ = 0;
    }
    RenderManager::GetInstance().RegisterFPSChangedListener(this);
    // this->SetFPSCalculateType(SysInfo::FPS_CT_PRECISE_SAMPLING);
    this->SetFPSCalculateType(SysInfo::FPS_CT_FIXED_TIME);
}

UIView *AnimatorDemo::GetTestView()
{
    UIKit_Animator_Test_BackEasing_001();
    return container_;
}

void AnimatorDemo::SetUpLabel(const char *title, int16_t x, int16_t y)
{
    UILabel *label = GetTitleLabel(title);
    scroll_->Add(label);
    label->SetPosition(x, y, 288, LABEL_HEIGHT); // 288: label width
}

void AnimatorDemo::SetUpButton(UILabelButton *btn, const char *title, int16_t x, int16_t y)
{
    if (btn == nullptr) {
        return;
    }
    scroll_->Add(btn);
    btn->SetPosition(x, y, BUTTON_WIDHT3, BUTTON_HEIGHT3);
    btn->SetText(title);
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    btn->SetOnClickListener(this);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    scroll_->Invalidate();
}

void AnimatorDemo::UIKit_Animator_Test_BackEasing_001()
{
    backOvershootBtn_ = new UILabelButton();
    backEaseInBtn_ = new UILabelButton();
    backEaseOutBtn_ = new UILabelButton();
    backEaseInOutBtn_ = new UILabelButton();
    positionX_ = TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ = 0;
    SetUpLabel("back动画效果： ", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(backOvershootBtn_, "overshoot+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(backEaseInBtn_, "BackEaseIn", positionX_, positionY_);
    positionX_ = TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE;
    SetUpButton(backEaseOutBtn_, "BackEaseOut", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(backEaseInOutBtn_, "BackEaseInOut", positionX_, positionY_);
}

bool AnimatorDemo::OnClick(UIView &view, const ClickEvent &event)
{
    static double overshoot = 1.7;
    if (&view == backOvershootBtn_) {
        overshoot += 1;
        EasingEquation::SetBackOvershoot(overshoot);
    } else if (&view == backEaseInBtn_) {
        callback_->SetEasingFunc(EasingEquation::BackEaseIn);
    } else if (&view == backEaseOutBtn_) {
        callback_->SetEasingFunc(EasingEquation::BackEaseOut);
    } else if (&view == backEaseInOutBtn_) {
        callback_->SetEasingFunc(EasingEquation::BackEaseInOut);
    }
    animator_->Start();
    container_->Invalidate();
    return true;
}

void AnimatorDemo::OnFPSChanged(float newFPS)
{
    static float lastFPS = 0.0f;
    if (abs(newFPS - lastFPS) < 1) {
        return;
    }
    lastFPS = newFPS;
    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "动画效果 %.0f fps", newFPS);
    HILOG_DEBUG(HILOG_MODULE_APP, "%.0f fps\n", newFPS);
    label_->SetText(reinterpret_cast<char *>(buf));
    label_->Invalidate();
}

/**
 * UIScaleRotateDemo
 */
class UIScaleRotateDemo : public AnimatorCallback
{
public:
    static UIScaleRotateDemo *GetInstance()
    {
        static UIScaleRotateDemo instance;
        return &instance;
    }

    void SetUp()
    {
        container_ = new UIScrollView();
        container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());

        label_ = new UILabel();
        label_->SetText("label");
        label_->SetPosition(100, 100, 100, 50);
        container_->Add(label_);

        button_ = new UILabelButton();
        button_->SetText("button");
        button_->SetPosition(100, 200, 100, 50);
        container_->Add(button_);

        animator_ = new Animator(this, nullptr, 50, true);
        animator_->Start();
        lastRun_ = HALTick::GetInstance().GetTime();
    }

    UIView *GetTestView()
    {
        return container_;
    }

private:
    UIScaleRotateDemo() {}
    ~UIScaleRotateDemo()
    {
        DeleteChildren(container_);
        container_ = nullptr;
        label_ = nullptr;
        button_ = nullptr;
        if (animator_) {
            delete animator_;
            animator_ = nullptr;
        }
    }

    void Callback(UIView *view) override
    {
        angleValue_++;

        if (scaleValue_.x_ < 0.5f) {
            scaleStep_ = 0.02f;
        } else if (scaleValue_.x_ > 1.5f) {
            scaleStep_ = -0.02f;
        }
        scaleValue_.x_ += scaleStep_;
        scaleValue_.y_ += scaleStep_;
        label_->Rotate(angleValue_, VIEW_CENTER1);
        label_->Scale(scaleValue_, VIEW_CENTER1);
        button_->Rotate(angleValue_, VIEW_CENTER2);
        button_->Scale(scaleValue_, VIEW_CENTER2);

        frame_cnt_++;
        if (HALTick::GetInstance().GetElapseTime(lastRun_) >= 1000) {
            HILOG_DEBUG(HILOG_MODULE_APP, "%u fps\n", frame_cnt_);
            lastRun_ = HALTick::GetInstance().GetTime();
            frame_cnt_ = 0;
        }
    }

private:
    UIViewGroup *container_ = nullptr;
    UILabel *label_ = nullptr;
    UILabelButton *button_ = nullptr;
    Animator *animator_ = nullptr;
    const Vector2<float> VIEW_CENTER1 = {100.0f, 100.0f};
    const Vector2<float> VIEW_CENTER2 = {100.0f, 200.0f};
    int16_t angleValue_ = 0;
    Vector2<float> scaleValue_ = {1.0f, 1.0f};
    float scaleStep_ = 0.02f;
    uint32_t lastRun_ = 0;
    uint32_t frame_cnt_ = 0;
};

void AnimatorDemoStart(void)
{
    RootView *rootView_ = RootView::GetInstance();
    rootView_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());

    AnimatorDemo *view = AnimatorDemo::GetInstance();
    // UIScaleRotateDemo *view = UIScaleRotateDemo::GetInstance();
    view->SetUp();
    rootView_->Add(view->GetTestView());
    rootView_->Invalidate();
}