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

#include "core/components_v2/inspector/calendar_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/calendar/calendar_element.h"
#include "core/components/common/layout/constants.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {

using CalendarJsonFunc = std::function<std::unique_ptr<JsonValue>(const CalendarComposedElement&)>;

const std::unordered_map<std::string, std::function<std::string(const CalendarComposedElement&)>> CREATE_JSON_MAP {
    { "showLunar", [](const CalendarComposedElement& inspector) { return inspector.GetShowLunar(); } },
    { "showHoliday", [](const CalendarComposedElement& inspector) { return inspector.GetShowHoliday(); } },
    { "needSlide", [](const CalendarComposedElement& inspector) { return inspector.GetNeedSlide(); } },
    { "startOfWeek", [](const CalendarComposedElement& inspector) { return inspector.GetStartOfWeek(); } },
    { "direction", [](const CalendarComposedElement& inspector) { return inspector.GetAxis(); } },
    { "offDay", [](const CalendarComposedElement& inspector) { return inspector.GetOffDay(); } },
};

const std::unordered_map<std::string, CalendarJsonFunc> CREATE_CALENDAR_THEME_JASON {
    { "currentDayStyle", [](const CalendarComposedElement& inspector) { return inspector.GetCurrentDayStyle(); } },
    { "nonCurrentDayStyle",
        [](const CalendarComposedElement& inspector) { return inspector.GetNonCurrentDayStyle(); } },
    { "todayStyle", [](const CalendarComposedElement& inspector) { return inspector.GetTodayStyle(); } },
    { "weekStyle", [](const CalendarComposedElement& inspector) { return inspector.GetWeekStyle(); } },
    { "workStateStyle", [](const CalendarComposedElement& inspector) { return inspector.GetWorkStateStyle(); } },
};

std::unique_ptr<JsonValue> CalendarComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    for (const auto& value : CREATE_CALENDAR_THEME_JASON) {
        resultJson->Put(value.first.c_str(), value.second(*this));
    }
    return resultJson;
}

std::string CalendarComposedElement::GetShowLunar() const
{
    auto renderCalendar = GetRenderCalendar();
    auto showLunar = renderCalendar ? renderCalendar->GetCalendarDataAdapter()->ShowLunar() : false;
    return ConvertBoolToString(showLunar);
}

std::string CalendarComposedElement::GetShowHoliday() const
{
    auto renderCalendar = GetRenderCalendar();
    auto showHoliday = renderCalendar ? renderCalendar->GetShowHoliday() : true;
    return ConvertBoolToString(showHoliday);
}

std::string CalendarComposedElement::GetNeedSlide() const
{
    auto renderCalendar = GetRenderCalendar();
    auto needSlide = renderCalendar ? renderCalendar->GetNeedSlide() : false;
    return ConvertBoolToString(needSlide);
}

std::string CalendarComposedElement::GetStartOfWeek() const

{
    auto renderCalendar = GetRenderCalendar();
    auto startDayOfWeek = renderCalendar ? renderCalendar->GetStartDayOfWeek() : 6;
    return ConvertWeekToString(std::to_string(startDayOfWeek));
}

std::string CalendarComposedElement::GetOffDay() const
{
    auto renderCalendar = GetRenderCalendar();
    auto offday = renderCalendar ? renderCalendar->GetOffdays() : "";
    return ConvertOffDayToString(offday);
}

std::unique_ptr<JsonValue> CalendarComposedElement::GetCurrentDayStyle() const
{
    auto renderCalendar = GetRenderCalendar();
    return renderCalendar ? ConvertThemeToDayStyle(renderCalendar->GetCalendarTheme()) : nullptr;
}

std::unique_ptr<JsonValue> CalendarComposedElement::GetNonCurrentDayStyle() const
{
    auto renderCalendar = GetRenderCalendar();
    return renderCalendar ? ConvertThemeToNonCurrentDayStyle(renderCalendar->GetCalendarTheme()) : nullptr;
}

std::unique_ptr<JsonValue> CalendarComposedElement::GetTodayStyle() const
{
    auto renderCalendar = GetRenderCalendar();
    return renderCalendar ? ConvertThemeToTodayStyle(renderCalendar->GetCalendarTheme()) : nullptr;
}

std::unique_ptr<JsonValue> CalendarComposedElement::GetWeekStyle() const
{
    auto renderCalendar = GetRenderCalendar();
    return renderCalendar ? ConvertThemeToWeekStyle(renderCalendar->GetCalendarTheme()) : nullptr;
}

std::unique_ptr<JsonValue> CalendarComposedElement::GetWorkStateStyle() const
{
    auto renderCalendar = GetRenderCalendar();
    return renderCalendar ? ConvertThemeToWorkStateStyle(renderCalendar->GetCalendarTheme()) : nullptr;
}

std::string CalendarComposedElement::GetAxis() const
{
    auto renderCalendar = GetRenderCalendar();
    auto axis = renderCalendar ? renderCalendar->GetAxis() : Axis::HORIZONTAL;
    switch (axis) {
        case Axis::VERTICAL:
            return "Axis.Vertical";
        case Axis::HORIZONTAL:
        default:
            return "Axis.Horizontal";
    }
    return ConvertAxisToString(axis);
}

