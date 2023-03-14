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

#include "bridge/declarative_frontend/jsview/js_ability_component_controller.h"

#include "bridge/declarative_frontend/jsview/js_view_common_def.h"

namespace OHOS::Ace::Framework {

void JSAbilityComponentController::JSBind(BindingTarget globalObj)
{
    JSClass<JSAbilityComponentController>::Declare("AbilityController");
    JSClass<JSAbilityComponentController>::CustomMethod("startAbility", &JSAbilityComponentController::StartAbility);
    JSClass<JSAbilityComponentController>::CustomMethod(
        "performBackPress", &JSAbilityComponentController::PerformBackPress);
    JSClass<JSAbilityComponentController>::CustomMethod("getStackCount", &JSAbilityComponentController::GetStackCount);
    JSClass<JSAbilityComponentController>::Bind(
        globalObj, JSAbilityComponentController::Constructor, JSAbilityComponentController::Destructor);
}

void JSAbilityComponentController::Constructor(const JSCallbackInfo& args)
{
    auto controller = Referenced::MakeRefPtr<JSAbilityComponentController>();
    controller->IncRefCount();
    args.SetReturnValue(Referenced::RawPtr(controller));
}

void JSAbilityComponentController::Destructor(JSAbilityComponentController* controller)
{
    if (controller) {
        controller->DecRefCount();
    }
}

void JSAbilityComponentController::StartAbility(const JSCallbackInfo& args)
{
    if (!controller_ || !args[0]->IsObject()) {
        LOGE("Start ability fail, controller is empty or params is not valid.");
        return;
    }
    controller_->StartAbility(args[0]->ToString());
}

void JSAbilityComponentController::PerformBackPress(const JSCallbackInfo& args)
{
    if (controller_) {
        controller_->PerformBackPress();
    }
}

void JSAbilityComponentController::GetStackCount(const JSCallbackInfo& args)
{
    if (controller_) {
        auto stackCount = controller_->GetStackCount();
        auto returnValue = JSVal(ToJSValue(stackCount));
        auto returnPtr = JSRef<JSVal>::Make(returnValue);
        args.SetReturnValue(returnPtr);
    }
}

} // namespace OHOS::Ace::Framework