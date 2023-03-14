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

#include "frameworks/bridge/declarative_frontend/jsview/js_stepper.h"

#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components/box/box_component.h"
#include "core/components/navigator/navigator_component.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "frameworks/core/components/stepper/stepper_component.h"
#include "frameworks/core/components/stepper/stepper_theme.h"

namespace OHOS::Ace::Framework {

void JSStepper::Create(const JSCallbackInfo& info)
{
    std::list<RefPtr<OHOS::Ace::Component>> componentChildren;
    auto stepperComponent = AceType::MakeRefPtr<OHOS::Ace::StepperComponent>(componentChildren);
    uint32_t index = 0;
    if (info.Length() < 1 || !info[0]->IsObject()) {
        index = 0;
    } else {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> stepperValue = obj->GetProperty("index");
        if (stepperValue->IsNumber()) {
            index = stepperValue->ToNumber<uint32_t>();
        }
    }
    stepperComponent->SetIndex(index);
    RefPtr<StepperTheme> theme = GetTheme<StepperTheme>();
    if (theme) {
        stepperComponent->SetDefaultPaddingStart(theme->GetDefaultPaddingStart());
        stepperComponent->SetDefaultPaddingEnd(theme->GetDefaultPaddingEnd());
        stepperComponent->SetProgressColor(theme->GetProgressColor());
        stepperComponent->SetProgressDiameter(theme->GetProgressDiameter());
        stepperComponent->SetArrowWidth(theme->GetArrowWidth());
        stepperComponent->SetArrowHeight(theme->GetArrowHeight());
        stepperComponent->SetArrowColor(theme->GetArrowColor());
        stepperComponent->SetDisabledColor(theme->GetDisabledColor());
        stepperComponent->SetRadius(theme->GetRadius());
        stepperComponent->SetButtonPressedColor(theme->GetButtonPressedColor());
        stepperComponent->SetButtonPressedHeight(theme->GetButtonPressedHeight());
        stepperComponent->SetControlHeight(theme->GetControlHeight());
        stepperComponent->SetControlMargin(theme->GetControlMargin());
        stepperComponent->SetControlPadding(theme->GetControlPadding());
        stepperComponent->SetFocusColor(theme->GetFocusColor());
        stepperComponent->SetFocusBorderWidth(theme->GetFocusBorderWidth());
        stepperComponent->SetMouseHoverColor(theme->GetMouseHoverColor());
        stepperComponent->SetDisabledAlpha(theme->GetDisabledAlpha());
    }
    ViewStackProcessor::GetInstance()->Push(stepperComponent);
}

void JSStepper::JSBind(BindingTarget globalObj)
{
    JSClass<JSStepper>::Declare("Stepper");

    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSStepper>::StaticMethod("create", &JSStepper::Create, opt);
    JSClass<JSStepper>::StaticMethod("onFinish", &JSStepper::OnFinish);
    JSClass<JSStepper>::StaticMethod("onSkip", &JSStepper::OnSkip);
    JSClass<JSStepper>::StaticMethod("onChange", &JSStepper::OnChange);
    JSClass<JSStepper>::StaticMethod("onNext", &JSStepper::OnNext);
    JSClass<JSStepper>::StaticMethod("onPrevious", &JSStepper::OnPrevious);
    JSClass<JSStepper>::Inherit<JSContainerBase>();
    JSClass<JSStepper>::Inherit<JSViewAbstract>();
    JSClass<JSStepper>::Bind<>(globalObj);
}

void JSStepper::OnFinish(const JSCallbackInfo& args)
{
    if (!JSViewBindEvent(&StepperComponent::SetOnFinish, args)) {
        LOGW("Failed to bind event");
    }
    args.ReturnSelf();
}

void JSStepper::OnSkip(const JSCallbackInfo& args)
{
    if (!JSViewBindEvent(&StepperComponent::SetOnSkip, args)) {
        LOGW("Failed to bind event");
    }
    args.ReturnSelf();
}

void JSStepper::OnChange(const JSCallbackInfo& args)
{
    if (!JSViewBindEvent(&StepperComponent::SetOnChange, args)) {
        LOGW("Failed to bind event");
    }
    args.ReturnSelf();
}

void JSStepper::OnNext(const JSCallbackInfo& args)
{
    if (!JSViewBindEvent(&StepperComponent::SetOnNext, args)) {
        LOGW("Failed to bind event");
    }
    args.ReturnSelf();
}

void JSStepper::OnPrevious(const JSCallbackInfo& args)
{
    if (!JSViewBindEvent(&StepperComponent::SetOnPrevious, args)) {
        LOGW("Failed to bind event");
    }
    args.ReturnSelf();
}

} // namespace OHOS::Ace::Framework
