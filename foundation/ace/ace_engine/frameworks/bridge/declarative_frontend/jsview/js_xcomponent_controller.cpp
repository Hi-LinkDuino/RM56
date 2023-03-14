/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "frameworks/bridge/declarative_frontend/jsview/js_xcomponent_controller.h"

#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_common_def.h"

namespace OHOS::Ace::Framework {
void JSXComponentController::JSBind(BindingTarget globalObj)
{
    JSClass<JSXComponentController>::Declare("XComponentController");
    JSClass<JSXComponentController>::CustomMethod("getXComponentSurfaceId", &JSXComponentController::GetSurfaceId);
    JSClass<JSXComponentController>::CustomMethod("getXComponentContext",
        &JSXComponentController::GetXComponentContext);
    JSClass<JSXComponentController>::CustomMethod("setXComponentSurfaceSize",
        &JSXComponentController::SetSurfaceConfig);
    JSClass<JSXComponentController>::Bind(globalObj, JSXComponentController::Constructor,
        JSXComponentController::Destructor);
}

void JSXComponentController::Constructor(const JSCallbackInfo& args)
{
    auto xcomponentController = Referenced::MakeRefPtr<JSXComponentController>();
    xcomponentController->IncRefCount();
    RefPtr<XComponentController> controller = AceType::MakeRefPtr<XComponentController>();
    xcomponentController->SetController(controller);
    args.SetReturnValue(Referenced::RawPtr(xcomponentController));
}

void JSXComponentController::Destructor(JSXComponentController* xcomponentController)
{
    if (xcomponentController) {
        xcomponentController->DecRefCount();
        const auto& controller = xcomponentController->GetController();
        if (controller) {
            controller->Clear();
        }
    }
}

void JSXComponentController::GetSurfaceId(const JSCallbackInfo& args)
{
    if (xcomponentController_) {
        auto surfaceId = xcomponentController_->GetSurfaceId();
        auto returnValue = JSVal(ToJSValue(std::to_string(surfaceId)));
        auto returnPtr = JSRef<JSVal>::Make(returnValue);
        args.SetReturnValue(returnPtr);
    }
}

void JSXComponentController::SetSurfaceConfig(const JSCallbackInfo& args)
{
    if (args.Length() < 1 || !args[0]->IsObject()) {
        LOGW("Invalid params");
        return;
    }

    JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
    int32_t surfaceWidth;
    int32_t surfaceHeight;
    if (!ConvertFromJSValue(obj->GetProperty("surfaceWidth"), surfaceWidth) ||
        !ConvertFromJSValue(obj->GetProperty("surfaceHeight"), surfaceHeight)) {
        LOGW("Failed to parse param 'surfaceWidth' or 'surfaceHeight'");
        return;
    }

    if (xcomponentController_) {
        xcomponentController_->ConfigSurface(surfaceWidth, surfaceHeight);
    }
}
} // namespace OHOS::Ace::Framework