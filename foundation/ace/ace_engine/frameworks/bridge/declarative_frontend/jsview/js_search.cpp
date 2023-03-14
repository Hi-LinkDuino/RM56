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

#include "bridge/declarative_frontend/jsview/js_search.h"

#include "core/components/search/search_component.h"
#include "core/components/search/search_theme.h"
#include "core/components/text_field/text_field_component.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

namespace {

const TextInputAction INPUT_TEXTINPUTACTION_VALUE_DEFAULT = TextInputAction::UNSPECIFIED;
const std::vector<std::string> INPUT_FONT_FAMILY_VALUE = {
    "sans-serif",
};

Radius defaultRadius;
constexpr Dimension BOX_HOVER_RADIUS = 18.0_vp;
bool isPaddingChanged;

void InitializeDefaultValue(const RefPtr<BoxComponent>& boxComponent,
    const RefPtr<TextFieldComponent>& component, const RefPtr<TextFieldTheme>& theme)
{
    component->SetAction(INPUT_TEXTINPUTACTION_VALUE_DEFAULT);
    component->SetCursorColor(theme->GetCursorColor());
    component->SetCursorRadius(theme->GetCursorRadius());
    component->SetPlaceholderColor(theme->GetPlaceholderColor());

    component->SetFocusBgColor(theme->GetFocusBgColor());
    component->SetFocusPlaceholderColor(theme->GetFocusPlaceholderColor());
    component->SetFocusTextColor(theme->GetFocusTextColor());
    component->SetBgColor(theme->GetBgColor());
    component->SetTextColor(theme->GetTextColor());
    component->SetSelectedColor(theme->GetSelectedColor());
    component->SetHoverColor(theme->GetHoverColor());
    component->SetPressColor(theme->GetPressColor());
    component->SetNeedFade(theme->NeedFade());
    component->SetShowEllipsis(theme->ShowEllipsis());

    TextStyle textStyle = component->GetTextStyle();
    textStyle.SetTextColor(theme->GetTextColor());
    textStyle.SetFontSize(theme->GetFontSize());
    textStyle.SetFontWeight(theme->GetFontWeight());
    textStyle.SetFontFamilies(INPUT_FONT_FAMILY_VALUE);
    component->SetTextStyle(textStyle);

    component->SetCountTextStyle(theme->GetCountTextStyle());
    component->SetOverCountStyle(theme->GetOverCountStyle());
    component->SetCountTextStyleOuter(theme->GetCountTextStyleOuter());
    component->SetOverCountStyleOuter(theme->GetOverCountStyleOuter());

    component->SetErrorTextStyle(theme->GetErrorTextStyle());
    component->SetErrorSpacing(theme->GetErrorSpacing());
    component->SetErrorIsInner(theme->GetErrorIsInner());
    component->SetErrorBorderWidth(theme->GetErrorBorderWidth());
    component->SetErrorBorderColor(theme->GetErrorBorderColor());

    RefPtr<Decoration> decoration = AceType::MakeRefPtr<Decoration>();
    decoration->SetPadding(theme->GetPadding());
    decoration->SetBackgroundColor(theme->GetBgColor());
    decoration->SetBorderRadius(theme->GetBorderRadius());
    defaultRadius = theme->GetBorderRadius();
    const auto& boxDecoration = boxComponent->GetBackDecoration();
    if (boxDecoration) {
        decoration->SetImage(boxDecoration->GetImage());
        decoration->SetGradient(boxDecoration->GetGradient());
    }
    component->SetDecoration(decoration);

    component->SetIconSize(theme->GetIconSize());
    component->SetIconHotZoneSize(theme->GetIconHotZoneSize());

    boxComponent->SetPadding(theme->GetPadding());
    component->SetHeight(theme->GetHeight());
}

void UpdateDecorationStyle(const RefPtr<BoxComponent>& boxComponent,
    const RefPtr<TextFieldComponent>& component, const Border& boxBorder, bool hasBoxRadius)
{
    RefPtr<Decoration> decoration = component->GetDecoration();
    if (!decoration) {
        decoration = AceType::MakeRefPtr<Decoration>();
    }
    if (hasBoxRadius) {
        decoration->SetBorder(boxBorder);
    } else {
        Border border = decoration->GetBorder();
        border.SetLeftEdge(boxBorder.Left());
        border.SetRightEdge(boxBorder.Right());
        border.SetTopEdge(boxBorder.Top());
        border.SetBottomEdge(boxBorder.Bottom());
        border.SetBorderRadius(defaultRadius);
        decoration->SetBorder(border);
    }
    component->SetOriginBorder(decoration->GetBorder());

    if (!boxComponent) {
        return;
    }
    RefPtr<Decoration> boxDecoration = boxComponent->GetBackDecoration();
    if (boxDecoration && (boxDecoration->GetImage() || boxDecoration->GetGradient().IsValid())) {
        // clear box properties except background image and radius.
        boxDecoration->SetBackgroundColor(Color::TRANSPARENT);
        Border border;
        if (!hasBoxRadius) {
            border.SetBorderRadius(defaultRadius);
        } else {
            border.SetTopLeftRadius(boxBorder.TopLeftRadius());
            border.SetTopRightRadius(boxBorder.TopRightRadius());
            border.SetBottomLeftRadius(boxBorder.BottomLeftRadius());
            border.SetBottomRightRadius(boxBorder.BottomRightRadius());
        }
        boxDecoration->SetBorder(border);
    } else {
        RefPtr<Decoration> backDecoration = AceType::MakeRefPtr<Decoration>();
        backDecoration->SetBorderRadius(Radius(BOX_HOVER_RADIUS));
        boxComponent->SetBackDecoration(backDecoration);
    }
    boxComponent->SetPadding(Edge());
}

void InitializeComponent(OHOS::Ace::RefPtr<OHOS::Ace::SearchComponent>& searchComponent,
                         OHOS::Ace::RefPtr<OHOS::Ace::TextFieldComponent>& textFieldComponent,
                         const OHOS::Ace::RefPtr<OHOS::Ace::SearchTheme>& searchTheme,
                         const OHOS::Ace::RefPtr<OHOS::Ace::TextFieldTheme>& textFieldTheme)
{
    textFieldComponent->SetTextFieldController(AceType::MakeRefPtr<TextFieldController>());

    auto boxComponent = ViewStackProcessor::GetInstance()->GetBoxComponent();

    InitializeDefaultValue(boxComponent, textFieldComponent, textFieldTheme);

    boxComponent->SetBackDecoration(nullptr);
    boxComponent->SetPadding(Edge());
    textFieldComponent->SetIconSize(searchTheme->GetIconSize());
    textFieldComponent->SetIconHotZoneSize(searchTheme->GetCloseIconHotZoneSize());
    Edge decorationPadding;
    Dimension leftPadding = searchTheme->GetLeftPadding();
    Dimension rightPadding = searchTheme->GetRightPadding();
    decorationPadding = Edge(rightPadding.Value(), 0.0, leftPadding.Value(), 0.0, leftPadding.Unit());
    auto textFieldDecoration = textFieldComponent->GetDecoration();
    if (textFieldDecoration) {
        textFieldDecoration->SetPadding(decorationPadding);
        textFieldDecoration->SetBorderRadius(searchTheme->GetBorderRadius());
        textFieldComponent->SetOriginBorder(textFieldDecoration->GetBorder());
    }
    textFieldComponent->SetAction(TextInputAction::SEARCH);
    textFieldComponent->SetWidthReserved(searchTheme->GetTextFieldWidthReserved());
    textFieldComponent->SetTextColor(searchTheme->GetTextColor());
    textFieldComponent->SetFocusTextColor(searchTheme->GetFocusTextColor());
    textFieldComponent->SetPlaceholderColor(searchTheme->GetPlaceholderColor());
    textFieldComponent->SetFocusPlaceholderColor(searchTheme->GetFocusPlaceholderColor());
    textFieldComponent->SetBlockRightShade(searchTheme->GetBlockRightShade());

    auto textStyle = textFieldComponent->GetTextStyle();
    searchComponent->SetPlaceHoldStyle(textStyle);
    searchComponent->SetEditingStyle(textStyle);

    std::function<void(const std::string&)> submitEvent;
    searchComponent->SetSubmitEvent(submitEvent);
    searchComponent->SetChild(textFieldComponent);
    searchComponent->SetTextEditController(textFieldComponent->GetTextEditController());
    searchComponent->SetCloseIconSize(searchTheme->GetCloseIconSize());
    searchComponent->SetCloseIconHotZoneHorizontal(searchTheme->GetCloseIconHotZoneSize());
    searchComponent->SetHoverColor(textFieldTheme->GetHoverColor());
    searchComponent->SetPressColor(textFieldTheme->GetPressColor());
    isPaddingChanged = false;
}

void PrepareSpecializedComponent(OHOS::Ace::RefPtr<OHOS::Ace::SearchComponent>& searchComponent,
                                 OHOS::Ace::RefPtr<OHOS::Ace::TextFieldComponent>& textFieldComponent)
{
    Border boxBorder;

    auto boxComponent = ViewStackProcessor::GetInstance()->GetBoxComponent();

    boxComponent->SetMouseAnimationType(HoverAnimationType::OPACITY);
    if (boxComponent->GetBackDecoration()) {
        boxBorder = boxComponent->GetBackDecoration()->GetBorder();
    }
    searchComponent->SetTextDirection(TextDirection::LTR);
    textFieldComponent->SetTextDirection(TextDirection::LTR);
    UpdateDecorationStyle(boxComponent, textFieldComponent, boxBorder, false);
    if (GreatOrEqual(boxComponent->GetHeightDimension().Value(), 0.0)) {
        textFieldComponent->SetHeight(boxComponent->GetHeightDimension());
    }
    if (isPaddingChanged) {
        auto padding = textFieldComponent->GetDecoration()->GetPadding();
        if (searchComponent->GetTextDirection() == TextDirection::RTL) {
            padding.SetLeft(padding.Left() + searchComponent->GetCloseIconHotZoneHorizontal());
        } else {
            padding.SetRight(padding.Right() + searchComponent->GetCloseIconHotZoneHorizontal());
        }
        textFieldComponent->GetDecoration()->SetPadding(padding);
        searchComponent->SetDecoration(textFieldComponent->GetDecoration());
        isPaddingChanged = false;
    }
}

}

