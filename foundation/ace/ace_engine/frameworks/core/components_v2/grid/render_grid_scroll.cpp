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

#include "core/components_v2/grid/render_grid_scroll.h"

#include "base/log/ace_trace.h"
#include "base/log/event_report.h"
#include "base/log/log.h"
#include "base/utils/string_utils.h"
#include "base/utils/time_util.h"
#include "base/utils/utils.h"
#include "core/animation/curve_animation.h"
#include "core/components/grid_layout/grid_layout_component.h"
#include "core/components/grid_layout/render_grid_layout_item.h"
#include "core/components_v2/grid/grid_scroll_controller.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace::V2 {

namespace {

const char UNIT_PERCENT[] = "%";
const char UNIT_RATIO[] = "fr";
constexpr int32_t TIMETHRESHOLD = 3 * 1000000; // 3 millisecond
constexpr int32_t MICROSEC_TO_NANOSEC = 1000;

} // namespace

std::string GridEventInfo::ToJSONString() const
{
    return std::string("\"grid\",{\"first\":").append(std::to_string(scrollIndex_)).append("},null");
}

RenderGridScroll::~RenderGridScroll()
{
    if (scrollBarProxy_) {
        scrollBarProxy_->UnRegisterScrollableNode(AceType::WeakClaim(this));
    }
}

void RenderGridScroll::Update(const RefPtr<Component>& component)
{
    InitScrollBar(component);
    if (!NeedUpdate(component)) {
        return;
    }

    LOGD("RenderGridScroll::Update");
    useScrollable_ = SCROLLABLE::NO_SCROLL;
    mainSize_ = &rowSize_;
    crossSize_ = &colSize_;
    mainCount_ = &rowCount_;
    crossCount_ = &colCount_;
    crossGap_ = &colGap_;
    mainGap_ = &rowGap_;
    startRankItemIndex_ = 0;
    currentItemIndex_ = 0;
    RenderGridLayout::Update(component);
    TakeBoundary();
    const RefPtr<GridLayoutComponent> grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (!grid) {
        LOGE("RenderGridLayout update failed.");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    scrolledEventFun_ =
        AceAsyncEvent<void(const std::shared_ptr<GridEventInfo>&)>::Create(grid->GetScrolledEvent(), context_);

    scrollBarProxy_ = grid->GetScrollBarProxy();
    InitScrollBarProxy();
}

bool RenderGridScroll::NeedUpdate(const RefPtr<Component>& component)
{
    const RefPtr<GridLayoutComponent> grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (!grid) {
        LOGE("RenderGridLayout update failed.");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return false;
    }
    auto controller = grid->GetController();
    if (controller) {
        controller->SetScrollNode(WeakClaim(this));
    }
    if (!animator_) {
        animator_ = AceType::MakeRefPtr<Animator>(GetContext());
    }
    cacheCount_ = grid->GetCacheCount();

    if (direction_ != grid->GetDirection() || crossAxisAlign_ != grid->GetFlexAlign() ||
        gridWidth_ != grid->GetWidth() || gridHeight_ != grid->GetHeight() || colsArgs_ != grid->GetColumnsArgs() ||
        rowsArgs_ != grid->GetRowsArgs() || userColGap_ != grid->GetColumnGap() || userRowGap_ != grid->GetRowGap() ||
        rightToLeft_ != grid->GetRightToLeft()) {
        return true;
    };
    return false;
}

void RenderGridScroll::AddChildByIndex(int32_t index, const RefPtr<RenderNode>& renderNode)
{
    auto iter = items_.try_emplace(index, renderNode);
    if (iter.second) {
        AddChild(renderNode);
        RefPtr<RenderGridLayoutItem> node = AceType::DynamicCast<RenderGridLayoutItem>(renderNode);
        if (node) {
            node->SetBoundary();
            node->SetIndex(index);
            node->SetHidden(false);
        }
    }
}

void RenderGridScroll::CreateScrollable()
{
    scrollable_ = nullptr;
    if (useScrollable_ == SCROLLABLE::NO_SCROLL) {
        return;
    }

    auto callback = [weak = AceType::WeakClaim(this)](double offset, int32_t source) {
        auto grid = weak.Upgrade();
        if (!grid) {
            return false;
        }
        // Stop animator of scroll bar.
        auto scrollBarProxy = grid->scrollBarProxy_;
        if (scrollBarProxy) {
            scrollBarProxy->StopScrollBarAnimator();
        }
        return grid->UpdateScrollPosition(offset, source);
    };
    scrollable_ = AceType::MakeRefPtr<Scrollable>(
        callback, useScrollable_ == SCROLLABLE::HORIZONTAL ? Axis::HORIZONTAL : Axis::VERTICAL);
    scrollable_->SetScrollEndCallback([weak = AceType::WeakClaim(this)]() {
        auto grid = weak.Upgrade();
        if (grid) {
            auto proxy = grid->scrollBarProxy_;
            if (proxy) {
                proxy->StartScrollBarAnimator();
            }
        }
    });
    scrollable_->Initialize(context_);
}

bool RenderGridScroll::UpdateScrollPosition(double offset, int32_t source)
{
    if (source == SCROLL_FROM_START) {
        return true;
    }

    if (NearZero(offset)) {
        return true;
    }
    if (scrollBar_ && scrollBar_->NeedScrollBar()) {
        scrollBar_->SetActive(SCROLL_FROM_CHILD != source);
    }

    if (reachHead_ && reachTail_) {
        return false;
    }

    if (offset > 0.0) {
        if (reachHead_) {
            return false;
        }
        reachTail_ = false;
    } else {
        if (reachTail_) {
            return false;
        }
        reachHead_ = false;
    }

    currentOffset_ += Round(offset);
    MarkNeedLayout(true);
    return true;
}

void RenderGridScroll::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!GetVisible()) {
        return;
    }
    if (!scrollable_ || !scrollable_->Available()) {
        return;
    }
    if (scrollBar_ && scrollBar_->InBarRegion(globalPoint_ - coordinateOffset)) {
        scrollBar_->AddScrollBarController(coordinateOffset, result);
    } else {
        scrollable_->SetCoordinateOffset(coordinateOffset);
        scrollable_->SetDragTouchRestrict(touchRestrict);
        result.emplace_back(scrollable_);
    }
    result.emplace_back(scrollable_);
}

bool RenderGridScroll::IsChildrenTouchEnable()
{
    return scrollable_->IsMotionStop();
}

