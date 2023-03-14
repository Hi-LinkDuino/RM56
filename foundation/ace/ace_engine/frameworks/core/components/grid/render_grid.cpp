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

#include "core/components/grid/render_grid.h"

#include <algorithm>
#include <cmath>

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/animation/curve_animation.h"
#include "core/components/grid/grid_component.h"
#include "core/components/grid/render_grid_item.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t FOCUS_LOST = -1;
constexpr int32_t SCROLL_NONE = 0;
constexpr int32_t SCROLL_TOUCH_DOWN = 1;
constexpr int32_t SCROLL_TOUCH_UP = 2;
constexpr int32_t SCROLL_BY_SPEED = 250; // move 250 pixels per second
constexpr int32_t DEFAULT_DEPTH = 10;
constexpr double DOUBLE_FACTOR = 2.0;
constexpr double DURATION_SECOND_TO_MS = 1000.0;

constexpr bool HORIZONTAL = false;
constexpr bool VERTICAL = true;
constexpr bool FORWARD = false;
constexpr bool REVERSE = true;

// first bool mean if vertical, second bool mean if reverse
// false, false --> RIGHT
// false, true --> LEFT
// true, false --> DOWN
// true, true ---> UP
// This map will adapter the Grid FlexDirection with Key Direction.
const std::map<FlexDirection, std::map<bool, std::map<bool, KeyDirection>>> DIRECTION_MAP = {
    { FlexDirection::ROW,
        {
            { HORIZONTAL, { { FORWARD, KeyDirection::DOWN }, { REVERSE, KeyDirection::UP } } },
            { VERTICAL, { { FORWARD, KeyDirection::RIGHT }, { REVERSE, KeyDirection::LEFT } } }
        }
    },
    { FlexDirection::COLUMN,
        {
            { HORIZONTAL, { { FORWARD, KeyDirection::RIGHT }, { REVERSE, KeyDirection::LEFT } } },
            { VERTICAL, { { FORWARD, KeyDirection::DOWN }, { REVERSE, KeyDirection::UP } } }
        }
    },
    { FlexDirection::ROW_REVERSE,
        {
            { HORIZONTAL, { { FORWARD, KeyDirection::UP }, { REVERSE, KeyDirection::DOWN } } },
            { VERTICAL, { { FORWARD, KeyDirection::RIGHT }, { REVERSE, KeyDirection::LEFT } } }
        }
    },
    { FlexDirection::COLUMN_REVERSE,
        {
            { HORIZONTAL, { { FORWARD, KeyDirection::RIGHT }, { REVERSE, KeyDirection::LEFT } } },
            { VERTICAL, { { FORWARD, KeyDirection::UP }, { REVERSE, KeyDirection::DOWN } } }
        }
    }
};

} // namespace

RenderGrid::RenderGrid()
{
    touchRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
    touchRecognizer_->SetOnTouchDown([wp = AceType::WeakClaim(this)](const TouchEventInfo&) {
        auto sp = wp.Upgrade();
        if (sp) {
            sp->HandleTouchDown();
        }
    });
    touchRecognizer_->SetOnTouchUp([wp = AceType::WeakClaim(this)](const TouchEventInfo&) {
        auto sp = wp.Upgrade();
        if (sp) {
            sp->HandleTouchUp();
        }
    });
    touchRecognizer_->SetOnTouchCancel([wp = AceType::WeakClaim(this)](const TouchEventInfo&) {
        auto sp = wp.Upgrade();
        if (sp) {
            sp->HandleTouchCancel();
        }
    });
}

void RenderGrid::OnChildAdded(const RefPtr<RenderNode>& renderNode)
{
    RenderNode::OnChildAdded(renderNode);
    childrenChanged_ = true;
    count_++;
}

