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

#include "components/ui_view_group.h"

#include <cstring>

#include "components/root_view.h"
#include "gfx_utils/graphic_log.h"

namespace OHOS {
UIViewGroup::UIViewGroup()
    : childrenHead_(nullptr),
      childrenTail_(nullptr),
      childrenNum_(0),
      isDragging_(false),
      disallowIntercept_(false),
      isAutoSize_(false)
{
    isViewGroup_ = true;
#if ENABLE_FOCUS_MANAGER
    isInterceptFocus_ = false;
#endif
}

UIViewGroup::~UIViewGroup() {}

void UIViewGroup::Add(UIView* view)
{
    if ((view == this) || (view == nullptr)) {
        GRAPHIC_LOGE("view can not be nullptr and added to self");
        return;
    }
    if (view->GetParent() != nullptr) {
        GRAPHIC_LOGE("can not add view multi times");
        return;
    }

    if (childrenHead_ == nullptr) {
        childrenHead_ = view;
    } else {
        if (childrenTail_ == nullptr) {
            return;
        }
        childrenTail_->SetNextSibling(view);
    }
    view->SetParent(this);
    view->SetNextSibling(nullptr);
    childrenTail_ = view;
    childrenNum_++;
    if (isAutoSize_) {
        AutoResize();
    }
    OnChildChanged();
}

void UIViewGroup::Insert(UIView* prevView, UIView* insertView)
{
    if ((insertView == nullptr) || (insertView == this)) {
        GRAPHIC_LOGE("insertView can not be nullptr and insert to self");
        return;
    }

    if (insertView->GetParent() != nullptr) {
        GRAPHIC_LOGE("can not insert view multi times");
        return;
    }

    if (childrenHead_ == nullptr) {
        Add(insertView);
        return;
    }

    if (prevView == nullptr) {
        insertView->SetNextSibling(childrenHead_);
        insertView->SetParent(this);
        childrenHead_ = insertView;
    } else {
        UIView* nextView = prevView->GetNextSibling();
        prevView->SetNextSibling(insertView);
        insertView->SetNextSibling(nextView);
        insertView->SetParent(this);
    }
    if (childrenTail_ == prevView) {
        childrenTail_ = insertView;
    }
    childrenNum_++;
    if (isAutoSize_) {
        AutoResize();
    }
    OnChildChanged();
}

void UIViewGroup::Remove(UIView* view)
{
    if ((childrenHead_ == nullptr) || (view == nullptr)) {
        return;
    }

#if LOCAL_RENDER
    RootView::GetInstance()->RemoveViewFromInvalidMap(view);
    InvalidateRect(view->GetRect());
#endif
    if (childrenHead_ == view) {
        childrenHead_ = childrenHead_->GetNextSibling();
        view->SetParent(nullptr);
        view->SetNextSibling(nullptr);
        if (childrenTail_ == view) {
            childrenTail_ = nullptr;
        }
        childrenNum_--;
        OnChildChanged();
        return;
    }
    UIView* node = childrenHead_;
    while (node->GetNextSibling() != nullptr) {
        if (node->GetNextSibling() == view) {
            node->SetNextSibling(view->GetNextSibling());
            view->SetParent(nullptr);
            view->SetNextSibling(nullptr);
            if (childrenTail_ == view) {
                childrenTail_ = node;
            }
            childrenNum_--;
            OnChildChanged();
            return;
        }
        node = node->GetNextSibling();
    }
}

void UIViewGroup::RemoveAll()
{
    UIView* node = childrenHead_;
    childrenHead_ = nullptr;
    childrenTail_ = nullptr;
    childrenNum_ = 0;
    UIView* tmp = nullptr;
    while (node != nullptr) {
        tmp = node;
        node = node->GetNextSibling();
        tmp->SetParent(nullptr);
        tmp->SetNextSibling(nullptr);
    }
    OnChildChanged();
}

void UIViewGroup::GetTargetView(const Point& point, UIView** last)
{
    if (last == nullptr) {
        return;
    }

    Rect rect = GetRect();
    if (disallowIntercept_) {
        *last = nullptr;
        return;
    }
    if (!rect.IsContains(point)) {
        return;
    }
    if (!visible_) {
        return;
    }
    if (touchable_) {
        *last = this;
    }
    if (isDragging_) {
        return;
    }
    UIView* view = childrenHead_;
    while (view != nullptr) {
        if (!view->IsViewGroup()) {
            rect = view->GetRect();
            if (rect.IsContains(point)) {
                view->GetTargetView(point, last);
            }
        } else {
            UIViewGroup* viewGroup = static_cast<UIViewGroup*>(view);
            viewGroup->GetTargetView(point, last);
        }
        view = view->GetNextSibling();
    }
}

void UIViewGroup::GetTargetView(const Point& point, UIView** current, UIView** target)
{
    if ((current == nullptr) || (target == nullptr)) {
        return;
    }

    Rect rect = GetRect();
    if (disallowIntercept_) {
        *current = nullptr;
        *target = nullptr;
        return;
    }
    if (!rect.IsContains(point)) {
        return;
    }
    if (!visible_) {
        return;
    }
    *target = this;
    if (touchable_) {
        *current = this;
    }
    if (isDragging_) {
        return;
    }
    UIView* view = childrenHead_;
    while (view != nullptr) {
        if (!view->IsViewGroup()) {
            rect = view->GetRect();
            if (rect.IsContains(point)) {
                view->GetTargetView(point, current, target);
            }
        } else {
            UIViewGroup* viewGroup = static_cast<UIViewGroup*>(view);
            viewGroup->GetTargetView(point, current, target);
        }
        view = view->GetNextSibling();
    }
}

Rect UIViewGroup::GetAllChildRelativeRect() const
{
    Rect rect = {0, 0, 0, 0};
    UIView* view = childrenHead_;
    bool isRectValid = false;
    while (view != nullptr) {
        if (!view->IsVisible()) {
            view = view->GetNextSibling();
            continue;
        }
        Rect rectChild = view->GetRelativeRect();
        if (!isRectValid) {
            rect = rectChild;
            isRectValid = true;
        } else {
            rect.Join(rect, rectChild);
        }
        view = view->GetNextSibling();
    }
    return rect;
}

UIView* UIViewGroup::GetChildById(const char* id) const
{
    if (id == nullptr || childrenHead_ == nullptr) {
        return nullptr;
    }
    UIView* child = childrenHead_;
    while (child != nullptr) {
        if ((child->GetViewId() != nullptr) && !strcmp(child->GetViewId(), id)) {
            return child;
        } else if (child->IsViewGroup() && static_cast<UIViewGroup*>(child)->GetChildrenHead() != nullptr) {
            child = static_cast<UIViewGroup*>(child)->GetChildrenHead();
            continue;
        } else if (child->GetNextSibling() != nullptr) {
            child = child->GetNextSibling();
            continue;
        }
        while (child->GetParent() != this && child->GetParent()->GetNextSibling() == nullptr) {
            child = child->GetParent();
        }
        if (child->GetParent() != this) {
            child = child->GetParent()->GetNextSibling();
            continue;
        }
        break;
    }
    return nullptr;
}

void UIViewGroup::MoveChildByOffset(int16_t xOffset, int16_t yOffset)
{
    UIView* view = childrenHead_;
    int16_t x;
    int16_t y;
    while (view != nullptr) {
        x = view->GetX() + xOffset;
        y = view->GetY() + yOffset;
        view->SetPosition(x, y);
        view = view->GetNextSibling();
    }
}

void UIViewGroup::AutoResize()
{
    Rect rect = GetAllChildRelativeRect();
    SetWidth(rect.GetWidth() + rect.GetLeft());
    SetHeight(rect.GetHeight() + rect.GetTop());
}
} // namespace OHOS
