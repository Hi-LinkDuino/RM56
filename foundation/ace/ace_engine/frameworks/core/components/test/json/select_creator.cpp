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

#include "core/components/test/json/select_creator.h"

#include "core/components/image/image_component.h"
#include "core/components/select/select_component.h"
#include "core/components/text/text_component.h"

namespace OHOS::Ace {
namespace {

constexpr FontWeight FONT_WEIGHTS[] = { FontWeight::W100, FontWeight::W200, FontWeight::W300, FontWeight::W400,
    FontWeight::W500, FontWeight::W600, FontWeight::W700, FontWeight::W800, FontWeight::W900, FontWeight::BOLD,
    FontWeight::NORMAL, FontWeight::BOLDER, FontWeight::LIGHTER };
constexpr TextDecoration TEXT_DECORATIONS[] = { TextDecoration::NONE, TextDecoration::UNDERLINE,
    TextDecoration::OVERLINE, TextDecoration::LINE_THROUGH, TextDecoration::INHERIT };

} // namespace

RefPtr<SelectTheme> SelectCreator::theme_ = nullptr;

RefPtr<Component> SelectCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("SelectCreator::CreateFromJson");
    std::string strValue;
    if (!GetStringValue(componentJson, CLASS_NAME, strValue)) {
        return nullptr;
    }

    if (strValue != SELECT_NAME) {
        LOGE("The class name[%{public}s] error, should be [%{public}s].", strValue.c_str(), SELECT_NAME);
        return nullptr;
    }

    LOGD("Create object of className[%{public}s].", strValue.c_str());
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    theme_ = themeManager->GetTheme<SelectTheme>();
    auto select = AceType::MakeRefPtr<SelectComponent>();
    select->InitTheme(themeManager);
    CreateCommonAttribute(select, componentJson);
    CreateCommonColorAttribute(select, componentJson);
    CreateOptionsAttribute(select, componentJson, themeManager);

    AdjustTextBySelectedOption(select);
    return select;
}

bool SelectCreator::GetDoubleValue(const JsonValue& json, const std::string& name, double& outValue)
{
    if (!json.Contains(name)) {
        LOGE("The json is error, do not contain element[%{private}s].", name.c_str());
        return false;
    }

    auto jsonValue = json.GetValue(name);
    if (!jsonValue->IsNumber()) {
        LOGE("The json is error, type of element[%{private}s] is not number.", name.c_str());
        return false;
    }

    outValue = jsonValue->GetDouble();
    return true;
}

bool SelectCreator::GetStringValue(const JsonValue& json, const std::string& name, std::string& outValue)
{
    if (!json.Contains(name)) {
        LOGE("The json is error, do not contain element[%{private}s].", name.c_str());
        return false;
    }

    auto jsonValue = json.GetValue(name);
    if (!jsonValue->IsString()) {
        LOGE("The json is error, type of element[%{private}s] is not string.", name.c_str());
        return false;
    }

    outValue = jsonValue->GetString();
    return true;
}

bool SelectCreator::GetColorValue(const JsonValue& json, const std::string& name, Color& outValue)
{
    if (!json.Contains(name)) {
        LOGE("The json is error, do not contain element[%{private}s].", name.c_str());
        return false;
    }

    auto jsonValue = json.GetValue(name);
    if (!jsonValue->IsObject()) {
        LOGE("The json is error, type of element[%{private}s] is not object.", name.c_str());
        return false;
    }

    outValue = ColorCreator::CreateFromJson(*jsonValue);
    return true;
}

void SelectCreator::CreateCommonAttribute(const RefPtr<Component>& component, const JsonValue& componentJson)
{
    std::string strValue;
    double attrValue = 0.0;
    RefPtr<SelectComponent> select = AceType::DynamicCast<SelectComponent>(component);
    if (!select || !theme_) {
        LOGE("select: select or theme is null.");
        return;
    }

    select->SetRRectSize(theme_->GetRRectSize());
    select->SetPopupShadowWidth(theme_->GetPopupShadowWidth());
    select->SetPopupBorderWidth(theme_->GetPopupBorderWidth());

    if (GetStringValue(componentJson, SELECT_DISABLED, strValue)) {
        select->SetDisabled(strValue == "true" ? true : false);
    }

    if (GetStringValue(componentJson, SELECT_ON_CHANGED, strValue)) {
        EventMarker evtMarker { strValue };
        select->SetOnChanged(evtMarker);
    }

    RefPtr<TextComponent> tipText = AceType::MakeRefPtr<TextComponent>("");
    if (GetStringValue(componentJson, SELECT_TIP_TEXT, strValue)) {
        tipText->SetData(strValue);
    }
    select->SetTipText(tipText);

    if (GetDoubleValue(componentJson, SELECT_OPTION_SIZE, attrValue)) {
        select->SetOptionSize(static_cast<std::size_t>(attrValue));
    } else {
        select->SetOptionSize(theme_->GetOptionSize());
    }

    if (GetDoubleValue(componentJson, TEXT_FONT_SIZE, attrValue)) {
        select->SetFontSize(Dimension(attrValue));
    } else {
        select->SetFontSize(theme_->GetFontSize());
    }

    if (GetStringValue(componentJson, TEXT_FONT_FAMILIES, strValue)) {
        select->SetFontFamily(strValue);
    } else {
        select->SetFontFamily(theme_->GetFontFamily());
    }
}