void RenderGridScroll::SetChildPosition(
    const RefPtr<RenderNode>& child, int32_t main, int32_t cross, int32_t mainSpan, int32_t crossSpan)
{
    // Calculate the position for current child.
    double positionMain = 0.0;
    double positionCross = 0.0;
    if (main < startIndex_) {
        positionMain -= GetSize(gridCells_.at(main).at(0));
        positionMain += (main - startIndex_) * (*mainGap_);
    } else {
        for (int32_t i = startIndex_; i < main; ++i) {
            positionMain += GetSize(gridCells_.at(i).at(0));
        }
        positionMain += (main - startIndex_) * (*mainGap_);
    }

    for (int32_t i = 0; i < cross; ++i) {
        positionCross += GetSize(gridCells_.at(main).at(i), false);
    }
    positionCross += cross * (*crossGap_);

    // Calculate the size for current child.
    double mainLen = 0.0;
    double crossLen = 0.0;
    for (int32_t i = 0; i < mainSpan; ++i) {
        mainLen += GetSize(gridCells_.at(main + i).at(0));
    }

    mainLen += (mainSpan - 1) * (*mainGap_);
    for (int32_t i = 0; i < crossSpan; ++i) {
        crossLen += GetSize(gridCells_.at(main).at(cross + i), false);
    }
    crossLen += (crossSpan - 1) * (*crossGap_);

    // If RTL, place the item from right.
    if (rightToLeft_) {
        if (useScrollable_ != SCROLLABLE::HORIZONTAL) {
            positionCross = colSize_ - positionCross - crossLen;
        }
    }

    double mainOffset = (mainLen - GetSize(child->GetLayoutSize())) / 2.0;
    double crossOffset = (crossLen - GetSize(child->GetLayoutSize(), false)) / 2.0;

    Offset offset;
    if (useScrollable_ != SCROLLABLE::HORIZONTAL) {
        offset = Offset(positionCross + crossOffset, positionMain + mainOffset - firstItemOffset_);
    } else {
        offset = Offset(positionMain + mainOffset - firstItemOffset_, positionCross + crossOffset);
    }

    child->SetPosition(offset);
}

int32_t RenderGridScroll::GetItemMainIndex(const RefPtr<RenderNode>& child, bool isMain) const
{
    if (useScrollable_ == SCROLLABLE::HORIZONTAL) {
        if (isMain) {
            return GetItemColumnIndex(child);
        } else {
            return GetItemRowIndex(child);
        }
    } else {
        if (isMain) {
            return GetItemRowIndex(child);
        } else {
            return GetItemColumnIndex(child);
        }
    }
}

void RenderGridScroll::SetMainSize(Size& dst, const Size& src)
{
    if (useScrollable_ == SCROLLABLE::HORIZONTAL) {
        dst.SetWidth(src.Width());
    } else {
        dst.SetHeight(src.Height());
    }
}

double RenderGridScroll::GetSize(const Size& src, bool isMain) const
{
    if (useScrollable_ == SCROLLABLE::HORIZONTAL) {
        return isMain ? src.Width() : src.Height();
    }

    return isMain ? src.Height() : src.Width();
}

bool RenderGridScroll::GetGridSize()
{
    double rowSize = ((gridHeight_ > 0.0) && (gridHeight_ < GetLayoutParam().GetMaxSize().Height()))
                         ? gridHeight_
                         : GetLayoutParam().GetMaxSize().Height();
    double colSize = ((gridWidth_ > 0.0) && (gridWidth_ < GetLayoutParam().GetMaxSize().Width()))
                         ? gridWidth_
                         : GetLayoutParam().GetMaxSize().Width();
    if (NearEqual(rowSize_, Size::INFINITE_SIZE)) {
        if ((rowsArgs_.find(UNIT_PERCENT) != std::string::npos || rowsArgs_.find(UNIT_RATIO) != std::string::npos)) {
            rowSize = viewPort_.Height();
        }
    } else if (rowsArgs_.empty()) {
        useScrollable_ = SCROLLABLE::VERTICAL;
    }
    if (NearEqual(colSize_, Size::INFINITE_SIZE)) {
        if ((colsArgs_.find(UNIT_PERCENT) != std::string::npos || colsArgs_.find(UNIT_RATIO) != std::string::npos)) {
            colSize = viewPort_.Width();
        }
    } else if (colsArgs_.empty()) {
        useScrollable_ = SCROLLABLE::HORIZONTAL;
        mainSize_ = &colSize_;
        crossSize_ = &rowSize_;
        mainCount_ = &colCount_;
        crossCount_ = &rowCount_;
        crossGap_ = &rowGap_;
        mainGap_ = &colGap_;
    }
    LOGD("GetGridSize %lf, %lf   [%lf- %lf]", rowSize, colSize, rowSize_, colSize_);
    if (rowSize != rowSize_ || colSize != colSize_) {
        rowSize_ = rowSize;
        colSize_ = colSize;
        CreateScrollable();
        return true;
    }
    return false;
}

void RenderGridScroll::BuildGrid(std::vector<double>& main, std::vector<double>& cross)
{
    if (useScrollable_ == SCROLLABLE::NO_SCROLL) {
        main = ParseArgs(PreParseRows(), rowSize_, rowGap_);
        cross = ParseArgs(PreParseCols(), colSize_, colGap_);
    } else if (useScrollable_ == SCROLLABLE::VERTICAL) {
        cross = ParseArgs(PreParseCols(), colSize_, colGap_);
        int32_t col = 0;
        for (auto width : cross) {
            metaData_[col] = Size(width, Size::INFINITE_SIZE);
            ++col;
        }
    } else if (useScrollable_ == SCROLLABLE::HORIZONTAL) {
        cross = ParseArgs(PreParseRows(), rowSize_, rowGap_);
        int32_t row = 0;
        for (auto height : cross) {
            metaData_[row] = Size(Size::INFINITE_SIZE, height);
            ++row;
        }
    }
}

