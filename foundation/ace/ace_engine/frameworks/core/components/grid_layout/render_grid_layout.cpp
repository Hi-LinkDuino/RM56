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

#include "core/components/grid_layout/render_grid_layout.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <regex>

#include "base/geometry/offset.h"
#include "base/log/event_report.h"
#include "base/log/log.h"
#include "base/memory/referenced.h"
#include "base/utils/string_utils.h"
#include "base/utils/utils.h"
#include "core/animation/curve_animation.h"
#include "core/components/grid_layout/grid_layout_component.h"
#include "core/components/grid_layout/render_grid_layout_item.h"
#include "core/event/ace_event_helper.h"
#include "core/gestures/long_press_recognizer.h"
#include "core/gestures/pan_recognizer.h"
#include "core/gestures/sequenced_recognizer.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t DEFAULT_DEPTH = 10;
constexpr bool HORIZONTAL = false;
constexpr bool VERTICAL = true;
constexpr bool FORWARD = false;
constexpr bool REVERSE = true;
constexpr double FULL_PERCENT = 100.0;
constexpr uint32_t REPEAT_MIN_SIZE = 6;
constexpr int32_t GAP_DIVIDE_CONSTEXPR = 2;
constexpr int32_t CELL_EMPTY = -1;
constexpr int32_t CELL_FOR_INSERT = -2;
const char UNIT_PIXEL[] = "px";
const char UNIT_PERCENT[] = "%";
const char UNIT_RATIO[] = "fr";
const char UNIT_AUTO[] = "auto";
const char UNIT_AUTO_FILL[] = "auto-fill";
const char REPEAT_PREFIX[] = "repeat";
const std::regex REPEAT_NUM_REGEX(R"(^repeat\((\d+),(.+)\))", std::regex::icase); // regex for "repeat(2, 100px)"
const std::regex AUTO_REGEX(R"(^repeat\((.+),(.+)\))", std::regex::icase);        // regex for "repeat(auto-fill, 10px)"

// first bool mean if vertical, second bool mean if reverse
// false, false --> RIGHT
// false, true --> LEFT
// true, false --> DOWN
// true, true ---> UP
// This map will adapter the Grid FlexDirection with Key Direction.
const std::map<bool, std::map<bool, std::map<bool, KeyDirection>>> DIRECTION_MAP = {
    { false, // RTL is false
        { { HORIZONTAL, { { FORWARD, KeyDirection::RIGHT }, { REVERSE, KeyDirection::LEFT } } },
            { VERTICAL, { { FORWARD, KeyDirection::DOWN }, { REVERSE, KeyDirection::UP } } } } },
    { true, // RTL is true
        { { HORIZONTAL, { { FORWARD, KeyDirection::LEFT }, { REVERSE, KeyDirection::RIGHT } } },
            { VERTICAL, { { FORWARD, KeyDirection::DOWN }, { REVERSE, KeyDirection::UP } } } } }
};

} // namespace

void RenderGridLayout::Update(const RefPtr<Component>& component)
{
    const RefPtr<GridLayoutComponent> grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (!grid) {
        LOGE("RenderGridLayout update failed.");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }

    updateFlag_ = true;
    direction_ = grid->GetDirection();
    crossAxisAlign_ = grid->GetFlexAlign();
    gridWidth_ = grid->GetWidth();
    gridHeight_ = grid->GetHeight();
    colsArgs_ = grid->GetColumnsArgs();
    rowsArgs_ = grid->GetRowsArgs();
    userColGap_ = grid->GetColumnGap();
    userRowGap_ = grid->GetRowGap();
    rightToLeft_ = grid->GetRightToLeft();
    scrollBarWidth_ = grid->GetScrollBarWidth();
    scrollBarColor_ = grid->GetScrollBarColor();
    displayMode_ = grid->GetScrollBar();
    isDeclarative_ = grid->IsDeclarative();

    // update other new prop
    cellLength_ = grid->GetCellLength();
    mainCountMax_ = grid->GetMaxCount();
    mainCountMin_ = grid->GetMinCount();
    editMode_ = grid->GetEditMode();

    supportAnimation_ = grid->GetSupportAnimation();
    dragAnimation_ = grid->GetDragAnimation();
    edgeEffect_ = grid->GetEdgeEffection();
    onGridDragStartFunc_ = grid->GetOnGridDragStartId();
    OnGridDragEnterFunc_ = grid->GetOnGridDragEnterId();
    onGridDragMoveFunc_ = grid->GetOnGridDragMoveId();
    onGridDragLeaveFunc_ = grid->GetOnGridDragLeaveId();
    onGridDropFunc_ = grid->GetOnGridDropId();

    if (((rowsArgs_.empty() && (!colsArgs_.empty())) || ((!rowsArgs_.empty()) && colsArgs_.empty())) &&
        (mainCountMax_ >= mainCountMin_) && (mainCountMin_ >= 1) && (cellLength_ > 0) && (editMode_ == true)) {
        isDynamicGrid_ = true;
    }
    LOGD("editMode: %{public}d, isDynamicGrid: %{public}d, cellLength: %{public}d, mainCountMin: %{public}d, "
         "mainCountMax: %{public}d .",
        editMode_, isDynamicGrid_, cellLength_, mainCountMin_, mainCountMax_);

    CalIsVertical();

    component_ = grid;

    CreateSlideRecognizer();
    CreateSpringController();
    if (editMode_) {
        if (grid->GetOnGridDropId()) {
            CreateDragDropRecognizer();
        }
        InitAnimationController(GetContext());
    }

    isMultiSelectable_ = grid->GetMultiSelectable();

    MarkNeedLayout();
}

void RenderGridLayout::UpdateFocusInfo(int32_t focusIndex)
{
    if (focusIndex < 0) {
        LOGW("Invalid focus index, update focus info failed.");
        return;
    }
    if (focusIndex != focusIndex_) {
        LOGD("Update focus index from %{public}d to %{public}d", focusIndex_, focusIndex);
        focusIndex_ = focusIndex;
        for (const auto& gridMap : gridMatrix_) {
            for (const auto& grid : gridMap.second) {
                if (grid.second == focusIndex) {
                    focusRow_ = gridMap.first;
                    focusCol_ = grid.first;
                }
            }
        }
    }
}

int32_t RenderGridLayout::RequestNextFocus(bool vertical, bool reverse)
{
    KeyDirection key = DIRECTION_MAP.at(rightToLeft_).at(vertical).at(reverse);
    int32_t index = focusMove(key);
    if (index < 0) {
        return index;
    }
    return focusIndex_;
}

// Handle direction key move
int32_t RenderGridLayout::focusMove(KeyDirection direction)
{
    int32_t nextRow = focusRow_ < 0 ? 0 : focusRow_;
    int32_t nextCol = focusCol_ < 0 ? 0 : focusCol_;
    int32_t next = focusIndex_;
    while (focusIndex_ == next || next < 0) {
        switch (direction) {
            case KeyDirection::UP:
                --nextRow;
                break;
            case KeyDirection::DOWN:
                ++nextRow;
                break;
            case KeyDirection::LEFT:
                --nextCol;
                break;
            case KeyDirection::RIGHT:
                ++nextCol;
                break;
            default:
                return -1;
        }
        if (nextRow < 0 || nextCol < 0 || nextRow >= rowCount_ || nextCol >= colCount_) {
            return -1;
        }
        next = GetIndexByGrid(nextRow, nextCol);
    }
    LOGD("PreFocus:%{public}d CurrentFocus:%{public}d", focusIndex_, next);
    focusRow_ = nextRow;
    focusCol_ = nextCol;
    focusIndex_ = next;
    return next;
}

int32_t RenderGridLayout::GetIndexByGrid(int32_t row, int32_t column) const
{
    LOGD("%{public}s begin. row: %{public}d, column: %{public}d", __PRETTY_FUNCTION__, row, column);
    auto rowIter = gridMatrix_.find(row);
    if (rowIter != gridMatrix_.end()) {
        auto colIter = rowIter->second.find(column);
        if (colIter != rowIter->second.end()) {
            return colIter->second;
        }
    }
    return -1;
}

LayoutParam RenderGridLayout::MakeInnerLayoutParam(int32_t row, int32_t col, int32_t rowSpan, int32_t colSpan) const
{
    LOGD("%{public}d %{public}d %{public}d %{public}d", row, col, rowSpan, colSpan);
    LayoutParam innerLayout;
    double rowLen = 0.0;
    double colLen = 0.0;
    for (int32_t i = 0; i < rowSpan; ++i) {
        rowLen += gridCells_.at(row + i).at(col).Height();
    }
    rowLen += (rowSpan - 1) * rowGap_;
    for (int32_t i = 0; i < colSpan; ++i) {
        colLen += gridCells_.at(row).at(col + i).Width();
    }
    colLen += (colSpan - 1) * colGap_;
    if (crossAxisAlign_ == FlexAlign::STRETCH) {
        innerLayout.SetMinSize(Size(colLen, rowLen));
        innerLayout.SetMaxSize(Size(colLen, rowLen));
    } else {
        innerLayout.SetMaxSize(Size(colLen, rowLen));
    }
    return innerLayout;
}

void RenderGridLayout::SetChildPosition(
    const RefPtr<RenderNode>& child, int32_t row, int32_t col, int32_t rowSpan, int32_t colSpan)
{
    LOGD("%{public}s begin. row: %{public}d, col: %{public}d, rowSpan: %{public}d, colSpan: %{public}d",
        __PRETTY_FUNCTION__, row, col, rowSpan, colSpan);
    if (focusRow_ < 0 && focusCol_ < 0) {
        // Make the first item obtain focus.
        focusRow_ = row;
        focusCol_ = col;
    }

    // Calculate the position for current child.
    double positionX = 0.0;
    double positionY = 0.0;
    for (int32_t i = 0; i < row; ++i) {
        positionY += gridCells_.at(i).at(0).Height();
    }
    positionY += row * rowGap_;
    for (int32_t i = 0; i < col; ++i) {
        positionX += gridCells_.at(0).at(i).Width();
    }
    positionX += col * colGap_;

    // Calculate the size for current child.
    double rowLen = 0.0;
    double colLen = 0.0;
    for (int32_t i = 0; i < rowSpan; ++i) {
        rowLen += gridCells_.at(row + i).at(col).Height();
    }
    rowLen += (rowSpan - 1) * rowGap_;
    for (int32_t i = 0; i < colSpan; ++i) {
        colLen += gridCells_.at(row).at(col + i).Width();
    }
    colLen += (colSpan - 1) * colGap_;

    // If RTL, place the item from right.
    if (rightToLeft_) {
        positionX = colSize_ - positionX - colLen;
    }

    double widthOffset = (colLen - child->GetLayoutSize().Width()) / GAP_DIVIDE_CONSTEXPR;
    double heightOffset = (rowLen - child->GetLayoutSize().Height()) / GAP_DIVIDE_CONSTEXPR;
    if (CheckNeedShrink()) {
        auto allocatedPositionY = allocatedRowSizes_[row] + (row + rowSpan - 1) * rowGap_;
        child->SetPosition(Offset(positionX + widthOffset, allocatedPositionY));
    } else {
        child->SetPosition(Offset(positionX + widthOffset, positionY + heightOffset));
    }
}

Point RenderGridLayout::CalcChildPosition(
    const RefPtr<RenderNode>& child, int32_t row, int32_t col, int32_t rowSpan, int32_t colSpan)
{
    LOGD("%{public}s begin. row: %{public}d, col: %{public}d, rowSpan: %{public}d, colSpan: %{public}d",
        __PRETTY_FUNCTION__, row, col, rowSpan, colSpan);
    if (focusRow_ < 0 && focusCol_ < 0) {
        // Make the first item obtain focus.
        focusRow_ = row;
        focusCol_ = col;
    }

    // Calculate the position for current child.
    double positionX = 0.0;
    double positionY = 0.0;
    for (int32_t i = 0; i < row; ++i) {
        positionY += gridCells_.at(i).at(0).Height();
    }
    positionY += row * rowGap_;
    for (int32_t i = 0; i < col; ++i) {
        positionX += gridCells_.at(0).at(i).Width();
    }
    positionX += col * colGap_;

    // Calculate the size for current child.
    double rowLen = 0.0;
    double colLen = 0.0;
    for (int32_t i = 0; i < rowSpan; ++i) {
        rowLen += gridCells_.at(row + i).at(col).Height();
    }
    rowLen += (rowSpan - 1) * rowGap_;
    for (int32_t i = 0; i < colSpan; ++i) {
        colLen += gridCells_.at(row).at(col + i).Width();
    }
    colLen += (colSpan - 1) * colGap_;

    // If RTL, place the item from right.
    if (rightToLeft_) {
        positionX = colSize_ - positionX - colLen;
    }

    double widthOffset = (colLen - child->GetLayoutSize().Width()) / GAP_DIVIDE_CONSTEXPR;
    double heightOffset = (rowLen - child->GetLayoutSize().Height()) / GAP_DIVIDE_CONSTEXPR;
    return Point(positionX + widthOffset, positionY + heightOffset);
}

Point RenderGridLayout::CalcDragChildStartPosition(const ItemDragInfo& info)
{
    double gridPositionX = GetGlobalOffset().GetX();
    double gridPositionY = GetGlobalOffset().GetY();
    double dragRelativelyX = info.GetX() - gridPositionX;
    double dragRelativelyY = info.GetY() - gridPositionY;
    return Point(dragRelativelyX, dragRelativelyY);
}

Point RenderGridLayout::CalcDragChildEndPosition(int32_t rowIndex, int32_t colIndex)
{
    LOGD("CalcDragChildEndPosition>>>>>>rowIndex=%{public}d, colIndex=%{public}d", rowIndex, colIndex);
    double positionX = 0.0;
    double positionY = 0.0;
    for (int32_t i = 0; i < rowIndex; ++i) {
        positionY += gridCells_.at(i).at(0).Height();
    }

    positionY += rowIndex * rowGap_;
    for (int32_t i = 0; i < colIndex; ++i) {
        positionX += gridCells_.at(0).at(i).Width();
    }
    positionX += colIndex * colGap_;

    // If RTL, place the item from right.
    if (rightToLeft_) {
        double colLen = gridCells_.at(rowIndex).at(colIndex).Width();
        positionX = colSize_ - positionX - colLen;
    }
    return Point(positionX + GetGlobalOffset().GetX(), positionY + GetGlobalOffset().GetY());
}

void RenderGridLayout::DisableChild(const RefPtr<RenderNode>& child, int32_t index)
{
    LOGD("No grid space for this item[%{public}d]", index);
    LayoutParam zeroLayout;
    zeroLayout.SetMinSize(Size(0.0, 0.0));
    zeroLayout.SetMaxSize(Size(0.0, 0.0));
    child->Layout(zeroLayout);
    child->SetPosition(Offset(0.0, 0.0));
}

Size RenderGridLayout::GetTargetLayoutSize(int32_t row, int32_t col)
{
    Size size;
    if (GetChildren().empty()) {
        return size;
    }
    LayoutParam innerLayout; // Init layout param for auto item.
    innerLayout.SetMaxSize(Size(colSize_, rowSize_));
    std::vector<std::string> rows, cols;
    StringUtils::StringSpliter(rowsArgs_, ' ', rows);
    rowCount_ = rows.size() == 0 ? 1 : rows.size();
    StringUtils::StringSpliter(colsArgs_, ' ', cols);
    colCount_ = cols.size() == 0 ? 1 : cols.size();
    int32_t rowIndex = 0;
    int32_t colIndex = 0;
    int32_t itemIndex = 0;
    for (const auto& item : GetChildren()) {
        int32_t itemRow = GetItemRowIndex(item);
        int32_t itemCol = GetItemColumnIndex(item);
        int32_t itemRowSpan = GetItemSpan(item, true);
        int32_t itemColSpan = GetItemSpan(item, false);
        if (itemRow >= 0 && itemRow < rowCount_ && itemCol >= 0 && itemCol < colCount_ &&
            CheckGridPlaced(itemIndex, itemRow, itemCol, itemRowSpan, itemColSpan)) {
            if (itemRow == row && itemCol == col) {
                item->Layout(innerLayout);
                return item->GetLayoutSize();
            }
        } else {
            while (!CheckGridPlaced(itemIndex, rowIndex, colIndex, itemRowSpan, itemColSpan)) {
                GetNextGrid(rowIndex, colIndex);
                if (rowIndex >= rowCount_ || colIndex >= colCount_) {
                    break;
                }
            }
            if (rowIndex == row && colIndex == col) {
                item->Layout(innerLayout);
                return item->GetLayoutSize();
            }
        }
        ++itemIndex;
    }
    return size;
}

