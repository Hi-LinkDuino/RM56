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

#include "frameworks/bridge/declarative_frontend/jsview/js_stepper_item.h"

#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components/box/box_component.h"
#include "core/components/focus_animation/focus_animation_theme.h"
#include "core/components/navigator/navigator_component.h"
#include "core/components/stepper/stepper_item_component_v2.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "frameworks/core/components/stepper/stepper_item_component.h"
#include "frameworks/core/components/stepper/stepper_theme.h"

namespace OHOS::Ace::Framework {

void JSStepperItem::Create(const JSCallbackInfo& info)
{
    std::list<RefPtr<OHOS::Ace::Component>> componentChildren;
    auto stepperItemComponentV2 = AceType::MakeRefPtr<StepperItemComponentV2>(
            FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START, componentChildren);
    ViewStackProcessor::GetInstance()->Push(stepperItemComponentV2);

    RefPtr<StepperItemComponent> stepperItemComponent = ViewStackProcessor::GetInstance()->GetStepperItemComponent();
    RefPtr<StepperTheme> stepperTheme = GetTheme<StepperTheme>();
    if (stepperTheme) {
        TextStyle textStyle_ = stepperTheme->GetTextStyle();
        textStyle_.SetAdaptTextSize(stepperTheme->GetTextStyle().GetFontSize(), stepperTheme->GetMinFontSize());
        textStyle_.SetMaxLines(stepperTheme->GetTextMaxLines());
        textStyle_.SetTextOverflow(TextOverflow::ELLIPSIS);
        stepperItemComponent->SetTextStyle(textStyle_);
    }
    auto focusAnimationTheme = GetTheme<FocusAnimationTheme>();
    if (focusAnimationTheme) {
        stepperItemComponent->SetFocusAnimationColor(focusAnimationTheme->GetColor());
    }
    ViewStackProcessor::GetInstance()->GetStepperDisplayComponent();
    ViewStackProcessor::GetInstance()->GetStepperScrollComponent();
}

void JSStepperItem::JSBind(BindingTarget globalObj)
{
    JSClass<JSStepperItem>::Declare("StepperItem");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSStepperItem>::StaticMethod("create", &JSStepperItem::Create, opt);

    JSClass<JSStepperItem>::StaticMethod("prevLabel", &JSStepperItem::SetPrevLabel);
    JSClass<JSStepperItem>::StaticMethod("nextLabel", &JSStepperItem::SetNextLabel);
    JSClass<JSStepperItem>::StaticMethod("status", &JSStepperItem::SetStatus);
    JSClass<JSStepperItem>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSStepperItem>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSStepperItem>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSStepperItem>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSStepperItem>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSStepperItem>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSStepperItem>::Inherit<JSContainerBase>();
    JSClass<JSStepperItem>::Inherit<JSViewAbstract>();
    JSClass<JSStepperItem>::Bind<>(globalObj);
}

void JSStepperItem::SetPrevLabel(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }
    RefPtr<StepperItemComponent> stepperItem = ViewStackProcessor::GetInstance()->GetStepperItemComponent();
    if (!stepperItem) {
        LOGE("StepperItemComponent.");
        return;
    }
    if (info[0]->IsString()) {
        StepperLabels label = stepperItem->GetLabel();
        label.leftLabel = info[0]->ToString();
        stepperItem->SetLabel(label);
    } else {
        LOGE("Arg is not String.");
    }
}

void JSStepperItem::SetNextLabel(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }
    RefPtr<StepperItemComponent> stepperItem = ViewStackProcessor::GetInstance()->GetStepperItemComponent();
    if (!stepperItem) {
        LOGE("StepperItemComponent.");
        return;
    }
    if (info[0]->IsString()) {
        StepperLabels label = stepperItem->GetLabel();
        label.rightLabel = info[0]->ToString();
        stepperItem->SetLabel(label);
    } else {
        LOGE("Arg is not String.");
    }
}

void JSStepperItem::SetStatus(const JSCallbackInfo& info)
{
    const std::string statusArray[] = {"normal", "disabled", "waiting", "skip"};
    std::string status = statusArray[0];
    RefPtr<StepperItemComponent> stepperItem = ViewStackProcessor::GetInstance()->GetStepperItemComponent();
    if (!stepperItem) {
        LOGE("StepperItemComponent is NULL");
        return;
    }
    StepperLabels label = stepperItem->GetLabel();
    if (info.Length() < 1) {
        label.initialStatus = status;
    }

    if (info[0]->IsNumber()) {
        auto index = info[0]->ToNumber<uint32_t>();
        if (index >= 0 && index < sizeof(statusArray) / sizeof(statusArray[0])) {
            status = statusArray[index];
            label.initialStatus = status;
        } else {
            status = statusArray[0];
            label.initialStatus = status;
        }
    }
    stepperItem->SetLabel(label);
}

} // namespace OHOS::Ace::Framework