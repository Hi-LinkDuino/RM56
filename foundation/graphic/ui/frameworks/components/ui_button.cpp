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

#include "components/ui_button.h"
#include "animator/interpolation.h"
#include "common/image.h"
#include "draw/draw_image.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/style.h"
#include "imgdecode/cache_manager.h"
#include "themes/theme_manager.h"

namespace OHOS {
UIButton::UIButton()
    : defaultImgSrc_(nullptr),
      triggeredImgSrc_(nullptr),
      currentImgSrc_(ButtonImageSrc::BTN_IMAGE_DEFAULT),
      imgX_(0),
      imgY_(0),
      contentWidth_(0),
      contentHeight_(0),
      state_(RELEASED),
      styleState_(RELEASED),
#if DEFAULT_ANIMATION
      enableAnimation_(true),
      animator_(*this),
#endif
      buttonStyleAllocFlag_(false)
{
    touchable_ = true;
    SetupThemeStyles();
}

UIButton::~UIButton()
{
    if (defaultImgSrc_ != nullptr) {
        delete defaultImgSrc_;
        defaultImgSrc_ = nullptr;
    }

    if (triggeredImgSrc_ != nullptr) {
        delete triggeredImgSrc_;
        triggeredImgSrc_ = nullptr;
    }

    if (buttonStyleAllocFlag_) {
        for (uint8_t i = 0; i < BTN_STATE_NUM; i++) {
            delete buttonStyles_[i];
            buttonStyles_[i] = nullptr;
        }
        buttonStyleAllocFlag_ = false;
    }
}

void UIButton::DrawImg(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, OpacityType opaScale)
{
    const Image* image = GetCurImageSrc();
    if (image == nullptr) {
        return;
    }

    ImageHeader header = {0};
    image->GetHeader(header);
    Rect coords;
    Rect viewRect = GetContentRect();
    coords.SetLeft(viewRect.GetLeft() + GetImageX());
    coords.SetTop(viewRect.GetTop() + GetImageY());
    coords.SetWidth(header.width);
    coords.SetHeight(header.height);

    Rect trunc(invalidatedArea);
    if (trunc.Intersect(trunc, viewRect)) {
        image->DrawImage(gfxDstBuffer, coords, trunc, *buttonStyles_[state_], opaScale);
    }
}

void UIButton::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    OpacityType opa = GetMixOpaScale();
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetOrigRect(), invalidatedArea, *buttonStyles_[state_], opa);
    DrawImg(gfxDstBuffer, invalidatedArea, opa);
}

void UIButton::SetupThemeStyles()
{
    Theme* theme = ThemeManager::GetInstance().GetCurrent();

    if (theme == nullptr) {
        buttonStyles_[RELEASED] = &(StyleDefault::GetButtonReleasedStyle());
        buttonStyles_[PRESSED] = &(StyleDefault::GetButtonPressedStyle());
        buttonStyles_[INACTIVE] = &(StyleDefault::GetButtonInactiveStyle());
    } else {
        buttonStyles_[RELEASED] = &(theme->GetButtonStyle().released);
        buttonStyles_[PRESSED] = &(theme->GetButtonStyle().pressed);
        buttonStyles_[INACTIVE] = &(theme->GetButtonStyle().inactive);
    }
    style_ = buttonStyles_[RELEASED];
}

int64_t UIButton::GetStyle(uint8_t key) const
{
    return GetStyleForState(key, styleState_);
}

void UIButton::SetStyle(uint8_t key, int64_t value)
{
    SetStyleForState(key, value, styleState_);
}

int64_t UIButton::GetStyleForState(uint8_t key, ButtonState state) const
{
    if (state < BTN_STATE_NUM) {
        return (buttonStyles_[state])->GetStyle(key);
    }
    return 0;
}

