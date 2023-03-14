/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "core/components/list/list_item_element.h"

#include "core/components/list/list_element.h"
#include "core/components/list/list_item_component.h"
#include "core/components/list/list_item_group_element.h"
#include "core/components/list/render_list_item.h"

namespace OHOS::Ace {

RefPtr<RenderNode> ListItemElement::CreateRenderNode()
{
    SetOnFocusCallback([weak = WeakClaim(this)](void) {
        auto element = weak.Upgrade();
        if (element) {
            element->HandleOnFocus();
        }
    });
    SetOnBlurCallback([weak = WeakClaim(this)](void) {
        auto element = weak.Upgrade();
        if (element) {
            element->HandleOnBlur();
        }
    });
    SetOnClickCallback([weak = WeakClaim(this)](void) {
        auto element = weak.Upgrade();
        if (element) {
            element->HandleOnClick();
        }
    });
    return SoleChildElement::CreateRenderNode();
}

void ListItemElement::Update()
{
    auto item = AceType::DynamicCast<ListItemComponent>(component_);
    listItemComponent_ = item;
    if (item && (key_ == -1 || key_ == item->GetKey())) {
        type_ = item->GetType();
        flags_ = item->GetFlags();
        index_ = item->GetIndex();
        key_ = item->GetKey();
        topLeftRadius_ = item->GetTopLeftRadius();
        topRightRadius_ = item->GetTopRightRadius();
        bottomLeftRadius_ = item->GetBottomLeftRadius();
        bottomRightRadius_ = item->GetBottomRightRadius();
        LOGD("[Focus] Update: radius: %{public}.1lf", topLeftRadius_.GetX().Value());
        SoleChildElement::Update();
    }
}

void ListItemElement::PerformBuild()
{
    if (listItemComponent_) {
        if (key_ == -1 || key_ == listItemComponent_->GetKey()) {
            const auto& child = children_.empty() ? nullptr : children_.front();
            UpdateChild(child, listItemComponent_->GetChild());
        }
    }
}

bool ListItemElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    LOGD("[ListFocus]RequestNextFocus, index:%{public}d.", index_);
    return false;
}

void ListItemElement::HandleOnFocus()
{
    LOGI("[ListFocus]HandleOnFocus, index:%{public}d.", index_);
    // Find the parent until parent is not listItemElement.
    RefPtr<FocusGroup> parentElement = FocusGroup::GetParent().Upgrade();
    RefPtr<ListElement> listElement = AceType::DynamicCast<ListElement>(parentElement);
    if (!listElement) {
        if (!AceType::InstanceOf<ListItemGroupElement>(parentElement)) {
            auto listItemElement = ListItemElement::GetListItem(listElement);
            if (!listItemElement) {
                LOGE("[ListFocus]HandleOnFocus, Get Parent list/item failed.");
                return;
            }
            listItemElement->HandleOnFocus();
            return;
        } else {
            // item in group
            auto parent = AceType::DynamicCast<ListItemGroupElement>(parentElement);
            if (parent) {
                parent->ItemFocus(index_);
            }
        }
    }
    RefPtr<RenderListItem> renderListItem = RenderListItem::GetRenderListItem(GetRenderNode());
    if (!renderListItem) {
        LOGE("[ListFocus]HandleOnFocus, Get RenderListItem failed, index:%{public}d.", index_);
        return;
    }
    if (renderListItem->GetIndex() < 0) {
        return;
    }
    // paint focus animation
    if (listElement) {
        renderListItem->HandleFocusEvent(true);
        listElement->MoveItemToViewPort(renderListItem->GetPositionInList());
    } else {
        renderListItem->HandleFocusEvent(true, true);
    }
}

bool ListItemElement::OnKeyEvent(const KeyEvent& keyEvent)
{
    if (keyEvent.code == KeyCode::TV_CONTROL_UP || keyEvent.code == KeyCode::TV_CONTROL_DOWN ||
        keyEvent.code == KeyCode::TV_CONTROL_LEFT || keyEvent.code == KeyCode::TV_CONTROL_RIGHT ||
        keyEvent.code == KeyCode::KEY_TAB) {
        RefPtr<FocusGroup> parentElement = FocusGroup::GetParent().Upgrade();
        RefPtr<ListElement> listElement = AceType::DynamicCast<ListElement>(parentElement);
        if (listElement) {
            listElement->NeedMoveFocusItem(true);
        }
    }

    if (keyEvent.action == KeyAction::CLICK) {
        if (keyEvent.code == KeyCode::KEY_ENTER || keyEvent.code == KeyCode::KEY_NUMPAD_ENTER ||
            keyEvent.code == KeyCode::KEY_DPAD_CENTER) {
            HandleOnClick();
        }
    }

    return FocusGroup::OnKeyEvent(keyEvent);
}

void ListItemElement::HandleOnClick()
{
    RefPtr<RenderListItem> renderListElement = RenderListItem::GetRenderListItem(GetRenderNode());
    if (!renderListElement) {
        LOGE("[ListFocus]HandleOnClick, Get RenderListItem failed, Dep:%{public}d.", GetDepth());
        return;
    }

    renderListElement->HandleClicked();
}

void ListItemElement::HandleOnBlur()
{
    RefPtr<RenderListItem> renderListElement = RenderListItem::GetRenderListItem(GetRenderNode());
    if (!renderListElement) {
        LOGE("[ListFocus]HandleOnBlur, Get RenderListItem failed, Dep:%{public}d.", GetDepth());
        return;
    }

    auto parentElement = FocusGroup::GetParent().Upgrade();
    if (AceType::InstanceOf<ListItemGroupElement>(parentElement)) {
        renderListElement->HandleFocusEvent(false, true);
    } else {
        // cancel focus animation
        renderListElement->HandleFocusEvent(false);
    }
}

bool ListItemElement::CanUpdate(const RefPtr<Component>& newComponent)
{
    auto listItemComponent = AceType::DynamicCast<ListItemComponent>(newComponent);
    if (!listItemComponent) {
        return false;
    }

    return listItemComponent->GetType() == type_;
}

} // namespace OHOS::Ace
