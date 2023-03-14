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

#include "bridge/declarative_frontend/jsview/js_textpicker.h"

#include "bridge/declarative_frontend/engine/functions/js_function.h"
#include "bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components/picker/picker_base_component.h"
#include "core/components/picker/picker_theme.h"

namespace OHOS::Ace::Framework {

void JSTextPicker::JSBind(BindingTarget globalObj)
{
    JSClass<JSTextPicker>::Declare("TextPicker");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSTextPicker>::StaticMethod("create", &JSTextPicker::Create, opt);
    JSClass<JSTextPicker>::StaticMethod("defaultPickerItemHeight", &JSTextPicker::SetDefaultPickerItemHeight);
    JSClass<JSTextPicker>::StaticMethod("onAccept", &JSTextPicker::OnAccept);
    JSClass<JSTextPicker>::StaticMethod("onCancel", &JSTextPicker::OnCancel);
    JSClass<JSTextPicker>::StaticMethod("onChange", &JSTextPicker::OnChange);
    JSClass<JSTextPicker>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSTextPicker>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSTextPicker>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSTextPicker>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSTextPicker>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSTextPicker>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSTextPicker>::Inherit<JSViewAbstract>();
    JSClass<JSTextPicker>::Bind(globalObj);
}

void JSTextPicker::Create(const JSCallbackInfo& info)
{
    std::string value = "0";
    uint32_t selected = 0;

    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("TextPicker create error, info is non-valid");
        return;
    }

    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto getSelected = paramObject->GetProperty("selected");
    JSRef<JSArray> getRange = paramObject->GetProperty("range");
    std::vector<std::string> getRangeVector;
    if (!ParseJsStrArray(getRange, getRangeVector)) {
        LOGE("parse range failed");
        return;
    }

    if (!ParseJsInteger(getSelected, selected)) {
        LOGE("parse selected value failed");
    }

    if (selected < 0 || selected >= getRangeVector.size()) {
        LOGE("selected is out of range");
    }

    RefPtr<Component> pickerTextComponent = AceType::MakeRefPtr<OHOS::Ace::PickerTextComponent>();
    ViewStackProcessor::GetInstance()->Push(pickerTextComponent);
    JSInteractableView::SetFocusNode(true);

    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto PickerText = AceType::DynamicCast<PickerTextComponent>(component);
    if (!PickerText) {
        LOGE("PickerText Component is null");
        return;
    }

    PickerText->SetSelected(selected);
    PickerText->SetRange(getRangeVector);

    PickerText->SetIsDialog(false);
    PickerText->SetHasButtons(false);

    auto theme = GetTheme<PickerTheme>();
    if (!theme) {
        LOGE("PickerText Theme is null");
        return;
    }
    PickerText->SetTheme(theme);
}

void JSTextPicker::SetDefaultPickerItemHeight(const JSCallbackInfo& info)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto TextPicker = AceType::DynamicCast<PickerTextComponent>(component);
    if (!TextPicker) {
        LOGE("PickerTextComponent is null");
        return;
    }

    Dimension height;
    if (!ParseJsDimensionFp(info[0], height)) {
        return;
    }

    TextPicker->SetColumnHeight(height);
}

void JSTextPicker::OnAccept(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&PickerBaseComponent::SetOnTextAccept, info)) {
        LOGW("Failed to bind event");
    }
    info.ReturnSelf();
}

void JSTextPicker::OnCancel(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&PickerBaseComponent::SetOnTextCancel, info)) {
        LOGW("Failed to bind event");
    }
    info.ReturnSelf();
}

void JSTextPicker::OnChange(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&PickerBaseComponent::SetOnTextChange, info)) {
        LOGW("Failed to bind event");
    }
    info.ReturnSelf();
}

void JSTextPickerDialog::JSBind(BindingTarget globalObj)
{
    JSClass<JSTextPickerDialog>::Declare("TextPickerDialog");
    JSClass<JSTextPickerDialog>::StaticMethod("show", &JSTextPickerDialog::Show);

    JSClass<JSTextPickerDialog>::Bind<>(globalObj);
}

void JSTextPickerDialog::Show(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("TextPicker create error, info is non-valid");
        return;
    }

    auto PickerText = AceType::MakeRefPtr<PickerTextComponent>();
    if (!PickerText) {
        LOGE("PickerText Component is null");
        return;
    }
    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    ParseText(PickerText, paramObject);
    DialogProperties properties {};
    properties.alignment = DialogAlignment::CENTER;
    properties.customComponent = PickerText;

    AddEvent(PickerText, info);
    PickerText->SetDialogName("pickerTextDialog");
    PickerText->OpenDialog(properties);
}

void JSTextPickerDialog::AddEvent(RefPtr<PickerTextComponent>& picker, const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("TextPicker AddEvent error, info is non-valid");
        return;
    }
    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto onAccept = paramObject->GetProperty("onAccept");
    if (!onAccept->IsUndefined() && onAccept->IsFunction()) {
        auto jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(onAccept));
        auto acceptId =
            EventMarker([execCtx = info.GetExecutionContext(), func = std::move(jsFunc)](const std::string& info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                std::vector<std::string> keys = { "value", "index"};
                ACE_SCORING_EVENT("TextPickerDialog.onAccept");
                func->Execute(keys, info);
            });
        picker->SetDialogAcceptEvent(acceptId);
    }
    auto onCancel = paramObject->GetProperty("onCancel");
    if (!onCancel->IsUndefined() && onCancel->IsFunction()) {
        auto jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(onCancel));
        auto cancelId =
            EventMarker([execCtx = info.GetExecutionContext(), func = std::move(jsFunc)]() {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                ACE_SCORING_EVENT("TextPickerDialog.onCancel");
                func->Execute();
            });
        picker->SetDialogCancelEvent(cancelId);
    }
    auto onChange = paramObject->GetProperty("onChange");
    if (!onChange->IsUndefined() && onChange->IsFunction()) {
        auto jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(onChange));
        auto changeId =
            EventMarker([execCtx = info.GetExecutionContext(), func = std::move(jsFunc)](const std::string& info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                std::vector<std::string> keys = { "value", "index"};
                ACE_SCORING_EVENT("TextPickerDialog.onChange");
                func->Execute(keys, info);
            });
        picker->SetDialogChangeEvent(changeId);
    }
}

void JSTextPickerDialog::ParseText(RefPtr<PickerTextComponent>& component, const JSRef<JSObject>& paramObj)
{
    auto getSelected = paramObj->GetProperty("selected");
    auto defaultHeight = paramObj->GetProperty("defaultPickerItemHeight");
    JSRef<JSArray> getRange = paramObj->GetProperty("range");
    std::vector<std::string> getRangeVector;
    if (!JSViewAbstract::ParseJsStrArray(getRange, getRangeVector)) {
        LOGE("parse range failed");
        return;
    }

    uint32_t selected = 0;
    if (!JSViewAbstract::ParseJsInteger(getSelected, selected)) {
        LOGE("parse selected value failed");
    }

    if (selected < 0 || selected >= getRangeVector.size()) {
        LOGE("selected is out of range");
    }

    Dimension height;
    if (defaultHeight->IsNumber() && !JSViewAbstract::ParseJsDimensionFp(defaultHeight, height)) {
        return;
    }

    component->SetIsDialog(false);
    component->SetIsCreateDialogComponent(true);
    component->SetColumnHeight(height);
    component->SetSelected(selected);
    component->SetRange(getRangeVector);
}
} // namespace OHOS::Ace::Framework
