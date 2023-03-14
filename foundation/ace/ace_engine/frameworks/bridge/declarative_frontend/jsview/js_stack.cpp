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

#include "frameworks/bridge/declarative_frontend/jsview/js_stack.h"

#include "base/log/ace_trace.h"
#include "frameworks/bridge/declarative_frontend/engine/js_ref_ptr.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

const static std::array<Alignment, 9> ALIGNMENT_ARR { Alignment::TOP_LEFT, Alignment::TOP_CENTER, Alignment::TOP_RIGHT,
    Alignment::CENTER_LEFT, Alignment::CENTER, Alignment::CENTER_RIGHT, Alignment::BOTTOM_LEFT,
    Alignment::BOTTOM_CENTER, Alignment::BOTTOM_RIGHT };

void JSStack::SetStackFit(int value)
{
    if (value >= (int)StackFit::KEEP && value <= (int)StackFit::FIRST_CHILD) {
        auto stack = AceType::DynamicCast<StackComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (stack) {
            stack->SetStackFit((StackFit)value);
        }
    } else {
        LOGE("Invalid value for stackfit");
    }
}

void JSStack::SetOverflow(int value)
{
    if (value >= (int)Overflow::CLIP && value <= (int)Overflow::OBSERVABLE) {
        auto stack = AceType::DynamicCast<StackComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (stack) {
            stack->SetOverflow((Overflow)value);
        }
    } else {
        LOGE("Invalid value for overflow");
    }
}

void JSStack::SetAlignment(int value)
{
    Alignment alignment = Alignment::TOP_LEFT;

    switch (value) {
        case 0:
            alignment = Alignment::TOP_LEFT;
            break;
        case 1:
            alignment = Alignment::TOP_CENTER;
            break;
        case 2:
            alignment = Alignment::TOP_RIGHT;
            break;
        case 3:
            alignment = Alignment::CENTER_LEFT;
            break;
        case 4:
            alignment = Alignment::CENTER;
            break;
        case 5:
            alignment = Alignment::CENTER_RIGHT;
            break;
        case 6:
            alignment = Alignment::BOTTOM_LEFT;
            break;
        case 7:
            alignment = Alignment::BOTTOM_CENTER;
            break;
        case 8:
            alignment = Alignment::BOTTOM_RIGHT;
            break;
        default:
            LOGE("Invalid value for alignment");
            return;
    }

    auto stack = AceType::DynamicCast<StackComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (stack) {
        stack->SetAlignment(alignment);
    }
}

void JSStack::SetWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    SetWidth(info[0]);
}

void JSStack::SetWidth(const JSRef<JSVal>& jsValue)
{
    JSViewAbstract::JsWidth(jsValue);

    auto stack = AceType::DynamicCast<StackComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (stack) {
        if (stack->GetMainStackSize() == MainStackSize::MAX || stack->GetMainStackSize() == MainStackSize::MAX_Y) {
            stack->SetMainStackSize(MainStackSize::MAX);
        } else {
            stack->SetMainStackSize(MainStackSize::MAX_X);
        }
    }
}

void JSStack::SetHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    SetHeight(info[0]);
}

void JSStack::SetHeight(const JSRef<JSVal>& jsValue)
{
    JSViewAbstract::JsHeight(jsValue);
    auto stack = AceType::DynamicCast<StackComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (stack) {
        if (stack->GetMainStackSize() == MainStackSize::MAX || stack->GetMainStackSize() == MainStackSize::MAX_X) {
            stack->SetMainStackSize(MainStackSize::MAX);
        } else {
            stack->SetMainStackSize(MainStackSize::MAX_Y);
        }
    }
}

void JSStack::SetSize(const JSCallbackInfo& info)
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
    SetWidth(sizeObj->GetProperty("width"));
    SetHeight(sizeObj->GetProperty("height"));
}

void JSStack::JSBind(BindingTarget globalObj)
{
    JSClass<JSStack>::Declare("Stack");

    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSStack>::StaticMethod("create", &JSStack::Create, opt);
    JSClass<JSStack>::StaticMethod("stackFit", &JSStack::SetStackFit, opt);
    JSClass<JSStack>::StaticMethod("overflow", &JSStack::SetOverflow, opt);
    JSClass<JSStack>::StaticMethod("alignContent", &JSStack::SetAlignment, opt);
    JSClass<JSStack>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSStack>::StaticMethod("width", SetWidth);
    JSClass<JSStack>::StaticMethod("height", SetHeight);
    JSClass<JSStack>::StaticMethod("size", SetSize);
    JSClass<JSStack>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSStack>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSStack>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSStack>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSStack>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSStack>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSStack>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);
    JSClass<JSStack>::Inherit<JSContainerBase>();
    JSClass<JSStack>::Inherit<JSViewAbstract>();
    JSClass<JSStack>::Bind<>(globalObj);
}

void JSStack::Create(const JSCallbackInfo& info)
{
    Alignment alignment = Alignment::CENTER;

    if (info.Length() > 0 && info[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> stackAlign = obj->GetProperty("alignContent");
        if (stackAlign->IsNumber()) {
            int32_t value = stackAlign->ToNumber<int32_t>();
            alignment = (value >= 0 && value < static_cast<int>(ALIGNMENT_ARR.size())) ? ALIGNMENT_ARR[value]
                                                                                       : Alignment::CENTER;
        }
    }

    std::list<RefPtr<Component>> children;
    RefPtr<OHOS::Ace::StackComponent> component =
        AceType::MakeRefPtr<StackComponent>(alignment, StackFit::KEEP, Overflow::OBSERVABLE, children);
    ViewStackProcessor::GetInstance()->Push(component);
    JSInteractableView::SetFocusNode(true);
}

} // namespace OHOS::Ace::Framework
