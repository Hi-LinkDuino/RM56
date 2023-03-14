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

#include "frameworks/bridge/common/dom/dom_calendar.h"

#include "base/utils/string_utils.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr uint32_t METHOD_GO_TO_ARGS_SIZE = 1;
const char GO_TO_ARG_KEY_YEAR[] = "year";
const char GO_TO_ARG_KEY_MONTH[] = "month";
const char GO_TO_ARG_KEY_DAY[] = "day";

} // namespace

DomCalendar::DomCalendar(NodeId nodeId, const std::string& nodeName)
    : DOMNode(nodeId, nodeName),
      calendarComponent_(AceType::MakeRefPtr<CalendarComponent>(std::to_string(nodeId), nodeName))
{
    if (IsRightToLeft()) {
        calendarComponent_->SetTextDirection(TextDirection::RTL);
    }
}

bool DomCalendar::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<bool (*)(const std::string&, DomCalendar&)> calendarAttrOperators[] = {
        { DOM_CALENDAR_DATA,
            [](const std::string& value, DomCalendar& calendar) {
                calendar.calendarComponent_->SetCalendarData(value);
                return true;
            } },
        { DOM_CALENDAR_CARD_CALENDAR,
            [](const std::string& value, DomCalendar& calendar) {
                calendar.calendarComponent_->SetCardCalendar(StringToBool(value));
                return true;
            } },
        { DOM_CALENDAR_DATE,
            [](const std::string& value, DomCalendar& calendar) {
                CalendarDay day;
                auto isLegal = StringUtils::StringToCalendarDay(value, day);
                if (isLegal) {
                    calendar.calendarComponent_->SetCalendarDate(day);
                    return true;
                }
                return false;
            } },
        { DOM_CALENDAR_DATE_ADAPTER,
            [](const std::string& value, DomCalendar& calendar) { return calendar.ParseDataAdapter(value); } },
        { DOM_CALENDAR_DIRECTION,
            [](const std::string& value, DomCalendar& calendar) {
              if (value == "vertical") {
                  calendar.calendarComponent_->SetAxis(Axis::VERTICAL);
              } else if (value == "horizontal") {
                  calendar.calendarComponent_->SetAxis(Axis::HORIZONTAL);
              } else {
                  LOGE("input do not match any direction");
              }
              return true;
            } },
        { DOM_CALENDAR_HOLIDAYS,
            [](const std::string& value, DomCalendar& calendar) {
                calendar.calendarComponent_->SetHolidays(value);
                return true;
            } },
        { DOM_CALENDAR_OFF_DAYS,
            [](const std::string& value, DomCalendar& calendar) {
                calendar.calendarComponent_->SetOffDays(value);
                return true;
            } },
        { DOM_CALENDAR_SHOW_HOLIDAY,
            [](const std::string& value, DomCalendar& calendar) {
                calendar.calendarComponent_->SetShowHoliday(StringToBool(value));
                return true;
            } },
        { DOM_CALENDAR_SHOW_LUNAR,
            [](const std::string& value, DomCalendar& calendar) {
                calendar.calendarComponent_->SetShowLunar(StringToBool(value));
                return true;
            } },
        { DOM_CALENDAR_START_DAY_OF_WEEK,
            [](const std::string& value, DomCalendar& calendar) {
                auto indexOfWeek = StringToInt(value);
                if (0 <= indexOfWeek && indexOfWeek < 7) {
                    calendar.calendarComponent_->SetStartDayOfWeek(indexOfWeek);
                    return true;
                }
                return false;
            } },
        { DOM_CALENDAR_TYPE,
            [](const std::string& value, DomCalendar& calendar) {
                if (value == "normal") {
                    calendar.calendarComponent_->SetCalendarType(CalendarType::NORMAL);
                } else if (value == "simple") {
                    calendar.calendarComponent_->SetCalendarType(CalendarType::SIMPLE);
                }
                return true;
            } },
        { DOM_CALENDAR_WORK_DAYS,
            [](const std::string& value, DomCalendar& calendar) {
                calendar.calendarComponent_->SetWorkDays(value);
                return true;
            } },
    };
    auto operatorIter =
        BinarySearchFindIndex(calendarAttrOperators, ArraySize(calendarAttrOperators), attr.first.c_str());
    if (operatorIter != -1) {
        return calendarAttrOperators[operatorIter].value(attr.second, *this);
    }
    return false;
}