void SelectCreator::CreateCommonColorAttribute(const RefPtr<Component>& component, const JsonValue& componentJson)
{
    Color clrValue = Color::WHITE;
    RefPtr<SelectComponent> select = AceType::DynamicCast<SelectComponent>(component);
    if (!select || !theme_) {
        LOGE("select: select or theme is null.");
        return;
    }

    if (GetColorValue(componentJson, SELECT_SELECTED_COLOR, clrValue)) {
        select->SetSelectedColor(clrValue);
    } else {
        select->SetSelectedColor(theme_->GetSelectedColor());
    }

    if (GetColorValue(componentJson, SELECT_CLICKED_COLOR, clrValue)) {
        select->SetClickedColor(clrValue);
    } else {
        select->SetClickedColor(theme_->GetClickedColor());
    }

    if (GetColorValue(componentJson, SELECT_DISABLED_COLOR, clrValue)) {
        select->SetDisabledColor(clrValue);
    } else {
        select->SetDisabledColor(theme_->GetDisabledColor());
    }
}

void SelectCreator::CreateOptionsAttribute(
    const RefPtr<Component>& component, const JsonValue& componentJson, const RefPtr<ThemeManager>& themeManager)
{
    std::string strValue;
    RefPtr<SelectComponent> select = AceType::DynamicCast<SelectComponent>(component);
    if (!select || !theme_) {
        LOGE("select: select or theme is null.");
        return;
    }

    if (!componentJson.Contains(SELECT_OPTIONS)) {
        LOGD("select: json do not contains select options.");
        return;
    }

    auto optionJsonArray = componentJson.GetValue(SELECT_OPTIONS);
    if (!optionJsonArray->IsArray()) {
        LOGD("select: select options of json is not array.");
        return;
    }

    select->ClearAllOptions();
    for (int32_t index = 0; index < optionJsonArray->GetArraySize(); index++) {
        auto optionJson = optionJsonArray->GetArrayItem(index);
        RefPtr<OptionComponent> option = AceType::MakeRefPtr<OptionComponent>();
        option->InitTheme(themeManager);

        if (GetStringValue(*optionJson, SELECT_OPTION_ICON, strValue)) {
            RefPtr<ImageComponent> icon = AceType::MakeRefPtr<ImageComponent>();
            icon->SetSrc(strValue);
            option->SetIcon(icon);
        }

        RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("");
        if (GetStringValue(*optionJson, SELECT_OPTION_TEXT, strValue)) {
            text->SetData(strValue);
        }
        option->SetText(text);
        option->SetSelectedColor(select->GetSelectedColor());
        option->SetClickedColor(select->GetClickedColor());
        CreateOptionCommonAttribute(option, *optionJson);
        select->AppendSelectOption(option);
    }
}

void SelectCreator::CreateOptionCommonAttribute(const RefPtr<Component>& component, const JsonValue& componentJson)
{
    std::string strValue;
    RefPtr<OptionComponent> option = AceType::DynamicCast<OptionComponent>(component);
    if (!option || !theme_) {
        LOGE("select: option or theme is null.");
        return;
    }

    if (GetStringValue(componentJson, SELECT_OPTION_VALUE, strValue)) {
        option->SetValue(strValue);
    }

    if (GetStringValue(componentJson, SELECT_OPTION_SELECTED, strValue)) {
        option->SetSelected(strValue == "true" ? true : false);
    }

    Color clrValue;
    if (GetColorValue(componentJson, TEXT_COLOR, clrValue)) {
        option->SetFontColor(clrValue);
    } else {
        option->SetFontColor(theme_->GetFontColor());
    }

    double attrValue = 0.0;
    if (GetDoubleValue(componentJson, TEXT_FONT_SIZE, attrValue)) {
        option->SetFontSize(Dimension(attrValue));
    } else {
        option->SetFontSize(theme_->GetFontSize());
    }

    if (GetDoubleValue(componentJson, TEXT_FONT_WEIGHT, attrValue)) {
        option->SetFontWeight(ConvertIntToEnum(static_cast<int32_t>(attrValue), FONT_WEIGHTS, FontWeight::NORMAL));
    } else {
        option->SetFontWeight(theme_->GetFontWeight());
    }

    if (GetDoubleValue(componentJson, TEXT_DECORATION, attrValue)) {
        option->SetTextDecoration(
            ConvertIntToEnum(static_cast<int32_t>(attrValue), TEXT_DECORATIONS, TextDecoration::NONE));
    } else {
        option->SetTextDecoration(theme_->GetTextDecoration());
    }

    if (GetStringValue(componentJson, TEXT_FONT_FAMILIES, strValue)) {
        option->SetFontFamily(strValue);
    } else {
        option->SetFontFamily(theme_->GetFontFamily());
    }
}

void SelectCreator::AdjustTextBySelectedOption(const RefPtr<Component>& component)
{
    RefPtr<SelectComponent> select = AceType::DynamicCast<SelectComponent>(component);
    if (!select) {
        return;
    }

    auto tipText = select->GetTipText();
    if (!tipText) {
        return;
    }

    for (std::size_t index = 0; index < select->GetSelectOptionCount(); index++) {
        auto option = select->GetSelectOption(index);
        if (!(option && option->GetSelected())) {
            continue;
        }

        auto optionText = option->GetText();
        if (!optionText) {
            continue;
        }

        tipText->SetData(optionText->GetData());
        return;
    }
}

} // namespace OHOS::Ace
