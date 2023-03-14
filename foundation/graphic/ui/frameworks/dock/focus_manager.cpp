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

#include "dock/focus_manager.h"
#if ENABLE_FOCUS_MANAGER
#include "components/root_view.h"
#include "gfx_utils/graphic_math.h"

namespace OHOS {
FocusManager* FocusManager::GetInstance()
{
    static FocusManager instance;
    return &instance;
}

bool FocusManager::RequestFocus(UIView* view)
{
    if (view == nullptr || view == focusView_ || !view->IsFocusable() ||
        !view->IsVisible()) {
        return false;
    }
    lastFocusView_ = focusView_;
    focusView_ = view;

    UIViewGroup* rootView = RootView::GetInstance();
    if (RootView::FindSubView(*rootView, lastFocusView_)) {
        lastFocusView_->Blur();
    }
    focusView_->Focus();
    return true;
}

bool FocusManager::ClearFocus()
{
    if (focusView_ == nullptr) {
        return false;
    }
    lastFocusView_ = focusView_;
    focusView_->Blur();
    focusView_ = nullptr;
    return true;
}

bool FocusManager::RequestFocusByDirection(uint8_t direction)
{
    if (focusView_ != nullptr) {
        UIView* candidate = nullptr;
        if (GetNextFocus(focusView_, candidate, direction)) {
            return RequestFocus(candidate);
        }
    }
    return false;
}

bool FocusManager::CompareCandidates(UIView* focusedView, UIView*& candidate, UIView* current, uint8_t direction)
{
    if (current == focusedView) {
        return false;
    }
    bool res = false;
    switch (direction) {
        case FOCUS_DIRECTION_UP: {
            res = CompareCandidatesByUp(focusedView, candidate, current);
            break;
        }
        case FOCUS_DIRECTION_DOWN: {
            res = CompareCandidatesByDown(focusedView, candidate, current);
            break;
        }
        case FOCUS_DIRECTION_LEFT: {
            res = CompareCandidatesByLeft(focusedView, candidate, current);
            break;
        }
        case FOCUS_DIRECTION_RIGHT: {
            res = CompareCandidatesByRight(focusedView, candidate, current);
            break;
        }
        default:
            return false;
    }
    return res;
}

bool FocusManager::IsAtSameCol(const Rect& rect1, const Rect& rect2)
{
    return ((rect1.GetLeft() < rect2.GetRight()) && (rect1.GetRight() > rect2.GetLeft()));
}

bool FocusManager::IsAtSameRow(const Rect& rect1, const Rect& rect2)
{
    return ((rect1.GetBottom() < rect2.GetTop()) && (rect1.GetTop() > rect2.GetBottom()));
}

bool FocusManager::CompareCandidatesByUp(UIView* focusedView, UIView*& candidate, UIView* current)
{
    Rect focusedViewRect = focusedView->GetRect();
    Rect currentRect = current->GetRect();
    if (currentRect.GetBottom() >= focusedViewRect.GetBottom()) {
        return false;
    }
    if (candidate != nullptr) {
        Rect candidateRect = candidate->GetRect();
        if (IsAtSameCol(focusedViewRect, currentRect) && IsAtSameCol(focusedViewRect, candidateRect)) {
            return currentRect.GetBottom() > candidateRect.GetBottom();
        }
        return CompareCandidatesDistance(focusedViewRect, candidateRect, currentRect);
    }
    return true;
}

bool FocusManager::CompareCandidatesByDown(UIView* focusedView, UIView*& candidate, UIView* current)
{
    Rect focusedViewRect = focusedView->GetRect();
    Rect currentRect = current->GetRect();
    if (currentRect.GetTop() <= focusedViewRect.GetTop()) {
        return false;
    }
    if (candidate != nullptr) {
        Rect candidateRect = candidate->GetRect();
        if (IsAtSameCol(focusedViewRect, currentRect) && IsAtSameCol(focusedViewRect, candidateRect)) {
            return currentRect.GetTop() < candidateRect.GetTop();
        }
        return CompareCandidatesDistance(focusedViewRect, candidateRect, currentRect);
    }
    return true;
}

bool FocusManager::CompareCandidatesByLeft(UIView* focusedView, UIView*& candidate, UIView* current)
{
    Rect focusedViewRect = focusedView->GetRect();
    Rect currentRect = current->GetRect();
    if (currentRect.GetRight() >= focusedViewRect.GetRight()) {
        return false;
    }
    if (candidate != nullptr) {
        Rect candidateRect = candidate->GetRect();
        if (IsAtSameRow(focusedViewRect, currentRect) && IsAtSameRow(focusedViewRect, candidateRect)) {
            return currentRect.GetRight() > candidateRect.GetRight();
        }
        return CompareCandidatesDistance(focusedViewRect, candidateRect, currentRect);
    }
    return true;
}

bool FocusManager::CompareCandidatesByRight(UIView* focusedView, UIView*& candidate, UIView* current)
{
    Rect focusedViewRect = focusedView->GetRect();
    Rect currentRect = current->GetRect();
    if (currentRect.GetLeft() <= focusedViewRect.GetLeft()) {
        return false;
    }
    if (candidate != nullptr) {
        Rect candidateRect = candidate->GetRect();
        if (IsAtSameRow(focusedViewRect, currentRect) && IsAtSameRow(focusedViewRect, candidateRect)) {
            return currentRect.GetLeft() < candidateRect.GetLeft();
        }
        return CompareCandidatesDistance(focusedViewRect, candidateRect, currentRect);
    }
    return true;
}

bool FocusManager::CompareCandidatesDistance(const Rect& focused, const Rect& candidate, const Rect& current)
{
    uint64_t xDiff1 = focused.GetX() - candidate.GetX();
    uint64_t yDiff1 = focused.GetY() - candidate.GetY();
    uint64_t xDiff2 = focused.GetX() - current.GetX();
    uint64_t yDiff2 = focused.GetY() - current.GetY();
    uint64_t distance1 = xDiff1 * xDiff1 + yDiff1 * yDiff1;
    uint64_t distance2 = xDiff2 * xDiff2 + yDiff2 * yDiff2;
    return distance2 < distance1;
}

bool FocusManager::GetNextFocus(UIView* focusedView, UIView*& candidate, uint8_t direction)
{
    UIView* parent = focusedView->GetParent();
    if (parent == nullptr) {
        return false;
    }
    UIView* child = nullptr;
    bool isFoundBestCandidate = false;
    UIView* current = focusedView;
    while (parent != nullptr) {
        if (isFoundBestCandidate) {
            return true;
        }
        child = static_cast<UIViewGroup*>(parent)->GetChildrenHead();
        while (child != nullptr) {
            if (child == current) {
                child = child->GetNextSibling();
                continue;
            }
            if (child->IsViewGroup()) {
                if (GetNextFocus(focusedView, candidate, static_cast<UIViewGroup*>(child), direction)) {
                    isFoundBestCandidate = true;
                }
            } else {
                if (GetNextFocus(focusedView, candidate, child, direction)) {
                    isFoundBestCandidate = true;
                }
            }
            child = child->GetNextSibling();
        }
        if (isFoundBestCandidate) {
            return true;
        } else {
            current = parent;
        }
        parent = parent->GetParent();
    }
    return false;
}

bool FocusManager::GetNextFocus(UIView* focusedView, UIView*& candidate, UIView* view, uint8_t direction)
{
    UIView* current = view;
    if (!current->IsVisible() || !current->IsFocusable()) {
        return false;
    }
    if (CompareCandidates(focusedView, candidate, current, direction)) {
        candidate = current;
        return true;
    }
    return false;
}

bool FocusManager::GetNextFocus(UIView* focusedView, UIView*& candidate, UIViewGroup* viewGroup, uint8_t direction)
{
    UIViewGroup* current = viewGroup;
    if (current->IsInterceptFocus()) {
        return GetNextFocus(focusedView, candidate, static_cast<UIView*>(current), direction);
    }

    UIView* child = current->GetChildrenHead();
    bool childFocusable = false;
    while (child != nullptr) {
        if (child == focusedView) {
            child = child->GetNextSibling();
            continue;
        }
        if (child->IsViewGroup()) {
            if (GetNextFocus(focusedView, candidate, static_cast<UIViewGroup*>(child), direction)) {
                childFocusable = true;
            }
        } else if (GetNextFocus(focusedView, candidate, child, direction)) {
            childFocusable = true;
        }
        child = child->GetNextSibling();
    }
    return childFocusable;
}
}
#endif