void RenderGridScroll::InitialGridProp()
{
    // Not first time layout after update, no need to initial.
    if (!GetGridSize() && !updateFlag_) {
        return;
    }
    ACE_SCOPED_TRACE("InitialGridProp");
    OnDataSourceUpdated(0);
    rowGap_ = NormalizePercentToPx(userRowGap_, true);
    colGap_ = NormalizePercentToPx(userColGap_, false);
    std::vector<double> main;
    std::vector<double> cross;
    BuildGrid(main, cross);

    // Initialize the columnCount and rowCount, default is 1
    *crossCount_ = cross.size();
    *mainCount_ = main.size();
    gridCells_.clear();
    items_.clear();
    UpdateAccessibilityAttr();
    if (buildChildByIndex_) {
        int32_t endIndex = -1;
        while (endIndex < currentItemIndex_) {
            if (!Rank(*mainCount_, *mainCount_ == 0 ? startRankItemIndex_ : -1)) {
                // When [firstLineToBottom_] does not equal to [std::nullopt], it indicates that this [InitialGridProp]
                // is called after [ScrollToIndex].
                // This is the case when it scrolls to the last line and the last line is not full.
                // So we need to add a line to [*mainCount_].
                (*mainCount_) += firstLineToBottom_ ? 1 : 0;
                break;
            }
            (*mainCount_)++;
            auto mainIter = gridMatrix_.find(*mainCount_ - 1);
            if (mainIter == gridMatrix_.end()) {
                break;
            }
            for (int32_t crossIndex = *crossCount_ - 1; crossIndex >= 0; crossIndex--) {
                auto iter = mainIter->second.find(crossIndex);
                if (iter != mainIter->second.end()) {
                    endIndex = iter->second;
                    break;
                }
            }
        }
        currentItemIndex_ = 0;

        SupplyItems(*mainCount_ > 0 ? *mainCount_ - 1 : 0);
        startIndex_ = *mainCount_ > 0 ? *mainCount_ - 1 : 0;

        if (NearZero(currentOffset_)) {
            needCalculateViewPort_ = true;
        }
    }
    updateFlag_ = false;
    if (firstLineToBottom_ && firstLineToBottom_.value()) {
        // calculate the distance from the first line to the last line
        currentOffset_ = *mainSize_ - GetSize(gridCells_.at(0).at(0));
        needCalculateViewPort_ = false;
    }
    firstLineToBottom_ = std::nullopt;
}

double RenderGridScroll::BuildLazyGridLayout(int32_t index, double sizeNeed)
{
    if (!buildChildByIndex_ || index < 0 || NearZero(sizeNeed)) {
        return 0.0;
    }
    LOGD("BuildLazyGridLayout index = %d sizeNeed = %lf", index, sizeNeed);
    double size = 0.0;
    int32_t startIndex = index;
    while (size < sizeNeed) {
        auto suppleSize = SupplyItems(startIndex);
        if (NearZero(suppleSize)) {
            break;
        }
        *mainCount_ = ++startIndex;
        size += suppleSize + *mainGap_;
    }
    return size;
}

bool RenderGridScroll::CheckGridPlaced(
    int32_t index, int32_t main, int32_t cross, int32_t& mainSpan, int32_t& crossSpan)
{
    LOGD("CheckGridPlaced %{public}d %{public}d %{public}d %{public}d %{public}d", index, main, cross, mainSpan,
        crossSpan);
    auto mainIter = gridMatrix_.find(main);
    if (mainIter != gridMatrix_.end()) {
        auto crossIter = mainIter->second.find(cross);
        if (crossIter != mainIter->second.end()) {
            return false;
        }
    }
    if (cross + crossSpan > *crossCount_) {
        return false;
    }

    for (int32_t i = 0; i < mainSpan; i++) {
        mainIter = gridMatrix_.find(i + main);
        if (mainIter != gridMatrix_.end()) {
            for (int32_t j = 0; j < crossSpan; j++) {
                if (mainIter->second.find(j + cross) != mainIter->second.end()) {
                    return false;
                }
            }
        }
    }

    for (int32_t i = main; i < main + mainSpan; ++i) {
        std::map<int32_t, int32_t> mainMap;
        auto iter = gridMatrix_.find(i);
        if (iter != gridMatrix_.end()) {
            mainMap = iter->second;
        }
        for (int32_t j = cross; j < cross + crossSpan; ++j) {
            mainMap.emplace(std::make_pair(j, index));
        }
        gridMatrix_[i] = mainMap;
    }
    LOGD("CheckGridPlaced done %{public}d %{public}d %{public}d %{public}d %{public}d", index, main, cross, mainSpan,
        crossSpan);
    return true;
}

void RenderGridScroll::LayoutChild(const RefPtr<RenderNode>& child, int32_t main, int32_t cross, int32_t mainSpan,
    int32_t crossSpan, bool needPosition)
{
    child->Layout(MakeInnerLayoutParam(main, cross, mainSpan, crossSpan));
    SetMainSize(gridCells_.at(main).at(cross), child->GetLayoutSize());
    if (GetSize(gridCells_.at(main).at(0)) < GetSize(gridCells_.at(main).at(cross))) {
        SetMainSize(gridCells_.at(main).at(0), gridCells_.at(main).at(cross));
    }
    if (!needPosition) {
        return;
    }
    if (useScrollable_ != SCROLLABLE::HORIZONTAL) {
        child->SetPosition(Offset(0, *mainSize_ + *mainGap_));
    } else {
        child->SetPosition(Offset(*mainSize_ + *mainGap_, 0));
    }
}

void RenderGridScroll::GetNextGrid(int32_t& curMain, int32_t& curCross) const
{
    ++curCross;
    if (curCross >= *crossCount_) {
        curCross = 0;
        ++curMain;
    }
}

void RenderGridScroll::GetPreviousGrid(int32_t& curMain, int32_t& curCross)
{
    --curCross;
    if (curCross < 0) {
        curCross = *crossCount_;
        --curMain;
    }
}

LayoutParam RenderGridScroll::MakeInnerLayoutParam(
    int32_t main, int32_t cross, int32_t mainSpan, int32_t crossSpan) const
{
    LayoutParam innerLayout;
    double mainLen = 0.0;
    double crossLen = 0.0;
    for (int32_t i = 0; i < mainSpan; ++i) {
        if (gridCells_.find(main + i) != gridCells_.end() &&
            gridCells_.at(main + i).find(cross) != gridCells_.at(main + i).end()) {
            mainLen += GetSize(gridCells_.at(main + i).at(cross));
        }
    }
    mainLen += (mainSpan - 1) * (*mainGap_);
    for (int32_t i = 0; i < crossSpan; ++i) {
        if (gridCells_.find(main) != gridCells_.end() &&
            gridCells_.at(main).find(cross + i) != gridCells_.at(main).end()) {
            crossLen += GetSize(gridCells_.at(main).at(cross + i), false);
        }
    }
    crossLen += (crossSpan - 1) * (*crossGap_);

    Size size;
    if (useScrollable_ == SCROLLABLE::HORIZONTAL) {
        size = Size(mainLen, crossLen);
    } else {
        size = Size(crossLen, mainLen);
    }
    if (crossAxisAlign_ == FlexAlign::STRETCH) {
        innerLayout.SetMinSize(size);
        innerLayout.SetMaxSize(size);
    } else {
        innerLayout.SetMaxSize(size);
    }
    return innerLayout;
}

