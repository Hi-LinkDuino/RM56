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

#include "components/ui_list.h"

#include "components/ui_abstract_scroll_bar.h"
#include "gfx_utils/graphic_log.h"

namespace OHOS {
UIList::Recycle::~Recycle()
{
    ListNode<UIView*>* node = scrapView_.Begin();
    while (node != scrapView_.End()) {
        if (node->data_) {
            UIView* deleteView = node->data_;
            if (deleteView != nullptr) {
                delete deleteView;
                deleteView = nullptr;
                node->data_ = nullptr;
            }
        }
        node = node->next_;
    }
    scrapView_.Clear();
}

void UIList::Recycle::MeasureAdapterRelativeRect()
{
    uint16_t i = 0;
    if (listView_ == nullptr) {
        return;
    }
    UIView* childHead = listView_->childrenHead_;
    if (childHead == nullptr) {
        return;
    }
    uint16_t idx = childHead->GetViewIndex();
    if (listView_->direction_ == VERTICAL) {
        int32_t height = 0;
        for (; i < idx; i++) {
            height += adapter_->GetItemHeightWithMargin(i);
        }
        int16_t y = childHead->GetRelativeRect().GetTop() - height - childHead->GetStyle(STYLE_MARGIN_TOP);
        for (; i < adapter_->GetCount(); i++) {
            height += adapter_->GetItemHeightWithMargin(i);
        }
        adapterRelativeRect_.SetRect(0, y, listView_->GetWidth() - 1, y + height - 1);
    } else {
        int32_t width = 0;
        for (; i < idx; i++) {
            width += adapter_->GetItemWidthWithMargin(i);
        }
        int16_t x = childHead->GetRelativeRect().GetLeft() - width - childHead->GetStyle(STYLE_MARGIN_LEFT);
        for (; i < adapter_->GetCount(); i++) {
            width += adapter_->GetItemWidthWithMargin(i);
        }
        adapterRelativeRect_.SetRect(x, 0, x + width - 1, listView_->GetHeight() - 1);
    }
}

void UIList::Recycle::InitRecycle()
{
    if ((adapter_ == nullptr) || (listView_ == nullptr)) {
        return;
    }
    FillActiveView();
    listView_->Invalidate();
    hasInitialiszed_ = true;
    if (listView_->xScrollBarVisible_ || listView_->yScrollBarVisible_) {
        MeasureAdapterRelativeRect();
    }
}

UIView* UIList::Recycle::GetView(int16_t index)
{
    if (adapter_ == nullptr) {
        return nullptr;
    }
    UIView* inView = nullptr;
    UIView* retView = nullptr;

    if (scrapView_.Size() != 0) {
        inView = scrapView_.Back();
    }

    retView = adapter_->GetView(inView, index);
    if (retView != nullptr) {
        retView->SetViewIndex(index);
        scrapView_.PopBack();
    }
    return retView;
}

void UIList::Recycle::FillActiveView()
{
    uint16_t index = listView_->GetStartIndex();
    if (listView_->GetDirection() == UIList::VERTICAL) {
        int16_t childBottom = 0;
        while ((index < adapter_->GetCount()) && (childBottom < listView_->GetHeight())) {
            UIView* view = GetView(index);
            if (view == nullptr) {
                break;
            }
            listView_->PushBack(view);
            if (listView_->childrenTail_) {
                childBottom =
                    listView_->childrenTail_->GetY() + listView_->childrenTail_->GetRelativeRect().GetHeight();
            } else {
                break;
            }
            index++;
        }
    } else {
        int16_t childRight = 0;
        while ((index < adapter_->GetCount()) && (childRight < listView_->GetWidth())) {
            UIView* view = GetView(index);
            listView_->PushBack(view);
            if (listView_->childrenTail_) {
                childRight = listView_->childrenTail_->GetX() + listView_->childrenTail_->GetRelativeRect().GetWidth();
            } else {
                break;
            }
            index++;
        }
    }
}

Rect32 UIList::Recycle::GetAdapterItemsReletiveRect()
{
    return adapterRelativeRect_;
}

void UIList::Recycle::MoveAdapterItemsRelativeRect(int16_t x, int16_t y)
{
    auto& rect = adapterRelativeRect_;
    rect.SetPosition(rect.GetX() + x, rect.GetY() + y);
}

UIList::UIList() : UIList(VERTICAL) {}

UIList::UIList(uint8_t direction)
    : onSelectedView_(nullptr),
      isLoopList_(false),
      isReCalculateDragEnd_(true),
      autoAlign_(false),
      alignTime_(DEFAULT_ALIGN_TIMES),
      startIndex_(0),
      topIndex_(0),
      bottomIndex_(0),
      selectPosition_(0),
      onSelectedIndex_(0),
      recycle_(this),
      scrollListener_(nullptr)
{
#if ENABLE_ROTATE_INPUT
    rotateFactor_ = DEFAULT_LIST_ROTATE_FACTOR;
    rotateThrowthreshold_ = LIST_ROTATE_THROW_THRESHOLD;
    rotateAccCoefficient_ = LIST_ROTATE_DISTANCE_COEFF;
#endif
#if ENABLE_FOCUS_MANAGER
    focusable_ = true;
#endif
    direction_ = direction;
    touchable_ = true;
    draggable_ = true;
    dragParentInstead_ = false;
}

UIList::~UIList()
{
    UIView* view = GetChildrenHead();
    while (view != nullptr) {
        UIView* tmp = view->GetNextSibling();
        UIViewGroup::Remove(view);
        delete view;
        view = tmp;
    }
}

bool UIList::OnDragEvent(const DragEvent& event)
{
    if (scrollAnimator_.GetState() != Animator::STOP) {
        UIAbstractScroll::StopAnimator();
    }
    isReCalculateDragEnd_ = true;
    int16_t xDistance = event.GetDeltaX();
    int16_t yDistance = event.GetDeltaY();
    if (direction_ == VERTICAL) {
        RefreshDelta(yDistance);
        DragYInner(yDistance);
    } else {
        RefreshDelta(xDistance);
        DragXInner(xDistance);
    }
    return UIView::OnDragEvent(event);
}

bool UIList::OnDragEndEvent(const DragEvent& event)
{
    Point last = event.GetPreLastPoint();
    Point current = event.GetLastPoint();
    if ((last.x == current.x) && (last.y == current.y)) {
        last = current;
        current = event.GetCurrentPos();
    }
    isReCalculateDragEnd_ = false;
    if (!DragThrowAnimator(current, last, event.GetDragDirection(), dragBack_)) {
        if (scrollListener_ && (scrollListener_->GetScrollState() == ListScrollListener::SCROLL_STATE_MOVE)) {
            scrollListener_->SetScrollState(ListScrollListener::SCROLL_STATE_STOP);
            scrollListener_->OnScrollEnd(onSelectedIndex_, onSelectedView_);
        }
    }
    return UIView::OnDragEndEvent(event);
}

bool UIList::OnPressEvent(const PressEvent& event)
{
    StopAnimator();
    return UIView::OnPressEvent(event);
}

#if ENABLE_ROTATE_INPUT
bool UIList::OnRotateStartEvent(const RotateEvent& event)
{
    isReCalculateDragEnd_ = true;
    return UIAbstractScroll::OnRotateStartEvent(event);
}

bool UIList::OnRotateEndEvent(const RotateEvent& event)
{
    isReCalculateDragEnd_ = false;
    return UIAbstractScroll::OnRotateEndEvent(event);
}
#endif

void UIList::ScrollBy(int16_t distance)
{
    if (direction_ == VERTICAL) {
        DragYInner(distance);
    } else {
        DragXInner(distance);
    }
    if (scrollListener_ && (scrollListener_->GetScrollState() == ListScrollListener::SCROLL_STATE_MOVE)) {
        scrollListener_->SetScrollState(ListScrollListener::SCROLL_STATE_STOP);
        scrollListener_->OnScrollEnd(onSelectedIndex_, onSelectedView_);
    }
}

bool UIList::DragXInner(int16_t distance)
{
    if (IsNeedReCalculateDragEnd()) {
        return false;
    }
    int16_t listWidth = GetWidth();
    if (distance == 0) {
        return true;
    }
    int16_t reboundSize = reboundSize_;
    if (isLoopList_ || (scrollAnimator_.GetState() != Animator::STOP)) {
        reboundSize = 0;
    }
    bool ret = 0;
    do {
        ret = MoveChildStep(distance);
    } while (ret);

    if (isLoopList_) {
        return MoveOffset(distance, 0);
    }
    if (distance > 0) {
        if (childrenHead_ && ((childrenHead_->GetX() + distance) > (scrollBlankSize_ + reboundSize))) {
            distance = scrollBlankSize_ + reboundSize - childrenHead_->GetX();
        }
    } else {
        if (childrenTail_) {
            if (childrenTail_->GetRelativeRect().GetRight() + childrenTail_->GetStyle(STYLE_MARGIN_RIGHT) <
                listWidth - scrollBlankSize_ - reboundSize) {
                distance = 0;
            } else if ((childrenTail_->GetRelativeRect().GetRight() + childrenTail_->GetStyle(STYLE_MARGIN_RIGHT) +
                        distance) <= (listWidth - scrollBlankSize_ - reboundSize)) {
                distance = listWidth - scrollBlankSize_ - reboundSize - childrenTail_->GetRelativeRect().GetRight() -
                           childrenTail_->GetStyle(STYLE_MARGIN_RIGHT) - 1;
            }
        }
    }
    return MoveOffset(distance, 0);
}

bool UIList::DragYInner(int16_t distance)
{
    if (IsNeedReCalculateDragEnd()) {
        return false;
    }
    int16_t listHeight = GetHeight();
    if (distance == 0) {
        return true;
    }
    int16_t reboundSize = reboundSize_;
    if (isLoopList_ || (scrollAnimator_.GetState() != Animator::STOP)) {
        reboundSize = 0;
    }
    bool ret = 0;
    do {
        ret = MoveChildStep(distance);
    } while (ret);

    if (isLoopList_) {
        return MoveOffset(0, distance);
    }
    if (distance > 0) {
        if (childrenHead_ && ((childrenHead_->GetY() + distance) > (scrollBlankSize_ + reboundSize))) {
            distance = scrollBlankSize_ + reboundSize - childrenHead_->GetY();
        }
    } else {
        if (childrenTail_) {
            if (childrenTail_->GetRelativeRect().GetBottom() + childrenTail_->GetStyle(STYLE_MARGIN_BOTTOM) <
                listHeight - scrollBlankSize_ - reboundSize) {
                distance = 0;
            } else if ((childrenTail_->GetRelativeRect().GetBottom() + childrenTail_->GetStyle(STYLE_MARGIN_BOTTOM) +
                        distance) <= (listHeight - scrollBlankSize_ - reboundSize)) {
                distance = listHeight - scrollBlankSize_ - reboundSize - childrenTail_->GetRelativeRect().GetBottom() -
                           childrenTail_->GetStyle(STYLE_MARGIN_BOTTOM) - 1;
            }
        }
    }
    return MoveOffset(0, distance);
}

bool UIList::MoveOffset(int16_t x, int16_t y)
{
    if ((x == 0) && (y == 0)) {
        return false;
    }
    MoveChildByOffset(x, y);
    if (xScrollBarVisible_ || yScrollBarVisible_) {
        recycle_.MoveAdapterItemsRelativeRect(x, y);
        UpdateScrollBar();
    }
    Invalidate();
    if (scrollListener_ && (scrollListener_->GetScrollState() == ListScrollListener::SCROLL_STATE_STOP)) {
        scrollListener_->SetScrollState(ListScrollListener::SCROLL_STATE_MOVE);
        scrollListener_->OnScrollStart(onSelectedIndex_, onSelectedView_);
    }

    if (!isLoopList_ && scrollListener_) {
        if (direction_ == VERTICAL) {
            if (childrenHead_ && (childrenHead_->GetViewIndex() == 0) &&
                childrenHead_->GetRelativeRect().GetTop() >= 0 && childrenHead_->GetRelativeRect().GetTop() - y < 0) {
                scrollListener_->OnScrollTop(childrenHead_->GetViewIndex(), childrenHead_);
            }
            if (childrenTail_ && (childrenTail_->GetViewIndex() == recycle_.GetAdapterItemCount() - 1) &&
                (childrenTail_->GetRelativeRect().GetBottom() <= GetContentRect().GetHeight() - 1) &&
                (childrenTail_->GetRelativeRect().GetBottom() - y > GetContentRect().GetHeight() - 1)) {
                scrollListener_->OnScrollBottom(childrenTail_->GetViewIndex(), childrenTail_);
            }
        } else {
            if (childrenHead_ && (childrenHead_->GetViewIndex() == 0) &&
                childrenHead_->GetRelativeRect().GetLeft() >= 0 && childrenHead_->GetRelativeRect().GetLeft() - x < 0) {
                scrollListener_->OnScrollTop(childrenHead_->GetViewIndex(), childrenHead_);
            }
            if (childrenTail_ && (childrenTail_->GetViewIndex() == recycle_.GetAdapterItemCount() - 1) &&
                (childrenTail_->GetRelativeRect().GetRight() <= GetContentRect().GetWidth() - 1) &&
                (childrenTail_->GetRelativeRect().GetRight() - x > GetContentRect().GetWidth() - 1)) {
                scrollListener_->OnScrollBottom(childrenTail_->GetViewIndex(), childrenTail_);
            }
        }
    }
    return true;
}

void UIList::UpdateScrollBar()
{
    auto allItemsRect = recycle_.GetAdapterItemsReletiveRect();
    float totalHeight = allItemsRect.GetHeight() + 2.0f * scrollBlankSize_; // 2: two blank spaces on both sides
    int16_t height = GetHeight();
    yScrollBar_->SetForegroundProportion(height / totalHeight);
    yScrollBar_->SetScrollProgress((scrollBlankSize_ - allItemsRect.GetTop()) /
                                   (totalHeight - height));
    RefreshAnimator();
}

bool UIList::IsNeedReCalculateDragEnd()
{
    if (!autoAlign_ || isReCalculateDragEnd_ || (onSelectedView_ == nullptr)) {
        return false;
    }
    int16_t animationLess = 0;
    if (direction_ == VERTICAL) {
        animationLess = animatorCallback_.endValueY_ - animatorCallback_.previousValueY_;
    } else {
        animationLess = animatorCallback_.endValueX_ - animatorCallback_.previousValueX_;
    }
    if (!isDragging_ || (MATH_ABS(animationLess) > RECALCULATE_DRAG_DISTANCE)) {
        return false;
    }
    return true;
}
bool UIList::ReCalculateDragEnd()
{
    if ((onSelectedView_ == nullptr) || isReCalculateDragEnd_ || !autoAlign_) {
        return false;
    }

    int16_t offsetX = 0;
    int16_t offsetY = 0;
    if (direction_ == VERTICAL) {
        // 2: half
        offsetY = selectPosition_ - (onSelectedView_->GetY() + (onSelectedView_->GetRelativeRect().GetHeight() / 2));
    } else {
        // 2: half
        offsetX = selectPosition_ - (onSelectedView_->GetX() + (onSelectedView_->GetRelativeRect().GetWidth() / 2));
    }
    animatorCallback_.ResetCallback();
    animatorCallback_.SetDragStartValue(0, 0);
    animatorCallback_.SetDragEndValue(offsetX, offsetY);
    animatorCallback_.SetDragTimes(GetAutoAlignTime() / DEFAULT_TASK_PERIOD);
    scrollAnimator_.Start();
    isReCalculateDragEnd_ = true;
    return true;
}

bool UIList::MoveChildStepVertical(int16_t distance)
{
    bool popRet = false;
    bool pushRet = false;
    if (distance > 0) {
        if ((childrenHead_ == nullptr) || (childrenHead_->GetRelativeRect().GetTop() + distance > 0)) {
            uint16_t index = GetIndexDec(topIndex_);
            if (index == topIndex_) {
                return false;
            }
            UIView* newView = recycle_.GetView(index);
            if (newView == nullptr) {
                return false;
            }
            PushFront(newView);
            pushRet = true;
        }
        if (childrenTail_ != nullptr && (childrenTail_->GetRelativeRect().GetTop() + distance > GetHeight())) {
            PopItem(childrenTail_);
            popRet = true;
        }
    } else {
        if (childrenTail_ == nullptr || (childrenTail_->GetRelativeRect().GetBottom() + distance < GetHeight())) {
            UIView* newView = recycle_.GetView(GetIndexInc(bottomIndex_));
            if (newView == nullptr) {
                return false;
            }
            PushBack(newView);
            pushRet = true;
        }
        if (childrenHead_ && (childrenHead_->GetRelativeRect().GetBottom() + distance < 0)) {
            PopItem(childrenHead_);
            popRet = true;
        }
    }
    return (popRet || pushRet);
}

bool UIList::MoveChildStepHorizontal(int16_t distance)
{
    bool popRet = false;
    bool pushRet = false;
    if (distance > 0) {
        if ((childrenHead_ == nullptr) || (childrenHead_->GetRelativeRect().GetLeft() + distance > 0)) {
            uint16_t index = GetIndexDec(topIndex_);
            if (index == topIndex_) {
                return false;
            }
            UIView* newView = recycle_.GetView(index);
            if (newView == nullptr) {
                return false;
            }
            PushFront(newView);
            pushRet = true;
        }
        if (childrenTail_ != nullptr && (childrenTail_->GetRelativeRect().GetLeft() + distance > GetWidth())) {
            PopItem(childrenTail_);
            popRet = true;
        }
    } else {
        if (childrenTail_ == nullptr || (childrenTail_->GetRelativeRect().GetRight() + distance < GetWidth())) {
            UIView* newView = recycle_.GetView(GetIndexInc(bottomIndex_));
            if (newView == nullptr) {
                return false;
            }
            PushBack(newView);
            pushRet = true;
        }
        if (childrenHead_ && (childrenHead_->GetRelativeRect().GetRight() + distance < 0)) {
            PopItem(childrenHead_);
            popRet = true;
        }
    }
    return (popRet || pushRet);
}

bool UIList::MoveChildStep(int16_t distance)
{
    if (direction_ == VERTICAL) {
        return MoveChildStepVertical(distance);
    } else {
        return MoveChildStepHorizontal(distance);
    }
}

void UIList::SetAdapter(AbstractAdapter* adapter)
{
    recycle_.SetAdapter(adapter);
    recycle_.InitRecycle();
}

UIView* UIList::GetSelectView()
{
    if (onSelectedView_ != nullptr) {
        return onSelectedView_;
    }
    if ((childrenHead_ == nullptr) || (selectPosition_ == 0)) {
        return nullptr;
    }
    UIView* child = childrenHead_;
    while (child != nullptr) {
        if (direction_ == VERTICAL) {
            if ((child->GetY() <= selectPosition_) &&
                (child->GetY() + child->GetRelativeRect().GetHeight() >= selectPosition_)) {
                if (scrollListener_ != nullptr) {
                    scrollListener_->OnItemSelected(child->GetViewIndex(), child);
                }
                return child;
            }
        } else {
            if ((child->GetX() <= selectPosition_) &&
                (child->GetX() + child->GetRelativeRect().GetWidth() >= selectPosition_)) {
                if (scrollListener_ != nullptr) {
                    scrollListener_->OnItemSelected(child->GetViewIndex(), child);
                }
                return child;
            }
        }
        child = child->GetNextSibling();
    }
    return nullptr;
}

void UIList::PushBack(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    if (childrenTail_ == nullptr) {
        SetHead(view);
    } else {
        if (direction_ == VERTICAL) {
            view->SetPosition(0, childrenTail_->GetY() + childrenTail_->GetHeightWithMargin());
        } else {
            view->SetPosition(childrenTail_->GetX() + childrenTail_->GetWidthWithMargin(), 0);
        }
        bottomIndex_ = GetIndexInc(bottomIndex_);
    }

    view->SetDragParentInstead(true);
    UIViewGroup::Add(view);
}

void UIList::PushFront(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    if (GetChildrenHead() == nullptr) {
        SetHead(view);
    } else {
        if (direction_ == VERTICAL) {
            view->SetPosition(0, GetChildrenHead()->GetY() - view->GetHeightWithMargin());
        } else {
            view->SetPosition(GetChildrenHead()->GetX() - view->GetWidthWithMargin(), 0);
        }
        topIndex_ = GetIndexDec(topIndex_);
    }
    view->SetDragParentInstead(true);
    UIViewGroup::Insert(nullptr, view);
}

void UIList::PopItem(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    recycle_.AddScrapView(view);
    if (view == GetChildrenHead()) {
        topIndex_ = GetIndexInc(topIndex_);
    }

    if (view == childrenTail_) {
        bottomIndex_ = GetIndexDec(bottomIndex_);
    }
    UIViewGroup::Remove(view);
}

void UIList::SetHead(UIView* view)
{
    if (view != nullptr) {
        view->SetPosition(0, 0);
        topIndex_ = startIndex_;
        bottomIndex_ = startIndex_;
    }
}

void UIList::MoveChildByOffset(int16_t xOffset, int16_t yOffset)
{
    UIView* view = GetChildrenHead();
    if (view == nullptr) {
        return;
    }
    int16_t x;
    int16_t y;
    int16_t height;
    int16_t width;

    if ((onSelectedIndex_ != NULL_SELECT_INDEX) && (selectPosition_ != 0)) {
        if (yOffset != 0) {
            height = view->GetRelativeRect().GetHeight();
            if ((view->GetY() + yOffset > selectPosition_) ||
                (childrenTail_->GetY() + height + childrenTail_->GetStyle(STYLE_MARGIN_BOTTOM) + yOffset <
                 selectPosition_)) {
                onSelectedIndex_ = NULL_SELECT_INDEX;
                onSelectedView_ = nullptr;
                if (scrollListener_ != nullptr) {
                    scrollListener_->OnItemSelected(onSelectedIndex_, onSelectedView_);
                }
            }
        }
        if (xOffset != 0) {
            width = view->GetRelativeRect().GetWidth();
            if ((view->GetX() + xOffset > selectPosition_) ||
                (childrenTail_->GetX() + width + childrenTail_->GetStyle(STYLE_MARGIN_RIGHT) < selectPosition_)) {
                onSelectedIndex_ = NULL_SELECT_INDEX;
                onSelectedView_ = nullptr;
                if (scrollListener_ != nullptr) {
                    scrollListener_->OnItemSelected(onSelectedIndex_, onSelectedView_);
                }
            }
        }
    }
    bool isSelectViewFind = false;
    do {
        x = view->GetX() + xOffset;
        y = view->GetY() + yOffset;
        view->SetPosition(x, y);
        if ((selectPosition_ != 0) && !isSelectViewFind) {
            if (direction_ == VERTICAL) {
                height = view->GetRelativeRect().GetHeight();
                /* Views may be the same but have different indexes because of view recycling. */
                if ((y - view->GetStyle(STYLE_PADDING_TOP) <= selectPosition_) &&
                    (y + view->GetStyle(STYLE_MARGIN_BOTTOM) + height >= selectPosition_) &&
                    ((onSelectedView_ != view) || (onSelectedIndex_ != view->GetViewIndex()))) {
                    onSelectedIndex_ = view->GetViewIndex();
                    onSelectedView_ = view;
                    if (scrollListener_ != nullptr) {
                        scrollListener_->OnItemSelected(onSelectedIndex_, onSelectedView_);
                    }
                    isSelectViewFind = true;
                }
            } else {
                width = view->GetRelativeRect().GetWidth();
                if ((x - view->GetStyle(STYLE_MARGIN_LEFT) <= selectPosition_) &&
                    (x + width + view->GetStyle(STYLE_MARGIN_RIGHT) >= selectPosition_) &&
                    ((onSelectedView_ != view) || (onSelectedIndex_ != view->GetViewIndex()))) {
                    onSelectedIndex_ = view->GetViewIndex();
                    onSelectedView_ = view;
                    if (scrollListener_ != nullptr) {
                        scrollListener_->OnItemSelected(onSelectedIndex_, onSelectedView_);
                    }
                    isSelectViewFind = true;
                }
            }
#if ENABLE_VIBRATOR
            VibratorFunc vibratorFunc = VibratorManager::GetInstance()->GetVibratorFunc();
            if (isSelectViewFind && isRotating_ && vibratorFunc != nullptr) {
                if (!isLoopList_ && (onSelectedIndex_ == 0 || onSelectedIndex_ == recycle_.adapter_->GetCount() - 1)) {
                    vibratorFunc(VibratorType::VIBRATOR_TYPE_THREE);
                    GRAPHIC_LOGI("UIList::MoveChildByOffset calls TYPE_THREE vibrator");
                } else {
                    vibratorFunc(VibratorType::VIBRATOR_TYPE_TWO);
                    GRAPHIC_LOGI("UIList::MoveChildByOffset calls TYPE_TWO vibrator");
                }
            }
#endif
        }
        view = view->GetNextSibling();
    } while (view != nullptr);
}

void UIList::StopAnimator()
{
    UIAbstractScroll::StopAnimator();
    if (!ReCalculateDragEnd()) {
        if ((scrollListener_ != nullptr) &&
            (scrollListener_->GetScrollState() == ListScrollListener::SCROLL_STATE_MOVE)) {
            scrollListener_->SetScrollState(ListScrollListener::SCROLL_STATE_STOP);
            scrollListener_->OnScrollEnd(onSelectedIndex_, onSelectedView_);
        }
    }
}

uint16_t UIList::GetIndexInc(uint16_t index)
{
    uint16_t ret = index + 1;
    if (isLoopList_ && (recycle_.GetAdapterItemCount() != 0)) {
        ret = ret % recycle_.GetAdapterItemCount();
    }
    return ret;
}

uint16_t UIList::GetIndexDec(uint16_t index)
{
    if (index == 0) {
        if (isLoopList_) {
            return recycle_.GetAdapterItemCount() - 1;
        } else {
            return 0;
        }
    } else {
        return index - 1;
    }
}

void UIList::ScrollTo(uint16_t index)
{
    UIView* child = GetChildrenHead();
    UIView* tmp = nullptr;
    while (child != nullptr) {
        tmp = child;
        child = child->GetNextSibling();
        PopItem(tmp);
    }
    onSelectedView_ = nullptr;
    SetStartIndex(index);
    recycle_.InitRecycle();
}

void UIList::RefreshList()
{
    int16_t topIndex = topIndex_;
    UIView* child = GetChildrenHead();
    UIView* tmp = nullptr;
    int16_t offset = 0;
    if (child != nullptr) {
        if (direction_ == VERTICAL) {
            offset = child->GetY();
        } else {
            offset = child->GetX();
        }
    }

    while (child != nullptr) {
        tmp = child;
        child = child->GetNextSibling();
        PopItem(tmp);
    }
    onSelectedView_ = nullptr;

    uint16_t tmpStartIndex = startIndex_;
    if (topIndex > recycle_.GetAdapterItemCount() - 1) {
        startIndex_ = 0;
        offset = 0;
    } else {
        startIndex_ = topIndex;
    }
    recycle_.InitRecycle();
    startIndex_ = tmpStartIndex;

    if (direction_ == VERTICAL) {
        DragYInner(offset);
    } else {
        DragXInner(offset);
    }
    Invalidate();
}

void UIList::RemoveAll()
{
    UIViewGroup::RemoveAll();
    recycle_.ClearScrapView();
}

void UIList::SetXScrollBarVisible(bool visible)
{
    bool lastVisible = xScrollBarVisible_;
    UIAbstractScroll::SetXScrollBarVisible(visible);
    if (!lastVisible && xScrollBarVisible_) {
        if (recycle_.HasInitialiszed()) {
            recycle_.MeasureAdapterRelativeRect();
        } else {
            recycle_.InitRecycle();
        }
    }
}

void UIList::SetYScrollBarVisible(bool visible)
{
    bool lastVisible = yScrollBarVisible_;
    UIAbstractScroll::SetYScrollBarVisible(visible);
    if (!lastVisible && yScrollBarVisible_) {
        if (recycle_.HasInitialiszed()) {
            recycle_.MeasureAdapterRelativeRect();
        } else {
            recycle_.InitRecycle();
        }
    }
}

void UIList::CalculateReboundDistance(int16_t& dragDistanceX, int16_t& dragDistanceY)
{
    if (isLoopList_) {
        return;
    }
    Rect rect = GetAllChildRelativeRect();
    int16_t top = rect.GetTop();
    int16_t bottom = rect.GetBottom();
    int16_t scrollHeight = GetHeight();
    int16_t left = rect.GetLeft();
    int16_t right = rect.GetRight();
    int16_t scrollWidth = GetWidth();
    if ((direction_ == VERTICAL) || (direction_ == HORIZONTAL_AND_VERTICAL)) {
        if (top > scrollBlankSize_) {
            if ((dragDistanceY + top) > (scrollBlankSize_ + reboundSize_)) {
                dragDistanceY = 0;
            }
            dragDistanceY += scrollBlankSize_ - (top + dragDistanceY);
        }
        if (bottom < (scrollHeight - scrollBlankSize_ - 1)) {
            if ((dragDistanceY + bottom) < (scrollHeight - scrollBlankSize_ - reboundSize_ - 1)) {
                dragDistanceY = 0;
            }
            dragDistanceY += scrollHeight - scrollBlankSize_ - 1 - (bottom + dragDistanceY);
        }
    } else {
        if (left > scrollBlankSize_) {
            if ((dragDistanceX + left) > (scrollBlankSize_ + reboundSize_)) {
                dragDistanceX = 0;
            }
            dragDistanceX += scrollBlankSize_ - (left + dragDistanceX);
        }
        if (right < (scrollWidth - scrollBlankSize_ - 1)) {
            if ((dragDistanceX + right) < (scrollWidth - scrollBlankSize_ - reboundSize_ - 1)) {
                dragDistanceX = 0;
            }
            dragDistanceX += scrollWidth - scrollBlankSize_ - 1 - (right + dragDistanceX);
        }
    }
}

/* this is a temporary implementation just used for list and will be replaced later,
   we assume size of all items in scroll are equal for now. */
void UIList::FixDistance(int16_t& distanceX, int16_t& distanceY)
{
    if (childrenHead_ == nullptr) {
        GRAPHIC_LOGW("cannot fix drag distance without children!");
        return;
    }

    if (direction_ == VERTICAL) {
        FixVerDistance(distanceY);
    } else {
        FixHorDistance(distanceX);
    }
}

void UIList::FixHorDistance(int16_t& distanceX)
{
    UIView* targetView = childrenHead_;
    while (targetView != nullptr) {
        int16_t pos = targetView->GetX();
        int16_t width = targetView->GetRelativeRect().GetWidth();
        if (width == 0) {
            return;
        }
        if (pos <= selectPosition_ && pos + width >= selectPosition_) {
            int16_t offset = selectPosition_ - (pos + width / 2); // 2 : half of width
            if ((distanceX < 0) && (offset > 0)) {
                offset = offset - width;
            } else if ((distanceX > 0) && (offset < 0)) {
                offset = offset + width;
            }
            distanceX = (distanceX / width) * width + offset;
            return;
        }
        targetView = targetView->GetNextSibling();
    }
}

void UIList::FixVerDistance(int16_t& distanceY)
{
    UIView* targetView = childrenHead_;
    while (targetView != nullptr) {
        int16_t pos = targetView->GetY();
        int16_t height = targetView->GetRelativeRect().GetHeight();
        if (height == 0) {
            return;
        }
        if (pos <= selectPosition_ && pos + height >= selectPosition_) {
            int16_t offset = selectPosition_ - (pos + height / 2); // 2 : half of height
            if ((distanceY < 0) && (offset > 0)) {
                offset = offset - height;
            } else if ((distanceY > 0) && (offset < 0)) {
                offset = offset + height;
            }
            distanceY = (distanceY / height) * height + offset;
            return;
        }
        targetView = targetView->GetNextSibling();
    }
}
} // namespace OHOS
