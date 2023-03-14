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

#include "frameworks/bridge/declarative_frontend/jsview/js_calendar.h"

#include "core/common/ace_application_info.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_calendar_controller.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr int32_t CALENDAR_INVALID = -1;

} // namespace

void JSCalendar::JSBind(BindingTarget globalObj)
{
    JSClass<JSCalendar>::Declare("Calendar");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSCalendar>::StaticMethod("create", &JSCalendar::Create, opt);
    JSClass<JSCalendar>::StaticMethod("showLunar", &JSCalendar::SetShowLunar, opt);
    JSClass<JSCalendar>::StaticMethod("showHoliday", &JSCalendar::SetShowHoliday, opt);
    JSClass<JSCalendar>::StaticMethod("needSlide", &JSCalendar::SetNeedSlide, opt);
    JSClass<JSCalendar>::StaticMethod("startOfWeek", &JSCalendar::SetStartOfWeek, opt);
    JSClass<JSCalendar>::StaticMethod("offDays", &JSCalendar::SetOffDays, opt);
    JSClass<JSCalendar>::StaticMethod("onSelectChange", &JSCalendar::JsOnSelectedChange, opt);
    JSClass<JSCalendar>::StaticMethod("onRequestData", &JSCalendar::JsOnRequestData, opt);
    JSClass<JSCalendar>::StaticMethod("direction", &JSCalendar::SetDirection, opt);
    JSClass<JSCalendar>::StaticMethod("currentDayStyle", &JSCalendar::SetCurrentDayStyle, opt);
    JSClass<JSCalendar>::StaticMethod("nonCurrentDayStyle", &JSCalendar::SetNonCurrentDayStyle, opt);
    JSClass<JSCalendar>::StaticMethod("todayStyle", &JSCalendar::SetTodayStyle, opt);
    JSClass<JSCalendar>::StaticMethod("weekStyle", &JSCalendar::SetWeekStyle, opt);
    JSClass<JSCalendar>::StaticMethod("workStateStyle", &JSCalendar::SetWorkStateStyle, opt);
    JSClass<JSCalendar>::Inherit<JSViewAbstract>();
    JSClass<JSCalendar>::Bind<>(globalObj);
}

void JSCalendar::Create(const JSCallbackInfo& info)
{
    if (info.Length() != 1 || !info[0]->IsObject()) {
        return;
    }
    auto calendarComponent = AceType::MakeRefPtr<OHOS::Ace::CalendarComponentV2>("", "calendar");
    auto obj = JSRef<JSObject>::Cast(info[0]);
    auto date = JSRef<JSObject>::Cast(obj->GetProperty("date"));
    SetDate(date, calendarComponent);
    auto currentData = JSRef<JSObject>::Cast(obj->GetProperty("currentData"));
    SetCurrentData(currentData, calendarComponent);
    auto preData = JSRef<JSObject>::Cast(obj->GetProperty("preData"));
    SetPreData(preData, calendarComponent);
    auto nextData = JSRef<JSObject>::Cast(obj->GetProperty("nextData"));
    SetNextData(nextData, calendarComponent);
    auto controllerObj = obj->GetProperty("controller");
    if (controllerObj->IsObject()) {
        auto jsCalendarController = JSRef<JSObject>::Cast(controllerObj)->Unwrap<JSCalendarController>();
        if (jsCalendarController) {
            auto controllerV2 = jsCalendarController->GetController();
            calendarComponent->SetControllerV2(controllerV2);
        }
    }
    auto theme = GetTheme<CalendarTheme>();
    calendarComponent->SetCalendarTheme(theme);
    calendarComponent->SetV2Componenet(true);
    ViewStackProcessor::GetInstance()->Push(calendarComponent);
}

