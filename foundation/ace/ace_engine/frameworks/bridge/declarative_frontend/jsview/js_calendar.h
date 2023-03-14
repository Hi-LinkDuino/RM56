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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_CALENDAR_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_CALENDAR_H

#include "core/components/calendar/calendar_component_v2.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {

class JSCalendar : public JSViewAbstract, public JSInteractableView {
public:
    static void Create(const JSCallbackInfo& info);
    static void JSBind(BindingTarget globalObj);

    static void SetCalendarData(
        const JSRef<JSObject>& obj, MonthState monthState, const RefPtr<CalendarComponentV2>& component);
    static void SetCurrentData(const JSRef<JSObject>& obj, const RefPtr<CalendarComponentV2>& component);
    static void SetPreData(const JSRef<JSObject>& obj, const RefPtr<CalendarComponentV2>& component);
    static void SetNextData(const JSRef<JSObject>& obj, const RefPtr<CalendarComponentV2>& component);
    static void SetCardCalendar(bool cardCalendar);
    static void SetDate(const JSRef<JSObject>& obj, const RefPtr<CalendarComponentV2>& component);
    static void SetHolidays(const std::string& holidays);
    static void SetOffDays(int32_t offDays);
    static void SetShowHoliday(bool showHoliday);
    static void SetShowLunar(bool showLunar);
    static void SetNeedSlide(bool needSlide);
    static void SetStartOfWeek(int32_t startOfWeek);
    static void SetWorkDays(const std::string& workDays);
    static void SetDirection(int32_t dir);
    static void SetCurrentDayStyle(const JSCallbackInfo& info);
    static void SetNonCurrentDayStyle(const JSCallbackInfo& info);
    static void SetTodayStyle(const JSCallbackInfo& info);
    static void SetWeekStyle(const JSCallbackInfo& info);
    static void SetWorkStateStyle(const JSCallbackInfo& info);
    static void JsOnSelectedChange(const JSCallbackInfo& info);
    static void JsOnRequestData(const JSCallbackInfo& info);

private:
    static RefPtr<CalendarComponentV2> GetComponent();
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_CALENDAR_H
