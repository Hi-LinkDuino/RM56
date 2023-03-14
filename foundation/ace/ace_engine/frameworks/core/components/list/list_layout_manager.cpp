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

#include "core/components/list/list_layout_manager.h"

#include <algorithm>

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/animation/bilateral_spring_node.h"
#include "core/animation/curve.h"
#include "core/animation/curve_animation.h"
#include "core/components/list/list_component.h"
#include "core/components/list/render_list_item.h"
#include "core/components/list/render_list_item_group.h"
#include "core/components/scroll/render_multi_child_scroll.h"

namespace OHOS::Ace {
namespace {

const double MIN_FRICTION = 0.0;
const double MAX_FRICTION = 1.0;
const int32_t ADD_DEL_DURATION = 300;
const int32_t CHAIN_ANIMATION_NODE_COUNT = 30;

} // namespace

ListLayoutManager::ListLayoutManager(RenderList& renderList) : renderList_(renderList) {}

void ListLayoutManager::Update()
{
    maxCount_ = renderList_.GetMaxCount();
    cachedCount_ = renderList_.GetCachedCount();
    beginIndex_ = renderList_.GetBeginIndex();
    endIndex_ = renderList_.GetEndIndex();
    repeatedLength_ = renderList_.GetRepeatedLength();
    length_ = renderList_.GetLength();
    indexOffset_ = renderList_.GetIndexOffset();
    itemExtent_ = renderList_.GetItemExtent();
    rightToLeft_ = renderList_.GetRightToLeft();
    crossAxisAlign_ = renderList_.GetFlexAlign();
    direction_ = renderList_.GetDirection();
    if (direction_ == FlexDirection::COLUMN || direction_ == FlexDirection::COLUMN_REVERSE) {
        isVertical_ = true;
    } else {
        isVertical_ = false;
    }

    if (enableChain_ != renderList_.IsEnableChain()) {
        if (renderList_.IsEnableChain()) {
            auto context = renderList_.GetContext().Upgrade();
            if (context) {
                chainInterval_ = context->NormalizeToPx(renderList_.GetChainProperty().Interval());
            }
            InitChainAnimation(CHAIN_ANIMATION_NODE_COUNT);
        } else {
            chainInterval_ = 0.0;
        }
        enableChain_ = renderList_.IsEnableChain();
    }
    renderList_.MarkNeedLayout();
}

LayoutParam ListLayoutManager::MakeInnerLayoutParam(FlexAlign crossAxisAlign) const
{
    LayoutParam innerLayout;
    Size maxSize;
    Size minSize;
    if (itemExtent_.IsValid()) {
        double extent = itemExtent_.Value();
        if (itemExtent_.Unit() == DimensionUnit::PERCENT) {
            extent *= renderList_.GetMainSize(viewPort_);
        } else {
            extent = renderList_.NormalizeToPx(itemExtent_);
        }
        if (direction_ == FlexDirection::ROW) {
            maxSize = Size(extent, renderList_.GetLayoutParam().GetMaxSize().Height());
            minSize = Size(extent, renderList_.GetLayoutParam().GetMinSize().Height());
        } else {
            maxSize = Size(renderList_.GetLayoutParam().GetMaxSize().Width(), extent);
            minSize = Size(renderList_.GetLayoutParam().GetMinSize().Width(), extent);
        }
        if (crossAxisAlign == FlexAlign::STRETCH) {
            minSize = maxSize;
        }
    } else {
        if (direction_ == FlexDirection::ROW) {
            maxSize = Size(Size::INFINITE_SIZE, renderList_.GetLayoutParam().GetMaxSize().Height());
            if (crossAxisAlign == FlexAlign::STRETCH) {
                minSize = Size(0.0, renderList_.GetLayoutParam().GetMaxSize().Height());
            } else {
                minSize = Size(0.0, renderList_.GetLayoutParam().GetMinSize().Height());
            }
        } else {
            maxSize = Size(renderList_.GetLayoutParam().GetMaxSize().Width(), Size::INFINITE_SIZE);
            if (crossAxisAlign == FlexAlign::STRETCH) {
                minSize = Size(renderList_.GetLayoutParam().GetMaxSize().Width(), 0.0);
            } else {
                minSize = Size(renderList_.GetLayoutParam().GetMinSize().Width(), 0.0);
            }
        }
    }
    if (NearEqual(renderList_.GetCrossSize(minSize), Size::INFINITE_SIZE)) {
        renderList_.SetCrossSize(minSize, renderList_.GetCrossSize(viewPort_));
    }
    if (NearEqual(renderList_.GetCrossSize(maxSize), Size::INFINITE_SIZE)) {
        renderList_.SetCrossSize(maxSize, renderList_.GetCrossSize(viewPort_));
    }
    innerLayout.SetMaxSize(maxSize);
    innerLayout.SetMinSize(minSize);
    return innerLayout;
}

void ListLayoutManager::CalculateCachedRange(
    int32_t viewBegin, int32_t viewEnd, int32_t cachedCount, int32_t& cachedBegin, int32_t& cachedEnd)
{
    cachedBegin = (viewBegin - cachedCount > 0) ? (viewBegin - cachedCount) : 0;
    if (length_ != LIST_LENGTH_INFINITE) {
        cachedEnd = (viewEnd + cachedCount > length_) ? length_ : viewEnd + cachedCount;
    } else {
        cachedEnd = viewEnd + cachedCount;
    }
}

void ListLayoutManager::RequestMoreItemsIfNeeded(int32_t viewBegin, int32_t viewEnd)
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

void ListLayoutManager::RefreshLayout()
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
                        itemPosition_.erase(item->GetIndex());
                        itemGroupsExpand_.erase(item->GetIndex());
                        itemGroupsFocusIndex_.erase(item->GetIndex());
                        ++rmCount;
                    } else {
                        if (rmCount > 0) {
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

void ListLayoutManager::NotifyNeedRefresh()
{
    if (isAnimating_) {
        controller_->ClearStopListeners();
        controller_->ClearInterpolators();
        controller_->Stop();
        isAnimating_ = false;
        friction_ = MAX_FRICTION;
        RefreshLayout();
        mainOffset_ = -1.0;
    }
}

bool ListLayoutManager::CheckNeedAnimation()
{
    // First layout all item is OP(Add), no need to animate.
    if (firstLayout_) {
        RefreshLayout();
        return false;
    }

    // If current refresh tag is false, do not trigger animate.
    if (!needRefresh_) {
        return false;
    }

    // If current is Playing animation, do not trigger again.
    if (isAnimating_) {
        return false;
    }

    // Check all the items Whether bring ADD/DEL Operation.
    bool needAnimation = false;
    auto iter = renderList_.GetItems().begin();
    while (iter != renderList_.GetItems().end()) {
        auto item = RenderListItem::GetRenderListItem(iter->second);
        if (item && item->GetOperation() != LIST_ITEM_OP_NONE) {
            needAnimation = true;
            break;
        }
        iter++;
    }
    if (!needAnimation) {
        return false;
    }

    // create the animate controller.
    if (!controller_) {
        auto context = renderList_.GetContext().Upgrade();
        if (!context) {
            return false;
        }
        controller_ = AceType::MakeRefPtr<Animator>(context);
    }
    return true;
}

void ListLayoutManager::AnimationForItemUpdate()
{
    if (!CheckNeedAnimation()) {
        return;
    }

    if (!controller_->IsStopped() || isAnimating_) {
        controller_->Stop();
        isAnimating_ = false;
    }
    controller_->ClearStopListeners();
    controller_->ClearInterpolators();
    auto animation = AceType::MakeRefPtr<CurveAnimation<double>>(MIN_FRICTION, MAX_FRICTION, Curves::FRICTION);
    animation->AddListener([weakList = AceType::WeakClaim(this)](double value) {
        auto list = weakList.Upgrade();
        if (list) {
            list->friction_ = value;
            list->renderList_.MarkNeedLayout(true);
        }
    });
    controller_->AddInterpolator(animation);
    controller_->SetDuration(ADD_DEL_DURATION);
    controller_->Play();
    isAnimating_ = true;
    friction_ = MIN_FRICTION;
    controller_->AddStopListener([weakList = AceType::WeakClaim(this)]() {
        auto list = weakList.Upgrade();
        if (list) {
            list->isAnimating_ = false;
            list->friction_ = MAX_FRICTION;
            list->RefreshLayout();
            list->mainOffset_ = -1.0;
            list->renderList_.RefreshScrollExtent();
        }
    });
}

LayoutParam ListLayoutManager::AdjustLayoutParam(const RefPtr<RenderNode> child, LayoutParam param)
{
    LayoutParam layoutParam = param;
    if (!isAnimating_) {
        return layoutParam;
    }
    auto listItem = RenderListItem::GetRenderListItem(child);
    if (!listItem) {
        return layoutParam;
    }
    double symbol = 1.0;
    double friction = 0.0;
    switch (listItem->GetOperation()) {
        case LIST_ITEM_OP_ADD: {
            friction = friction_;
            break;
        }
        case LIST_ITEM_OP_REMOVE: {
            friction = 1.0 - friction_;
            symbol = -1.0;
            break;
        }
        default: {
            return layoutParam;
        }
    }

    Size layoutSize = listItem->GetPreLayoutSize();
    if (layoutSize == Size(0.0, 0.0)) {
        child->Layout(param);
        layoutSize = child->GetLayoutSize();
        listItem->SetPreLayoutSize(layoutSize);
    }
    double listMainSize = renderList_.GetMainSize(renderList_.GetLayoutSize());
    double viewPortSize = renderList_.GetMainSize(viewPort_);
    double maxOffset = std::max(0.0, listMainSize - viewPortSize);
    // Only remove will adjust current offset.
    if (symbol < 0.0 && listItem->GetIndex() >= renderList_.GetCurrentMaxIndex()) {
        if (mainOffset_ < 0.0) {
            mainOffset_ = -renderList_.GetMainPosition(position_);
        }
        double offset = mainOffset_ + renderList_.GetMainSize(layoutSize) * friction_ * symbol;
        offset = std::clamp(offset, 0.0, maxOffset);
        renderList_.RefreshOffset(offset);
        ResetLayoutRange(head_, tail_, renderList_.MakeValue<Offset>(-offset, 0.0), viewPort_);
    } else if (maxOffset < -renderList_.GetMainPosition(position_)) {
        renderList_.RefreshOffset(maxOffset);
        ResetLayoutRange(head_, tail_, renderList_.MakeValue<Offset>(-maxOffset, 0.0), viewPort_);
    }
    Size size = isVertical_ ? Size(layoutSize.Width(), layoutSize.Height() * friction)
                            : Size(layoutSize.Width() * friction, layoutSize.Height());
    layoutParam.SetMinSize(size);
    layoutParam.SetMaxSize(size);
    return layoutParam;
}

double ListLayoutManager::FlushChainAnimation()
{
    if (!enableChain_ || !chain_ || !chainAdapter_) {
        return 0.0;
    }
    double deltaDistance = 0.0;
    bool needSetValue = false;
    RefPtr<RenderNode> parentNode = renderList_.GetParent().Upgrade();
    RefPtr<RenderMultiChildScroll> scroll = AceType::DynamicCast<RenderMultiChildScroll>(parentNode);
    if (!scroll) {
        LOGE("Flush chain animation failed, Get Parent failed.");
        return 0.0;
    }
    bool overScroll = scroll->IsSpringMotionRunning();
    if (chainOverScroll_ != overScroll) {
        if (overScroll) {
            const auto& springProperty = renderList_.GetOverSpringProperty();
            if (springProperty && springProperty->IsValid()) {
                chain_->SetControlStiffness(springProperty->Stiffness());
                chain_->SetControlDamping(springProperty->Damping());
            }
        } else {
            chain_->SetControlStiffness(renderList_.GetChainProperty().ControlStiffness());
            chain_->SetControlDamping(renderList_.GetChainProperty().ControlDamping());
        }
        chain_->OnControlNodeChange();
        chainOverScroll_ = overScroll;
    }
    chain_->FlushAnimation();
    if (renderList_.GetDragStartIndexPending() != renderList_.GetDragStartIndex()) {
        deltaDistance =
            chainAdapter_->ResetControl(renderList_.GetDragStartIndexPending() - renderList_.GetDragStartIndex());
        LOGD("Switch chain control node. %{public}d -> %{public}d, deltaDistance: %{public}.1f",
            renderList_.GetDragStartIndex(), renderList_.GetDragStartIndexPending(), deltaDistance);
        renderList_.SetDragStartIndex(renderList_.GetDragStartIndexPending());
        chainAdapter_->SetDeltaValue(-deltaDistance);
        chainOffset_ += -deltaDistance;
        needSetValue = true;
    }
    if (!NearZero(renderList_.GetCurrentDelta())) {
        LOGD("Set delta chain value. delta: %{public}.1f", renderList_.GetCurrentDelta());
        chainAdapter_->SetDeltaValue(renderList_.GetCurrentDelta());
        chainOffset_ += renderList_.GetCurrentDelta();
        renderList_.ResetCurrentDelta();
        needSetValue = true;
    }
    if (needSetValue) {
        LOGD("FlushChainAnimation: %{public}s", chainAdapter_->DumpNodes().c_str());
        chain_->SetValue(0.0);
    }
    return deltaDistance;
}

void ListLayoutManager::PerformLayout()
{
    LOGD("PerformLayout head:%{public}lf tail:%{public}lf", head_, tail_);
    if (renderList_.GetAddDeleteEffect()) {
        AnimationForItemUpdate();
    } else {
        RefreshLayout();
    }

    int32_t itemIndex = GetIndexByPosition(head_ - chainOffset_);
    int32_t firstIndex = itemIndex;
    renderList_.RecycleHead(itemIndex - 1); // Recycle head items.
    double curMainSize = GetItemPosition(itemIndex);
    double curAnimateMainSize = curMainSize + GetItemAnimationValue(itemIndex);
    LayoutParam innerLayout = MakeInnerLayoutParam(crossAxisAlign_);
    auto itemChild = renderList_.GetChildByIndex(itemIndex);
    while (!itemChild && curAnimateMainSize < tail_ - chainOffset_ && CheckItemPosition(itemIndex)) {
        itemChild = renderList_.GetChildByIndex(itemIndex);
        curMainSize = GetItemPosition(itemIndex);
        curAnimateMainSize = curMainSize + GetItemAnimationValue(itemIndex);
        ++itemIndex;
    }
    while (itemChild) {
        UpdateItemGroupAttr(itemChild);
        itemChild->Layout(AdjustLayoutParam(itemChild, innerLayout));
        if (enableChain_ && firstIndex != itemIndex) {
            curMainSize += chainInterval_ * HALF_ITEM_SIZE;
        }
        itemPosition_[itemIndex] = curMainSize;
        SetChildPosition(itemChild, itemIndex, curMainSize);
        curAnimateMainSize = renderList_.GetMainPosition(itemChild->GetPosition() - position_);
        double childLayoutSize = renderList_.GetMainSize(itemChild->GetLayoutSize());
        curMainSize += childLayoutSize;
        curAnimateMainSize += childLayoutSize;
        if (curAnimateMainSize >= tail_ - chainOffset_) {
            break;
        }
        itemChild = renderList_.GetChildByIndex(++itemIndex);
        if (enableChain_ && itemChild) {
            curMainSize += chainInterval_ * HALF_ITEM_SIZE;
        }
    }
    RequestMoreItemsIfNeeded(firstIndex, itemIndex);
    renderList_.RecycleTail(itemIndex + 1); // Recycle tail items.

    Size layoutSize = renderList_.MakeValue<Size>(curMainSize, renderList_.GetCrossSize(viewPort_));
    renderList_.SetLayoutSize(layoutSize);
    renderList_.CalculateStickyItem(position_);
    ShowItemFocusAnimation();
    firstLayout_ = false;
    chainOffset_ = 0.0;
}

void ListLayoutManager::MoveItemToViewPort(double position)
{
    RefPtr<RenderNode> item = renderList_.GetChildByPosition(position);
    if (!item) {
        LOGE("[ListFocus]MoveItemToViewPort, Get item failed.");
        return;
    }
    double size = renderList_.GetMainSize(item->GetLayoutSize());

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

void ListLayoutManager::MoveItemGroupToViewPort(double position, double size)
{
    // jump to this item using position
    RefPtr<RenderNode> parentNode = renderList_.GetParent().Upgrade();
    RefPtr<RenderMultiChildScroll> scroll = AceType::DynamicCast<RenderMultiChildScroll>(parentNode);
    if (!scroll) {
        LOGE("[ListFocus]MoveItemGroupToViewPort, Get Parent failed.");
        return;
    }
    double focusEffectWidth = size * (TV_ITEM_SCALE - DEFAULT_SCALE) * HALF_ITEM_SIZE;
    double animationOffset = scroll->NormalizeToPx(FOCUS_BOUNDARY);
    scroll->MoveItemToViewPort(position, size, animationOffset + focusEffectWidth);
    LOGD("[ListFocus][list] Direction:%{public}d, Item position:%{public}.1lf.", direction_, position);
}

void ListLayoutManager::ShowItemFocusAnimation()
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
                auto focusItemGroup = AceType::DynamicCast<RenderListItemGroup>(focusItem);
                if (focusItemGroup) {
                    focusItemGroup->ShowFocusAnimation();
                } else {
                    focusItem->ShowFocusAnimation(true, Rect(renderList_.GetGlobalOffset(), viewPort_));
                }
            }
        }
    }
}