void JSCalendar::SetCalendarData(
    const JSRef<JSObject>& obj, MonthState monthState, const RefPtr<CalendarComponentV2>& component)
{
    if (!component) {
        LOGE("component is not valid");
        return;
    }

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    if (obj->IsUndefined()) {
        LOGE("obj is undefined");
        return;
    }
#endif

    auto yearValue = obj->GetProperty("year");
    auto monthValue = obj->GetProperty("month");
    auto arrayValue = obj->GetProperty("data");
    auto data = JsonUtil::ParseJsonString(arrayValue->ToString());
    if (!yearValue->IsNumber() || !monthValue->IsNumber() || !data->IsArray()) {
        return;
    }
    ObtainedMonth obtainedMonth;
    obtainedMonth.year = yearValue->ToNumber<int32_t>();
    obtainedMonth.month = monthValue->ToNumber<int32_t>();
    std::vector<CalendarDay> days;
    auto child = data->GetChild();
    while (child && child->IsValid()) {
        CalendarDay day;
        day.index = child->GetInt("index");
        day.lunarMonth = child->GetString("lunarMonth");
        day.lunarDay = child->GetString("lunarDay");
        day.dayMark = child->GetString("dayMark");
        day.dayMarkValue = child->GetString("dayMarkValue");
        day.month.year = child->GetInt("year");
        day.month.month = child->GetInt("month");
        day.day = child->GetInt("day");
        if (day.day == 1 && obtainedMonth.firstDayIndex == CALENDAR_INVALID) {
            obtainedMonth.firstDayIndex = day.index;
        }
        day.isFirstOfLunar = child->GetBool("isFirstOfLunar");
        day.hasSchedule = child->GetBool("hasSchedule");
        day.markLunarDay = child->GetBool("markLunarDay");
        days.emplace_back(std::move(day));
        child = child->GetNext();
    }
    obtainedMonth.days = days;
    component->SetCalendarData(obtainedMonth);
}

void JSCalendar::SetCardCalendar(bool cardCalendar)
{
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    component->SetCardCalendar(cardCalendar);
}

void JSCalendar::SetDate(const JSRef<JSObject>& obj, const RefPtr<CalendarComponentV2>& component)
{
    if (component) {
        auto yearValue = obj->GetProperty("year");
        auto monthValue = obj->GetProperty("month");
        auto dayValue = obj->GetProperty("day");
        if (!yearValue->IsNumber() || !monthValue->IsNumber() || !dayValue->IsNumber()) {
            return;
        }
        CalendarDay day;
        day.month.year = yearValue->ToNumber<int32_t>();
        day.month.month = monthValue->ToNumber<int32_t>();
        day.day = dayValue->ToNumber<int32_t>();
        component->SetCalendarDate(day);
    }
}

void JSCalendar::SetHolidays(const std::string& holidays)
{
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    component->SetHolidays(holidays);
}

void JSCalendar::SetOffDays(int32_t offDays)
{
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }
    uint32_t bit = 0b1;
    std::string result;
    const static int32_t dayOfWeek = 7;
    for (auto i = 0; i < dayOfWeek; ++i) {
        if (bit & static_cast<uint32_t>(offDays)) {
            result += std::to_string(i);
            result += ",";
        }
        bit <<= 1;
    }

    component->SetOffDays(result);
}

void JSCalendar::SetShowHoliday(bool showHoliday)
{
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    component->SetShowHoliday(showHoliday);
}

void JSCalendar::SetShowLunar(bool showLunar)
{
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    component->SetShowLunar(showLunar);
}

void JSCalendar::SetStartOfWeek(int32_t startOfWeek)
{
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    if (0 <= startOfWeek && startOfWeek < 7) {
        component->SetStartDayOfWeek(startOfWeek);
    }
}

void JSCalendar::SetNeedSlide(bool needSlide)
{
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }
    component->SetNeedSlide(needSlide);
}

void JSCalendar::SetWorkDays(const std::string& workDays)
{
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    component->SetWorkDays(workDays);
}

RefPtr<CalendarComponentV2> JSCalendar::GetComponent()
{
    auto stack = ViewStackProcessor::GetInstance();
    if (!stack) {
        return nullptr;
    }
    auto component = AceType::DynamicCast<CalendarComponentV2>(stack->GetMainComponent());
    if (AceApplicationInfo::GetInstance().IsRightToLeft()) {
        component->SetTextDirection(TextDirection::RTL);
    }
    return component;
}

void JSCalendar::JsOnSelectedChange(const JSCallbackInfo& info)
{
    auto component = GetComponent();
    if (info[0]->IsFunction() && component) {
        auto selectedChangeFuc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));
        EventMarker onSelectedChangeId(
            [execCtx = info.GetExecutionContext(), func = std::move(selectedChangeFuc)](const std::string& info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                std::vector<std::string> keys = { "year", "month", "day" };
                ACE_SCORING_EVENT("Calendar.onSelectedChange");
                func->Execute(keys, info);
            });
        component->SetSelectedChangeEvent(onSelectedChangeId);
    }
}

