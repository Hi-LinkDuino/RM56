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

#include "frameworks/bridge/declarative_frontend/jsview/js_camera.h"

#include "base/log/ace_trace.h"
#include "core/components/camera/camera_component.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSCamera::Create(const JSCallbackInfo& info)
{
    RefPtr<CameraComponent> cameraComponent = AceType::MakeRefPtr<OHOS::Ace::CameraComponent>();
    if (info.Length() > 0 && info[0]->IsObject()) {
        JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> idValue = sizeObj->GetProperty("id");
        if (!idValue->IsNull() && !idValue->IsEmpty()) {
            cameraComponent->SetCameraId(idValue->ToString());
        }

        JSRef<JSVal> resolutionWidthValue = sizeObj->GetProperty("resolutionWidth");
        int32_t resolutionWidth = -1;
        if (!resolutionWidthValue->IsNull() && !resolutionWidthValue->IsEmpty()
            && resolutionWidthValue->IsNumber()) {
            resolutionWidth = resolutionWidthValue->ToNumber<int32_t>();
        }

        JSRef<JSVal> resolutionHeightValue = sizeObj->GetProperty("resolutionHeight");
        int32_t resolutionHeight = -1;
        if (!resolutionHeightValue->IsNull() && !resolutionHeightValue->IsEmpty() &&
            resolutionHeightValue->IsNumber()) {
            resolutionHeight = resolutionHeightValue->ToNumber<int32_t>();
        }

        if (resolutionWidth != -1 && resolutionHeight != -1) {
            cameraComponent->SetResolutionWidth(resolutionWidth);
            cameraComponent->SetResolutionHeight(resolutionHeight);
            cameraComponent->SignSetResolution(true);
        }
    }

    ViewStackProcessor::GetInstance()->Push(cameraComponent);
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    box->SetColor(Color::BLACK);
}

void JSCamera::JsDeviceposition(int32_t value)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto cameraComponent = AceType::DynamicCast<CameraComponent>(stack->GetMainComponent());
    if (!cameraComponent) {
        LOGE("JSCamera: MainComponent is null.");
        return;
    }
    cameraComponent->SetDevicePosition(static_cast<DevicePosition>(value));
}

void JSCamera::JSBind(BindingTarget globalObj)
{
    JSClass<JSCamera>::Declare("Camera");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSCamera>::StaticMethod("create", &JSCamera::Create, opt);
    JSClass<JSCamera>::StaticMethod("devicePosition", &JSCamera::JsDeviceposition, opt);

    JSClass<JSCamera>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSCamera>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSCamera>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSCamera>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSCamera>::StaticMethod("onClick", &JSInteractableView::JsOnClick);

    JSClass<JSCamera>::Inherit<JSViewAbstract>();
    JSClass<JSCamera>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework
