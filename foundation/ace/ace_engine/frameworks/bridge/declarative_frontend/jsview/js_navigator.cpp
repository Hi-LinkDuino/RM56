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

#include "frameworks/bridge/declarative_frontend/jsview/js_navigator.h"

#include "core/components/box/box_component.h"
#include "core/components/navigator/navigator_component.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSNavigator::Create(const JSCallbackInfo& info)
{
    LOGD("Create component: JSNavigator");
    // when the navigator is created, we don't know the child. so I use an empty box here
    RefPtr<OHOS::Ace::Component> child;
    auto navigatorComponent = AceType::MakeRefPtr<OHOS::Ace::NavigatorComponent>(child);

    if (info.Length() > 0 && info[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> target = obj->GetProperty("target");
        if (target->IsString()) {
            navigatorComponent->SetUri(target->ToString());
        }

        JSRef<JSVal> type = obj->GetProperty("type");
        if (type->IsNumber()) {
            NavigatorType navigatorType = NavigatorType(type->ToNumber<uint32_t>());
            if (navigatorType == NavigatorType::DEFAULT) {
                navigatorComponent->SetType(NavigatorType::PUSH);
            } else {
                navigatorComponent->SetType(navigatorType);
            }
        }
    } else {
        navigatorComponent->SetType(NavigatorType::BACK);
    }

    ViewStackProcessor::GetInstance()->Push(navigatorComponent);
}

void JSNavigator::JSBind(BindingTarget globalObj)
{
    JSClass<JSNavigator>::Declare("Navigator");

    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSNavigator>::StaticMethod("create", &JSNavigator::Create, opt);
    JSClass<JSNavigator>::StaticMethod("active", &JSNavigator::SetActive, opt);
    JSClass<JSNavigator>::StaticMethod("params", &JSNavigator::SetParams, opt);
    JSClass<JSNavigator>::StaticMethod("width", &JSNavigator::JsWidth);
    JSClass<JSNavigator>::StaticMethod("height", &JSNavigator::JsHeight);
    JSClass<JSNavigator>::StaticMethod("size", &JSNavigator::JsSize);
    JSClass<JSNavigator>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSNavigator>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSNavigator>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSNavigator>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSNavigator>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSNavigator>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSNavigator>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSNavigator>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);
    JSClass<JSNavigator>::Inherit<JSContainerBase>();
    JSClass<JSNavigator>::Inherit<JSViewAbstract>();
    JSClass<JSNavigator>::Bind<>(globalObj);
}

void JSNavigator::SetActive(bool active)
{
    auto navigator = AceType::DynamicCast<NavigatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (navigator) {
        navigator->SetActive(active);
    }
}

void JSNavigator::SetParams(const JSCallbackInfo& args)
{
    auto navigator = AceType::DynamicCast<NavigatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (navigator) {
        if (args.Length() > 0 && args[0]->IsObject()) {
            JSRef<JSVal> val = JSRef<JSVal>::Cast(args[0]);
            navigator->SetParams(val->ToString());
        }
    }
}

void JSNavigator::JsWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsWidth(info[0]);
}

void JSNavigator::JsWidth(const JSRef<JSVal>& jsValue)
{
    auto navigator = AceType::DynamicCast<NavigatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (navigator) {
        navigator->SetIsDefWidth(true);
    }
    JSViewAbstract::JsWidth(jsValue);
}

void JSNavigator::JsHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsHeight(info[0]);
}

void JSNavigator::JsHeight(const JSRef<JSVal>& jsValue)
{
    auto navigator = AceType::DynamicCast<NavigatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (navigator) {
        navigator->SetIsDefHeight(true);
    }
    JSViewAbstract::JsHeight(jsValue);
}

void JSNavigator::JsSize(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    if (!info[0]->IsObject()) {
        LOGE("arg is not Object or String.");
        return;
    }

    JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);
    JsWidth(sizeObj->GetProperty("width"));
    JsHeight(sizeObj->GetProperty("height"));
}

} // namespace OHOS::Ace::Framework