std::string RenderGridLayout::PreParseRows()
{
    if (rowsArgs_.empty() || rowsArgs_.find(UNIT_AUTO) == std::string::npos) {
        return rowsArgs_;
    }
    std::string rowsArgs;
    std::vector<std::string> strs;
    StringUtils::StringSpliter(rowsArgs_, ' ', strs);
    std::string current;
    int32_t rowArgSize = strs.size();
    for (int32_t i = 0; i < rowArgSize; ++i) {
        current = strs[i];
        if (strs[i] == std::string(UNIT_AUTO)) {
            Size layoutSize = GetTargetLayoutSize(i, 0);
            current = StringUtils::DoubleToString(layoutSize.Height()) + std::string(UNIT_PIXEL);
            gridMatrix_.clear();
        }
        rowsArgs += ' ' + current;
    }
    return rowsArgs;
}

std::string RenderGridLayout::PreParseCols()
{
    if (colsArgs_.empty() || colsArgs_.find(UNIT_AUTO) == std::string::npos) {
        return colsArgs_;
    }
    std::string colsArgs;
    std::vector<std::string> strs;
    StringUtils::StringSpliter(colsArgs_, ' ', strs);
    std::string current;
    int32_t colArgSize = strs.size();
    for (int32_t i = 0; i < colArgSize; ++i) {
        current = strs[i];
        if (strs[i] == std::string(UNIT_AUTO)) {
            Size layoutSize = GetTargetLayoutSize(0, i);
            current = StringUtils::DoubleToString(layoutSize.Width()) + std::string(UNIT_PIXEL);
            gridMatrix_.clear();
        }
        colsArgs += ' ' + current;
    }
    return colsArgs;
}

void RenderGridLayout::InitialGridProp()
{
    // Not first time layout after update, no need to initial.
    if (!updateFlag_) {
        return;
    }
    rowGap_ = NormalizePercentToPx(userRowGap_, true);
    colGap_ = NormalizePercentToPx(userColGap_, false);

    rowSize_ = ((gridHeight_ > 0.0) && (gridHeight_ < GetLayoutParam().GetMaxSize().Height())) ? gridHeight_
        : GetLayoutParam().GetMaxSize().Height();
    colSize_ = ((gridWidth_ > 0.0) && (gridWidth_ < GetLayoutParam().GetMaxSize().Width())) ? gridWidth_
        : GetLayoutParam().GetMaxSize().Width();
    if (NearEqual(rowSize_, Size::INFINITE_SIZE) &&
        (rowsArgs_.find(UNIT_PERCENT) != std::string::npos || rowsArgs_.find(UNIT_RATIO) != std::string::npos)) {
        rowSize_ = viewPort_.Height();
    }
    if (NearEqual(colSize_, Size::INFINITE_SIZE) &&
        (colsArgs_.find(UNIT_PERCENT) != std::string::npos || colsArgs_.find(UNIT_RATIO) != std::string::npos)) {
        colSize_ = viewPort_.Width();
    }
    LOGD("Row[%{public}s]: %{public}lf %{public}lf", rowsArgs_.c_str(), rowSize_, rowGap_);
    LOGD("Col[%{public}s]: %{public}lf %{public}lf", colsArgs_.c_str(), colSize_, colGap_);
    std::vector<double> rows = ParseArgs(PreParseRows(), rowSize_, rowGap_);
    std::vector<double> cols = ParseArgs(PreParseCols(), colSize_, colGap_);
    if (rows.empty()) {
        rows.push_back(rowSize_);
    }
    if (cols.empty()) {
        cols.push_back(colSize_);
    }
    if (NearEqual(rowSize_, Size::INFINITE_SIZE)) {
        rowSize_ = std::accumulate(rows.begin(), rows.end(), (rows.size() - 1) * rowGap_);
    }
    if (NearEqual(colSize_, Size::INFINITE_SIZE)) {
        colSize_ = std::accumulate(cols.begin(), cols.end(), (cols.size() - 1) * colGap_);
    }
    // Initialize the columnCount and rowCount, default is 1
    colCount_ = cols.size();
    rowCount_ = rows.size();
    itemCountMax_ = colCount_ * rowCount_;
    gridCells_.clear();
    int32_t row = 0;
    for (auto height : rows) {
        int32_t col = 0;
        for (auto width : cols) {
            gridCells_[row][col] = Size(width, height);
            ++col;
        }
        ++row;
    }
    UpdateAccessibilityAttr();
    LOGD("GridLayout: %{public}lf %{public}lf %{public}d %{public}d", colSize_, rowSize_, colCount_, rowCount_);
}

void RenderGridLayout::UpdateAccessibilityAttr()
{
    auto refPtr = accessibilityNode_.Upgrade();
    if (!refPtr) {
        LOGI("accessibility node is not enabled.");
        return;
    }
    auto collectionInfo = refPtr->GetCollectionInfo();
    collectionInfo.rows = rowCount_;
    collectionInfo.columns = colCount_;
    refPtr->SetCollectionInfo(collectionInfo);
}

// Support five ways below:
// (1) 50px 100px 60px
// (2) 1fr 1fr 2fr
// (3) 30% 20% 50%
// (4) repeat(2,100px 20%) -- will be prebuild by JS Engine to --- 100px 20% 100px 20%
// (5) repeat(auto-fill, 100px 300px)  -- will be prebuild by JS Engine to --- auto-fill 100px 300px
std::vector<double> RenderGridLayout::ParseArgs(const std::string& args, double size, double gap)
{
    std::vector<double> lens;
    if (args.empty()) {
        return lens;
    }
    double pxSum = 0.0; // First priority: such as 50px
    double peSum = 0.0; // Second priority: such as 20%
    double frSum = 0.0; // Third priority: such as 2fr
    std::vector<std::string> strs;
    std::string handledArg = args;
    ConvertRepeatArgs(handledArg);
    StringUtils::StringSpliter(handledArg, ' ', strs);
    if (!strs.empty() && strs[0] == UNIT_AUTO_FILL) {
        return ParseAutoFill(strs, size, gap);
    }
    // first loop calculate all type sums.
    for (auto str : strs) {
        if (str.find(UNIT_PIXEL) != std::string::npos) {
            pxSum += StringUtils::StringToDouble(str);
        } else if (str.find(UNIT_PERCENT) != std::string::npos) {
            peSum += StringUtils::StringToDouble(str);
        } else if (str.find(UNIT_RATIO) != std::string::npos) {
            frSum += StringUtils::StringToDouble(str);
        } else {
            LOGE("Unsupported type: %{public}s, and use 0.0", str.c_str());
        }
    }
    if (GreatOrEqual(peSum, FULL_PERCENT)) {
        peSum = FULL_PERCENT;
    }
    // Second loop calculate actual width or height.
    double sizeLeft = size - (strs.size() - 1) * gap;
    double prSumLeft = FULL_PERCENT;
    double frSizeSum = size * (FULL_PERCENT - peSum) / FULL_PERCENT - (strs.size() - 1) * gap - pxSum;
    for (const auto& str : strs) {
        double num = StringUtils::StringToDouble(str);
        if (str.find(UNIT_PIXEL) != std::string::npos) {
            lens.push_back(sizeLeft < 0.0 ? 0.0 : std::clamp(num, 0.0, sizeLeft));
            sizeLeft -= num;
        } else if (str.find(UNIT_PERCENT) != std::string::npos) {
            num = prSumLeft < num ? prSumLeft : num;
            auto prSize = size * num / FULL_PERCENT;
            lens.push_back(prSize);
            prSumLeft -= num;
            sizeLeft -= prSize;
        } else if (str.find(UNIT_RATIO) != std::string::npos) {
            lens.push_back(NearZero(frSum) ? 0.0 : frSizeSum / frSum * num);
        } else {
            lens.push_back(0.0);
        }
    }
    return lens;
}

void RenderGridLayout::ConvertRepeatArgs(std::string& handledArg)
{
    if (handledArg.find(REPEAT_PREFIX) == std::string::npos) {
        return;
    }
    handledArg.erase(0, handledArg.find_first_not_of(" ")); // trim the input str
    std::smatch matches;
    if (handledArg.find(UNIT_AUTO_FILL) != std::string::npos) {
        if (handledArg.size() > REPEAT_MIN_SIZE && std::regex_match(handledArg, matches, AUTO_REGEX)) {
            handledArg = matches[1].str() + matches[2].str();
        }
    } else {
        if (handledArg.size() > REPEAT_MIN_SIZE && std::regex_match(handledArg, matches, REPEAT_NUM_REGEX)) {
            auto count = StringUtils::StringToInt(matches[1].str());
            std::string repeatString = matches[2].str();
            while (count > 1) {
                repeatString.append(std::string(matches[2].str()));
                --count;
            }
            handledArg = repeatString;
        }
    }
}

std::vector<double> RenderGridLayout::ParseAutoFill(const std::vector<std::string>& strs, double size, double gap)
{
    std::vector<double> lens;
    if (strs.size() <= 1) {
        return lens;
    }
    auto allocatedSize = size - (strs.size() - 2) * gap; // size() - 2 means 'auto-fill' should be erased.
    double pxSum = 0.0;
    double peSum = 0.0;
    for (const auto& str : strs) {
        auto num = StringUtils::StringToDouble(str);
        if (str.find(UNIT_PIXEL) != std::string::npos) {
            num = pxSum > allocatedSize ? 0.0 : num;
            pxSum += num;
            lens.emplace_back(num);
        } else if (str.find(UNIT_PERCENT) != std::string::npos) {
            // adjust invalid percent
            num = peSum >= FULL_PERCENT ? 0.0 : num;
            peSum += num;
            pxSum += num / FULL_PERCENT * size;
            lens.emplace_back(num / FULL_PERCENT * size);
        } else {
            LOGD("Unsupported type: %{public}s, and use 0.0", str.c_str());
        }
    }
    allocatedSize -= pxSum;
    if (LessOrEqual(allocatedSize, 0.0)) {
        return lens;
    }
    pxSum += lens.size() * gap;
    int32_t repeatCount = allocatedSize / pxSum;
    std::vector<double> newLens;
    for (int32_t i = 0; i < repeatCount + 1; i++) {
        newLens.insert(newLens.end(), lens.begin(), lens.end());
    }
    allocatedSize -= pxSum * repeatCount;
    for (auto lenIter = lens.begin(); lenIter != lens.end(); lenIter++) {
        allocatedSize -= *lenIter + gap;
        if (LessNotEqual(allocatedSize, 0.0)) {
            break;
        }
        newLens.emplace_back(*lenIter);
    }
    return newLens;
}

void RenderGridLayout::SetItemIndex(const RefPtr<RenderNode>& child, int32_t index)
{
    int32_t depth = DEFAULT_DEPTH;
    auto item = child;
    auto gridLayoutItem = AceType::DynamicCast<RenderGridLayoutItem>(item);
    while (!gridLayoutItem && depth > 0) {
        if (!item || item->GetChildren().empty()) {
            return;
        }
        item = item->GetChildren().front();
        gridLayoutItem = AceType::DynamicCast<RenderGridLayoutItem>(item);
        --depth;
    }
    if (gridLayoutItem) {
        gridLayoutItem->SetIndex(index);
    }
}

int32_t RenderGridLayout::GetItemRowIndex(const RefPtr<RenderNode>& child) const
{
    int32_t depth = DEFAULT_DEPTH;
    int32_t rowIndex = -1;
    auto item = child;
    auto gridLayoutItem = AceType::DynamicCast<RenderGridLayoutItem>(item);
    while (!gridLayoutItem && depth > 0) {
        if (!item || item->GetChildren().empty()) {
            return rowIndex;
        }
        item = item->GetChildren().front();
        gridLayoutItem = AceType::DynamicCast<RenderGridLayoutItem>(item);
        --depth;
    }
    if (gridLayoutItem) {
        rowIndex = gridLayoutItem->GetRowIndex();
    }
    return rowIndex;
}

int32_t RenderGridLayout::GetItemColumnIndex(const RefPtr<RenderNode>& child) const
{
    int32_t depth = DEFAULT_DEPTH;
    int32_t columnIndex = -1;
    auto item = child;
    auto gridLayoutItem = AceType::DynamicCast<RenderGridLayoutItem>(item);
    while (!gridLayoutItem && depth > 0) {
        if (!item || item->GetChildren().empty()) {
            return columnIndex;
        }
        item = item->GetChildren().front();
        gridLayoutItem = AceType::DynamicCast<RenderGridLayoutItem>(item);
        --depth;
    }
    if (gridLayoutItem) {
        columnIndex = gridLayoutItem->GetColumnIndex();
    }
    return columnIndex;
}

int32_t RenderGridLayout::GetItemSpan(const RefPtr<RenderNode>& child, bool isRow) const
{
    int32_t depth = DEFAULT_DEPTH;
    int32_t span = -1;
    auto item = child;
    auto gridLayoutItem = AceType::DynamicCast<RenderGridLayoutItem>(item);
    while (!gridLayoutItem && depth > 0) {
        if (!item || item->GetChildren().empty()) {
            return span;
        }
        item = item->GetChildren().front();
        gridLayoutItem = AceType::DynamicCast<RenderGridLayoutItem>(item);
        --depth;
    }
    if (gridLayoutItem) {
        span = isRow ? gridLayoutItem->GetRowSpan() : gridLayoutItem->GetColumnSpan();
    }
    return span < 1 ? 1 : span;
}

void RenderGridLayout::GetNextGrid(int32_t& curRow, int32_t& curCol) const
{
    LOGD("%{public}s begin. curRow: %{public}d, curCol: %{public}d", __PRETTY_FUNCTION__, curRow, curCol);
    if (isVertical_) {
        ++curCol;
        if (curCol >= colCount_) {
            curCol = 0;
            ++curRow;
        }
    } else {
        ++curRow;
        if (curRow >= rowCount_) {
            curRow = 0;
            ++curCol;
        }
    }
}

void RenderGridLayout::GetPreviousGird(int32_t& curRow, int32_t& curCol) const
{
    LOGD("%{public}s begin. curRow: %{public}d, curCol: %{public}d", __PRETTY_FUNCTION__, curRow, curCol);
    if (isVertical_) {
        --curCol;
        if (curCol < 0) {
            curCol = colCount_ - 1;
            --curRow;
        }
    } else {
        --curRow;
        if (curRow < 0) {
            curRow = rowCount_ - 1;
            --curCol;
        }
    }
}

