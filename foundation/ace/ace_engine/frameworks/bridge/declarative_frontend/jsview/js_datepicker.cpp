/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "bridge/declarative_frontend/jsview/js_datepicker.h"
#include "bridge/declarative_frontend/jsview/js_interactable_view.h"

#include "bridge/declarative_frontend/engine/functions/js_function.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components/picker/picker_date_component.h"
#include "core/components/picker/picker_theme.h"
#include "core/components/picker/picker_time_component.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace::Framework {
namespace {
void AddEvent(RefPtr<PickerBaseComponent>& picker, const JSCallbackInfo& info, DatePickerType pickerType)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("DatePicker AddEvent error, info is non-valid");
        return;
    }
    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto onAccept = paramObject->GetProperty("onAccept");
    if (!onAccept->IsUndefined() && onAccept->IsFunction()) {
        auto jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(onAccept));
        auto acceptId =
            EventMarker([execCtx = info.GetExecutionContext(), func = std::move(jsFunc)](const std::string& info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                std::vector<std::string> keys = { "year", "month", "day", "hour", "minute", "second"};
                ACE_SCORING_EVENT("DatePickerDialog.onAccept");
                func->Execute(keys, info);
            });
        picker->SetDialogAcceptEvent(acceptId);
    }
    auto onCancel = paramObject->GetProperty("onCancel");
    if (!onCancel->IsUndefined() && onCancel->IsFunction()) {
        auto jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(onCancel));
        auto cancelId =
            EventMarker([execCtx = info.GetExecutionContext(), func = std::move(jsFunc)]() {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                ACE_SCORING_EVENT("DatePickerDialog.onCancel");
                func->Execute();
            });
        picker->SetDialogCancelEvent(cancelId);
    }
    auto onChange = paramObject->GetProperty("onChange");
    if (!onChange->IsUndefined() && onChange->IsFunction()) {
        auto jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(onChange));
        auto changeId =
            EventMarker([execCtx = info.GetExecutionContext(),
                type = pickerType, func = std::move(jsFunc)](const std::string& info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                std::vector<std::string> keys;
                if (type == DatePickerType::DATE) {
                    keys = { "year", "month", "day"};
                } else {
                    keys = {"hour", "minute"};
                }
                ACE_SCORING_EVENT("DatePickerDialog.onChange");
                func->Execute(keys, info);
            });
        picker->SetDialogChangeEvent(changeId);
    }
}

JSRef<JSVal> DatePickerChangeEventToJSValue(const DatePickerChangeEvent& eventInfo)
{
    JSRef<JSObject> obj = JSRef<JSObject>::New();
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(eventInfo.GetSelectedStr());
    if (!argsPtr) {
        LOGW("selectedStr is not exist.");
        return JSRef<JSVal>::Cast(obj);
    }
    std::vector<std::string> keys = { "year", "month", "day", "hour", "minute", "second" };
    for (auto iter = keys.begin(); iter != keys.end(); iter++) {
        const std::string key = *iter;
        const auto value = argsPtr->GetValue(key);
        if (!value || value->ToString().empty()) {
            LOGI("key[%{public}s] is not exist.", key.c_str());
            continue;
        }
        obj->SetProperty<std::string>(key.c_str(), value->ToString().c_str());
    }
    return JSRef<JSVal>::Cast(obj);
}
}

void JSDatePicker::JSBind(BindingTarget globalObj)
{
    JSClass<JSDatePicker>::Declare("DatePicker");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSDatePicker>::StaticMethod("create", &JSDatePicker::Create, opt);
    JSClass<JSDatePicker>::StaticMethod("lunar", &JSDatePicker::SetLunar);
    JSClass<JSDatePicker>::StaticMethod("onChange", &JSDatePicker::OnChange);
    // keep compatible, need remove after
    JSClass<JSDatePicker>::StaticMethod("useMilitaryTime", &JSDatePicker::UseMilitaryTime);
    JSClass<JSDatePicker>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSDatePicker>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSDatePicker>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSDatePicker>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSDatePicker>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSDatePicker>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSDatePicker>::Inherit<JSViewAbstract>();
    JSClass<JSDatePicker>::Bind(globalObj);
}

void JSDatePicker::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("DatePicker create error, info is non-valid");
        return;
    }

    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    DatePickerType pickerType = DatePickerType::DATE;
    auto type = paramObject->GetProperty("type");
    if (type->IsNumber()) {
        pickerType = static_cast<DatePickerType>(type->ToNumber<int32_t>());
    }
    switch (pickerType) {
        case DatePickerType::TIME: {
            CreateTimePicker(paramObject);
            break;
        }
        case DatePickerType::DATE: {
            CreateDatePicker(paramObject);
            break;
        }
        default: {
            LOGE("Undefined date picker type.");
            break;
        }
    }
}

