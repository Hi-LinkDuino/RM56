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

#include "components/ui_slider.h"

#include "common/image.h"
#include "dock/focus_manager.h"
#include "dock/vibrator_manager.h"
#include "draw/draw_image.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "gfx_utils/graphic_log.h"
#include "imgdecode/cache_manager.h"
#include "themes/theme_manager.h"

namespace OHOS {
UISlider::UISlider() : knobWidth_(0), knobStyleAllocFlag_(false), knobImage_(nullptr), listener_(nullptr)
{
    touchable_ = true;
    draggable_ = true;
    dragParentInstead_ = false;
#if ENABLE_FOCUS_MANAGER
    focusable_ = true;
#endif
#if ENABLE_ROTATE_INPUT
    rotateFactor_ = DEFAULT_SLIDER_ROTATE_FACTOR;
    cachedRotation_ = 0;
#endif

    Theme* theme = ThemeManager::GetInstance().GetCurrent();
    if (theme != nullptr) {
        knobStyle_ = &(theme->GetSliderKnobStyle());
    } else {
        knobStyle_ = &(StyleDefault::GetSliderKnobStyle());
    }
}

UISlider::~UISlider()
{
    if (knobImage_ != nullptr) {
        delete knobImage_;
        knobImage_ = nullptr;
    }

    if (knobStyleAllocFlag_) {
        delete knobStyle_;
        knobStyle_ = nullptr;
        knobStyleAllocFlag_ = false;
    }
}

void UISlider::SetKnobStyle(const Style& style)
{
    if (!knobStyleAllocFlag_) {
        knobStyle_ = new Style;
        if (knobStyle_ == nullptr) {
            GRAPHIC_LOGE("new Style fail");
            return;
        }
        knobStyleAllocFlag_ = true;
    }
    *knobStyle_ = style;
}

void UISlider::SetKnobStyle(uint8_t key, int64_t value)
{
    if (!knobStyleAllocFlag_) {
        knobStyle_ = new Style(*knobStyle_);
        if (knobStyle_ == nullptr) {
            GRAPHIC_LOGE("new Style fail");
            return;
        }
        knobStyleAllocFlag_ = true;
    }
    knobStyle_->SetStyle(key, value);
}

const Style& UISlider::GetKnobStyle() const
{
    return *knobStyle_;
}

int64_t UISlider::GetKnobStyle(uint8_t key) const
{
    return knobStyle_->GetStyle(key);
}

void UISlider::SetKnobImage(const ImageInfo* knobImage)
{
    if (!InitImage()) {
        return;
    }
    knobImage_->SetSrc(knobImage);
}

void UISlider::SetKnobImage(const char* knobImage)
{
    if (!InitImage()) {
        return;
    }
    knobImage_->SetSrc(knobImage);
}

void UISlider::DrawKnob(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, const Rect& foregroundRect)
{
    int16_t halfKnobWidth = GetKnobWidth() / 2; // 2: half
    int16_t offset;
    Rect knobBar;
    switch (direction_) {
        case Direction::DIR_LEFT_TO_RIGHT: {
            offset = (knobWidth_ - progressHeight_) / 2; // 2: half
            knobBar.SetRect(foregroundRect.GetRight() - halfKnobWidth, foregroundRect.GetTop() - offset,
                            foregroundRect.GetRight() + halfKnobWidth, foregroundRect.GetBottom() + offset);
            break;
        }
        case Direction::DIR_RIGHT_TO_LEFT: {
            offset = (knobWidth_ - progressHeight_) / 2; // 2: half
            knobBar.SetRect(foregroundRect.GetLeft() - halfKnobWidth, foregroundRect.GetTop() - offset,
                            foregroundRect.GetLeft() + halfKnobWidth, foregroundRect.GetBottom() + offset);
            break;
        }
        case Direction::DIR_BOTTOM_TO_TOP: {
            offset = (knobWidth_ - progressWidth_) / 2; // 2: half
            knobBar.SetRect(foregroundRect.GetLeft() - offset, foregroundRect.GetTop() - halfKnobWidth,
                            foregroundRect.GetRight() + offset, foregroundRect.GetTop() + halfKnobWidth);
            break;
        }
        case Direction::DIR_TOP_TO_BOTTOM: {
            offset = (knobWidth_ - progressWidth_) / 2; // 2: half
            knobBar.SetRect(foregroundRect.GetLeft() - offset, foregroundRect.GetBottom() - halfKnobWidth,
                            foregroundRect.GetRight() + offset, foregroundRect.GetBottom() + halfKnobWidth);
            break;
        }
        default: {
            GRAPHIC_LOGW("UISlider::DrawKnob Direction error!\n");
        }
    }
    DrawValidRect(gfxDstBuffer, knobImage_, knobBar, invalidatedArea, *knobStyle_, 0);
}

bool UISlider::InitImage()
{
    if (!UIAbstractProgress::InitImage()) {
        return false;
    }
    if (knobImage_ == nullptr) {
        knobImage_ = new Image();
        if (knobImage_ == nullptr) {
            GRAPHIC_LOGE("new Image fail");
            return false;
        }
    }
    return true;
}

void UISlider::SetImage(const ImageInfo* backgroundImage, const ImageInfo* foregroundImage)
{
    if (!InitImage()) {
        return;
    }
    backgroundImage_->SetSrc(backgroundImage);
    foregroundImage_->SetSrc(foregroundImage);
}

void UISlider::SetImage(const char* backgroundImage, const char* foregroundImage)
{
    if (!InitImage()) {
        return;
    }
    backgroundImage_->SetSrc(backgroundImage);
    foregroundImage_->SetSrc(foregroundImage);
}

void UISlider::DrawForeground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, Rect& coords)
{
    Point startPoint;
    int16_t progressWidth;
    int16_t progressHeight;
    uint16_t radius;
    GetBackgroundParam(startPoint, progressWidth, progressHeight, radius, *foregroundStyle_);

    int16_t left;
    int16_t right;
    int16_t top;
    int16_t bottom;
    int16_t length;
    Rect foregroundRect;

    switch (direction_) {
        case Direction::DIR_LEFT_TO_RIGHT: {
            length = GetCurrentPos(progressWidth_ + 1);
            foregroundRect.SetRect(startPoint.x, startPoint.y, startPoint.x + progressWidth - 1,
                                   startPoint.y + progressHeight_ - 1);

            left = startPoint.x - radius - 1;
            right = left + length;
            coords.SetRect(left, startPoint.y, right, startPoint.y + progressHeight_ - 1);
            break;
        }
        case Direction::DIR_RIGHT_TO_LEFT: {
            length = GetCurrentPos(progressWidth_ + 1);
            foregroundRect.SetRect(startPoint.x, startPoint.y, startPoint.x + progressWidth - 1,
                                   startPoint.y + progressHeight_ - 1);

            right = startPoint.x + progressWidth + radius + 1;
            left = right - length;
            coords.SetRect(left, startPoint.y, right, startPoint.y + progressHeight_ - 1);
            break;
        }
        case Direction::DIR_TOP_TO_BOTTOM: {
            length = GetCurrentPos(progressHeight_ + 1);
            foregroundRect.SetRect(startPoint.x, startPoint.y, startPoint.x + progressWidth_ - 1,
                                   startPoint.y + progressHeight - 1);

            top = startPoint.y - radius - 1;
            bottom = top + length;
            coords.SetRect(startPoint.x, top, startPoint.x + progressWidth_ - 1, bottom);
            break;
        }
        case Direction::DIR_BOTTOM_TO_TOP: {
            length = GetCurrentPos(progressHeight_ + 1);
            foregroundRect.SetRect(startPoint.x, startPoint.y, startPoint.x + progressWidth_ - 1,
                                   startPoint.y + progressHeight - 1);

            bottom = startPoint.y + progressHeight + radius + 1;
            top = bottom - length;
            coords.SetRect(startPoint.x, top, startPoint.x + progressWidth_ - 1, bottom);
            break;
        }
        default: {
            GRAPHIC_LOGE("UISlider: DrawForeground direction Err!\n");
            return;
        }
    }
    if (coords.Intersect(coords, invalidatedArea)) {
        DrawValidRect(gfxDstBuffer, foregroundImage_, foregroundRect, coords, *foregroundStyle_, radius);
    }
}

