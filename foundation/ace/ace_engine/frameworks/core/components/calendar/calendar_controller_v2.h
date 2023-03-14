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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_CONTROLLER_V2_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_CONTROLLER_V2_H

#include "core/components/calendar/calendar_component.h"

namespace OHOS::Ace {

class ACE_EXPORT CalendarControllerV2 : public AceType {
    DECLARE_ACE_TYPE(CalendarControllerV2, AceType);

public:
    CalendarControllerV2() = default;
    ~CalendarControllerV2() override = default;

    void SetCalendarController(const RefPtr<CalendarController>& controller)
    {
        controller_ = controller;
    }

    void BackToToday()
    {
        if (controller_) {
            auto today = controller_->GetToday();
            controller_->SetFirstSetToday(true);
            controller_->SetHasMoved(true);
            controller_->GoTo(today.month.year, today.month.month, today.day);
        }
    }

    void GoTo(int32_t year, int32_t month, int32_t day)
    {
        if (controller_) {
            CalendarDay calendarDay;
            calendarDay.month.month = month;
            calendarDay.month.year = year;
            controller_->SetCrossMonth(true);
            controller_->SetCrossMonthDay(calendarDay);
            controller_->SetHasMoved(true);
            controller_->GoTo(year, month, day);
        }
    }

private:
    RefPtr<CalendarController> controller_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_CONTROLLER_V2_H