void JSDatePicker::SetLunar(bool isLunar)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto datePicker = AceType::DynamicCast<PickerDateComponent>(component);
    if (!datePicker) {
        LOGE("PickerDateComponent is null");
        return;
    }
    datePicker->SetShowLunar(isLunar);
}

void JSDatePicker::UseMilitaryTime(bool isUseMilitaryTime)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto timePicker = AceType::DynamicCast<PickerTimeComponent>(component);
    if (!timePicker) {
        LOGE("PickerTimeComponent is null");
        return;
    }
    timePicker->SetHour24(isUseMilitaryTime);
}

void JSDatePicker::OnChange(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsFunction()) {
        LOGI("info not function");
        return;
    }

    auto jsFunc = AceType::MakeRefPtr<JsEventFunction<DatePickerChangeEvent, 1>>(
        JSRef<JSFunc>::Cast(info[0]), DatePickerChangeEventToJSValue);
    auto datePicker = AceType::DynamicCast<PickerBaseComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    datePicker->SetOnChange(EventMarker(
        [execCtx = info.GetExecutionContext(), func = std::move(jsFunc)](const BaseEventInfo* info) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            ACE_SCORING_EVENT("datePicker.onChange");
            auto eventInfo = TypeInfoHelper::DynamicCast<DatePickerChangeEvent>(info);
            func->Execute(*eventInfo);
        }));
}

PickerDate JSDatePicker::ParseDate(const JSRef<JSVal>& dateVal)
{
    auto pickerDate = PickerDate();
    if (!dateVal->IsObject()) {
        return pickerDate;
    }
    auto dateObj = JSRef<JSObject>::Cast(dateVal);
    auto yearFunc = JSRef<JSFunc>::Cast(dateObj->GetProperty("getFullYear"));
    auto monthFunc = JSRef<JSFunc>::Cast(dateObj->GetProperty("getMonth"));
    auto dateFunc = JSRef<JSFunc>::Cast(dateObj->GetProperty("getDate"));
    JSRef<JSVal> year = yearFunc->Call(dateObj);
    JSRef<JSVal> month = monthFunc->Call(dateObj);
    JSRef<JSVal> date = dateFunc->Call(dateObj);

    if (year->IsNumber() && month->IsNumber() && date->IsNumber()) {
        pickerDate.SetYear(year->ToNumber<int32_t>());
        pickerDate.SetMonth(month->ToNumber<int32_t>() + 1); // 0-11 means 1 to 12 months
        pickerDate.SetDay(date->ToNumber<int32_t>());
    }
    return pickerDate;
}

PickerTime JSDatePicker::ParseTime(const JSRef<JSVal>& timeVal)
{
    auto pickerTime = PickerTime();
    if (!timeVal->IsObject()) {
        return pickerTime;
    }
    auto timeObj = JSRef<JSObject>::Cast(timeVal);
    auto hourFunc = JSRef<JSFunc>::Cast(timeObj->GetProperty("getHours"));
    auto minuteFunc = JSRef<JSFunc>::Cast(timeObj->GetProperty("getMinutes"));
    auto secondFunc = JSRef<JSFunc>::Cast(timeObj->GetProperty("getSeconds"));
    JSRef<JSVal> hour = hourFunc->Call(timeObj);
    JSRef<JSVal> minute = minuteFunc->Call(timeObj);
    JSRef<JSVal> second = secondFunc->Call(timeObj);

    if (hour->IsNumber() && minute->IsNumber() && second->IsNumber()) {
        pickerTime.SetHour(hour->ToNumber<int32_t>());
        pickerTime.SetMinute(minute->ToNumber<int32_t>());
        pickerTime.SetSecond(second->ToNumber<int32_t>());
    }
    return pickerTime;
}