void JSSearch::JSBind(BindingTarget globalObj)
{
    JSClass<JSSearch>::Declare("Search");
    MethodOptions opt = MethodOptions::NONE;

    JSClass<JSSearch>::StaticMethod("create", &JSSearch::Create, opt);
    JSClass<JSSearch>::StaticMethod("searchButton", &JSSearch::SetSearchButton, opt);
    JSClass<JSSearch>::StaticMethod("placeholderColor", &JSSearch::SetPlaceholderColor, opt);
    JSClass<JSSearch>::StaticMethod("placeholderFont", &JSSearch::SetPlaceholderFont, opt);
    JSClass<JSSearch>::StaticMethod("textFont", &JSSearch::SetTextFont, opt);
    JSClass<JSSearch>::StaticMethod("onSubmit", &JSSearch::OnSubmit, opt);
    JSClass<JSSearch>::StaticMethod("onChange", &JSSearch::OnChange, opt);

    JSClass<JSSearch>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSSearch>::StaticMethod("height", &JSSearch::SetHeight);
    JSClass<JSSearch>::StaticMethod("width", &JSViewAbstract::JsWidth);
    JSClass<JSSearch>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSSearch>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSSearch>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSSearch>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSSearch>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSSearch>::StaticMethod("onCopy", &JSSearch::SetOnCopy);
    JSClass<JSSearch>::StaticMethod("onCut", &JSSearch::SetOnCut);
    JSClass<JSSearch>::StaticMethod("onPaste", &JSSearch::SetOnPaste);
    JSClass<JSSearch>::Inherit<JSViewAbstract>();
    JSClass<JSSearch>::Bind(globalObj);
}

