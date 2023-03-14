/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_LIST_RENDER_LIST_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_LIST_RENDER_LIST_H

#include <functional>
#include <limits>
#include <list>

#include "core/animation/bilateral_spring_adapter.h"
#include "core/animation/simple_spring_chain.h"
#include "core/components/positioned/positioned_component.h"
#include "core/components/scroll/scroll_edge_effect.h"
#include "core/components/scroll/scrollable.h"
#include "core/components_v2/list/list_component.h"
#include "core/components_v2/list/render_list_item.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace::V2 {

using UpdateBuilderFunc = std::function<void(const Dimension&, const Dimension&)>;

class ListItemGenerator : virtual public Referenced {
public:
    static constexpr size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

    virtual RefPtr<RenderListItem> RequestListItem(size_t index) = 0;
    virtual void RecycleListItem(size_t index) = 0;
    virtual size_t TotalCount() = 0;
    virtual size_t FindPreviousStickyListItem(size_t index) = 0;
};

class RenderList : public RenderNode {
    DECLARE_ACE_TYPE(V2::RenderList, RenderNode);

public:
    using ScrollEventBack = std::function<void(void)>;
    static RefPtr<RenderNode> Create();

    RenderList() = default;
    ~RenderList() override;

    void Update(const RefPtr<Component>& component) override;

    void PerformLayout() override;

    void OnPaintFinish() override;

    bool IsUseOnly() override;

    template<class T>
    T MakeValue(double mainValue, double crossValue) const
    {
        return vertical_ ? T(crossValue, mainValue) : T(mainValue, crossValue);
    }

    double GetMainSize(const Size& size) const
    {
        return vertical_ ? size.Height() : size.Width();
    }

    double GetCrossSize(const Size& size) const
    {
        return vertical_ ? size.Width() : size.Height();
    }

    double GetMainAxis(const Offset& size) const
    {
        return vertical_ ? size.GetY() : size.GetX();
    }

    double GetCrossAxis(const Offset& size) const
    {
        return vertical_ ? size.GetX() : size.GetY();
    }

    double GetSpace() const
    {
        return spaceWidth_;
    }

    double GetStartIndex() const
    {
        return startIndex_;
    }

    double GetIndex() const
    {
        return initialIndex_;
    }

    bool GetDirection() const
    {
        return vertical_;
    }

    Axis GetAxis() const
    {
        return vertical_ ? Axis::VERTICAL : Axis::HORIZONTAL;
    }

    bool GetEditable() const
    {
        if (component_) {
            return component_->GetEditMode();
        }
        return false;
    }

    bool GetLinkage() const
    {
        if (component_) {
            return component_->GetChainAnimation();
        }
        return false;
    }

    void RegisterItemGenerator(WeakPtr<ListItemGenerator>&& listItemGenerator)
    {
        itemGenerator_ = std::move(listItemGenerator);
    }

    void RemoveAllItems();

    void JumpToIndex(int32_t idx, int32_t source);

    void AnimateTo(const Dimension& position, float duration, const RefPtr<Curve>& curve);

    RefPtr<Component> GetComponent() override
    {
        return component_;
    }

    Offset GetLastOffset() const
    {
        return vertical_ ? Offset(0.0, -currentOffset_) : Offset(-currentOffset_, 0.0);
    }

    double GetEstimatedHeight() const
    {
        return realMainSize_;
    }

    Dimension GetListSpace() const
    {
        return listSpace_;
    }

    const UpdateBuilderFunc& GetUpdateBuilderFuncId() const
    {
        return updateBuilder_;
    }

    void SetUpdateBuilderFuncId(const UpdateBuilderFunc& updateBuilder)
    {
        updateBuilder_ = updateBuilder;
    }

    const OnItemDragEnterFunc& GetOnItemDragEnter() const
    {
        return onItemDragEnter_;
    }

    const OnItemDragMoveFunc& GetOnItemDragMove() const
    {
        return onItemDragMove_;
    }

    const OnItemDragLeaveFunc& GetOnItemDragLeave() const
    {
        return onItemDragLeave_;
    }

    const OnItemDropFunc& GetOnItemDrop() const
    {
        return onItemDrop_;
    }

    void SetPreTargetRenderList(const RefPtr<RenderList>& preTargetRenderList)
    {
        preTargetRenderList_ = preTargetRenderList;
    }

    const RefPtr<RenderList>& GetPreTargetRenderList() const
    {
        return preTargetRenderList_;
    }