void JSDatePicker::CreateDatePicker(const JSRef<JSObject>& paramObj)
{
    auto datePicker = AceType::MakeRefPtr<PickerDateComponent>();
    auto startDate = paramObj->GetProperty("start");
    auto endDate = paramObj->GetProperty("end");
    auto selectedDate = paramObj->GetProperty("selected");
    auto parseStartDate = ParseDate(startDate);
    auto parseEndDate = ParseDate(endDate);
    auto parseSelectedDate = ParseDate(selectedDate);
    auto startDays = parseStartDate.ToDays();
    auto endDays = parseEndDate.ToDays();
    auto selectedDays = parseSelectedDate.ToDays();
    if (startDays > endDays || selectedDays < startDays || selectedDays > endDays) {
        LOGE("date error");
    }
    if (startDate->IsObject()) {
        datePicker->SetStartDate(parseStartDate);
    }
    if (endDate->IsObject()) {
        datePicker->SetEndDate(parseEndDate);
    }
    if (selectedDate->IsObject()) {
        datePicker->SetSelectedDate(parseSelectedDate);
    }

    datePicker->SetIsDialog(false);
    datePicker->SetHasButtons(false);

    auto theme = GetTheme<PickerTheme>();
    if (!theme) {
        LOGE("datePicker Theme is null");
        return;
    }

    datePicker->SetTheme(theme);
    ViewStackProcessor::GetInstance()->Push(datePicker);
}

void JSDatePicker::CreateTimePicker(const JSRef<JSObject>& paramObj)
{
    auto timePicker = AceType::MakeRefPtr<PickerTimeComponent>();
    auto selectedTime = paramObj->GetProperty("selected");
    if (selectedTime->IsObject()) {
        timePicker->SetSelectedTime(ParseTime(selectedTime));
    }
    timePicker->SetIsDialog(false);
    timePicker->SetHasButtons(false);

    auto theme = GetTheme<PickerTheme>();
    if (!theme) {
        LOGE("timePicker Theme is null");
        return;
    }

    timePicker->SetTheme(theme);
    ViewStackProcessor::GetInstance()->Push(timePicker);
}

void JSDatePickerDialog::JSBind(BindingTarget globalObj)
{
    JSClass<JSDatePickerDialog>::Declare("DatePickerDialog");
    JSClass<JSDatePickerDialog>::StaticMethod("show", &JSDatePickerDialog::Show);

    JSClass<JSDatePickerDialog>::Bind<>(globalObj);
}

void JSDatePickerDialog::Show(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("DatePicker Show dialog error, info is non-valid");
        return;
    }

    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    DatePickerType pickerType = DatePickerType::DATE;
    auto type = paramObject->GetProperty("type");
    if (type->IsNumber()) {
        pickerType = static_cast<DatePickerType>(type->ToNumber<int32_t>());
    }

    std::string name;
    RefPtr<Component> component;
    switch (pickerType) {
        case DatePickerType::TIME: {
            CreateTimePicker(component, paramObject);
            name = "TimePickerDialog";
            break;
        }
        case DatePickerType::DATE: {
            CreateDatePicker(component, paramObject);
            name = "DatePickerDialog";
            break;
        }
        default: {
            LOGE("Undefined date picker type.");
            return;
        }
    }

    auto datePicker = AceType::DynamicCast<PickerBaseComponent>(component);
    DialogProperties properties {};
    properties.alignment = DialogAlignment::CENTER;
    properties.customComponent = datePicker;

    if (pickerType == DatePickerType::DATE) {
        AddEvent(datePicker, info, DatePickerType::DATE);
    } else {
        AddEvent(datePicker, info, DatePickerType::TIME);
    }
    datePicker->SetDialogName(name);
    datePicker->OpenDialog(properties);
}

void JSDatePickerDialog::CreateDatePicker(RefPtr<Component> &component, const JSRef<JSObject>& paramObj)
{
    auto datePicker = AceType::MakeRefPtr<PickerDateComponent>();
    auto startDate = paramObj->GetProperty("start");
    auto endDate = paramObj->GetProperty("end");
    auto selectedDate = paramObj->GetProperty("selected");
    auto lunar = paramObj->GetProperty("lunar");
    bool isLunar = lunar->ToBoolean();
    auto parseStartDate = ParseDate(startDate);
    auto parseEndDate = ParseDate(endDate);
    auto parseSelectedDate = ParseDate(selectedDate);
    auto startDays = parseStartDate.ToDays();
    auto endDays = parseEndDate.ToDays();
    auto selectedDays = parseSelectedDate.ToDays();
    if (startDays > endDays || selectedDays < startDays || selectedDays > endDays) {
        LOGE("date error");
    }
    if (startDate->IsObject()) {
        datePicker->SetStartDate(parseStartDate);
    }
    if (endDate->IsObject()) {
        datePicker->SetEndDate(parseEndDate);
    }
    if (selectedDate->IsObject()) {
        datePicker->SetSelectedDate(parseSelectedDate);
    }
    datePicker->SetIsDialog(false);
    datePicker->SetIsCreateDialogComponent(true);
    datePicker->SetShowLunar(isLunar);

    component = datePicker;
}

