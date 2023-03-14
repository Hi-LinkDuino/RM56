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

#include "core/components/list/list_item_group_element.h"

#include "core/components/list/list_element.h"
#include "core/components/list/list_item_group_component.h"
#include "core/components/proxy/render_item_proxy.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace {

RefPtr<RenderNode> ListItemGroupElement::CreateRenderNode()
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
    RefPtr<RenderNode> node = SoleChildElement::CreateRenderNode();
    renderItemGroup_ = AceType::DynamicCast<RenderListItemGroup>(node);
    if (renderItemGroup_) {
        renderItemGroup_->RegisterRebuildCallback(
            [weakElement = AceType::WeakClaim(this)]() {
                auto groupElement = weakElement.Upgrade();
                if (groupElement) {
                    groupElement->MarkNeedRebuild();
                    groupElement->Rebuild();
                }
            });

    }
    return node;
}

void ListItemGroupElement::PerformBuild()
{
    LOGD("ListItemGroupElement::PerformBuild");
    auto groupComponent = AceType::DynamicCast<ListItemGroupComponent>(component_);
    if (!groupComponent) {
        LOGE("component is not list item group component");
        return;
    }

    auto childElementIter = children_.begin();
    auto listItems = groupComponent->GetChildren();
    auto listItemIter = listItems.begin();

    auto primary = GetPrimary();
    int32_t itemIndex = 0;
    while (listItemIter != listItems.end() && childElementIter != children_.end()) {
        auto element = UpdateChild(*childElementIter++, *listItemIter);
        auto itemElement = ListItemElement::GetListItem(element);
        if (itemElement) {
            primary != *listItemIter ? itemElement->SetIndex(++itemIndex) : itemElement->SetIndex(0);
        }
        listItemIter++;
    }

    while (listItemIter != listItems.end()) {
        auto element = UpdateChild(nullptr, *listItemIter);
        auto itemElement = ListItemElement::GetListItem(element);
        if (itemElement) {
            primary != *listItemIter ? itemElement->SetIndex(++itemIndex) : itemElement->SetIndex(0);
        }
        listItemIter++;
    }

    while (childElementIter != children_.end()) {
        auto current = childElementIter++;
        Element::RemoveChild(*current);
    }
}

void ListItemGroupElement::ApplyRenderChild(const RefPtr<RenderElement>& renderChild)
{
    if (!renderChild) {
        LOGE("Element child is null");
        return;
    }

    if (!renderNode_) {
        LOGE("RenderElement don't have a render node");
        return;
    }

    RefPtr<RenderNode> proxy;
    auto listItemElement = ListItemElement::GetListItem(renderChild);
    if (listItemElement) {
        proxy = listItemElement->GetProxyRenderNode();
    }
    if (!proxy) {
        proxy = RenderItemProxy::Create();
    }

    proxy->AddChild(renderChild->GetRenderNode());
    proxy->Attach(context_);
    renderNode_->AddChild(proxy);
}

bool ListItemGroupElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    LOGD("[ListItemGroupFocus]RequestNextFocus, this:%{public}p, Dep:%{public}d.", this, GetDepth());
    bool ret = false;
    if (!renderItemGroup_->GetExpand()) {
        AddItemGroupFocusIndex(0);
        return ret;
    }
    int32_t nextFocusIndex = GetNextFocusIndex(vertical, reverse);
    if (nextFocusIndex < 0) {
        return ret;
    }
    if (reverse) {
        while (nextFocusIndex >= 0) {
            ret = TraverseFocusNode(nextFocusIndex, true);
            if (ret) {
                AddItemGroupFocusIndex(nextFocusIndex);
                return ret;
            }
            --nextFocusIndex;
        }
    } else {
        int32_t maxSize = static_cast<int32_t>(children_.size());
        while (nextFocusIndex < maxSize) {
            ret = TraverseFocusNode(nextFocusIndex, false);
            if (ret) {
                AddItemGroupFocusIndex(nextFocusIndex);
                return ret;
            }
            ++nextFocusIndex;
        }
    }
    return ret;
}

int32_t ListItemGroupElement::GetNextFocusIndex(bool vertical, bool reverse)
{
    int32_t nextFocusIndex = renderItemGroup_->GetNextFocusIndex(GetItemGroupFocusIndex(), vertical, reverse);
    int32_t childrenSize = static_cast<int32_t>(children_.size());
    if (nextFocusIndex < 0 || nextFocusIndex >= childrenSize) {
        return -1;
    } else {
        return nextFocusIndex;
    }
}

bool ListItemGroupElement::TraverseFocusNode(int32_t currentFocusIndex, bool reverse)
{
    for (const auto& element: children_) {
        auto listItem = ListItemElement::GetListItem(element);
        if (listItem && listItem->IsFocusable() && currentFocusIndex == listItem->GetIndex()) {
            bool ret = listItem->RequestFocusImmediately();
            MoveItemToViewPort(listItem, reverse);
            return ret;
        }
    }
    return false;
}

void ListItemGroupElement::MoveItemToViewPort(const RefPtr<Element>& listItem, bool reverse)
{
    auto parentElement = FocusGroup::GetParent().Upgrade();
    auto listElement = AceType::DynamicCast<ListElement>(parentElement);
    if (listElement) {
        if (GetItemGroupFocusIndex() == 0) {
            listElement->MoveItemToViewPort(renderItemGroup_->GetPositionInList());
        } else {
            auto renderListItem = RenderListItem::GetRenderListItem(listItem->GetRenderNode());
            if (renderListItem) {
                double itemSize = renderItemGroup_->GetMainSize(renderListItem->GetLayoutSize());
                size_ = reverse ? size_ - itemSize : size_ + itemSize;
            }
            listElement->MoveItemGroupToViewPort(renderItemGroup_->GetPositionInList(), size_);
        }
    }
}

