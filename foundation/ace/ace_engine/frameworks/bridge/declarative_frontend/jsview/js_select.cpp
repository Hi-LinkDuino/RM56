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

#include "bridge/declarative_frontend/jsview/js_select.h"

#include <string>
#include <vector>

#include "bridge/declarative_frontend/jsview/js_interactable_view.h"

#include "core/components/option/option_component.h"
#include "core/components/select/select_component.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_common_def.h"

namespace OHOS::Ace::Framework {
void JSSelect::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 0) {
        return;
    }

    auto selectTheme = GetTheme<SelectTheme>();
    auto selectComponent = AceType::MakeRefPtr<SelectComponent>();
    selectComponent->SetTheme(selectTheme);

    auto tipText = AceType::MakeRefPtr<TextComponent>("");
    selectComponent->SetTipText(tipText);

    if (info[0]->IsArray()) {
        auto paramArray = JSRef<JSArray>::Cast(info[0]);
        size_t size = paramArray->Length();
        for (size_t i = 0; i < size; i++) {
            std::string value;
            std::string icon;

            auto indexObject = JSRef<JSObject>::Cast(paramArray->GetValueAt(i));
            auto selectValue = indexObject->GetProperty("value");
            auto selectIcon = indexObject->GetProperty("icon");
            if (!ParseJsString(selectValue, value)) {
                LOGE("selectValue is null");
                return;
            }
            if (!ParseJsMedia(selectIcon, icon)) {
                LOGE("selectValue is null");
            }

            auto optionTheme = GetTheme<SelectTheme>();
            auto optionComponent = AceType::MakeRefPtr<OHOS::Ace::OptionComponent>(optionTheme);
            auto textComponent = AceType::MakeRefPtr<OHOS::Ace::TextComponent>(value);
            auto iconComponent = AceType::MakeRefPtr<OHOS::Ace::ImageComponent>(icon);
            optionComponent->SetTheme(optionTheme);
            optionComponent->SetText(textComponent);
            optionComponent->SetIcon(iconComponent);
            optionComponent->SetTextStyle(optionTheme->GetTitleStyle());
            optionComponent->SetSelectedTextStyle(optionTheme->GetTitleStyle());
            optionComponent->SetSelectedBackgroundColor(optionTheme->GetSelectedColor());
            optionComponent->SetValue(value);
            selectComponent->AppendSelectOption(optionComponent);
        }
    }

    ViewStackProcessor::GetInstance()->Push(selectComponent);
}

void JSSelect::JSBind(BindingTarget globalObj)
{
    JSClass<JSSelect>::Declare("Select");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSSelect>::StaticMethod("create", &JSSelect::Create, opt);

    JSClass<JSSelect>::StaticMethod("selected", &JSSelect::Selected, opt);
    JSClass<JSSelect>::StaticMethod("value", &JSSelect::Value, opt);
    JSClass<JSSelect>::StaticMethod("font", &JSSelect::Font, opt);
    JSClass<JSSelect>::StaticMethod("fontColor", &JSSelect::FontColor, opt);
    JSClass<JSSelect>::StaticMethod("selectedOptionBgColor", &JSSelect::SelectedOptionBgColor, opt);
    JSClass<JSSelect>::StaticMethod("selectedOptionFont", &JSSelect::SelectedOptionFont, opt);
    JSClass<JSSelect>::StaticMethod("selectedOptionFontColor", &JSSelect::SelectedOptionFontColor, opt);
    JSClass<JSSelect>::StaticMethod("optionBgColor", &JSSelect::OptionBgColor, opt);
    JSClass<JSSelect>::StaticMethod("optionFont", &JSSelect::OptionFont, opt);
    JSClass<JSSelect>::StaticMethod("optionFontColor", &JSSelect::OptionFontColor, opt);
    JSClass<JSSelect>::StaticMethod("onSelected", &JSSelect::OnSelected, opt);
    JSClass<JSSelect>::StaticMethod("width", &JSSelect::JsWidth);
    JSClass<JSSelect>::StaticMethod("height", &JSSelect::JsHeight);
    JSClass<JSSelect>::StaticMethod("size", &JSSelect::JsSize);
    JSClass<JSSelect>::StaticMethod("padding", &JSSelect::JsPadding);
    JSClass<JSSelect>::StaticMethod("paddingTop", &JSSelect::SetPaddingTop, opt);
    JSClass<JSSelect>::StaticMethod("paddingBottom", &JSSelect::SetPaddingBottom, opt);
    JSClass<JSSelect>::StaticMethod("paddingLeft", &JSSelect::SetPaddingLeft, opt);
    JSClass<JSSelect>::StaticMethod("paddingRight", &JSSelect::SetPaddingRight, opt);

    JSClass<JSSelect>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSSelect>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSSelect>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSSelect>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSSelect>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSSelect>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSSelect>::Inherit<JSViewAbstract>();
    JSClass<JSSelect>::Bind(globalObj);
}