Offset RenderGrid::LimitOffset(const Offset& offset)
{
    Offset limitOffset = offset;
    switch (direction_) {
        case FlexDirection::ROW:
            if (limitOffset.GetX() < 0.0) {
                // reach the smallest point X.
                limitOffset.SetX(0.0);
            } else if (limitOffset.GetX() > totalMainSize_ - GetLayoutSize().Width()) {
                // reach the largest point X.
                limitOffset.SetX(totalMainSize_ - GetLayoutSize().Width());
            } else {
                reachBetween_ = true;
            }
            limitOffset.SetY(0.0);
            break;
        case FlexDirection::COLUMN:
            if (limitOffset.GetY() < 0.0) {
                // reach the smallest point Y.
                limitOffset.SetY(0.0);
            } else if (limitOffset.GetY() > totalMainSize_ - GetLayoutSize().Height()) {
                // reach the largest point Y.
                limitOffset.SetY(totalMainSize_ - GetLayoutSize().Height());
            } else {
                reachBetween_ = true;
            }
            limitOffset.SetX(0.0);
            break;
        case FlexDirection::ROW_REVERSE:
            if (limitOffset.GetX() > 0.0) {
                // reach the largest point X.
                limitOffset.SetX(0.0);
            } else if (limitOffset.GetX() < GetLayoutSize().Width() - totalMainSize_) {
                // reach the smallest point X.
                limitOffset.SetX(GetLayoutSize().Width() - totalMainSize_);
            } else {
                reachBetween_ = true;
            }
            limitOffset.SetY(0.0);
            break;
        case FlexDirection::COLUMN_REVERSE:
            if (limitOffset.GetY() > 0.0) {
                // reach the largest point Y.
                limitOffset.SetY(0.0);
            } else if (limitOffset.GetY() < GetLayoutSize().Height() - totalMainSize_) {
                // reach the smallest point Y.
                limitOffset.SetY(GetLayoutSize().Height() - totalMainSize_);
            } else {
                reachBetween_ = true;
            }
            limitOffset.SetX(0.0);
            break;
        default:
            break;
    }
    return limitOffset;
}

bool RenderGrid::UpdateOffset(const Offset& delta)
{
    // if viewport display all content, no need to scroll.
    if (totalMainSize_ < GetMainSize(GetLayoutSize())) {
        return false;
    }
    if (NearZero(delta.GetX()) && NearZero(delta.GetY())) {
        return true;
    }
    Offset oldOffset = offset_;
    offset_ += delta;
    offset_ = LimitOffset(offset_);

    if (oldOffset != offset_) {
        if (onTouch_) {
            // TouchDown and scrolling: continuity send event scroll(state:1).
            OnScroll(delta.GetX(), delta.GetY(), SCROLL_TOUCH_DOWN);
        } else {
            // TouchUp and scroll continue: send event scrolltouchup once.
            if (triggerTouchUp_) {
                OnScrollTouchUp();
                triggerTouchUp_ = false;
            }
            // TouchUp and scrolling: continuity send event scroll(state:2).
            OnScroll(delta.GetX(), delta.GetY(), SCROLL_TOUCH_UP);
        }
        MarkNeedLayout();
        return true;
    }
    // delta offset make no change, mean reach to the scroll limit.
    return false;
}

void RenderGrid::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!GetVisible()) {
        return;
    }

    if (!scrollable_ || !touchRecognizer_) {
        return;
    }
    scrollable_->SetCoordinateOffset(coordinateOffset);
    touchRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(scrollable_);
    result.emplace_back(touchRecognizer_);
}

void RenderGrid::Update(const RefPtr<Component>& component)
{
    const RefPtr<GridComponent> grid = AceType::DynamicCast<GridComponent>(component);
    if (!grid) {
        return;
    }

    updateFlag_ = true;
    bool directionChanged = direction_ != grid->GetDirection();
    direction_ = grid->GetDirection();
    if (direction_ == FlexDirection::COLUMN || direction_ == FlexDirection::COLUMN_REVERSE) {
        isVertical_ = true;
    }

    crossAxisAlign_ = grid->GetFlexAlign();
    maxCount_ = grid->GetTotalCount();
    columnCount_ = grid->GetColumnCount();
    columnExtent_ = grid->GetColumnExtent();
    cachedCount_ = grid->GetCachedCount();
    gridWidth_ = grid->GetWidth();
    gridHeight_ = grid->GetHeight();
    onScroll_ = AceAsyncEvent<void(const std::string&)>::Create(grid->GetOnScroll(), context_);
    onScrollBottom_ = AceAsyncEvent<void(const std::string&)>::Create(grid->GetOnScrollBottom(), context_);
    onScrollTop_ = AceAsyncEvent<void(const std::string&)>::Create(grid->GetOnScrollTop(), context_);
    onScrollEnd_ = AceAsyncEvent<void(const std::string&)>::Create(grid->GetOnScrollEnd(), context_);
    onScrollTouchUp_ = AceAsyncEvent<void(const std::string&)>::Create(grid->GetOnScrollTouchUp(), context_);

    gridController_ = grid->GetGridController();
    if (gridController_) {
        gridController_->SetGridNode(AceType::WeakClaim(this));
    }

    if (directionChanged || !scrollable_) {
        ResetScrollable();
    }

    if (!animator_) {
        animator_ = AceType::MakeRefPtr<Animator>(GetContext());
    }
    MarkNeedLayout();
}

