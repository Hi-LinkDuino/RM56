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

#include "frameworks/bridge/declarative_frontend/jsview/js_row.h"

#include "base/log/ace_trace.h"
#include "core/components/wrap/wrap_component.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSRow::Create(const JSCallbackInfo& info)
{
    std::list<RefPtr<Component>> children;
    RefPtr<RowComponent> rowComponent =
        AceType::MakeRefPtr<OHOS::Ace::RowComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, children);
    rowComponent->SetMainAxisSize(MainAxisSize::MIN);
    rowComponent->SetCrossAxisSize(CrossAxisSize::MIN);

    if (info.Length() > 0 && info[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> space = obj->GetProperty("space");
        Dimension value;
        if (ParseJsDimensionVp(space, value)) {
            rowComponent->SetSpace(value);
        }

        JSRef<JSVal> useAlign = obj->GetProperty("useAlign");
        if (useAlign->IsObject()) {
            VerticalAlignDeclaration* declaration =
                JSRef<JSObject>::Cast(useAlign)->Unwrap<VerticalAlignDeclaration>();
            if (declaration != nullptr) {
                rowComponent->SetAlignDeclarationPtr(declaration);
            }
        }
    }

    ViewStackProcessor::GetInstance()->Push(rowComponent);
}

void JSRow::CreateWithWrap(const JSCallbackInfo& info)
{
    std::list<RefPtr<Component>> children;
    RefPtr<OHOS::Ace::WrapComponent> component = AceType::MakeRefPtr<WrapComponent>(0.0, 0.0, children);

    component->SetDirection(WrapDirection::HORIZONTAL);
    component->SetMainAlignment(WrapAlignment::START);
    component->SetCrossAlignment(WrapAlignment::START);
    component->SetAlignment(WrapAlignment::START);
    component->SetDialogStretch(false);

    ViewStackProcessor::GetInstance()->Push(component);
}

void VerticalAlignDeclaration::ConstructorCallback(const JSCallbackInfo& args)
{
    auto align = VerticalAlign::CENTER;
    if (args.Length() > 0 && args[0]->IsNumber()) {
        auto value = args[0]->ToNumber<int32_t>();
        if (value >= static_cast<int32_t>(VerticalAlign::TOP) && value <= static_cast<int32_t>(VerticalAlign::BOTTOM)) {
            align = static_cast<VerticalAlign>(value);
        }
    }
    auto obj = new VerticalAlignDeclaration(align);
    args.SetReturnValue(obj);
}

void VerticalAlignDeclaration::DestructorCallback(VerticalAlignDeclaration* obj)
{
    delete obj;
}

void JSRow::JSBind(BindingTarget globalObj)
{
    JSClass<JSRow>::Declare("Row");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSRow>::StaticMethod("create", &JSRow::Create, opt);
    JSClass<JSRow>::StaticMethod("createWithWrap", &JSRow::CreateWithWrap, opt);
    JSClass<JSRow>::StaticMethod("fillParent", &JSFlex::SetFillParent, opt);
    JSClass<JSRow>::StaticMethod("wrapContent", &JSFlex::SetWrapContent, opt);
    JSClass<JSRow>::StaticMethod("justifyContent", &JSFlex::SetJustifyContent, opt);
    JSClass<JSRow>::StaticMethod("alignItems", &JSFlex::SetAlignItems, opt);
    JSClass<JSRow>::StaticMethod("alignContent", &JSFlex::SetAlignContent, opt);
    JSClass<JSRow>::StaticMethod("height", &JSFlex::JsHeight, opt);
    JSClass<JSRow>::StaticMethod("width", &JSFlex::JsWidth, opt);
    JSClass<JSRow>::StaticMethod("size", &JSFlex::JsSize, opt);
    JSClass<JSRow>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSRow>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSRow>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSRow>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSRow>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSRow>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSRow>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSRow>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);
    JSClass<JSRow>::Inherit<JSContainerBase>();
    JSClass<JSRow>::Inherit<JSViewAbstract>();
    JSClass<JSRow>::Bind<>(globalObj);

    JSClass<VerticalAlignDeclaration>::Declare("VerticalAlignDeclaration");
    JSClass<VerticalAlignDeclaration>::Bind(
        globalObj, VerticalAlignDeclaration::ConstructorCallback, VerticalAlignDeclaration::DestructorCallback);
}

} // namespace OHOS::Ace::Framework