void JSDatePickerDialog::CreateTimePicker(RefPtr<Component> &component, const JSRef<JSObject>& paramObj)
{
    auto timePicker = AceType::MakeRefPtr<PickerTimeComponent>();
    auto selectedTime = paramObj->GetProperty("selected");
    auto useMilitaryTime = paramObj->GetProperty("useMilitaryTime");
    bool isUseMilitaryTime = useMilitaryTime->ToBoolean();
    if (selectedTime->IsObject()) {
        timePicker->SetSelectedTime(ParseTime(selectedTime));
    }
    timePicker->SetIsDialog(false);
    timePicker->SetIsCreateDialogComponent(true);
    timePicker->SetHour24(isUseMilitaryTime);
    component = timePicker;
}

PickerDate JSDatePickerDialog::ParseDate(const JSRef<JSVal>& dateVal)
{
    auto pickerDate = PickerDate();
    if (!dateVal->IsObject()) {
        return pickerDate;
    }
    auto dateObj = JSRef<JSObject>::Cast(dateVal);
    auto yearFunc = JSRef<JSFunc>::Cast(dateObj->GetProperty("getFullYear"));
    auto monthFunc = JSRef<JSFunc>::Cast(dateObj->GetProperty("getMonth"));
    auto dateFunc = JSRef<JSFunc>::Cast(dateObj->GetProperty("getDate"));
    JSRef<JSVal> year = yearFunc->Call(dateObj);
    JSRef<JSVal> month = monthFunc->Call(dateObj);
    JSRef<JSVal> date = dateFunc->Call(dateObj);

    if (year->IsNumber() && month->IsNumber() && date->IsNumber()) {
        pickerDate.SetYear(year->ToNumber<int32_t>());
        pickerDate.SetMonth(month->ToNumber<int32_t>() + 1); // 0-11 means 1 to 12 months
        pickerDate.SetDay(date->ToNumber<int32_t>());
    }
    return pickerDate;
}

PickerTime JSDatePickerDialog::ParseTime(const JSRef<JSVal>& timeVal)
{
    auto pickerTime = PickerTime();
    if (!timeVal->IsObject()) {
        return pickerTime;
    }
    auto timeObj = JSRef<JSObject>::Cast(timeVal);
    auto hourFunc = JSRef<JSFunc>::Cast(timeObj->GetProperty("getHours"));
    auto minuteFunc = JSRef<JSFunc>::Cast(timeObj->GetProperty("getMinutes"));
    auto secondFunc = JSRef<JSFunc>::Cast(timeObj->GetProperty("getSeconds"));
    JSRef<JSVal> hour = hourFunc->Call(timeObj);
    JSRef<JSVal> minute = minuteFunc->Call(timeObj);
    JSRef<JSVal> second = secondFunc->Call(timeObj);

    if (hour->IsNumber() && minute->IsNumber() && second->IsNumber()) {
        pickerTime.SetHour(hour->ToNumber<int32_t>());
        pickerTime.SetMinute(minute->ToNumber<int32_t>());
        pickerTime.SetSecond(second->ToNumber<int32_t>());
    }
    return pickerTime;
}

void JSTimePicker::JSBind(BindingTarget globalObj)
{
    JSClass<JSTimePicker>::Declare("TimePicker");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSTimePicker>::StaticMethod("create", &JSTimePicker::Create, opt);
    JSClass<JSTimePicker>::StaticMethod("onChange", &JSDatePicker::OnChange);
    JSClass<JSTimePicker>::StaticMethod("useMilitaryTime", &JSTimePicker::UseMilitaryTime);
    JSClass<JSTimePicker>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSTimePicker>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSTimePicker>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSTimePicker>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSTimePicker>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSTimePicker>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSTimePicker>::Inherit<JSViewAbstract>();
    JSClass<JSTimePicker>::Bind(globalObj);
}

void JSTimePicker::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("DatePicker create error, info is non-valid");
        return;
    }

    CreateTimePicker(JSRef<JSObject>::Cast(info[0]));
}

void JSTimePicker::UseMilitaryTime(bool isUseMilitaryTime)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto timePicker = AceType::DynamicCast<PickerTimeComponent>(component);
    if (!timePicker) {
        LOGE("PickerTimeComponent is null");
        return;
    }
    timePicker->SetHour24(isUseMilitaryTime);
}