void UISlider::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetOrigRect(), invalidatedArea, *style_, opaScale_);

    Rect trunc(invalidatedArea);
    if (trunc.Intersect(trunc, GetOrigRect())) {
        DrawBackground(gfxDstBuffer, trunc);
        Rect foregroundRect;
        DrawForeground(gfxDstBuffer, trunc, foregroundRect);
        DrawKnob(gfxDstBuffer, trunc, foregroundRect);
    }
}

int32_t UISlider::CalculateCurrentValue(int16_t length, int16_t totalLength)
{
    if (totalLength != 0) {
        return static_cast<int32_t>(rangeMin_ + (static_cast<int64_t>(rangeMax_) - rangeMin_) * length / totalLength);
    }
    return 0;
}

int32_t UISlider::UpdateCurrentValue(const Point& knobPosition)
{
    Point startPoint;
    Rect rect = GetOrigRect();
    // 2: Half of the gap
    startPoint.x = rect.GetLeft() + style_->borderWidth_ + style_->paddingLeft_ + (GetWidth() - progressWidth_) / 2;
    // 2: Half of the gap
    startPoint.y = rect.GetTop() + style_->borderWidth_ + style_->paddingTop_ + (GetHeight() - progressHeight_) / 2;

    int32_t value = curValue_;
    switch (direction_) {
        case Direction::DIR_LEFT_TO_RIGHT:
            if (knobPosition.x <= startPoint.x) {
                value = rangeMin_;
            } else if (knobPosition.x >= startPoint.x + progressWidth_) {
                value = rangeMax_;
            } else {
                value = CalculateCurrentValue(knobPosition.x - startPoint.x, progressWidth_);
            }
            break;
        case Direction::DIR_RIGHT_TO_LEFT:
            if (knobPosition.x <= startPoint.x) {
                value = rangeMax_;
            } else if (knobPosition.x >= startPoint.x + progressWidth_) {
                value = rangeMin_;
            } else {
                value = CalculateCurrentValue(startPoint.x + progressWidth_ - knobPosition.x, progressWidth_);
            }
            break;
        case Direction::DIR_BOTTOM_TO_TOP:
            if (knobPosition.y <= startPoint.y) {
                value = rangeMax_;
            } else if (knobPosition.y >= startPoint.y + progressHeight_) {
                value = rangeMin_;
            } else {
                value = CalculateCurrentValue(startPoint.y + progressHeight_ - knobPosition.y, progressHeight_);
            }
            break;
        case Direction::DIR_TOP_TO_BOTTOM:
            if (knobPosition.y <= startPoint.y) {
                value = rangeMin_;
            } else if (knobPosition.y >= startPoint.y + progressHeight_) {
                value = rangeMax_;
            } else {
                value = CalculateCurrentValue(knobPosition.y - startPoint.y, progressHeight_);
            }
            break;
        default:
            GRAPHIC_LOGW("UISlider::UpdateCurrentValue Direction error!\n");
    }
    SetValue(value);
    return value;
}