int32_t RenderGrid::GetCurrentIndex() const
{
    if (NearEqual(GetCurrentPosition(), 0.0)) {
        return 0;
    }
    int32_t index = 0;
    double vertex = 0.0;
    for (const auto& item : GetChildren()) {
        switch (direction_) {
            case FlexDirection::ROW:
                if (std::abs(item->GetPosition().GetX()) < item->GetLayoutSize().Width()) {
                    return index;
                }
                break;
            case FlexDirection::COLUMN:
                if (std::abs(item->GetPosition().GetY()) < item->GetLayoutSize().Height()) {
                    return index;
                }
                break;
            case FlexDirection::ROW_REVERSE:
                vertex = item->GetPosition().GetX() + item->GetLayoutSize().Width();
                if (std::abs(vertex - GetLayoutSize().Width()) < item->GetLayoutSize().Width()) {
                    return index;
                }
                break;
            case FlexDirection::COLUMN_REVERSE:
                vertex = item->GetPosition().GetY() + item->GetLayoutSize().Height();
                if (std::abs(vertex - GetLayoutSize().Height()) < item->GetLayoutSize().Height()) {
                    return index;
                }
                break;
            default:
                break;
        }
        ++index;
    }
    return index;
}

double RenderGrid::GetCurrentPosition() const
{
    if (isVertical_) {
        return offset_.GetY();
    } else {
        return offset_.GetX();
    }
}

Offset RenderGrid::GetChildOffset(const RefPtr<RenderNode>& child) const
{
    Offset offset;
    Offset position = child->GetPosition() + offset_;
    Size layout = child->GetLayoutSize();
    switch (direction_) {
        case FlexDirection::ROW:
            offset.SetX(position.GetX());
            break;
        case FlexDirection::COLUMN:
            offset.SetY(position.GetY());
            break;
        case FlexDirection::ROW_REVERSE:
            offset.SetX((position.GetX() + layout.Width()) - GetLayoutSize().Width());
            break;
        case FlexDirection::COLUMN_REVERSE:
            offset.SetY((position.GetY() + layout.Height()) - GetLayoutSize().Height());
            break;
        default:
            break;
    }
    return offset;
}

void RenderGrid::JumpTo(int32_t index)
{
    LOGD("Jump to index: %{public}d.", index);
    int32_t size = static_cast<int32_t>(GetChildren().size());
    if (index < 0 || index >= size) {
        LOGW("Invalid index: %{public}d.", index);
        return;
    }
    auto iter = GetChildren().begin();
    std::advance(iter, index);
    if (!(*iter)) {
        LOGW("Target child is nullptr.");
        return;
    }
    Offset offset = GetChildOffset(*iter);
    JumpTo(isVertical_ ? offset.GetY() : offset.GetX());
}

void RenderGrid::AnimateTo(int32_t index, float duration, const RefPtr<Curve>& curve)
{
    LOGD("Animate to index: %{public}d.", index);
    int32_t size = static_cast<int32_t>(GetChildren().size());
    if (index < 0 || index >= size) {
        LOGW("Invalid index: %{public}d.", index);
        return;
    }
    auto iter = GetChildren().begin();
    std::advance(iter, index);
    if (!(*iter)) {
        LOGW("Target child is nullptr.");
        return;
    }
    Offset offset = GetChildOffset(*iter);
    AnimateTo(isVertical_ ? offset.GetY() : offset.GetX(), duration, curve);
}

void RenderGrid::JumpTo(double position)
{
    if (NearEqual(GetCurrentPosition(), position)) {
        LOGW("Already at position: %{public}lf.", position);
        return;
    }
    LOGD("Jump from %{public}lf to %{public}lf.", GetCurrentPosition(), position);
    Offset offset;
    if (isVertical_) {
        offset.SetY(position);
    } else {
        offset.SetX(position);
    }
    offset_ = LimitOffset(offset);
    MarkNeedLayout();
}