bool RenderGridLayout::CheckGridPlaced(int32_t index, int32_t row, int32_t col, int32_t& rowSpan, int32_t& colSpan)
{
    auto rowIter = gridMatrix_.find(row);
    if (rowIter != gridMatrix_.end()) {
        auto colIter = rowIter->second.find(col);
        if (colIter != rowIter->second.end()) {
            return false;
        }
    }
    rowSpan = std::min(rowCount_ - row, rowSpan);
    colSpan = std::min(colCount_ - col, colSpan);
    int32_t rSpan = 0;
    int32_t cSpan = 0;
    int32_t retColSpan = 1;
    while (rSpan < rowSpan) {
        rowIter = gridMatrix_.find(rSpan + row);
        if (rowIter != gridMatrix_.end()) {
            cSpan = 0;
            while (cSpan < colSpan) {
                if (rowIter->second.find(cSpan + col) != rowIter->second.end()) {
                    colSpan = cSpan;
                    break;
                }
                ++cSpan;
            }
        } else {
            cSpan = colSpan;
        }
        if (retColSpan > cSpan) {
            break;
        }
        retColSpan = cSpan;
        ++rSpan;
    }
    rowSpan = rSpan;
    colSpan = retColSpan;
    for (int32_t i = row; i < row + rowSpan; ++i) {
        std::map<int32_t, int32_t> rowMap;
        auto iter = gridMatrix_.find(i);
        if (iter != gridMatrix_.end()) {
            rowMap = iter->second;
        }
        for (int32_t j = col; j < col + colSpan; ++j) {
            rowMap.emplace(std::make_pair(j, index));
        }
        gridMatrix_[i] = rowMap;
    }
    LOGD("%{public}d %{public}d %{public}d %{public}d %{public}d", index, row, col, rowSpan, colSpan);
    return true;
}

void RenderGridLayout::PerformLayout()
{
    if (CheckAnimation()) {
        return;
    }
    if (isDragChangeLayout_ && !needRestoreScene_) {
        isDragChangeLayout_ = false;
        return;
    }
    needRestoreScene_ = false;
    isDragChangeLayout_ = false;
    gridMatrix_.clear();
    itemsInGrid_.clear();
    if (GetChildren().empty()) {
        return;
    }

    // register the item selected callback
    if (editMode_) {
        RegisterLongPressedForItems();
    }
    // Initialize the the grid layout prop
    if (isDynamicGrid_) {
        InitialDynamicGridProp();
    } else {
        InitialGridProp();
    }
    allocatedRowSizes_.clear();
    allocatedRowSizes_.insert(allocatedRowSizes_.begin(), rowCount_ + 1, 0.0);
    if (editMode_) {
        PerformLayoutForEditGrid();
        if (needResetItemPosition_) {
            ResetItemPosition();
            needResetItemPosition_ = false;
        }
    } else {
        PerformLayoutForStaticGrid();
    }
    if (CheckNeedShrink()) {
        SetLayoutSize(GetLayoutParam().Constrain(Size(colSize_,
            allocatedRowSizes_.back() + (rowCount_ - 1) * rowGap_)));
    } else {
        SetLayoutSize(GetLayoutParam().Constrain(Size(colSize_, rowSize_)));
    }
}

bool RenderGridLayout::IsUseOnly()
{
    return true;
}

bool RenderGridLayout::CouldBeInserted()
{
    int32_t itemCount = CountItemInGrid();
    if (itemCount >= itemCountMax_) {
        return false;
    }
    return true;
}

bool RenderGridLayout::NeedBeLarger()
{
    int32_t itemCount = CountItemInGrid();
    if (curItemCountMax_ >= (itemCount + 1)) {
        return false;
    }
    return true;
}

bool RenderGridLayout::NeedBeSmaller()
{
    int32_t itemCount = CountItemInGrid();
    int32_t crossItemCount = 0;
    if (isVertical_) {
        crossItemCount = colCount_;
    } else {
        crossItemCount = rowCount_;
    }
    if ((curItemCountMax_ - crossItemCount) < itemCount) {
        return false;
    }
    return true;
}

void RenderGridLayout::BackGridMatrix()
{
    gridMatrixBack_.clear();
    gridMatrixBack_ = gridMatrix_;
    if (supportAnimation_ || dragAnimation_) {
        gridItemPosition_.clear();
        std::map<int32_t, GridItemIndexPosition> backData;
        ParseRestoreScenePosition(gridMatrixBack_, backData);
        for (auto& iter : backData) {
            if (iter.first >= 0 && iter.first < (int32_t)itemsInGrid_.size()) {
                auto item = itemsInGrid_[iter.first];
                gridItemPosition_[iter.first] = Point(item->GetPosition().GetX(), item->GetPosition().GetY());
            }
        }
    }
}

void RenderGridLayout::RestoreScene(const ItemDragInfo& info)
{
    // Until the moving animation is done, only performlayout needs to be triggered here to retrieve the original data
    // for layout
    needRestoreScene_ = true;
    if (supportAnimation_ || dragAnimation_) {
        CalcRestoreScenePosition(info);
        if (needRunAnimation_.load()) {
            StartAnimationController(GridLayoutAnimationAct::ANIMATION_RESTORE_SCENE, nullptr);
        }
    }
    StartFlexController(startGlobalPoint_);
}

void RenderGridLayout::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (dragDropGesture_) {
        dragDropGesture_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(dragDropGesture_);
    }
    if (slideRecognizer_) {
        slideRecognizer_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(slideRecognizer_);
    }
}

void RenderGridLayout::ClearPartDragInfo()
{
    LOGD("%{public}s begin.", __PRETTY_FUNCTION__);
    curInsertRowIndex_ = -1;
    curInsertColumnIndex_ = -1;
    dragPosRowIndex_ = -1;
    dragPosColumnIndex_ = -1;
    dragPosChanged_ = false;
}

void RenderGridLayout::ClearAllDragInfo()
{
    curInsertRowIndex_ = -1;
    curInsertColumnIndex_ = -1;
    dragPosRowIndex_ = -1;
    dragPosColumnIndex_ = -1;
    dragingItemIndex_ = -1;
    dragPosChanged_ = false;
    itemLongPressed_ = false;
    itemDragEntered_ = false;
    itemDragStarted_ = false;
    isInMainGrid_ = false;
    isMainGrid_ = false;
    reEnter_ = false;
    isDragChangeLayout_ = false;
    dragingItemRenderNode_.Reset();
    subGrid_.Reset();
    mainGrid_.Reset();
}

void RenderGridLayout::CalIsVertical()
{
    if (isDynamicGrid_) {
        if (!colsArgs_.empty() && rowsArgs_.empty()) {
            isVertical_ = true;
        } else {
            isVertical_ = false;
        }
    } else {
        if (direction_ == FlexDirection::COLUMN) {
            isVertical_ = true;
        } else {
            isVertical_ = false;
        }
    }
}

void RenderGridLayout::RegisterLongPressedForItems()
{
    if (GetChildren().empty()) {
        LOGE("%{public}s. has no children", __PRETTY_FUNCTION__);
        return;
    }

    for (const auto& child : GetChildren()) {
        int32_t depth = DEFAULT_DEPTH;
        auto item = child;
        auto gridLayoutItem = AceType::DynamicCast<RenderGridLayoutItem>(item);
        while (!gridLayoutItem && depth > 0) {
            if (!item || item->GetChildren().empty()) {
                LOGE("%{public}s. item has no children anymore", __PRETTY_FUNCTION__);
                break;
            }
            item = item->GetChildren().front();
            gridLayoutItem = AceType::DynamicCast<RenderGridLayoutItem>(item);
            --depth;
        }
        if (gridLayoutItem) {
            gridLayoutItem->SetOnItemLongPressed(
                [weak = WeakClaim(this)](int32_t index, const WeakPtr<RenderNode>& itemRenderNode) {
                    auto render = weak.Upgrade();
                    if (!render) {
                        LOGE("%{public}s .renderGrid is null", __PRETTY_FUNCTION__);
                        return false;
                    }
                    auto item = itemRenderNode.Upgrade();
                    if ((index < 0) || (!item)) {
                        LOGE("%{public}s .index invalid or item is null", __PRETTY_FUNCTION__);
                        return false;
                    }

                    if (render->GetSlideDirect() != GridSlideDirect::SLIDE_NODE) {
                        LOGE("%{public}s .the grid is sliding now", __PRETTY_FUNCTION__);
                        return false;
                    }
                    render->dragingItemIndex_ = index;
                    render->dragingItemRenderNode_ = itemRenderNode;
                    render->itemLongPressed_ = true;
                    return true;
                });
        } else {
            LOGI("%{public}s begin child is not item.", __PRETTY_FUNCTION__);
        }
    }
}

void RenderGridLayout::CreateDragDropRecognizer()
{
    if (dragDropGesture_) {
        return;
    }
    auto longPress = AceType::MakeRefPtr<LongPressRecognizer>(GetContext(), DEFAULT_DURATION, DEFAULT_FINGERS, false);
    longPress->SetOnLongPress(
        [weak = WeakClaim<RenderGridLayout>(this), context = GetContext()](const LongPressInfo& info) {
            auto renderGrid = weak.Upgrade();
            if (renderGrid) {
                GestureEvent eventinfo;
                eventinfo.SetGlobalPoint(Point(info.GetGlobalLocation().GetX(), info.GetGlobalLocation().GetY()));
                auto targetItem = renderGrid->FindTargetRenderNode<RenderGridLayoutItem>(context.Upgrade(), eventinfo);
                if (targetItem) {
                    targetItem->OnLongPressEvent();
                    renderGrid->SetMainTargetRenderGrid(renderGrid);
                    renderGrid->SetPreTargetRenderGrid(renderGrid);
                    Point lastLongPressPoint;
                    lastLongPressPoint.SetX(eventinfo.GetGlobalPoint().GetX());
                    lastLongPressPoint.SetY(eventinfo.GetGlobalPoint().GetY());
                    renderGrid->SetLongPressPoint(lastLongPressPoint);
                    // update curInsertIndex
                    ItemDragInfo dragInfo = ItemDragInfo();
                    dragInfo.SetX(info.GetGlobalLocation().GetX());
                    dragInfo.SetY(info.GetGlobalLocation().GetY());
                    if (renderGrid->CalDragCell(dragInfo)) {
                        renderGrid->UpdateCurInsertPos(
                            renderGrid->GetDragPosRowIndex(), renderGrid->GetDragPosColumnIndex());
                    }
                }
            }
        });
    PanDirection panDirection;
    auto pan = AceType::MakeRefPtr<PanRecognizer>(GetContext(), DEFAULT_FINGERS, panDirection, DEFAULT_DISTANCE);
    pan->SetOnActionUpdate(std::bind(&RenderGridLayout::PanOnActionUpdate, this, std::placeholders::_1));
    pan->SetOnActionEnd(std::bind(&RenderGridLayout::PanOnActionEnd, this, std::placeholders::_1));
    pan->SetOnActionCancel([weak = WeakClaim<RenderGridLayout>(this)]() {
        auto renderGrid = weak.Upgrade();
        if (renderGrid) {
            renderGrid->SetPreTargetRenderGrid(nullptr);
            renderGrid->SetMainTargetRenderGrid(nullptr);
        }
    });

    std::vector<RefPtr<GestureRecognizer>> recognizers { longPress, pan };
    dragDropGesture_ = AceType::MakeRefPtr<OHOS::Ace::SequencedRecognizer>(GetContext(), recognizers);
}

void RenderGridLayout::ActionStart(const ItemDragInfo& info, RefPtr<Component> customComponent)
{
    auto pipelineContext = GetContext().Upgrade();
    if (pipelineContext) {
        auto stackElement = pipelineContext->GetLastStack();
        auto positionedComponent = AceType::MakeRefPtr<PositionedComponent>(customComponent);
        positionedComponent->SetTop(Dimension(info.GetY()));
        positionedComponent->SetLeft(Dimension(info.GetX()));

        auto updatePosition = [renderGirdLayout = AceType::Claim(this)](const OnItemDragFunc& func) {
            if (!renderGirdLayout) {
                return;
            }
            renderGirdLayout->SetUpdatePositionId(func);
        };

        positionedComponent->SetUpdatePositionFuncId(updatePosition);
        stackElement->PushComponent(positionedComponent);
        stackElement->PerformBuild();
    }
}

void RenderGridLayout::PanOnActionUpdate(const GestureEvent& info)
{
    auto renderGirdLayout = AceType::Claim(this);
    if (!renderGirdLayout) {
        return;
    }
    if (renderGirdLayout->GetUpdatePositionId() && isExistComponent_) {
        Point point = info.GetGlobalPoint();
        renderGirdLayout->GetUpdatePositionId()(Dimension(point.GetX()), Dimension(point.GetY()));
    }
    ItemDragInfo event;
    // MMIO could not provide correct point info when touch up, so keep the last point info
    lastGlobalPoint_.SetX(info.GetGlobalPoint().GetX());
    lastGlobalPoint_.SetY(info.GetGlobalPoint().GetY());
    event.SetX(info.GetGlobalPoint().GetX());
    event.SetY(info.GetGlobalPoint().GetY());
    auto targetRenderGrid = FindTargetRenderNode<RenderGridLayout>(GetContext().Upgrade(), info);
    auto preTargetRenderGrid = GetPreTargetRenderGrid();
    auto mainTargetRenderGrid = GetMainTargetRenderGrid();

    if (!mainTargetRenderGrid) {
        return;
    }

    if (targetRenderGrid) {
        if (preTargetRenderGrid == targetRenderGrid) {
            mainTargetRenderGrid->OnDragMove(event);
            return;
        }
    }
    if (preTargetRenderGrid) {
        if (itemLongPressed_) {
            ItemDragInfo eventLongPress;
            eventLongPress.SetX(lastLongPressPoint_.GetX());
            eventLongPress.SetY(lastLongPressPoint_.GetY());
            mainTargetRenderGrid->OnDragMove(eventLongPress);
        }
        preTargetRenderGrid->OnDragLeave(event);
        subGrid_.Reset();
        mainGrid_.Reset();
    } else if (targetRenderGrid && !itemLongPressed_) {
        targetRenderGrid->mainGrid_ = mainTargetRenderGrid;
        subGrid_ = AceType::WeakClaim(AceType::RawPtr(targetRenderGrid));
        targetRenderGrid->OnDragEnter(event);
    } else {
        // drag out of all gridLayout
        mainTargetRenderGrid->OnDragMove(event);
    }
    SetPreTargetRenderGrid(targetRenderGrid);
}

void RenderGridLayout::PanOnActionEnd(const GestureEvent& info)
{
    if (!(supportAnimation_ || dragAnimation_)) {
        CloseFlexComponent();
    }
    ItemDragInfo event;
    // MMIO could not provide correct point info when touch up, so restore the last point info
    event.SetX(lastGlobalPoint_.GetX());
    event.SetY(lastGlobalPoint_.GetY());
    auto targetRenderGrid = GetPreTargetRenderGrid();
    auto mainTargetRenderGrid = GetMainTargetRenderGrid();

    if (targetRenderGrid) {
        if (mainTargetRenderGrid) {
            mainTargetRenderGrid->OnDrop(event);
            SetPreTargetRenderGrid(nullptr);
            SetMainTargetRenderGrid(nullptr);
        }
    } else {
        // drag out of all gridLayout
        if (mainTargetRenderGrid) {
            mainTargetRenderGrid->OnDrop(event);
        }
    }
}

void RenderGridLayout::OnDragEnter(const ItemDragInfo& info)
{
    LOGD("%{public}s begin. itemDragStarted_ %{public}d, itemDragEntered_ %{public}d, itemLongPressed_ %{public}d, "
         "isInMainGrid_ %{public}d, isMainGrid_ %{public}d",
         __PRETTY_FUNCTION__, itemDragStarted_, itemDragEntered_, itemLongPressed_, isInMainGrid_, isMainGrid_);
    if (!editMode_) {
        LOGW("%{public}s no editMode no dragevent.", __PRETTY_FUNCTION__);
        return;
    }

    if (component_->GetOnGridDragEnterId()) {
        component_->GetOnGridDragEnterId()(info);
    }
    if (isMainGrid_) {
        if (itemDragStarted_) {
            ImpDragEnterMainGrid(info);
        }
    } else {
        ImpDragEnterSubGrid(info);
    }
}