void RenderGridScroll::LoadForward()
{
    auto firstItem = GetStartingItem(startRankItemIndex_ - 1);

    decltype(gridCells_) gridCells(std::move(gridCells_));
    decltype(gridMatrix_) gridMatrix(std::move(gridMatrix_));

    int32_t count = 0;
    int32_t endIndex = -1;
    while (endIndex < startRankItemIndex_ - 1) {
        if (!Rank(count, count == 0 ? firstItem : -1)) {
            break;
        }
        count++;
        auto mainIter = gridMatrix_.find(count - 1);
        if (mainIter == gridMatrix_.end()) {
            break;
        }
        for (int32_t cross = *crossCount_ - 1; cross >= 0; cross--) {
            auto iter = mainIter->second.find(cross);
            if (iter != mainIter->second.end()) {
                endIndex = iter->second;
                break;
            }
        }
    }
    startRankItemIndex_ = firstItem;
    if (count == 0) {
        return;
    }
    for (const auto& item : gridMatrix) {
        gridMatrix_[item.first + count] = item.second;
    }
    for (const auto& item : gridCells) {
        gridCells_[item.first + count] = item.second;
    }

    decltype(inCache_) inCache(std::move(inCache_));
    for (const auto& item : inCache) {
        inCache_.insert(item + count);
    }

    *mainCount_ += count;
    startIndex_ += count;
}

void RenderGridScroll::CalculateViewPort()
{
    while (!NearZero(currentOffset_) || needCalculateViewPort_) {
        if (currentOffset_ > 0) {
            // move to top/left of first row/column
            if (!NearZero(firstItemOffset_)) {
                if (gridCells_.find(startIndex_ + 1) != gridCells_.end()) {
                    currentOffset_ += GetSize(gridCells_.at(startIndex_++).at(0)) + *mainGap_ - firstItemOffset_;
                }
                firstItemOffset_ = 0.0;
            }
            // Move up
            while (currentOffset_ > 0) {
                if (startIndex_ > 0) {
                    if (gridCells_.find(startIndex_ - 1) == gridCells_.end()) {
                        SupplyItems(startIndex_ - 1);
                    }
                    currentOffset_ -= GetSize(gridCells_.at(startIndex_-- - 1).at(0)) + *mainGap_;
                }
                if (startIndex_ == 0 && startRankItemIndex_ > 0 && currentOffset_ > 0) {
                    LoadForward();
                }
                if (startIndex_ == 0) {
                    break;
                }
            }
            if (currentOffset_ < 0) {
                firstItemOffset_ -= currentOffset_;
            } else {
                if (startIndex_ == 0) {
                    reachHead_ = true;
                }
            }
            currentOffset_ = 0.0;
        } else {
            if (!NearZero(firstItemOffset_)) {
                currentOffset_ -= firstItemOffset_;
                firstItemOffset_ = 0.0;
            }
            // Move down
            while (startIndex_ < *mainCount_ && (currentOffset_ < 0 || needCalculateViewPort_)) {
                currentOffset_ += GetSize(gridCells_.at(startIndex_++).at(0)) + *mainGap_;
            }
            needCalculateViewPort_ = false;
            if (currentOffset_ > 0) {
                firstItemOffset_ = GetSize(gridCells_.at(--startIndex_).at(0)) + *mainGap_ - currentOffset_;
                currentOffset_ = 0.0;
            } else {
                if (!GreatOrEqual(0.0, BuildLazyGridLayout(*mainCount_, -currentOffset_))) {
                    continue;
                }
            }
            currentOffset_ = 0.0;
            auto blank = CalculateBlankOfEnd();
            if (GreatOrEqual(0.0, blank)) {
                return;
            }
            // request new item.
            blank -= BuildLazyGridLayout(*mainCount_, blank);
            if (blank < 0) {
                return;
            }
            blank = blank - firstItemOffset_;
            firstItemOffset_ = 0;
            // Move up
            while (blank > 0) {
                if (startIndex_ == 0 && startRankItemIndex_ > 0) {
                    LoadForward();
                }
                if (startIndex_ == 0) {
                    break;
                }
                if (gridCells_.find(startIndex_ - 1) == gridCells_.end()) {
                    SupplyItems(startIndex_ - 1);
                }
                blank -= GetSize(gridCells_.at(--startIndex_).at(0)) + *mainGap_;
            }
            firstItemOffset_ -= blank;
            if (firstItemOffset_ < 0) {
                firstItemOffset_ = 0;
            }
            reachTail_ = true;
        }
    }
}

double RenderGridScroll::CalculateBlankOfEnd()
{
    double drawLength = 0.0 - firstItemOffset_;
    for (int32_t main = startIndex_; main < *mainCount_; main++) {
        drawLength += GetSize(gridCells_.at(main).at(0)) + *mainGap_;
        if (GreatOrEqual(drawLength, *mainSize_)) {
            break;
        }
    }
    return *mainSize_ - drawLength;
}

double RenderGridScroll::SupplyItems(int32_t mainIndex, int32_t itemIndex, bool needPosition)
{
    ACE_SCOPED_TRACE("SupplyItems %d", mainIndex);
    if (loadingIndex_ == mainIndex) {
        loadingIndex_ = -1;
    }
    if (gridMatrix_.find(mainIndex) == gridMatrix_.end()) {
        Rank(mainIndex, itemIndex);
    }
    gridCells_.try_emplace(mainIndex, metaData_);
    auto iter = gridMatrix_.find(mainIndex);
    if (iter != gridMatrix_.end()) {
        int32_t frontIndex = -1;
        for (const auto& item : iter->second) {
            if (item.second != frontIndex) {
                if (items_.find(item.second) != items_.end() || buildChildByIndex_(item.second)) {
                    int32_t itemRowSpan = GetItemSpan(items_[item.second], true);
                    int32_t itemColSpan = GetItemSpan(items_[item.second], false);
                    LayoutChild(items_[item.second], mainIndex, item.first, itemRowSpan, itemColSpan, needPosition);
                }
            }
            frontIndex = item.second;
        }
        inCache_.insert(mainIndex);
        return NearEqual(GetSize(gridCells_[mainIndex][0]), Size::INFINITE_SIZE) ? 0.0
                                                                                 : GetSize(gridCells_[mainIndex][0]);
    }
    return 0.0;
}