int32_t ListLayoutManager::focusMove(KeyDirection direction)
{
    int32_t index = focusMove_;
    switch (direction) {
        case KeyDirection::UP:
        case KeyDirection::DOWN: {
            auto next = renderList_.GetItemByIndex(direction == KeyDirection::UP ? --index : ++index);
            if (next) {
                return index;
            }
            break;
        }
        default:
            break;
    }
    return -1;
}

void ListLayoutManager::LayoutToItem(int32_t toIndex)
{
    int32_t curHeadIndex = renderList_.GetCurrentMinIndex();
    int32_t curTailIndex = renderList_.GetCurrentMaxIndex();
    curTailIndex = std::max(curTailIndex, 0);
    double curMainSize = GetItemPosition(curTailIndex);
    LayoutParam innerLayout = MakeInnerLayoutParam(crossAxisAlign_);
    auto itemChild = renderList_.GetChildByIndex(curTailIndex);
    while (itemChild) {
        itemChild->Layout(innerLayout);
        if (enableChain_ && curTailIndex != curHeadIndex) {
            curMainSize += chainInterval_ * HALF_ITEM_SIZE;
        }
        SetChildPosition(itemChild, curTailIndex, curMainSize);
        itemPosition_[curTailIndex] = curMainSize;
        curMainSize += renderList_.GetMainSize(itemChild->GetLayoutSize());
        if (curTailIndex >= toIndex) {
            break; // Already layout to the target index.
        }
        itemChild = renderList_.GetChildByIndex(++curTailIndex);
        renderList_.RecycleHead(curHeadIndex++);
        if (enableChain_ && itemChild) {
            curMainSize += chainInterval_ * HALF_ITEM_SIZE;
        }
    }
    curMainSize = AdjustLayoutSize(curMainSize);
    Size layoutSize = renderList_.MakeValue<Size>(curMainSize, renderList_.GetCrossSize(viewPort_));
    renderList_.SetLayoutSize(layoutSize);
}

