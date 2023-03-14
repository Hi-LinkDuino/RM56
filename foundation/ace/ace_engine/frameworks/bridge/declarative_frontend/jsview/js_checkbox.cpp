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

#include "bridge/declarative_frontend/jsview/js_checkbox.h"

#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components/checkable/checkable_component.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"

namespace OHOS::Ace::Framework {

void JSCheckbox::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("checkboxgroup create error, info is not-valid");
        return;
    }
    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto name = paramObject->GetProperty("name");
    auto group = paramObject->GetProperty("group");
    RefPtr<CheckboxTheme> checkBoxTheme = GetTheme<CheckboxTheme>();
    auto checkboxComponent = AceType::MakeRefPtr<OHOS::Ace::CheckboxComponent>(checkBoxTheme);
    auto checkboxName = name->ToString();
    checkboxComponent->SetCheckboxName(checkboxName);
    if (!group->IsNull()) {
        auto checkboxGroup = group->ToString();
        checkboxComponent->SetBelongGroup(checkboxGroup);
        auto checkboxGroupmap = ViewStackProcessor::GetInstance()->GetCheckboxGroupCompnent();
        auto item = checkboxGroupmap->find(checkboxGroup);
        if (item != checkboxGroupmap->end()) {
            item->second->AddCheckbox(checkboxComponent);
            checkboxComponent->SetGroup(item->second);
        }
    }
    checkboxComponent->SetInspectorTag("Checkbox");
    ViewStackProcessor::GetInstance()->Push(checkboxComponent);

    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    auto horizontalPadding = checkBoxTheme->GetHotZoneHorizontalPadding();
    auto verticalPadding = checkBoxTheme->GetHotZoneVerticalPadding();
    box->SetWidth(checkBoxTheme->GetWidth() + horizontalPadding * 2);
    box->SetHeight(checkBoxTheme->GetHeight() + verticalPadding * 2);
}

void JSCheckbox::JSBind(BindingTarget globalObj)
{
    JSClass<JSCheckbox>::Declare("Checkbox");

    JSClass<JSCheckbox>::StaticMethod("create", &JSCheckbox::Create);
    JSClass<JSCheckbox>::StaticMethod("select", &JSCheckbox::SetSelect);
    JSClass<JSCheckbox>::StaticMethod("onChange", &JSCheckbox::SetOnChange);
    JSClass<JSCheckbox>::StaticMethod("selectedColor", &JSCheckbox::SelectedColor);
    JSClass<JSCheckbox>::StaticMethod("width", &JSCheckbox::JsWidth);
    JSClass<JSCheckbox>::StaticMethod("height", &JSCheckbox::JsHeight);
    JSClass<JSCheckbox>::StaticMethod("size", &JSCheckbox::JsSize);
    JSClass<JSCheckbox>::StaticMethod("padding", &JSCheckbox::JsPadding);
    JSClass<JSCheckbox>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSCheckbox>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSCheckbox>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSCheckbox>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSCheckbox>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSCheckbox>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSCheckbox>::Inherit<JSViewAbstract>();
    JSClass<JSCheckbox>::Bind<>(globalObj);
}

void JSCheckbox::SetSelect(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsBoolean()) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments, arg is not a bool");
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto checkboxComponent = AceType::DynamicCast<CheckboxComponent>(stack->GetMainComponent());
    checkboxComponent->SetValue(info[0]->ToBoolean());
}

void JSCheckbox::SetOnChange(const JSCallbackInfo& args)
{
    if (!JSViewBindEvent(&CheckboxComponent::SetOnChange, args)) {
        LOGW("Failed to bind event");
    }
    args.ReturnSelf();
}

void JSCheckbox::JsWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsWidth(info[0]);
}

void JSCheckbox::JsWidth(const JSRef<JSVal>& jsValue)
{
    Dimension value;
    if (!ParseJsDimensionVp(jsValue, value)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    Dimension padding;
    auto box = stack->GetBoxComponent();
    auto checkboxComponent = AceType::DynamicCast<CheckboxComponent>(stack->GetMainComponent());
    if (checkboxComponent) {
        padding = checkboxComponent->GetHotZoneHorizontalPadding();
        checkboxComponent->SetWidth(value + padding * 2);
        box->SetWidth(value + padding * 2);
    }
}

void JSCheckbox::JsHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsHeight(info[0]);
}

void JSCheckbox::JsHeight(const JSRef<JSVal>& jsValue)
{
    Dimension value;
    if (!ParseJsDimensionVp(jsValue, value)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto box = stack->GetBoxComponent();
    Dimension padding;
    auto checkboxComponent = AceType::DynamicCast<CheckboxComponent>(stack->GetMainComponent());
    if (checkboxComponent) {
        padding = checkboxComponent->GetHotZoneVerticalPadding();
        checkboxComponent->SetHeight(value + padding * 2);
        box->SetHeight(value + padding * 2);
    }
}

void JSCheckbox::JsSize(const JSCallbackInfo& info)
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

void JSCheckbox::SelectedColor(const JSCallbackInfo& info)
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
    auto checkable = AceType::DynamicCast<CheckboxComponent>(mainComponent);
    if (checkable) {
        checkable->SetActiveColor(selectedColor);
        return;
    }
}

void JSCheckbox::JsPadding(const JSCallbackInfo& info)
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
    auto box = stack->GetBoxComponent();
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
            auto checkboxComponent = AceType::DynamicCast<CheckboxComponent>(stack->GetMainComponent());
            if (checkboxComponent) {
                auto width = checkboxComponent->GetWidth() - checkboxComponent->GetHotZoneHorizontalPadding();
                auto height = checkboxComponent->GetHeight() - checkboxComponent->GetHotZoneVerticalPadding();
                checkboxComponent->SetHeight(height + topDimen * 2);
                checkboxComponent->SetWidth(width + leftDimen * 2);
                box->SetHeight(height + topDimen * 2);
                box->SetWidth(width + leftDimen * 2);
                checkboxComponent->SetHotZoneVerticalPadding(topDimen);
                checkboxComponent->SetHorizontalPadding(leftDimen);
            }
            return;
        }
    }
    Dimension length;
    if (!ParseJsDimensionVp(info[0], length)) {
        return;
    }
    auto checkboxComponent = AceType::DynamicCast<CheckboxComponent>(stack->GetMainComponent());
    if (checkboxComponent) {
        auto width = checkboxComponent->GetWidth() - checkboxComponent->GetHotZoneHorizontalPadding();
        auto height = checkboxComponent->GetHeight() - checkboxComponent->GetHotZoneVerticalPadding();
        checkboxComponent->SetHeight(height + length * 2);
        checkboxComponent->SetWidth(width + length * 2);
        box->SetHeight(height + length * 2);
        box->SetWidth(width + length * 2);
        checkboxComponent->SetHotZoneVerticalPadding(length);
        checkboxComponent->SetHorizontalPadding(length);
    }
}
} // namespace OHOS::Ace::Framework
