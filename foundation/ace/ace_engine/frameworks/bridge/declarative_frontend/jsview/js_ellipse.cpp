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

#include "frameworks/bridge/declarative_frontend/jsview/js_ellipse.h"

#include "core/components/box/box_component.h"
#include "core/components/shape/shape_component.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSEllipse::Create(const JSCallbackInfo& info)
{
    RefPtr<Component> ellipseComponent = AceType::MakeRefPtr<OHOS::Ace::ShapeComponent>(ShapeType::ELLIPSE);
    ViewStackProcessor::GetInstance()->Push(ellipseComponent);
    JSShapeAbstract::SetSize(info);
}

void JSEllipse::ConstructorCallback(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    if (!info[0]->IsObject()) {
        LOGE("The arg is not Object");
        return;
    }
    auto ellipse = AceType::MakeRefPtr<Ellipse>();
    JSRef<JSObject> params = JSRef<JSObject>::Cast(info[0]);
    JSRef<JSVal> width = params->GetProperty("width");
    Dimension dimWidth;
    if (ParseJsDimensionVp(width, dimWidth)) {
        ellipse->SetWidth(dimWidth);
    }
    JSRef<JSVal> height = params->GetProperty("height");
    Dimension dimHeight;
    if (ParseJsDimensionVp(height, dimHeight)) {
        ellipse->SetHeight(dimHeight);
    }

    auto jsEllipse = AceType::MakeRefPtr<JSEllipse>();
    jsEllipse->SetBasicShape(ellipse);
    jsEllipse->IncRefCount();
    info.SetReturnValue(AceType::RawPtr(jsEllipse));
}

void JSEllipse::DestructorCallback(JSEllipse* jsEllipse)
{
    if (jsEllipse != nullptr) {
        jsEllipse->DecRefCount();
    }
}

void JSEllipse::JSBind(BindingTarget globalObj)
{
    JSClass<JSEllipse>::Declare("Ellipse");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSEllipse>::StaticMethod("create", &JSEllipse::Create, opt);

    JSClass<JSEllipse>::CustomMethod("width", &JSShapeAbstract::ObjectWidth);
    JSClass<JSEllipse>::CustomMethod("height", &JSShapeAbstract::ObjectHeight);
    JSClass<JSEllipse>::CustomMethod("size", &JSShapeAbstract::ObjectSize);
    JSClass<JSEllipse>::CustomMethod("offset", &JSShapeAbstract::ObjectOffset);
    JSClass<JSEllipse>::CustomMethod("fill", &JSShapeAbstract::ObjectFill);

    JSClass<JSEllipse>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSEllipse>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSEllipse>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSEllipse>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSEllipse>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSEllipse>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);

    JSClass<JSEllipse>::Inherit<JSShapeAbstract>();
    JSClass<JSEllipse>::Bind(globalObj, JSEllipse::ConstructorCallback, JSEllipse::DestructorCallback);
}

} // namespace OHOS::Ace::Framework