void JSSearch::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }

    if (!info[0]->IsObject()) {
        LOGE("The arg is wrong, it is supposed to be an object");
        return;
    }

    auto searchComponent = AceType::MakeRefPtr<OHOS::Ace::SearchComponent>();
    ViewStackProcessor::GetInstance()->Push(searchComponent);

    auto textFieldComponent = AceType::MakeRefPtr<OHOS::Ace::TextFieldComponent>();
    auto textFieldTheme = GetTheme<TextFieldTheme>();
    auto searchTheme = GetTheme<SearchTheme>();

    InitializeComponent(searchComponent, textFieldComponent, searchTheme, textFieldTheme);
    PrepareSpecializedComponent(searchComponent, textFieldComponent);

    auto param = JSRef<JSObject>::Cast(info[0]);
    auto value = param->GetProperty("value");
    if (!value->IsUndefined() && value->IsString()) {
        auto key = value->ToString();
        textFieldComponent->SetValue(key);
    }

    auto placeholder = param->GetProperty("placeholder");
    if (!placeholder->IsUndefined() && placeholder->IsString()) {
        auto tip = placeholder->ToString();
        textFieldComponent->SetPlaceholder(tip);
    }

    auto controllerObj = param->GetProperty("controller");
    if (!controllerObj->IsUndefined() && !controllerObj->IsNull()) {
        JSSearchController* jsController = JSRef<JSObject>::Cast(controllerObj)->Unwrap<JSSearchController>();
        if (jsController) {
            jsController->SetController(textFieldComponent->GetTextFieldController());
        }
    } else {
        LOGI("controller is nullptr");
    }

    auto icon = param->GetProperty("icon");
    if (!icon->IsUndefined() && icon->IsString()) {
        auto src = icon->ToString();
        textFieldComponent->SetIconImage(src);
    }
}

