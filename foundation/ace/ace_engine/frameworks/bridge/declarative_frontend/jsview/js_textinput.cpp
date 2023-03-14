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

#include "frameworks/bridge/declarative_frontend/jsview/js_textinput.h"

#include <vector>

#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_clipboard_function.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_textarea.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"
#include "frameworks/core/common/ime/text_input_action.h"
#include "frameworks/core/common/ime/text_input_type.h"
#include "frameworks/core/components/text_field/text_field_component.h"
#include "frameworks/core/components/text_field/textfield_theme.h"

namespace OHOS::Ace::Framework {

namespace {

const std::vector<std::string> INPUT_FONT_FAMILY_VALUE = { "sans-serif" };
const std::vector<FontStyle> FONT_STYLES = { FontStyle::NORMAL, FontStyle::ITALIC };
constexpr Dimension INNER_PADDING = 0.0_vp;

} // namespace

void JSTextInput::InitDefaultStyle()
{
    auto boxComponent = ViewStackProcessor::GetInstance()->GetBoxComponent();
    auto stack = ViewStackProcessor::GetInstance();
    auto textInputComponent = AceType::DynamicCast<TextFieldComponent>(stack->GetMainComponent());
    auto theme = GetTheme<TextFieldTheme>();
    if (!boxComponent || !textInputComponent || !theme) {
        return;
    }

    textInputComponent->SetCursorColor(theme->GetCursorColor());
    textInputComponent->SetCursorRadius(theme->GetCursorRadius());
    textInputComponent->SetPlaceholderColor(theme->GetPlaceholderColor());
    textInputComponent->SetFocusBgColor(theme->GetFocusBgColor());
    textInputComponent->SetFocusPlaceholderColor(theme->GetFocusPlaceholderColor());
    textInputComponent->SetFocusTextColor(theme->GetFocusTextColor());
    textInputComponent->SetBgColor(theme->GetBgColor());
    textInputComponent->SetTextColor(theme->GetTextColor());
    textInputComponent->SetSelectedColor(theme->GetSelectedColor());
    textInputComponent->SetHoverColor(theme->GetHoverColor());
    textInputComponent->SetPressColor(theme->GetPressColor());
    textInputComponent->SetNeedFade(theme->NeedFade());
    textInputComponent->SetShowEllipsis(theme->ShowEllipsis());

    TextStyle textStyle = textInputComponent->GetTextStyle();
    textStyle.SetTextColor(theme->GetTextColor());
    textStyle.SetFontSize(theme->GetFontSize());
    textStyle.SetFontWeight(theme->GetFontWeight());
    textStyle.SetFontFamilies(INPUT_FONT_FAMILY_VALUE);
    textInputComponent->SetTextStyle(textStyle);
    textInputComponent->SetEditingStyle(textStyle);
    textInputComponent->SetPlaceHoldStyle(textStyle);

    textInputComponent->SetCountTextStyle(theme->GetCountTextStyle());
    textInputComponent->SetOverCountStyle(theme->GetOverCountStyle());
    textInputComponent->SetCountTextStyleOuter(theme->GetCountTextStyleOuter());
    textInputComponent->SetOverCountStyleOuter(theme->GetOverCountStyleOuter());

    textInputComponent->SetErrorTextStyle(theme->GetErrorTextStyle());
    textInputComponent->SetErrorSpacing(theme->GetErrorSpacing());
    textInputComponent->SetErrorIsInner(theme->GetErrorIsInner());
    textInputComponent->SetErrorBorderWidth(theme->GetErrorBorderWidth());
    textInputComponent->SetErrorBorderColor(theme->GetErrorBorderColor());

    RefPtr<Decoration> decoration = AceType::MakeRefPtr<Decoration>();
    // Need to update when UX of PC supported.
    auto edge = theme->GetPadding();
    edge.SetTop(INNER_PADDING);
    edge.SetBottom(INNER_PADDING);
    decoration->SetPadding(edge);
    decoration->SetBackgroundColor(theme->GetBgColor());
    decoration->SetBorderRadius(theme->GetBorderRadius());
    const auto& boxDecoration = boxComponent->GetBackDecoration();
    if (boxDecoration) {
        decoration->SetImage(boxDecoration->GetImage());
        decoration->SetGradient(boxDecoration->GetGradient());
    }
    textInputComponent->SetOriginBorder(decoration->GetBorder());
    textInputComponent->SetDecoration(decoration);
    textInputComponent->SetIconSize(theme->GetIconSize());
    textInputComponent->SetIconHotZoneSize(theme->GetIconHotZoneSize());
    textInputComponent->SetHeight(theme->GetHeight());
}

void JSTextInput::JSBind(BindingTarget globalObj)
{
    JSClass<JSTextInput>::Declare("TextInput");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSTextInput>::StaticMethod("create", &JSTextInput::Create, opt);
    JSClass<JSTextInput>::StaticMethod("type", &JSTextInput::SetType);
    JSClass<JSTextInput>::StaticMethod("placeholderColor", &JSTextInput::SetPlaceholderColor);
    JSClass<JSTextInput>::StaticMethod("placeholderFont", &JSTextInput::SetPlaceholderFont);
    JSClass<JSTextInput>::StaticMethod("backgroundColor", &JSTextInput::SetBackgroundColor);
    JSClass<JSTextInput>::StaticMethod("enterKeyType", &JSTextInput::SetEnterKeyType);
    JSClass<JSTextInput>::StaticMethod("caretColor", &JSTextInput::SetCaretColor);
    JSClass<JSTextInput>::StaticMethod("maxLength", &JSTextInput::SetMaxLength);
    JSClass<JSTextInput>::StaticMethod("height", &JSTextInput::JsHeight);
    JSClass<JSTextInput>::StaticMethod("padding", &JSTextInput::JsPadding);
    JSClass<JSTextInput>::StaticMethod("fontSize", &JSTextInput::SetFontSize);
    JSClass<JSTextInput>::StaticMethod("fontColor", &JSTextInput::SetTextColor);
    JSClass<JSTextInput>::StaticMethod("fontWeight", &JSTextInput::SetFontWeight);
    JSClass<JSTextInput>::StaticMethod("fontStyle", &JSTextInput::SetFontStyle);
    JSClass<JSTextInput>::StaticMethod("fontFamily", &JSTextInput::SetFontFamily);
    JSClass<JSTextInput>::StaticMethod("inputFilter", &JSTextInput::SetInputFilter);
    // API7 onEditChanged deprecated
    JSClass<JSTextInput>::StaticMethod("onEditChanged", &JSTextInput::SetOnEditChanged);
    JSClass<JSTextInput>::StaticMethod("onEditChange", &JSTextInput::SetOnEditChanged);
    JSClass<JSTextInput>::StaticMethod("onSubmit", &JSTextInput::SetOnSubmit);
    JSClass<JSTextInput>::StaticMethod("onChange", &JSTextInput::SetOnChange);
    JSClass<JSTextInput>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSTextInput>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSTextInput>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSTextInput>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSTextInput>::StaticMethod("onClick", &JSTextInput::SetOnClick);
    JSClass<JSTextInput>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSTextInput>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSTextInput>::StaticMethod("onCopy", &JSTextInput::SetOnCopy);
    JSClass<JSTextInput>::StaticMethod("onCut", &JSTextInput::SetOnCut);
    JSClass<JSTextInput>::StaticMethod("onPaste", &JSTextInput::SetOnPaste);
    JSClass<JSTextInput>::Inherit<JSViewAbstract>();
    JSClass<JSTextInput>::Bind(globalObj);
}

void JSTextInput::Create(const JSCallbackInfo& info)
{
    RefPtr<TextFieldComponent> textInputComponent = AceType::MakeRefPtr<TextFieldComponent>();
    textInputComponent->SetTextFieldController(AceType::MakeRefPtr<TextFieldController>());
    // default type is text, default action is done.
    textInputComponent->SetTextInputType(TextInputType::TEXT);
    textInputComponent->SetAction(TextInputAction::DONE);
    textInputComponent->SetInspectorTag("TextInput");
    ViewStackProcessor::GetInstance()->Push(textInputComponent);
    InitDefaultStyle();
    Border boxBorder;
    auto boxComponent = ViewStackProcessor::GetInstance()->GetBoxComponent();
    auto theme = GetTheme<TextFieldTheme>();
    if (boxComponent->GetBackDecoration()) {
        boxBorder = boxComponent->GetBackDecoration()->GetBorder();
    }
    JSTextArea::UpdateDecoration(boxComponent, textInputComponent, boxBorder, theme);

    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGW("text-input create error, info is non-valid");
        return;
    }
    auto paramObject = JSRef<JSObject>::Cast(info[0]);

