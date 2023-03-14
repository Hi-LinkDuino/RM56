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

#include "core/components/grid_layout/grid_layout_component.h"

#include "core/components/grid_layout/grid_layout_element.h"
#include "core/components/grid_layout/render_grid_layout.h"
#include "core/components_v2/grid/grid_element.h"
#include "core/components_v2/grid/render_grid_scroll.h"
#include "core/pipeline/base/multi_composed_component.h"

namespace OHOS::Ace {

RefPtr<Element> GridLayoutComponent::CreateElement()
{
    // judge to create GridLayoutElement for dynamic grid
    if (((rowsArgs_.empty() && (!columnsArgs_.empty())) || ((!rowsArgs_.empty()) && columnsArgs_.empty())) &&
        (maxCount_ >= minCount_) && (minCount_ >= 1) && (cellLength_ > 0) && (editMode_ == true)) {
        return AceType::MakeRefPtr<GridLayoutElement>();
    }
    if (isDeclarative_ && useScroll_ && (rowsArgs_.empty() || columnsArgs_.empty())) {
        return AceType::MakeRefPtr<V2::GridElement>();
    }
    return AceType::MakeRefPtr<GridLayoutElement>();
}

RefPtr<RenderNode> GridLayoutComponent::CreateRenderNode()
{
    // judge to create RenderGridLayout for dynamic grid
    if (((rowsArgs_.empty() && (!columnsArgs_.empty())) || ((!rowsArgs_.empty()) && columnsArgs_.empty())) &&
        (maxCount_ >= minCount_) && (minCount_ >= 1) && (cellLength_ > 0) && (editMode_ == true)) {
        return RenderGridLayout::Create();
    }
    if (isDeclarative_ && useScroll_ && (rowsArgs_.empty() || columnsArgs_.empty())) {
        return V2::RenderGridScroll::Create();
    }

    return RenderGridLayout::Create();
}

void GridLayoutComponent::SetDirection(FlexDirection direction)
{
    if (direction < FlexDirection::ROW || direction > FlexDirection::COLUMN_REVERSE) {
        LOGW("Invalid direction %{public}d", direction);
        return;
    }
    direction_ = direction;
}

void GridLayoutComponent::SetFlexAlign(FlexAlign flexAlign)
{
    if (flexAlign < FlexAlign::FLEX_START || flexAlign > FlexAlign::STRETCH) {
        LOGW("Invalid flexAlign %{public}d", flexAlign);
        return;
    }
    flexAlign_ = flexAlign;
}

void GridLayoutComponent::SetColumnCount(int32_t count)
{
    if (count <= 0) {
        LOGW("Invalid ColumnCount %{public}d", count);
        return;
    }
    columnCount_ = count;
}

void GridLayoutComponent::SetRowCount(int32_t count)
{
    if (count <= 0) {
        LOGW("Invalid RowCount %{public}d", count);
        return;
    }
    rowCount_ = count;
}

void GridLayoutComponent::SetWidth(double width)
{
    if (width <= 0.0) {
        LOGW("Invalid Width %{public}lf", width);
        return;
    }
    width_ = width;
}

void GridLayoutComponent::SetHeight(double height)
{
    if (height <= 0.0) {
        LOGW("Invalid Height %{public}lf", height);
        return;
    }
    height_ = height;
}

void GridLayoutComponent::SetColumnsArgs(const std::string& columnsArgs)
{
    columnsArgs_ = columnsArgs;
}

void GridLayoutComponent::SetRowsArgs(const std::string& rowsArgs)
{
    rowsArgs_ = rowsArgs;
}

void GridLayoutComponent::SetColumnGap(const Dimension& columnGap)
{
    if (columnGap.Value() < 0.0) {
        LOGW("Invalid RowGap, use 0px");
        columnGap_ = 0.0_px;
        return;
    }
    columnGap_ = columnGap;
}

void GridLayoutComponent::SetRowGap(const Dimension& rowGap)
{
    if (rowGap.Value() < 0.0) {
        LOGW("Invalid RowGap, use 0px");
        rowGap_ = 0.0_px;
        return;
    }
    rowGap_ = rowGap;
}

void GridLayoutComponent::SetRightToLeft(bool rightToLeft)
{
    rightToLeft_ = rightToLeft;
}

void GridLayoutComponent::SetScrollBarColor(const std::string& color)
{
    scrollBarColor_ = color;
}

void GridLayoutComponent::SetScrollBarWidth(const std::string& width)
{
    scrollBarWidth_ = width;
}

void GridLayoutComponent::SetScrollBar(DisplayMode displayMode)
{
    displayMode_ = displayMode;
}

void GridLayoutComponent::SetOnGridDragEnterId(const OnGridDragEnterFunc& onGridDragEnterId)
{
    onGridDragEnterId_ = onGridDragEnterId;
}

void GridLayoutComponent::SetOnGridDragMoveId(const OnGridDragMoveFunc& onGridDragMoveId)
{
    onGridDragMoveId_ = onGridDragMoveId;
}

void GridLayoutComponent::SetOnGridDragLeaveId(const OnGridDragLeaveFunc& onGridDragLeaveId)
{
    onGridDragLeaveId_ = onGridDragLeaveId;
}

void GridLayoutComponent::SetOnGridDragStartId(const OnGridDragStartFunc& onGridDragStartId)
{
    onGridDragStartId_ = onGridDragStartId;
}

void GridLayoutComponent::SetOnGridDropId(const OnGridDropFunc& onGridDropId)
{
    onGridDropId_ = onGridDropId;
}

const OnGridDragEnterFunc& GridLayoutComponent::GetOnGridDragEnterId() const
{
    return onGridDragEnterId_;
}

const OnGridDragMoveFunc& GridLayoutComponent::GetOnGridDragMoveId() const
{
    return onGridDragMoveId_;
}

const OnGridDragLeaveFunc& GridLayoutComponent::GetOnGridDragLeaveId() const
{
    return onGridDragLeaveId_;
}

const OnGridDragStartFunc& GridLayoutComponent::GetOnGridDragStartId() const
{
    return onGridDragStartId_;
}

const OnGridDropFunc& GridLayoutComponent::GetOnGridDropId() const
{
    return onGridDropId_;
}

} // namespace OHOS::Ace