std::string CalendarComposedElement::ConvertWeekToString(std::string week) const
{
    if (week == "0") {
        return "Mon";
    } else if (week == "1") {
        return "Tue";
    } else if (week == "2") {
        return "Wed";
    } else if (week == "3") {
        return "Thur";
    } else if (week == "4") {
        return "Fri";
    } else if (week == "5") {
        return "Sat";
    } else if (week == "6") {
        return "Sun";
    } else {
        return "Sun";
    }
}

std::string CalendarComposedElement::ConvertOffDayToString(std::string offDay) const
{
    std::string result = "Sun|Sat";
    if (!offDay.empty()) {
        offDay = offDay.substr(0, offDay.size() - 1);
        auto data = SplitString(offDay, ",");
        result = ConvertOffDayFamily(data);
    }
    return result;
}

std::vector<std::string> CalendarComposedElement::SplitString(const std::string& s, const std::string& c) const
{
    std::vector<std::string> v;
    std::string::size_type pos1, pos2;
    pos1 = 0;
    pos2 = s.find(c);
    while (std::string::npos != pos2) {
        v.push_back(s.substr(pos1, pos2 - pos1));
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length()) {
        v.push_back(s.substr(pos1));
    }
    return v;
}

std::string CalendarComposedElement::ConvertOffDayFamily(const std::vector<std::string>& data) const
{
    std::string result = "";
    for (const auto& item : data) {
        result += ConvertWeekToString(item);
        result += "|";
    }
    result = result.substr(0, result.size() - 1);
    return result;
}

RefPtr<RenderCalendar> CalendarComposedElement::GetRenderCalendar() const
{
    auto node = GetInspectorNode(CalendarElement::TypeId());
    if (node) {
        while (!AceType::DynamicCast<RenderCalendar>(node)) {
            node = node->GetChildren().front();
        }
        return AceType::DynamicCast<RenderCalendar>(node);
    }
    return nullptr;
}

std::unique_ptr<JsonValue> CalendarComposedElement::ConvertThemeToDayStyle(const CalendarThemeStructure& theme) const
{
    Color dayColor = theme.dayColor;
    Color lunarColor = theme.lunarColor;
    Color markLunarColor = theme.markLunarColor;
    Dimension dayFontSize = theme.dayFontSize;
    Dimension lunarDayFontSize = theme.lunarDayFontSize;
    Dimension dayHeight = theme.dayHeight;
    Dimension dayWidth = theme.dayWidth;
    Dimension gregorianCalendarHeight = theme.gregorianCalendarHeight;
    Dimension dayYAxisOffset = theme.dayYAxisOffset;
    Dimension lunarDayYAxisOffset = theme.lunarDayYAxisOffset;
    Dimension underscoreXAxisOffset = theme.underscoreXAxisOffset;
    Dimension underscoreYAxisOffset = theme.underscoreYAxisOffset;
    Dimension scheduleMarkerXAxisOffset = theme.scheduleMarkerXAxisOffset;
    Dimension scheduleMarkerYAxisOffset = theme.scheduleMarkerYAxisOffset;
    auto dayStyle = JsonUtil::Create(false);
    dayStyle->Put("dayColor", ConvertColorToString(dayColor).c_str());
    dayStyle->Put("lunarColor", ConvertColorToString(lunarColor).c_str());
    dayStyle->Put("markLunarColor", ConvertColorToString(markLunarColor).c_str());
    dayStyle->Put("dayFontSize", std::to_string(dayFontSize.Value()).c_str());
    dayStyle->Put("lunarDayFontSize", std::to_string(lunarDayFontSize.Value()).c_str());
    dayStyle->Put("dayHeight", std::to_string(dayHeight.Value()).c_str());
    dayStyle->Put("dayWidth", std::to_string(dayWidth.Value()).c_str());
    dayStyle->Put("gregorianCalendarHeight", std::to_string(gregorianCalendarHeight.Value()).c_str());
    dayStyle->Put("dayYAxisOffset", std::to_string(dayYAxisOffset.Value()).c_str());
    dayStyle->Put("lunarDayYAxisOffset", std::to_string(lunarDayYAxisOffset.Value()).c_str());
    dayStyle->Put("underscoreXAxisOffset", std::to_string(underscoreXAxisOffset.Value()).c_str());
    dayStyle->Put("underscoreYAxisOffset", std::to_string(underscoreYAxisOffset.Value()).c_str());
    dayStyle->Put("scheduleMarkerXAxisOffset", std::to_string(scheduleMarkerXAxisOffset.Value()).c_str());
    dayStyle->Put("scheduleMarkerYAxisOffset", std::to_string(scheduleMarkerYAxisOffset.Value()).c_str());
    return dayStyle;
}

