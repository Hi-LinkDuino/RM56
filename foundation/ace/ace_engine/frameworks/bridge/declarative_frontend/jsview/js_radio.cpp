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

#include "bridge/declarative_frontend/jsview/js_radio.h"

#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components/checkable/checkable_component.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"

namespace OHOS::Ace::Framework {

void JSRadio::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("radio create error, info is not-valid");
        return;
    }
    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto groupName = paramObject->GetProperty("group");
    auto radioValue = paramObject->GetProperty("value");
    RefPtr<RadioTheme> radioTheme = GetTheme<RadioTheme>();
    auto radioComponent = AceType::MakeRefPtr<OHOS::Ace::RadioComponent<std::string>>(radioTheme);
    auto radioGroupName = groupName->ToString();
    std::string value = radioValue->ToString();
    radioComponent->SetValue(value);
    auto radioGroupComponent = ViewStackProcessor::GetInstance()->GetRadioGroupCompnent();
    radioComponent->SetGroupName(radioGroupName);
    auto& radioGroup = (*radioGroupComponent)[radioGroupName];
    radioGroup.SetIsDeclarative(true);
    radioGroup.AddRadio(radioComponent);
    ViewStackProcessor::GetInstance()->Push(radioComponent);
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    box->SetDeliverMinToChild(true);
    box->SetWidth(radioTheme->GetWidth());
    box->SetHeight(radioTheme->GetHeight());
}

void JSRadio::JSBind(BindingTarget globalObj)
{
    JSClass<JSRadio>::Declare("Radio");

    JSClass<JSRadio>::StaticMethod("create", &JSRadio::Create);
    JSClass<JSRadio>::StaticMethod("checked", &JSRadio::Checked);
    JSClass<JSRadio>::StaticMethod("width", &JSRadio::JsWidth);
    JSClass<JSRadio>::StaticMethod("height", &JSRadio::JsHeight);
    JSClass<JSRadio>::StaticMethod("size", &JSRadio::JsSize);
    JSClass<JSRadio>::StaticMethod("padding", &JSRadio::JsPadding);
    JSClass<JSRadio>::StaticMethod("onChange", &JSRadio::OnChange);
    JSClass<JSRadio>::StaticMethod("onClick", &JSRadio::JsOnClick);
    JSClass<JSRadio>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSRadio>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSRadio>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSRadio>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSRadio>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSRadio>::Inherit<JSViewAbstract>();
    JSClass<JSRadio>::Bind<>(globalObj);
}

void JSRadio::Checked(bool checked)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto radioComponent = AceType::DynamicCast<RadioComponent<std::string>>(stack->GetMainComponent());
    if (checked) {
        radioComponent->SetGroupValue(radioComponent->GetValue());
        radioComponent->SetOriginChecked(checked);
    } else {
        radioComponent->SetGroupValue("");
    }
}

void JSRadio::Checked(const JSCallbackInfo& info)
{
    if (info[0]->IsBoolean()) {
        Checked(info[0]->ToBoolean());
    }
}


void JSRadio::JsWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsWidth(info[0]);
}

void JSRadio::JsWidth(const JSRef<JSVal>& jsValue)
{
    Dimension value;
    if (!ParseJsDimensionVp(jsValue, value)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto box = stack->GetBoxComponent();
    auto radioComponent = AceType::DynamicCast<RadioComponent<std::string>>(stack->GetMainComponent());
    if (radioComponent) {
        Dimension padding;
        padding = radioComponent->GetHotZoneHorizontalPadding();
        radioComponent->SetWidth(value + padding * 2);
        box->SetWidth(value + padding * 2);
    }
}

void JSRadio::JsHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsHeight(info[0]);
}

void JSRadio::JsHeight(const JSRef<JSVal>& jsValue)
{
    Dimension value;
    if (!ParseJsDimensionVp(jsValue, value)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto box = stack->GetBoxComponent();
    Dimension padding;
    auto radioComponent = AceType::DynamicCast<RadioComponent<std::string>>(stack->GetMainComponent());
    if (radioComponent) {
        padding = radioComponent->GetHotZoneVerticalPadding();
        radioComponent->SetHeight(value + padding * 2);
        box->SetHeight(value + padding * 2);
    }
}

void JSRadio::JsSize(const JSCallbackInfo& info)
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

void JSRadio::JsPadding(const JSCallbackInfo& info)
{
    if (!info[0]->IsString() && !info[0]->IsNumber() && !info[0]->IsObject()) {
        LOGE("arg is not a string, number or object.");
        return;
    }

    if (info[0]->IsObject()) {
        auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
        if (!argsPtrItem || argsPtrItem->IsNull()) {
            LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
            return;
        }
        if (argsPtrItem->Contains("top") || argsPtrItem->Contains("bottom") || argsPtrItem->Contains("left") ||
            argsPtrItem->Contains("right")) {
            Dimension topDimen = Dimension(0.0, DimensionUnit::VP);
            Dimension leftDimen = Dimension(0.0, DimensionUnit::VP);
            Dimension rightDimen = Dimension(0.0, DimensionUnit::VP);
            Dimension bottomDimen = Dimension(0.0, DimensionUnit::VP);
            ParseJsonDimensionVp(argsPtrItem->GetValue("top"), topDimen);
            ParseJsonDimensionVp(argsPtrItem->GetValue("left"), leftDimen);
            ParseJsonDimensionVp(argsPtrItem->GetValue("right"), rightDimen);
            ParseJsonDimensionVp(argsPtrItem->GetValue("bottom"), bottomDimen);
            if (leftDimen == 0.0_vp) {
                leftDimen = rightDimen;
            }
            if (topDimen == 0.0_vp) {
                topDimen = bottomDimen;
            }
            if (leftDimen == 0.0_vp) {
                leftDimen = topDimen;
            }
            SetPadding(topDimen, leftDimen);
            return;
        }
    }
    Dimension length;
    if (ParseJsDimensionVp(info[0], length)) {
        SetPadding(length, length);
    }
}

void JSRadio::SetPadding(const Dimension& topDimen, const Dimension& leftDimen)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto radioComponent = AceType::DynamicCast<RadioComponent<std::string>>(stack->GetMainComponent());
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();

    if (radioComponent) {
        auto width = radioComponent->GetWidth() - radioComponent->GetHotZoneHorizontalPadding();
        auto height = radioComponent->GetHeight() - radioComponent->GetHotZoneVerticalPadding();
        radioComponent->SetHeight(height + topDimen * 2);
        radioComponent->SetWidth(width + leftDimen * 2);
        box->SetHeight(height + topDimen * 2);
        box->SetWidth(width + leftDimen * 2);
        radioComponent->SetHotZoneVerticalPadding(topDimen);
        radioComponent->SetHorizontalPadding(leftDimen);
    }
}

void JSRadio::OnChange(const JSCallbackInfo& args)
{
    if (!JSViewBindEvent(&CheckableComponent::SetOnChange, args)) {
        LOGW("Failed to bind event");
    }
    args.ReturnSelf();
}

void JSRadio::JsOnClick(const JSCallbackInfo& args)
{
    if (JSViewBindEvent(&CheckableComponent::SetOnClick, args)) {
    } else {
        LOGW("Failed to bind event");
    }

    args.ReturnSelf();
}

} // namespace OHOS::Ace::Framework
