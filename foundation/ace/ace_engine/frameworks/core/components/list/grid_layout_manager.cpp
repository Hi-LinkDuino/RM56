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

#include "core/components/list/grid_layout_manager.h"

#include <algorithm>

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/list/list_component.h"
#include "core/components/list/render_list_item.h"
#include "core/components/scroll/render_multi_child_scroll.h"

namespace {

#ifndef WEARABLE_PRODUCT
constexpr int32_t MIN_COUNT_OF_ADJUST_SLIP = 50;
constexpr double SLIP_FACTOR = 250.0;
#else
constexpr int32_t MIN_COUNT_OF_ADJUST_SLIP = 30;
constexpr double SLIP_FACTOR = 250.0;
#endif

} // namespace

namespace OHOS::Ace {

GridLayoutManager::GridLayoutManager(RenderList& renderList) : renderList_(renderList) {}

void GridLayoutManager::Update()
{
    direction_ = renderList_.GetDirection();
    maxCount_ = renderList_.GetMaxCount();
    cachedCount_ = renderList_.GetCachedCount();
    gridWidth_ = renderList_.GetWidth();
    gridHeight_ = renderList_.GetHeight();
    columnCount_ = renderList_.GetColumnCount();
    columnExtent_ = renderList_.GetColumnExtent();
    itemExtent_ = renderList_.GetItemExtent();
    if (direction_ == FlexDirection::COLUMN || direction_ == FlexDirection::COLUMN_REVERSE) {
        isVertical_ = true;
    } else {
        isVertical_ = false;
    }
    updateFlag_ = true;
    beginIndex_ = renderList_.GetBeginIndex();
    endIndex_ = renderList_.GetEndIndex();
    repeatedLength_ = renderList_.GetRepeatedLength();
    indexOffset_ = renderList_.GetIndexOffset();
    length_ = renderList_.GetLength();
    rightToLeft_ = renderList_.GetRightToLeft();
    crossAxisAlign_ = renderList_.GetFlexAlign();
    renderList_.MarkNeedLayout();
}

LayoutParam GridLayoutManager::MakeInnerLayoutParam(int32_t columnSpan) const
{
    LayoutParam innerLayout;
    if (itemExtent_.IsValid()) {
        double extent = itemExtent_.Value();
        if (itemExtent_.Unit() == DimensionUnit::PERCENT) {
            extent *= renderList_.GetMainSize(viewPort_);
        } else {
            extent = renderList_.NormalizeToPx(itemExtent_);
        }
        if (crossAxisAlign_ == FlexAlign::STRETCH) {
            if (isVertical_) {
                innerLayout.SetMinSize(Size(gridLen_ * columnSpan, extent));
                innerLayout.SetMaxSize(Size(gridLen_ * columnSpan, extent));
            } else {
                innerLayout.SetMinSize(Size(extent, gridLen_ * columnSpan));
                innerLayout.SetMaxSize(Size(extent, gridLen_ * columnSpan));
            }
        } else {
            if (isVertical_) {
                innerLayout.SetMaxSize(Size(gridLen_ * columnSpan, extent));
            } else {
                innerLayout.SetMaxSize(Size(extent, gridLen_ * columnSpan));
            }
        }
    } else {
        if (crossAxisAlign_ == FlexAlign::STRETCH) {
            if (isVertical_) {
                innerLayout.SetMinSize(Size(gridLen_ * columnSpan, innerLayout.GetMinSize().Height()));
                innerLayout.SetMaxSize(Size(gridLen_ * columnSpan, innerLayout.GetMaxSize().Height()));
            } else {
                innerLayout.SetMinSize(Size(innerLayout.GetMinSize().Width(), gridLen_ * columnSpan));
                innerLayout.SetMaxSize(Size(innerLayout.GetMaxSize().Width(), gridLen_ * columnSpan));
            }
        } else {
            if (isVertical_) {
                innerLayout.SetMaxSize(Size(gridLen_ * columnSpan, innerLayout.GetMaxSize().Height()));
            } else {
                innerLayout.SetMaxSize(Size(innerLayout.GetMaxSize().Width(), gridLen_ * columnSpan));
            }
        }
    }
    return innerLayout;
}

void GridLayoutManager::SetChildPosition(
    const RefPtr<RenderNode>& child, double mainSize, int32_t gridPos, int32_t columnSpan)
{
    double crossAxis = 0.0;
    FlexAlign align = crossAxisAlign_;
    if (rightToLeft_) {
        if (align == FlexAlign::FLEX_END) {
            align = FlexAlign::FLEX_START;
        } else if (align == FlexAlign::FLEX_START) {
            align = FlexAlign::FLEX_END;
        }
    }
    switch (align) {
        case FlexAlign::FLEX_END:
            crossAxis = gridLen_ * columnSpan - renderList_.GetCrossSize(child->GetLayoutSize());
            break;
        case FlexAlign::CENTER:
            crossAxis = (gridLen_ * columnSpan - renderList_.GetCrossSize(child->GetLayoutSize())) / 2.0;
            break;
        case FlexAlign::STRETCH:
        case FlexAlign::FLEX_START:
        default:
            break;
    }
    crossAxis += gridPos * gridLen_;
    if (isVertical_) {
        if (rightToLeft_) {
            crossAxis = crossSize_ - crossAxis - gridLen_ * columnSpan;
        }
        child->SetPosition(Offset(crossAxis, mainSize) + position_);
    } else {
        if (rightToLeft_) {
            mainSize = mainSize_ - renderList_.GetMainSize(child->GetLayoutSize()) - mainSize;
        }
        child->SetPosition(Offset(mainSize, crossAxis) + position_);
    }
    LOGD("Child:%{public}lf %{public}lf %{public}s", crossAxis, mainSize, position_.ToString().c_str());
}

void GridLayoutManager::CalculateCachedRange(
    int32_t viewBegin, int32_t viewEnd, int32_t cachedCount, int32_t& cachedBegin, int32_t& cachedEnd)
{
    cachedBegin = (viewBegin - cachedCount > 0) ? (viewBegin - cachedCount) : 0;
    if (length_ != LIST_LENGTH_INFINITE) {
        cachedEnd = (viewEnd + cachedCount > length_) ? length_ : viewEnd + cachedCount;
    } else {
        cachedEnd = viewEnd + cachedCount;
    }
}

void GridLayoutManager::RequestMoreItemsIfNeeded(int32_t viewBegin, int32_t viewEnd)
{
    int32_t cachedBegin;
    int32_t cachedEnd;
    if (beginIndex_ == LIST_PARAM_INVAID || endIndex_ == LIST_PARAM_INVAID) {
        return;
    }

    if (viewBegin > viewEnd) {
        std::swap(viewBegin, viewEnd);
    }

    CalculateCachedRange(viewBegin, viewEnd, cachedCount_, cachedBegin, cachedEnd);
    if (CalculateRepeatedIndex(cachedBegin) < beginIndex_ || CalculateRepeatedIndex(cachedEnd) > endIndex_) {
        int32_t requestBegin;
        int32_t requestEnd;
        CalculateCachedRange(viewBegin, viewEnd, 2 * cachedCount_, requestBegin, requestEnd);
        renderList_.RequestMoreItems(CalculateRepeatedIndex(requestBegin), CalculateRepeatedIndex(requestEnd));
    }
}

void GridLayoutManager::CalculateAxisSize()
{
    // Not first time layout after update, no need to initial.
    if (!updateFlag_ && !renderList_.IsLayoutChanged()) {
        return;
    }

    if (isVertical_) {
        mainSize_ = ((gridHeight_ > 0.0) && (gridHeight_ < renderList_.GetLayoutParam().GetMaxSize().Height()))
                        ? gridHeight_
                        : renderList_.GetLayoutParam().GetMaxSize().Height();
        crossSize_ = ((gridWidth_ > 0.0) && (gridWidth_ < renderList_.GetLayoutParam().GetMaxSize().Width()))
                         ? gridWidth_
                         : renderList_.GetLayoutParam().GetMaxSize().Width();
    } else {
        mainSize_ = ((gridWidth_ > 0.0) && (gridWidth_ < renderList_.GetLayoutParam().GetMaxSize().Width()))
                        ? gridWidth_
                        : renderList_.GetLayoutParam().GetMaxSize().Width();
        crossSize_ = ((gridHeight_ > 0.0) && (gridHeight_ < renderList_.GetLayoutParam().GetMaxSize().Height()))
                         ? gridHeight_
                         : renderList_.GetLayoutParam().GetMaxSize().Height();
    }

    // Initialize the columnCount, default is 1
    if (columnCount_ == 0) {
        if (columnExtent_ > 0) {
            columnCount_ = crossSize_ / columnExtent_;
            if (columnCount_ * columnExtent_ < crossSize_) {
                ++columnCount_;
            }
        } else {
            columnCount_ = 1;
        }
    }

    // Initialize the column length
    if (NearEqual(crossSize_, Size::INFINITE_SIZE)) {
        crossSize_ = renderList_.GetCrossSize(viewPort_);
    }
    gridLen_ = crossSize_ / columnCount_;
}

void GridLayoutManager::RefreshLayout()
{
    if (!needRefresh_) {
        return;
    }
    needRefresh_ = false;
    auto items = renderList_.GetItems();
    auto iter = items.begin();
    if (iter != items.end()) {
        auto firstItem = RenderListItem::GetRenderListItem(iter->second);
        if (firstItem && firstItem->GetOperation() != LIST_ITEM_OP_NONE) {
            LOGI("First item changed, recycle all child and layout again.");
            renderList_.RecycleAllChild();
            renderList_.RefreshOffset(0.0);
            itemGrid_.clear();
            itemPosition_.clear();
            itemGroupsExpand_.clear();
            itemGroupsFocusIndex_.clear();
        } else {
            std::vector<int32_t> needRemoveItems;
            std::map<int32_t, RefPtr<RenderNode>> newItems;
            int32_t rmCount = 0;
            while (iter != items.end()) {
                auto item = RenderListItem::GetRenderListItem(iter->second);
                if (item) {
                    if (item->GetOperation() == LIST_ITEM_OP_REMOVE) {
                        LOGI("This item[%{public}d] removed, notify element to recycle.", item->GetIndex());
                        needRemoveItems.emplace_back(iter->first);
                        itemGrid_.erase(item->GetIndex());
                        itemPosition_.erase(item->GetIndex());
                        itemGroupsExpand_.erase(item->GetIndex());
                        itemGroupsFocusIndex_.erase(item->GetIndex());
                        ++rmCount;
                    } else {
                        if (rmCount > 0) {
                            itemGrid_.erase(item->GetIndex());
                            itemPosition_.erase(item->GetIndex());
                            itemGroupsExpand_.erase(item->GetIndex());
                            itemGroupsFocusIndex_.erase(item->GetIndex());
                        }
                        item->SetIndex(item->GetIndex() - rmCount);
                        newItems.emplace(std::make_pair(item->GetIndex(), iter->second));
                    }
                }
                iter++;
            }
            renderList_.RecycleByItems(needRemoveItems);
            renderList_.ResetItems(newItems);
        }
    }
    renderList_.OnRefreshed();
}

void GridLayoutManager::PerformLayout()
{
    LOGD("PerformLayout head:%{public}lf tail:%{public}lf", head_, tail_);
    CalculateAxisSize();
    RefreshLayout();
    int32_t itemIndex = GetIndexByPosition(head_);
    int32_t firstIndex = itemIndex;
    renderList_.RecycleHead(itemIndex - 1); // Recycle head items.
    double curMainSize = GetItemPosition(itemIndex);
    auto itemChild = renderList_.GetChildByIndex(itemIndex);
    while (!itemChild && curMainSize < tail_ && CheckItemPosition(itemIndex)) {
        itemChild = renderList_.GetChildByIndex(itemIndex);
        curMainSize = GetItemPosition(itemIndex);
        ++itemIndex;
    }
    int32_t curGrid = GetItemGrid(itemIndex);
    double childMainSize = 0.0;
    while (itemChild) {
        auto listItem = RenderListItem::GetRenderListItem(itemChild);
        if (!listItem) {
            LOGE("Get render list item failed index: %{public}d", itemIndex);
            return;
        }
        int32_t span = std::min(listItem->GetColumnSpan(), columnCount_ - curGrid);
        LayoutParam innerLayout = MakeInnerLayoutParam(span);
        itemChild->Layout(innerLayout);
        LOGD("Index:%{public}d mainSize:%{public}lf grid:%{public}d", itemIndex, curMainSize, curGrid);
        SetChildPosition(itemChild, curMainSize, curGrid, span);
        itemGrid_[itemIndex] = curGrid;
        itemPosition_[itemIndex] = curMainSize;
        childMainSize = std::max(renderList_.GetMainSize(itemChild->GetLayoutSize()), childMainSize);
        curGrid += span;
        if (curGrid >= columnCount_) {
            curGrid = 0;
            curMainSize += childMainSize;
            childMainSize = 0.0;
        }
        if (curMainSize >= tail_) {
            break;
        }
        itemChild = renderList_.GetChildByIndex(++itemIndex);
    }
    curMainSize += childMainSize;
    RequestMoreItemsIfNeeded(firstIndex, itemIndex);
    renderList_.RecycleTail(itemIndex + 1); // Recycle tail items.
    Size layoutSize = renderList_.MakeValue<Size>(curMainSize, renderList_.GetCrossSize(viewPort_));
    renderList_.SetLayoutSize(layoutSize);
    ShowItemFocusAnimation();
    updateFlag_ = false;
    if (itemCountOfPage_ != (itemIndex - firstIndex)) {
        itemCountOfPage_ = itemIndex - firstIndex;

        if (itemCountOfPage_ > MIN_COUNT_OF_ADJUST_SLIP) {
            slipFactor_ = itemCountOfPage_ * SLIP_FACTOR;
        }
    }
}

int32_t GridLayoutManager::focusMove(KeyDirection direction)
{
    int32_t index = focusMove_;
    int32_t curGrid = 0;
    int32_t curSpan = 1;
    auto curFocus = renderList_.GetItemByIndex(index);
    if (curFocus) {
        curGrid = GetItemGrid(index);
        curSpan = curFocus->GetColumnSpan();
    }
    switch (direction) {
        case KeyDirection::UP:
        case KeyDirection::DOWN: {
            auto next = renderList_.GetItemByIndex(direction == KeyDirection::UP ? --index : ++index);
            while (next) {
                int32_t nextGrid = GetItemGrid(index);
                int32_t nextSpan = next->GetColumnSpan();
                if (nextGrid == curGrid || (nextGrid < curGrid && nextGrid + nextSpan > curGrid)) {
                    return index;
                }
                next = renderList_.GetItemByIndex(direction == KeyDirection::UP ? --index : ++index);
            }
            break;
        }
        case KeyDirection::LEFT:
            if (curGrid != 0) {
                auto next = renderList_.GetItemByIndex(--index);
                if (next && GetItemGrid(index) < curGrid) {
                    return index;
                }
            }
            break;
        case KeyDirection::RIGHT:
            if (curGrid + curSpan < columnCount_) {
                auto next = renderList_.GetItemByIndex(++index);
                if (next && GetItemGrid(index) > curGrid) {
                    return index;
                }
            }
            break;
        default:
            break;
    }
    return -1;
}

void GridLayoutManager::LayoutToItem(int32_t toIndex)
{
    int32_t curHeadIndex = renderList_.GetCurrentMinIndex();
    int32_t curTailIndex = renderList_.GetCurrentMaxIndex();
    double curMainSize = GetItemPosition(curTailIndex);
    int32_t curGrid = GetItemGrid(curTailIndex);
    auto itemChild = renderList_.GetChildByIndex(curTailIndex);
    double childMainSize = 0.0;
    while (itemChild) {
        auto listItem = RenderListItem::GetRenderListItem(itemChild);
        if (!listItem) {
            LOGE("Get render list item failed index: %{public}d", curTailIndex);
            return;
        }
        int32_t span = std::min(listItem->GetColumnSpan(), columnCount_ - curGrid);
        LayoutParam innerLayout = MakeInnerLayoutParam(span);
        itemChild->Layout(innerLayout);
        LOGD("Index:%{public}d mainSize:%{public}lf grid:%{public}d", curTailIndex, curMainSize, curGrid);
        SetChildPosition(itemChild, curMainSize, curGrid, span);
        itemGrid_[curTailIndex] = curGrid;
        itemPosition_[curTailIndex] = curMainSize;
        childMainSize = std::max(renderList_.GetMainSize(itemChild->GetLayoutSize()), childMainSize);
        curGrid += span;
        if (curGrid >= columnCount_) {
            curGrid = 0;
            curMainSize += childMainSize;
            childMainSize = 0.0;
        }
        if (curTailIndex >= toIndex) {
            break;
        }
        itemChild = renderList_.GetChildByIndex(++curTailIndex);
        renderList_.RecycleHead(curHeadIndex++);
    }
    curMainSize += childMainSize;
    Size layoutSize = renderList_.MakeValue<Size>(curMainSize, renderList_.GetCrossSize(viewPort_));
    renderList_.SetLayoutSize(layoutSize);
}

void GridLayoutManager::LayoutMore(double incDistance)
{
    // Use to load about one page size, so not need to recycle child.
    int32_t curTailIndex = renderList_.GetCurrentMaxIndex();
    double curMainSize = GetItemPosition(curTailIndex);
    int32_t curGrid = GetItemGrid(curTailIndex);
    auto itemChild = renderList_.GetChildByIndex(curTailIndex);
    double childMainSize = 0.0;
    double incMainSize = 0.0;
    while (itemChild) {
        auto listItem = RenderListItem::GetRenderListItem(itemChild);
        if (!listItem) {
            LOGE("Get render list item failed index: %{public}d", curTailIndex);
            return;
        }
        int32_t span = std::min(listItem->GetColumnSpan(), columnCount_ - curGrid);
        LayoutParam innerLayout = MakeInnerLayoutParam(span);
        itemChild->Layout(innerLayout);
        LOGD("Index:%{public}d mainSize:%{public}lf grid:%{public}d", curTailIndex, curMainSize, curGrid);
        SetChildPosition(itemChild, curMainSize, curGrid, span);
        itemGrid_[curTailIndex] = curGrid;
        itemPosition_[curTailIndex] = curMainSize;
        childMainSize = std::max(renderList_.GetMainSize(itemChild->GetLayoutSize()), childMainSize);
        curGrid += span;
        if (curGrid >= columnCount_) {
            curGrid = 0;
            curMainSize += childMainSize;
            incMainSize += childMainSize;
            childMainSize = 0.0;
        }
        if (incMainSize >= incDistance) {
            break;
        }
        itemChild = renderList_.GetChildByIndex(++curTailIndex);
    }
    curMainSize += childMainSize;
    Size layoutSize = renderList_.MakeValue<Size>(curMainSize, renderList_.GetCrossSize(viewPort_));
    renderList_.SetLayoutSize(layoutSize);
}

void GridLayoutManager::MoveItemToViewPort(double position)
{
    RefPtr<RenderNode> item = renderList_.GetChildByPosition(position);
    if (!item) {
        LOGE("[ListFocus]MoveItemToViewPort, Get item failed.");
        return;
    }
    Size itemSize = item->GetLayoutSize();
    double size = (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) ? itemSize.Width()
                                                                                                 : itemSize.Height();

    // jump to this item using position
    RefPtr<RenderNode> parentNode = renderList_.GetParent().Upgrade();
    RefPtr<RenderMultiChildScroll> scroll = AceType::DynamicCast<RenderMultiChildScroll>(parentNode);
    if (!scroll) {
        LOGE("[ListFocus]MoveItemToViewPort, Get Parent failed.");
        return;
    }
    double focusEffectWidth = size * (TV_ITEM_SCALE - DEFAULT_SCALE) * HALF_ITEM_SIZE;
    double animationOffset = scroll->NormalizeToPx(FOCUS_BOUNDARY);
    scroll->MoveItemToViewPort(position, size, animationOffset + focusEffectWidth);
    LOGD("[ListFocus][list] Direction:%{public}d, Item position:%{public}.1lf.", direction_, position);
}

void GridLayoutManager::ShowItemFocusAnimation()
{
    RefPtr<RenderListItem> focusItem;
    for (const auto& item : renderList_.GetItems()) {
        if (!item.second || item.second->GetChildren().empty()) {
            continue;
        }

        focusItem = RenderListItem::GetRenderListItem(item.second);
        if (!focusItem) {
            break;
        } else {
            if (focusItem->IsFocused()) {
                focusItem->ShowFocusAnimation(true, Rect(renderList_.GetGlobalOffset(), viewPort_));
            }
        }
    }
}

} // namespace OHOS::Ace
