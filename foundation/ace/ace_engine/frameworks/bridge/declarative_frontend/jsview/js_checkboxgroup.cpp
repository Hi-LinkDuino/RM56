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

#include "bridge/declarative_frontend/jsview/js_checkboxgroup.h"
#include "bridge/declarative_frontend/jsview/js_interactable_view.h"

#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components/checkable/checkable_component.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"

namespace OHOS::Ace::Framework {

constexpr int32_t PADDING_MULTIPLE = 2;

JSRef<JSVal> CheckboxGroupResultEventToJSValue(const CheckboxGroupResult& eventInfo)
{
    JSRef<JSObject> obj = JSRef<JSObject>::New();
    JSRef<JSArray> nameArr = JSRef<JSArray>::New();
    std::vector<std::string> nameList = eventInfo.GetNameList();
    for (int idx = 0; idx < static_cast<int32_t>(nameList.size()); ++idx) {
        JSRef<JSVal> name = JSRef<JSVal>::Make(ToJSValue(nameList[idx]));
        nameArr->SetValueAt(idx, name);
    }
    obj->SetPropertyObject("name", nameArr);
    obj->SetProperty("status", eventInfo.GetStatus());
    return JSRef<JSVal>::Cast(obj);
}

void JSCheckboxGroup::JSBind(BindingTarget globalObj)
{
    JSClass<JSCheckboxGroup>::Declare("CheckboxGroup");

    JSClass<JSCheckboxGroup>::StaticMethod("create", &JSCheckboxGroup::Create);
    JSClass<JSCheckboxGroup>::StaticMethod("selectAll", &JSCheckboxGroup::SetSelectAll);
    JSClass<JSCheckboxGroup>::StaticMethod("onChange", &JSCheckboxGroup::SetOnChange);
    JSClass<JSCheckboxGroup>::StaticMethod("selectedColor", &JSCheckboxGroup::SelectedColor);
    JSClass<JSCheckboxGroup>::StaticMethod("width", &JSCheckboxGroup::JsWidth);
    JSClass<JSCheckboxGroup>::StaticMethod("height", &JSCheckboxGroup::JsHeight);
    JSClass<JSCheckboxGroup>::StaticMethod("size", &JSCheckboxGroup::JsSize);
    JSClass<JSCheckboxGroup>::StaticMethod("padding", &JSCheckboxGroup::JsPadding);
    JSClass<JSCheckboxGroup>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSCheckboxGroup>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSCheckboxGroup>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSCheckboxGroup>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSCheckboxGroup>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSCheckboxGroup>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSCheckboxGroup>::Inherit<JSViewAbstract>();
    JSClass<JSCheckboxGroup>::Bind<>(globalObj);
}

void JSCheckboxGroup::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("checkboxgroup create error, info is not-valid");
        return;
    }
    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto groupName = paramObject->GetProperty("group");
    RefPtr<CheckboxTheme> checkBoxTheme = GetTheme<CheckboxTheme>();
    auto checkboxComponent = AceType::MakeRefPtr<OHOS::Ace::CheckboxComponent>(checkBoxTheme);
    auto checkboxGroupName = groupName->ToString();
    checkboxComponent->SetGroupName(checkboxGroupName);
    auto checkboxGroupmap = ViewStackProcessor::GetInstance()->GetCheckboxGroupCompnent();
    checkboxGroupmap->emplace(checkboxGroupName, checkboxComponent);
    checkboxComponent->SetInspectorTag("CheckboxGroupComponent");
    ViewStackProcessor::GetInstance()->Push(checkboxComponent);
    
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    auto horizontalPadding = checkBoxTheme->GetHotZoneHorizontalPadding();
    auto verticalPadding = checkBoxTheme->GetHotZoneVerticalPadding();
    box->SetWidth(checkBoxTheme->GetWidth() + horizontalPadding * PADDING_MULTIPLE);
    box->SetHeight(checkBoxTheme->GetHeight() + verticalPadding * PADDING_MULTIPLE);
}

void JSCheckboxGroup::SetSelectAll(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsBoolean()) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments, arg is not a bool");
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto checkboxComponent = AceType::DynamicCast<CheckboxComponent>(stack->GetMainComponent());
    checkboxComponent->SetValue(info[0]->ToBoolean());
}

void JSCheckboxGroup::SetOnChange(const JSCallbackInfo& args)
{
    if (args.Length() < 1 || !args[0]->IsFunction()) {
        LOGI("args not function");
        return;
    }

    auto jsFunc = AceType::MakeRefPtr<JsEventFunction<CheckboxGroupResult, 1>>(
        JSRef<JSFunc>::Cast(args[0]), CheckboxGroupResultEventToJSValue);
    auto checkbox = AceType::DynamicCast<CheckboxComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    checkbox->SetOnGroupChange(EventMarker(
        [execCtx = args.GetExecutionContext(), func = std::move(jsFunc)](const BaseEventInfo* info) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            auto eventInfo = TypeInfoHelper::DynamicCast<CheckboxGroupResult>(info);
            func->Execute(*eventInfo);
        }));
}

void JSCheckboxGroup::JsWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsWidth(info[0]);
}

void JSCheckboxGroup::JsWidth(const JSRef<JSVal>& jsValue)
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

void JSCheckboxGroup::JsHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsHeight(info[0]);
}

void JSCheckboxGroup::JsHeight(const JSRef<JSVal>& jsValue)
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

void JSCheckboxGroup::JsSize(const JSCallbackInfo& info)
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

void JSCheckboxGroup::SelectedColor(const JSCallbackInfo& info)
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

void JSCheckboxGroup::JsPadding(const JSCallbackInfo& info)
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
