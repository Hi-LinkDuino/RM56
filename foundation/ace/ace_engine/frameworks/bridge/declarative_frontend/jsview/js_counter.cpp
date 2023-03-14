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

#include "frameworks/bridge/declarative_frontend/jsview/js_counter.h"

#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/declarative_frontend/engine/bindings.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_click_function.h"
#include "frameworks/bridge/declarative_frontend/engine/js_ref_ptr.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"
#include "frameworks/core/components/counter/counter_theme.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr Dimension COUNTER_DEFAULT_HEIGHT = 32.0_vp;
constexpr Dimension COUNTER_DEFAULT_WIDTH = 100.0_vp;
constexpr Dimension COUNTER_DEFAULT_CONTROLWIDTH = 32.0_vp;
constexpr Dimension COUNTER_DEFAULT_RADIUS = 4.0_vp;

} // namespace

void JSCounter::JSBind(BindingTarget globalObj)
{
    JSClass<JSCounter>::Declare("Counter");
    JSClass<JSCounter>::StaticMethod("create", &JSCounter::Create, MethodOptions::NONE);
    JSClass<JSCounter>::StaticMethod("onInc", &JSCounter::JsOnInc);
    JSClass<JSCounter>::StaticMethod("onDec", &JSCounter::JsOnDec);
    JSClass<JSCounter>::StaticMethod("height", &JSCounter::JSHeight);
    JSClass<JSCounter>::StaticMethod("width", &JSCounter::JSWidth);
    JSClass<JSCounter>::StaticMethod("size", &JSCounter::SetSize);
    JSClass<JSCounter>::StaticMethod("controlWidth", &JSCounter::JSControlwidth);
    JSClass<JSCounter>::StaticMethod("state", &JSCounter::JSStateChange);
    JSClass<JSCounter>::StaticMethod("backgroundColor", &JSCounter::JsBackgroundColor);
    JSClass<JSCounter>::Inherit<JSContainerBase>();
    JSClass<JSCounter>::Bind(globalObj);
}

void JSCounter::JsOnInc(const JSCallbackInfo& args)
{
    if (!JSViewBindEvent(&CounterComponent::SetOnInc, args)) {
        LOGW("Failed to bind event");
    }
    args.ReturnSelf();
}

void JSCounter::JsOnDec(const JSCallbackInfo& args)
{
    if (!JSViewBindEvent(&CounterComponent::SetOnDec, args)) {
        LOGW("Failed to bind event");
    }
    args.ReturnSelf();
}

void JSCounter::JSHeight(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    Dimension value;
    if (!ConvertFromJSValue(args[0], value)) {
        LOGE("args can not set height");
        return;
    }

    if (LessNotEqual(value.Value(), 0.0)) {
        return;
    }

    JSViewSetProperty(&CounterComponent::SetHeight, value);
    args.ReturnSelf();
}

void JSCounter::JSWidth(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    Dimension value;
    if (!ConvertFromJSValue(args[0], value)) {
        LOGE("args can not set width");
        return;
    }

    if (LessNotEqual(value.Value(), 0.0)) {
        return;
    }

    JSViewSetProperty(&CounterComponent::SetWidth, value);
    args.ReturnSelf();
}

void JSCounter::SetSize(const JSCallbackInfo& args)
{
    if (args.Length() >= 1 && args[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);

        Dimension height;
        if (ConvertFromJSValue(obj->GetProperty("height"), height) && height.IsValid()) {
            if (GreatOrEqual(height.Value(), 0.0)) {
                JSViewSetProperty(&CounterComponent::SetHeight, height);
            }
        }

        Dimension width;
        if (ConvertFromJSValue(obj->GetProperty("width"), width) && width.IsValid()) {
            if (GreatOrEqual(width.Value(), 0.0)) {
                JSViewSetProperty(&CounterComponent::SetWidth, width);
            }
        }
    }
    args.ReturnSelf();
}

void JSCounter::JSControlwidth(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    Dimension value;
    if (!ConvertFromJSValue(args[0], value)) {
        LOGE("args can not set controlwidth");
        return;
    }

    JSViewSetProperty(&CounterComponent::SetControlWidth, value);
    args.ReturnSelf();
}

void JSCounter::JSStateChange(bool state)
{
    JSViewSetProperty(&CounterComponent::SetState, state);
}

void JSCounter::JsBackgroundColor(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    if (!args[0]->IsString() && !args[0]->IsNumber()) {
        LOGE("arg is not a string or number.");
        return;
    }

    Color color;
    if (!ConvertFromJSValue(args[0], color)) {
        LOGE("args can not set backgroundColor");
        return;
    }
    JSViewSetProperty(&CounterComponent::SetBackgroundColor, color);
    args.ReturnSelf();
}

void JSCounter::Create()
{
    std::list<RefPtr<Component>> children;
    RefPtr<OHOS::Ace::CounterComponent> component = AceType::MakeRefPtr<CounterComponent>(children);
    ViewStackProcessor::GetInstance()->Push(component);

    component->SetHeight(COUNTER_DEFAULT_HEIGHT);
    component->SetWidth(COUNTER_DEFAULT_WIDTH);
    component->SetControlWidth(COUNTER_DEFAULT_CONTROLWIDTH);
    component->SetControlRadius(COUNTER_DEFAULT_RADIUS);
}

} // namespace OHOS::Ace::Framework