void RenderGrid::AnimateTo(double position, float duration, const RefPtr<Curve>& curve)
{
    if (NearEqual(GetCurrentPosition(), position)) {
        LOGW("Already at position: %{public}lf.", position);
        return;
    }
    LOGD("Animate from %{public}lf to %{public}lf", GetCurrentPosition(), position);
    if (!animator_->IsStopped()) {
        animator_->Stop();
    }
    animator_->ClearInterpolators();
    auto animation = AceType::MakeRefPtr<CurveAnimation<double>>(GetCurrentPosition(), position, curve);
    animation->AddListener([weakGrid = AceType::WeakClaim(this)](const double& value) {
        auto grid = weakGrid.Upgrade();
        if (grid) {
            grid->JumpTo(value);
        }
    });
    animator_->AddInterpolator(animation);
    animator_->SetDuration(duration);
    animator_->Play();
}

void RenderGrid::ScrollBy(int32_t pixelX, int32_t pixelY, bool smooth)
{
    double distance = isVertical_ ? pixelY : pixelX;
    if (NearZero(distance)) {
        return;
    }
    double offset = distance + (isVertical_ ? offset_.GetY() : offset_.GetX());
    if (smooth) {
        AnimateTo(offset, std::abs(distance) * DURATION_SECOND_TO_MS / SCROLL_BY_SPEED, Curves::LINEAR);
    } else {
        JumpTo(offset);
    }
}

int32_t RenderGrid::RequestNextFocus(bool vertical, bool reverse, Offset position)
{
    int32_t focusIndex = FOCUS_LOST;
    if (GetChildren().empty()) {
        LOGD("No Child, can not obtain focus.");
        return focusIndex;
    }
    // (false false)->RIGHT   (false true)->LEFT   (true false)->DOWN   (true true)->UP
    KeyDirection key = DIRECTION_MAP.at(direction_).at(vertical).at(reverse);
    if (focused_) {
        auto curIter = childRelations_.find(focusIndex_);
        if (curIter != childRelations_.end() && curIter->second[key] >= 0) {
            focusIndex = curIter->second[key];
        }
    } else {
        switch (key) {
            case KeyDirection::UP:
                focusIndex = focusMoveUp(position);
                break;
            case KeyDirection::DOWN:
                focusIndex = focusMoveDown(position);
                break;
            case KeyDirection::LEFT:
                focusIndex = focusMoveLeft(position);
                break;
            case KeyDirection::RIGHT:
                focusIndex = focusMoveRight(position);
                break;
            default:
                break;
        }
    }
    LOGD("Current focusIndex:%{public}d focused:%{public}d", focusIndex, focused_);
    if (focusIndex >= 0) {
        focusIndex_ = focusIndex;
        MoveChildToViewPort(focusIndex);
    }
    return focusIndex;
}

void RenderGrid::HandleOnFocus(int32_t focusIndex)
{
    LOGD("OnFocus current focusIndex : %{public}d", focusIndex);
    focusIndex_ = focusIndex;
    focused_ = true;
    MoveChildToViewPort(focusIndex);
}

void RenderGrid::HandleOnBlur()
{
    LOGD("OnBlur previous focusIndex : %{public}d", focusIndex_);
    focused_ = false;
}

void RenderGrid::MoveChildToViewPort(int32_t index)
{
    auto child = GetChildByIndex(index);
    if (!child) {
        LOGE("Index:%{public}d no child exist.", index);
        return;
    }
    double childMainSize = GetMainSize(child->GetLayoutSize());
    double gridMainSize = GetMainSize(GetLayoutSize());
    double childMainPos = isVertical_ ? child->GetPosition().GetY() : child->GetPosition().GetX();
    double curOffset = isVertical_ ? offset_.GetY() : offset_.GetX();
    if (childMainPos < 0.0) {
        // The head is out of the viewport
        JumpTo(childMainPos + curOffset);
        return;
    }
    if (childMainPos + childMainSize > gridMainSize) {
        // The tail is out of the viewport
        JumpTo(childMainPos + curOffset + childMainSize - gridMainSize);
        return;
    }
}

RefPtr<RenderNode> RenderGrid::GetChildByIndex(int32_t index) const
{
    int32_t size = static_cast<int32_t>(GetChildren().size());
    if (index < 0 || index >= size) {
        return nullptr;
    }
    auto pos = GetChildren().begin();
    std::advance(pos, index);
    return *pos;
}

Offset RenderGrid::GetCenterPosition(const Offset& pos, const Size& size) const
{
    double width = size.Width();
    double height = size.Height();
    Offset center = (pos + Offset(width / DOUBLE_FACTOR, height / DOUBLE_FACTOR));
    return center;
}