    std::string placeholder;
    if (ParseJsString(paramObject->GetProperty("placeholder"), placeholder)) {
        textInputComponent->SetPlaceholder(placeholder);
    }

    std::string text;
    if (ParseJsString(paramObject->GetProperty("text"), text)) {
        textInputComponent->SetValue(text);
    }

    auto controllerObj = paramObject->GetProperty("controller");
    if (!controllerObj->IsUndefined() && !controllerObj->IsNull()) {
        JSTextInputController* jsController = JSRef<JSObject>::Cast(controllerObj)->Unwrap<JSTextInputController>();
        if (jsController) {
            jsController->SetController(textInputComponent->GetTextFieldController());
        }
    } else {
        LOGI("controller is nullptr");
    }
}

void JSTextInput::SetBackgroundColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg(SetBackgroundColor) is wrong, it is supposed to have atleast 1 argument");
        return;
    }

    Color backgroundColor;
    if (!ParseJsColor(info[0], backgroundColor)) {
        LOGE("the info[0] is null");
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::TextFieldComponent>(stack->GetMainComponent());
    if (component) {
        component->SetBgColor(backgroundColor);
    } else {
        LOGE("The component(SetPlaceholderColor) is null");
    }
}

void JSTextInput::SetType(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("SetType create error, info is non-valid");
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<TextFieldComponent>(stack->GetMainComponent());
    if (!info[0]->IsNumber()) {
        LOGE("The inputType is not number");
        return;
    }
    TextInputType textInputType = static_cast<TextInputType>(info[0]->ToNumber<int32_t>());
    component->SetTextInputType(textInputType);
    component->SetObscure(textInputType == TextInputType::VISIBLE_PASSWORD);
}

void JSTextInput::JsPadding(const JSCallbackInfo& info)
{
    if (!info[0]->IsString() && !info[0]->IsNumber() && !info[0]->IsObject()) {
        LOGE("arg is not a string, number or object.");
        return;
    }
    Edge padding;
    if (info[0]->IsNumber()) {
        Dimension edgeValue;
        if (ParseJsDimensionVp(info[0], edgeValue)) {
            padding = Edge(edgeValue);
        }
    }
    if (info[0]->IsObject()) {
        auto object = JsonUtil::ParseJsonString(info[0]->ToString());
        if (!object) {
            LOGE("Js Parse object failed. argsPtr is null.");
            return;
        }
        if (object->Contains("top") || object->Contains("bottom") || object->Contains("left") ||
            object->Contains("right")) {
            Dimension left = Dimension(0.0, DimensionUnit::VP);
            Dimension top = Dimension(0.0, DimensionUnit::VP);
            Dimension right = Dimension(0.0, DimensionUnit::VP);
            Dimension bottom = Dimension(0.0, DimensionUnit::VP);
            ParseJsonDimensionVp(object->GetValue("left"), left);
            ParseJsonDimensionVp(object->GetValue("top"), top);
            ParseJsonDimensionVp(object->GetValue("right"), right);
            ParseJsonDimensionVp(object->GetValue("bottom"), bottom);
            padding = Edge(left, top, right, bottom);
        }
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<TextFieldComponent>(stack->GetMainComponent());
    if (component) {
        auto decoration = component->GetDecoration();
        decoration->SetPadding(padding);
    }
}

void JSTextInput::SetPlaceholderColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg(SetPlaceholderColor) is wrong, it is supposed to have atleast 1 argument");
        return;
    }

    Color color;
    if (!ParseJsColor(info[0], color)) {
        LOGE("the info[0] is null");
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<TextFieldComponent>(stack->GetMainComponent());
    if (component) {
        component->SetPlaceholderColor(color);
        component->SetFocusPlaceholderColor(color);
    } else {
        LOGE("The component(SetPlaceholderColor) is null");
        return;
    }
}

