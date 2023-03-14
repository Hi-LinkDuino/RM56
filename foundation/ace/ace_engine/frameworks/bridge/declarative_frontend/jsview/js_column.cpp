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

#include "frameworks/bridge/declarative_frontend/jsview/js_column.h"

#include "base/log/ace_trace.h"
#include "core/components/wrap/wrap_component.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {
std::string JSColumn::inspectorTag_ = "";

void JSColumn::Create(const JSCallbackInfo& info)
{
    std::list<RefPtr<Component>> children;
    RefPtr<ColumnComponent> columnComponent =
        AceType::MakeRefPtr<OHOS::Ace::ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, children);
    columnComponent->SetMainAxisSize(MainAxisSize::MIN);
    columnComponent->SetCrossAxisSize(CrossAxisSize::MIN);

    if (info.Length() > 0 && info[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> space = obj->GetProperty("space");

        Dimension value;
        if (ParseJsDimensionVp(space, value)) {
            columnComponent->SetSpace(value);
        }
        JSRef<JSVal> useAlign = obj->GetProperty("useAlign");
        if (useAlign->IsObject()) {
            HorizontalAlignDeclaration* declaration =
                JSRef<JSObject>::Cast(useAlign)->Unwrap<HorizontalAlignDeclaration>();
            if (declaration != nullptr) {
                columnComponent->SetAlignDeclarationPtr(declaration);
            }
        }
    }
    columnComponent->SetInspectorTag(inspectorTag_);
    ViewStackProcessor::GetInstance()->Push(columnComponent, false);
    JSInteractableView::SetFocusNode(true);
}

void JSColumn::CreateWithWrap(const JSCallbackInfo& info)
{
    std::list<RefPtr<Component>> children;
    RefPtr<OHOS::Ace::WrapComponent> component = AceType::MakeRefPtr<WrapComponent>(0.0, 0.0, children);

    component->SetDirection(WrapDirection::VERTICAL);
    component->SetMainAlignment(WrapAlignment::START);
    component->SetCrossAlignment(WrapAlignment::START);
    component->SetAlignment(WrapAlignment::START);
    component->SetDialogStretch(false);

    ViewStackProcessor::GetInstance()->Push(component);
}

void JSColumn::SetInspectorTag(const std::string& inspectorTag)
{
    inspectorTag_ = inspectorTag;
}

void JSColumn::ClearInspectorTag()
{
    inspectorTag_.clear();
}


void HorizontalAlignDeclaration::ConstructorCallback(const JSCallbackInfo& args)
{
    auto align = HorizontalAlign::CENTER;
    if (args.Length() > 0 && args[0]->IsNumber()) {
        auto value = args[0]->ToNumber<int32_t>();
        if (value >= static_cast<int32_t>(HorizontalAlign::START) &&
            value <= static_cast<int32_t>(HorizontalAlign::END)) {
            align = static_cast<HorizontalAlign>(value);
        }
    }
    auto obj = new HorizontalAlignDeclaration(align);
    args.SetReturnValue(obj);
}

void HorizontalAlignDeclaration::DestructorCallback(HorizontalAlignDeclaration* obj)
{
    delete obj;
}

void JSColumn::JSBind(BindingTarget globalObj)
{
    JSClass<JSColumn>::Declare("Column");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSColumn>::StaticMethod("create", &JSColumn::Create, opt);
    JSClass<JSColumn>::StaticMethod("createWithWrap", &JSColumn::CreateWithWrap, opt);
    JSClass<JSColumn>::StaticMethod("fillParent", &JSFlex::SetFillParent, opt);
    JSClass<JSColumn>::StaticMethod("wrapContent", &JSFlex::SetWrapContent, opt);
    JSClass<JSColumn>::StaticMethod("justifyContent", &JSFlex::SetJustifyContent, opt);
    JSClass<JSColumn>::StaticMethod("alignItems", &JSFlex::SetAlignItems, opt);
    JSClass<JSColumn>::StaticMethod("alignContent", &JSFlex::SetAlignContent, opt);
    JSClass<JSColumn>::StaticMethod("height", &JSFlex::JsHeight, opt);
    JSClass<JSColumn>::StaticMethod("width", &JSFlex::JsWidth, opt);
    JSClass<JSColumn>::StaticMethod("size", &JSFlex::JsSize, opt);
    JSClass<JSColumn>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSColumn>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSColumn>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSColumn>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSColumn>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSColumn>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSColumn>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSColumn>::StaticMethod("onPan", &JSInteractableView::JsOnPan);
    JSClass<JSColumn>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);
    JSClass<JSColumn>::Inherit<JSContainerBase>();
    JSClass<JSColumn>::Inherit<JSViewAbstract>();
    JSClass<JSColumn>::Bind<>(globalObj);

    JSClass<HorizontalAlignDeclaration>::Declare("HorizontalAlignDeclaration");
    JSClass<HorizontalAlignDeclaration>::Bind(
        globalObj, HorizontalAlignDeclaration::ConstructorCallback, HorizontalAlignDeclaration::DestructorCallback);
}

} // namespace OHOS::Ace::Framework