void JSSelect::Selected(int value)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto selectComponent = AceType::DynamicCast<SelectComponent>(component);
    if (!selectComponent) {
        LOGE("search component error");
        return;
    }
    auto popup = selectComponent->GetPopup();
    if (!popup) {
        LOGE("popup is null");
        return;
    }
    auto option = popup->GetSelectOptions();
    if (value < 0 || value >= static_cast<int32_t>(option.size())) {
        LOGE("Input selected index error, use the default value");
        value = 0;
    }

    auto tipText = selectComponent->GetTipText();
    auto optionComponent = selectComponent->GetSelectOption(value);
    optionComponent->SetSelected(true);

    auto optionText = optionComponent->GetText();
    if (!optionText) {
        return;
    }
    tipText->SetData(optionText->GetData());
}

void JSSelect::Value(const std::string& value)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto selectComponent = AceType::DynamicCast<SelectComponent>(component);
    if (!selectComponent) {
        LOGE("search component error");
        return;
    }
    auto tipText = selectComponent->GetTipText();
    tipText->SetData(value);
}

void JSSelect::Font(const JSCallbackInfo& info)
{
    if (!info[0]->IsObject()) {
        return;
    }
    auto param = JSRef<JSObject>::Cast(info[0]);
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto selectComponent = AceType::DynamicCast<SelectComponent>(component);
    if (!selectComponent) {
        LOGE("search component error");
        return;
    }

    auto size = param->GetProperty("size");
    TextStyle textStyle = selectComponent->GetSelectStyle();
    if (!size->IsNull() && size->IsNumber()) {
        Dimension fontSize;
        if (ParseJsDimensionFp(size, fontSize)) {
            textStyle.SetFontSize(fontSize);
        }
    }

    std::string weight;
    auto fontWeight = param->GetProperty("weight");
    if (!fontWeight->IsNull()) {
        if (fontWeight->IsNumber()) {
            weight = std::to_string(fontWeight->ToNumber<int32_t>());
        } else {
            ParseJsString(fontWeight, weight);
        }
        textStyle.SetFontWeight(ConvertStrToFontWeight(weight));
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
    selectComponent->SetSelectStyle(std::move(textStyle));
}

void JSSelect::FontColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }

    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto selectComponent = AceType::DynamicCast<SelectComponent>(component);
    if (!selectComponent) {
        LOGE("search component error");
        return;
    }

    Color textColor;
    if (!ParseJsColor(info[0], textColor)) {
        return;
    }
    auto textStyle = selectComponent->GetSelectStyle();
    textStyle.SetTextColor(textColor);
    selectComponent->SetSelectStyle(std::move(textStyle));
}

void JSSelect::SelectedOptionBgColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color bgColor;
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto selectComponent = AceType::DynamicCast<SelectComponent>(component);
    if (!selectComponent) {
        LOGE("search component error");
        return;
    }
    auto popup = selectComponent->GetPopup();
    if (!popup) {
        LOGE("popup is null");
        return;
    }
    auto option = popup->GetSelectOptions();
    if (!ParseJsColor(info[0], bgColor)) {
        return;
    }
    for (auto& optionItem : option) {
        if (optionItem) {
            optionItem->SetSelectedBackgroundColor(bgColor);
        }
    }
}