void ListLayoutManager::LayoutToPosition(double position)
{
    int32_t curHeadIndex = renderList_.GetCurrentMinIndex();
    int32_t curTailIndex = renderList_.GetCurrentMaxIndex();
    curTailIndex = std::max(curTailIndex, 0);
    double curMainSize = GetItemPosition(curTailIndex);
    LayoutParam innerLayout = MakeInnerLayoutParam(crossAxisAlign_);
    auto itemChild = renderList_.GetChildByIndex(curTailIndex);
    while (itemChild) {
        itemChild->Layout(innerLayout);
        if (enableChain_ && curTailIndex != curHeadIndex) {
            curMainSize += chainInterval_ * HALF_ITEM_SIZE;
        }
        SetChildPosition(itemChild, curTailIndex, curMainSize);
        itemPosition_[curTailIndex] = curMainSize;
        curMainSize += renderList_.GetMainSize(itemChild->GetLayoutSize());
        if (curMainSize >= position) {
            break; // Already layout to the target position.
        }
        itemChild = renderList_.GetChildByIndex(++curTailIndex);
        renderList_.RecycleHead(curHeadIndex++);
        if (enableChain_ && itemChild) {
            curMainSize += chainInterval_ * HALF_ITEM_SIZE;
        }
    }
    curMainSize = AdjustLayoutSize(curMainSize);
    Size layoutSize = renderList_.MakeValue<Size>(curMainSize, renderList_.GetCrossSize(viewPort_));
    renderList_.SetLayoutSize(layoutSize);
}