void JSTextInput::SetPlaceholderFont(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("PlaceholderFont create error, info is non-valid");
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<TextFieldComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("The component(SetPlaceholderFont) is null");
        return;
    }
    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto textStyle = component->GetPlaceHoldStyle();

    auto fontSize = paramObject->GetProperty("size");
    if (!fontSize->IsNull()) {
        Dimension size;
        ParseJsDimensionFp(fontSize, size);
        textStyle.SetFontSize(size);
    }

    std::string weight;
    auto fontWeight = paramObject->GetProperty("weight");
    if (!fontWeight->IsNull()) {
        if (fontWeight->IsNumber()) {
            weight = std::to_string(fontWeight->ToNumber<int32_t>());
        } else {
            ParseJsString(fontWeight, weight);
        }
        textStyle.SetFontWeight(ConvertStrToFontWeight(weight));
    }

    auto fontFamily = paramObject->GetProperty("family");
    if (!fontFamily->IsNull()) {
        std::vector<std::string> fontFamilies;
        if (ParseJsFontFamilies(fontFamily, fontFamilies)) {
            textStyle.SetFontFamilies(fontFamilies);
        }
    }

    auto style = paramObject->GetProperty("style");
    if (!style->IsNull()) {
        FontStyle fontStyle = static_cast<FontStyle>(style->ToNumber<int32_t>());
        textStyle.SetFontStyle(fontStyle);
    }
    component->SetPlaceHoldStyle(textStyle);
}

void JSTextInput::SetEnterKeyType(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<TextFieldComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("The component(SetEnterKeyType) is null");
        return;
    }
    if (!info[0]->IsNumber()) {
        LOGE("Info(SetEnterKeyType) is not number");
        return;
    }
    TextInputAction textInputAction = static_cast<TextInputAction>(info[0]->ToNumber<int32_t>());
    component->SetAction(textInputAction);
}

void JSTextInput::SetCaretColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg(SetCareColor) is wrong, it is supposed to have atleast 1 argument");
        return;
    }

    Color color;
    if (!ParseJsColor(info[0], color)) {
        LOGE("info[0] is null");
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<TextFieldComponent>(stack->GetMainComponent());
    if (component) {
        component->SetCursorColor(color);
    } else {
        LOGE("The component(SetCaretColor) is null");
        return;
    }
}

void JSTextInput::SetMaxLength(uint32_t value)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<TextFieldComponent>(stack->GetMainComponent());
    if (component) {
        component->SetMaxLength(value);
    }
}

void JSTextInput::JsHeight(const JSCallbackInfo& info)
{
    JSViewAbstract::JsHeight(info);
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }
    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }
    if (LessNotEqual(value.Value(), 0.0)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto textInputComponent = AceType::DynamicCast<TextFieldComponent>(stack->GetMainComponent());
    if (!textInputComponent) {
        LOGE("JSTextInput set height failed, textInputComponent is null.");
        return;
    }
    textInputComponent->SetHeight(value);
}

void JSTextInput::SetFontSize(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("JSTextInput::SetFontSize The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension fontSize;
    if (!ParseJsDimensionFp(info[0], fontSize)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::TextFieldComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("JSTextInput::SetFontSize component is not valid");
        return;
    }

    auto textStyle = component->GetEditingStyle();
    textStyle.SetFontSize(fontSize);
    component->SetEditingStyle(textStyle);
}

void JSTextInput::SetTextColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color textColor;
    if (!ParseJsColor(info[0], textColor)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::TextFieldComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetEditingStyle();
    textStyle.SetTextColor(textColor);
    component->SetEditingStyle(textStyle);
}

void JSTextInput::SetFontWeight(const std::string& value)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::TextFieldComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetEditingStyle();
    textStyle.SetFontWeight(ConvertStrToFontWeight(value));
    component->SetEditingStyle(textStyle);
}