    void SetBetweenItemAndBuilder(const Offset& betweenItemAndBuilder)
    {
        betweenItemAndBuilder_ = betweenItemAndBuilder;
    }

    const Offset& GetBetweenItemAndBuilder() const
    {
        return betweenItemAndBuilder_;
    }

    size_t CalculateSelectedIndex(
        const RefPtr<RenderList> targetRenderlist, const GestureEvent& info, Size& selectedItemSize);
    size_t CalculateInsertIndex(
        const RefPtr<RenderList> targetRenderlist, const GestureEvent& info, Size selectedItemSize);

    void HandleAxisEvent(const AxisEvent& event) override;

    bool IsAxisScrollable(AxisDirection direction) override;

    WeakPtr<RenderNode> CheckAxisNode() override;

    int32_t RequestNextFocus(bool vertical, bool reverse);

    std::string ProvideRestoreInfo() override;

protected:
    void UpdateAccessibilityAttr();
    bool HandleActionScroll(bool forward);
    LayoutParam MakeInnerLayout();
    Size SetItemsPosition(double mainSize, const LayoutParam& layoutParam);
    bool UpdateScrollPosition(double offset, int32_t source);

    bool TouchTest(const Point& globalPoint, const Point& parentLocalPoint, const TouchRestrict& touchRestrict,
        TouchTestResult& result) override;
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

    double ApplyLayoutParam();
    double LayoutOrRecycleCurrentItems(const LayoutParam& layoutParam, double mainSize);
    RefPtr<RenderListItem> RequestAndLayoutNewItem(size_t index, const LayoutParam& layoutParam);

    RefPtr<RenderListItem> RequestListItem(size_t index);
    void RecycleListItem(size_t index);
    size_t TotalCount();
    size_t FindPreviousStickyListItem(size_t index);

    void OnItemDelete(const RefPtr<RenderListItem>& item);
    void OnItemSelect(const RefPtr<RenderListItem>& item);
    size_t GetIndexByListItem(const RefPtr<RenderListItem>& item) const;
    bool PrepareRawRecognizer();
    void OnSelectedItemMove(double position);
    void OnSelectedItemStopMoving(bool canceled);

    void UpdateStickyListItem(const RefPtr<RenderListItem>& newStickyItem, size_t newStickyItemIndex,
        const RefPtr<RenderListItem>& nextStickyItem, const LayoutParam& layoutParam);

    void ApplyPreviousStickyListItem(
        size_t index, bool forceLayout = false, const LayoutParam& layoutParam = LayoutParam());

    double GetCurrentPosition() const;
    void AdjustOffset(Offset& delta, int32_t source);
    bool IsOutOfBoundary() const;
    void ResetEdgeEffect();
    void SetEdgeEffectAttribute();
    void CalculateMainScrollExtent(double curMainPos, double mainSize);

    // notify start position in global main axis when drag start
    void ProcessDragStart(double startPosition);
    // notify drag offset in global main axis
    void processDragUpdate(double dragOffset);
    // notify scroll over
    void ProcessScrollOverCallback(double velocity);
    void InitChainAnimation(int32_t nodeCount);
    double GetChainDelta(int32_t index) const;
    size_t GetNearChildByPosition(double mainOffset) const;
    double FlushChainAnimation();
    const RefPtr<SpringProperty>& GetOverSpringProperty() const
    {
        return overSpringProperty_;
    }
    const SpringChainProperty& GetChainProperty() const
    {
        return chainProperty_;
    }

    RefPtr<ListComponent> component_;

    static constexpr size_t INVALID_CHILD_INDEX = std::numeric_limits<size_t>::max();
    static constexpr size_t INITIAL_CHILD_INDEX = INVALID_CHILD_INDEX - 1;

    size_t startIndex_ = INITIAL_CHILD_INDEX;
    size_t initialIndex_ = INITIAL_CHILD_INDEX;
    std::list<RefPtr<RenderListItem>> items_;

    double spaceWidth_ = 0.0;
    double startMainPos_ = 0.0;
    double endMainPos_ = 0.0;
    double currentOffset_ = 0.0;
    double mainScrollExtent_ = 0.0;

    bool reachStart_ = false;
    bool reachEnd_ = false;
    bool isOutOfBoundary_ = false;
    bool vertical_ = true;
    bool fixedMainSizeByLayoutParam_ = true;
    bool fixedMainSize_ = true;
    bool fixedCrossSize_ = false;
    bool chainAnimation_ = false;
    bool chainOverScroll_ = false;
    double currentDelta_ = 0.0;

