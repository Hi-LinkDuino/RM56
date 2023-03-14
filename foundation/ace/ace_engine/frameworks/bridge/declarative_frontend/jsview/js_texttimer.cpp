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

#include "frameworks/bridge/declarative_frontend/jsview/js_texttimer.h"

#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components/common/layout/constants.h"
#include "core/components/texttimer/texttimer_component.h"

namespace OHOS::Ace::Framework {
namespace {
const std::vector<FontStyle> FONT_STYLES = { FontStyle::NORMAL, FontStyle::ITALIC };
const std::vector<TextCase> TEXT_CASES = { TextCase::NORMAL, TextCase::LOWERCASE, TextCase::UPPERCASE };
} // namespace

void JSTextTimer::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("TextTimer create error, info is non-valid");
        return;
    }

    auto timerComponent = AceType::MakeRefPtr<TextTimerComponent>();
    if (!timerComponent) {
        LOGE("timerComponent is nullptr");
        return;
    }

    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto tempIsCountDown = paramObject->GetProperty("isCountDown");
    bool isCountDown = tempIsCountDown->ToBoolean();
    timerComponent->SetIsCountDown(isCountDown);

    if (isCountDown) {
        auto count = paramObject->GetProperty("count");
        if (count->IsNumber()) {
            double inputCount = count->ToNumber<double>();
            if (inputCount > 0) {
                timerComponent->SetInputCount(inputCount);
            }
        }
    }

    auto controllerObj = paramObject->GetProperty("controller");
    if (controllerObj->IsObject()) {
        JSTextTimerController* jsController = JSRef<JSObject>::Cast(controllerObj)->Unwrap<JSTextTimerController>();
        if (jsController) {
            jsController->SetController(timerComponent->GetTextTimerController());
        }
    }

    ViewStackProcessor::GetInstance()->Push(timerComponent);
}

void JSTextTimer::JSBind(BindingTarget globalObj)
{
    JSClass<JSTextTimer>::Declare("TextTimer");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSTextTimer>::StaticMethod("create", &JSTextTimer::Create, opt);
    JSClass<JSTextTimer>::StaticMethod("format", &JSTextTimer::SetFormat);
    JSClass<JSTextTimer>::StaticMethod("fontColor", &JSTextTimer::SetTextColor);
    JSClass<JSTextTimer>::StaticMethod("fontSize", &JSTextTimer::SetFontSize);
    JSClass<JSTextTimer>::StaticMethod("fontWeight", &JSTextTimer::SetFontWeight, opt);
    JSClass<JSTextTimer>::StaticMethod("fontStyle", &JSTextTimer::SetFontStyle, opt);
    JSClass<JSTextTimer>::StaticMethod("fontFamily", &JSTextTimer::SetFontFamily, opt);
    JSClass<JSTextTimer>::StaticMethod("onTimer", &JSTextTimer::OnTimer);
    JSClass<JSTextTimer>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSTextTimer>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSTextTimer>::Inherit<JSViewAbstract>();
    JSClass<JSTextTimer>::Bind<>(globalObj);
}

void JSTextTimer::SetFormat(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    if (!info[0]->IsString()) {
        LOGE("arg is not string.");
        return;
    }

    auto format = info[0]->ToString();
    auto pos = format.find("hh");
    if (pos != std::string::npos) {
        format.replace(pos, sizeof("hh") - 1, "HH");
    }

    pos = format.find("ms");
    if (pos != std::string::npos) {
        format.replace(pos, sizeof("ms") - 1, "SS");
    }

    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto timerComponent = AceType::DynamicCast<TextTimerComponent>(component);
    if (!timerComponent) {
        LOGE("TextTimer Component is null");
        return;
    }
    timerComponent->SetFormat(format);
}