bool RenderGridScroll::Rank(int32_t mainIndex, int32_t itemIndex)
{
    if (gridMatrix_.find(mainIndex) != gridMatrix_.end()) {
        return true;
    }
    ACE_SCOPED_TRACE("Rank [%d]", mainIndex);
    if (itemIndex == -1) {
        auto mainIter = gridMatrix_.find(mainIndex - 1);
        if (mainIter != gridMatrix_.end()) {
            for (int32_t cross = *crossCount_ - 1; cross >= 0; cross--) {
                auto iter = mainIter->second.find(cross);
                if (iter != mainIter->second.end()) {
                    itemIndex = iter->second + 1;
                    break;
                }
            }
        }
    }
    if (itemIndex == -1) {
        LOGE("failed, itemIndex = -1, mainIndex = %d", mainIndex);
        return false;
    }

    bool isFilled = false;
    int32_t index = mainIndex;
    int32_t crossIndex = 0;
    while (!isFilled) {
        int32_t itemMain = -1;
        int32_t itemCross = -1;
        int32_t itemMainSpan = -1;
        int32_t itemCrossSpan = -1;
        auto item = items_.find(itemIndex);
        if (item != items_.end()) {
            itemMain = GetItemMainIndex(item->second, true);
            itemCross = GetItemMainIndex(item->second, false);
            itemMainSpan = GetItemSpan(item->second, useScrollable_ != SCROLLABLE::HORIZONTAL);
            itemCrossSpan = GetItemSpan(item->second, useScrollable_ == SCROLLABLE::HORIZONTAL);
        } else {
            if (!getChildSpanByIndex_(itemIndex, useScrollable_ == SCROLLABLE::HORIZONTAL, itemMain, itemCross,
                    itemMainSpan, itemCrossSpan)) {
                return false;
            }
        }

        if (itemCrossSpan > *crossCount_) {
            itemIndex++;
            continue;
        }
        if (itemMain >= 0 && itemCross >= 0 && itemCross < *crossCount_ &&
            CheckGridPlaced(itemIndex, itemMain, itemCross, itemMainSpan, itemCrossSpan)) {
        } else {
            while (!CheckGridPlaced(itemIndex, mainIndex, crossIndex, itemMainSpan, itemCrossSpan)) {
                GetNextGrid(mainIndex, crossIndex);
                if (mainIndex > index) {
                    isFilled = true;
                    break;
                }
            }
        }
        itemIndex++;
    }
    return true;
}

void RenderGridScroll::PerformLayout()
{
    if (rowsArgs_.empty() && colsArgs_.empty()) {
        return;
    }
    if (RenderGridLayout::GetChildren().empty() && !buildChildByIndex_) {
        return;
    }
    lastOffset_ = startMainPos_ + firstItemOffset_ - currentOffset_;
    InitialGridProp();
    CalculateViewPort();
    showItem_.clear();
    childrenInRect_.clear();
    double drawLength = 0.0 - firstItemOffset_;
    int32_t main = startIndex_ > 0 ? startIndex_ - 1 : startIndex_;
    LOGD("startIndex_=[%d], firstItemOffset_=[%lf]", startIndex_, firstItemOffset_);
    for (; main < *mainCount_; main++) {
        if (gridCells_.find(main) == gridCells_.end()) {
            continue;
        }
        for (int32_t cross = 0; cross < *crossCount_; cross++) {
            auto mainIter = gridMatrix_.find(main);
            if (mainIter == gridMatrix_.end()) {
                continue;
            }
            auto crossIter = mainIter->second.find(cross);
            if (crossIter == mainIter->second.end()) {
                continue;
            }
            if (buildChildByIndex_ && inCache_.count(main) == 0) {
                SupplyItems(main);
            }
            if (showItem_.count(crossIter->second) == 0) {
                showItem_.insert(crossIter->second);
                auto item = items_.find(crossIter->second);
                if (item != items_.end()) {
                    childrenInRect_.push_back(item->second);
                    int32_t itemMainSpan = GetItemSpan(item->second, useScrollable_ != SCROLLABLE::HORIZONTAL);
                    int32_t itemCrossSpan = GetItemSpan(item->second, useScrollable_ == SCROLLABLE::HORIZONTAL);
                    SetChildPosition(item->second, main, cross, itemMainSpan, itemCrossSpan);
                }
            }
        }
        if (main >= startIndex_) {
            drawLength += GetSize(gridCells_.at(main).at(0)) + *mainGap_;
        }
        if (GreatOrEqual(drawLength, *mainSize_)) {
            break;
        }
    }
    SetLayoutSize(GetLayoutParam().Constrain(Size(colSize_, rowSize_)));
    endIndex_ = main;
    MarkNeedPredictLayout();
    CalculateWholeSize(drawLength);

    int32_t firstIndex = GetIndexByPosition(0);
    if (lastFirstIndex_ != firstIndex) {
        if (!animatorJumpFlag_) {
            OnScrolled(firstIndex);
        }
        lastFirstIndex_ = firstIndex;
    }
    animatorJumpFlag_ = false;
}

void RenderGridScroll::DealCache(int32_t start, int32_t end)
{
    if (loadingIndex_ != -1) {
        return;
    }

    std::set<int32_t> deleteItem;
    for (const auto& item : inCache_) {
        if (item < start - cacheCount_ || item > end + cacheCount_) {
            deleteItem.insert(item);
        }
    }

    for (const auto& item : deleteItem) {
        DeleteItems(item, false);
    }

    for (int32_t i = 1; i <= cacheCount_; i++) {
        if (inCache_.count(i + end) == 0) {
            loadingIndex_ = i + end;
            break;
        }

        if (start >= i && inCache_.count(start - i) == 0) {
            loadingIndex_ = start - i;
            break;
        }
    }
}

void RenderGridScroll::DeleteItems(int32_t index, bool isTail)
{
    if (!deleteChildByIndex_) {
        return;
    }

    auto iter = gridMatrix_.find(index);
    if (iter == gridMatrix_.end()) {
        return;
    }
    for (const auto& item : iter->second) {
        deleteChildByIndex_(item.second);
        RemoveChildByIndex(item.second);
    }

    inCache_.erase(index);
}

