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

#include "components/ui_scroll_view.h"

#include "components/ui_abstract_scroll_bar.h"
#include "dock/focus_manager.h"
#include "dock/vibrator_manager.h"
#include "draw/draw_rect.h"
#include "gfx_utils/graphic_log.h"

namespace OHOS {
UIScrollView::UIScrollView() : scrollListener_(nullptr)
{
#if ENABLE_ROTATE_INPUT
    rotateFactor_ = DEFAULT_SCROLL_VIEW_ROTATE_FACTOR;
    rotateThrowthreshold_ = SCROLLVIEW_ROTATE_THROW_THRESHOLD;
    rotateAccCoefficient_ = SCROLLVIEW_ROTATE_DISTANCE_COEFF;
#endif
#if ENABLE_VIBRATOR
    totalRotateLen_ = 0;
    lastVibratorRotateLen_ = 0;
#endif
#if ENABLE_FOCUS_MANAGER
    focusable_ = true;
#endif
    direction_ = HORIZONTAL_AND_VERTICAL;
}

bool UIScrollView::OnDragEvent(const DragEvent& event)
{
    if (scrollAnimator_.GetState() != Animator::STOP) {
        UIAbstractScroll::StopAnimator();
    }
    Drag(event);
    return UIView::OnDragEvent(event);
}

bool UIScrollView::OnDragEndEvent(const DragEvent& event)
{
    Point last = event.GetPreLastPoint();
    Point current = event.GetLastPoint();
    if ((last.x == current.x) && (last.y == current.y)) {
        last = current;
        current = event.GetCurrentPos();
    }

    if (!DragThrowAnimator(current, last, event.GetDragDirection())) {
        if (scrollListener_ && (scrollListener_->GetScrollState() == OnScrollListener::SCROLL_STATE_MOVE)) {
            scrollListener_->OnScrollEnd();
            scrollListener_->SetScrollState(OnScrollListener::SCROLL_STATE_STOP);
        }
    }
    return UIView::OnDragEndEvent(event);
}

void UIScrollView::Drag(const DragEvent& event)
{
    int16_t xDistance = event.GetDeltaX();
    int16_t yDistance = event.GetDeltaY();

    if ((direction_ == HORIZONTAL || direction_ == HORIZONTAL_AND_VERTICAL) && xDistance != 0) {
        DragXInner(xDistance);
    }
    if ((direction_ == VERTICAL || direction_ == HORIZONTAL_AND_VERTICAL) && yDistance != 0) {
        RefreshDelta(yDistance);
        DragYInner(yDistance);
    }
}

bool UIScrollView::OnPressEvent(const PressEvent& event)
{
    StopAnimator();
    return UIView::OnPressEvent(event);
}

#if ENABLE_ROTATE_INPUT
bool UIScrollView::OnRotateEvent(const RotateEvent& event)
{
    if (direction_ == HORIZONTAL_NOR_VERTICAL) {
        return UIView::OnRotateEvent(event);
    }
    int16_t rotateLen = static_cast<int16_t>(event.GetRotate() * rotateFactor_);
#if ENABLE_VIBRATOR
    bool lastIsEdge = false;
    Rect childRect = GetAllChildRelativeRect();
    if (direction_ == HORIZONTAL) {
        if (childRect.GetLeft() - scrollBlankSize_ >= 0 || childRect.GetRight() + scrollBlankSize_ <= GetWidth()) {
            lastIsEdge = true;
        }
    } else {
        if (childRect.GetTop() - scrollBlankSize_ >= 0 || childRect.GetBottom() + scrollBlankSize_ <= GetHeight()) {
            lastIsEdge = true;
        }
    }
#endif
    RefreshRotate(rotateLen);
    if (direction_ == HORIZONTAL) {
        DragXInner(rotateLen);
    } else {
        DragYInner(rotateLen);
    }
#if ENABLE_VIBRATOR
    totalRotateLen_ += rotateLen;
    childRect = GetAllChildRelativeRect();
    bool isEdge = false;
    if (direction_ == HORIZONTAL) {
        if (childRect.GetLeft() - scrollBlankSize_ >= 0 || childRect.GetRight() + scrollBlankSize_ <= GetWidth()) {
            isEdge = true;
        }
    } else {
        if (childRect.GetTop() - scrollBlankSize_ >= 0 || childRect.GetBottom() + scrollBlankSize_ <= GetHeight()) {
            isEdge = true;
        }
    }
    VibratorFunc vibratorFunc = VibratorManager::GetInstance()->GetVibratorFunc();
    if (vibratorFunc != nullptr && !isEdge) {
        uint16_t rotateLen = MATH_ABS(totalRotateLen_ - lastVibratorRotateLen_);
        if (rotateLen > DEFAULT_SCROLL_VIEW_VIBRATION_LEN) {
            uint16_t vibrationCnt = rotateLen / DEFAULT_SCROLL_VIEW_VIBRATION_LEN;
            for (uint16_t i = 0; i < vibrationCnt; i++) {
                GRAPHIC_LOGI("UIScrollView::OnRotateEvent calls TYPE_ONE vibrator");
                vibratorFunc(VibratorType::VIBRATOR_TYPE_ONE);
            }
            lastVibratorRotateLen_ = totalRotateLen_;
        }
    }
    if (vibratorFunc != nullptr && (!lastIsEdge && isEdge)) {
        GRAPHIC_LOGI("UIScrollView::OnRotateEvent calls TYPE_THREE vibrator");
        vibratorFunc(VibratorType::VIBRATOR_TYPE_THREE);
    }
#endif
    return UIView::OnRotateEvent(event);
}

bool UIScrollView::OnRotateEndEvent(const RotateEvent& event)
{
    if (direction_ == HORIZONTAL_NOR_VERTICAL) {
        return UIView::OnRotateEvent(event);
    }
    return UIAbstractScroll::OnRotateEndEvent(event);
}
#endif

void UIScrollView::ScrollBy(int16_t xDistance, int16_t yDistance)
{
    if ((direction_ == HORIZONTAL || direction_ == HORIZONTAL_AND_VERTICAL) && xDistance != 0) {
        DragXInner(xDistance);
    }
    if ((direction_ == VERTICAL || direction_ == HORIZONTAL_AND_VERTICAL) && yDistance != 0) {
        DragYInner(yDistance);
    }
    if ((scrollListener_ != nullptr) && (scrollListener_->GetScrollState() == OnScrollListener::SCROLL_STATE_MOVE)) {
        scrollListener_->OnScrollEnd();
        scrollListener_->SetScrollState(OnScrollListener::SCROLL_STATE_STOP);
    }
}

bool UIScrollView::DragXInner(int16_t distance)
{
    Rect childRect = GetAllChildRelativeRect();
    int16_t reboundSize = reboundSize_;
    if (scrollAnimator_.GetState() != Animator::STOP) {
        reboundSize = 0;
    }

    if (childRect.GetWidth() <= (GetWidth() - (scrollBlankSize_ << 1)) ||
        !(direction_ == HORIZONTAL || direction_ == HORIZONTAL_AND_VERTICAL)) {
        return false;
    }

    if (distance > 0) {
        if (childRect.GetLeft() > scrollBlankSize_ + reboundSize) {
            distance = 0;
        } else if ((childRect.GetLeft() + distance) > scrollBlankSize_ + reboundSize) {
            distance = scrollBlankSize_ - childRect.GetLeft() + reboundSize;
        }
    } else {
        int16_t childRight = childRect.GetRight();
        int16_t scrollWidth = GetWidth();
        if (childRight < scrollWidth - (scrollBlankSize_ + reboundSize)) {
            distance = 0;
        } else if (childRight + distance < scrollWidth - (scrollBlankSize_ + reboundSize)) {
            distance = scrollWidth - (scrollBlankSize_ + reboundSize) - childRight - 1;
        }
    }

    return MoveOffset(distance, 0);
}

bool UIScrollView::DragYInner(int16_t distance)
{
    Rect childRect = GetAllChildRelativeRect();
    int16_t reboundSize = reboundSize_;
    if (scrollAnimator_.GetState() != Animator::STOP) {
        reboundSize = 0;
    }

    if (childRect.GetHeight() <= (GetHeight() - (scrollBlankSize_ << 1)) ||
        !(direction_ == VERTICAL || direction_ == HORIZONTAL_AND_VERTICAL)) {
        return false;
    }

    if (distance > 0) {
        if (childRect.GetTop() > scrollBlankSize_ + reboundSize) {
            distance = 0;
        } else if ((childRect.GetTop() + distance) > scrollBlankSize_ + reboundSize) {
            distance = scrollBlankSize_ - childRect.GetTop() + reboundSize;
        }
    } else {
        int16_t childBottom = childRect.GetBottom();
        int16_t scrollHeight = GetHeight();
        if (childBottom < scrollHeight - (scrollBlankSize_ + reboundSize)) {
            distance = 0;
        } else if (childBottom + distance < scrollHeight - (scrollBlankSize_ + reboundSize)) {
            distance = scrollHeight - (scrollBlankSize_ + reboundSize) - childBottom - 1;
        }
    }

    return MoveOffset(0, distance);
}

bool UIScrollView::MoveOffset(int16_t offsetX, int16_t offsetY)
{
    if ((offsetX != 0) || (offsetY != 0)) {
        if ((scrollListener_ != nullptr) &&
            (scrollListener_->GetScrollState() == OnScrollListener::SCROLL_STATE_STOP)) {
            scrollListener_->OnScrollStart();
            scrollListener_->SetScrollState(OnScrollListener::SCROLL_STATE_MOVE);
        }
        UIAbstractScroll::MoveChildByOffset(offsetX, offsetY);
        if (xScrollBarVisible_ || yScrollBarVisible_) {
            RefreshScrollBar();
        }
        Invalidate();
        return true;
    }
    return false;
}

void UIScrollView::RefreshScrollBar()
{
    Rect childrenRect = GetAllChildRelativeRect();
    /* calculate scrollBar's the proportion of foreground */
    int16_t totalLen = childrenRect.GetHeight() + 2 * scrollBlankSize_; // 2: two blank space on both sizes
    int16_t len = GetHeight();
    if (yScrollBarVisible_) {
        yScrollBar_->SetForegroundProportion(static_cast<float>(len) / totalLen);
        /* calculate scrolling progress */
        yScrollBar_->SetScrollProgress(static_cast<float>(scrollBlankSize_ - childrenRect.GetTop()) / (totalLen - len));
    }
    if (xScrollBarVisible_) {
        /* so do x-bar */
        totalLen = childrenRect.GetWidth() + 2 * scrollBlankSize_; // 2: two blank space on both sizes
        len = GetWidth();
        xScrollBar_->SetForegroundProportion(static_cast<float>(len) / totalLen);
        xScrollBar_->SetScrollProgress(static_cast<float>(scrollBlankSize_ - childrenRect.GetLeft()) /
                                       (totalLen - len));
    }
    RefreshAnimator();
}

void UIScrollView::CalculateReboundDistance(int16_t& dragDistanceX, int16_t& dragDistanceY)
{
    Rect rect = GetAllChildRelativeRect();
    int16_t top = rect.GetTop();
    int16_t bottom = rect.GetBottom();
    int16_t scrollHeight = GetHeight();
    int16_t left = rect.GetLeft();
    int16_t right = rect.GetRight();
    int16_t scrollWidth = GetWidth();
    if (scrollBlankSize_ < top) {
        dragDistanceY = scrollBlankSize_ - top;
    } else if (bottom < scrollHeight - 1) {
        dragDistanceY = scrollHeight - scrollBlankSize_ - bottom - 1;
    }

    if (scrollBlankSize_ < left) {
        dragDistanceX = scrollBlankSize_ - left;
    } else if (right < scrollWidth - 1) {
        dragDistanceX = scrollWidth - scrollBlankSize_ - right - 1;
    }
}

void UIScrollView::StopAnimator()
{
    if ((scrollListener_ != nullptr) && (scrollListener_->GetScrollState() == OnScrollListener::SCROLL_STATE_MOVE)) {
        scrollListener_->OnScrollEnd();
        scrollListener_->SetScrollState(OnScrollListener::SCROLL_STATE_STOP);
    }
    UIAbstractScroll::StopAnimator();
}
} // namespace OHOS
