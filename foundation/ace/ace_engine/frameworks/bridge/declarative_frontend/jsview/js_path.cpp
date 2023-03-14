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

#include "frameworks/bridge/declarative_frontend/jsview/js_path.h"

#include "core/components/shape/shape_component.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSPath::Create(const JSCallbackInfo& info)
{
    RefPtr<Component> component = AceType::MakeRefPtr<OHOS::Ace::ShapeComponent>(ShapeType::PATH);
    ViewStackProcessor::GetInstance()->Push(component);
    JSShapeAbstract::SetSize(info);
    if (info.Length() > 0 && info[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> commands = obj->GetProperty("commands");
        if (commands->IsString()) {
            SetCommands(commands->ToString());
        }
    }
}

void JSPath::SetCommands(const std::string& commands)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeComponent>(stack->GetMainComponent());
    if (component) {
        AnimationOption option = stack->GetImplicitAnimationOption();
        component->SetPathCmd(commands, option);
    }
}

void JSPath::ObjectCommands(const JSCallbackInfo& info)
{
    info.ReturnSelf();
    if (info.Length() > 0 && info[0]->IsString()) {
        auto path = AceType::DynamicCast<Path>(basicShape_);
        if (path) {
            path->SetValue(info[0]->ToString());
        }
    }
}

void JSPath::ConstructorCallback(const JSCallbackInfo& info)
{
    auto jsPath = AceType::MakeRefPtr<JSPath>();
    auto path = AceType::MakeRefPtr<Path>();
    if (info.Length() > 0 && info[0]->IsObject()) {
        JSRef<JSObject> params = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> commands = params->GetProperty("commands");
        if (commands->IsString()) {
            path->SetValue(commands->ToString());
        }
    }
    jsPath->SetBasicShape(path);
    jsPath->IncRefCount();
    info.SetReturnValue(AceType::RawPtr(jsPath));
}

void JSPath::DestructorCallback(JSPath* jsPath)
{
    if (jsPath != nullptr) {
        jsPath->DecRefCount();
    }
}

void JSPath::JSBind(BindingTarget globalObj)
{
    JSClass<JSPath>::Declare("Path");
    JSClass<JSPath>::StaticMethod("create", &JSPath::Create);
    JSClass<JSPath>::StaticMethod("commands", &JSPath::SetCommands);

    JSClass<JSPath>::CustomMethod("commands", &JSPath::ObjectCommands);
    JSClass<JSPath>::CustomMethod("offset", &JSShapeAbstract::ObjectOffset);
    JSClass<JSPath>::CustomMethod("fill", &JSShapeAbstract::ObjectFill);

    JSClass<JSPath>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSPath>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSPath>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSPath>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSPath>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSPath>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);

    JSClass<JSPath>::Inherit<JSShapeAbstract>();
    JSClass<JSPath>::Bind(globalObj, JSPath::ConstructorCallback, JSPath::DestructorCallback);
}

} // namespace OHOS::Ace::Framework