void RenderGridScroll::ClearLayout(bool needReservedPlace)
{
    if (needReservedPlace) {
        RecordLocation();
    } else {
        currentOffset_ = 0.0;
    }
    showItem_.clear();
    childrenInRect_.clear();
    inCache_.clear();

    updateFlag_ = true;
    reachHead_ = false;
    reachTail_ = false;
    startMainPos_ = 0.0;
    firstItemOffset_ = 0.0;
    startIndex_ = 0;
    endIndex_ = -1;
    mainScrollExtent_ = 0.0;
    lastOffset_ = 0.0;
    estimateHeight_ = 0.0;

    colCount_ = 0;
    rowCount_ = 0;

    gridMatrix_.clear();
    gridCells_.clear();
}

void RenderGridScroll::RecordLocation()
{
    double positionMain = 0.0;

    for (int32_t i = 0; i < startIndex_; ++i) {
        if (i < static_cast<int32_t>(gridCells_.size())) {
            positionMain += GetSize(gridCells_.at(i).at(0));
        }
    }
    positionMain += (startIndex_) * (*mainGap_);
    currentOffset_ += -positionMain - firstItemOffset_;
}

void RenderGridScroll::ClearItems()
{
    decltype(items_) items(std::move(items_));
    for (const auto& item : items) {
        if (item.first < startRankItemIndex_) {
            deleteChildByIndex_(item.first);
        }
        RemoveChildByIndex(item.first);
    }
    loadingIndex_ = -1;
}

void RenderGridScroll::GetMinAndMaxIndex(int32_t& min, int32_t& max)
{
    for (const auto& item : items_) {
        if (item.first < min) {
            min = item.first;
        }
        if (item.first > max) {
            max = item.first;
        }
    }
}

int32_t RenderGridScroll::GetItemMainIndex(int32_t index)
{
    for (const auto& main : gridMatrix_) {
        for (const auto& cross : main.second) {
            if (cross.second == index) {
                return main.first;
            }
        }
    }
    return -1;
}

int32_t RenderGridScroll::GetStartingItem(int32_t first)
{
    int32_t firstIndex = 0;
    int32_t index = first;
    int32_t itemMain = -1;
    int32_t itemCross = -1;
    int32_t itemMainSpan = -1;
    int32_t itemCrossSpan = -1;
    while (index > 0) {
        if (getChildSpanByIndex_(
                index, useScrollable_ == SCROLLABLE::HORIZONTAL, itemMain, itemCross, itemMainSpan, itemCrossSpan)) {
            LOGD("index %d %d,  %d,  %d,  %d", index, itemMain, itemCross, itemMainSpan, itemCrossSpan);
            if (itemCross == 0) {
                firstIndex = index;
                break;
            }
        }

        index--;
    }
    return firstIndex;
}

void RenderGridScroll::OnDataSourceUpdated(int32_t index)
{
    if (items_.empty() && updateFlag_) {
        return;
    }
    ACE_SCOPED_TRACE("OnDataSourceUpdated %d", index);
    auto items = gridMatrix_.find(startIndex_);
    if (items != gridMatrix_.end() && !items->second.empty()) {
        currentItemIndex_ = items->second.begin()->second;
    }
    startRankItemIndex_ = GetStartingItem(currentItemIndex_);
    auto offset = firstItemOffset_;
    ClearItems();
    ClearLayout(false);

    currentOffset_ = -offset;
    MarkNeedLayout();
}

void RenderGridScroll::CalculateWholeSize(double drawLength)
{
    if (gridMatrix_.empty() || gridCells_.empty()) {
        return;
    }
    if (totalCountFlag_) {
        int currentItemCount = 0;
        auto lastRow = gridMatrix_.rbegin()->second;
        if (!lastRow.empty()) {
            currentItemCount = lastRow.rbegin()->second;
        }
        if (currentItemCount != 0) {
            mainScrollExtent_ = 0.0;
            // calculate the whole size
            mainScrollExtent_ = static_cast<double>(totalCount_) / static_cast<double>(currentItemCount) * drawLength;
            estimateHeight_ = mainScrollExtent_;
            totalCountFlag_ = false;
        }
    }
    bool isScrollable = false;
    if (estimateHeight_ > viewPort_.Height()) {
        isScrollable = true;
    }
    if (scrollBar_) {
        scrollBar_->SetScrollable(isScrollable);
    }
    scrollBarExtent_ = 0.0;
    startMainPos_ = 0.0;
    for (int index = 0; index < *mainCount_; index++) {
        if (index == startIndex_) {
            // get the start position in grid
            startMainPos_ = scrollBarExtent_;
        }
        if (gridCells_.find(index) == gridCells_.end()) {
            continue;
        }
        scrollBarExtent_ += GetSize(gridCells_.at(index).at(0)) + *mainGap_;
    }
    if (!isScrollable) {
        currentOffset_ = 0.0;
    }
}

double RenderGridScroll::GetEstimatedHeight()
{
    if (reachTail_) {
        // reach the end og grid, update the total scroll bar length
        estimateHeight_ = scrollBarExtent_;
    } else {
        estimateHeight_ = std::max(estimateHeight_, scrollBarExtent_);
    }
    return estimateHeight_;
}

