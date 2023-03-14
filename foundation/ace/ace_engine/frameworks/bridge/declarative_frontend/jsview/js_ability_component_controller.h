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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_ABILITY_COMPONENT_CONTROLLER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_ABILITY_COMPONENT_CONTROLLER_H

#include "bridge/declarative_frontend/engine/bindings.h"

#include "base/memory/referenced.h"
#include "core/components/ability_component/ability_component_controller.h"

namespace OHOS::Ace::Framework {

class JSAbilityComponentController : public Referenced {
public:
    JSAbilityComponentController() : controller_(AceType::MakeRefPtr<AbilityComponentController>()) {}
    ~JSAbilityComponentController() override = default;

    static void JSBind(BindingTarget globalObj);
    static void Constructor(const JSCallbackInfo& args);
    static void Destructor(JSAbilityComponentController* controller);

    void StartAbility(const JSCallbackInfo& args);
    void PerformBackPress(const JSCallbackInfo& args);
    void GetStackCount(const JSCallbackInfo& args);

    const RefPtr<AbilityComponentController>& GetController() const
    {
        return controller_;
    }

private:
    RefPtr<AbilityComponentController> controller_;

    ACE_DISALLOW_COPY_AND_MOVE(JSAbilityComponentController);
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_ABILITY_COMPONENT_CONTROLLER_H