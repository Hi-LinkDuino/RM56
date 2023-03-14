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

#include "frameworks/bridge/declarative_frontend/jsview/js_toggle.h"

#include <string>

#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/view_stack_processor.h"

#include "core/components/toggle/toggle_component.h"
#include "core/components/toggle/toggle_theme.h"

namespace OHOS::Ace::Framework {

void JSToggle::JSBind(BindingTarget globalObj)
{
    JSClass<JSToggle>::Declare("Toggle");
    JSClass<JSToggle>::StaticMethod("create", &JSToggle::Create);
    JSClass<JSToggle>::StaticMethod("onChange", &JSToggle::OnChange);
    JSClass<JSToggle>::StaticMethod("selectedColor", &JSToggle::SelectedColor);
    JSClass<JSToggle>::StaticMethod("width", &JSToggle::JsWidth);
    JSClass<JSToggle>::StaticMethod("height", &JSToggle::JsHeight);
    JSClass<JSToggle>::StaticMethod("size", &JSToggle::JsSize);
    JSClass<JSToggle>::StaticMethod("padding", &JSToggle::JsPadding);
    JSClass<JSToggle>::StaticMethod("switchPointColor", &JSToggle::SwitchPointColor);
    JSClass<JSToggle>::Inherit<JSViewAbstract>();
    JSClass<JSToggle>::Bind(globalObj);
}

void JSToggle::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("toggle create error, info is non-valid");
        return;
    }

    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto type = paramObject->GetProperty("type");
    if (!type->IsNumber()) {
        LOGE("toggle create error, type is non-valid");
        return;
    }

    auto tempIsOn = paramObject->GetProperty("isOn");
    bool isOn = tempIsOn->IsBoolean() ? tempIsOn->ToBoolean() : false;
    auto toggleType = static_cast<ToggleType>(type->ToNumber<int32_t>());
    RefPtr<Component> component;
    if (toggleType == ToggleType::CHECKBOX) {
        RefPtr<CheckboxTheme> checkBoxTheme = GetTheme<CheckboxTheme>();
        RefPtr<CheckboxComponent> checkboxComponent = AceType::MakeRefPtr<OHOS::Ace::CheckboxComponent>(checkBoxTheme);
        checkboxComponent->SetValue(isOn);
        component = checkboxComponent;
    } else if (toggleType == ToggleType::SWITCH) {
        RefPtr<SwitchTheme> switchTheme = GetTheme<SwitchTheme>();
        RefPtr<SwitchComponent> switchComponent = AceType::MakeRefPtr<OHOS::Ace::SwitchComponent>(switchTheme);
        switchComponent->SetValue(isOn);
        component = switchComponent;
    } else {
        RefPtr<ToggleTheme> toggleTheme = GetTheme<ToggleTheme>();
        RefPtr<ToggleComponent> toggleComponent = AceType::MakeRefPtr<ToggleComponent>();
        toggleComponent->SetBackgroundColor(toggleTheme->GetBackgroundColor());
        toggleComponent->SetCheckedColor(toggleTheme->GetCheckedColor());
        toggleComponent->SetPressedBlendColor(toggleTheme->GetPressedBlendColor());
        toggleComponent->SetCheckedState(isOn);
        component = toggleComponent;
    }

    ViewStackProcessor::GetInstance()->Push(component);
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    box->SetDeliverMinToChild(true);
    if (toggleType == ToggleType::CHECKBOX) {
        RefPtr<CheckboxTheme> checkBoxTheme = GetTheme<CheckboxTheme>();
        auto horizontalPadding = checkBoxTheme->GetHotZoneHorizontalPadding();
        auto verticalPadding = checkBoxTheme->GetHotZoneVerticalPadding();
        box->SetWidth(checkBoxTheme->GetWidth() + horizontalPadding * 2);
        box->SetHeight(checkBoxTheme->GetHeight() + verticalPadding * 2);
    } else if (toggleType == ToggleType::SWITCH) {
        RefPtr<SwitchTheme> switchTheme = GetTheme<SwitchTheme>();
        auto horizontalPadding = switchTheme->GetHotZoneHorizontalPadding();
        auto verticalPadding = switchTheme->GetHotZoneVerticalPadding();
        box->SetWidth(switchTheme->GetWidth() + horizontalPadding * 2);
        box->SetHeight(switchTheme->GetHeight() + verticalPadding * 2);
    } else {
        RefPtr<ToggleTheme> toggleTheme = GetTheme<ToggleTheme>();
        box->SetHeight(toggleTheme->GetHeight().Value(), toggleTheme->GetHeight().Unit());
    }
}

void JSToggle::JsWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsWidth(info[0]);
}

