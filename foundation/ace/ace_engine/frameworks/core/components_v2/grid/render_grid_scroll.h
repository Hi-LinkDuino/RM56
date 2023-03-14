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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_GRID_RENDER_GRID_SCROLL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_GRID_RENDER_GRID_SCROLL_H

#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <utility>

#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/scroll_bar.h"
#include "core/components/grid_layout/grid_layout_component.h"
#include "core/components/grid_layout/render_grid_layout.h"
#include "core/components/scroll/scroll_bar_theme.h"
#include "core/components/scroll/scrollable.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace::V2 {

class GridEventInfo : public BaseEventInfo, public EventToJSONStringAdapter {
    DECLARE_RELATIONSHIP_OF_CLASSES(GridEventInfo, BaseEventInfo, EventToJSONStringAdapter);

public:
    explicit GridEventInfo(int32_t scrollIndex) : BaseEventInfo("grid"), scrollIndex_(scrollIndex) {}

    ~GridEventInfo() = default;

    std::string ToJSONString() const override;

    int32_t GetScrollIndex() const
    {
        return scrollIndex_;
    }

private:
    int32_t scrollIndex_ = 0;
};

class RenderGridScroll : public RenderGridLayout {
    DECLARE_ACE_TYPE(RenderGridScroll, RenderGridLayout);

public:
    using BuildChildByIndex = std::function<bool(int32_t)>;
    using GetChildSpanByIndex = std::function<bool(int32_t, bool, int32_t&, int32_t&, int32_t&, int32_t&)>;
    using DeleteChildByIndex = std::function<void(int32_t)>;
    using OnScrolledFunc = std::function<void(std::shared_ptr<GridEventInfo>&)>;

    RenderGridScroll() = default;
    ~RenderGridScroll() override;

    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void OnPredictLayout(int64_t deadline) override;

    const std::list<RefPtr<RenderNode>>& GetChildren() const override
    {
        return childrenInRect_;
    }

    void SetBuildChildByIndex(BuildChildByIndex func)
    {
        buildChildByIndex_ = std::move(func);
    }

    void SetDeleteChildByIndex(DeleteChildByIndex func)
    {
        deleteChildByIndex_ = std::move(func);
    }

    void SetGetChildSpanByIndex(GetChildSpanByIndex func)
    {
        getChildSpanByIndex_ = std::move(func);
    }

    void AddChildByIndex(int32_t index, const RefPtr<RenderNode>& renderNode);
    void RemoveChildByIndex(int32_t index)
    {
        auto item = items_.find(index);
        if (item != items_.end()) {
            RemoveChild(item->second);
            items_.erase(item);
        }
    }
    void ClearLayout(bool needReservedPlace = false);
    void ClearItems();
    void OnDataSourceUpdated(int32_t index);

    void SetTotalCount(int32_t totalCount)
    {
        if (totalCount_ == totalCount) {
            return;
        }
        totalCount_ = totalCount;
        totalCountFlag_ = true;
    }

    double GetEstimatedHeight();
    // Used in GridPositionController
    void ScrollToIndex(int32_t index, int32_t source);
    bool AnimateTo(const Dimension& position, float duration, const RefPtr<Curve>& curve);
    Offset CurrentOffset();
    void ScrollToEdge(ScrollEdgeType edgeType, bool smooth);

    Axis GetAxis() const
    {
        return useScrollable_ == SCROLLABLE::VERTICAL ? Axis::VERTICAL : Axis::HORIZONTAL;
    }

    void OnPaintFinish() override;

    bool IsChildrenTouchEnable() override;

    size_t GetCachedSize() const
    {
        return endShowItemIndex_ - startShowItemIndex_;
    }

    Offset GetLastOffset() const
    {
        return useScrollable_ == SCROLLABLE::VERTICAL ? Offset(0, lastOffset_) : Offset(lastOffset_, 0);
    }

    void HandleAxisEvent(const AxisEvent& event) override;

    bool IsAxisScrollable(AxisDirection direction) override;

    WeakPtr<RenderNode> CheckAxisNode() override;

protected:
    int32_t GetItemMainIndex(const RefPtr<RenderNode>& child, bool isMain) const;
    void SetMainSize(Size& dst, const Size& src);
    double GetSize(const Size& src, bool isMain = true) const;
    void GetNextGrid(int32_t& curMain, int32_t& curCross) const override;
    void GetPreviousGrid(int32_t& curMain, int32_t& curCross);
    LayoutParam MakeInnerLayoutParam(int32_t row, int32_t col, int32_t rowSpan, int32_t colSpan) const override;
    bool CheckGridPlaced(int32_t index, int32_t row, int32_t col, int32_t& rowSpan, int32_t& colSpan) override;

