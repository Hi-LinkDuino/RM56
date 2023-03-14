/*
 * Copyright (c) 20212022 Huawei Device Co., Ltd.
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

#include "bridge/declarative_frontend/jsview/js_textarea.h"

#include <vector>

#include "bridge/common/utils/utils.h"
#include "bridge/declarative_frontend/engine/functions/js_clipboard_function.h"
#include "bridge/declarative_frontend/engine/functions/js_function.h"
#include "bridge/declarative_frontend/jsview/js_container_base.h"
#include "bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

namespace {

const std::vector<TextAlign> TEXT_ALIGNS = { TextAlign::START, TextAlign::CENTER, TextAlign::END };
const std::vector<FontStyle> FONT_STYLES = { FontStyle::NORMAL, FontStyle::ITALIC };
const std::vector<std::string> INPUT_FONT_FAMILY_VALUE = { "sans-serif" };
constexpr uint32_t TEXTAREA_MAXLENGTH_VALUE_DEFAULT = std::numeric_limits<uint32_t>::max();

}; // namespace

void JSTextArea::InitDefaultStyle()
{
    auto boxComponent = ViewStackProcessor::GetInstance()->GetBoxComponent();
    auto stack = ViewStackProcessor::GetInstance();
    auto textAreaComponent = AceType::DynamicCast<OHOS::Ace::TextFieldComponent>(stack->GetMainComponent());
    auto theme = GetTheme<TextFieldTheme>();
    if (!boxComponent || !textAreaComponent || !theme) {
        return;
    }

    textAreaComponent->SetTextMaxLines(TEXTAREA_MAXLENGTH_VALUE_DEFAULT);
    textAreaComponent->SetCursorColor(theme->GetCursorColor());
    textAreaComponent->SetPlaceholderColor(theme->GetPlaceholderColor());
    textAreaComponent->SetFocusBgColor(theme->GetFocusBgColor());
    textAreaComponent->SetFocusPlaceholderColor(theme->GetFocusPlaceholderColor());
    textAreaComponent->SetFocusTextColor(theme->GetFocusTextColor());
    textAreaComponent->SetBgColor(theme->GetBgColor());
    textAreaComponent->SetTextColor(theme->GetTextColor());
    textAreaComponent->SetSelectedColor(theme->GetSelectedColor());
    textAreaComponent->SetHoverColor(theme->GetHoverColor());
    textAreaComponent->SetPressColor(theme->GetPressColor());

    TextStyle textStyle = textAreaComponent->GetTextStyle();
    textStyle.SetTextColor(theme->GetTextColor());
    textStyle.SetFontSize(theme->GetFontSize());
    textStyle.SetFontWeight(theme->GetFontWeight());
    textStyle.SetFontFamilies(INPUT_FONT_FAMILY_VALUE);
    textAreaComponent->SetTextStyle(textStyle);
    textAreaComponent->SetEditingStyle(textStyle);
    textAreaComponent->SetPlaceHoldStyle(textStyle);

    textAreaComponent->SetCountTextStyle(theme->GetCountTextStyle());
    textAreaComponent->SetOverCountStyle(theme->GetOverCountStyle());
    textAreaComponent->SetCountTextStyleOuter(theme->GetCountTextStyleOuter());
    textAreaComponent->SetOverCountStyleOuter(theme->GetOverCountStyleOuter());
    textAreaComponent->SetErrorBorderWidth(theme->GetErrorBorderWidth());
    textAreaComponent->SetErrorBorderColor(theme->GetErrorBorderColor());

    RefPtr<Decoration> backDecoration = AceType::MakeRefPtr<Decoration>();
    backDecoration->SetPadding(theme->GetPadding());
    backDecoration->SetBackgroundColor(theme->GetBgColor());
    backDecoration->SetBorderRadius(theme->GetBorderRadius());
    const auto& boxDecoration = boxComponent->GetBackDecoration();
    if (boxDecoration) {
        backDecoration->SetImage(boxDecoration->GetImage());
        backDecoration->SetGradient(boxDecoration->GetGradient());
    }
    textAreaComponent->SetOriginBorder(backDecoration->GetBorder());
    textAreaComponent->SetDecoration(backDecoration);
    textAreaComponent->SetIconSize(theme->GetIconSize());
    textAreaComponent->SetIconHotZoneSize(theme->GetIconHotZoneSize());
    textAreaComponent->SetHeight(theme->GetHeight());

    // text area need to extend height.
    textAreaComponent->SetExtend(true);
    boxComponent->SetHeight(-1.0, DimensionUnit::VP);
}

void JSTextArea::JSBind(BindingTarget globalObj)
{
    JSClass<JSTextArea>::Declare("TextArea");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSTextArea>::StaticMethod("create", &JSTextArea::Create, opt);
    JSClass<JSTextArea>::StaticMethod("placeholderColor", &JSTextArea::SetPlaceholderColor);
    JSClass<JSTextArea>::StaticMethod("placeholderFont", &JSTextArea::SetPlaceholderFont);
    JSClass<JSTextArea>::StaticMethod("backgroundColor", &JSTextArea::SetBackgroundColor);
    JSClass<JSTextArea>::StaticMethod("textAlign", &JSTextArea::SetTextAlign);
    JSClass<JSTextArea>::StaticMethod("caretColor", &JSTextArea::SetCaretColor);
    JSClass<JSTextArea>::StaticMethod("height", &JSTextArea::JsHeight);
    JSClass<JSTextArea>::StaticMethod("fontSize", &JSTextArea::SetFontSize);
    JSClass<JSTextArea>::StaticMethod("fontColor", &JSTextArea::SetTextColor);
    JSClass<JSTextArea>::StaticMethod("fontWeight", &JSTextArea::SetFontWeight);
    JSClass<JSTextArea>::StaticMethod("fontStyle", &JSTextArea::SetFontStyle);
    JSClass<JSTextArea>::StaticMethod("fontFamily", &JSTextArea::SetFontFamily);
    JSClass<JSTextArea>::StaticMethod("inputFilter", &JSTextArea::SetInputFilter);
    JSClass<JSTextArea>::StaticMethod("onChange", &JSTextArea::SetOnChange);
    JSClass<JSTextArea>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSTextArea>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSTextArea>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSTextArea>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSTextArea>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSTextArea>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSTextArea>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSTextArea>::StaticMethod("onCopy", &JSTextArea::SetOnCopy);
    JSClass<JSTextArea>::StaticMethod("onCut", &JSTextArea::SetOnCut);
    JSClass<JSTextArea>::StaticMethod("onPaste", &JSTextArea::SetOnPaste);
    JSClass<JSTextArea>::Inherit<JSViewAbstract>();
    JSClass<JSTextArea>::Bind(globalObj);
}

void JSTextArea::UpdateDecoration(const RefPtr<BoxComponent>& boxComponent,
    const RefPtr<TextFieldComponent>& component, const Border& boxBorder,
    const OHOS::Ace::RefPtr<OHOS::Ace::TextFieldTheme>& textFieldTheme)
{
    RefPtr<Decoration> decoration = component->GetDecoration();
    RefPtr<Decoration> boxDecoration = boxComponent->GetBackDecoration();
    if (!decoration) {
        decoration = AceType::MakeRefPtr<Decoration>();
    }
    if (boxDecoration) {
        Border border = decoration->GetBorder();
        border.SetLeftEdge(boxBorder.Left());
        border.SetRightEdge(boxBorder.Right());
        border.SetTopEdge(boxBorder.Top());
        border.SetBottomEdge(boxBorder.Bottom());
        border.SetBorderRadius(textFieldTheme->GetBorderRadius());
        decoration->SetBorder(border);
        component->SetOriginBorder(decoration->GetBorder());

        if (boxDecoration->GetImage() || boxDecoration->GetGradient().IsValid()) {
            // clear box properties except background image and radius.
            boxDecoration->SetBackgroundColor(Color::TRANSPARENT);
            Border border;
            border.SetBorderRadius(textFieldTheme->GetBorderRadius());
            boxDecoration->SetBorder(border);
        } else {
            boxDecoration = AceType::MakeRefPtr<Decoration>();
            boxDecoration->SetBorderRadius(textFieldTheme->GetBorderRadius());
            boxComponent->SetBackDecoration(boxDecoration);
        }
    }
}

void JSTextArea::Create(const JSCallbackInfo& info)
{
    RefPtr<TextFieldComponent> textAreaComponent = AceType::MakeRefPtr<TextFieldComponent>();
    textAreaComponent->SetTextFieldController(AceType::MakeRefPtr<TextFieldController>());
    textAreaComponent->SetTextInputType(TextInputType::MULTILINE);
    auto paramObject = JSRef<JSObject>::Cast(info[0]);

    ViewStackProcessor::GetInstance()->Push(textAreaComponent);
    InitDefaultStyle();
    Border boxBorder;
    auto boxComponent = ViewStackProcessor::GetInstance()->GetBoxComponent();
    auto theme = GetTheme<TextFieldTheme>();
    if (boxComponent->GetBackDecoration()) {
        boxBorder = boxComponent->GetBackDecoration()->GetBorder();
    }
    UpdateDecoration(boxComponent, textAreaComponent, boxBorder, theme);

    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("textarea create error, info is non-valid");
        return;
    }

    std::string placeholder;
    if (ParseJsString(paramObject->GetProperty("placeholder"), placeholder)) {
        textAreaComponent->SetPlaceholder(placeholder);
    }
    std::string text;
    if (ParseJsString(paramObject->GetProperty("text"), text)) {
        textAreaComponent->SetValue(text);
    }

    auto controllerObj = paramObject->GetProperty("controller");
    if (!controllerObj->IsUndefined() && !controllerObj->IsNull()) {
        JSTextAreaController* jsController = JSRef<JSObject>::Cast(controllerObj)->Unwrap<JSTextAreaController>();
        if (jsController) {
            jsController->SetController(textAreaComponent->GetTextFieldController());
        }
    } else {
        LOGI("controller is nullptr");
    }
}

void JSTextArea::SetBackgroundColor(const JSCallbackInfo& info)
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

void JSTextArea::SetPlaceholderColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg(SetPlaceholderColor) is wrong, it is supposed to have atleast 1 argument");
        return;
    }

    Color placeholderColor;
    if (!ParseJsColor(info[0], placeholderColor)) {
        LOGE("the info[0] is null");
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::TextFieldComponent>(stack->GetMainComponent());
    if (component) {
        component->SetPlaceholderColor(placeholderColor);
        component->SetFocusPlaceholderColor(placeholderColor);
    } else {
        LOGE("The component(SetPlaceholderColor) is null");
    }
}

void JSTextArea::SetPlaceholderFont(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("PlaceholderFont create error, info is non-valid");
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::TextFieldComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("The component(SetPlaceholderFont) is null");
        return;
    }
    auto paramObject = JSRef<JSObject>::Cast(info[0]);

    auto fontSize = paramObject->GetProperty("size");
    TextStyle textStyle = component->GetPlaceHoldStyle();

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

void JSTextArea::SetTextAlign(int32_t value)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::TextFieldComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("textAlign component is not valid");
        return;
    }

    if (value >= 0 && value < static_cast<int32_t>(TEXT_ALIGNS.size())) {
        component->SetTextAlign(TEXT_ALIGNS[value]);
    } else {
        LOGE("the value is error");
    }
}

void JSTextArea::SetCaretColor(const JSCallbackInfo& info)
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
    auto component = AceType::DynamicCast<OHOS::Ace::TextFieldComponent>(stack->GetMainComponent());
    if (component) {
        component->SetCursorColor(color);
    } else {
        LOGE("The component(SetCaretColor) is null");
    }
}

void JSTextArea::JsHeight(const JSCallbackInfo& info)
{
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
    auto textAreaComponent = AceType::DynamicCast<TextFieldComponent>(stack->GetMainComponent());
    if (!textAreaComponent) {
        LOGE("JSTextArea set height failed, textAreaComponent is null.");
        return;
    }
    textAreaComponent->SetHeight(value);
}

void JSTextArea::SetFontSize(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension fontSize;
    if (!ParseJsDimensionFp(info[0], fontSize)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::TextFieldComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetEditingStyle();
    textStyle.SetFontSize(fontSize);
    component->SetEditingStyle(textStyle);
}

void JSTextArea::SetTextColor(const JSCallbackInfo& info)
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

void JSTextArea::SetFontWeight(const std::string& value)
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

void JSTextArea::SetFontStyle(int32_t value)
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
        LOGE("TextArea fontStyle(%d) illegal value", value);
    }
}

void JSTextArea::SetFontFamily(const JSCallbackInfo& info)
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

void JSTextArea::SetInputFilter(const JSCallbackInfo& info)
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

void JSTextArea::SetOnChange(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&TextFieldComponent::SetOnChange, info)) {
        LOGW("Failed(OnChange) to bind event");
    }
    info.ReturnSelf();
}

void JSTextArea::SetOnCopy(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&TextFieldComponent::SetOnCopy, info)) {
        LOGW("Failed(OnCopy) to bind event");
    }
    info.ReturnSelf();
}

void JSTextArea::SetOnCut(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&TextFieldComponent::SetOnCut, info)) {
        LOGW("Failed(OnCut) to bind event");
    }
    info.ReturnSelf();
}

void JSTextArea::SetOnPaste(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&TextFieldComponent::SetOnPaste, info)) {
        LOGW("Failed(OnPaste) to bind event");
    }
    info.ReturnSelf();
}

void JSTextAreaController::JSBind(BindingTarget globalObj)
{
    JSClass<JSTextAreaController>::Declare("TextAreaController");
    JSClass<JSTextAreaController>::Method("caretPosition", &JSTextAreaController::CaretPosition);
    JSClass<JSTextAreaController>::Bind(globalObj, JSTextAreaController::Constructor, JSTextAreaController::Destructor);
}

void JSTextAreaController::Constructor(const JSCallbackInfo& args)
{
    auto scroller = Referenced::MakeRefPtr<JSTextAreaController>();
    scroller->IncRefCount();
    args.SetReturnValue(Referenced::RawPtr(scroller));
}

void JSTextAreaController::Destructor(JSTextAreaController* scroller)
{
    if (scroller != nullptr) {
        scroller->DecRefCount();
    }
}

void JSTextAreaController::CaretPosition(int32_t caretPosition)
{
    if (controller_) {
        controller_->CaretPosition(caretPosition);
    }
}

} // namespace OHOS::Ace::Framework