void JSTextInput::SetFontStyle(int32_t value)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::TextFieldComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    if (value >= 0 && value < static_cast<int32_t>(FONT_STYLES.size())) {
        auto textStyle = component->GetEditingStyle();
        textStyle.SetFontStyle(FONT_STYLES[value]);
        component->SetEditingStyle(textStyle);
    } else {
        LOGE("TextInput fontStyle(%d) illegal value", value);
    }
}

void JSTextInput::SetFontFamily(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    std::vector<std::string> fontFamilies;
    if (!ParseJsFontFamilies(info[0], fontFamilies)) {
        LOGE("Parse FontFamilies failed");
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::TextFieldComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetEditingStyle();
    textStyle.SetFontFamilies(fontFamilies);
    component->SetEditingStyle(textStyle);
}

void JSTextInput::SetInputFilter(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    std::string inputFilter;
    if (!ParseJsString(info[0], inputFilter)) {
        LOGE("Parse inputFilter failed");
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::TextFieldComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    component->SetInputFilter(inputFilter);

    if (info[1]->IsFunction()) {
        auto jsFunc = AceType::MakeRefPtr<JsClipboardFunction>(JSRef<JSFunc>::Cast(info[1]));
        auto resultId =
            [execCtx = info.GetExecutionContext(), func = std::move(jsFunc)](const std::string& info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                func->Execute(info);
            };
        component->SetOnError(resultId);
    }
}

void JSTextInput::SetOnEditChanged(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&TextFieldComponent::SetOnEditChanged, info)) {
        LOGW("Failed(OnEditChanged) to bind event");
    }
    info.ReturnSelf();
}

void JSTextInput::SetOnSubmit(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&TextFieldComponent::SetOnSubmit, info)) {
        LOGW("Failed(OnSubmit) to bind event");
    }
    info.ReturnSelf();
}

void JSTextInput::SetOnChange(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&TextFieldComponent::SetOnChange, info)) {
        LOGW("Failed(OnChange) to bind event");
    }
    info.ReturnSelf();
}

void JSTextInput::SetOnCopy(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&TextFieldComponent::SetOnCopy, info)) {
        LOGW("Failed(OnCopy) to bind event");
    }
    info.ReturnSelf();
}

void JSTextInput::SetOnCut(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&TextFieldComponent::SetOnCut, info)) {
        LOGW("Failed(OnCut) to bind event");
    }
    info.ReturnSelf();
}

void JSTextInput::SetOnPaste(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&TextFieldComponent::SetOnPaste, info)) {
        LOGW("Failed(OnPaste) to bind event");
    }
    info.ReturnSelf();
}

void JSTextInput::SetOnClick(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&TextFieldComponent::SetOnClick, info)) {
        LOGW("Failed(OnPaste) to bind event");
    }
    info.ReturnSelf();
}

void JSTextInputController::JSBind(BindingTarget globalObj)
{
    JSClass<JSTextInputController>::Declare("TextInputController");
    JSClass<JSTextInputController>::Method("caretPosition", &JSTextInputController::CaretPosition);
    JSClass<JSTextInputController>::Bind(globalObj, JSTextInputController::Constructor,
                                         JSTextInputController::Destructor);
}

void JSTextInputController::Constructor(const JSCallbackInfo& args)
{
    auto scroller = Referenced::MakeRefPtr<JSTextInputController>();
    scroller->IncRefCount();
    args.SetReturnValue(Referenced::RawPtr(scroller));
}

void JSTextInputController::Destructor(JSTextInputController* scroller)
{
    if (scroller != nullptr) {
        scroller->DecRefCount();
    }
}

void JSTextInputController::CaretPosition(int32_t caretPosition)
{
    if (controller_) {
        controller_->CaretPosition(caretPosition);
    }
}

} // namespace OHOS::Ace::Framework