bool RenderGrid::IsChildVisible(const RefPtr<RenderNode>& node) const
{
    Offset childCenter = GetCenterPosition(node->GetPosition(), node->GetLayoutSize());
    Offset parentCenter = GetCenterPosition(Offset(0.0, 0.0), GetLayoutSize());
    Offset distance = childCenter - parentCenter;
    Size nodeSize = node->GetLayoutSize();
    Size viewPortSize = GetLayoutSize();
    double halfW = (nodeSize.Width() + viewPortSize.Width()) / DOUBLE_FACTOR;
    double halfH = (nodeSize.Height() + viewPortSize.Height()) / DOUBLE_FACTOR;
    if (std::abs(distance.GetX()) < halfW && std::abs(distance.GetY()) < halfH) {
        return true;
    }
    return false;
}

int32_t RenderGrid::FocusMove(const Offset& position, const std::function<bool(int32_t)>& condition,
    KeyDirection keyDirection, bool checkChildVisible)
{
    double distance = -1.0;
    int32_t index = -1;
    auto iter = childRelations_.begin();
    for (; iter != childRelations_.end() && (!condition || condition(iter->first)); ++iter) {
        if ((keyDirection == KeyDirection::LEFT || keyDirection == KeyDirection::RIGHT) &&
            iter->second[keyDirection] != -1) {
            // Not the keyDirection column, just continue
            continue;
        }
        auto child = GetChildByIndex(iter->first);
        if (child == nullptr) {
            continue;
        }
        if (checkChildVisible && !IsChildVisible(child)) {
            continue;
        }

        Offset curPos = GetCenterPosition(child->GetPosition(), child->GetLayoutSize());
        double curDis = sqrt((position.GetX() - curPos.GetX()) * (position.GetX() - curPos.GetX()) +
                             (position.GetY() - curPos.GetY()) * (position.GetY() - curPos.GetY()));
        if (distance < 0) {
            distance = curDis;
            index = iter->first;
        } else {
            if (distance < curDis) {
                break;
            } else {
                distance = curDis;
                index = iter->first;
            }
        }
    }
    return index;
}

// Handle direction key up
int32_t RenderGrid::focusMoveUp(const Offset& position)
{
    double distance = -1.0;
    int32_t index = -1;
    auto iter = childRelations_.rbegin();
    int32_t startPos = indexPositions_[iter->first];
    int32_t scope = columnCount_ != 0 ? startPos % columnCount_ : 0;
    for (; iter != childRelations_.rend() && scope > 0; ++iter) {
        auto child = GetChildByIndex(iter->first);
        if (child == nullptr) {
            continue;
        }
        Offset curPos = GetCenterPosition(child->GetPosition(), child->GetLayoutSize());
        double curDis = sqrt((position.GetX() - curPos.GetX()) * (position.GetX() - curPos.GetX()) +
                             (position.GetY() - curPos.GetY()) * (position.GetY() - curPos.GetY()));
        if (distance < 0) {
            distance = curDis;
            index = iter->first;
        } else {
            if (distance < curDis) {
                break;
            } else {
                distance = curDis;
                index = iter->first;
            }
        }
        scope = scope - (startPos - indexPositions_[iter->first]);
    }
    return index;
}

// Handle direction key down
int32_t RenderGrid::focusMoveDown(const Offset& position)
{
    return FocusMove(
        position, [grid = Claim(this)](int32_t index) { return grid->indexPositions_[index] < grid->columnCount_; },
        KeyDirection::DOWN, false);
}

// Handle direction key left
int32_t RenderGrid::focusMoveLeft(const Offset& position)
{
    return FocusMove(position, nullptr, KeyDirection::RIGHT, true);
}

// Handle direction key right
int32_t RenderGrid::focusMoveRight(const Offset& position)
{
    return FocusMove(position, nullptr, KeyDirection::LEFT, true);
}

LayoutParam RenderGrid::MakeInnerLayoutParam(int32_t columnSpan) const
{
    LayoutParam innerLayout;

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
    return innerLayout;
}