void RenderGridLayout::OnDragLeave(const ItemDragInfo& info)
{
    LOGD("%{public}s begin. itemDragStarted_ %{public}d, itemDragEntered_ %{public}d, itemLongPressed_ %{public}d, "
         "isInMainGrid_ %{public}d, isMainGrid_ %{public}d",
         __PRETTY_FUNCTION__, itemDragStarted_, itemDragEntered_, itemLongPressed_, isInMainGrid_, isMainGrid_);
    if (!editMode_) {
        LOGW("%{public}s no editMode no dragevent.", __PRETTY_FUNCTION__);
        return;
    }

    if (isMainGrid_) {
        if (itemDragStarted_) {
            ImpDragLeaveMainGrid(info);
        }
    } else {
        if (itemDragEntered_) {
            ImpDragLeaveSubGrid(info);
        }
    }
}

void RenderGridLayout::OnDragMove(const ItemDragInfo& info)
{
    LOGD("%{public}s begin. itemDragStarted_ %{public}d, itemDragEntered_ %{public}d, itemLongPressed_ %{public}d, "
         "isInMainGrid_ %{public}d, isMainGrid_ %{public}d",
         __PRETTY_FUNCTION__, itemDragStarted_, itemDragEntered_, itemLongPressed_, isInMainGrid_, isMainGrid_);
    if (!editMode_) {
        LOGW("%{public}s no editMode no dragevent.", __PRETTY_FUNCTION__);
        return;
    }

    if (!itemDragEntered_ && itemLongPressed_ && !itemDragStarted_) {
        ImpDragStart(info);
    }

    if ((!isInMainGrid_ && itemDragEntered_) || (itemDragStarted_ && isInMainGrid_ && isMainGrid_)) {
        ImpDragMove(info);
        return;
    }
    if (isMainGrid_) {
        auto subGrid = subGrid_.Upgrade();
        if (subGrid && !isInMainGrid_) {
            subGrid->OnDragMove(info);
            if (component_->GetOnGridDragMoveId()) {
                component_->GetOnGridDragMoveId()(info, dragingItemIndex_, -1);
            }
        } else if (!isInMainGrid_) {
            if (component_->GetOnGridDragMoveId()) {
                component_->GetOnGridDragMoveId()(info, dragingItemIndex_, -1);
            }
        }
    }
}

bool RenderGridLayout::ImpDropInGrid(const ItemDragInfo& info)
{
    itemDragStarted_ = false;
    itemDragEntered_ = false;
    bool result = false;
    int32_t insertIndex = -1;
    gridMatrixBack_.clear();
    if (CouldBeInserted()) {
        if (CalDragCell(info)) {
            MoveItems();
        }
        insertIndex = CalIndexForItemByRowAndColum(curInsertRowIndex_, curInsertColumnIndex_);
        if (insertIndex >= 0 && insertIndex < itemCountMax_) {
            result = true;
            Point endPoint = CalcDragChildEndPosition(curInsertRowIndex_, curInsertColumnIndex_);
            RegisterDropJSEvent(info, insertIndex, result);

            if (needRunAnimation_.load()) {
                StartAnimationController(GridLayoutAnimationAct::ANIMATION_DRAG_DROP, nullptr);
            }

            if (isMainGrid_ && isInMainGrid_) {
                StartFlexController(endPoint);
            } else {
                auto mainGrid = mainGrid_.Upgrade();
                if (mainGrid) {
                    mainGrid->RegisterDropJSEvent(info, -1, result);
                    mainGrid->StartFlexController(endPoint, true);
                }
            }
        } else {
            RegisterDropJSEvent(info, -1, false);
            RestoreScene(info);
            result = true;
        }
    } else {
        RegisterDropJSEvent(info, -1, false);
        RestoreScene(info);
    }
    return result;
}

void RenderGridLayout::ImpDragMove(const ItemDragInfo& info)
{
    LOGD("%{public}s begin.", __PRETTY_FUNCTION__);
    if (CouldBeInserted() || itemDragStarted_) {
        LOGD("%{public}s couldn be inserted.", __PRETTY_FUNCTION__);
        if (CalDragCell(info)) {
            MoveItems();
            if (needRunAnimation_.load()) {
                StartAnimationController(GridLayoutAnimationAct::ANIMATION_DRAG_MOVE, nullptr);
            }
        }
        if (supportAnimation_ || dragAnimation_) {
            TriggerMoveEventForJS(info);
        } else {
            isDragChangeLayout_ = true;
            TriggerMoveEventForJS(info);
            MarkNeedLayout();
        }
    } else {
        LOGD("%{public}s couldn't be inserted.", __PRETTY_FUNCTION__);
        if (component_->GetOnGridDragMoveId()) {
            LOGD("%{public}s. could not be insert. InsertIndex: %{public}d ,ItemIndex: %{public}d", __PRETTY_FUNCTION__,
                -1, dragingItemIndex_);
            component_->GetOnGridDragMoveId()(info, dragingItemIndex_, -1);
        } else {
            LOGE("%{public}s no onGridDragMove registered.", __PRETTY_FUNCTION__);
        }
    }
}

void RenderGridLayout::ImpDragLeaveMainGrid(const ItemDragInfo& info)
{
    LOGD("%{public}s begin.", __PRETTY_FUNCTION__);
    isInMainGrid_ = false;
    if (component_->GetOnGridDragLeaveId()) {
        LOGD("%{public}s. from dragstart ItemIndex: %{public}d", __PRETTY_FUNCTION__, dragingItemIndex_);
        component_->GetOnGridDragLeaveId()(info, dragingItemIndex_);
    } else {
        LOGE("%{public}s no onGridDragLeave registered.", __PRETTY_FUNCTION__);
    }
    FakeRemoveDragItem();
    ClearPartDragInfo();
    if ((supportAnimation_ || dragAnimation_) && needRunAnimation_.load()) {
        StartAnimationController(GridLayoutAnimationAct::ANIMATION_DRAG_MOVE, [weak = WeakClaim(this)]() {
            auto renderGrid = weak.Upgrade();
            if (renderGrid) {
                renderGrid->FakeRemoveDragItemUpdate();
            }
        });
    } else {
        FakeRemoveDragItemUpdate();
        isDragChangeLayout_ = true;
        MarkNeedLayout();
    }
}

void RenderGridLayout::ImpDragLeaveSubGrid(const ItemDragInfo& info)
{
    LOGD("%{public}s begin.", __PRETTY_FUNCTION__);
    ClearAllDragInfo();
    if (component_->GetOnGridDragLeaveId()) {
        LOGD("%{public}s. from dragenter ItemIndex: %{public}d", __PRETTY_FUNCTION__, -1);
        component_->GetOnGridDragLeaveId()(info, -1);
    } else {
        LOGE("%{public}s no onGridDragLeave registered.", __PRETTY_FUNCTION__);
    }
    if (isDynamicGrid_ && NeedBeSmaller()) {
        // BeSmaller
        InitialDynamicGridProp(DRAG_LEAVE);
        if (rightToLeft_) {
            needResetItemPosition_ = true;
        }
    }
    RestoreScene(info);
    isDragChangeLayout_ = true;
    MarkNeedLayout();
}

void RenderGridLayout::ImpDragEnterMainGrid(const ItemDragInfo& info)
{
    LOGD("%{public}s begin.", __PRETTY_FUNCTION__);
    LOGD("%{public}s drag reEnter_ the maingrid.", __PRETTY_FUNCTION__);
    isInMainGrid_ = true;
    reEnter_ = true;
    if ((supportAnimation_ || dragAnimation_) && needRunAnimation_.load()) {
        StartAnimationController(GridLayoutAnimationAct::ANIMATION_DRAG_MOVE, [weak = WeakClaim(this)]() {
            auto renderGrid = weak.Upgrade();
            if (renderGrid) {
                renderGrid->ImpDragEnterMainGridUpdate();
            }
        });
    } else {
        ImpDragEnterMainGridUpdate();
    }
}

void RenderGridLayout::ImpDragEnterMainGridUpdate()
{
    if (isDynamicGrid_ && NeedBeLarger()) {
        // BeLager and render
        InitialDynamicGridProp(DRAG_ENTER);
        SetLayoutSize(GetLayoutParam().Constrain(Size(colSize_, rowSize_)));
        isDragChangeLayout_ = true;
        if (rightToLeft_) {
            ResetItemPosition();
        }
        MarkNeedLayout();
    }
}

void RenderGridLayout::ImpDragEnterSubGrid(const ItemDragInfo& info)
{
    LOGD("%{public}s begin.", __PRETTY_FUNCTION__);
    itemDragEntered_ = true;
    if (CouldBeInserted()) {
        LOGD("%{public}s could be inserted.", __PRETTY_FUNCTION__);
        BackGridMatrix();
        if (isDynamicGrid_ && NeedBeLarger()) {
            // BeLager and render
            InitialDynamicGridProp(DRAG_ENTER);
            SetLayoutSize(GetLayoutParam().Constrain(Size(colSize_, rowSize_)));
            isDragChangeLayout_ = true;
            if (rightToLeft_) {
                ResetItemPosition();
            }
            MarkNeedLayout();
        }
    } else {
        LOGD("%{public}s couldn't be inserted.", __PRETTY_FUNCTION__);
    }
}

bool RenderGridLayout::OnDrop(const ItemDragInfo& info)
{
    LOGD("%{public}s begin. itemDragStarted_ %{public}d, itemDragEntered_ %{public}d, itemLongPressed_ %{public}d, "
         "isInMainGrid_ %{public}d, isMainGrid_ %{public}d",
         __PRETTY_FUNCTION__, itemDragStarted_, itemDragEntered_, itemLongPressed_, isInMainGrid_, isMainGrid_);
    if (!editMode_) {
        LOGW("%{public}s no editMode no dragevent.", __PRETTY_FUNCTION__);
        return false;
    }

    if ((isMainGrid_ && isInMainGrid_ && itemDragStarted_) || (!isMainGrid_ && itemDragEntered_)) {
        bool ret = ImpDropInGrid(info);
        if (!(supportAnimation_ || dragAnimation_)) {
            ClearAllDragInfo();
        }
        return ret;
    }

    if (isMainGrid_) {
        auto subGrid = subGrid_.Upgrade();
        if (subGrid && !isInMainGrid_) {
            bool result = subGrid->OnDrop(info);
            if (!result) {
                RestoreScene(info);
                MarkNeedLayout();
            }
            return result;
        } else if (!isInMainGrid_) {
            RegisterDropJSEvent(info, -1, false);
            RestoreScene(info);
            isDragChangeLayout_ = true;
            return false;
        }
    }
    ClearAllDragInfo();
    return false;
}

void RenderGridLayout::ImpDragStart(const ItemDragInfo& info)
{
    itemDragStarted_ = true;
    itemLongPressed_ = false;
    isMainGrid_ = true;
    isInMainGrid_ = true;
    ClearSpringSlideData();
    BackGridMatrix();
    isDragging_.store(true);
    auto itemRender = dragingItemRenderNode_.Upgrade();
    if (itemRender) {
        startGlobalPoint_.SetX(itemRender->GetGlobalOffset().GetX());
        startGlobalPoint_.SetY(itemRender->GetGlobalOffset().GetY());
        itemRender->SetVisible(false);
        DisableChild(itemRender, dragingItemIndex_);
    }
    if (component_->GetOnGridDragStartId()) {
        auto customComponent = component_->GetOnGridDragStartId()(info, dragingItemIndex_);
        if (customComponent) {
            isExistComponent_ = true;
            ActionStart(info, customComponent);
        }
    } else {
        LOGE("%{public}s no onGridDragStart registered.", __PRETTY_FUNCTION__);
    }
}

void RenderGridLayout::OnCallSubDragEnter(const ItemDragInfo& info)
{
    auto subGrid = subGrid_.Upgrade();
    if (subGrid) {
        subGrid->OnDragEnter(info);
    }
}

void RenderGridLayout::OnCallSubDragLeave(const ItemDragInfo& info)
{
    auto subGrid = subGrid_.Upgrade();
    if (subGrid) {
        subGrid->OnDragLeave(info);
    }
}

int32_t RenderGridLayout::CountItemInGrid()
{
    int32_t count = 0;
    for (int main = 0; main < rowCount_; main++) {
        auto mainIter = gridMatrix_.find(main);
        if (mainIter != gridMatrix_.end()) {
            count += CountItemInRow(mainIter);
        }
    }
    return count;
}

int32_t RenderGridLayout::CountItemInRow(const std::map<int32_t, std::map<int32_t, int32_t>>::iterator& rowGrid)
{
    LOGD("%{public}s begin.", __PRETTY_FUNCTION__);
    int32_t count = 0;
    for (int cross = 0; cross < colCount_; cross++) {
        auto crossIter = rowGrid->second.find(cross);
        if (crossIter != rowGrid->second.end()) {
            int32_t index = crossIter->second;
            if (index >= 0) {
                count++;
            }
        }
    }
    LOGD("RenderGridLayout CountItemInRow the count is %{public}d ", count);
    return count;
}

void RenderGridLayout::ResetItemPosition()
{
    LOGD("%{public}s begin.", __PRETTY_FUNCTION__);
    for (const auto& gridMap : gridMatrix_) {
        int32_t row = gridMap.first;
        for (const auto& grid : gridMap.second) {
            int32_t col = grid.first;
            int32_t index = grid.second;
            if (index >= 0 && index < (int32_t)itemsInGrid_.size()) {
                SetChildPosition(itemsInGrid_[index], row, col, 1, 1);
            }
        }
    }
}

void RenderGridLayout::InitialDynamicGridProp(int32_t dragLeaveOrEnter)
{
    rowGap_ = NormalizePercentToPx(userRowGap_, true);
    colGap_ = NormalizePercentToPx(userColGap_, false);

    SetGridLayoutParam();

    std::vector<double> cols;
    std::vector<double> rows;
    if (isVertical_) {
        CalculateVerticalSize(cols, rows, dragLeaveOrEnter);
        itemCountMax_ = colCount_ * mainCountMax_;
    } else {
        CalculateHorizontalSize(cols, rows, dragLeaveOrEnter);
        itemCountMax_ = rowCount_ * mainCountMax_;
    }
    curItemCountMax_ = colCount_ * rowCount_;

    UpdateCollectionInfo(cols, rows);
    LOGD("GridLayout: %{public}lf %{public}lf %{public}d %{public}d", colSize_, rowSize_, colCount_, rowCount_);
}

void RenderGridLayout::SetGridLayoutParam()
{
    LayoutParam gridLayoutParam = GetLayoutParam();
    auto maxHeight = isVertical_ ? mainCountMax_ * cellLength_ + (mainCountMax_ - 1) * rowGap_
                                 : GetLayoutParam().GetMaxSize().Height();
    auto maxWidth = isVertical_ ? GetLayoutParam().GetMaxSize().Width()
                                : mainCountMax_ * cellLength_ + (mainCountMax_ - 1) * colGap_;
    auto minHeight = isVertical_ ? mainCountMin_ * cellLength_ + (mainCountMin_ - 1) * rowGap_
                                 : GetLayoutParam().GetMaxSize().Height();
    auto minWidth = isVertical_ ? GetLayoutParam().GetMaxSize().Width()
                                : mainCountMin_ * cellLength_ + (mainCountMin_ - 1) * colGap_;
    gridLayoutParam.SetMaxSize(Size(maxWidth, maxHeight));
    gridLayoutParam.SetMinSize(Size(minWidth, minHeight));
    SetLayoutParam(gridLayoutParam);
}