void ListLayoutManager::LayoutMore(double incDistance)
{
    int32_t curHeadIndex = renderList_.GetCurrentMinIndex();
    // Use to load about one page size, so not need to recycle child.
    int32_t curTailIndex = renderList_.GetCurrentMaxIndex();
    curTailIndex = std::max(curTailIndex, 0);
    double curMainSize = GetItemPosition(curTailIndex);
    LayoutParam innerLayout = MakeInnerLayoutParam(crossAxisAlign_);
    auto itemChild = renderList_.GetChildByIndex(curTailIndex);
    double incMainSize = 0.0;
    while (itemChild) {
        itemChild->Layout(innerLayout);
        if (enableChain_ && curTailIndex != curHeadIndex) {
            curMainSize += chainInterval_ * HALF_ITEM_SIZE;
        }
        SetChildPosition(itemChild, curTailIndex, curMainSize);
        itemPosition_[curTailIndex] = curMainSize;
        curMainSize += renderList_.GetMainSize(itemChild->GetLayoutSize());
        incMainSize += renderList_.GetMainSize(itemChild->GetLayoutSize());
        if (incMainSize >= incDistance) {
            break; // Already layout enough main size.
        }
        itemChild = renderList_.GetChildByIndex(++curTailIndex);
        if (enableChain_ && itemChild) {
            curMainSize += chainInterval_ * HALF_ITEM_SIZE;
        }
    }
    curMainSize = AdjustLayoutSize(curMainSize);
    Size layoutSize = renderList_.MakeValue<Size>(curMainSize, renderList_.GetCrossSize(viewPort_));
    renderList_.SetLayoutSize(layoutSize);
}