void JSSearch::SetSearchButton(const std::string& text)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto searchComponent = AceType::DynamicCast<SearchComponent>(component);
    if (!searchComponent) {
        LOGE("component error");
        return;
    }

    searchComponent->SetSearchText(text);
}

void JSSearch::SetPlaceholderColor(const JSCallbackInfo& info)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto searchComponent = AceType::DynamicCast<SearchComponent>(component);
    if (!searchComponent) {
        LOGE("search component error");
        return;
    }

    auto childComponent = searchComponent->GetChild();
    if (!childComponent) {
        LOGE("component error");
        return;
    }

    auto textFieldComponent = AceType::DynamicCast<TextFieldComponent>(childComponent);
    if (!textFieldComponent) {
        LOGE("text component error");
        return;
    }

    auto value = JSRef<JSVal>::Cast(info[0]);

    Color colorVal;
    if (ParseJsColor(value, colorVal)) {
        textFieldComponent->SetPlaceholderColor(colorVal);
        textFieldComponent->SetFocusPlaceholderColor(colorVal);
    }
}

void JSSearch::SetPlaceholderFont(const JSCallbackInfo& info)
{
    auto param = JSRef<JSObject>::Cast(info[0]);
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto searchComponent = AceType::DynamicCast<SearchComponent>(component);
    if (!searchComponent) {
        LOGE("search component error");
        return;
    }

    auto size = param->GetProperty("size");
    TextStyle textStyle = searchComponent->GetPlaceHoldStyle();

    if (!size->IsNull() && size->IsNumber()) {
        Dimension fontSize;
        if (ParseJsDimensionFp(size, fontSize)) {
            textStyle.SetFontSize(fontSize);
        }
    }

    auto weight = param->GetProperty("weight");
    if (!weight->IsNull() && weight->IsNumber()) {
        FontWeight weightVal = static_cast<FontWeight>(weight->ToNumber<int32_t>());
        textStyle.SetFontWeight(weightVal);
    }

    auto family = param->GetProperty("family");
    if (!family->IsNull() && family->IsString()) {
        auto familyVal = family->ToString();
        textStyle.SetFontFamilies(ConvertStrToFontFamilies(familyVal));
    }

    auto style = param->GetProperty("style");
    if (!style->IsNull() && style->IsNumber()) {
        FontStyle styleVal = static_cast<FontStyle>(style->ToNumber<int32_t>());
        textStyle.SetFontStyle(styleVal);
    }
    searchComponent->SetPlaceHoldStyle(textStyle);
}