void JSTimePicker::CreateTimePicker(const JSRef<JSObject>& paramObj)
{
    auto timePicker = AceType::MakeRefPtr<PickerTimeComponent>();
    auto selectedTime = paramObj->GetProperty("selected");
    if (selectedTime->IsObject()) {
        timePicker->SetSelectedTime(ParseTime(selectedTime));
    }
    timePicker->SetIsDialog(false);
    timePicker->SetHasButtons(false);

    auto theme = GetTheme<PickerTheme>();
    if (!theme) {
        LOGE("timePicker Theme is null");
        return;
    }

    timePicker->SetTheme(theme);
    ViewStackProcessor::GetInstance()->Push(timePicker);
}

PickerTime JSTimePicker::ParseTime(const JSRef<JSVal>& timeVal)
{
    auto pickerTime = PickerTime();
    if (!timeVal->IsObject()) {
        return pickerTime;
    }
    auto timeObj = JSRef<JSObject>::Cast(timeVal);
    auto hourFunc = JSRef<JSFunc>::Cast(timeObj->GetProperty("getHours"));
    auto minuteFunc = JSRef<JSFunc>::Cast(timeObj->GetProperty("getMinutes"));
    auto secondFunc = JSRef<JSFunc>::Cast(timeObj->GetProperty("getSeconds"));
    JSRef<JSVal> hour = hourFunc->Call(timeObj);
    JSRef<JSVal> minute = minuteFunc->Call(timeObj);
    JSRef<JSVal> second = secondFunc->Call(timeObj);

    if (hour->IsNumber() && minute->IsNumber() && second->IsNumber()) {
        pickerTime.SetHour(hour->ToNumber<int32_t>());
        pickerTime.SetMinute(minute->ToNumber<int32_t>());
        pickerTime.SetSecond(second->ToNumber<int32_t>());
    }
    return pickerTime;
}

void JSTimePickerDialog::JSBind(BindingTarget globalObj)
{
    JSClass<JSTimePickerDialog>::Declare("TimePickerDialog");
    JSClass<JSTimePickerDialog>::StaticMethod("show", &JSTimePickerDialog::Show);

    JSClass<JSTimePickerDialog>::Bind<>(globalObj);
}

void JSTimePickerDialog::Show(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("DatePicker Show dialog error, info is non-valid");
        return;
    }

    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    RefPtr<Component> component;
    CreateTimePicker(component, paramObject);

    auto datePicker = AceType::DynamicCast<PickerBaseComponent>(component);
    DialogProperties properties {};
    properties.alignment = DialogAlignment::CENTER;
    properties.customComponent = datePicker;

    AddEvent(datePicker, info, DatePickerType::TIME);
    datePicker->SetDialogName("TimePickerDialog");
    datePicker->OpenDialog(properties);
}

void JSTimePickerDialog::CreateTimePicker(RefPtr<Component> &component, const JSRef<JSObject>& paramObj)
{
    auto timePicker = AceType::MakeRefPtr<PickerTimeComponent>();
    auto selectedTime = paramObj->GetProperty("selected");
    auto useMilitaryTime = paramObj->GetProperty("useMilitaryTime");
    bool isUseMilitaryTime = useMilitaryTime->ToBoolean();
    if (selectedTime->IsObject()) {
        timePicker->SetSelectedTime(ParseTime(selectedTime));
    }
    timePicker->SetIsDialog(false);
    timePicker->SetIsCreateDialogComponent(true);
    timePicker->SetHour24(isUseMilitaryTime);
    component = timePicker;
}

PickerTime JSTimePickerDialog::ParseTime(const JSRef<JSVal>& timeVal)
{
    auto pickerTime = PickerTime();
    if (!timeVal->IsObject()) {
        return pickerTime;
    }
    auto timeObj = JSRef<JSObject>::Cast(timeVal);
    auto hourFunc = JSRef<JSFunc>::Cast(timeObj->GetProperty("getHours"));
    auto minuteFunc = JSRef<JSFunc>::Cast(timeObj->GetProperty("getMinutes"));
    auto secondFunc = JSRef<JSFunc>::Cast(timeObj->GetProperty("getSeconds"));
    JSRef<JSVal> hour = hourFunc->Call(timeObj);
    JSRef<JSVal> minute = minuteFunc->Call(timeObj);
    JSRef<JSVal> second = secondFunc->Call(timeObj);

    if (hour->IsNumber() && minute->IsNumber() && second->IsNumber()) {
        pickerTime.SetHour(hour->ToNumber<int32_t>());
        pickerTime.SetMinute(minute->ToNumber<int32_t>());
        pickerTime.SetSecond(second->ToNumber<int32_t>());
    }
    return pickerTime;
}
} // namespace OHOS::Ace::Framework