void ListLayoutManager::CalculateFocusIndexPosition()
{
    double focusPosition = GetItemPosition(focusIndex_);
    if (NearZero(focusPosition) && focusIndex_ != 0) {
        renderList_.CalculateItemPosition(focusIndex_, ScrollType::SCROLL_INDEX);
    }
}

void ListLayoutManager::SetChildPosition(const RefPtr<RenderNode>& child, int32_t index, double mainSize)
{
    double mainLen = renderList_.GetMainSize(viewPort_);
    double crossLen = renderList_.GetCrossSize(viewPort_);
    double mainAxis = mainSize;
    double crossAxis = 0.0;
    auto listItemChild = RenderListItem::GetRenderListItem(child);
    FlexAlign selfAlign = listItemChild ? listItemChild->GetSelfAlign() : FlexAlign::AUTO;
    FlexAlign align = selfAlign == FlexAlign::AUTO ? crossAxisAlign_ : selfAlign;
    // second layout for self align.
    if (crossAxisAlign_ == FlexAlign::STRETCH && selfAlign != FlexAlign::AUTO && selfAlign != FlexAlign::STRETCH) {
        auto innerLayout = MakeInnerLayoutParam(align);
        child->Layout(innerLayout);
    }
    if (crossAxisAlign_ != FlexAlign::STRETCH && selfAlign == FlexAlign::STRETCH) {
        auto innerLayout = MakeInnerLayoutParam(align);
        child->Layout(innerLayout);
    }
    if (enableChain_) {
        mainAxis += GetChainDelta(index);
    }
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

    if (isVertical_) {
        if (direction_ == FlexDirection::COLUMN_REVERSE) {
            mainAxis = mainLen - renderList_.GetMainSize(child->GetLayoutSize()) - mainAxis;
        }
        child->SetPosition(Offset(crossAxis, mainAxis) + position_);
    } else {
        if (IsRowReverse()) {
            mainAxis = mainLen - renderList_.GetMainSize(child->GetLayoutSize()) - mainAxis;
        }
        child->SetPosition(Offset(mainAxis, crossAxis) + position_);
    }
    LOGD("Child[%{public}d]:%{public}lf %{public}lf %{public}s %{public}s", index, mainAxis, mainSize,
        position_.ToString().c_str(), child->GetPosition().ToString().c_str());
}

