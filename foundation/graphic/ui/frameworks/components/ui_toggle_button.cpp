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

#include "components/ui_toggle_button.h"

#include "common/image.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "imgdecode/cache_manager.h"
namespace {
constexpr int16_t TOGGLE_BTN_WIDTH = 32;
constexpr int16_t TOGGLE_BTN_CORNER_RADIUS = 11;
constexpr int16_t TOGGLE_BTN_RADIUS_DIFF = 2;
constexpr uint8_t TOGGLE_BTN_UNSELECTED_OPA = 97;
#if DEFAULT_ANIMATION
constexpr int16_t DEFAULT_ANIMATOR_TIME = 150;
constexpr float BEZIER_CONTROL_POINT_X_1 = 0.2;
constexpr float BEZIER_CONTROL_POINT_X_2 = 0.2;
#endif
} // namespace
namespace OHOS {
UIToggleButton::UIToggleButton()
    : corner_(TOGGLE_BTN_CORNER_RADIUS),
      radius_(TOGGLE_BTN_CORNER_RADIUS - TOGGLE_BTN_RADIUS_DIFF),
      rectWidth_(TOGGLE_BTN_WIDTH)
{
    backgroundOpacity_ = TOGGLE_BTN_UNSELECTED_OPA;
    image_[UNSELECTED].SetSrc("");
    image_[SELECTED].SetSrc("");
#if DEFAULT_ANIMATION
    checkBoxAnimator_.SetTime(DEFAULT_ANIMATOR_TIME);
#endif
    Resize(width_, height_);
}

void UIToggleButton::SetState(bool state)
{
    if (state) {
        UICheckBox::SetState(SELECTED);
    } else {
        UICheckBox::SetState(UNSELECTED);
    }
    Invalidate();
}

void UIToggleButton::CalculateSize()
{
    width_ = GetWidth();
    height_ = GetHeight();
    int16_t minValue = (width_ > height_) ? height_ : width_;
    corner_ = TOGGLE_BTN_CORNER_RADIUS * minValue / DEFAULT_HOT_HEIGHT;
    int16_t radiusDiff = TOGGLE_BTN_RADIUS_DIFF * minValue / DEFAULT_HOT_WIDTH;
    radius_ = corner_ - radiusDiff;
    rectWidth_ = TOGGLE_BTN_WIDTH * minValue / DEFAULT_HOT_WIDTH;

    Rect contentRect = GetContentRect();
    int16_t dx = (width_ - rectWidth_) / 2; // 2: half
    int16_t dy = (height_ / 2) - corner_;   // 2: half
    int16_t x = contentRect.GetX() + dx;
    int16_t y = contentRect.GetY() + dy;
    leftCenter_ = {static_cast<int16_t>(x + corner_), static_cast<int16_t>(y + corner_)};
    rightCenter_ = {static_cast<int16_t>(x + rectWidth_ - corner_), static_cast<int16_t>(y + corner_)};
#if DEFAULT_ANIMATION
    if (checkBoxAnimator_.GetState() != Animator::START) {
        currentCenter_ = (state_ == SELECTED) ? rightCenter_ : leftCenter_;
        backgroundOpacity_ = (state_ == SELECTED) ? OPA_OPAQUE : TOGGLE_BTN_UNSELECTED_OPA;
        bgColor_ = (state_ == SELECTED) ? selectedStateColor_ : Color::White();
    }
#else
    currentCenter_ = (state_ == SELECTED) ? rightCenter_ : leftCenter_;
    backgroundOpacity_ = (state_ == SELECTED) ? OPA_OPAQUE : TOGGLE_BTN_UNSELECTED_OPA;
    bgColor_ = (state_ == SELECTED) ? selectedStateColor_ : Color::White();
#endif
    rectMid_.SetRect(x, y, x + rectWidth_, y + (corner_ << 1) + 1);
}

void UIToggleButton::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    if ((image_[SELECTED].GetSrcType() != IMG_SRC_UNKNOWN) && (image_[UNSELECTED].GetSrcType() != IMG_SRC_UNKNOWN)) {
        UICheckBox::OnDraw(gfxDstBuffer, invalidatedArea);
    } else {
        CalculateSize();
        BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetRect(), invalidatedArea, *style_, opaScale_);
        Rect contentRect = GetContentRect();
        Rect trunc = invalidatedArea;
        bool isIntersect = trunc.Intersect(trunc, contentRect);
        if (!isIntersect) {
            return;
        }
        Style styleUnSelect = StyleDefault::GetBackgroundTransparentStyle();
        styleUnSelect.bgColor_ = bgColor_;
        styleUnSelect.bgOpa_ = backgroundOpacity_;
        styleUnSelect.borderRadius_ = corner_;
        BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, rectMid_, trunc, styleUnSelect, opaScale_);
        ArcInfo arcInfoLeft = {currentCenter_, {0}, radius_, 0, CIRCLE_IN_DEGREE, nullptr};
        styleUnSelect.lineColor_ = Color::White();
        styleUnSelect.lineWidth_ = radius_;
        BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfoLeft, trunc, styleUnSelect, OPA_OPAQUE,
                                              CapType::CAP_NONE);
    }
}
#if DEFAULT_ANIMATION
void UIToggleButton::Callback(UIView* view)
{
    runTime_ = checkBoxAnimator_.GetRunTime();
    float x = static_cast<float>(runTime_) / checkBoxAnimator_.GetTime();
    float coefficient = Interpolation::GetBezierY(x, BEZIER_CONTROL_POINT_X_1, 0, BEZIER_CONTROL_POINT_X_2, 1);
    if (state_ == SELECTED) {
        currentCenter_.y = rightCenter_.y;
        currentCenter_.x = static_cast<int16_t>((rightCenter_.x - leftCenter_.x) * coefficient) + leftCenter_.x;
        backgroundOpacity_ =
            static_cast<uint8_t>(TOGGLE_BTN_UNSELECTED_OPA + (OPA_OPAQUE - TOGGLE_BTN_UNSELECTED_OPA) * coefficient);
        bgColor_ =
            Color::GetMixColor(selectedStateColor_, Color::White(), static_cast<uint8_t>(OPA_OPAQUE * coefficient));
    } else {
        currentCenter_.y = leftCenter_.y;
        currentCenter_.x = rightCenter_.x - static_cast<uint16_t>((rightCenter_.x - leftCenter_.x) * coefficient);
        backgroundOpacity_ = static_cast<uint8_t>(OPA_OPAQUE - (OPA_OPAQUE - TOGGLE_BTN_UNSELECTED_OPA) * coefficient);
        bgColor_ = Color::GetMixColor(selectedStateColor_, Color::White(),
                                      static_cast<uint8_t>(OPA_OPAQUE * (1 - coefficient)));
    }
    Invalidate();
}

void UIToggleButton::OnStop(UIView& view)
{
    if (state_ == SELECTED) {
        bgColor_ = selectedStateColor_;
    } else {
        bgColor_ = Color::White();
    }
    backgroundOpacity_ = (state_ == SELECTED) ? OPA_OPAQUE : TOGGLE_BTN_UNSELECTED_OPA;
    Invalidate();
}
#endif
} // namespace OHOS