void JSCalendar::JsOnRequestData(const JSCallbackInfo& info)
{
    auto component = GetComponent();
    if (info[0]->IsFunction() && component) {
        auto requestDataFuc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));
        EventMarker onRequestDataId(
            [execCtx = info.GetExecutionContext(), func = std::move(requestDataFuc)](const std::string& info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                std::vector<std::string> keys = { "year", "month", "currentMonth", "currentYear", "monthState" };
                ACE_SCORING_EVENT("Calendar.onRequestData");
                func->Execute(keys, info);
            });
        component->SetRequestDataEvent(onRequestDataId);
    }
}

void JSCalendar::SetCurrentData(const JSRef<JSObject>& obj, const RefPtr<CalendarComponentV2>& component)
{
    SetCalendarData(obj, MonthState::CUR_MONTH, component);
}

void JSCalendar::SetPreData(const JSRef<JSObject>& obj, const RefPtr<CalendarComponentV2>& component)
{
    SetCalendarData(obj, MonthState::PRE_MONTH, component);
}

void JSCalendar::SetNextData(const JSRef<JSObject>& obj, const RefPtr<CalendarComponentV2>& component)
{
    SetCalendarData(obj, MonthState::NEXT_MONTH, component);
}

void JSCalendar::SetDirection(int32_t dir)
{
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }
    if (dir == 0) {
        component->SetAxis(Axis::VERTICAL);
    } else if (dir == 1) {
        component->SetAxis(Axis::HORIZONTAL);
    }
}

void JSCalendar::SetCurrentDayStyle(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGW("Invalid params");
        return;
    }
    auto component = GetComponent();
    if (!component) {
        return;
    }
    auto obj = JSRef<JSObject>::Cast(info[0]);
    auto& themePtr = component->GetCalendarTheme();
    if (!themePtr) {
        return;
    }
    auto& theme = themePtr->GetCalendarTheme();
    ConvertFromJSValue(obj->GetProperty("dayColor"), theme.dayColor);
    ConvertFromJSValue(obj->GetProperty("lunarColor"), theme.lunarColor);
    ConvertFromJSValue(obj->GetProperty("markLunarColor"), theme.markLunarColor);
    Dimension dayFontSize;
    if (ParseJsDimensionFp(obj->GetProperty("dayFontSize"), dayFontSize)) {
        theme.dayFontSize = dayFontSize;
    }
    Dimension lunarDayFontSize;
    if (ParseJsDimensionFp(obj->GetProperty("lunarDayFontSize"), lunarDayFontSize)) {
        theme.lunarDayFontSize = lunarDayFontSize;
    }
    ConvertFromJSValue(obj->GetProperty("dayHeight"), theme.dayHeight);
    ConvertFromJSValue(obj->GetProperty("dayWidth"), theme.dayWidth);
    ConvertFromJSValue(obj->GetProperty("gregorianCalendarHeight"), theme.gregorianCalendarHeight);
    ConvertFromJSValue(obj->GetProperty("lunarHeight"), theme.lunarHeight);
    ConvertFromJSValue(obj->GetProperty("dayYAxisOffset"), theme.dayYAxisOffset);
    ConvertFromJSValue(obj->GetProperty("lunarDayYAxisOffset"), theme.lunarDayYAxisOffset);
    ConvertFromJSValue(obj->GetProperty("underscoreXAxisOffset"), theme.underscoreXAxisOffset);
    ConvertFromJSValue(obj->GetProperty("underscoreYAxisOffset"), theme.underscoreYAxisOffset);
    ConvertFromJSValue(obj->GetProperty("scheduleMarkerXAxisOffset"), theme.scheduleMarkerXAxisOffset);
    ConvertFromJSValue(obj->GetProperty("scheduleMarkerYAxisOffset"), theme.scheduleMarkerYAxisOffset);
    ConvertFromJSValue(obj->GetProperty("colSpace"), theme.colSpace);
    ConvertFromJSValue(obj->GetProperty("dailyFiveRowSpace"), theme.dailyFiveRowSpace);
    ConvertFromJSValue(obj->GetProperty("dailySixRowSpace"), theme.dailySixRowSpace);
    ConvertFromJSValue(obj->GetProperty("underscoreWidth"), theme.underscoreWidth);
    ConvertFromJSValue(obj->GetProperty("underscoreLength"), theme.underscoreLength);
    ConvertFromJSValue(obj->GetProperty("scheduleMarkerRadius"), theme.scheduleMarkerRadius);
    ConvertFromJSValue(obj->GetProperty("boundaryRowOffset"), theme.boundaryRowOffset);
    ConvertFromJSValue(obj->GetProperty("boundaryColOffset"), theme.boundaryColOffset);
    ConvertFromJSValue(obj->GetProperty("touchCircleStrokeWidth"), theme.touchCircleStrokeWidth);
}

