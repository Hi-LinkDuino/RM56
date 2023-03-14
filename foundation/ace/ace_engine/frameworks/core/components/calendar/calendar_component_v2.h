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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_COMPONENT_V2_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_COMPONENT_V2_H

#include "core/components/calendar/calendar_component.h"
#include "core/components/calendar/calendar_controller_v2.h"
#include "core/components_v2/common/common_def.h"

namespace OHOS::Ace {

class ACE_EXPORT CalendarComponentV2 : public CalendarComponent {
    DECLARE_ACE_TYPE(CalendarComponentV2, CalendarComponent);

public:
    CalendarComponentV2(const ComposeId& id, const std::string& name) : CalendarComponent(id, name) {}
    ~CalendarComponentV2() override = default;

    std::queue<ObtainedMonth>&& GetObtainedMonths()
    {
        return std::move(obtainedMonths_);
    }

    void SetCalendarData(const ObtainedMonth& days)
    {
        obtainedMonths_.emplace(days);
    }

    const RefPtr<CalendarTheme>& GetCalendarTheme()
    {
        return calendarTheme_;
    }

    void SetCalendarTheme(const RefPtr<CalendarTheme>& calendarTheme)
    {
        calendarTheme_ = calendarTheme;
    }

    void SetControllerV2(const RefPtr<CalendarControllerV2>& controllerV2)
    {
        controllerV2_ = controllerV2;
    }

    const RefPtr<CalendarControllerV2>& GetControllerV2()
    {
        return controllerV2_;
    }

private:
    std::queue<ObtainedMonth> obtainedMonths_;
    RefPtr<CalendarTheme> calendarTheme_;
    RefPtr<CalendarControllerV2> controllerV2_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_COMPONENT_V2_H