void DomCalendar::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    if (method == DOM_CALENDAR_METHOD_GO_TO) {
        HandleGoTo(args);
    }
}

bool DomCalendar::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    if (event == DOM_CALENDAR_EVENT_SELECTED_CHANGE) {
        selectedChangeEvent_ = EventMarker(GetNodeIdForEvent(), event, pageId);
        calendarComponent_->SetSelectedChangeEvent(selectedChangeEvent_);
        return true;
    } else if (event == DOM_CALENDAR_EVENT_REQUEST_DATA) {
        requestDataEvent_ = EventMarker(GetNodeIdForEvent(), event, pageId);
        calendarComponent_->SetRequestDataEvent(requestDataEvent_);
        return true;
    }
    return false;
}

bool DomCalendar::ParseDataAdapter(const std::string& value)
{
    std::unique_ptr<JsonValue> dataAdapterValue = JsonUtil::ParseJsonString(value);
    if (!dataAdapterValue) {
        LOGE("data adapter format is error");
        return false;
    }
    std::unique_ptr<JsonValue> bundleNameValue = dataAdapterValue->GetValue("bundleName");
    if (!bundleNameValue || !bundleNameValue->IsString()) {
        LOGE("get bundleName failed");
        return false;
    }
    std::unique_ptr<JsonValue> abilityNameValue = dataAdapterValue->GetValue("abilityName");
    if (!abilityNameValue || !abilityNameValue->IsString()) {
        LOGE("get abilityName failed");
        return false;
    }
    std::unique_ptr<JsonValue> messageCodeValue = dataAdapterValue->GetValue("messageCode");
    if (!messageCodeValue || !messageCodeValue->IsNumber()) {
        LOGE("get messageCode failed");
        return false;
    }
    std::string bundleName = bundleNameValue->GetString();
    std::string abilityName = abilityNameValue->GetString();
    int32_t messageCode = messageCodeValue->GetInt();
    CalendarDataAdapterAction dataAdapterAction {
        { .bundleName = bundleName, .abilityName = abilityName, .messageCode = messageCode }
    };
    calendarComponent_->SetDataAdapterAction(dataAdapterAction);
    return true;
}

void DomCalendar::HandleGoTo(const std::string& args)
{
    std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
    if (!argsValue || !argsValue->IsArray() || argsValue->GetArraySize() != METHOD_GO_TO_ARGS_SIZE) {
        LOGE("parse args error: %{private}s", args.c_str());
        return;
    }
    auto gotoArg = argsValue->GetArrayItem(0);
    if (!gotoArg || !gotoArg->Contains(GO_TO_ARG_KEY_YEAR) || !gotoArg->Contains(GO_TO_ARG_KEY_MONTH)) {
        LOGE("calendar goto arg no year or month");
        return;
    }

    std::unique_ptr<JsonValue> yearValue = gotoArg->GetValue(GO_TO_ARG_KEY_YEAR);
    if (!yearValue || !yearValue->IsNumber()) {
        LOGE("get year failed");
        return;
    }

    std::unique_ptr<JsonValue> monthValue = gotoArg->GetValue(GO_TO_ARG_KEY_MONTH);
    if (!monthValue || !monthValue->IsNumber()) {
        LOGE("get month failed");
        return;
    }
    int32_t year = yearValue->GetInt();
    int32_t month = monthValue->GetInt();
    // default selected first day of month
    int32_t day = -1;

    std::unique_ptr<JsonValue> dayValue = gotoArg->GetValue(GO_TO_ARG_KEY_DAY);
    if (dayValue && dayValue->IsNumber()) {
        day = dayValue->GetInt();
    }
    calendarComponent_->GoTo(year, month, day);
}