void RenderGridLayout::CalculateVerticalSize(
    std::vector<double>& cols, std::vector<double>& rows, int32_t dragLeaveOrEnter)
{
    colSize_ = ((gridWidth_ > 0.0) && (gridWidth_ < GetLayoutParam().GetMaxSize().Width()))
                   ? gridWidth_
                   : GetLayoutParam().GetMaxSize().Width();
    if (NearEqual(colSize_, Size::INFINITE_SIZE) &&
        (colsArgs_.find(UNIT_PERCENT) != std::string::npos || colsArgs_.find(UNIT_RATIO) != std::string::npos)) {
        colSize_ = viewPort_.Width();
    }
    // Get item width
    cols = ParseArgs(PreParseCols(), colSize_, colGap_);
    if (cols.empty()) {
        cols.push_back(colSize_);
    }
    // Get the number of items in each row
    colCount_ = cols.size();
    // Get the number of items
    auto totalNum = (int32_t)GetChildren().size() + dragLeaveOrEnter;
    // Count the number of rows
    rowCount_ =
        std::clamp((totalNum / colCount_ + (((totalNum % colCount_) == 0) ? 0 : 1)), mainCountMin_, mainCountMax_);
    rows = std::vector<double>(rowCount_, cellLength_);
    rowSize_ = rowCount_ * cellLength_ + (rowCount_ - 1) * rowGap_;
}

void RenderGridLayout::CalculateHorizontalSize(
    std::vector<double>& cols, std::vector<double>& rows, int32_t dragLeaveOrEnter)
{
    rowSize_ = ((gridHeight_ > 0.0) && (gridHeight_ < GetLayoutParam().GetMaxSize().Height()))
                   ? gridHeight_
                   : GetLayoutParam().GetMaxSize().Height();
    if (NearEqual(rowSize_, Size::INFINITE_SIZE) &&
        (rowsArgs_.find(UNIT_PERCENT) != std::string::npos || rowsArgs_.find(UNIT_RATIO) != std::string::npos)) {
        rowSize_ = viewPort_.Height();
    }
    // Get item width
    rows = ParseArgs(PreParseRows(), rowSize_, rowGap_);
    if (rows.empty()) {
        rows.push_back(rowSize_);
    }
    // Get the number of items in each col
    rowCount_ = rows.size();
    // Get the number of items
    auto totalNum = (int32_t)GetChildren().size() + dragLeaveOrEnter;
    // Count the number of cols
    colCount_ =
        std::clamp((totalNum / rowCount_ + (((totalNum % rowCount_) == 0) ? 0 : 1)), mainCountMin_, mainCountMax_);
    cols = std::vector<double>(colCount_, cellLength_);
    colSize_ = colCount_ * cellLength_ + (colCount_ - 1) * colGap_;
}

void RenderGridLayout::UpdateCollectionInfo(std::vector<double> cols, std::vector<double> rows)
{
    gridCells_.clear();
    int32_t row = 0;
    for (auto height : rows) {
        int32_t col = 0;
        for (auto width : cols) {
            gridCells_[row][col] = Size(width, height);
            ++col;
        }
        ++row;
    }
    UpdateAccessibilityAttr();
}

void RenderGridLayout::PerformLayoutForEditGrid()
{
    int32_t itemIndex = 0;
    int32_t rowIndex = 0;
    int32_t colIndex = 0;
    itemsInGrid_.clear();
    for (const auto& item : GetChildren()) {
        int32_t itemRowSpan = 1;
        int32_t itemColSpan = 1;
        if (rowIndex >= 0 && rowIndex < rowCount_ && colIndex >= 0 && colIndex < colCount_) {
            while (!CheckGridPlaced(itemIndex, rowIndex, colIndex, itemRowSpan, itemColSpan)) {
                GetNextGrid(rowIndex, colIndex);
                if (rowIndex >= rowCount_ || colIndex >= colCount_) {
                    break;
                }
            }
            if (rowIndex >= rowCount_ || colIndex >= colCount_) {
                DisableChild(item, itemIndex);
                continue;
            }
            item->Layout(MakeInnerLayoutParam(rowIndex, colIndex, itemRowSpan, itemColSpan));
            SetChildPosition(item, rowIndex, colIndex, itemRowSpan, itemColSpan);
            itemsInGrid_.push_back(item);
            RefreshAllocatedRowSizes(rowIndex, itemRowSpan, item);
            SetItemIndex(item, itemIndex); // Set index for focus adjust.
            ++itemIndex;
            LOGD("%{public}d %{public}d %{public}d %{public}d", rowIndex, colIndex, itemRowSpan, itemColSpan);
        }
    }
    auto hiddenItem = dragingItemRenderNode_.Upgrade();
    if (hiddenItem) {
        hiddenItem->SetVisible(false);
        DisableChild(hiddenItem, dragingItemIndex_);
    }
}

void RenderGridLayout::PerformLayoutForStaticGrid()
{
    LOGD("%{public}s begin.", __PRETTY_FUNCTION__);
    int32_t rowIndex = 0;
    int32_t colIndex = 0;
    int32_t itemIndex = 0;
    for (const auto& item : GetChildren()) {
        int32_t itemRow = GetItemRowIndex(item);
        int32_t itemCol = GetItemColumnIndex(item);
        int32_t itemRowSpan = GetItemSpan(item, true);
        int32_t itemColSpan = GetItemSpan(item, false);
        if (itemRow >= 0 && itemRow < rowCount_ && itemCol >= 0 && itemCol < colCount_ &&
            CheckGridPlaced(itemIndex, itemRow, itemCol, itemRowSpan, itemColSpan)) {
            item->Layout(MakeInnerLayoutParam(itemRow, itemCol, itemRowSpan, itemColSpan));
            SetChildPosition(item, itemRow, itemCol, itemRowSpan, itemColSpan);
        } else {
            while (!CheckGridPlaced(itemIndex, rowIndex, colIndex, itemRowSpan, itemColSpan)) {
                GetNextGrid(rowIndex, colIndex);
                if (rowIndex >= rowCount_ || colIndex >= colCount_) {
                    break;
                }
            }
            if (rowIndex >= rowCount_ || colIndex >= colCount_) {
                DisableChild(item, itemIndex);
                continue;
            }
            item->Layout(MakeInnerLayoutParam(rowIndex, colIndex, itemRowSpan, itemColSpan));
            SetChildPosition(item, rowIndex, colIndex, itemRowSpan, itemColSpan);
        }
        RefreshAllocatedRowSizes(rowIndex, itemRowSpan, item);
        SetItemIndex(item, itemIndex); // Set index for focus adjust.
        ++itemIndex;
        LOGD("%{public}d %{public}d %{public}d %{public}d", rowIndex, colIndex, itemRowSpan, itemColSpan);
    }
}

bool RenderGridLayout::CalDragCell(const ItemDragInfo& info)
{
    double gridPositionX = GetGlobalOffset().GetX();
    double gridPositionY = GetGlobalOffset().GetY();
    double dragRelativelyX = info.GetX() - gridPositionX;
    double dragRelativelyY = info.GetY() - gridPositionY;
    if (rightToLeft_) {
        dragRelativelyX = colSize_ - dragRelativelyX;
    }

    int32_t tmpDragPosRowIndex = -1;
    int32_t tmpDragPosColumIndex = -1;
    if (!CalDragRowIndex(dragRelativelyY, tmpDragPosRowIndex)) {
        return false;
    }

    if (!CalDragColumIndex(dragRelativelyX, tmpDragPosColumIndex)) {
        return false;
    }

    if ((dragPosRowIndex_ != tmpDragPosRowIndex) || (dragPosColumnIndex_ != tmpDragPosColumIndex)) {
        dragPosRowIndex_ = tmpDragPosRowIndex;
        dragPosColumnIndex_ = tmpDragPosColumIndex;
        dragPosChanged_ = true;
    }
    return true;
}

bool RenderGridLayout::CalDragRowIndex(double dragRelativelyY, int32_t& dragRowIndex)
{
    double rowStart = 0.0;
    double rowEnd = 0.0;
    for (int row = 0; row < rowCount_; row++) {
        rowStart = rowEnd;
        double offsetY = 0.0;
        if (row > 0 && row < (rowCount_ - 1)) {
            offsetY = rowGap_ / GAP_DIVIDE_CONSTEXPR;
        } else {
            offsetY = rowGap_;
        }
        rowEnd += gridCells_.at(row).at(0).Height() + offsetY;
        if (dragRelativelyY >= rowStart && dragRelativelyY <= rowEnd) {
            dragRowIndex = row;
            return true;
        }
    }
    return false;
}

bool RenderGridLayout::CalDragColumIndex(double dragRelativelyX, int32_t& dragColIndex)
{
    double columStart = 0.0;
    double columEnd = 0.0;
    for (int col = 0; col < colCount_; col++) {
        columStart = columEnd;
        double offsetX = 0.0;
        if (col > 0 && col < (colCount_ - 1)) {
            offsetX = colGap_ / GAP_DIVIDE_CONSTEXPR;
        } else {
            offsetX = colGap_;
        }
        columEnd += gridCells_.at(0).at(col).Width() + offsetX;
        if (dragRelativelyX >= columStart && dragRelativelyX <= columEnd) {
            dragColIndex = col;
            return true;
        }
    }
    return false;
}

void RenderGridLayout::MoveItems()
{
    if (!dragPosChanged_) {
        LOGI("%{public}s dragPos not change no need to move.", __PRETTY_FUNCTION__);
        return;
    }
    if (curInsertRowIndex_ >= 0 && curInsertColumnIndex_ >= 0) {
        // If there has been insert Cell now.
        MoveWhenWithInsertCell();
    } else {
        MoveWhenNoInsertCell();
    }
    dragPosChanged_ = false;
}

void RenderGridLayout::MoveWhenNoInsertCell()
{
    int32_t dragposIndex = GetIndexByGrid(dragPosRowIndex_, dragPosColumnIndex_);
    if (dragposIndex == -1) {
        // If there is no item in the cell
        MoveWhenNoInsertCellAndNoItemInDragCell();
    } else {
        MoveWhenNoInsertCellButWithItemInDragCell();
    }
}

void RenderGridLayout::MoveWhenNoInsertCellAndNoItemInDragCell()
{
    int32_t FirstEmptyCellRowIndex = -1;
    int32_t FirstEmptyCellColumIndex = -1;
    if (CalTheFirstEmptyCell(FirstEmptyCellRowIndex, FirstEmptyCellColumIndex, true)) {
        UpdateCurInsertPos(FirstEmptyCellRowIndex, FirstEmptyCellColumIndex);
    }
}

void RenderGridLayout::MoveWhenNoInsertCellButWithItemInDragCell()
{
    if (itemDragEntered_ || (itemDragStarted_ && reEnter_)) {
        MoveWhenNoInsertCellButWithItemInDragCellAndDragEnter();
    } else if (itemDragStarted_ && !reEnter_) {
        MoveWhenNoInsertCellButWithItemInDragCellAndDragStart();
    }
}

void RenderGridLayout::MoveWhenNoInsertCellButWithItemInDragCellAndDragEnter()
{
    int32_t endRow = -1;
    int32_t endColum = -1;
    if (CalTheFirstEmptyCell(endRow, endColum, true)) {
        GetPreviousGird(endRow, endColum);
        if (MoveItemsForward(dragPosRowIndex_, dragPosColumnIndex_, endRow, endColum)) {
            if (supportAnimation_ || dragAnimation_) {
                std::string key(__FUNCTION__);
                RegisterAnimationFinishedFunc(
                    key, [weak = WeakClaim(this), rowIndex = dragPosRowIndex_, colIndex = dragPosColumnIndex_]() {});
                UpdateCurInsertPos(dragPosRowIndex_, dragPosColumnIndex_);
                PrepareAnimationController(key);
            } else {
                UpdateCurInsertPos(dragPosRowIndex_, dragPosColumnIndex_);
            }
        }
    }
}

void RenderGridLayout::MoveWhenNoInsertCellButWithItemInDragCellAndDragStart()
{
    UpdateCurInsertPos(dragPosRowIndex_, dragPosColumnIndex_);
}

void RenderGridLayout::MoveWhenWithInsertCell()
{
    int32_t dragposIndex = GetIndexByGrid(dragPosRowIndex_, dragPosColumnIndex_);
    if (dragposIndex == -1) {
        // If there is no item in the cell
        MoveWhenWithInsertCellAndNoItemInDragCell();
    } else {
        MoveWhenWithInsertCellButWithItemInDragCell();
    }
}

void RenderGridLayout::MoveWhenWithInsertCellAndNoItemInDragCell()
{
    int32_t endRow = -1;
    int32_t endColum = -1;
    if (CalTheFirstEmptyCell(endRow, endColum, true)) {
        GetPreviousGird(endRow, endColum);
        int32_t startRow = curInsertRowIndex_;
        int32_t startColum = curInsertColumnIndex_;
        GetNextGrid(startRow, startColum);
        if (MoveItemsBackward(startRow, startColum, endRow, endColum)) {
            if (supportAnimation_ || dragAnimation_) {
                std::string key(__FUNCTION__);
                RegisterAnimationFinishedFunc(
                    key, [weak = WeakClaim(this), rowIndex = endRow, colIndex = endColum]() {});
                UpdateCurInsertPos(endRow, endColum);
                PrepareAnimationController(key);
            } else {
                UpdateCurInsertPos(endRow, endColum);
            }
        }
    }
}

void RenderGridLayout::MoveWhenWithInsertCellButWithItemInDragCell()
{
    bool dragIsBefore = false;
    bool dragIsCur =
        SortCellIndex(dragPosRowIndex_, dragPosColumnIndex_, curInsertRowIndex_, curInsertColumnIndex_, dragIsBefore);
    if (!dragIsCur && dragIsBefore) {
        MoveWhenWithInsertCellButWithItemInDragCellDragBeforeInsert();
    } else if (!dragIsCur && !dragIsBefore) {
        MoveWhenWithInsertCellButWithItemInDragCellDragAfterInsert();
    }
}

void RenderGridLayout::MoveWhenWithInsertCellButWithItemInDragCellDragBeforeInsert()
{
    int32_t endRow = curInsertRowIndex_;
    int32_t endColum = curInsertColumnIndex_;
    GetPreviousGird(endRow, endColum);
    if (MoveItemsForward(dragPosRowIndex_, dragPosColumnIndex_, endRow, endColum)) {
        if (supportAnimation_ || dragAnimation_) {
            std::string key(__FUNCTION__);
            RegisterAnimationFinishedFunc(
                key, [weak = WeakClaim(this), rowIndex = dragPosRowIndex_, colIndex = dragPosColumnIndex_]() {});
            UpdateCurInsertPos(dragPosRowIndex_, dragPosColumnIndex_);
            PrepareAnimationController(key);
        } else {
            UpdateCurInsertPos(dragPosRowIndex_, dragPosColumnIndex_);
        }
    }
}

void RenderGridLayout::MoveWhenWithInsertCellButWithItemInDragCellDragAfterInsert()
{
    int32_t startRow = curInsertRowIndex_;
    int32_t startColum = curInsertColumnIndex_;
    GetNextGrid(startRow, startColum);
    if (MoveItemsBackward(startRow, startColum, dragPosRowIndex_, dragPosColumnIndex_)) {
        if (supportAnimation_ || dragAnimation_) {
            std::string key(__FUNCTION__);
            RegisterAnimationFinishedFunc(
                key, [weak = WeakClaim(this), rowIndex = dragPosRowIndex_, colIndex = dragPosColumnIndex_]() {});
            UpdateCurInsertPos(dragPosRowIndex_, dragPosColumnIndex_);
            PrepareAnimationController(key);
        } else {
            UpdateCurInsertPos(dragPosRowIndex_, dragPosColumnIndex_);
        }
    }
}