void JSToggle::JsWidth(const JSRef<JSVal>& jsValue)
{
    Dimension value;
    if (!ParseJsDimensionVp(jsValue, value)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    Dimension padding;
    auto box = stack->GetBoxComponent();
    auto checkableComponent = AceType::DynamicCast<CheckableComponent>(stack->GetMainComponent());
    if (checkableComponent) {
        padding = checkableComponent->GetHotZoneHorizontalPadding();
        checkableComponent->SetWidth(value + padding * 2);
        box->SetWidth(value + padding * 2);
    }

    auto toggleComponent = AceType::DynamicCast<ToggleComponent>(stack->GetMainComponent());
    if (toggleComponent) {
        toggleComponent->SetWidth(value);
        box->SetWidth(value);
    }
}

void JSToggle::JsHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsHeight(info[0]);
}

void JSToggle::JsHeight(const JSRef<JSVal>& jsValue)
{
    Dimension value;
    if (!ParseJsDimensionVp(jsValue, value)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto box = stack->GetBoxComponent();
    Dimension padding;
    auto checkableComponent = AceType::DynamicCast<CheckableComponent>(stack->GetMainComponent());
    if (checkableComponent) {
        padding = checkableComponent->GetHotZoneVerticalPadding();
        checkableComponent->SetHeight(value + padding * 2);
        box->SetHeight(value + padding * 2);
    }

    auto toggleComponent = AceType::DynamicCast<ToggleComponent>(stack->GetMainComponent());
    if (toggleComponent) {
        toggleComponent->SetHeight(value);
        box->SetHeight(value);
    }
}

void JSToggle::JsSize(const JSCallbackInfo& info)
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

void JSToggle::OnChange(const JSCallbackInfo& args)
{
    if (JSViewBindEvent(&ToggleComponent::SetOnChange, args) ||
        JSViewBindEvent(&CheckableComponent::SetOnChange, args)) {
    } else {
        LOGW("Failed to bind event");
    }

    args.ReturnSelf();
}

void JSToggle::SelectedColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }
    Color selectedColor;
    if (!ParseJsColor(info[0], selectedColor)) {
        return;
    }
    auto mainComponent = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto toggle = AceType::DynamicCast<ToggleComponent>(mainComponent);
    if (toggle) {
        toggle->SetCheckedColor(selectedColor);
        return;
    }
    auto checkable = AceType::DynamicCast<CheckableComponent>(mainComponent);
    if (checkable) {
        checkable->SetActiveColor(selectedColor);
        return;
    }
}

void JSToggle::SwitchPointColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    auto mainComponent = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto switchComponent = AceType::DynamicCast<SwitchComponent>(mainComponent);
    if (!switchComponent) {
        LOGE("pointstyle only support switch");
        return;
    }
    Color color;
    if (JSContainerBase::ParseJsColor(info[0], color)) {
        switchComponent->SetPointColor(color);
    }
}

void JSToggle::JsPadding(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }
    if (!info[0]->IsString() && !info[0]->IsNumber() && !info[0]->IsObject()) {
        LOGE("arg is not a string, number or object.");
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto toggleComponent = AceType::DynamicCast<ToggleComponent>(stack->GetMainComponent());
    auto box = stack->GetBoxComponent();
    if (toggleComponent) {
        JSViewAbstract::JsPadding(info);
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
            auto checkableComponent = AceType::DynamicCast<CheckableComponent>(stack->GetMainComponent());
            if (checkableComponent) {
                auto width = checkableComponent->GetWidth() - checkableComponent->GetHotZoneHorizontalPadding();
                auto height = checkableComponent->GetHeight() - checkableComponent->GetHotZoneVerticalPadding();
                checkableComponent->SetHeight(height + topDimen * 2);
                checkableComponent->SetWidth(width + leftDimen * 2);
                box->SetHeight(height + topDimen * 2);
                box->SetWidth(width + leftDimen * 2);
                checkableComponent->SetHotZoneVerticalPadding(topDimen);
                checkableComponent->SetHorizontalPadding(leftDimen);
            }
            return;
        }
    }
    Dimension length;
    if (!ParseJsDimensionVp(info[0], length)) {
        return;
    }
    auto checkableComponent = AceType::DynamicCast<CheckableComponent>(stack->GetMainComponent());
    if (checkableComponent) {
        auto width = checkableComponent->GetWidth() - checkableComponent->GetHotZoneHorizontalPadding();
        auto height = checkableComponent->GetHeight() - checkableComponent->GetHotZoneVerticalPadding();
        checkableComponent->SetHeight(height + length * 2);
        checkableComponent->SetWidth(width + length * 2);
        box->SetHeight(height + length * 2);
        box->SetWidth(width + length * 2);
        checkableComponent->SetHotZoneVerticalPadding(length);
        checkableComponent->SetHorizontalPadding(length);
    }
}

} // namespace OHOS::Ace::Framework