bool DomCalendar::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<void (*)(const std::string&, CalendarThemeStructure&, const DomCalendar&)>
        calendarStyleOperators[] = {
            { "boundaryColumnOffset",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.boundaryColOffset = node.ParseDimension(value);
                } },
            { "boundaryRowOffset",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.boundaryRowOffset = node.ParseDimension(value);
                } },
            { "columnSpace", [](const std::string& value, CalendarThemeStructure& theme,
                                 const DomCalendar& node) { theme.colSpace = node.ParseDimension(value); } },
            { "dailyFiveRowSpace",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.dailyFiveRowSpace = node.ParseDimension(value);
                } },
            { "dailySixRowSpace",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.dailySixRowSpace = node.ParseDimension(value);
                } },
            { "dayColor", [](const std::string& value, CalendarThemeStructure& theme,
                              const DomCalendar& node) { theme.dayColor = node.ParseColor(value); } },
            { "dayFontSize", [](const std::string& value, CalendarThemeStructure& theme,
                                 const DomCalendar& node) { theme.dayFontSize = node.ParseDimension(value); } },
            { "dayHeight", [](const std::string& value, CalendarThemeStructure& theme,
                               const DomCalendar& node) { theme.dayHeight = node.ParseDimension(value); } },
            { "dayWidth", [](const std::string& value, CalendarThemeStructure& theme,
                              const DomCalendar& node) { theme.dayWidth = node.ParseDimension(value); } },
            { "dayYAxisOffset", [](const std::string& value, CalendarThemeStructure& theme,
                                    const DomCalendar& node) { theme.dayYAxisOffset = node.ParseDimension(value); } },
            { "focusedAreaBackgroundColor",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.focusedAreaBackgroundColor = node.ParseColor(value);
                } },
            { "focusedAreaRadius",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.focusedAreaRadius = node.ParseDimension(value);
                } },
            { "focusedDayColor", [](const std::string& value, CalendarThemeStructure& theme,
                                     const DomCalendar& node) { theme.focusedDayColor = node.ParseColor(value); } },
            { "focusedLunarColor", [](const std::string& value, CalendarThemeStructure& theme,
                                       const DomCalendar& node) { theme.focusedLunarColor = node.ParseColor(value); } },
            { "gregorianCalendarHeight",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.gregorianCalendarHeight = node.ParseDimension(value);
                } },
            { "lunarColor", [](const std::string& value, CalendarThemeStructure& theme,
                                const DomCalendar& node) { theme.lunarColor = node.ParseColor(value); } },
            { "lunarDayFontSize",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.lunarDayFontSize = node.ParseDimension(value);
                } },
            { "lunarDayYAxisOffset",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.lunarDayYAxisOffset = node.ParseDimension(value);
                } },
            { "lunarHeight", [](const std::string& value, CalendarThemeStructure& theme,
                                 const DomCalendar& node) { theme.lunarHeight = node.ParseDimension(value); } },
            { "markLunarColor", [](const std::string& value, CalendarThemeStructure& theme,
                                    const DomCalendar& node) { theme.markLunarColor = node.ParseColor(value); } },
            { "nonCurrentMonthDayColor",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.nonCurrentMonthDayColor = node.ParseColor(value);
                } },
            { "nonCurrentMonthLunarColor",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.nonCurrentMonthLunarColor = node.ParseColor(value);
                } },
            { "nonCurrentMonthOffDayMarkColor",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.nonCurrentMonthOffDayMarkColor = node.ParseColor(value);
                } },
            { "nonCurrentMonthWorkDayMarkColor",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.nonCurrentMonthWorkDayMarkColor = node.ParseColor(value);
                } },
            { "offDayMarkColor", [](const std::string& value, CalendarThemeStructure& theme,
                                     const DomCalendar& node) { theme.offDayMarkColor = node.ParseColor(value); } },
            { "offDayMarkSize", [](const std::string& value, CalendarThemeStructure& theme,
                                    const DomCalendar& node) { theme.offDayMarkSize = node.ParseDimension(value); } },
            { "scheduleMarkerRadius",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.scheduleMarkerRadius = node.ParseDimension(value);
                } },
            { "scheduleMarkerXAxisOffset",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.scheduleMarkerXAxisOffset = node.ParseDimension(value);
                } },
            { "scheduleMarkerYAxisOffset",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.scheduleMarkerYAxisOffset = node.ParseDimension(value);
                } },
            { "simpleOffTextColor",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.simpleOffTextColor = node.ParseColor(value);
                } },
            { "simpleWorkTextColor",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.simpleWorkTextColor = node.ParseColor(value);
                } },
            { "underscoreLength",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.underscoreLength = node.ParseDimension(value);
                } },
            { "underscoreWidth", [](const std::string& value, CalendarThemeStructure& theme,
                                     const DomCalendar& node) { theme.underscoreWidth = node.ParseDimension(value); } },
            { "underscoreXAxisOffset",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.underscoreXAxisOffset = node.ParseDimension(value);
                } },
            { "underscoreYAxisOffset",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.underscoreYAxisOffset = node.ParseDimension(value);
                } },
            { "weekAndDayRowSpace",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.weekAndDayRowSpace = node.ParseDimension(value);
                } },
            { "weekColor", [](const std::string& value, CalendarThemeStructure& theme,
                               const DomCalendar& node) { theme.weekColor = node.ParseColor(value); } },
            { "weekFontSize", [](const std::string& value, CalendarThemeStructure& theme,
                                  const DomCalendar& node) { theme.weekFontSize = node.ParseDimension(value); } },
            { "weekHeight", [](const std::string& value, CalendarThemeStructure& theme,
                                const DomCalendar& node) { theme.weekHeight = node.ParseDimension(value); } },
            { "weekWidth", [](const std::string& value, CalendarThemeStructure& theme,
                               const DomCalendar& node) { theme.weekWidth = node.ParseDimension(value); } },
            { "weekendDayColor", [](const std::string& value, CalendarThemeStructure& theme,
                                     const DomCalendar& node) { theme.weekendDayColor = node.ParseColor(value); } },
            { "weekendLunarColor", [](const std::string& value, CalendarThemeStructure& theme,
                                       const DomCalendar& node) { theme.weekendLunarColor = node.ParseColor(value); } },
            { "workDayMarkColor", [](const std::string& value, CalendarThemeStructure& theme,
                                      const DomCalendar& node) { theme.workDayMarkColor = node.ParseColor(value); } },
            { "workDayMarkSize", [](const std::string& value, CalendarThemeStructure& theme,
                                     const DomCalendar& node) { theme.workDayMarkSize = node.ParseDimension(value); } },
            { "workStateHorizontalMovingDistance",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.workStateHorizontalMovingDistance = node.ParseDimension(value);
                } },
            { "workStateOffset", [](const std::string& value, CalendarThemeStructure& theme,
                                     const DomCalendar& node) { theme.workStateOffset = node.ParseDimension(value); } },
            { "workStateVerticalMovingDistance",
                [](const std::string& value, CalendarThemeStructure& theme, const DomCalendar& node) {
                    theme.workStateVerticalMovingDistance = node.ParseDimension(value);
                } },
            { "workStateWidth", [](const std::string& value, CalendarThemeStructure& theme,
                                    const DomCalendar& node) { theme.workStateWidth = node.ParseDimension(value); } },
        };
    auto context = GetPipelineContext().Upgrade();
    if (!context) {
        return false;
    }
    auto theme = calendarComponent_->GetCalendarTheme();
    if (!theme) {
        theme = GetTheme<CalendarTheme>();
        calendarComponent_->SetCalendarTheme(theme);
    }
    auto operatorIter =
        BinarySearchFindIndex(calendarStyleOperators, ArraySize(calendarStyleOperators), style.first.c_str());
    if (operatorIter != -1) {
        auto& calendarTheme = context->IsJsCard() ? theme->GetCardCalendarTheme() : theme->GetCalendarTheme();
        calendarStyleOperators[operatorIter].value(style.second, calendarTheme, *this);
        return true;
    }
    return false;
}

} // namespace OHOS::Ace::Framework