bool ListItemGroupElement::OnKeyEvent(const KeyEvent& keyEvent)
{
    LOGD("[ListItemGroupFocus]OnKeyEvent, this:%{public}p, Dep:%{public}d.", this, GetDepth());
    if (keyEvent.action == KeyAction::CLICK && keyEvent.code == KeyCode::TV_CONTROL_ENTER
        && GetItemGroupFocusIndex() == 0) {
        renderItemGroup_->HandleClicked();
        return true;
    } else {
        RefPtr<FocusGroup> parentElement = FocusGroup::GetParent().Upgrade();
        RefPtr<ListElement> listElement = AceType::DynamicCast<ListElement>(parentElement);
        if (listElement) {
            listElement->NeedMoveFocusItem(true);
        }
        return FocusGroup::OnKeyEvent(keyEvent);
    }
}

void ListItemGroupElement::HandleOnFocus()
{
    LOGD("[ListItemGroupFocus]HandleOnFocus, this:%{public}p, Dep:%{public}d.", this, GetDepth());
    // Find the parent until parent is not listItemElement.
    RefPtr<FocusGroup> parentElement = FocusGroup::GetParent().Upgrade();
    RefPtr<ListElement> listElement = AceType::DynamicCast<ListElement>(parentElement);
    if (!listElement) {
        auto listItemElement = ListItemElement::GetListItem(listElement);
        if (!listItemElement) {
            LOGE("[ListFocus]HandleOnFocus, Get Parent list/item failed.");
            return;
        }
        listItemElement->HandleOnFocus();
        return;
    }
    renderItemGroup_->SetFocused(true);
    if (!renderItemGroup_->GetExpand()) {
        AddItemGroupFocusIndex(0);
    }
    listElement->MoveItemToViewPort(renderItemGroup_->GetPositionInList());
}

void ListItemGroupElement::HandleOnBlur()
{
    RefPtr<RenderListItem> renderListItem = RenderListItem::GetRenderListItem(GetRenderNode());
    if (!renderListItem) {
        LOGE("[ListItemGroup]HandleOnBlur, Get RenderListItem failed, Dep:%{public}d.", GetDepth());
        return;
    }
    for (auto child : GetChildren()) {
        auto listItem = ListItemElement::GetListItem(child);
        if (listItem && listItem->IsCurrentFocus()) {
            listItem->LostFocus();
        }
    }
    renderListItem->SetFocused(false);
}

RefPtr<Component> ListItemGroupElement::GetPrimary()
{
    auto groupComponent = AceType::DynamicCast<ListItemGroupComponent>(component_);
    if (!groupComponent) {
        LOGE("component is not list item group component");
        return nullptr;
    }

    auto listItems = groupComponent->GetChildren();
    if (listItems.empty()) {
        LOGE("no child of group");
        return nullptr;
    }

    auto primary = listItems.front();
    // Find primary item.
    for (const auto& child : listItems) {
        auto listItem = ListItemComponent::GetListItem(child);
        if (listItem && listItem->GetPrimary()) {
            primary = child;
            break;
        }
    }
    return primary;
}

void ListItemGroupElement::OnFocus()
{
    if (focusNodes_.empty()) {
        return;
    }
    auto itFocusNode = itLastFocusNode_;
    do {
        if (itLastFocusNode_ == focusNodes_.end()) {
            itLastFocusNode_ = focusNodes_.begin();
            if (itLastFocusNode_ == itFocusNode) {
                break;
            }
        }
        auto element = AceType::DynamicCast<Element>(*itLastFocusNode_);
        auto itemElement = ListItemElement::GetListItem(element);
        if (itemElement->GetIndex() != GetItemGroupFocusIndex()) {
            continue;
        }
        if ((*itLastFocusNode_)->RequestFocusImmediately()) {
            FocusNode::OnFocus();
            return;
        }
    } while ((++itLastFocusNode_) != itFocusNode);
    // Not found any focusable node, clear focus.
    itLastFocusNode_ = focusNodes_.end();
}

void ListItemGroupElement::AddItemGroupFocusIndex(int32_t groupFocusIndex)
{
    auto parentElement = FocusGroup::GetParent().Upgrade();
    auto listElement = AceType::DynamicCast<ListElement>(parentElement);
    if (listElement) {
        listElement->AddItemGroupFocusIndex(GetIndex(), groupFocusIndex);
    }
}

int32_t ListItemGroupElement::GetItemGroupFocusIndex()
{
    auto parentElement = FocusGroup::GetParent().Upgrade();
    auto listElement = AceType::DynamicCast<ListElement>(parentElement);
    if (listElement) {
        int32_t focusIndex = listElement->GetItemGroupFocusIndex(GetIndex());
        return focusIndex;
    }
    return 0;
}

void ListItemGroupElement::ItemFocus(int32_t itemIndex)
{
    int32_t focusIndex = GetItemGroupFocusIndex();
    if (focusIndex == itemIndex) {
        return;
    }
    for (auto child : GetChildren()) {
        auto listItem = ListItemElement::GetListItem(child);
        if (listItem && listItem->GetIndex() == focusIndex) {
            listItem->LostFocus();
        }
    }
    AddItemGroupFocusIndex(itemIndex);
}

} // namespace OHOS::Ace
