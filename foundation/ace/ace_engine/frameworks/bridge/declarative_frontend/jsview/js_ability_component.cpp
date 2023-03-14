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

#include "frameworks/bridge/declarative_frontend/jsview/js_ability_component.h"

#include "frameworks/bridge/declarative_frontend/jsview/js_ability_component_controller.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSAbilityComponent::JSBind(BindingTarget globalObj)
{
    JSClass<JSAbilityComponent>::Declare("AbilityComponent");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSAbilityComponent>::StaticMethod("create", &JSAbilityComponent::Create, opt);
    JSClass<JSAbilityComponent>::StaticMethod("onReady", &JSAbilityComponent::JsOnReady, opt);
    JSClass<JSAbilityComponent>::StaticMethod("onDestroy", &JSAbilityComponent::JsOnDestroy, opt);
    JSClass<JSAbilityComponent>::StaticMethod("onAbilityCreated", &JSAbilityComponent::JsOnAbilityCreated, opt);
    JSClass<JSAbilityComponent>::StaticMethod("onAbilityMoveToFront", &JSAbilityComponent::JsOnAbilityMovedFront, opt);
    JSClass<JSAbilityComponent>::StaticMethod("onAbilityWillRemove", &JSAbilityComponent::JsOnAbilityWillRemove, opt);
    JSClass<JSAbilityComponent>::Inherit<JSViewAbstract>();
    JSClass<JSAbilityComponent>::Bind<>(globalObj);
}

void JSAbilityComponent::Create(const JSCallbackInfo& info)
{
    if (info.Length() != 1 || !info[0]->IsObject()) {
        return;
    }
    auto component = AceType::MakeRefPtr<OHOS::Ace::AbilityComponent>();
    auto obj = JSRef<JSObject>::Cast(info[0]);

    // Parse want
    JSRef<JSVal> wantValue = obj->GetProperty("want");
    if (wantValue->IsObject()) {
        component->SetWant(wantValue->ToString());
    }

    // Parse controller
    auto controllerObj = obj->GetProperty("controller");
    if (controllerObj->IsObject()) {
        auto controller = JSRef<JSObject>::Cast(controllerObj)->Unwrap<JSAbilityComponentController>();
        if (controller) {
            component->SetController(controller->GetController());
        }
    }

    ViewStackProcessor::GetInstance()->Push(component);
}

void JSAbilityComponent::JsOnReady(const JSCallbackInfo& info)
{
    JSViewBindEvent(&AbilityComponent::SetOnReady, info);
}

void JSAbilityComponent::JsOnDestroy(const JSCallbackInfo& info)
{
    JSViewBindEvent(&AbilityComponent::SetOnDestroy, info);
}

void JSAbilityComponent::JsOnAbilityCreated(const JSCallbackInfo& info)
{
    JSViewBindEvent(&AbilityComponent::SetOnAbilityCreated, info);
}

void JSAbilityComponent::JsOnAbilityMovedFront(const JSCallbackInfo& info)
{
    JSViewBindEvent(&AbilityComponent::SetOnAbilityMovedFront, info);
}

void JSAbilityComponent::JsOnAbilityWillRemove(const JSCallbackInfo& info)
{
    JSViewBindEvent(&AbilityComponent::SetOnAbilityWillRemove, info);
}

} // namespace OHOS::Ace::Framework