void RenderGrid::BuildPositionRelations(int32_t gridPos, int32_t columnSpan, int32_t index)
{
    gridPositions_[gridPos] = index;
    indexPositions_[index] = gridPos;
    RelationMap relation;
    int32_t size = static_cast<int32_t>(GetChildren().size());
    relation[KeyDirection::UP] = -1;
    relation[KeyDirection::DOWN] = -1;
    relation[KeyDirection::LEFT] = gridPos % columnCount_ > 0 ? index - 1 : -1;
    relation[KeyDirection::RIGHT] = (gridPos + columnSpan) % columnCount_ == 0 ? -1 :
                                    index + 1 < size ? index + 1 : -1;
    if (gridPos >= columnCount_) {
        for (int32_t i = 0; i < columnSpan; ++i) {
            int32_t targetPos = gridPos - columnCount_ + i;
            do {
                auto iter = gridPositions_.find(targetPos);
                if (iter != gridPositions_.end()) {
                    if (relation[KeyDirection::UP] == -1) {
                        relation[KeyDirection::UP] = iter->second;
                    }
                    auto upItem = childRelations_.find(iter->second);
                    if (upItem != childRelations_.end() && upItem->second[KeyDirection::DOWN] == -1) {
                        upItem->second[KeyDirection::DOWN] = index;
                    }
                    break;
                }
                --targetPos;
            } while (targetPos >= 0 && (targetPos + 1) % columnCount_ > 0);
        }
    }
    childRelations_[index] = relation;
}

void RenderGrid::SetChildPosition(const RefPtr<RenderNode>& child, double mainSize, int32_t gridPos, int32_t columnSpan)
{
    double mainAxis = mainSize - GetMainSize(child->GetLayoutSize());
    double crossAxis = 0.0;
    switch (crossAxisAlign_) {
        case FlexAlign::FLEX_END:
            crossAxis = gridLen_ * columnSpan - GetCrossSize(child->GetLayoutSize());
            break;
        case FlexAlign::CENTER:
            crossAxis = (gridLen_ * columnSpan - GetCrossSize(child->GetLayoutSize())) / DOUBLE_FACTOR;
            break;
        case FlexAlign::STRETCH:
        case FlexAlign::FLEX_START:
        default:
            break;
    }
    if (isVertical_) {
        if (direction_ == FlexDirection::COLUMN_REVERSE) {
            mainAxis = mainSize_ - GetMainSize(child->GetLayoutSize()) - mainAxis;
        }
        crossAxis += (gridPos % columnCount_) * gridLen_;
        child->SetPosition(Offset(crossAxis, mainAxis) - offset_);
    } else {
        if (direction_ == FlexDirection::ROW_REVERSE) {
            mainAxis = mainSize_ - GetMainSize(child->GetLayoutSize()) - mainAxis;
        }
        crossAxis += (gridPos % columnCount_) * gridLen_;
        child->SetPosition(Offset(mainAxis, crossAxis) - offset_);
    }
}

void RenderGrid::CalculateAxisSize()
{
    if (isVertical_) {
        mainSize_ = ((gridHeight_ > 0.0) && (gridHeight_ < GetLayoutParam().GetMaxSize().Height())) ?
            gridHeight_ : GetLayoutParam().GetMaxSize().Height();
        crossSize_ = ((gridWidth_ > 0.0) && (gridWidth_ < GetLayoutParam().GetMaxSize().Width())) ?
            gridWidth_ : GetLayoutParam().GetMaxSize().Width();
    } else {
        mainSize_ = ((gridWidth_ > 0.0) && (gridWidth_ < GetLayoutParam().GetMaxSize().Width())) ?
            gridWidth_ : GetLayoutParam().GetMaxSize().Width();
        crossSize_ = ((gridHeight_ > 0.0) && (gridHeight_ < GetLayoutParam().GetMaxSize().Height())) ?
            gridHeight_ : GetLayoutParam().GetMaxSize().Height();
    }
}