void JSSelect::SelectedOptionFont(const JSCallbackInfo& info)
{
    if (!info[0]->IsObject()) {
        return;
    }
    auto param = JSRef<JSObject>::Cast(info[0]);
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto selectComponent = AceType::DynamicCast<SelectComponent>(component);
    if (!selectComponent) {
        LOGE("search component error");
        return;
    }
    auto popup = selectComponent->GetPopup();
    if (!popup) {
        LOGE("popup is null");
        return;
    }
    auto option = popup->GetSelectOptions();
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    for (auto& optionItem : option) {
        auto size = param->GetProperty("size");
        TextStyle textStyle = optionItem->GetSelectedTextStyle();

        if (!size->IsNull() && size->IsNumber()) {
            Dimension fontSize;
            if (ParseJsDimensionFp(size, fontSize)) {
                textStyle.SetFontSize(fontSize);
            }
        }

        std::string weight;
        auto fontWeight = param->GetProperty("weight");
        if (!fontWeight->IsNull()) {
            if (fontWeight->IsNumber()) {
                weight = std::to_string(fontWeight->ToNumber<int32_t>());
            } else {
                ParseJsString(fontWeight, weight);
            }
            textStyle.SetFontWeight(ConvertStrToFontWeight(weight));
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

        optionItem->SetSelectedTextStyle(std::move(textStyle));
    }
}

void JSSelect::SelectedOptionFontColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color textColor;
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto selectComponent = AceType::DynamicCast<SelectComponent>(component);
    if (!selectComponent) {
        LOGE("search component error");
        return;
    }
    auto popup = selectComponent->GetPopup();
    if (!popup) {
        LOGE("popup is null");
        return;
    }
    auto option = popup->GetSelectOptions();
    if (!ParseJsColor(info[0], textColor)) {
        return;
    }
    for (auto& optionItem : option) {
        if (optionItem) {
            TextStyle textStyle = optionItem->GetSelectedTextStyle();
            textStyle.SetTextColor(textColor);
            optionItem->SetSelectedTextStyle(textStyle);
        }
    }
}

void JSSelect::OptionBgColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color bgColor;
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto selectComponent = AceType::DynamicCast<SelectComponent>(component);
    if (!selectComponent) {
        LOGE("search component error");
        return;
    }
    auto popup = selectComponent->GetPopup();
    if (!popup) {
        LOGE("popup is null");
        return;
    }
    auto option = popup->GetSelectOptions();
    if (!ParseJsColor(info[0], bgColor)) {
        return;
    }
    for (auto& optionItem : option) {
        if (optionItem) {
            optionItem->SetBackgroundColor(bgColor);
        }
    }
}

void JSSelect::OptionFont(const JSCallbackInfo& info)
{
    if (!info[0]->IsObject()) {
        return;
    }
    auto param = JSRef<JSObject>::Cast(info[0]);
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto selectComponent = AceType::DynamicCast<SelectComponent>(component);
    if (!selectComponent) {
        LOGE("search component error");
        return;
    }
    auto popup = selectComponent->GetPopup();
    if (!popup) {
        LOGE("popup is null");
        return;
    }
    auto option = popup->GetSelectOptions();
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    for (auto& optionItem : option) {
        TextStyle textStyle = optionItem->GetTextStyle();
        auto size = param->GetProperty("size");
        if (!size->IsNull() && size->IsNumber()) {
            Dimension fontSize;
            if (ParseJsDimensionFp(size, fontSize)) {
                textStyle.SetFontSize(fontSize);
            }
        }

        std::string weight;
        auto fontWeight = param->GetProperty("weight");
        if (!fontWeight->IsNull()) {
            if (fontWeight->IsNumber()) {
                weight = std::to_string(fontWeight->ToNumber<int32_t>());
            } else {
                ParseJsString(fontWeight, weight);
            }
            textStyle.SetFontWeight(ConvertStrToFontWeight(weight));
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

        optionItem->SetTextStyle(std::move(textStyle));
    }
}

void JSSelect::OptionFontColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color textColor;
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto selectComponent = AceType::DynamicCast<SelectComponent>(component);
    if (!selectComponent) {
        LOGE("search component error");
        return;
    }
    auto popup = selectComponent->GetPopup();
    if (!popup) {
        LOGE("popup is null");
        return;
    }
    auto option = popup->GetSelectOptions();
    if (!ParseJsColor(info[0], textColor)) {
        return;
    }
    for (auto& optionItem : option) {
        if (optionItem) {
            TextStyle textStyle = optionItem->GetTextStyle();
            textStyle.SetTextColor(textColor);
            optionItem->SetTextStyle(textStyle);
        }
    }
}

void JSSelect::OnSelected(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&SelectComponent::SetOnSelected, info)) {
        LOGE("Failed to bind event");
    }
    info.ReturnSelf();
}

