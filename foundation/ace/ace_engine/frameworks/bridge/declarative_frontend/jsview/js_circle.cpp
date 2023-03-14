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

#include "frameworks/bridge/declarative_frontend/jsview/js_circle.h"

#include "core/components/box/box_component.h"
#include "core/components/shape/shape_component.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSCircle::Create(const JSCallbackInfo& info)
{
    RefPtr<Component> circleComponent = AceType::MakeRefPtr<OHOS::Ace::ShapeComponent>(ShapeType::CIRCLE);
    ViewStackProcessor::GetInstance()->Push(circleComponent);
    JSShapeAbstract::SetSize(info);
}

void JSCircle::ConstructorCallback(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    if (!info[0]->IsObject()) {
        LOGE("The arg is not Object");
        return;
    }
    auto circle = AceType::MakeRefPtr<Circle>();
    JSRef<JSObject> params = JSRef<JSObject>::Cast(info[0]);
    JSRef<JSVal> width = params->GetProperty("width");
    Dimension dimWidth;
    if (ParseJsDimensionVp(width, dimWidth)) {
        circle->SetWidth(dimWidth);
    }
    JSRef<JSVal> height = params->GetProperty("height");
    Dimension dimHeight;
    if (ParseJsDimensionVp(height, dimHeight)) {
        circle->SetHeight(dimHeight);
    }

    auto jsCircle = AceType::MakeRefPtr<JSCircle>();
    jsCircle->SetBasicShape(circle);
    jsCircle->IncRefCount();
    info.SetReturnValue(AceType::RawPtr(jsCircle));
}

void JSCircle::DestructorCallback(JSCircle* jsCircle)
{
    if (jsCircle != nullptr) {
        jsCircle->DecRefCount();
    }
}

void JSCircle::JSBind(BindingTarget globalObj)
{
    JSClass<JSCircle>::Declare("Circle");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSCircle>::StaticMethod("create", &JSCircle::Create, opt);

    JSClass<JSCircle>::CustomMethod("width", &JSShapeAbstract::ObjectWidth);
    JSClass<JSCircle>::CustomMethod("height", &JSShapeAbstract::ObjectHeight);
    JSClass<JSCircle>::CustomMethod("size", &JSShapeAbstract::ObjectSize);
    JSClass<JSCircle>::CustomMethod("offset", &JSShapeAbstract::ObjectOffset);
    JSClass<JSCircle>::CustomMethod("fill", &JSShapeAbstract::ObjectFill);

    JSClass<JSCircle>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSCircle>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSCircle>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSCircle>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSCircle>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSCircle>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);

    JSClass<JSCircle>::Inherit<JSShapeAbstract>();
    JSClass<JSCircle>::Bind(globalObj, JSCircle::ConstructorCallback, JSCircle::DestructorCallback);
}

} // namespace OHOS::Ace::Framework
