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

#include "core/components/calendar/focusable_grid.h"

#include "base/log/log.h"
#include "core/common/ace_application_info.h"

namespace OHOS::Ace {
namespace {

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
const std::map<bool, std::map<bool, KeyDirection>> DIRECTION_MAP = {
    { HORIZONTAL, { { FORWARD, KeyDirection::RIGHT }, { REVERSE, KeyDirection::LEFT } } },
    { VERTICAL, { { FORWARD, KeyDirection::DOWN }, { REVERSE, KeyDirection::UP } } }
};

} // namespace

int32_t FocusableGrid::RequestNextFocus(bool vertical, bool reverse)
{
    KeyDirection key = DIRECTION_MAP.at(vertical).at(reverse);
    int32_t index = FocusMove(key);
    if (index < 0) {
        return index;
    }
    return focusIndex_;
}

// Handle direction key move
int32_t FocusableGrid::FocusMove(KeyDirection direction)
{
    int32_t nextRow = focusRow_;
    int32_t nextCol = focusCol_;
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
                AceApplicationInfo::GetInstance().IsRightToLeft() ? ++nextCol : --nextCol;
                break;
            case KeyDirection::RIGHT:
                AceApplicationInfo::GetInstance().IsRightToLeft() ? --nextCol : ++nextCol;
                break;
            default:
                return -1;
        }
        if (nextCol < 0) {
            if (nextRow == 0) {
                FocusChanged(focusIndex_, -1);
                return 1;
            }
            --nextRow;
            nextCol = colCount_ - 1;
        }
        if (nextCol >= colCount_) {
            if (nextRow == rowCount_ - 1) {
                FocusChanged(focusIndex_, GetIndexByGrid(nextRow, nextCol));
                return 1;
            }
            // when move to the end of row, move to newline
            ++nextRow;
            nextCol = 0;
        }
        if ((nextRow < 0) || (nextCol < 0) || (nextRow >= rowCount_) || (nextCol >= colCount_)) {
            return -1;
        }
        next = GetIndexByGrid(nextRow, nextCol);
    }
    focusRow_ = nextRow;
    focusCol_ = nextCol;
    if (next < firstDayIndex_ && direction == KeyDirection::UP) {
        LOGD("focus move out of calendar");
        return -1;
    }
    FocusChanged(focusIndex_, next);
    focusIndex_ = next;
    selectedDayNumber_ = next - firstDayIndex_ + 1;
    return next;
}

} // namespace OHOS::Ace