void RenderGridScroll::InitScrollBar(const RefPtr<Component>& component)
{
    const RefPtr<GridLayoutComponent> grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (!grid) {
        LOGE("RenderGridLayout update failed.");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    const RefPtr<ScrollBarTheme> theme = GetTheme<ScrollBarTheme>();
    if (!theme) {
        return;
    }
    if (scrollBar_) {
        scrollBar_->Reset();
    } else {
        RefPtr<GridScrollController> controller = AceType::MakeRefPtr<GridScrollController>();
        scrollBar_ = AceType::MakeRefPtr<ScrollBar>(grid->GetScrollBar(), theme->GetShapeMode());
        scrollBar_->SetScrollBarController(controller);
    }
    // set the scroll bar style
    scrollBar_->SetReservedHeight(theme->GetReservedHeight());
    scrollBar_->SetMinHeight(theme->GetMinHeight());
    scrollBar_->SetMinDynamicHeight(theme->GetMinDynamicHeight());
    auto& scrollBarColor = grid->GetScrollBarColor();
    if (!scrollBarColor.empty()) {
        scrollBarColor_ = Color::FromString(scrollBarColor);
    } else {
        scrollBarColor_ = theme->GetForegroundColor();
    }
    scrollBar_->SetForegroundColor(scrollBarColor_);
    scrollBar_->SetBackgroundColor(theme->GetBackgroundColor());
    scrollBar_->SetPadding(theme->GetPadding());
    scrollBar_->SetScrollable(true);
    if (!grid->GetScrollBarWidth().empty()) {
        const auto& width = StringUtils::StringToDimension(grid->GetScrollBarWidth());
        scrollBar_->SetInactiveWidth(width);
        scrollBar_->SetNormalWidth(width);
        scrollBar_->SetActiveWidth(width);
        scrollBar_->SetTouchWidth(width);
    } else {
        scrollBar_->SetInactiveWidth(theme->GetNormalWidth());
        scrollBar_->SetNormalWidth(theme->GetNormalWidth());
        scrollBar_->SetActiveWidth(theme->GetActiveWidth());
        scrollBar_->SetTouchWidth(theme->GetTouchWidth());
    }
    scrollBar_->InitScrollBar(AceType::WeakClaim(this), GetContext());
    SetScrollBarCallback();
}

void RenderGridScroll::InitScrollBarProxy()
{
    if (!scrollBarProxy_) {
        return;
    }
    auto&& scrollCallback = [weakScroll = AceType::WeakClaim(this)](double value, int32_t source) {
        auto grid = weakScroll.Upgrade();
        if (!grid) {
            LOGE("render grid is released");
            return false;
        }
        return grid->UpdateScrollPosition(value, source);
    };
    scrollBarProxy_->UnRegisterScrollableNode(AceType::WeakClaim(this));
    scrollBarProxy_->RegisterScrollableNode({ AceType::WeakClaim(this), scrollCallback });
}

void RenderGridScroll::SetScrollBarCallback()
{
    if (!scrollBar_ || !scrollBar_->NeedScrollBar()) {
        return;
    }
    auto&& barEndCallback = [weakGrid = AceType::WeakClaim(this)](int32_t value) {
        auto grid = weakGrid.Upgrade();
        if (!grid) {
            LOGE("render grid is released");
            return;
        }
        grid->scrollBarOpacity_ = value;
        grid->MarkNeedLayout(true);
    };
    auto&& scrollEndCallback = [weakGrid = AceType::WeakClaim(this)]() {
        auto grid = weakGrid.Upgrade();
        if (!grid) {
            LOGE("render grid is released");
            return;
        }
        LOGD("trigger scroll end callback");
    };
    auto&& scrollCallback = [weakScroll = AceType::WeakClaim(this)](double value, int32_t source) {
        auto grid = weakScroll.Upgrade();
        if (!grid) {
            LOGE("render grid is released");
            return false;
        }
        return grid->UpdateScrollPosition(value, source);
    };
    scrollBar_->SetCallBack(scrollCallback, barEndCallback, scrollEndCallback);
}

void RenderGridScroll::ScrollToIndex(int32_t index, int32_t source)
{
    if (useScrollable_ == SCROLLABLE::NO_SCROLL || index < 0) {
        LOGW("Not supported.");
        return;
    }
    auto inputIdx = GetItemMainIndex(index);
    if (inputIdx < endIndex_ && inputIdx > startIndex_) {
        LOGI("already in map, not need to jump.");
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("context is null");
        return;
    }
    // Build items
    if (index < startShowItemIndex_ || index > endShowItemIndex_) {
        // do not need layout transition
        auto option = context->GetExplicitAnimationOption();
        context->SaveExplicitAnimationOption(AnimationOption());
        firstLineToBottom_.emplace(index > endShowItemIndex_);
        if (scrollable_ && !scrollable_->IsStopped()) {
            scrollable_->StopScrollable();
        }
        startRankItemIndex_ = GetStartingItem(index);
        ClearItems();
        ClearLayout(false);
        currentOffset_ = 0;
        MarkNeedLayout();
        context->SaveExplicitAnimationOption(option);
        return;
    }
    // Calculate scroll length
    double scrollLength = 0.0;
    inputIdx = GetItemMainIndex(index);
    if (inputIdx >= endIndex_) {
        double mainLen = -firstItemOffset_;
        for (int32_t i = startIndex_; i <= endIndex_; ++i) {
            if (gridCells_.find(i) != gridCells_.end()) {
                mainLen += GetSize(gridCells_.at(i).at(0)) + *mainGap_;
            }
        }
        scrollLength += (mainLen - GetSize(GetLayoutSize()));
        for (int32_t i = endIndex_; i < inputIdx; ++i) {
            SupplyItems(i, index);
            if (i != inputIdx) {
                scrollLength += GetSize(gridCells_.at(i).at(0)) + *mainGap_;
            }
        }
        scrollLength = -scrollLength;
    } else if (inputIdx <= startIndex_) {
        scrollLength += firstItemOffset_;
        for (int32_t i = startIndex_; i >= inputIdx; --i) {
            SupplyItems(i, index, i != inputIdx);
            if (i != inputIdx) {
                scrollLength += GetSize(gridCells_.at(i).at(0)) + *mainGap_;
            }
        }
    } else {
        return;
    }
    // Start animation
    if (!animator_->IsStopped()) {
        animator_->Stop();
    }
    animator_->ClearInterpolators();
    animateDelta_ = 0.0;
    auto animation = AceType::MakeRefPtr<CurveAnimation<double>>(0, scrollLength, Curves::LINEAR);
    animation->AddListener([weakScroll = AceType::WeakClaim(this)](double value) {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            scroll->animatorJumpFlag_ = true;
            scroll->DoJump(value, SCROLL_FROM_JUMP);
        }
    });
    animator_->AddInterpolator(animation);
    animator_->ApplyOption(context->GetExplicitAnimationOption());
    animator_->ClearStopListeners();
    animator_->AddStopListener([weakScroll = AceType::WeakClaim(this)]() {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            scroll->animateDelta_ = 0.0;
        }
    });
    animator_->Play();
}

bool RenderGridScroll::AnimateTo(const Dimension& position, float duration, const RefPtr<Curve>& curve)
{
    if (!animator_->IsStopped()) {
        animator_->Stop();
    }
    animator_->ClearInterpolators();
    animateDelta_ = 0.0;
    auto animation = AceType::MakeRefPtr<CurveAnimation<double>>(GetCurrentOffset(), NormalizeToPx(position), curve);
    animation->AddListener([weakScroll = AceType::WeakClaim(this)](double value) {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            scroll->DoJump(value, SCROLL_FROM_JUMP);
        }
    });
    animator_->AddInterpolator(animation);
    animator_->SetDuration(duration);
    animator_->ClearStopListeners();
    animator_->AddStopListener([weakScroll = AceType::WeakClaim(this)]() {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            scroll->animateDelta_ = 0.0;
        }
    });
    animator_->Play();
    return true;
}