void ListLayoutManager::UpdateItemGroupAttr(RefPtr<RenderNode>& itemChild)
{
    auto renderListItem = RenderListItem::GetRenderListItem(itemChild);
    if (renderListItem) {
        auto renderListItemGroup = AceType::DynamicCast<RenderListItemGroup>(renderListItem);
        if (renderListItemGroup) {
            int32_t groupIndex = renderListItemGroup->GetIndex();
            bool expand = GetExpandStatus(groupIndex);
            renderListItemGroup->SetExpand(expand);
            AddItemGroupExpand(groupIndex, expand);
            if (expand) {
                int32_t groupFocusIndex = GetItemGroupFocusIndex(groupIndex);
                AddItemGroupFocusIndex(groupIndex, groupFocusIndex);
            } else {
                AddItemGroupFocusIndex(groupIndex, 0);
            }
            renderListItemGroup->ChangeDirection(renderList_.GetDirection());
            renderListItemGroup->SetRightToLeft(rightToLeft_);
        }
    }
}

double ListLayoutManager::GetChainDelta(int32_t index) const
{
    if (!chainAdapter_) {
        return 0.0;
    }
    double value = 0.0;
    RefPtr<BilateralSpringNode> node;
    int32_t controlIndex = renderList_.GetDragStartIndex();
    int32_t baseIndex = controlIndex - chainAdapter_->GetControlIndex();
    int32_t targetIndex = std::clamp(index - baseIndex, 0, CHAIN_ANIMATION_NODE_COUNT - 1);
    node = AceType::DynamicCast<BilateralSpringNode>(chainAdapter_->GetNode(targetIndex));
    if (node) {
        value = node->GetValue();
    }
    LOGD("ChainDelta. controlIndex: %{public}d, index: %{public}d, value: %{public}.3lf", controlIndex, index, value);
    return value;
}