void JSCalendar::SetNonCurrentDayStyle(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGW("Invalid params");
        return;
    }
    auto component = GetComponent();
    if (!component) {
        return;
    }
    auto obj = JSRef<JSObject>::Cast(info[0]);
    auto& themePtr = component->GetCalendarTheme();
    if (!themePtr) {
        return;
    }
    auto& theme = themePtr->GetCalendarTheme();
    ConvertFromJSValue(obj->GetProperty("nonCurrentMonthDayColor"), theme.nonCurrentMonthDayColor);
    ConvertFromJSValue(obj->GetProperty("nonCurrentMonthLunarColor"), theme.nonCurrentMonthLunarColor);
    ConvertFromJSValue(obj->GetProperty("nonCurrentMonthWorkDayMarkColor"), theme.nonCurrentMonthWorkDayMarkColor);
    ConvertFromJSValue(obj->GetProperty("nonCurrentMonthOffDayMarkColor"), theme.nonCurrentMonthOffDayMarkColor);
}

void JSCalendar::SetTodayStyle(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGW("Invalid params");
        return;
    }
    auto component = GetComponent();
    if (!component) {
        return;
    }
    auto obj = JSRef<JSObject>::Cast(info[0]);
    auto& themePtr = component->GetCalendarTheme();
    if (!themePtr) {
        return;
    }
    auto& theme = themePtr->GetCalendarTheme();
    ConvertFromJSValue(obj->GetProperty("focusedDayColor"), theme.focusedDayColor);
    ConvertFromJSValue(obj->GetProperty("focusedLunarColor"), theme.focusedLunarColor);
    ConvertFromJSValue(obj->GetProperty("focusedAreaBackgroundColor"), theme.focusedAreaBackgroundColor);
    ConvertFromJSValue(obj->GetProperty("focusedAreaRadius"), theme.focusedAreaRadius);
}

void JSCalendar::SetWeekStyle(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGW("Invalid params");
        return;
    }
    auto component = GetComponent();
    if (!component) {
        return;
    }
    auto obj = JSRef<JSObject>::Cast(info[0]);
    auto& themePtr = component->GetCalendarTheme();
    if (!themePtr) {
        return;
    }
    auto& theme = themePtr->GetCalendarTheme();
    ConvertFromJSValue(obj->GetProperty("weekColor"), theme.weekColor);
    ConvertFromJSValue(obj->GetProperty("weekendDayColor"), theme.weekendDayColor);
    ConvertFromJSValue(obj->GetProperty("weekendLunarColor"), theme.weekendLunarColor);
    Dimension weekFontSize;
    if (ParseJsDimensionFp(obj->GetProperty("weekFontSize"), weekFontSize)) {
        theme.weekFontSize = weekFontSize;
    }
    ConvertFromJSValue(obj->GetProperty("weekHeight"), theme.weekHeight);
    ConvertFromJSValue(obj->GetProperty("weekWidth"), theme.weekWidth);
    ConvertFromJSValue(obj->GetProperty("weekAndDayRowSpace"), theme.weekAndDayRowSpace);
}

void JSCalendar::SetWorkStateStyle(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGW("Invalid params");
        return;
    }
    auto component = GetComponent();
    if (!component) {
        return;
    }
    auto obj = JSRef<JSObject>::Cast(info[0]);
    auto& themePtr = component->GetCalendarTheme();
    if (!themePtr) {
        return;
    }
    auto& theme = themePtr->GetCalendarTheme();
    ConvertFromJSValue(obj->GetProperty("workDayMarkColor"), theme.workDayMarkColor);
    ConvertFromJSValue(obj->GetProperty("offDayMarkColor"), theme.offDayMarkColor);
    ConvertFromJSValue(obj->GetProperty("workDayMarkSize"), theme.workDayMarkSize);
    ConvertFromJSValue(obj->GetProperty("offDayMarkSize"), theme.offDayMarkSize);
    ConvertFromJSValue(obj->GetProperty("workStateWidth"), theme.workStateWidth);
    ConvertFromJSValue(obj->GetProperty("workStateHorizontalMovingDistance"), theme.workStateHorizontalMovingDistance);
    ConvertFromJSValue(obj->GetProperty("workStateVerticalMovingDistance"), theme.workStateVerticalMovingDistance);
}

} // namespace OHOS::Ace::Framework
