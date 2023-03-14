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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_FOCUSABLE_GRID_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_FOCUSABLE_GRID_H

#include <map>

#include "base/memory/ace_type.h"
#include "core/components/common/layout/constants.h"

namespace OHOS::Ace {

class FocusableGrid : public virtual AceType {
    DECLARE_ACE_TYPE(FocusableGrid, AceType);

public:
    int32_t RequestNextFocus(bool vertical, bool reverse);

protected:
    virtual int32_t GetIndexByGrid(int32_t row, int32_t column) = 0;
    virtual void FocusChanged(int32_t oldIndex, int32_t newIndex) {}

    int32_t focusRow_ = -1;
    int32_t focusCol_ = -1;
    int32_t colCount_ = 0;
    int32_t rowCount_ = 0;
    int32_t focusIndex_ = 0;
    int32_t firstDayIndex_ = 0;
    int32_t selectedDayNumber_ = 1;

private:
    // Handle direction key move
    int32_t FocusMove(KeyDirection direction);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_FOCUSABLE_GRID_H