void RenderGridLayout::FakeRemoveDragItem()
{
    dragPosRowIndex_ = -1;
    dragPosColumnIndex_ = -1;

    int32_t endRow = -1;
    int32_t endColum = -1;
    bool ret = CalTheFirstEmptyCell(endRow, endColum, true);
    if (!ret) {
        endRow = rowCount_ - 1;
        endColum = colCount_ - 1;
    }
    if (curInsertRowIndex_ == endRow && curInsertColumnIndex_ == endColum) {
        UpdateMatrixByIndexStrong(CELL_EMPTY, curInsertRowIndex_, curInsertColumnIndex_);
        curInsertRowIndex_ = -1;
        curInsertColumnIndex_ = -1;
    } else {
        int32_t startRow = curInsertRowIndex_;
        int32_t startColum = curInsertColumnIndex_;
        if (ret) {
            GetPreviousGird(endRow, endColum);
        }
        GetNextGrid(startRow, startColum);
        if (MoveItemsBackward(startRow, startColum, endRow, endColum)) {
            if ((supportAnimation_ || dragAnimation_) && needRunAnimation_.load()) {
                std::string key(__FUNCTION__);
                RegisterAnimationFinishedFunc(key, [weak = WeakClaim(this), rowIndex = endRow, colIndex = endColum]() {
                    auto renderGrid = weak.Upgrade();
                    if (renderGrid) {
                        renderGrid->curInsertRowIndex_ = -1;
                        renderGrid->curInsertColumnIndex_ = -1;
                        renderGrid->UpdateMatrixByIndexStrong(CELL_EMPTY, rowIndex, colIndex);
                    }
                });
                PrepareAnimationController(key);
            } else {
                curInsertRowIndex_ = -1;
                curInsertColumnIndex_ = -1;
                UpdateMatrixByIndexStrong(CELL_EMPTY, endRow, endColum);
            }
        } else {
            if ((supportAnimation_ || dragAnimation_) && needRunAnimation_.load()) {
                std::string key(__FUNCTION__);
                PrepareAnimationController(key);
            }
        }
    }
}

void RenderGridLayout::FakeRemoveDragItemUpdate()
{
    if (isDynamicGrid_ && NeedBeSmaller()) {
        InitialDynamicGridProp(DRAG_LEAVE);
        SetLayoutSize(GetLayoutParam().Constrain(Size(colSize_, rowSize_)));
        if (rightToLeft_) {
            ResetItemPosition();
        }
    }
}

bool RenderGridLayout::MoveItemsForward(int32_t fromRow, int32_t fromColum, int32_t toRow, int32_t toColum)
{
    bool valid = false;
    if (!SortCellIndex(fromRow, fromColum, toRow, toColum, valid) && (!valid)) {
        return false;
    }

    int32_t curRow = toRow;
    int32_t curColum = toColum;
    int32_t targetRow = toRow;
    int32_t targetColum = toColum;
    int32_t tmpIndex = -1;

    bool equal = false;
    equal = SortCellIndex(fromRow, fromColum, curRow, curColum, valid);

    if (supportAnimation_ || dragAnimation_) {
        animationItemList_.clear();
    }

    while (valid || equal) {
        // Get target pos
        GetNextGrid(targetRow, targetColum);

        // Get index in the curpos
        tmpIndex = GetIndexByGrid(curRow, curColum);
        if (tmpIndex < 0 || (int32_t)itemsInGrid_.size() <= tmpIndex) {
            return false;
        }

        // Move the curpos index to the targetpos
        UpdateMatrixByIndexStrong(tmpIndex, targetRow, targetColum);
        UpdateMatrixByIndexStrong(CELL_EMPTY, curRow, curColum);

        auto item = itemsInGrid_[tmpIndex];
        if (supportAnimation_ || dragAnimation_) {
            AddNodeAnimationToController(tmpIndex, targetRow, targetColum, 1, 1);
        } else {
            item->Layout(MakeInnerLayoutParam(targetRow, targetColum, 1, 1));
            SetChildPosition(item, targetRow, targetColum, 1, 1);
        }

        // move the curpos backward
        GetPreviousGird(curRow, curColum);
        targetRow = curRow;
        targetColum = curColum;

        equal = SortCellIndex(fromRow, fromColum, curRow, curColum, valid);
    }
    return true;
}

bool RenderGridLayout::MoveItemsBackward(int32_t fromRow, int32_t fromColum, int32_t toRow, int32_t toColum)
{
    bool valid = false;
    if (!SortCellIndex(fromRow, fromColum, toRow, toColum, valid) && (!valid)) {
        return false;
    }

    int32_t curRow = fromRow;
    int32_t curColum = fromColum;
    int32_t targetRow = fromRow;
    int32_t targetColum = fromColum;
    int32_t tmpIndex = -1;

    bool equal = false;
    equal = SortCellIndex(curRow, curColum, toRow, toColum, valid);
    if (supportAnimation_ || dragAnimation_) {
        animationItemList_.clear();
    }
    while (valid || equal) {
        // Get target pos
        GetPreviousGird(targetRow, targetColum);

        // Get index in the curpos
        tmpIndex = GetIndexByGrid(curRow, curColum);
        if (tmpIndex < 0 || (int32_t)itemsInGrid_.size() <= tmpIndex) {
            return false;
        }

        // Move the curpos index to the targetpos
        UpdateMatrixByIndexStrong(tmpIndex, targetRow, targetColum);
        UpdateMatrixByIndexStrong(CELL_EMPTY, curRow, curColum);

        auto item = itemsInGrid_[tmpIndex];
        if (supportAnimation_ || dragAnimation_) {
            AddNodeAnimationToController(tmpIndex, targetRow, targetColum, 1, 1);
        } else {
            item->Layout(MakeInnerLayoutParam(targetRow, targetColum, 1, 1));
            SetChildPosition(item, targetRow, targetColum, 1, 1);
        }
        // move the curpos and targetpos backward
        GetNextGrid(curRow, curColum);
        targetRow = curRow;
        targetColum = curColum;

        equal = SortCellIndex(curRow, curColum, toRow, toColum, valid);
    }

    return true;
}

void RenderGridLayout::UpdateMatrixByIndexStrong(int32_t index, int32_t row, int32_t column)
{
    std::map<int32_t, int32_t> rowMap;

    auto rowIter = gridMatrix_.find(row);
    if (rowIter != gridMatrix_.end()) {
        rowMap = rowIter->second;
    }

    auto indexIter = rowMap.find(column);
    if (indexIter != rowMap.end()) {
        rowMap[column] = index;
    } else {
        rowMap.emplace(std::make_pair(column, index));
    }

    gridMatrix_[row] = rowMap;
}

void RenderGridLayout::UpdateCurInsertPos(int32_t curInsertRow, int32_t curInsertColum)
{
    curInsertRowIndex_ = curInsertRow;
    curInsertColumnIndex_ = curInsertColum;
    UpdateMatrixByIndexStrong(CELL_FOR_INSERT, curInsertRowIndex_, curInsertColumnIndex_);
}

int32_t RenderGridLayout::CalIndexForItemByRowAndColum(int32_t row, int32_t column)
{
    int32_t curRow = 0;
    int32_t curColum = 0;
    int32_t targetIndex = 0;
    if (row >= 0 && row < rowCount_ && column >= 0 && column < colCount_) {
        while (curRow != row || curColum != column) {
            GetNextGrid(curRow, curColum);
            if (curRow >= rowCount_ || curColum >= colCount_) {
                targetIndex = -1;
                break;
            }
            targetIndex++;
        }
    } else {
        targetIndex = -1;
    }
    return targetIndex;
}

bool RenderGridLayout::SortCellIndex(
    int32_t rowFirst, int32_t columFirst, int32_t rowSecond, int32_t columSecond, bool& firstIsPre)
{
    if ((rowFirst == rowSecond) && (columFirst == columSecond)) {
        firstIsPre = false;
        return true;
    }

    if (isVertical_) {
        if (rowFirst > rowSecond) {
            firstIsPre = false;
        } else if (rowFirst == rowSecond) {
            if (columFirst > columSecond) {
                firstIsPre = false;
            } else {
                firstIsPre = true;
            }
        } else {
            firstIsPre = true;
        }
    } else {
        if (columFirst > columSecond) {
            firstIsPre = false;
        } else if (columFirst == columSecond) {
            if (rowFirst > rowSecond) {
                firstIsPre = false;
            } else {
                firstIsPre = true;
            }
        } else {
            firstIsPre = true;
        }
    }
    return false;
}

bool RenderGridLayout::CalTheFirstEmptyCell(int32_t& rowIndex, int32_t& columIndex, bool ignoreInsert)
{
    int32_t row = 0;
    int32_t column = 0;
    int32_t index = -3;

    index = GetIndexByGrid(row, column);

    while ((-1 != index) && (ignoreInsert || (CELL_FOR_INSERT != index))) {
        GetNextGrid(row, column);
        if (row >= rowCount_ || column >= colCount_) {
            return false;
        }
        index = GetIndexByGrid(row, column);
    }

    rowIndex = row;
    columIndex = column;
    return true;
}

void RenderGridLayout::InitAnimationController(const WeakPtr<PipelineContext>& context)
{
    if (!animationController_) {
        animationController_ = AceType::MakeRefPtr<Animator>(context);
    }
    if (!flexController_) {
        flexController_ = AceType::MakeRefPtr<Animator>(context);
    }
}

bool RenderGridLayout::AddNodeAnimationToController(
    int32_t itemIndex, int32_t row, int32_t col, int32_t rowSpan, int32_t colSpan)
{
    auto item = itemsInGrid_[itemIndex];
    if (!item || !animationController_) {
        return false;
    }

    Point startPoint(item->GetPosition().GetX(), item->GetPosition().GetY());
    Point endPoint = CalcChildPosition(item, row, col, rowSpan, colSpan);
    auto animationRef = AceType::MakeRefPtr<CurveAnimation<Point>>(startPoint, endPoint, Curves::FRICTION);
    animationRef->AddListener(
        [item, weak = WeakClaim(this)](const Point newPoint) {
            if (item) {
                item->SetPosition(Offset(newPoint.GetX(), newPoint.GetY()));
            }
            auto renderGrid = weak.Upgrade();
            if (renderGrid) {
                renderGrid->MarkNeedLayout();
            }
    });

    auto gridLayoutItem = AceType::DynamicCast<RenderGridLayoutItem>(item);
    int32_t depth = DEFAULT_DEPTH;
    while (!gridLayoutItem && depth > 0) {
        if (!item || item->GetChildren().empty()) {
            LOGE("%{public}s. item has no children anymore", __PRETTY_FUNCTION__);
            break;
        }
        item = item->GetChildren().front();
        gridLayoutItem = AceType::DynamicCast<RenderGridLayoutItem>(item);
        --depth;
    }
    if (gridLayoutItem) {
        if (gridLayoutItem->AnimationAddInterpolator(animationRef)) {
            needRunAnimation_.store(true);
            animationItemList_.emplace_back(item);
        }
    }
    return true;
}

void RenderGridLayout::AddNodeAnimationToControllerForDrop(
    const RefPtr<RenderNode>& item, const Point& startPoint, const Point& endPoint)
{
    if (!item || !animationController_ || startPoint == endPoint) {
        return;
    }
    item->SetNeedRender(true);
    item->MarkNeedPredictLayout();

    if (curInsertRowIndex_ >= 0 && curInsertColumnIndex_ >= 0) {
        item->Layout(MakeInnerLayoutParam(curInsertRowIndex_, curInsertColumnIndex_, 1, 1));
    }
    auto animationRef = AceType::MakeRefPtr<CurveAnimation<Point>>(startPoint, endPoint, Curves::FRICTION);
    animationRef->AddListener([weak = WeakClaim(this), item](const Point newPoint) {
        auto renderGrid = weak.Upgrade();
        if (renderGrid) {
            if (item) {
                item->SetPosition(Offset(newPoint.GetX(), newPoint.GetY()));
            }
            renderGrid->MarkNeedLayout();
        }
    });

    auto itemTmp = item;
    auto gridLayoutItem = AceType::DynamicCast<RenderGridLayoutItem>(itemTmp);
    int32_t depth = DEFAULT_DEPTH;
    while (!gridLayoutItem && depth > 0) {
        if (!itemTmp || itemTmp->GetChildren().empty()) {
            LOGE("%{public}s. itemTmp has no children anymore", __PRETTY_FUNCTION__);
            break;
        }
        itemTmp = itemTmp->GetChildren().front();
        gridLayoutItem = AceType::DynamicCast<RenderGridLayoutItem>(itemTmp);
        --depth;
    }
    if (gridLayoutItem) {
        if (gridLayoutItem->AnimationAddInterpolator(animationRef)) {
            needRunAnimation_.store(true);
            animationItemList_.emplace_back(itemTmp);
        }
    }
}

void RenderGridLayout::PrepareAnimationController(const std::string& key)
{
    LOGD("%{public}s called.", __FUNCTION__);
    if (animationAct_ != GridLayoutAnimationAct::ANIMATION_NONE) {
        LOGD("%{public}s indicator animation is processing.", __FUNCTION__);
        return;
    }

    needRunAnimation_.store(true);
    animationController_->SetDuration(ITEM_ANIMATION_DURATION);
    animationController_->ClearStopListeners();
    animationController_->AddStopListener([weak = AceType::WeakClaim(this), key]() {
        auto renderGrid = weak.Upgrade();
        if (renderGrid) {
            renderGrid->FinishedAnimationController(key);
        }
    });
}

void RenderGridLayout::StartAnimationController(GridLayoutAnimationAct animationAct, const OnAnimationCallJSFunc& func)
{
    LOGD("%{public}s called.", __FUNCTION__);
    if (needRunAnimation_) {
        animationAct_ = animationAct;
        needRunAnimation_.store(false);
        switch (animationAct_) {
            case GridLayoutAnimationAct::ANIMATION_DRAG_MOVE:
                jsMoveFunc_ = func;
                break;
            case GridLayoutAnimationAct::ANIMATION_DRAG_DROP:
                jsDropFunc_ = func;
                break;
            case GridLayoutAnimationAct::ANIMATION_RESTORE_SCENE:
                restoreSceneFunc_ = func;
                break;
            default:
                jsMoveFunc_ = nullptr;
                jsDropFunc_ = nullptr;
                restoreSceneFunc_ = nullptr;
                break;
        }
        animationController_->Play();

        for (auto iter = animationItemList_.begin(); iter != animationItemList_.end(); ++iter) {
            AceType::DynamicCast<RenderGridLayoutItem>(*iter)->AnimationPlay();
        }
    }
}

void RenderGridLayout::StopAnimationController()
{
    LOGD("%{public}s called.", __FUNCTION__);
    if (animationController_ && !animationController_->IsStopped()) {
        animationController_->ClearStopListeners();
        animationController_->Stop();
        animationAct_ = GridLayoutAnimationAct::ANIMATION_NONE;
    }
}

void RenderGridLayout::FinishedAnimationController(const std::string& key)
{
    LOGD("%{public}s called.", __FUNCTION__);
    {
        std::lock_guard<std::mutex> funLock(animationLock_);
        auto iter = animationFinishedFuncList_.find(key);
        if (iter != animationFinishedFuncList_.end()) {
            iter->second();
            animationFinishedFuncList_.erase(iter);
        }
        animationController_->ClearInterpolators();
    }

    OnAnimationCallJSFunc func = nullptr;
    switch (animationAct_) {
        case GridLayoutAnimationAct::ANIMATION_DRAG_MOVE:
            func = jsMoveFunc_;
            break;
        case GridLayoutAnimationAct::ANIMATION_DRAG_DROP:
            func = jsDropFunc_;
            break;
        case GridLayoutAnimationAct::ANIMATION_RESTORE_SCENE:
            func = restoreSceneFunc_;
            break;
        default:
            break;
    }
    animationAct_ = GridLayoutAnimationAct::ANIMATION_NONE;
    if (func) {
        func();
    }
    MarkNeedLayout();
}