void ListLayoutManager::InitChainAnimation(int32_t nodeCount)
{
    auto context = renderList_.GetContext().Upgrade();
    if (!context) {
        LOGE("Init chain animation failed. context is null");
        return;
    }

    if (chainAdapter_ && chain_) {
        return;
    }
    chainAdapter_ = AceType::MakeRefPtr<BilateralSpringAdapter>();
    chain_ = AceType::MakeRefPtr<SimpleSpringChain>(chainAdapter_);
    const auto& property = renderList_.GetChainProperty();
    chain_->SetFrameDelta(property.FrameDelay());
    if (property.StiffnessTransfer()) {
        chain_->SetStiffnessTransfer(AceType::MakeRefPtr<ExpParamTransfer>(property.StiffnessCoefficient()));
    } else {
        chain_->SetStiffnessTransfer(AceType::MakeRefPtr<LinearParamTransfer>(property.StiffnessCoefficient()));
    }
    if (property.DampingTransfer()) {
        chain_->SetDampingTransfer(AceType::MakeRefPtr<ExpParamTransfer>(property.DampingCoefficient()));
    } else {
        chain_->SetDampingTransfer(AceType::MakeRefPtr<LinearParamTransfer>(property.DampingCoefficient()));
    }
    chain_->SetControlDamping(property.ControlDamping());
    chain_->SetControlStiffness(property.ControlStiffness());
    chain_->SetDecoration(context->NormalizeToPx(renderList_.GetChainProperty().Interval()));
    chain_->SetMinDecoration(context->NormalizeToPx(renderList_.GetChainProperty().MinInterval()));
    chain_->SetMaxDecoration(context->NormalizeToPx(renderList_.GetChainProperty().MaxInterval()));
    for (int32_t index = 0; index < nodeCount; index++) {
        auto node = AceType::MakeRefPtr<BilateralSpringNode>(renderList_.GetContext(), index, 0.0);
        WeakPtr<BilateralSpringNode> nodeWeak(node);
        WeakPtr<SimpleSpringAdapter> adapterWeak(chainAdapter_);
        node->AddUpdateListener(
            [weak = AceType::WeakClaim(this), nodeWeak, adapterWeak](double value, double velocity) {
                auto manager = weak.Upgrade();
                auto node = nodeWeak.Upgrade();
                auto adapter = adapterWeak.Upgrade();
                if (!manager || !node || !adapter) {
                    return;
                }
                if (node->GetIndex() == adapter->GetControlIndex()) {
                    manager->controlValue_ = std::abs(value);
                }
                manager->renderList_.MarkNeedLayout();
            });
        chainAdapter_->AddNode(node);
    }
    chainAdapter_->NotifyControlIndexChange();
}

double ListLayoutManager::GetItemAnimationValue(int32_t index) const
{
    return enableChain_ ? GetChainDelta(index) : 0.0;
}

} // namespace OHOS::Ace