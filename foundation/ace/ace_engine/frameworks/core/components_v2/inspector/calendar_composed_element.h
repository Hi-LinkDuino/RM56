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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_CALENDAR_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_CALENDAR_COMPOSED_ELEMENT_H

#include "core/components/calendar/calendar_theme.h"
#include "core/components/calendar/render_calendar.h"
#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT CalendarComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(CalendarComposedElement, InspectorComposedElement)

public:
    explicit CalendarComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~CalendarComposedElement() override = default;

    std::unique_ptr<JsonValue> ToJsonObject() const override;
    std::string GetShowLunar() const;
    std::string GetShowHoliday() const;
    std::string GetNeedSlide() const;
    std::string GetStartOfWeek() const;
    std::string GetOffDay() const;
    std::string GetCalendarDirection() const;
    std::unique_ptr<JsonValue> GetCurrentDayStyle() const;
    std::unique_ptr<JsonValue> GetNonCurrentDayStyle() const;
    std::unique_ptr<JsonValue> GetTodayStyle() const;
    std::unique_ptr<JsonValue> GetWeekStyle() const;
    std::unique_ptr<JsonValue> GetWorkStateStyle() const;
    std::string GetAxis() const;
    std::string ConvertWeekToString(std::string week) const;
    std::string ConvertOffDayToString(std::string offDay) const;
    std::vector<std::string> SplitString(const std::string& s, const std::string& c) const;
    std::string ConvertOffDayFamily(const std::vector<std::string>& data) const;

private:
    RefPtr<RenderCalendar> GetRenderCalendar() const;
    std::unique_ptr<JsonValue> ConvertThemeToDayStyle(const CalendarThemeStructure& theme) const;
    std::unique_ptr<JsonValue> ConvertThemeToNonCurrentDayStyle(const CalendarThemeStructure& theme) const;
    std::unique_ptr<JsonValue> ConvertThemeToTodayStyle(const CalendarThemeStructure& theme) const;
    std::unique_ptr<JsonValue> ConvertThemeToWeekStyle(const CalendarThemeStructure& theme) const;
    std::unique_ptr<JsonValue> ConvertThemeToWorkStateStyle(const CalendarThemeStructure& theme) const;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_CALENDAR_COMPOSED_ELEMENT_H