void JSSearch::SetTextFont(const JSCallbackInfo& info)
{
    auto param = JSRef<JSObject>::Cast(info[0]);
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto searchComponent = AceType::DynamicCast<SearchComponent>(component);
    if (!searchComponent) {
        LOGE("search component error");
        return;
    }

    auto size = param->GetProperty("size");
    TextStyle textStyle = searchComponent->GetEditingStyle();

    if (!size->IsNull() && size->IsNumber()) {
        Dimension fontSize;
        if (ParseJsDimensionFp(size, fontSize)) {
            textStyle.SetFontSize(fontSize);
        }
    }

    auto weight = param->GetProperty("weight");
    if (!weight->IsNull() && weight->IsNumber()) {
        FontWeight weightVal = static_cast<FontWeight>(weight->ToNumber<int32_t>());
        textStyle.SetFontWeight(weightVal);
    }

    auto family = param->GetProperty("family");
    if (!family->IsNull() && family->IsString()) {
        auto familyVal = family->ToString();
        textStyle.SetFontFamilies(ConvertStrToFontFamilies(familyVal));
    }

    auto style = param->GetProperty("style");
    if (!style->IsNull() && style->IsNumber()) {
        FontStyle styleVal = static_cast<FontStyle>(style->ToNumber<int32_t>());
        textStyle.SetFontStyle(styleVal);
    }
    searchComponent->SetEditingStyle(textStyle);
}

void JSSearch::OnSubmit(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&SearchComponent::SetOnSubmit, info)) {
        LOGE("Failed to bind event");
    }
    info.ReturnSelf();
}

void JSSearch::OnChange(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&SearchComponent::SetOnChange, info)) {
        LOGE("Failed to bind event");
    }
    info.ReturnSelf();
}

void JSSearch::SetHeight(const JSCallbackInfo& info)
{
    JSViewAbstract::JsHeight(info);
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }
    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        LOGE("The arg is wrong, it is supposed to be a number arguments");
        return;
    }
    if (LessNotEqual(value.Value(), 0.0)) {
        value.SetValue(0.0);
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto searchComponent = AceType::DynamicCast<SearchComponent>(stack->GetMainComponent());
    if (!searchComponent) {
        LOGE("SearchComponent set height failed, SearchComponent is null.");
        return;
    }
    auto childComponent = searchComponent->GetChild();
    if (!childComponent) {
        LOGE("component error");
        return;
    }
    auto textFieldComponent = AceType::DynamicCast<TextFieldComponent>(childComponent);
    if (!textFieldComponent) {
        LOGE("text component error");
        return;
    }
    textFieldComponent->SetHeight(value);
}

void JSSearch::SetOnCopy(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&TextFieldComponent::SetOnCopy, info)) {
        LOGW("Failed(OnCopy) to bind event");
    }
    info.ReturnSelf();
}

void JSSearch::SetOnCut(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&TextFieldComponent::SetOnCut, info)) {
        LOGW("Failed(OnCut) to bind event");
    }
    info.ReturnSelf();
}

void JSSearch::SetOnPaste(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&TextFieldComponent::SetOnPaste, info)) {
        LOGW("Failed(OnPaste) to bind event");
    }
    info.ReturnSelf();
}

void JSSearchController::JSBind(BindingTarget globalObj)
{
    JSClass<JSSearchController>::Declare("SearchController");
    JSClass<JSSearchController>::Method("caretPosition", &JSSearchController::CaretPosition);
    JSClass<JSSearchController>::Bind(globalObj, JSSearchController::Constructor, JSSearchController::Destructor);
}

void JSSearchController::Constructor(const JSCallbackInfo& args)
{
    auto scroller = Referenced::MakeRefPtr<JSSearchController>();
    scroller->IncRefCount();
    args.SetReturnValue(Referenced::RawPtr(scroller));
}

void JSSearchController::Destructor(JSSearchController* scroller)
{
    if (scroller != nullptr) {
        scroller->DecRefCount();
    }
}

void JSSearchController::CaretPosition(int32_t caretPosition)
{
    auto controller = controller_.Upgrade();
    if (controller) {
        controller->CaretPosition(caretPosition);
    }
}

} // namespace OHOS::Ace::Framework