    SpringChainProperty chainProperty_;
    RefPtr<SpringProperty> overSpringProperty_;
    RefPtr<BilateralSpringAdapter> chainAdapter_;
    RefPtr<SimpleSpringChain> chain_;
    RefPtr<Animator> animator_;

    size_t firstDisplayIndex_ = INITIAL_CHILD_INDEX;
    size_t lastDisplayIndex_ = INITIAL_CHILD_INDEX;
    size_t dragStartIndexPending_ = 0;
    size_t dragStartIndex_ = 0;
    bool hasActionScroll_ = false;
    bool isActionByScroll_ = false;
    ScrollEventBack scrollFinishEventBack_;

    WeakPtr<ListItemGenerator> itemGenerator_;
    RefPtr<Scrollable> scrollable_;
    RefPtr<ScrollEdgeEffect> scrollEffect_;
    RefPtr<ScrollBarProxy> scrollBarProxy_;

    size_t currentStickyIndex_ = INITIAL_CHILD_INDEX;
    RefPtr<RenderListItem> currentStickyItem_;

    size_t targetIndex_ = INITIAL_CHILD_INDEX;
    size_t selectedItemIndex_ = INITIAL_CHILD_INDEX;
    RefPtr<RenderListItem> selectedItem_;
    double selectedItemMainAxis_ = 0.0;
    double targetMainAxis_ = 0.0;
    RefPtr<RawRecognizer> rawRecognizer_;
    double lastPos_ = 0.0f;
    bool autoScrollingForItemMove_ = false;
    bool movingForward_ = false;
    double dipScale_ = 1.0;
    double offset_ = 0.0;

    size_t insertItemIndex_ = INITIAL_CHILD_INDEX;
    Offset betweenItemAndBuilder_;
    RefPtr<RenderListItem> selectedDragItem_;

    Offset mouseStartOffset_;
    Offset mouseEndOffset_;
    int32_t focusIndex_ = 0;

private:
    bool ActionByScroll(bool forward, ScrollEventBack scrollEventBack);
    void ModifyActionScroll();
    void InitScrollBarProxy();
    Dimension listSpace_;
    double realMainSize_ = 0.0; // Real size of main axis.
    size_t startCachedCount_ = 0;
    size_t endCachedCount_ = 0;
    size_t cachedCount_ = 0;

    void CreateDragDropRecognizer();
    RefPtr<RenderListItem> FindCurrentListItem(const Point& point);

    RefPtr<GestureRecognizer> dragDropGesture_;
    RefPtr<RenderList> preTargetRenderList_;
    OnItemDragStartFunc onItemDragStart_;
    OnItemDragEnterFunc onItemDragEnter_;
    OnItemDragMoveFunc onItemDragMove_;
    OnItemDragLeaveFunc onItemDragLeave_;
    OnItemDropFunc onItemDrop_;

    UpdateBuilderFunc updateBuilder_;

    bool HandleMouseEvent(const MouseEvent& event) override;
    bool isMultiSelectable_ = false;
    void ClearMultiSelect();
    bool mouseIsHover_ = false;
    bool hasHeight_ = false;
    bool hasWidth_ = false;
    bool isAxisResponse_ = true;

    void MultiSelectWithoutKeyboard(const Rect& selectedZone);
    void HandleMouseEventWithoutKeyboard(const MouseEvent& event);

    void MultiSelectWhenCtrlDown(const Rect& selectedZone);
    void HandleMouseEventWhenCtrlDown(const MouseEvent& event);
    void CollectSelectedItems();
    std::set<RefPtr<RenderListItem>> selectedItemsWithCtrl_;

    void MultiSelectWhenShiftDown(const Rect& selectedZone);
    RefPtr<RenderListItem> GetPressItemWhenShiftDown(const Rect& selectedZone);
    void HandleMouseEventWhenShiftDown(const MouseEvent& event);
    void MultiSelectAllInRange(const RefPtr<RenderListItem>& firstItem,
        const RefPtr<RenderListItem>& secondItem);
    RefPtr<RenderListItem> firstItemWithShift_;
    RefPtr<RenderListItem> secondItemWithShift_;

    void MultiSelectAllWhenCtrlA();

    void ApplyRestoreInfo();

    bool hasDragItem_ = false;

    ACE_DISALLOW_COPY_AND_MOVE(RenderList);
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_LIST_RENDER_LIST_H
