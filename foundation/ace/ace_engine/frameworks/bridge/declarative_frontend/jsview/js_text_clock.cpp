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

#include "frameworks/bridge/declarative_frontend/jsview/js_text_clock.h"

#include <sys/time.h>

#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "core/components/text_clock/text_clock_component.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_text.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

namespace {
const std::vector<FontStyle> FONT_STYLES = { FontStyle::NORMAL, FontStyle::ITALIC };
const int32_t TWENTY_FOUR_HOUR_BASE = 24;
constexpr int32_t HOURS_WEST_LOWER_LIMIT = -14;
constexpr int32_t HOURS_WEST_UPPER_LIMIT = 12;
constexpr int32_t HOURS_WEST_GEOGRAPHICAL_LOWER_LIMIT = -12;
constexpr int32_t TOTAL_MINUTE_OF_HOUR = 60;
constexpr char DEFAULT_FORMAT[] = "hhmmss";

bool IsHoursWestValid(double& hoursWest)
{
    if (hoursWest < HOURS_WEST_LOWER_LIMIT || hoursWest > HOURS_WEST_UPPER_LIMIT) {
        return false;
    }
    if (hoursWest < HOURS_WEST_GEOGRAPHICAL_LOWER_LIMIT) {
        hoursWest += TWENTY_FOUR_HOUR_BASE;
    }
    return true;
}

double GetSystemTimeZone()
{
    struct timeval currentTime;
    struct timezone timeZone;
    gettimeofday(&currentTime, &timeZone);
    int32_t minutesWest = timeZone.tz_minuteswest;
    double hoursWest = static_cast<double>(minutesWest) / static_cast<double>(TOTAL_MINUTE_OF_HOUR);
    return hoursWest;
}
} // namespace

void JSTextClock::Create(const JSCallbackInfo& info)
{
    double hourWest = GetSystemTimeZone();
    RefPtr<TextClockComponent> textClockComponent = AceType::MakeRefPtr<TextClockComponent>(std::string(""));
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGD("TextClock Info is non-valid");
    } else {
        JSRef<JSObject> optionsObject = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> hourWestVal = optionsObject->GetProperty("timeZoneOffset");
        if (hourWestVal->IsNumber()) {
            double hourWest_ = hourWestVal->ToNumber<double>();
            if (IsHoursWestValid(hourWest_)) {
                hourWest = hourWest_;
            } else {
                LOGE("hourWest args is invalid");
            }
        } else {
            LOGE("hourWest args is not number,args is invalid");
        }
        auto controllerObj = optionsObject->GetProperty("controller");
        if (!controllerObj->IsUndefined() && !controllerObj->IsNull()) {
            textClockComponent->SetTextClockController(AceType::MakeRefPtr<TextClockController>());
            JSTextClockController* jsController = JSRef<JSObject>::Cast(controllerObj)->Unwrap<JSTextClockController>();
            if (jsController != nullptr) {
                auto controller = textClockComponent->GetTextClockController();
                if (controller) {
                    jsController->SetController(controller);
                } else {
                    LOGD("TextClockController is nullptr");
                }
            }
        } else {
            LOGI("controllerObj is nullptr or undefined");
        }
    }
    textClockComponent->SetHoursWest(hourWest);
    std::string defaultFormat = DEFAULT_FORMAT;
    textClockComponent->SetFormat(defaultFormat);
    ViewStackProcessor::GetInstance()->Push(textClockComponent);
}

void JSTextClock::JSBind(BindingTarget globalObj)
{
    JSClass<JSTextClock>::Declare("TextClock");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSTextClock>::StaticMethod("create", &JSTextClock::Create, opt);
    JSClass<JSTextClock>::StaticMethod("format", &JSTextClock::SetFormat, opt);
    JSClass<JSTextClock>::StaticMethod("onDateChange", &JSTextClock::JsOnDateChange, opt);
    JSClass<JSTextClock>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSTextClock>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSTextClock>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSTextClock>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSTextClock>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSTextClock>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSTextClock>::StaticMethod("fontColor", &JSText::SetTextColor, opt);
    JSClass<JSTextClock>::StaticMethod("fontSize", &JSText::SetFontSize, opt);
    JSClass<JSTextClock>::StaticMethod("fontStyle", &JSText::SetFontStyle, opt);
    JSClass<JSTextClock>::StaticMethod("fontWeight", &JSText::SetFontWeight, opt);
    JSClass<JSTextClock>::StaticMethod("fontFamily", &JSText::SetFontFamily, opt);
    JSClass<JSTextClock>::Inherit<JSViewAbstract>();
    JSClass<JSTextClock>::Bind<>(globalObj);
}

void JSTextClock::SetFormat(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 argument.");
        return;
    }
    auto textClockComponent = AceType::DynamicCast<TextClockComponent>
                              (ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!textClockComponent) {
        LOGE("textClockComponent is null.");
        return;
    }
    if (info[0]->IsString()) {
        textClockComponent->SetFormat(info[0]->ToString());
    } else {
        LOGE("arg is not string");
        return;
    }
}

void JSTextClock::JsOnDateChange(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&TextClockComponent::SetOnDateChange, info)) {
        LOGW("failed to bind event");
    }
    info.ReturnSelf();
}

void JSTextClockController::JSBind(BindingTarget globalObj)
{
    JSClass<JSTextClockController>::Declare("TextClockController");
    JSClass<JSTextClockController>::Method("start", &JSTextClockController::Start);
    JSClass<JSTextClockController>::Method("stop", &JSTextClockController::Stop);
    JSClass<JSTextClockController>::Bind(globalObj, JSTextClockController::Constructor,
                                         JSTextClockController::Destructor);
}

void JSTextClockController::Constructor(const JSCallbackInfo& args)
{
    auto scroller = Referenced::MakeRefPtr<JSTextClockController>();
    scroller->IncRefCount();
    args.SetReturnValue(Referenced::RawPtr(scroller));
}

void JSTextClockController::Destructor(JSTextClockController* scroller)
{
    if (scroller != nullptr) {
        scroller->DecRefCount();
    }
}

void JSTextClockController::Start()
{
    if (controller_) {
        controller_->Start();
    }
}

void JSTextClockController::Stop()
{
    if (controller_) {
        controller_->Stop();
    }
}
} // namespace OHOS::Ace::Framework