bool UISlider::OnClickEvent(const ClickEvent& event)
{
    Point knobPosition = event.GetCurrentPos();
    int32_t value = UpdateCurrentValue(knobPosition);
    if (listener_ != nullptr) {
        listener_->OnChange(value);
    }
    bool ret = UIView::OnClickEvent(event);
    Invalidate();
    return ret;
}

bool UISlider::OnDragEvent(const DragEvent& event)
{
    Point knobPosition = event.GetCurrentPos();
    int32_t value = UpdateCurrentValue(knobPosition);
    if (listener_ != nullptr) {
        listener_->OnChange(value);
    }
    Invalidate();
    return UIView::OnDragEvent(event);
}

bool UISlider::OnDragEndEvent(const DragEvent& event)
{
    Point knobPosition = event.GetCurrentPos();
    int32_t value = UpdateCurrentValue(knobPosition);
    if (listener_ != nullptr) {
        listener_->OnChange(value);
        listener_->OnRelease(value);
    }
    Invalidate();
    return UIView::OnDragEndEvent(event);
}

#if ENABLE_ROTATE_INPUT
bool UISlider::OnRotateEvent(const RotateEvent& event)
{
    int32_t realRotation = 0;
    cachedRotation_ += event.GetRotate() * rotateFactor_;
    realRotation = static_cast<int32_t>(cachedRotation_);
    if (realRotation == 0) {
        return UIView::OnRotateEvent(event);
    }
    cachedRotation_ = 0;
#if ENABLE_VIBRATOR
    int32_t lastValue = curValue_;
#endif
    SetValue(curValue_ + realRotation);
    if (listener_ != nullptr) {
        listener_->OnChange(curValue_);
    }
#if ENABLE_VIBRATOR
    VibratorFunc vibratorFunc = VibratorManager::GetInstance()->GetVibratorFunc();
    if (vibratorFunc != nullptr && lastValue != curValue_) {
        if (curValue_ == rangeMin_ || curValue_ == rangeMax_) {
            GRAPHIC_LOGI("UISlider::OnRotateEvent calls TYPE_THREE vibrator");
            vibratorFunc(VibratorType::VIBRATOR_TYPE_THREE);
        } else {
            int32_t changedValue = MATH_ABS(curValue_ - lastValue);
            for (int32_t i = 0; i < changedValue; i++) {
                GRAPHIC_LOGI("UISlider::OnRotateEvent calls TYPE_TWO vibrator");
                vibratorFunc(VibratorType::VIBRATOR_TYPE_TWO);
            }
        }
    }
#endif
    return UIView::OnRotateEvent(event);
}

bool UISlider::OnRotateEndEvent(const RotateEvent& event)
{
    cachedRotation_ = 0;
    return UIView::OnRotateEndEvent(event);
}
#endif
} // namespace OHOS