Offset RenderGridScroll::CurrentOffset()
{
    auto ctx = GetContext().Upgrade();
    if (!ctx) {
        return useScrollable_ == SCROLLABLE::HORIZONTAL ? Offset(GetCurrentOffset(), 0.0)
                                                        : Offset(0.0, GetCurrentOffset());
    }
    auto mainOffset = ctx->ConvertPxToVp(Dimension(GetCurrentOffset(), DimensionUnit::PX));
    Offset currentOffset = useScrollable_ == SCROLLABLE::HORIZONTAL ? Offset(mainOffset, 0.0) : Offset(0.0, mainOffset);
    return currentOffset;
}

bool RenderGridScroll::GetItemPropsByIndex(
    int32_t itemIndex, int32_t& itemMain, int32_t& itemCross, int32_t& itemMainSpan, int32_t& itemCrossSpan)
{
    auto item = items_.find(itemIndex);
    if (item != items_.end()) {
        itemMain = GetItemMainIndex(item->second, true);
        itemCross = GetItemMainIndex(item->second, false);
        itemMainSpan = GetItemSpan(item->second, useScrollable_ != SCROLLABLE::HORIZONTAL);
        itemCrossSpan = GetItemSpan(item->second, useScrollable_ == SCROLLABLE::HORIZONTAL);
    } else {
        if (!getChildSpanByIndex_(itemIndex, useScrollable_ == SCROLLABLE::HORIZONTAL, itemMain, itemCross,
                itemMainSpan, itemCrossSpan)) {
            LOGW("Not valid.");
            return false;
        }
    }
    return true;
}

void RenderGridScroll::ScrollToEdge(OHOS::Ace::ScrollEdgeType edgeType, bool smooth)
{
    if (edgeType != ScrollEdgeType::SCROLL_TOP) {
        LOGW("Not supported yet");
        return;
    }
    if (items_.empty() && updateFlag_) {
        return;
    }
    if (scrollable_ && !scrollable_->IsStopped()) {
        scrollable_->StopScrollable();
    }
    currentItemIndex_ = 0;
    startRankItemIndex_ = GetStartingItem(currentItemIndex_);
    ClearItems();
    ClearLayout(false);
    MarkNeedLayout();
}

void RenderGridScroll::DoJump(double position, int32_t source)
{
    double delta = position - animateDelta_;
    UpdateScrollPosition(delta, source);
    animateDelta_ = position;
}

int32_t RenderGridScroll::GetIndexByPosition(double position) const
{
    int32_t index = 0;
    double startPosition = 0.0;
    double endPosition = 0.0;

    for (const auto& item : childrenInRect_) {
        if (!item || item->GetChildren().empty()) {
            continue;
        }

        auto gridItem = AceType::DynamicCast<RenderGridLayoutItem>(item);
        if (!gridItem) {
            break;
        }

        startPosition = item->GetPosition().GetY();
        endPosition = item->GetPosition().GetY() + item->GetLayoutSize().Height();

        if ((position > startPosition && position < endPosition) || NearEqual(position, startPosition) ||
            startPosition > position) {
            return gridItem->GetIndex();
        }
    }
    return index;
}

void RenderGridScroll::OnScrolled(int32_t scrolled) const
{
    if (scrolledEventFun_) {
        auto event = std::make_shared<GridEventInfo>(scrolled);
        if (event) {
            scrolledEventFun_(event);
        }
    }
}

void RenderGridScroll::OnPaintFinish()
{
    RenderNode::OnPaintFinish();
    if (showItem_.empty()) {
        return;
    }
    auto currentStartItemCount = *showItem_.begin();
    auto currentEndItemCount = *showItem_.rbegin();
    if ((startShowItemIndex_ != currentStartItemCount) || (endShowItemIndex_ != currentEndItemCount)) {
        startShowItemIndex_ = currentStartItemCount;
        endShowItemIndex_ = currentEndItemCount;
    }
}

void RenderGridScroll::OnPredictLayout(int64_t deadline)
{
    auto startTime = GetSysTimestamp();  // unit: ns
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    if (!context->IsTransitionStop()) {
        LOGI("In page transition, skip predict.");
        return;
    }
    if (loadingIndex_ == -1) {
        DealCache(startIndex_, endIndex_);
        if (loadingIndex_ == -1) {
            if (startIndex_ == 0 && startRankItemIndex_ > 0) {
                LoadForward();
                MarkNeedPredictLayout();
            }
            return;
        }
    }
    ACE_SCOPED_TRACE("OnPredictLayout %d", loadingIndex_);
    if (gridMatrix_.find(loadingIndex_) != gridMatrix_.end() || Rank(loadingIndex_)) {
        auto iter = gridMatrix_.find(loadingIndex_);
        if (iter != gridMatrix_.end()) {
            for (const auto& item : iter->second) {
                if (items_.find(item.second) == items_.end()) {
                    if (!buildChildByIndex_(item.second)) {
                        break;
                    }
                }
                // Stop predictLayout less than 3 milliseconds before the next vsync arrives.
                if (GetSysTimestamp() - startTime + TIMETHRESHOLD > deadline * MICROSEC_TO_NANOSEC) {
                    MarkNeedPredictLayout();
                    return;
                }
            }
            SupplyItems(loadingIndex_);
        }
        MarkNeedPredictLayout();
    } else {
        loadingIndex_ = -1;
    }
}

bool RenderGridScroll::IsAxisScrollable(AxisDirection direction)
{
    return (((direction == AxisDirection::UP || direction == AxisDirection::LEFT) && !reachHead_) ||
        ((direction == AxisDirection::DOWN || direction == AxisDirection::RIGHT) && !reachTail_));
}

void RenderGridScroll::HandleAxisEvent(const AxisEvent& event)
{
    double degree = 0.0f;
    if (!NearZero(event.horizontalAxis)) {
        degree = event.horizontalAxis;
    } else if (!NearZero(event.verticalAxis)) {
        degree = event.verticalAxis;
    }
    double offset = SystemProperties::Vp2Px(DP_PER_LINE_DESKTOP * LINE_NUMBER_DESKTOP * degree / MOUSE_WHEEL_DEGREES);
    UpdateScrollPosition(-offset, SCROLL_FROM_ROTATE);
}

WeakPtr<RenderNode> RenderGridScroll::CheckAxisNode()
{
    return AceType::WeakClaim<RenderNode>(this);
}

} // namespace OHOS::Ace::V2