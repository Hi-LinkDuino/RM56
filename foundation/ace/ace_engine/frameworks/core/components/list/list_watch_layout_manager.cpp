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

#include "core/components/list/list_watch_layout_manager.h"

#include "core/components/list/render_list_item_group.h"
#include "core/components/scroll/render_multi_child_scroll.h"

namespace OHOS::Ace {

namespace {

const double HALF_SIZE = 0.5;

} // namespace

ListWatchLayoutManager::ListWatchLayoutManager(RenderList& renderList)
    : ListLayoutManager(renderList) {}

void ListWatchLayoutManager::PerformLayout()
{
    LOGD("PerformLayout head:%{public}lf tail:%{public}lf", head_, tail_);
    renderList_.SetCenterIndex(INVALID_INDEX);
    if (renderList_.GetAddDeleteEffect()) {
        AnimationForItemUpdate();
    } else {
        RefreshLayout();
    }

    int32_t itemIndex = GetIndexByPosition(head_);
    renderList_.RecycleHead(itemIndex - 1); // Recycle head items.
    double curMainSize = GetItemPosition(itemIndex);
    LayoutParam innerLayout = MakeInnerLayoutParam(crossAxisAlign_);
    auto itemChild = renderList_.GetChildByIndex(itemIndex);
    int32_t firstIndex = itemIndex;
    while (itemChild) {
        UpdateItemGroupAttr(itemChild);
        itemChild->Layout(AdjustLayoutParam(itemChild, innerLayout));
        if (itemIndex == ZERO_INDEX) {
            firstChildSize_ = renderList_.GetMainSize(itemChild->GetLayoutSize());
        }
        lastChildSize_ = renderList_.GetMainSize(itemChild->GetLayoutSize());
        LOGD("Index:%{public}d mainSize:%{public}lf", itemIndex, curMainSize);

        SetChildPosition(itemChild, itemIndex, curMainSize);
        itemPosition_[itemIndex] = curMainSize;
        curMainSize += renderList_.GetMainSize(itemChild->GetLayoutSize());
        if (curMainSize >= tail_) {
            break;
        }
        CalculateItemState(itemChild);
        itemChild = renderList_.GetChildByIndex(++itemIndex);
    }
    RequestMoreItemsIfNeeded(firstIndex, itemIndex);
    renderList_.RecycleTail(itemIndex + 1); // Recycle tail items.

    if (renderList_.GetCenterIndex() == INVALID_INDEX) {
        MarkAllItemBlur();
    }
    if (renderList_.IsCenterLayout() && firstChildSize_ < renderList_.GetMainSize(viewPort_)) {
        if (NearZero(lastChildSize_)) {
            lastChildSize_ = firstChildSize_;
        }
        curMainSize = AdjustLayoutSize(curMainSize);
    }

    Size layoutSize = renderList_.MakeValue<Size>(curMainSize, renderList_.GetCrossSize(viewPort_));
    renderList_.SetLayoutSize(layoutSize);
    renderList_.CalculateStickyItem(position_);
    ShowItemFocusAnimation();
    HandleItemStateAndEffect();
    firstLayout_ = false;
}

void ListWatchLayoutManager::SetChildPosition(const RefPtr<RenderNode>& child, int32_t index, double mainSize)
{
    double mainLen = renderList_.GetMainSize(viewPort_);
    double crossLen = renderList_.GetCrossSize(viewPort_);
    double mainAxis = mainSize;
    double crossAxis = 0.0;
    auto listItemChild = RenderListItem::GetRenderListItem(child);
    FlexAlign selfAlign = listItemChild ? listItemChild->GetSelfAlign() : FlexAlign::AUTO;
    FlexAlign align = selfAlign == FlexAlign::AUTO ? crossAxisAlign_ : selfAlign;
    if (rightToLeft_) {
        if (align == FlexAlign::FLEX_END) {
            align = FlexAlign::FLEX_START;
        } else if (align == FlexAlign::FLEX_START) {
            align = FlexAlign::FLEX_END;
        }
    }
    switch (align) {
        case FlexAlign::FLEX_END:
            crossAxis = crossLen - renderList_.GetCrossSize(child->GetLayoutSize());
            break;
        case FlexAlign::CENTER:
            crossAxis = (crossLen - renderList_.GetCrossSize(child->GetLayoutSize())) / 2.0;
            break;
        case FlexAlign::STRETCH:
        case FlexAlign::FLEX_START:
        default:
            break;
    }
    auto isPlatformFive = IsPlatformFive();

    if (isVertical_) {
        if (direction_ == FlexDirection::COLUMN_REVERSE) {
            mainAxis = mainLen - renderList_.GetMainSize(child->GetLayoutSize()) - mainAxis;
        }

        if (isPlatformFive && renderList_.IsCenterLayout() && firstChildSize_ < renderList_.GetMainSize(viewPort_)) {
            mainAxis += (renderList_.GetMainSize(viewPort_) - firstChildSize_) * HALF_SIZE;
        }
        child->SetPosition(Offset(crossAxis, mainAxis) + position_);
    } else {
        if (IsRowReverse()) {
            mainAxis = mainLen - renderList_.GetMainSize(child->GetLayoutSize()) - mainAxis;
        }
        if (isPlatformFive && renderList_.IsCenterLayout() && firstChildSize_ < renderList_.GetMainSize(viewPort_)) {
            mainAxis += (renderList_.GetMainSize(viewPort_) - firstChildSize_) * HALF_SIZE;
        }
        child->SetPosition(Offset(mainAxis, crossAxis) + position_);
    }
    LOGD("Child:%{public}lf %{public}lf %{public}s", mainAxis, crossAxis, position_.ToString().c_str());
}

void ListWatchLayoutManager::HandleItemStateAndEffect()
{
    auto isFromRotate = false;
    auto parentScroll = AceType::DynamicCast<RenderScroll>(renderList_.GetParent().Upgrade());
    if (parentScroll) {
        isFromRotate = parentScroll->IsFromRotate();
    }
    RefPtr<RenderListItem> listItem;
    for (const auto& item : renderList_.GetItems()) {
        if (!item.second || item.second->GetChildren().empty()) {
            continue;
        }

        listItem = AceType::DynamicCast<RenderListItem>(item.second->GetChildren().front());
        if (!listItem) {
            break;
        }
        auto index = listItem->GetIndex();
        auto centerIndex = renderList_.GetCenterIndex();
        if (index == centerIndex - 1 || index == centerIndex + 1 ||
            (centerIndex == INVALID_INDEX && index == renderList_.GetCurrentMaxIndex())) {
            listItem->SetCurrentState(ItemState::NEARBY);
        }
        listItem->HandleItemEffect(isFromRotate);
    }
}

void ListWatchLayoutManager::CalculateItemState(RefPtr<RenderNode>& item)
{
    auto centerItem = RenderListItem::GetRenderListItem(item);
    auto isVertical = direction_ == FlexDirection::COLUMN || direction_ == FlexDirection::COLUMN_REVERSE;
    if (centerItem) {
        if (centerItem->IsItemCenter(isVertical, viewPort_)) {
            centerItem->SetCurrentState(ItemState::FOCUS);
            renderList_.SetCenterIndex(centerItem->GetIndex());
        } else {
            centerItem->SetCurrentState(ItemState::BLUR);
        }
        centerItem->CalculateScaleFactorOnWatch();
    }
}

void ListWatchLayoutManager::MarkAllItemBlur()
{
    RefPtr<RenderListItem> listItem;
    for (const auto& item : renderList_.GetItems()) {
        if (!item.second || item.second->GetChildren().empty()) {
            continue;
        }

        listItem = AceType::DynamicCast<RenderListItem>(item.second->GetChildren().front());
        if (!listItem) {
            break;
        } else {
            listItem->SetCurrentState(ItemState::BLUR);
        }
    }
    renderList_.SetCenterIndex(INVALID_INDEX);
}

double ListWatchLayoutManager::GetFirstChildSize(RefPtr<RenderNode>& itemChild)
{
    auto renderListItem = RenderListItem::GetRenderListItem(itemChild);
    if (!renderListItem) {
        return renderList_.GetMainSize(itemChild->GetLayoutSize());
    }
    auto renderListItemGroup = AceType::DynamicCast<RenderListItemGroup>(renderListItem);
    if (!renderListItemGroup) {
        return renderList_.GetMainSize(itemChild->GetLayoutSize());
    }
    auto children = renderListItemGroup->GetChildren();
    auto rIter = children.rbegin();
    while (rIter != children.rend()) {
        auto child = *rIter++;
        auto listItemNode = RenderListItem::GetRenderListItem(child);
        if (listItemNode && listItemNode->GetPrimary()) {
            return renderList_.GetMainSize(child->GetLayoutSize());
        }
    }
    return renderList_.GetMainSize(itemChild->GetLayoutSize());
}

double ListWatchLayoutManager::AdjustLayoutSize(double size)
{
    if (!renderList_.IsCenterLayout()) {
        return size;
    }
    if (NearZero(lastChildSize_)) {
        lastChildSize_ = firstChildSize_;
    }
    if (IsPlatformFive()) {
        double changeSize = size + renderList_.GetMainSize(viewPort_);
        changeSize = changeSize - (firstChildSize_ + lastChildSize_) * HALF_SIZE;
        return changeSize;
    } else {
        return size + (renderList_.GetMainSize(viewPort_) - lastChildSize_) * HALF_SIZE;
    }
}

bool ListWatchLayoutManager::IsPlatformFive()
{
    auto context = renderList_.GetContext().Upgrade();
    const static int32_t PLATFORM_VERSION_FIVE = 5;
    return context && context->GetMinPlatformVersion() <= PLATFORM_VERSION_FIVE;
}

} // namespace OHOS::Ace