void RenderGridLayout::RegisterAnimationFinishedFunc(const std::string& key, std::function<void()> func)
{
    std::lock_guard<std::mutex> funLock(animationLock_);
    if (func) {
        animationFinishedFuncList_[key] = func;
    }
}

void RenderGridLayout::TriggerMoveEventForJS(const ItemDragInfo& info)
{
    int32_t insertIndex = CalIndexForItemByRowAndColum(curInsertRowIndex_, curInsertColumnIndex_);
    if (component_->GetOnGridDragMoveId()) {
        LOGD("%{public}s. could be insert. InsertIndex: %{public}d ,ItemIndex: %{public}d", __PRETTY_FUNCTION__,
            insertIndex, dragingItemIndex_);
        component_->GetOnGridDragMoveId()(info, dragingItemIndex_, insertIndex);
    } else {
        LOGE("%{public}s no onGridDragMove registered.", __PRETTY_FUNCTION__);
    }
}

void RenderGridLayout::TriggerDropEventForJS(const ItemDragInfo& info, int32_t insertIndex, bool success)
{
    if (component_->GetOnGridDropId()) {
        LOGD("%{public}s. could be insert. ItemIndex: %{public}d, InsertIndex: %{public}d ", __PRETTY_FUNCTION__,
            dragingItemIndex_, insertIndex);
        component_->GetOnGridDropId()(info, dragingItemIndex_, insertIndex, success);
    } else {
        LOGE("%{public}s no onGridDrop registered.", __PRETTY_FUNCTION__);
    }
}

void RenderGridLayout::RegisterDropJSEvent(const ItemDragInfo& info, int32_t insertIndex, bool success)
{
    auto weak = WeakClaim(this);
    RegisterDropJSFunc([weak, info, insertIndex, success]() {
        auto renderGrid = weak.Upgrade();
        if (renderGrid) {
            renderGrid->TriggerDropEventForJS(info, insertIndex, success);
        }
    });
    triggerJSDrop_.store(true);
}

void RenderGridLayout::RegisterDropJSFunc(const OnCallJSDropFunc& func)
{
    std::lock_guard<std::mutex> lock(dropJSFuncListLock_);
    if (func != nullptr) {
        dropJSFuncList_.push_back(std::move(func));
    }
}

void RenderGridLayout::CallDropJSFunc()
{
    std::lock_guard<std::mutex> lock(dropJSFuncListLock_);
    for (auto& func: dropJSFuncList_) {
        func();
    }
    dropJSFuncList_.clear();
}

bool RenderGridLayout::CheckAnimation()
{
    if (!(supportAnimation_ || dragAnimation_ || edgeEffect_ == EdgeEffect::SPRING)) {
        return false;
    }
    if (animationAct_ != GridLayoutAnimationAct::ANIMATION_NONE || runFlexAnimation_.load()) {
        return true;
    }
    if (!(GetSlideStatus() == GridSlideStatus::SLIDE_NONE || GetSlideStatus() == GridSlideStatus::SLIDE_START)) {
        return true;
    }

    if (!isMainGrid_) {
        if (itemDragEntered_) {
            auto mainGrid = mainGrid_.Upgrade();
            if (mainGrid) {
                return mainGrid->isDragging_.load();
            }
        } else {
            return false;
        }
    } else {
        return isDragging_.load();
    }
    return false;
}

bool RenderGridLayout::CheckNeedShrink() const
{
    return isDeclarative_ && needShrink_;
}

void RenderGridLayout::RefreshAllocatedRowSizes(int32_t rowIndex, int32_t itemRowSpan, const RefPtr<RenderNode>& item)
{
    if (CheckNeedShrink()) {
        allocatedRowSizes_[rowIndex + itemRowSpan] = allocatedRowSizes_[rowIndex] + item->GetLayoutSize().Height();
    }
}

void RenderGridLayout::ParseRestoreScenePosition(
    const std::map<int32_t, std::map<int32_t, int32_t>>& data, std::map<int32_t, GridItemIndexPosition>& info)
{
    info.clear();
    for (auto rowIter = data.begin(); rowIter != data.end(); rowIter++) {
        for (auto colIter = rowIter->second.begin(); colIter != rowIter->second.end(); colIter++) {
            if (info.find(colIter->second) == info.end()) {
                GridItemIndexPosition itemIndexPosition(rowIter->first, colIter->first);
                info.emplace(colIter->second, itemIndexPosition);
            }
        }
    }
}

void RenderGridLayout::CalcRestoreScenePosition(const ItemDragInfo& info)
{
    std::map<int32_t, GridItemIndexPosition> backData;
    std::map<int32_t, GridItemIndexPosition> recentData;
    ParseRestoreScenePosition(gridMatrixBack_, backData);
    ParseRestoreScenePosition(gridMatrix_, recentData);

    for (auto backIter = backData.begin(); backIter != backData.end(); backIter++) {
        auto recentIter = recentData.find(backIter->first);
        if (recentIter != recentData.end() && backIter->second != recentIter->second &&
            backIter->first != dragingItemIndex_ && backIter->first >= 0 &&
            backIter->first < (int32_t)itemsInGrid_.size()) {
            auto item = itemsInGrid_[backIter->first];
            if (item) {
                Point startPoint(item->GetPosition().GetX(), item->GetPosition().GetY());
                AddNodeAnimationToControllerForDrop(item, startPoint, gridItemPosition_[backIter->first]);
            }
        }
    }
}

void RenderGridLayout::StartFlexController(const Point& endPoint, bool includeSubGrid)
{
    if (!(supportAnimation_ || dragAnimation_)) {
        CloseFlexComponent();
        if (includeSubGrid) {
            FinishedFlexControllerForSubGrid();
        }
        FinishedFlexController();
        ClearAllDragInfo();
        MarkNeedLayout();
        return;
    }
    runFlexAnimation_.store(true);
    auto animationRef = AceType::MakeRefPtr<CurveAnimation<Point>>(lastGlobalPoint_, endPoint, Curves::FRICTION);
    animationRef->AddListener(
        [weak = WeakClaim(this)](const Point newPoint) {
            auto renderGrid = weak.Upgrade();
            if (renderGrid) {
                renderGrid->UpdateFlexComponentPosition(newPoint);
            }
        });

    flexController_->AddInterpolator(animationRef);
    if (isExistComponent_) {
        flexController_->SetDuration(ITEM_ANIMATION_DURATION);
    } else {
        flexController_->SetDuration(ITEM_ANIMATION_DURATION_NO);
    }
    flexController_->ClearStopListeners();
    flexController_->AddStopListener([weak = AceType::WeakClaim(this), includeSubGrid]() {
        auto renderGrid = weak.Upgrade();
        if (renderGrid) {
            if (includeSubGrid) {
                renderGrid->FinishedFlexControllerForSubGrid();
            }
            renderGrid->FinishedFlexController();
            renderGrid->ClearAllDragInfo();
            renderGrid->MarkNeedLayout();
        }
    });
    flexController_->Play();
}

void RenderGridLayout::FinishedFlexController()
{
    runFlexAnimation_.store(false);
    CloseFlexComponent();
    if (animationAct_ == GridLayoutAnimationAct::ANIMATION_NONE) {
        if (triggerJSDrop_.load()) {
            triggerJSDrop_.store(false);
            CallDropJSFunc();
        }
    }
}

void RenderGridLayout::FinishedFlexControllerForSubGrid()
{
    auto subGrid = subGrid_.Upgrade();
    if (subGrid) {
        subGrid->CallDropJSFunc();
        subGrid->ClearAllDragInfo();
    }
}

void RenderGridLayout::CloseFlexComponent()
{
    auto pipelineContext = GetContext().Upgrade();
    if (pipelineContext && isExistComponent_) {
        LOGD("RenderGridLayout::PanOnActionEnd PopComponent");
        auto stackElement = pipelineContext->GetLastStack();
        stackElement->PopComponent();
        isExistComponent_ = false;
    }
    isDragging_.store(false);
}

void RenderGridLayout::UpdateFlexComponentPosition(const Point& pos)
{
    if (GetUpdatePositionId() && isExistComponent_) {
        LOGD("RenderGridLayout::PanOnActionUpdate move x=%{public}f, y=%{public}f.", pos.GetX(), pos.GetY());
        auto eventInfo = ItemDragInfo();
        eventInfo.SetX(pos.GetX());
        eventInfo.SetY(pos.GetY());
        GetUpdatePositionId()(Dimension(pos.GetX()), Dimension(pos.GetY()));
        MarkNeedLayout();
    }
}

void RenderGridLayout::ClearSpringSlideData()
{
    gravitationDirect_ = GridSpringGravitationDirect::SPRING_NONE;
    slideDirect_ = GridSlideDirect::SLIDE_NODE;
    slideStatus_.store(GridSlideStatus::SLIDE_NONE);
    slideStartPoint_ = Point(0.0, 0.0);
    slideDistance_ = Point(0.0, 0.0);
    slidePriPoint_ = Point(0.0, 0.0);
    slideCurPoint_ = Point(0.0, 0.0);
}

void RenderGridLayout::CreateSlideRecognizer()
{
    if (!(supportAnimation_ || edgeEffect_ == EdgeEffect::SPRING) || slideRecognizer_) {
        return;
    }
    slideRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
    auto weak = AceType::WeakClaim(this);
    slideRecognizer_->SetOnTouchDown([weak](const TouchEventInfo& info) {
        auto renderGrid = weak.Upgrade();
        if (renderGrid) {
            renderGrid->HandleSlideStart(info);
        }
    });
    slideRecognizer_->SetOnTouchUp([weak](const TouchEventInfo& info) {
        auto renderGrid = weak.Upgrade();
        if (renderGrid) {
            renderGrid->HandleSlideEnd(info);
        }
    });
    slideRecognizer_->SetOnTouchMove([weak](const TouchEventInfo& info) {
        auto renderGrid = weak.Upgrade();
        if (renderGrid) {
            renderGrid->HandleSlideUpdate(info);
        }
    });
}

void RenderGridLayout::HandleSlideStart(const TouchEventInfo& info)
{
    if (CheckLongPress() || GetSlideStatus() != GridSlideStatus::SLIDE_NONE) {
        return;
    }
    slideStartPoint_ = GetPointFromTouchInfo(info);
    slidePriPoint_ = slideStartPoint_;
    slideCurPoint_ = slideStartPoint_;
    slideDistance_ = Point(0.0, 0.0);
    UpdateSlideStatus(GridSlideStatus::SLIDE_START);
}

void RenderGridLayout::HandleSlideUpdate(const TouchEventInfo& info)
{
    if (CheckLongPress()) {
        return;
    }
    if (GetSlideStatus() != GridSlideStatus::SLIDE_START && GetSlideStatus() != GridSlideStatus::SLIDE_SLIDING) {
        return;
    }
    if (GetSlideStatus() == GridSlideStatus::SLIDE_START && !MayStartToSlide(info)) {
        return;
    }
    UpdateSlideStatus(GridSlideStatus::SLIDE_SLIDING);
    slideCurPoint_ = GetPointFromTouchInfo(info);
    CalcSlideDirect(slideCurPoint_);
    double dx = slideCurPoint_.GetX() - slidePriPoint_.GetX();
    double dy = slideCurPoint_.GetY() - slidePriPoint_.GetY();
    MoveRelativeDistance(dx, dy);
    for (auto& item : itemsInGrid_) {
        if (slideDirect_ == GridSlideDirect::SLIDE_VERTICAL) {
            item->SetPosition(Offset(item->GetPosition().GetX(), item->GetPosition().GetY() + dy));
        } else {
            item->SetPosition(Offset(item->GetPosition().GetX() + dx, item->GetPosition().GetY()));
        }
    }
    MarkNeedLayout();
    slidePriPoint_ = slideCurPoint_;
}

void RenderGridLayout::HandleSlideEnd(const TouchEventInfo& info)
{
    if (CheckLongPress()) {
        return;
    }
    if (GetSlideStatus() == GridSlideStatus::SLIDE_SPRING_START || GetSlideStatus() == GridSlideStatus::SLIDE_NONE) {
        return;
    }
    UpdateSlideStatus(GridSlideStatus::SLIDE_SPRING_START);
    CalcSpringGravitationDirect();
    if (gravitationDirect_ != GridSpringGravitationDirect::SPRING_NONE) {
        BackupSpringItemsData();
        Point startPoint = GetSpringStartPoint();
        Point endPoint = GetSpringEndPoint();
        StartSpringAnimation(startPoint, endPoint);
    } else {
        FinishedSpringAnimation();
    }
}

bool RenderGridLayout::MayStartToSlide(const TouchEventInfo& info)
{
    bool result = false;
    Point movePoint = GetPointFromTouchInfo(info);
    double dx = std::fabs(movePoint.GetX() - slideStartPoint_.GetX());
    double dy = std::fabs(movePoint.GetY() - slideStartPoint_.GetY());
    if (dx >= GRID_SPRING_SLIDE_LIMIT || dy >= GRID_SPRING_SLIDE_LIMIT) {
        slideCurPoint_ = movePoint;
        result = true;
    }
    return result;
}

bool RenderGridLayout::CheckLongPress()
{
    if (animationAct_ != GridLayoutAnimationAct::ANIMATION_NONE || itemLongPressed_ || isDragging_.load() ||
        itemDragStarted_ || GetChildren().empty()) {
        return true;
    } else {
        return false;
    }
}

void RenderGridLayout::UpdateSlideStatus(GridSlideStatus status)
{
    slideStatus_.store(status);
}

GridSlideStatus RenderGridLayout::GetSlideStatus()
{
    return slideStatus_.load();
}

GridSlideDirect RenderGridLayout::GetSlideDirect()
{
    return slideDirect_;
}

Point RenderGridLayout::GetPointFromTouchInfo(const TouchEventInfo& info)
{
    const auto& locationInfo = info.GetTouches().front();
    return Point(locationInfo.GetLocalLocation().GetX() - GetGlobalOffset().GetX(),
        locationInfo.GetLocalLocation().GetY() - GetGlobalOffset().GetY());
}

void RenderGridLayout::MoveRelativeDistance(double& dx, double& dy)
{
    if (slideDistance_.GetX() + dx < 0.0) {
        dx = -slideDistance_.GetX();
    }
    slideDistance_.SetX(slideDistance_.GetX() + dx);

    if (slideDistance_.GetY() + dy < 0.0) {
        dy = -slideDistance_.GetY();
    }
    slideDistance_.SetY(slideDistance_.GetY() + dy);
}

void RenderGridLayout::CreateSpringController()
{
    if (!springController_) {
        springController_ = AceType::MakeRefPtr<Animator>(GetContext());
    }
}

Point RenderGridLayout::GetSpringStartPoint()
{
    double dx = 0.0;
    double dy = 0.0;
    if (slideDirect_ == GridSlideDirect::SLIDE_HORIZON) {
        dx = GetLeft().Value();
        dy = (GetTop().Value() + GetBottom().Value()) / GAP_DIVIDE_CONSTEXPR;
    } else {
        dx = (GetLeft().Value() + GetRight().Value()) / GAP_DIVIDE_CONSTEXPR;
        dy = GetTop().Value();
    }
    return Point(dx, dy);
}

Point RenderGridLayout::GetSpringEndPoint()
{
    double dx = 0.0;
    double dy = 0.0;
    if (slideDirect_ == GridSlideDirect::SLIDE_HORIZON) {
        dx = GetLeft().Value() + slideDistance_.GetX();
        dy = (GetTop().Value() + GetRight().Value()) / GAP_DIVIDE_CONSTEXPR;
    } else {
        dx = (GetLeft().Value() + GetRight().Value()) / GAP_DIVIDE_CONSTEXPR;
        dy = GetTop().Value() + slideDistance_.GetY();
    }
    return Point(dx, dy);
}