void UIButton::SetStyleForState(uint8_t key, int64_t value, ButtonState state)
{
    if (state < BTN_STATE_NUM) {
        if (!buttonStyleAllocFlag_) {
            for (uint8_t i = 0; i < BTN_STATE_NUM; i++) {
                Style styleSaved = *buttonStyles_[i];
                buttonStyles_[i] = new Style;
                if (buttonStyles_[i] == nullptr) {
                    GRAPHIC_LOGE("new Style fail");
                    return;
                }
                *(buttonStyles_[i]) = styleSaved;
            }
            buttonStyleAllocFlag_ = true;
        }
        style_ = buttonStyles_[RELEASED];
        int16_t width = GetWidth();
        int16_t height = GetHeight();
        int16_t x = GetX();
        int16_t y = GetY();
        buttonStyles_[state]->SetStyle(key, value);
        Rect rect(x, y, x + width - 1, y + height -  1);
        UpdateRectInfo(key, rect);
    }
}

bool UIButton::OnPressEvent(const PressEvent& event)
{
    currentImgSrc_ = ButtonImageSrc::BTN_IMAGE_TRIGGERED;
    SetState(PRESSED);
    Resize(contentWidth_, contentHeight_);
    Invalidate();
#if DEFAULT_ANIMATION
    if (enableAnimation_) {
        animator_.Start();
    }
#endif
    return UIView::OnPressEvent(event);
}

bool UIButton::OnReleaseEvent(const ReleaseEvent& event)
{
    currentImgSrc_ = ButtonImageSrc::BTN_IMAGE_DEFAULT;
    SetState(RELEASED);
    Resize(contentWidth_, contentHeight_);
    Invalidate();
#if DEFAULT_ANIMATION
    if (enableAnimation_) {
        animator_.Start();
    }
#endif
    return UIView::OnReleaseEvent(event);
}

bool UIButton::OnCancelEvent(const CancelEvent& event)
{
    currentImgSrc_ = ButtonImageSrc::BTN_IMAGE_DEFAULT;
    SetState(RELEASED);
    Resize(contentWidth_, contentHeight_);
    Invalidate();
#if DEFAULT_ANIMATION
    if (enableAnimation_) {
        animator_.Start();
    }
#endif
    return UIView::OnCancelEvent(event);
}

const Image* UIButton::GetCurImageSrc() const
{
    if (currentImgSrc_ == ButtonImageSrc::BTN_IMAGE_DEFAULT) {
        return defaultImgSrc_;
    } else if (currentImgSrc_ == ButtonImageSrc::BTN_IMAGE_TRIGGERED) {
        return triggeredImgSrc_;
    } else {
        return nullptr;
    }
}

void UIButton::SetImageSrc(const char* defaultImgSrc, const char* triggeredImgSrc)
{
    if (!InitImage()) {
        return;
    }
    defaultImgSrc_->SetSrc(defaultImgSrc);
    triggeredImgSrc_->SetSrc(triggeredImgSrc);
}

void UIButton::SetImageSrc(const ImageInfo* defaultImgSrc, const ImageInfo* triggeredImgSrc)
{
    if (!InitImage()) {
        return;
    }
    defaultImgSrc_->SetSrc(defaultImgSrc);
    triggeredImgSrc_->SetSrc(triggeredImgSrc);
}

void UIButton::Disable()
{
    SetState(INACTIVE);
    touchable_ = false;
}

void UIButton::Enable()
{
    SetState(RELEASED);
    touchable_ = true;
}

void UIButton::SetState(ButtonState state)
{
    state_ = state;
    style_ = buttonStyles_[state_];
    Invalidate();
}

bool UIButton::InitImage()
{
    if (defaultImgSrc_ == nullptr) {
        defaultImgSrc_ = new Image();
        if (defaultImgSrc_ == nullptr) {
            GRAPHIC_LOGE("new Image fail");
            return false;
        }
    }
    if (triggeredImgSrc_ == nullptr) {
        triggeredImgSrc_ = new Image();
        if (triggeredImgSrc_ == nullptr) {
            GRAPHIC_LOGE("new Image fail");
            return false;
        }
    }
    return true;
}

