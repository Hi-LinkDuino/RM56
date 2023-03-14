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

#include "core/components/list/render_list.h"

#include <algorithm>

#include "base/log/ace_trace.h"
#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/list/grid_layout_manager.h"
#include "core/components/list/list_component.h"
#include "core/components/list/list_layout_manager.h"
#include "core/components/list/list_watch_layout_manager.h"
#include "core/components/list/render_list_item.h"
#include "core/components/list/render_list_item_group.h"
#include "core/components/scroll/render_multi_child_scroll.h"

namespace OHOS::Ace {
namespace {

constexpr double ITEM_STICKY_OFFSET = 1.0;

} // namespace

void RenderList::Update(const RefPtr<Component>& component)
{
    const RefPtr<ListComponent> list = AceType::DynamicCast<ListComponent>(component);
    if (!list) {
        return;
    }

    needRefresh_ = true;
    int32_t preColumnCount = columnCount_;
    direction_ = list->GetDirection();
    maxCount_ = list->GetTotalCount();
    itemsCount_ = list->GetItemsCount();
    cachedCount_ = list->GetCachedCount();
    beginIndex_ = list->GetBeginIndex();
    endIndex_ = list->GetEndIndex();
    repeatLength_ = list->GetRepeatedLength();
    // length_ is set in ListElement::Update
    indexOffset_ = list->GetIndexOffset();
    crossAxisAlign_ = list->GetFlexAlign();
    columnCount_ = list->GetColumnCount();
    columnExtent_ = list->GetColumnExtent();
    listWidth_ = list->GetWidth();
    listHeight_ = list->GetHeight();
    controller_ = list->GetPositionController();
    itemExtent_ = list->GetItemExtent();
    rightToLeft_ = list->GetRightToLeft();
    supportItemCenter_ = list->GetSupportItemCenter();
    updateEffect_ = list->GetUpdateEffect();
    pageReady_ = list->GetPageReady();
    itemScale_ = list->IsItemScale();
    isCenterLayout_ = list->IsCenterLayout();
    const static int32_t PLATFORM_VERSION_FIVE = 5;
    auto context = GetContext().Upgrade();
    if (context && context->GetMinPlatformVersion() <= PLATFORM_VERSION_FIVE) {
        // api 5 not have center layout use item scale
        isCenterLayout_ = itemScale_;
    }

    if (SystemProperties::GetDeviceType() == DeviceType::PHONE) {
        chainAnimation_ = list->GetChainAnimation();
    } else {
        chainAnimation_ = false;
    }
    if (chainAnimation_) {
        chainProperty_ = list->GetChainProperty();
    }

    if (!layoutManager_ || preColumnCount != columnCount_) {
        if (columnCount_ <= 1) {
            if (SystemProperties::GetDeviceType() == DeviceType::WATCH) {
                layoutManager_ = AceType::MakeRefPtr<ListWatchLayoutManager>(*this);
            } else {
                layoutManager_ = AceType::MakeRefPtr<ListLayoutManager>(*this);
            }
        } else {
            layoutManager_ = AceType::MakeRefPtr<GridLayoutManager>(*this);
        }
    }

    const auto& rotationController = list->GetRotationController();
    if (rotationController) {
        // list parent is scroll
        rotationController->SetRequestRotationImpl(GetParent(), context_);
    }
    SetOnRotateCallback(list);

    UpdateAccessibilityAttr();
    layoutManager_->Update();
    overSpringProperty_ = list->OverSpringProperty();
}

void RenderList::UpdateTouchRect()
{
    touchRect_.SetSize(viewPort_);
    touchRect_.SetOffset(GetPosition());
    touchRectList_.emplace_back(touchRect_);
    SetTouchRectList(touchRectList_);
}

void RenderList::MarkNeedRefresh()
{
    layoutManager_->MarkNeedRefresh();
}

void RenderList::RefreshOffset(double offset)
{
    RefPtr<RenderNode> parentNode = GetParent().Upgrade();
    RefPtr<RenderMultiChildScroll> scroll = AceType::DynamicCast<RenderMultiChildScroll>(parentNode);
    if (!scroll) {
        LOGE("Parent scroll is null, RefreshOffset failed.");
        return;
    }
    scroll->RefreshOffset(offset);
}

void RenderList::RefreshScrollExtent()
{
    RefPtr<RenderNode> parentNode = GetParent().Upgrade();
    RefPtr<RenderMultiChildScroll> scroll = AceType::DynamicCast<RenderMultiChildScroll>(parentNode);
    if (!scroll) {
        LOGE("Parent scroll is null, RefreshScrollExtent failed.");
        return;
    }
    scroll->CalculateMainScrollExtent();
}

void RenderList::PerformLayout()
{
    layoutManager_->PerformLayout();
    int32_t firstIndex = GetIndexByPosition(-GetMainPosition(currentOffset_));
    if (firstIndex >= 0) {
        firstItemIndex_ = firstIndex;
        if (controller_) {
            controller_->SetFirstItemIndex(firstIndex);
        }
    }
    auto slipFactor = layoutManager_->GetSlipFactor();
    if (slipFactor > 0.0 && slipFactorSetting_) {
        slipFactorSetting_(slipFactor);
    }
}

void RenderList::OnPaintFinish()
{
    RefPtr<RenderListItem> listItem;
    Offset globalOffset = GetGlobalOffset();
    Rect listItemRect;
    Rect viewPortRect = Rect(globalOffset, viewPort_);
    for (const auto& item : items_) {
        if (!item.second || item.second->GetChildren().empty()) {
            continue;
        }
        listItem = RenderListItem::GetRenderListItem(item.second);
        if (!listItem) {
            continue;
        }
        auto node = listItem->GetAccessibilityNode().Upgrade();
        if (!node) {
            continue;
        }
        bool visible = GetVisible();
        if (visible) {
            listItemRect.SetSize(item.second->GetLayoutSize());
            listItemRect.SetOffset(globalOffset + item.second->GetPosition());
            visible = listItemRect.IsIntersectWith(viewPortRect);
        }
        listItem->SetAccessibilityVisible(visible);
        if (visible) {
            Rect clampRect = listItemRect.Constrain(viewPortRect);
            listItem->SetAccessibilityRect(clampRect);
        } else {
            listItem->NotifyPaintFinish();
        }
    }
}

void RenderList::BuildNextItem(double start, double end, Offset position, Size viewPort)
{
    ACE_FUNCTION_TRACE();
    int32_t maxIndex = GetCurrentMaxIndex();
    int32_t itemIndex = GetIndexByPosition(start);
    double offset = GetMainPosition(position);
    auto itemChild = GetChildByIndex(itemIndex);
    double curMainSize = GetItemPosition(itemIndex);
    if (offset < 0.0) {
        bool needCreateNewItem = false;
        while (itemChild) {
            curMainSize += GetMainSize(itemChild->GetLayoutSize());
            if (curMainSize >= end) {
                break;
            } else if (itemIndex < maxIndex) {
                itemChild = GetChildByIndex(++itemIndex);
            } else {
                needCreateNewItem = true;
                break;
            }
        }
        if (needCreateNewItem) {
            MarkNeedLayout();
            itemChild = GetChildByIndex(++itemIndex);
            layoutManager_->LayoutMore(end - curMainSize);
        }
    }
}

int32_t RenderList::RequestNextFocus(bool vertical, bool reverse)
{
    return layoutManager_->RequestNextFocus(vertical, reverse);
}

void RenderList::ListItemFocused(int32_t focusIndex)
{
    layoutManager_->ListItemFocused(focusIndex);
}

void RenderList::CalculateFocusIndexPosition()
{
    layoutManager_->CalculateFocusIndexPosition();
}

double RenderList::GetStickyMainSize(int32_t index)
{
    double size = 0.0;
    for (const auto& item : stickyItemMap_) {
        if (item.first > index) {
            return size;
        } else if (item.first == index) {
            return 0.0;
        } else {
            size = item.second;
        }
    }
    return size;
}

double RenderList::CalculateItemPosition(int32_t index, ScrollType type)
{
    switch (type) {
        case ScrollType::SCROLL_INDEX: {
            if (index >= GetCurrentMaxIndex()) {
                layoutManager_->LayoutToItem(index);
                layoutManager_->LayoutMore(GetMainSize(viewPort_));
            }
            double sticky = 0.0; // When exist sticky items, add sticky size when jump.
            if (SystemProperties::GetDeviceType() != DeviceType::WATCH || !IsSupportScale()) {
                CalculateStickyItem(Offset(0.0, -GetItemPosition(index)));
                if (stickyItem_) {
                    sticky = GetStickyMainSize(index);
                }
            }
            return std::max(GetItemPosition(index) - sticky, 0.0);
        }
        case ScrollType::SCROLL_PAGE_DOWN: {
            layoutManager_->LayoutMore(GetMainSize(viewPort_));
            double curMainSize = GetMainSize(GetLayoutSize());
            double maxJumpSize = curMainSize - GetMainSize(viewPort_);
            maxJumpSize += GetMainPosition(currentOffset_);
            return std::clamp(maxJumpSize, 0.0, GetMainSize(viewPort_)) - GetMainPosition(currentOffset_);
        }
        case ScrollType::SCROLL_PAGE_UP: {
            double position = -GetMainPosition(currentOffset_) - GetMainSize(viewPort_);
            return std::max(position, 0.0);
        }
        case ScrollType::SCROLL_BOTTOM: {
            layoutManager_->LayoutToItem(maxCount_);
            double curMainSize = GetMainSize(GetLayoutSize());
            double maxJumpSize = curMainSize - GetMainSize(viewPort_);
            return std::max(maxJumpSize, 0.0);
        }
        case ScrollType::SCROLL_TOP: {
            return GetItemPosition(0);
        }
        default: {
            LOGE("Unknown type:%{public}d", type);
            return -1.0;
        }
    }
}

void RenderList::CalculateItemPosition(double targetPos)
{
    layoutManager_->LayoutToPosition(targetPos);
    layoutManager_->LayoutMore(GetMainSize(viewPort_));
}

void RenderList::MoveItemToViewPort(double position)
{
    layoutManager_->MoveItemToViewPort(position);
}

void RenderList::MoveItemGroupToViewPort(double position, double size)
{
    layoutManager_->MoveItemGroupToViewPort(position, size);
}

void RenderList::ResetLayoutRange(double head, double tail, Offset position, Size viewport)
{
    viewPort_ = viewport;
    currentOffset_ = position;
    UpdateTouchRect();
    layoutManager_->ResetLayoutRange(head, tail, position, viewport);
}

void RenderList::AddListItem(int32_t index, const RefPtr<RenderNode>& renderNode)
{
    if (renderNode) {
        items_[index] = renderNode;
    }
}

RefPtr<RenderNode> RenderList::GetChildByPosition(double position) const
{
    double startPosition = 0.0;
    double endPosition = 0.0;
    RefPtr<RenderNode> node;
    RefPtr<RenderListItem> listItem;
    for (const auto& item : items_) {
        if (!item.second || item.second->GetChildren().empty()) {
            continue;
        }
        listItem = RenderListItem::GetRenderListItem(item.second);
        if (!listItem) {
            break;
        }
        startPosition = GetItemPosition(listItem->GetIndex());
        endPosition = startPosition + GetMainSize(item.second->GetLayoutSize());
        if ((position > startPosition && position < endPosition) || NearEqual(position, startPosition)) {
            return item.second;
        }
    }
    return node;
}

RefPtr<RenderNode> RenderList::GetNearChildByPosition(double position) const
{
    if (!layoutManager_) {
        LOGE("Get near child by position failed. layout manager is null.");
        return nullptr;
    }
    double startPosition = 0.0;
    double endPosition = 0.0;
    RefPtr<RenderNode> node;
    RefPtr<RenderListItem> listItem;
    for (const auto& item : items_) {
        if (!item.second || item.second->GetChildren().empty()) {
            continue;
        }
        listItem = RenderListItem::GetRenderListItem(item.second);
        if (!listItem) {
            break;
        }
        layoutManager_->GetChainItemRange(listItem->GetIndex(), startPosition, endPosition);
        if ((position > startPosition && position < endPosition) || NearEqual(position, startPosition)) {
            return item.second;
        }
        node = item.second;
    }
    return node;
}

RefPtr<RenderNode> RenderList::GetChildByIndex(int32_t index)
{
    if (index < 0) {
        LOGE("invalid index: %{public}d", index);
        return nullptr;
    }

    auto item = items_.find(index);
    if (item != items_.end()) {
        return item->second;
    }

    if (beginIndex_ == LIST_PARAM_INVAID && endIndex_ == LIST_PARAM_INVAID && maxCount_ > 0 && index >= maxCount_) {
        LOGW("reach max count! index: %{public}d", index);
        return nullptr;
    }

    // Try to build one new data;
    if (buildItem_ && !buildItem_(index)) {
        LOGW("no more new item to be built, index: %{public}d", index);
        return nullptr;
    }

    item = items_.find(index);
    if (item != items_.end()) {
        return item->second;
    }

    LOGD("build succ but get child failed, index: %{public}d", index);
    return nullptr;
}

RefPtr<RenderListItem> RenderList::GetItemByIndex(int32_t index)
{
    auto child = GetChildByIndex(index);
    if (!child || child->GetChildren().empty()) {
        return nullptr;
    }
    return RenderListItem::GetRenderListItem(child);
}

int32_t RenderList::GetIndexByPosition(double position) const
{
    int32_t index = -1;
    auto child = GetChildByPosition(position);
    if (child && !child->GetChildren().empty()) {
        auto listItem = RenderListItem::GetRenderListItem(child);
        if (listItem) {
            index = listItem->GetIndex();
        }
    }
    return index;
}

bool RenderList::RecycleByItems(const std::vector<int32_t>& needRemoveItems)
{
    if (needRemoveItems.empty()) {
        return true;
    }
    if (!recycleByItems_) {
        return false;
    }
    return recycleByItems_(needRemoveItems);
}

bool RenderList::RecycleByRange(int32_t from, int32_t to)
{
    LOGD("RecycleChild: fromIndex: %{public}d, toIndex: %{public}d", from, to);
    if (!recycleByRange_ || from < 0 || from > to) {
        return false;
    }
    bool recycleResult = false;
    bool success = recycleByRange_(from, to);
    if (success && from >= 0) {
        for (int32_t i = from; i <= to; i++) {
            auto iter = items_.find(i);
            if (iter != items_.end()) {
                LOGD("Recycle index %{public}d success", i);
                ResetGroupItem(iter->second);
                items_.erase(iter);
                recycleResult = true;
            }
        }
    }
    return recycleResult;
}

bool RenderList::RecycleAllChild()
{
    if (items_.empty()) {
        return true;
    }

    int32_t fromIndex = items_.begin()->first;
    int32_t toIndex = items_.rbegin()->first;

    return RecycleByRange(fromIndex, toIndex);
}

void RenderList::SyncIndex(int32_t begin, int32_t end)
{
    if (items_.empty()) {
        return;
    }

    int32_t fromIndex = items_.begin()->first;
    int32_t toIndex = items_.rbegin()->first;

    if (begin > 0 && fromIndex < begin) {
        RecycleByRange(fromIndex, begin - 1);
    }
    if (end > 0 && toIndex > end) {
        RecycleByRange(end + 1, toIndex);
    }
}

// recycle from [start to head] index.
void RenderList::RecycleHead(int32_t head)
{
    if (head >= 0) {
        RecycleByRange(0, head);
    }
}

// recycle from [tail to end] index.
void RenderList::RecycleTail(int32_t tail)
{
    if (tail < maxCount_) {
        RecycleByRange(tail, maxCount_);
    }
}

void RenderList::RequestMoreItems(int32_t index, int32_t count)
{
    if (requestItems_ && count > 0) {
        requestItems_(index, count);
    }
}

void RenderList::CalculateStickyItem(const Offset& position)
{
    // reset item state
    if (stickyItem_) {
        stickyItem_->SetVisible(false);
    }
    stickyNext_ = nullptr;
    RefPtr<RenderListItem> listItem = RenderListItem::GetRenderListItem(stickyItem_);
    if (listItem) {
        listItem->SetSticky(false);
    }
    double listPosition = -GetMainPosition(position);
    if (listPosition < 0.0) {
        return;
    }

    // calculate current ceiling item
    int32_t index = GetIndexByPosition(listPosition);
    if (index < 0) {
        return;
    }
    int32_t newStickyIndex = stickyItemSearcher_(index);
    if (newStickyIndex < 0) {
        if (stickyItem_) {
            listItem = RenderListItem::GetRenderListItem(stickyItem_);
            if (listItem) {
                listItem->HandleStickyEvent(false);
            }
        }
        stickyItem_ = nullptr;
        return;
    }

    listItem = RenderListItem::GetRenderListItem(stickyItem_);
    int32_t currentStickyIndex = INVALID_INDEX;
    stickyItemOffset_ = Offset::Zero();
    if (listItem) {
        currentStickyIndex = listItem->GetIndex();
        listItem->SetSticky(false);
        if (stickyItem_) {
            stickyItem_->SetVisible(false);
        }
    }

    if (newStickyIndex != currentStickyIndex) {
        LOGD("Sticky index change from %{public}d to %{public}d", currentStickyIndex, newStickyIndex);
        if (listItem) {
            listItem->HandleStickyEvent(false);
        }
        stickyItem_ = stickyItemBuilder_(newStickyIndex, false);
        listItem = RenderListItem::GetRenderListItem(stickyItem_);
        if (listItem) {
            listItem->HandleStickyEvent(true);
        }
    }

    if (listItem) {
        listItem->SetSticky(true);
    }

    // layout sticky item
    LayoutParam itemLayout;
    itemLayout.SetMaxSize(viewPort_);
    if (stickyItem_) {
        stickyItem_->Layout(itemLayout);
        stickyItem_->SetVisible(false);
        stickyItemMap_[newStickyIndex] = GetMainSize(stickyItem_->GetLayoutSize());
    }

    // check next item
    CalculateStickyItemOffset(index, listPosition);
}

void RenderList::CalculateStickyItemOffset(int32_t index, double position)
{
    stickyItemOffset_ = Offset::Zero();
    if (!stickyItem_) {
        return;
    }

    // check next item
    auto nextStickyIndex = GetIndexByPosition(position + GetMainSize(stickyItem_->GetLayoutSize()));
    auto item = items_.find(nextStickyIndex);
    if (item == items_.end()) {
        return;
    }

    RefPtr<RenderListItem> listItem = RenderListItem::GetRenderListItem(item->second);
    if (!listItem || !listItem->GetSticky()) {
        stickyNext_ = nullptr;
        return;
    }

    double nextItemPosition = GetItemPosition(listItem->GetIndex());
    double ceilingItemPosition = GetMainSize(stickyItem_->GetLayoutSize());
    double offset = nextItemPosition - position - ceilingItemPosition;
    if (offset > 0.0) {
        offset = 0.0;
        stickyNext_ = nullptr;
    } else {
        RefPtr<RenderListItem> listItemNext = RenderListItem::GetRenderListItem(item->second);
        if (listItemNext) {
            stickyNext_ = stickyItemBuilder_(listItemNext->GetIndex(), true);
        }
    }
    // layout sticky next
    if (stickyNext_) {
        LayoutParam itemLayout;
        itemLayout.SetMaxSize(viewPort_);
        stickyNext_->Layout(itemLayout);
        stickyNext_->SetVisible(false);
    }
    double nextItemOffset = nextItemPosition - position;
    nextItemOffset = nextItemOffset > ITEM_STICKY_OFFSET ? nextItemOffset - ITEM_STICKY_OFFSET : nextItemOffset;
    if (direction_ == FlexDirection::COLUMN || direction_ == FlexDirection::COLUMN_REVERSE) {
        stickyItemOffset_.SetY(offset);
        stickyNextOffset_.SetY(nextItemOffset);
    } else {
        stickyItemOffset_.SetX(offset);
        stickyNextOffset_.SetX(nextItemOffset);
    }
}

void RenderList::UpdateAccessibilityAttr()
{
    auto scroll = AceType::DynamicCast<RenderMultiChildScroll>(GetParent().Upgrade());
    if (!scroll) {
        LOGE("GetRenderMultiChildScroll failed.");
        return;
    }
    auto refPtr = scroll->GetAccessibilityNode().Upgrade();
    if (!refPtr) {
        return;
    }
    auto collectionInfo = refPtr->GetCollectionInfo();
    collectionInfo.rows = maxCount_ > 0 ? maxCount_ : itemsCount_;
    collectionInfo.columns = columnCount_;
    refPtr->SetCollectionInfo(collectionInfo);
    refPtr->SetScrollableState(true);
    refPtr->SetActionScrollForward([weakList = AceType::WeakClaim(this)]() {
        auto list = weakList.Upgrade();
        if (list) {
            LOGI("Trigger ScrollForward by Accessibility.");
            return list->HandleActionScroll(true);
        }
        return false;
    });
    refPtr->SetActionScrollBackward([weakList = AceType::WeakClaim(this)]() {
        auto list = weakList.Upgrade();
        if (list) {
            LOGI("Trigger ScrollBackward by Accessibility.");
            return list->HandleActionScroll(false);
        }
        return false;
    });
    refPtr->AddSupportAction(AceAction::ACTION_SCROLL_FORWARD);
    refPtr->AddSupportAction(AceAction::ACTION_SCROLL_BACKWARD);
}

bool RenderList::HandleActionScroll(bool forward)
{
    RefPtr<RenderNode> parent = GetParent().Upgrade();
    RefPtr<RenderMultiChildScroll> scroll = AceType::DynamicCast<RenderMultiChildScroll>(parent);
    if (!scroll) {
        LOGE("Parent scroll is null, trigger scroll failed.");
        return false;
    }
    return scroll->ScrollPage(!forward, true);
}

void RenderList::ResetGroupItem(const RefPtr<RenderNode>& renderNode)
{
    auto renderListItem = RenderListItem::GetRenderListItem(renderNode);
    if (renderListItem) {
        auto renderListItemGroup = AceType::DynamicCast<RenderListItemGroup>(renderListItem);
        if (renderListItemGroup) {
            renderListItemGroup->ResetLayout();
        }
    }
}

void RenderList::OnChildRemoved(const RefPtr<RenderNode>& child)
{
    if (child) {
        child->SetAccessibilityVisible(false);
        child->ClearAccessibilityRect();
    }
}

void RenderList::SetGroupState(int32_t index, bool expand)
{
    if (index >= 0) {
        layoutManager_->AddItemGroupExpand(index, expand);
    } else if (index == INDEX_EXPAND_ALL) {
        layoutManager_->ClearItemPosition();
        layoutManager_->SetExpandAll(expand);
        layoutManager_->ClearItemGroupsExpand();
        RefreshOffset(0.0);
    }
}

bool RenderList::TouchTest(const Point& globalPoint, const Point& parentLocalPoint,
    const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    RefPtr<RenderNode> parent = GetParent().Upgrade();
    RefPtr<RenderMultiChildScroll> scroll = AceType::DynamicCast<RenderMultiChildScroll>(parent);

    if (stickyItem_) {
        const auto localPoint = parentLocalPoint - GetPaintRect().GetOffset();
        stickyItem_->TouchTest(globalPoint, localPoint, touchRestrict, result);
    }

    if (scroll->IsScrollStop()) {
        return RenderNode::TouchTest(globalPoint, parentLocalPoint, touchRestrict, result);
    }

    return true;
}

// notify start position in global main axis
void RenderList::NotifyDragStart(double startPosition)
{
    if (!layoutManager_) {
        LOGE("NotifyDragStart failed. layout manager is null.");
        return;
    }
    double globalMainOffset = direction_ == FlexDirection::ROW ? GetGlobalOffset().GetX() : GetGlobalOffset().GetY();
    double localOffset = startPosition - globalMainOffset;
    double scrollPosition =
        direction_ == FlexDirection::ROW ? layoutManager_->GetPosition().GetX() : layoutManager_->GetPosition().GetY();
    double position = localOffset - scrollPosition;
    auto render = RenderListItem::GetRenderListItem(GetNearChildByPosition(position));
    if (render) {
        dragStartIndexPending_ = render->GetIndex();
        LOGD("DragStart. startPosition: %{public}.1f, position: %{public}.1f, index: %{public}d", startPosition,
            position, render->GetIndex());
    } else {
        LOGE("DragStart Mismatch. position: %{public}.1f, scrollPosition: %{public}.1f", position, scrollPosition);
    }
}

// notify drag offset in global main axis
void RenderList::NotifyDragUpdate(double dragOffset)
{
    LOGD("NotifyDragUpdate. dragOffset: %{public}.1lf.", dragOffset);
    currentDelta_ = dragOffset;
}

void RenderList::NotifyScrollOver(double velocity, bool isCrashTop, bool isCrashBottom)
{
    if (!chainAnimation_) {
        return;
    }
    if (NearZero(velocity)) {
        LOGD("velocity is zero, no need to handle in chain animation.");
        return;
    }
    // when scroll over head/tail, control node needs to switch to head/tail node.
    if (isCrashTop) {
        dragStartIndexPending_ = GetCurrentMinIndex();
    } else if (isCrashBottom) {
        dragStartIndexPending_ = GetCurrentMaxIndex();
    } else {
        LOGW("ScrollOver but neither top nor bottom crashed. velocity: %{public}f", velocity);
    }
    LOGD("NotifyScrollOver. velocity: %{public}.1lf, dragStartIndex_: %{public}d", velocity, dragStartIndexPending_);
}

void RenderList::SetOnRotateCallback(const RefPtr<ListComponent>& component)
{
    const auto& onRotateId = component->GetOnRotateId();
    if (onRotateId.IsEmpty()) {
        return;
    }
    rotationEvent_ = AceAsyncEvent<void(const RotationEvent&)>::Create(onRotateId, context_);
}
} // namespace OHOS::Ace
