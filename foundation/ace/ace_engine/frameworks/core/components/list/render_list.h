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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_RENDER_LIST_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_RENDER_LIST_H

#include <functional>
#include <map>
#include <vector>

#include "core/animation/simple_spring_chain.h"
#include "core/components/common/layout/constants.h"
#include "core/components/list/layout_manager.h"
#include "core/components/list/render_list_item.h"
#include "core/components/scroll/render_scroll.h"
#include "core/components/scroll/scrollable.h"

namespace OHOS::Ace {

class ListComponent;

constexpr int32_t INVALID_INDEX = -1;
constexpr int32_t ZERO_INDEX = 0;

using RotationEventFunc = std::function<void(const RotationEvent& info)>;

class RenderList : public RenderNode {
    DECLARE_ACE_TYPE(RenderList, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;

    void UpdateTouchRect() override;

    void PerformLayout() override;

    void OnPaintFinish() override;

    bool TouchTest(const Point& globalPoint, const Point& parentLocalPoint, const TouchRestrict& touchRestrict,
        TouchTestResult& result) override;

    void ResetLayoutRange(double head, double tail, Offset position, Size viewport);

    using RequestListDataFunc = std::function<void(int32_t index, int32_t count)>;
    void RegisterRequestItemsCallback(const RequestListDataFunc& requestItems)
    {
        requestItems_ = requestItems;
    }

    using StickyItemBuildFunc = std::function<RefPtr<RenderNode>(int32_t index, bool next)>;
    void RegisterStickyItemBuilderCallback(const StickyItemBuildFunc& stickyItemBuilder)
    {
        stickyItemBuilder_ = stickyItemBuilder;
    }

    using StickyItemSearchFunc = std::function<int32_t(int32_t index)>;
    void RegisterStickyItemSearcherCallback(const StickyItemSearchFunc& stickyItemSearcher)
    {
        stickyItemSearcher_ = stickyItemSearcher;
    }

    using RecycleByRangeFunc = std::function<bool(int32_t& from, int32_t& to)>;
    void RegisterRecycleByRangeCallback(const RecycleByRangeFunc& recycle)
    {
        recycleByRange_ = recycle;
    }

    using RecycleByItemsFunc = std::function<bool(const std::vector<int32_t>& items)>;
    void RegisterRecycleByItemsCallback(const RecycleByItemsFunc& recycle)
    {
        recycleByItems_ = recycle;
    }

    using BuildListDataFunc = std::function<bool(int32_t index)>;
    void RegisterBuildItemCallback(const BuildListDataFunc& buildItem)
    {
        buildItem_ = buildItem;
    }

    using OnRefreshedFunc = std::function<void()>;
    void RegisterOnRefreshedCallback(const OnRefreshedFunc& onRefreshed)
    {
        onRefreshed_ = onRefreshed;
    }

    void AddListItem(int32_t index, const RefPtr<RenderNode>& renderNode);

    template<typename T>
    T MakeValue(double mainValue, double crossValue) const
    {
        return direction_ == FlexDirection::ROW ? T(mainValue, crossValue) : T(crossValue, mainValue);
    }

    void SetMainSize(Size& size, double mainValue)
    {
        direction_ == FlexDirection::ROW ? size.SetWidth(mainValue) : size.SetHeight(mainValue);
    }

    void SetCrossSize(Size& size, double crossValue)
    {
        direction_ == FlexDirection::ROW ? size.SetHeight(crossValue) : size.SetWidth(crossValue);
    }

    double GetMainSize(const Size& size) const
    {
        return direction_ == FlexDirection::ROW ? size.Width() : size.Height();
    }

    double GetCrossSize(const Size& size) const
    {
        return direction_ == FlexDirection::ROW ? size.Height() : size.Width();
    }

    double GetMainPosition(const Offset& offset) const
    {
        return direction_ == FlexDirection::ROW ? offset.GetX() : offset.GetY();
    }

    bool IsLayoutChanged() const
    {
        return IsLayoutParamChanged();
    }

    int32_t GetMaxCount() const
    {
        return maxCount_;
    }

    void SetMaxCount(int32_t maxCount)
    {
        maxCount_ = maxCount;
    }

    int32_t GetCurrentMinIndex() const
    {
        int32_t currentMin = -1;
        if (items_.begin() != items_.end()) {
            currentMin = items_.begin()->first;
        }
        return currentMin;
    }

    int32_t GetCurrentMaxIndex() const
    {
        int32_t currentMax = -1;
        if (items_.rbegin() != items_.rend()) {
            currentMax = items_.rbegin()->first;
        }
        return currentMax;
    }

    FlexDirection GetDirection() const
    {
        return direction_;
    }

    int32_t GetCachedCount() const
    {
        return cachedCount_;
    }

    int32_t GetBeginIndex() const
    {
        return beginIndex_;
    }

    int32_t GetEndIndex() const
    {
        return endIndex_;
    }

    int32_t GetRepeatedLength() const
    {
        return repeatLength_;
    }

    int32_t GetLength() const
    {
        return length_;
    }

    void SetLength(int32_t length)
    {
        length_ = length;
    }

    int32_t GetIndexOffset() const
    {
        return indexOffset_;
    }

    FlexAlign GetFlexAlign() const
    {
        return crossAxisAlign_;
    }

    int32_t GetColumnCount() const
    {
        return columnCount_;
    }

    int32_t GetColumnExtent() const
    {
        return columnExtent_;
    }

    double GetWidth() const
    {
        return listWidth_;
    }

    double GetHeight() const
    {
        return listHeight_;
    }

    const Dimension& GetItemExtent() const
    {
        return itemExtent_;
    }

    bool GetRightToLeft() const
    {
        return rightToLeft_;
    }

    const std::map<int32_t, RefPtr<RenderNode>>& GetItems() const
    {
        return items_;
    }

    void ResetItems(std::map<int32_t, RefPtr<RenderNode>>& items)
    {
        items_.clear();
        items_.swap(items);
    }

    void CalculateStickyItem(const Offset& position);

    void RequestMoreItems(int32_t index, int32_t count);

    int32_t RequestNextFocus(bool vertical, bool reverse);

    void ListItemFocused(int32_t focusIndex);

    void MoveItemToViewPort(double position);

    void MoveItemGroupToViewPort(double position, double size);

    RefPtr<RenderListItem> GetItemByIndex(int32_t index);

    RefPtr<RenderNode> GetChildByPosition(double position) const;
    RefPtr<RenderNode> GetNearChildByPosition(double position) const;
    RefPtr<RenderNode> GetChildByIndex(int32_t index);
    int32_t GetIndexByPosition(double position) const;
    bool RecycleByRange(int32_t from, int32_t to);
    bool RecycleByItems(const std::vector<int32_t>& items);
    bool RecycleAllChild();
    void RecycleHead(int32_t head);
    void RecycleTail(int32_t tail);
    void SyncIndex(int32_t begin, int32_t end);

    void CalculateFocusIndexPosition();

    double CalculateItemPosition(int32_t index, ScrollType type);

    void CalculateItemPosition(double targetPos);

    void MarkNeedRefresh();

    void RefreshOffset(double offset);

    void RefreshScrollExtent();

    void SetGroupState(int32_t index, bool expand);

    void OnRefreshed()
    {
        onRefreshed_();
    }

    const RefPtr<LayoutManager>& GetLayoutManager() const
    {
        return layoutManager_;
    }

    double GetItemPosition(int32_t index) const
    {
        if (layoutManager_) {
            return layoutManager_->GetItemPosition(index);
        }
        return 0.0;
    }

    double GetItemAnimationPosition(int32_t index) const
    {
        if (layoutManager_) {
            return layoutManager_->GetItemAnimationValue(index);
        }
        return 0.0;
    }

    const RefPtr<ScrollPositionController>& GetController() const
    {
        return controller_;
    }

    bool IsSupportScale() const
    {
        return itemScale_;
    }

    void SetSupportItemCenter(bool center)
    {
        supportItemCenter_ = center;
    }

    bool GetSupportItemCenter() const
    {
        return supportItemCenter_;
    }

    bool SupportStickyItem() const
    {
        return !chainAnimation_ && direction_ == FlexDirection::COLUMN;
    }

    bool IsCenterLayout() const
    {
        return isCenterLayout_;
    }

    bool IsPageReady() const
    {
        return pageReady_;
    }

    bool GetAddDeleteEffect() const
    {
        return updateEffect_ && pageReady_;
    }

    void SetAddDeleteEffect(bool enabled)
    {
        updateEffect_ = enabled;
    }

    void BuildNextItem(double start, double end, Offset position, Size viewPort);

    void SetShiftHeight(double shiftHeight)
    {
        shiftHeight_ = shiftHeight;
    }

    // notify start position in global main axis
    void NotifyDragStart(double startPosition);

    // notify drag offset in global main axis
    void NotifyDragUpdate(double dragOffset);

    void NotifyScrollOver(double velocity, bool isCrashTop, bool isCrashBottom);

    bool IsEnableChain() const
    {
        return chainAnimation_;
    }

    int32_t GetDragStartIndexPending() const
    {
        return dragStartIndexPending_;
    }

    void SetDragStartIndexPending(int32_t index)
    {
        dragStartIndexPending_ = index;
    }

    int32_t GetDragStartIndex() const
    {
        return dragStartIndex_;
    }

    void SetDragStartIndex(int32_t index)
    {
        dragStartIndex_ = index;
    }

    double GetCurrentDelta() const
    {
        return currentDelta_;
    }

    // Reset when performLayout done.
    void ResetCurrentDelta()
    {
        currentDelta_ = 0.0;
    }

    const RefPtr<SpringProperty>& GetOverSpringProperty() const
    {
        return overSpringProperty_;
    }

    const SpringChainProperty& GetChainProperty() const
    {
        return chainProperty_;
    }

    double FlushChainAnimation()
    {
        if (!layoutManager_) {
            return 0.0;
        }
        return layoutManager_->FlushChainAnimation();
    }

    double GetHeadAnimationValue() const
    {
        if (chainAnimation_ && layoutManager_) {
            return layoutManager_->GetItemAnimationValue(GetCurrentMinIndex());
        } else {
            return 0.0;
        }
    }

    bool IsVertical() const
    {
        return direction_ == FlexDirection::COLUMN || direction_ == FlexDirection::COLUMN_REVERSE;
    }

    double GetListPosition() const
    {
        if (layoutManager_) {
            return IsVertical() ? layoutManager_->GetPosition().GetY() : layoutManager_->GetPosition().GetX();
        }
        return 0.0;
    }

    double EstimateIndexByPosition(double position) const
    {
        if (layoutManager_) {
            return layoutManager_->GetIndexByPosition(position);
        }
        return INVALID_INDEX;
    }

    double GetTailAnimationValue() const
    {
        if (chainAnimation_ && layoutManager_) {
            return GetItemAnimationPosition(GetCurrentMaxIndex());
        } else {
            return 0.0;
        }
    }

    void SetMakeCardTransition(bool makeCardTransition)
    {
        makeCardTransition_ = makeCardTransition;
    }

    int32_t GetCenterIndex() const
    {
        return centerIndex_;
    }

    void SetCenterIndex(int32_t centerIndex)
    {
        centerIndex_ = centerIndex;
    }

    bool NeedRefresh() const
    {
        return needRefresh_;
    }

    void SetNeedRefresh(bool needRefresh)
    {
        needRefresh_ = needRefresh;
    }

    void OnChildRemoved(const RefPtr<RenderNode>& child) override;
    void SetOnRotateCallback(const RefPtr<ListComponent>& component);

    const RotationEventFunc& GetOnRotateCallback() const
    {
        return rotationEvent_;
    }

protected:
    RefPtr<RenderNode> GetStickyItem() const
    {
        return stickyItem_;
    }

    RefPtr<RenderNode> GetStickyNext() const
    {
        return stickyNext_;
    }

    Offset GetStickyItemOffset() const
    {
        return stickyItemOffset_;
    }

    Offset GetStickyNextOffset() const
    {
        return stickyNextOffset_;
    }

    double shiftHeight_ = 0.0;
    bool makeCardTransition_ = false;

private:
    double GetStickyMainSize(int32_t index);
    void CalculateStickyItemOffset(int32_t index, double position);
    void UpdateAccessibilityAttr();
    bool HandleActionScroll(bool forward);
    void ResetGroupItem(const RefPtr<RenderNode>& renderNode);

    FlexDirection direction_ { FlexDirection::COLUMN };
    FlexAlign crossAxisAlign_ = FlexAlign::STRETCH;

    int32_t cachedCount_ = 1;
    int32_t beginIndex_ = LIST_PARAM_INVAID;
    int32_t endIndex_ = LIST_PARAM_INVAID;
    int32_t repeatLength_ = 0;
    int32_t length_ = 0;
    int32_t indexOffset_ = 0;
    int32_t maxCount_ = 0;
    int32_t itemsCount_ = 0;
    // center list-item index if item center option is enabled.
    int32_t centerIndex_ = INVALID_INDEX;

    double listWidth_ = -1.0;
    double listHeight_ = -1.0;
    double currentDelta_ = 0.0;
    SpringChainProperty chainProperty_;
    RefPtr<SpringProperty> overSpringProperty_;
    int32_t dragStartIndexPending_ = 0;
    int32_t dragStartIndex_ = 0;
    int32_t columnCount_ = 0;
    int32_t columnExtent_ = 0;
    int32_t firstItemIndex_ = 0;
    Dimension itemExtent_;
    bool rightToLeft_ = false;
    bool updateEffect_ = false;
    bool supportItemCenter_ = false;
    bool isCenterLayout_ = false;
    bool pageReady_ = false;
    bool itemScale_ = false;
    bool chainAnimation_ = false;
    bool needRefresh_ = false;

    std::map<int32_t, RefPtr<RenderNode>> items_;
    RequestListDataFunc requestItems_;
    RecycleByRangeFunc recycleByRange_;
    RecycleByItemsFunc recycleByItems_;
    BuildListDataFunc buildItem_;
    OnRefreshedFunc onRefreshed_;
    RefPtr<ScrollPositionController> controller_;
    RefPtr<LayoutManager> layoutManager_;

    StickyItemBuildFunc stickyItemBuilder_;
    StickyItemSearchFunc stickyItemSearcher_;
    RefPtr<RenderNode> stickyItem_;
    RefPtr<RenderNode> stickyNext_;
    std::map<int32_t, double> stickyItemMap_;
    Offset stickyItemOffset_;
    Offset stickyNextOffset_;
    Offset currentOffset_;
    RotationEventFunc rotationEvent_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_RENDER_LIST_H