void RenderGrid::InitialGridProp()
{
    // Not first time layout after update, no need to initial.
    if (!updateFlag_) {
        return;
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

    // Initialize the columnLenth
    if (!NearZero(columnCount_)) {
        gridLen_ = crossSize_ / columnCount_;
    }
}

int32_t RenderGrid::GetChildSpan(const RefPtr<RenderNode>& child) const
{
    int32_t depth = DEFAULT_DEPTH;
    int32_t span = 1; // Default span is 1, means this child takes up one column.
    auto item = child;
    auto gridItem = AceType::DynamicCast<RenderGridItem>(item);
    while (!gridItem && depth > 0) {
        if (!item || item->GetChildren().empty()) {
            return span;
        }

        item = item->GetChildren().front();
        gridItem = AceType::DynamicCast<RenderGridItem>(item);
        --depth;
    }

    if (gridItem) {
        span = gridItem->GetColumnSpan();
        if (span <= 0) {
            span = 1; // Invalid span and make it default value 1.
        }
    }

    return span;
}

void RenderGrid::PerformLayout()
{
    const auto& children = GetChildren();
    LOGD("Grid Perform layout with %{public}zu children", children.size());

    posOfChildren_.clear();

    // Calculate the mainAxisSize and crossAxisSize
    CalculateAxisSize();

    // Initialize the columnCount columnLength columnSpan
    InitialGridProp();

    if (!children.empty()) {
        int32_t index = 0;
        double totalMainSize = 0.0;
        LayoutParam innerLayout;
        // First time layout all children and update relative position.
        for (const auto& item : children) {
            int32_t columnSpan = GetChildSpan(item);
            columnSpan = std::min(columnSpan, columnCount_ - (index % columnCount_));
            innerLayout = MakeInnerLayoutParam(columnSpan);
            item->Layout(innerLayout);
            double curMainSize = index - columnCount_ < 0 ? 0.0 : posOfChildren_[index - columnCount_];
            for (int32_t i = 0; i < columnSpan; ++i) {
                posOfChildren_.emplace_back(GetMainSize(item->GetLayoutSize()) + curMainSize);
            }
            totalMainSize = std::max(totalMainSize, GetMainSize(item->GetLayoutSize()) + curMainSize);
            index += columnSpan;
        }
        totalMainSize_ = totalMainSize;
    }

    SetLayoutSize(GetLayoutParam().Constrain(MakeValue<Size>(mainSize_, crossSize_)));

    // Requests more child.
    RequestChildIfNeeded();

    // Traverses all the children to update visible flag.
    Traverse();

    if (updateFlag_) {
        // When initial index not 0, jumpto target index.
        if (gridController_->GetInitialIndex() > 0) {
            JumpTo(gridController_->GetInitialIndex());
            Traverse();
        }
        updateFlag_ = false;
        OnScroll(0.0, 0.0, SCROLL_NONE); // Trigger a scroll stop event when finish first layout.
    }
}

void RenderGrid::RequestChildIfNeeded()
{
    if (GetChildren().empty()) {
        if (maxCount_ > 0 && callback_) {
            callback_(0, std::min(cachedCount_, maxCount_));
        }
        return;
    }

    // Calculates the start and end of the viewport.
    double viewportSize = GetMainSize(GetLayoutSize());
    double viewportStart = 0.0;
    switch (direction_) {
        case FlexDirection::ROW:
            viewportStart = offset_.GetX();
            break;
        case FlexDirection::COLUMN:
            viewportStart = offset_.GetY();
            break;
        case FlexDirection::ROW_REVERSE:
            viewportStart = -offset_.GetX();
            break;
        case FlexDirection::COLUMN_REVERSE:
            viewportStart = -offset_.GetY();
            break;
        default:
            break;
    }
    double viewportEnd = viewportStart + viewportSize;

    // Calculates the number of child nodes in the cache.
    auto beginCached = std::lower_bound(posOfChildren_.begin(), posOfChildren_.end(), viewportEnd);
    auto cachedNum = static_cast<int32_t>(std::distance(beginCached, posOfChildren_.end()));

    // Requests more child.
    if ((cachedNum < cachedCount_)) {
        int32_t remain = maxCount_ - (index_ + count_);
        if (remain > 0 && callback_) {
            callback_(index_ + count_, std::min(cachedCount_ - cachedNum, remain));
        }
    }
}

void RenderGrid::Traverse()
{
    if (GetChildren().empty()) {
        return;
    }
    if (posOfChildren_.empty()) {
        return;
    }

    // Traverses child to update position and render flag.
    int32_t gridPos = 0;
    int32_t index = 0;
    if (childrenChanged_) {
        gridPositions_.clear();
        childRelations_.clear();
    }
    for (const auto& item : GetChildren()) {
        int32_t columnSpan = GetChildSpan(item);
        columnSpan = std::min(columnSpan, columnCount_ - (gridPos % columnCount_));
        LOGD("GridPos %{private}d Span: %{private}d", gridPos, columnSpan);
        SetChildPosition(item, posOfChildren_[gridPos], gridPos, columnSpan);
        if (childrenChanged_) {
            BuildPositionRelations(gridPos, columnSpan, index);
        }
        gridPos += columnSpan;
        ++index;
    }
    childrenChanged_ = false;
}

void RenderGrid::ResetScrollable()
{
    const auto isVertical = isVertical_;
    auto&& callback = [weakGrid = AceType::WeakClaim(this), isVertical](double value, int32_t source) {
        auto grid = weakGrid.Upgrade();
        if (!grid) {
            LOGE("grid is nullptr when reset scrollable");
            return false;
        }
        if (source == SCROLL_FROM_START) {
            return false;
        }
        Offset delta;
        if (isVertical) {
            delta.SetX(0.0);
            delta.SetY(-value);
        } else {
            delta.SetX(-value);
            delta.SetY(0.0);
        }
        return grid->UpdateOffset(delta);
    };
    // Initializes scrollable with different direction.
    if (isVertical) {
        scrollable_ = AceType::MakeRefPtr<Scrollable>(callback, Axis::VERTICAL);
    } else {
        scrollable_ = AceType::MakeRefPtr<Scrollable>(callback, Axis::HORIZONTAL);
    }
    scrollable_->Initialize(GetContext());
    scrollable_->SetScrollEndCallback([weakGrid = AceType::WeakClaim(this)](void) {
        auto grid = weakGrid.Upgrade();
        if (grid) {
            grid->HandleScrollEnd();
        }
    });
}

void RenderGrid::HandleTouchDown()
{
    onTouch_ = true;
    triggerTouchUp_ = true;
}

void RenderGrid::HandleTouchUp()
{
    onTouch_ = false;
}

void RenderGrid::HandleTouchCancel()
{
    onTouch_ = false;
}

void RenderGrid::HandleScrollEnd()
{
    LOGD("HandleScrollEnd");
    if (NearZero(offset_.GetX()) && NearZero(offset_.GetY())) {
        reachTop_ = true;
    }
    switch (direction_) {
        case FlexDirection::ROW:
            if (NearEqual(offset_.GetX(), totalMainSize_ - GetLayoutSize().Width())) {
                reachBottom_ = true;
            }
            break;
        case FlexDirection::COLUMN:
            if (NearEqual(offset_.GetY(), totalMainSize_ - GetLayoutSize().Height())) {
                reachBottom_ = true;
            }
            break;
        case FlexDirection::ROW_REVERSE:
            if (NearEqual(offset_.GetX(), GetLayoutSize().Width() - totalMainSize_)) {
                reachBottom_ = true;
            }
            break;
        case FlexDirection::COLUMN_REVERSE:
            if (NearEqual(offset_.GetY(), GetLayoutSize().Height() - totalMainSize_)) {
                reachBottom_ = true;
            }
            break;
        default:
            break;
    }
    OnScrollEnd();
    if (reachBottom_ && reachBetween_) {
        OnScrollBottom();
        reachBottom_ = false;
        reachBetween_ = false;
    }
    if (reachTop_ && reachBetween_) {
        OnScrollTop();
        reachTop_ = false;
        reachBetween_ = false;
    }
    OnScroll(0.0, 0.0, SCROLL_NONE);
}

void RenderGrid::OnScroll(double scrollX, double scrollY, int32_t scrollState) const
{
    if (onScroll_) {
        std::string param = std::string("\"scroll\",{\"scrollX\":")
            .append(std::to_string(scrollX))
            .append(",\"scrollY\":")
            .append(std::to_string(scrollY))
            .append(",\"scrollState\":")
            .append(std::to_string(scrollState))
            .append("},null");
        onScroll_(param);
    }
}

void RenderGrid::OnScrollBottom() const
{
    LOGD("OnScrollBottom");
    if (onScrollBottom_) {
        onScrollBottom_(std::string("\"scrollbottom\",null"));
    }
}

void RenderGrid::OnScrollTop() const
{
    LOGD("OnScrollTop");
    if (onScrollTop_) {
        onScrollTop_(std::string("\"scrolltop\",null"));
    }
}

void RenderGrid::OnScrollEnd() const
{
    LOGD("OnScrollEnd");
    if (onScrollEnd_) {
        onScrollEnd_(std::string("\"scrollend\",null"));
    }
}

void RenderGrid::OnScrollTouchUp() const
{
    LOGD("OnScrollTouchUp");
    if (onScrollTouchUp_) {
        onScrollTouchUp_(std::string("\"scrolltouchup\",null"));
    }
}

} // namespace OHOS::Ace