bool UIButton::OnPreDraw(Rect& invalidatedArea) const
{
    Rect rect(GetRect());
    int16_t r = buttonStyles_[styleState_]->borderRadius_;
    if (r == COORD_MAX) {
        return true;
    }

    if (r != 0) {
        r = ((r & 0x1) == 0) ? (r >> 1) : ((r + 1) >> 1);
        rect.SetLeft(rect.GetX() + r);
        rect.SetWidth(rect.GetWidth() - r);
        rect.SetTop(rect.GetY() + r);
        rect.SetHeight(rect.GetHeight() - r);
    }
    if (rect.IsContains(invalidatedArea)) {
        return true;
    }
    invalidatedArea.Intersect(invalidatedArea, rect);
    return false;
}

#if DEFAULT_ANIMATION
void UIButton::OnPostDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    if (state_ == ButtonState::PRESSED && enableAnimation_) {
        animator_.DrawMask(gfxDstBuffer, invalidatedArea);
    }
    UIView::OnPostDraw(gfxDstBuffer, invalidatedArea);
}

namespace {
constexpr float FULL_SCALE = 1.0f;
constexpr float SHRINK_SCALE = 0.8f;
constexpr uint32_t SHRINK_DURATION = 150;
constexpr uint32_t RECOVER_DURATION = 200;
constexpr int64_t MASK_OPA = 25;
constexpr float BEZIER_CONTROL = 0.2f;
} // namespace

void UIButton::ButtonAnimator::Start()
{
    bool isReverse = (button_.state_ == UIButton::ButtonState::PRESSED);
    float targetScale = isReverse ? SHRINK_SCALE : FULL_SCALE;
    if ((animator_.GetState() == Animator::STOP) && FloatEqual(targetScale, scale_)) {
        return;
    }

    if (isReverse) {
        animator_.SetTime(SHRINK_DURATION);
    } else {
        animator_.SetTime(RECOVER_DURATION);
    }
    animator_.Start();
    /* reverse the animator direction */
    float x = isReverseAnimation_ ? (FULL_SCALE - scale_) : (scale_ - SHRINK_SCALE);
    float y = x / (FULL_SCALE - SHRINK_SCALE);
    x = Interpolation::GetBezierY(FULL_SCALE - y, 0, BEZIER_CONTROL, FULL_SCALE, BEZIER_CONTROL);
    animator_.SetRunTime(static_cast<uint32_t>(animator_.GetTime() * x));
    isReverseAnimation_ = isReverse;
}

void UIButton::ButtonAnimator::DrawMask(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    Style maskStyle;
    maskStyle.SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    maskStyle.SetStyle(STYLE_BACKGROUND_OPA, MASK_OPA);
    maskStyle.SetStyle(STYLE_BORDER_RADIUS, button_.GetStyle(STYLE_BORDER_RADIUS));
    OpacityType opa = button_.GetMixOpaScale();
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, button_.GetRect(), invalidatedArea, maskStyle, opa);
}

static inline void ScaleButton(UIButton& button, float scale)
{
    Vector2<float> scaleValue_ = {scale, scale};
    Vector2<float> centrePoint(button.GetWidth() / 2.0f, button.GetHeight() / 2.0f);
    button.Scale(scaleValue_, centrePoint);
}

void UIButton::ButtonAnimator::Callback(UIView* view)
{
    float x = static_cast<float>(animator_.GetRunTime()) / animator_.GetTime();
    float offset = Interpolation::GetBezierY(x, BEZIER_CONTROL, 0, BEZIER_CONTROL, FULL_SCALE);
    float scale = (FULL_SCALE - SHRINK_SCALE) * offset;

    scale_ = isReverseAnimation_ ? (FULL_SCALE - scale) : (scale + SHRINK_SCALE);
    ScaleButton(button_, scale_);
}

void UIButton::ButtonAnimator::OnStop(UIView& view)
{
    if (isReverseAnimation_) {
        scale_ = SHRINK_SCALE;
        ScaleButton(button_, SHRINK_SCALE);
    } else {
        scale_ = FULL_SCALE;
        button_.ResetTransParameter();
    }
}
#endif
} // namespace OHOS