void JSTextTimer::SetFontSize(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("JSTextInput::SetFontSize The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension fontSize;
    if (!ParseJsDimensionFp(info[0], fontSize)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto timerComponent = AceType::DynamicCast<OHOS::Ace::TextTimerComponent>(stack->GetMainComponent());
    if (!timerComponent) {
        LOGE("JSTextTimer::SetFontSize timerComponent is not valid");
        return;
    }

    auto textStyle = timerComponent->GetTextStyle();
    textStyle.SetFontSize(fontSize);
    timerComponent->SetTextStyle(textStyle);
}

void JSTextTimer::SetTextColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color textColor;
    if (!ParseJsColor(info[0], textColor)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto timerComponent = AceType::DynamicCast<OHOS::Ace::TextTimerComponent>(stack->GetMainComponent());
    if (!timerComponent) {
        LOGE("JSTextTimer::SetTextColor timerComponent is not valid");
        return;
    }

    auto textStyle = timerComponent->GetTextStyle();
    textStyle.SetTextColor(textColor);
    timerComponent->SetTextStyle(textStyle);
}

void JSTextTimer::SetFontWeight(const std::string& value)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto timerComponent = AceType::DynamicCast<OHOS::Ace::TextTimerComponent>(stack->GetMainComponent());
    if (!timerComponent) {
        LOGE("JSTextTimer::SetFontWeight timerComponent is not valid");
        return;
    }

    auto textStyle = timerComponent->GetTextStyle();
    textStyle.SetFontWeight(ConvertStrToFontWeight(value));
    timerComponent->SetTextStyle(std::move(textStyle));
}

void JSTextTimer::SetFontStyle(int32_t value)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto timerComponent = AceType::DynamicCast<OHOS::Ace::TextTimerComponent>(stack->GetMainComponent());
    if (!timerComponent) {
        LOGE("JSTextTimer::SetFontStyle timerComponent is not valid");
        return;
    }

    if (value >= 0 && value < static_cast<int32_t>(FONT_STYLES.size())) {
        auto textStyle = timerComponent->GetTextStyle();
        textStyle.SetFontStyle(FONT_STYLES[value]);
        timerComponent->SetTextStyle(std::move(textStyle));
    } else {
        LOGE("Text fontStyle(%d) illegal value", value);
    }
}

void JSTextTimer::SetFontFamily(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    std::vector<std::string> fontFamilies;
    if (!ParseJsFontFamilies(info[0], fontFamilies)) {
        LOGE("Parse FontFamilies failed");
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto timerComponent = AceType::DynamicCast<OHOS::Ace::TextTimerComponent>(stack->GetMainComponent());
    if (!timerComponent) {
        LOGE("JSTextTimer::SetFontFamily timerComponent is not valid");
        return;
    }

    auto textStyle = timerComponent->GetTextStyle();
    textStyle.SetFontFamilies(fontFamilies);
    timerComponent->SetTextStyle(std::move(textStyle));
}


void JSTextTimer::OnTimer(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&TextTimerComponent::SetOnTimer, info)) {
        LOGW("Failed(OnTimer) to bind event");
    }
    info.ReturnSelf();
}

void JSTextTimerController::JSBind(BindingTarget globalObj)
{
    JSClass<JSTextTimerController>::Declare("TextTimerController");
    JSClass<JSTextTimerController>::CustomMethod("start", &JSTextTimerController::Start);
    JSClass<JSTextTimerController>::CustomMethod("pause", &JSTextTimerController::Pause);
    JSClass<JSTextTimerController>::CustomMethod("reset", &JSTextTimerController::Reset);
    JSClass<JSTextTimerController>::Bind(globalObj,
        JSTextTimerController::Constructor, JSTextTimerController::Destructor);
}

void JSTextTimerController::Constructor(const JSCallbackInfo& info)
{
    auto timerController = Referenced::MakeRefPtr<JSTextTimerController>();
    timerController->IncRefCount();
    info.SetReturnValue(Referenced::RawPtr(timerController));
}

void JSTextTimerController::Destructor(JSTextTimerController* timerController)
{
    if (timerController != nullptr) {
        timerController->DecRefCount();
    }
}

void JSTextTimerController::Start(const JSCallbackInfo& info)
{
    if (controller_) {
        controller_->Start();
    }
}

void JSTextTimerController::Pause(const JSCallbackInfo& info)
{
    if (controller_) {
        controller_->Pause();
    }
}

void JSTextTimerController::Reset(const JSCallbackInfo& info)
{
    if (controller_) {
        controller_->Reset();
    }
}
} // namespace OHOS::Ace::Framework