void JSSelect::JsWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    Width(info[0]);
}

void JSSelect::Width(const JSRef<JSVal>& jsValue)
{
    Dimension value;
    if (!ParseJsDimensionVp(jsValue, value)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto selectComponent = AceType::DynamicCast<SelectComponent>(stack->GetMainComponent());
    if (selectComponent) {
        selectComponent->SetWidth(value);
    }
}

void JSSelect::JsHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    Height(info[0]);
}

void JSSelect::Height(const JSRef<JSVal>& jsValue)
{
    Dimension value;
    if (!ParseJsDimensionVp(jsValue, value)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto selectComponent = AceType::DynamicCast<SelectComponent>(stack->GetMainComponent());
    if (selectComponent) {
        selectComponent->SetHeight(value);
    }
}

void JSSelect::JsSize(const JSCallbackInfo& info)
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
    Width(sizeObj->GetProperty("width"));
    Height(sizeObj->GetProperty("height"));
}

void JSSelect::JsPadding(const JSCallbackInfo& info)
{
    if (!info[0]->IsString() && !info[0]->IsNumber() && !info[0]->IsObject()) {
        LOGE("arg is not a string, number or object.");
        return;
    }

    if (info[0]->IsObject()) {
        auto stack = ViewStackProcessor::GetInstance();
        auto selectComponent = AceType::DynamicCast<SelectComponent>(stack->GetMainComponent());
        if (!selectComponent) {
            LOGE("search component error");
            return;
        }
        auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
        if (!argsPtrItem || argsPtrItem->IsNull()) {
            LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
            return;
        }
        if (argsPtrItem->Contains("top")) {
            Dimension topDimen = Dimension(0.0, DimensionUnit::VP);
            if (ParseJsonDimensionVp(argsPtrItem->GetValue("top"), topDimen)) {
                selectComponent->SetTopPadding(topDimen);
            }  
        }
        if (argsPtrItem->Contains("left")) {
            Dimension leftDimen = Dimension(0.0, DimensionUnit::VP);
            if (ParseJsonDimensionVp(argsPtrItem->GetValue("left"), leftDimen)) {
                selectComponent->SetLeftPadding(leftDimen);
            }  
        }
        if (argsPtrItem->Contains("right")) {
            Dimension rightDimen = Dimension(0.0, DimensionUnit::VP);
            if (ParseJsonDimensionVp(argsPtrItem->GetValue("right"), rightDimen)) {
                selectComponent->SetRightPadding(rightDimen);
            }  
        }
        if (argsPtrItem->Contains("bottom")) {
            Dimension bottomDimen = Dimension(0.0, DimensionUnit::VP);
            if (ParseJsonDimensionVp(argsPtrItem->GetValue("bottom"), bottomDimen)) {
                selectComponent->SetBottomPadding(bottomDimen);
            }  
        }
    }
    Dimension length;
    if (ParseJsDimensionVp(info[0], length)) {
        auto stack = ViewStackProcessor::GetInstance();
        auto selectComponent = AceType::DynamicCast<SelectComponent>(stack->GetMainComponent());
        if (selectComponent) {
            selectComponent->SetLeftPadding(length);
            selectComponent->SetTopPadding(length);
            selectComponent->SetRightPadding(length);
            selectComponent->SetBottomPadding(length);
        }
    }
}

void JSSelect::SetPaddingLeft(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto selectComponent = AceType::DynamicCast<SelectComponent>(stack->GetMainComponent());
    if (selectComponent) {
        selectComponent->SetLeftPadding(value);
    }
}

void JSSelect::SetPaddingTop(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto selectComponent = AceType::DynamicCast<SelectComponent>(stack->GetMainComponent());
    if (selectComponent) {
        selectComponent->SetTopPadding(value);
    }
}

void JSSelect::SetPaddingRight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto selectComponent = AceType::DynamicCast<SelectComponent>(stack->GetMainComponent());
    if (selectComponent) {
        selectComponent->SetRightPadding(value);
    }
}

void JSSelect::SetPaddingBottom(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto selectComponent = AceType::DynamicCast<SelectComponent>(stack->GetMainComponent());
    if (selectComponent) {
        selectComponent->SetBottomPadding(value);
    }
}
} // namespace OHOS::Ace::Framework