std::unique_ptr<JsonValue> CalendarComposedElement::ConvertThemeToNonCurrentDayStyle(
    const CalendarThemeStructure& theme) const
{
    Color nonCurrentMonthDayColor = theme.nonCurrentMonthDayColor;
    Color nonCurrentMonthLunarColor = theme.nonCurrentMonthLunarColor;
    Color nonCurrentMonthWorkDayMarkColor = theme.nonCurrentMonthWorkDayMarkColor;
    Color nonCurrentMonthOffDayMarkColor = theme.nonCurrentMonthOffDayMarkColor;
    auto nonCurrentDayStyle = JsonUtil::Create(false);
    nonCurrentDayStyle->Put("nonCurrentMonthDayColor", ConvertColorToString(nonCurrentMonthDayColor).c_str());
    nonCurrentDayStyle->Put("nonCurrentMonthLunarColor", ConvertColorToString(nonCurrentMonthLunarColor).c_str());
    nonCurrentDayStyle->Put(
        "nonCurrentMonthWorkDayMarkColor", ConvertColorToString(nonCurrentMonthWorkDayMarkColor).c_str());
    nonCurrentDayStyle->Put(
        "nonCurrentMonthOffDayMarkColor", ConvertColorToString(nonCurrentMonthOffDayMarkColor).c_str());
    return nonCurrentDayStyle;
}

std::unique_ptr<JsonValue> CalendarComposedElement::ConvertThemeToTodayStyle(const CalendarThemeStructure& theme) const
{
    Color focusedDayColor = theme.focusedDayColor;
    Color focusedLunarColor = theme.focusedLunarColor;
    Color focusedAreaBackgroundColor = theme.focusedAreaBackgroundColor;
    Dimension focusedAreaRadius = theme.focusedAreaRadius;
    auto todayStyle = JsonUtil::Create(false);
    todayStyle->Put("focusedDayColor", ConvertColorToString(focusedDayColor).c_str());
    todayStyle->Put("focusedLunarColor", ConvertColorToString(focusedLunarColor).c_str());
    todayStyle->Put("focusedAreaBackgroundColor", ConvertColorToString(focusedAreaBackgroundColor).c_str());
    todayStyle->Put("focusedAreaRadius", std::to_string(focusedAreaRadius.Value()).c_str());
    return todayStyle;
}

std::unique_ptr<JsonValue> CalendarComposedElement::ConvertThemeToWeekStyle(const CalendarThemeStructure& theme) const
{
    Color weekColor = theme.weekColor;
    Color weekendDayColor = theme.weekendDayColor;
    Color weekendLunarColor = theme.weekendLunarColor;
    Dimension weekFontSize = theme.weekFontSize;
    Dimension weekHeight = theme.weekHeight;
    Dimension weekWidth = theme.weekWidth;
    Dimension weekAndDayRowSpace = theme.weekAndDayRowSpace;
    auto weekStyle = JsonUtil::Create(false);
    weekStyle->Put("weekColor", ConvertColorToString(weekColor).c_str());
    weekStyle->Put("weekendDayColor", ConvertColorToString(weekendDayColor).c_str());
    weekStyle->Put("weekendLunarColor", ConvertColorToString(weekendLunarColor).c_str());
    weekStyle->Put("weekFontSize", std::to_string(weekFontSize.Value()).c_str());
    weekStyle->Put("weekHeight", std::to_string(weekHeight.Value()).c_str());
    weekStyle->Put("weekWidth", std::to_string(weekWidth.Value()).c_str());
    weekStyle->Put("weekAndDayRowSpace", std::to_string(weekAndDayRowSpace.Value()).c_str());
    return weekStyle;
}

std::unique_ptr<JsonValue> CalendarComposedElement::ConvertThemeToWorkStateStyle(
    const CalendarThemeStructure& theme) const
{
    Color workDayMarkColor = theme.workDayMarkColor;
    Color offDayMarkColor = theme.offDayMarkColor;
    Dimension workDayMarkSize = theme.workDayMarkSize;
    Dimension offDayMarkSize = theme.offDayMarkSize;
    Dimension workStateWidth = theme.workStateWidth;
    Dimension workStateHorizontalMovingDistance = theme.workStateHorizontalMovingDistance;
    Dimension workStateVerticalMovingDistance = theme.workStateVerticalMovingDistance;
    auto workStateStyle = JsonUtil::Create(false);
    workStateStyle->Put("workDayMarkColor", ConvertColorToString(workDayMarkColor).c_str());
    workStateStyle->Put("offDayMarkColor", ConvertColorToString(offDayMarkColor).c_str());
    workStateStyle->Put("workDayMarkSize", std::to_string(workDayMarkSize.Value()).c_str());
    workStateStyle->Put("offDayMarkSize", std::to_string(offDayMarkSize.Value()).c_str());
    workStateStyle->Put("workStateWidth", std::to_string(workStateWidth.Value()).c_str());
    workStateStyle->Put(
        "workStateHorizontalMovingDistance", std::to_string(workStateHorizontalMovingDistance.Value()).c_str());
    workStateStyle->Put(
        "workStateVerticalMovingDistance", std::to_string(workStateVerticalMovingDistance.Value()).c_str());
    return workStateStyle;
}

} // namespace OHOS::Ace::V2