void RenderGridLayout::StartSpringAnimation(const Point& startPoint, const Point& endPoint)
{
    double start = 0.0;
    double end = 0.0;
    GetMotionPosition(startPoint, endPoint, start, end);
    auto springDes = AceType::MakeRefPtr<SpringProperty>(GRID_SPRING_MASS, GRID_SPRING_STIFF, GRID_SPRING_DAMP);
    if (!springMotion_) {
        springMotion_ = AceType::MakeRefPtr<SpringMotion>(start, end, 0.0, springDes);
    } else {
        springMotion_->Reset(start, end, 0.0, springDes);
    }

    springMotion_->ClearListeners();
    springMotion_->AddListener([weak = AceType::WeakClaim(this)](double position) {
        auto renderGrid = weak.Upgrade();
        if (renderGrid) {
            renderGrid->UpdateSprintAnimationPosition(position);
        }
    });
    springController_->AddStopListener([weak = AceType::WeakClaim(this)]() {
        auto renderGrid = weak.Upgrade();
        if (renderGrid) {
            renderGrid->FinishedSpringAnimation();
        }
    });
    springController_->PlayMotion(springMotion_);
}

void RenderGridLayout::FinishedSpringAnimation()
{
    ClearSpringSlideData();
    MarkNeedLayout();
}

void RenderGridLayout::UpdateSprintAnimationPosition(double position)
{
    LOGD("UpdateSprintAnimationPosition: distance(%{public}f,%{public}f), offset=%{public}f",
        slideDistance_.GetX(), slideDistance_.GetY(), position);
    size_t index = 0;
    for (auto& item : itemsInGrid_) {
        auto& itemPos = springStartPosition_[index];
        if (slideDirect_ == GridSlideDirect::SLIDE_HORIZON) {
            item->SetPosition(Offset(itemPos.GetX() - position, itemPos.GetY()));
        } else {
            item->SetPosition(Offset(itemPos.GetX(), itemPos.GetY() - position));
        }
        index++;
    }
    MarkNeedLayout();
}

void RenderGridLayout::BackupSpringItemsData()
{
    springStartPosition_.clear();
    for (auto& item : itemsInGrid_) {
        springStartPosition_.push_back(item->GetPosition());
    }
}

void RenderGridLayout::GetMotionPosition(const Point& startPoint, const Point& endPoint, double& start, double& end)
{
    if (slideDirect_ == GridSlideDirect::SLIDE_HORIZON) {
        start = startPoint.GetX();
        end = endPoint.GetX();
    } else {
        start = startPoint.GetY();
        end = endPoint.GetY();
    }
}

void RenderGridLayout::CalcSlideDirect(const Point& curPos)
{
    if (slideDirect_ != GridSlideDirect::SLIDE_NODE) {
        return;
    }
    if (isVertical_) {
        slideDirect_ = GridSlideDirect::SLIDE_VERTICAL;
    } else {
        slideDirect_ = GridSlideDirect::SLIDE_HORIZON;
    }
}

void RenderGridLayout::CalcSpringGravitationDirect()
{
    if (slideDirect_ == GridSlideDirect::SLIDE_HORIZON) {
        gravitationDirect_ = GridSpringGravitationDirect::SPRING_TO_LEFT;
    } else {
        gravitationDirect_ = GridSpringGravitationDirect::SPRING_TO_UP;
    }
}

bool RenderGridLayout::HandleMouseEvent(const MouseEvent& event)
{
    if (!isMultiSelectable_) {
        return false;
    }

    auto context = context_.Upgrade();
    if (context) {
        context->SubscribeCtrlA([wp = AceType::WeakClaim(this)]() {
            auto sp = wp.Upgrade();
            if (sp) {
                sp->MultiSelectAllWhenCtrlA();
            }
        });
    }

    if (context->IsCtrlDown()) {
        if (context->IsKeyboardA()) {
            MultiSelectAllWhenCtrlA();
            return true;
        }
        HandleMouseEventWhenCtrlDown(event);
        return true;
    }
    selectedItemsWithCtrl_.clear();

    if (context->IsShiftDown()) {
        HandleMouseEventWhenShiftDown(event);
        return true;
    }
    firstItemWithShift_ = nullptr;

    HandleMouseEventWithoutKeyboard(event);
    return true;
}

void RenderGridLayout::ClearMultiSelect()
{
    for (const auto& item : GetChildren()) {
        auto gridLayoutItem = FindChildOfClass<RenderGridLayoutItem>(item);
        if (!gridLayoutItem) {
            continue;
        }
        gridLayoutItem->MarkIsSelected(false);
    }
}

void RenderGridLayout::MultiSelectWithoutKeyboard(const Rect& selectedZone)
{
    if (!selectedZone.IsValid()) {
        Point mousePoint(selectedZone.GetOffset().GetX(), selectedZone.GetOffset().GetY());
        auto gridLayoutItem = FindChildNodeOfClass<RenderGridLayoutItem>(mousePoint, mousePoint);
        if (!gridLayoutItem) {
            return;
        }
        if (!gridLayoutItem->GetSelectable()) {
            return;
        }
        gridLayoutItem->MarkIsSelected(true);
        if (gridLayoutItem->GetOnSelectId()) {
            (gridLayoutItem->GetOnSelectId())(gridLayoutItem->IsSelected());
        }
        return;
    }

    for (const auto& item : GetChildren()) {
        auto gridLayoutItem = FindChildOfClass<RenderGridLayoutItem>(item);
        if (!gridLayoutItem) {
            continue;
        }
        if (!gridLayoutItem->GetSelectable()) {
            continue;
        }
        if (!selectedZone.IsIntersectWith(item->GetPaintRect())) {
            gridLayoutItem->MarkIsSelected(false);
            if (gridLayoutItem->GetOnSelectId()) {
                (gridLayoutItem->GetOnSelectId())(gridLayoutItem->IsSelected());
            }
            continue;
        }
        gridLayoutItem->MarkIsSelected(true);
        if (gridLayoutItem->GetOnSelectId()) {
            (gridLayoutItem->GetOnSelectId())(gridLayoutItem->IsSelected());
        }
    }
}

void RenderGridLayout::HandleMouseEventWithoutKeyboard(const MouseEvent& event)
{
    if (event.button == MouseButton::LEFT_BUTTON) {
        if (event.action == MouseAction::PRESS) {
            ClearMultiSelect();
            mouseStartOffset_ = event.GetOffset() - GetPaintRect().GetOffset();
            mouseEndOffset_ = event.GetOffset() - GetPaintRect().GetOffset();
            auto selectedZone = ComputeSelectedZone(mouseStartOffset_, mouseEndOffset_);
            MultiSelectWithoutKeyboard(selectedZone);
            MarkNeedRender();
        } else if (event.action == MouseAction::MOVE) {
            mouseEndOffset_ = event.GetOffset() - GetPaintRect().GetOffset();
            auto selectedZone = ComputeSelectedZone(mouseStartOffset_, mouseEndOffset_);
            MultiSelectWithoutKeyboard(selectedZone);
            MarkNeedRender();
        } else if (event.action == MouseAction::RELEASE) {
            mouseStartOffset_ = Offset(0.0, 0.0);
            mouseEndOffset_ = Offset(0.0, 0.0);
            MarkNeedRender();
        }
    }
}

RefPtr<RenderGridLayoutItem> RenderGridLayout::GetPressItemWhenShiftDown(const Rect& selectedZone)
{
    if (!selectedZone.IsValid()) {
        Point mousePoint(selectedZone.GetOffset().GetX(), selectedZone.GetOffset().GetY());
        auto gridLayoutItem = FindChildNodeOfClass<RenderGridLayoutItem>(mousePoint, mousePoint);
        if (!gridLayoutItem) {
            return nullptr;
        }
        if (!gridLayoutItem->GetSelectable()) {
            return nullptr;
        }
        return gridLayoutItem;
    }
    return nullptr;
}

void RenderGridLayout::MultiSelectWhenShiftDown(const Rect& selectedZone)
{
    for (const auto& item : GetChildren()) {
        auto gridLayoutItem = FindChildOfClass<RenderGridLayoutItem>(item);
        if (!gridLayoutItem) {
            continue;
        }
        if (!gridLayoutItem->GetSelectable()) {
            continue;
        }
        if (!selectedZone.IsIntersectWith(item->GetPaintRect())) {
            continue;
        }
        gridLayoutItem->MarkIsSelected(true);
        if (gridLayoutItem->GetOnSelectId()) {
            (gridLayoutItem->GetOnSelectId())(gridLayoutItem->IsSelected());
        }
    }
}

void RenderGridLayout::HandleMouseEventWhenShiftDown(const MouseEvent& event)
{
    if (event.button == MouseButton::LEFT_BUTTON) {
        if (event.action == MouseAction::PRESS) {
            mouseStartOffset_ = event.GetOffset() - GetPaintRect().GetOffset();
            mouseEndOffset_ = event.GetOffset() - GetPaintRect().GetOffset();
            auto selectedZone = ComputeSelectedZone(mouseStartOffset_, mouseEndOffset_);
            if (firstItemWithShift_ == nullptr) {
                firstItemWithShift_ = GetPressItemWhenShiftDown(selectedZone);
            }
            secondItemWithShift_ = GetPressItemWhenShiftDown(selectedZone);
            MultiSelectAllInRange(firstItemWithShift_, secondItemWithShift_);
            MarkNeedRender();
        } else if (event.action == MouseAction::MOVE) {
            mouseEndOffset_ = event.GetOffset() - GetPaintRect().GetOffset();
            auto selectedZone = ComputeSelectedZone(mouseStartOffset_, mouseEndOffset_);
            MultiSelectWhenShiftDown(selectedZone);
            MarkNeedRender();
        } else if (event.action == MouseAction::RELEASE) {
            mouseStartOffset_ = Offset(0.0, 0.0);
            mouseEndOffset_ = Offset(0.0, 0.0);
            MarkNeedRender();
        }
    }
}

void RenderGridLayout::MultiSelectAllInRange(const RefPtr<RenderGridLayoutItem>& firstItem,
    const RefPtr<RenderGridLayoutItem>& secondItem)
{
    ClearMultiSelect();
    if (!firstItem) {
        return;
    }

    if (!secondItem) {
        firstItem->MarkIsSelected(true);
        if (firstItem->GetOnSelectId()) {
            (firstItem->GetOnSelectId())(firstItem->IsSelected());
        }
        return;
    }

    auto fromItemIndex = std::min(firstItem->GetIndex(), secondItem->GetIndex());
    auto toItemIndex = std::max(firstItem->GetIndex(), secondItem->GetIndex());
    
    for (const auto& item : GetChildren()) {
        auto gridLayoutItem = FindChildOfClass<RenderGridLayoutItem>(item);
        if (!gridLayoutItem) {
            continue;
        }
        if (!gridLayoutItem->GetSelectable()) {
            continue;
        }

        auto nowIndex = gridLayoutItem->GetIndex();
        if (nowIndex <= toItemIndex && nowIndex >= fromItemIndex) {
            gridLayoutItem->MarkIsSelected(true);
            if (gridLayoutItem->GetOnSelectId()) {
                (gridLayoutItem->GetOnSelectId())(gridLayoutItem->IsSelected());
            }
        }
    }
}

void RenderGridLayout::MultiSelectWhenCtrlDown(const Rect& selectedZone)
{
    if (!selectedZone.IsValid()) {
        Point mousePoint(selectedZone.GetOffset().GetX(), selectedZone.GetOffset().GetY());
        auto gridLayoutItem = FindChildNodeOfClass<RenderGridLayoutItem>(mousePoint, mousePoint);
        if (!gridLayoutItem) {
            return;
        }
        if (!gridLayoutItem->GetSelectable()) {
            return;
        }

        if (selectedItemsWithCtrl_.find(gridLayoutItem) != selectedItemsWithCtrl_.end()) {
            gridLayoutItem->MarkIsSelected(false);
        } else {
            gridLayoutItem->MarkIsSelected(true);
        }

        if (gridLayoutItem->GetOnSelectId()) {
            (gridLayoutItem->GetOnSelectId())(gridLayoutItem->IsSelected());
        }
        return;
    }

    for (const auto& item : GetChildren()) {
        auto gridLayoutItem = FindChildOfClass<RenderGridLayoutItem>(item);
        if (!gridLayoutItem) {
            continue;
        }
        if (!gridLayoutItem->GetSelectable()) {
            continue;
        }
        if (!selectedZone.IsIntersectWith(item->GetPaintRect())) {
            if (selectedItemsWithCtrl_.find(gridLayoutItem) != selectedItemsWithCtrl_.end()) {
                gridLayoutItem->MarkIsSelected(true);
            }  else {
                gridLayoutItem->MarkIsSelected(false);
            }
            if (gridLayoutItem->GetOnSelectId()) {
                (gridLayoutItem->GetOnSelectId())(gridLayoutItem->IsSelected());
            }
            continue;
        }

        if (selectedItemsWithCtrl_.find(gridLayoutItem) != selectedItemsWithCtrl_.end()) {
            gridLayoutItem->MarkIsSelected(false);
        } else {
            gridLayoutItem->MarkIsSelected(true);
        }

        if (gridLayoutItem->GetOnSelectId()) {
            (gridLayoutItem->GetOnSelectId())(gridLayoutItem->IsSelected());
        }
    }
}

void RenderGridLayout::HandleMouseEventWhenCtrlDown(const MouseEvent& event)
{
    if (event.button == MouseButton::LEFT_BUTTON) {
        if (event.action == MouseAction::PRESS) {
            mouseStartOffset_ = event.GetOffset() - GetPaintRect().GetOffset();
            mouseEndOffset_ = event.GetOffset() - GetPaintRect().GetOffset();
            auto selectedZone = ComputeSelectedZone(mouseStartOffset_, mouseEndOffset_);
            MultiSelectWhenCtrlDown(selectedZone);
            MarkNeedRender();
        } else if (event.action == MouseAction::MOVE) {
            mouseEndOffset_ = event.GetOffset() - GetPaintRect().GetOffset();
            auto selectedZone = ComputeSelectedZone(mouseStartOffset_, mouseEndOffset_);
            MultiSelectWhenCtrlDown(selectedZone);
            MarkNeedRender();
        } else if (event.action == MouseAction::RELEASE) {
            mouseStartOffset_ = Offset(0.0, 0.0);
            mouseEndOffset_ = Offset(0.0, 0.0);
            MarkNeedRender();
            CollectSelectedItems();
        }
    }
}

void RenderGridLayout::CollectSelectedItems()
{
    selectedItemsWithCtrl_.clear();
    for (const auto& item : GetChildren()) {
        auto gridLayoutItem = FindChildOfClass<RenderGridLayoutItem>(item);
        if (!gridLayoutItem) {
            continue;
        }
        if (!gridLayoutItem->GetSelectable()) {
            continue;
        }
        if (gridLayoutItem->IsSelected()) {
            selectedItemsWithCtrl_.insert(gridLayoutItem);
        }
    }
}

void RenderGridLayout::MultiSelectAllWhenCtrlA()
{
    for (const auto& item : GetChildren()) {
        auto gridLayoutItem = FindChildOfClass<RenderGridLayoutItem>(item);
        if (!gridLayoutItem) {
            continue;
        }
        if (!gridLayoutItem->GetSelectable()) {
            continue;
        }
        gridLayoutItem->MarkIsSelected(true);
        if (gridLayoutItem->GetOnSelectId()) {
            (gridLayoutItem->GetOnSelectId())(gridLayoutItem->IsSelected());
        }
    }
    MarkNeedRender();
}

} // namespace OHOS::Ace