    // Sets child position, the mainAxis does not contain the offset.
    void SetChildPosition(
        const RefPtr<RenderNode>& child, int32_t row, int32_t col, int32_t rowSpan, int32_t colSpan) override;

    void CreateScrollable();
    void LayoutChild(const RefPtr<RenderNode>& child, int32_t row, int32_t col, int32_t rowSpan, int32_t colSpan,
        bool needPosition = true);
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    bool UpdateScrollPosition(double offset, int32_t source);
    void RecordLocation();

    void InitialGridProp() override;
    void CalculateViewPort();
    double BuildLazyGridLayout(int32_t index, double sizeNeed);
    bool GetGridSize();
    void BuildGrid(std::vector<double>& rows, std::vector<double>& cols);
    double CalculateBlankOfEnd();
    double SupplyItems(int32_t mainIndex, int32_t itemIndex = -1, bool needPosition = true);
    bool Rank(int32_t mainIndex, int32_t itemIndex = -1);
    bool GetItemPropsByIndex(
        int32_t index, int32_t& itemMain, int32_t& itemCross, int32_t& itemMainSpan, int32_t& itemCrossSpan);

    void DealCache(int32_t start, int32_t end);
    void DeleteItems(int32_t index, bool isTail);

    void GetMinAndMaxIndex(int32_t& min, int32_t& max);
    int32_t GetItemMainIndex(int32_t index);

    bool NeedUpdate(const RefPtr<Component>& component);

    void CalculateWholeSize(double drawLength);

    void InitScrollBar(const RefPtr<Component>& component);
    void InitScrollBarProxy();

    void DoJump(double position, int32_t source);

    int32_t GetStartingItem(int32_t first);
    void LoadForward();

    double GetCurrentOffset() const
    {
        return startMainPos_ + currentOffset_ - firstItemOffset_;
    }

    void SetScrollBarCallback();

    int32_t GetIndexByPosition(double position) const;

    void OnScrolled(int32_t scrolled) const;

    enum class SCROLLABLE : uint32_t {
        NO_SCROLL = 0,
        VERTICAL,
        HORIZONTAL,
    };

    SCROLLABLE useScrollable_ = SCROLLABLE::NO_SCROLL;

    std::map<int32_t, Size> metaData_;
    std::unordered_map<int32_t, RefPtr<RenderNode>> items_;
    std::set<int32_t> showItem_;
    std::set<int32_t> inCache_;
    std::list<RefPtr<RenderNode>> childrenInRect_;

    RefPtr<Scrollable> scrollable_;
    bool reachHead_ = false;
    bool reachTail_ = false;
    std::optional<bool> firstLineToBottom_;
    bool needCalculateViewPort_ = false;
    double startMainPos_ = 0.0;
    double currentOffset_ = 0.0;
    double animateDelta_ = 0.0;
    double lastOffset_ = 0.0;
    double firstItemOffset_ = 0.0;
    int32_t startIndex_ = 0;
    int32_t endIndex_ = -1;

    int32_t startShowItemIndex_ = 0;
    int32_t endShowItemIndex_ = -1;

    int32_t startRankItemIndex_ = 0;
    int32_t currentItemIndex_ = 0;

    double* mainSize_ = &rowSize_;
    double* crossSize_ = &colSize_;
    int32_t* mainCount_ = &rowCount_;
    int32_t* crossCount_ = &colCount_;
    int32_t totalCount_ = 0;
    double* crossGap_ = &colGap_;
    double* mainGap_ = &rowGap_;

    // used for scrollbar
    double scrollBarExtent_ = 0.0;
    double mainScrollExtent_ = 0.0;
    int32_t scrollBarOpacity_ = 0;
    double estimateHeight_ = 0.0;
    bool totalCountFlag_ = false;
    bool animatorJumpFlag_ = false;
    Color scrollBarColor_;

    RefPtr<ScrollBarProxy> scrollBarProxy_;
    RefPtr<ScrollBar> scrollBar_;
    RefPtr<Animator> animator_;
    RefPtr<GridLayoutComponent> component_;
    BuildChildByIndex buildChildByIndex_;
    DeleteChildByIndex deleteChildByIndex_;
    GetChildSpanByIndex getChildSpanByIndex_;
    OnScrolledFunc scrolledEventFun_;

    int32_t lastFirstIndex_ = -1;
    int32_t loadingIndex_ = -1;
    int32_t cacheCount_ = 1;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_GRID_RENDER_GRID_SCROLL_H