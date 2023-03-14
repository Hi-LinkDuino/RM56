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

#include "core/components/declaration/textfield/textfield_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/core/components/text_field/textfield_theme.h"

namespace OHOS::Ace {
namespace {

constexpr uint32_t METHOD_SHOW_ERROR_ARGS_SIZE = 1;
const TextInputAction INPUT_TEXTINPUTACTION_VALUE_DEFAULT = TextInputAction::UNSPECIFIED;
const std::vector<std::string> INPUT_FONT_FAMILY_VALUE = {
    "sans-serif",
};

bool IsRadiusStyle(const std::string& style)
{
    return (style == DOM_BORDER_TOP_LEFT_RADIUS || style == DOM_BORDER_TOP_RIGHT_RADIUS ||
            style == DOM_BORDER_BOTTOM_LEFT_RADIUS || style == DOM_BORDER_BOTTOM_RIGHT_RADIUS ||
            style == DOM_BORDER_RADIUS);
}

} // namespace

using namespace Framework;

void TextFieldDeclaration::InitSpecialized()
{
    AddCommonStyle(StyleTag::COMMON_IMAGE_STYLE);
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_TEXTFIELD_ATTR);
    AddSpecializedStyle(DeclarationConstants::DEFAULT_TEXTFIELD_STYLE);
    AddSpecializedEvent(DeclarationConstants::DEFAULT_TEXTFIELD_EVENT);
    AddSpecializedMethod(DeclarationConstants::DEFAULT_TEXTFIELD_METHOD);
}

void TextFieldDeclaration::InitializeStyle()
{
    auto theme = GetTheme<TextFieldTheme>();
    if (!theme) {
        return;
    }

    auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
    attribute.action = INPUT_TEXTINPUTACTION_VALUE_DEFAULT;
    attribute.showEllipsis = theme->ShowEllipsis();
    attribute.needFade = theme->NeedFade();
    attribute.iconSize = theme->GetIconSize();
    attribute.iconHotZoneSize = theme->GetIconHotZoneSize();

    auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
    style.height = theme->GetHeight();
    style.cursorColor = theme->GetCursorColor();
    style.cursorColorIsSet = true;
    style.cursorRadius = theme->GetCursorRadius();
    style.textColor = theme->GetTextColor();
    style.focusTextColor = theme->GetFocusTextColor();
    style.placeholderColor = theme->GetPlaceholderColor();
    style.focusPlaceholderColor = theme->GetFocusPlaceholderColor();
    style.bgColor = theme->GetBgColor();
    style.focusBgColor = theme->GetFocusBgColor();
    style.selectedColor = theme->GetSelectedColor();
    style.hoverColor = theme->GetHoverColor();
    style.pressColor = theme->GetPressColor();

    style.textStyle.SetTextColor(theme->GetTextColor());
    style.textStyle.SetFontSize(theme->GetFontSize());
    style.textStyle.SetFontWeight(theme->GetFontWeight());
    style.textStyle.SetFontFamilies(INPUT_FONT_FAMILY_VALUE);

    style.countTextStyle = theme->GetCountTextStyle();
    style.overCountStyle = theme->GetOverCountStyle();
    style.countTextStyleOuter = theme->GetCountTextStyleOuter();
    style.overCountStyleOuter = theme->GetOverCountStyleOuter();

    style.errorTextStyle = theme->GetErrorTextStyle();
    style.errorSpacing = theme->GetErrorSpacing();
    style.errorIsInner = theme->GetErrorIsInner();
    style.errorBorderWidth = theme->GetErrorBorderWidth();
    style.errorBorderColor = theme->GetErrorBorderColor();

    RefPtr<Decoration> decoration = AceType::MakeRefPtr<Decoration>();
    decoration->SetPadding(theme->GetPadding());
    decoration->SetBackgroundColor(theme->GetBgColor());
    decoration->SetBorderRadius(theme->GetBorderRadius());
    auto boxDecoration = GetDecoration();
    if (boxDecoration) {
        decoration->SetImage(boxDecoration->GetImage());
        decoration->SetGradient(boxDecoration->GetGradient());
    }
    style.decoration = decoration;

    textEditController_ = AceType::MakeRefPtr<TextEditController>();
    textFieldController_ = AceType::MakeRefPtr<TextFieldController>();
}

bool TextFieldDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    // this static map should be sorted by key.
    static const LinearMapNode<void (*)(TextFieldDeclaration&, const std::string&)> attrOperators[] = {
        { DOM_AUTO_FOCUS, [](TextFieldDeclaration& declaration,
                              const std::string& value) { declaration.SetAutoFocus(StringToBool(value)); } },
        { DOM_DISABLED, [](TextFieldDeclaration& declaration,
                            const std::string& value) { declaration.SetEnabled(!StringToBool(value)); } },
        { DOM_INPUT_ENTERKEYTYPE,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                declaration.SetAction(ConvertStrToTextInputAction(value));
            } },
        { DOM_ICON_SRC,
            [](TextFieldDeclaration& declaration, const std::string& value) { declaration.SetIconImage(value); } },
        { DOM_HIDE_ICON_SRC,
            [](TextFieldDeclaration& declaration, const std::string& value) { declaration.SetHideIconImage(value); } },
        { DOM_INPUT_MAXLENGTH,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                int32_t tmp = StringUtils::StringToInt(value);
                declaration.SetMaxLength(tmp >= 0 ? (uint32_t)(tmp) : std::numeric_limits<uint32_t>::max());
            } },
        { DOM_INPUT_OBSCURE, [](TextFieldDeclaration& declaration,
                                 const std::string& value) { declaration.SetObscure(StringToBool(value)); } },
        { DOM_INPUT_PLACEHOLDER,
            [](TextFieldDeclaration& declaration, const std::string& value) { declaration.SetPlaceholder(value); } },
        { DOM_INPUT_SELECTED_END, [](TextFieldDeclaration& declaration,
                                      const std::string& value) { declaration.SetSelectedEnd(StringToInt(value)); } },
        { DOM_INPUT_SELECTED_START,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                declaration.SetSelectedStart(StringToInt(value));
            } },
        { DOM_INPUT_SHOW_COUNTER, [](TextFieldDeclaration& declaration,
                                      const std::string& value) { declaration.SetShowCounter(StringToBool(value)); } },
        { DOM_SHOW_ICON_SRC,
            [](TextFieldDeclaration& declaration, const std::string& value) { declaration.SetShowIconImage(value); } },
        { DOM_INPUT_SHOW_PASSWORD_ICON,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                declaration.SetShowPasswordIcon(StringToBool(value));
            } },
        { DOM_INPUT_SOFT_KEYBOARD_ENABLED,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                declaration.SetSoftKeyboardEnabled(StringToBool(value));
            } },
        { DOM_INPUT_TYPE,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                declaration.SetTextInputType(ConvertStrToTextInputType(value));
                declaration.SetObscure(value == DOM_INPUT_TYPE_PASSWORD);
            } },
        { DOM_INPUT_VALUE,
            [](TextFieldDeclaration& declaration, const std::string& value) { declaration.SetValue(value); } },
    };
    auto operatorIter = BinarySearchFindIndex(attrOperators, ArraySize(attrOperators), attr.first.c_str());
    if (operatorIter != -1) {
        attrOperators[operatorIter].value(*this, attr.second);
        return true;
    }
    return false;
}

bool TextFieldDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    // static linear map must be sorted by key.
    static const LinearMapNode<void (*)(TextFieldDeclaration&, const std::string&)> styleOperators[] = {
        { DOM_TEXT_ALLOW_SCALE,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                declaration.GetTextStyle().SetAllowScale(StringToBool(value));
            } },
        { DOM_BACKGROUND_COLOR,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                declaration.SetBgColor(declaration.ParseColor(value));
                declaration.SetFocusBgColor(declaration.ParseColor(value));
            } },
        { DOM_CARET_COLOR,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                declaration.SetCursorColor(declaration.ParseColor(value));
            } },
        { DOM_INPUT_COLOR,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                declaration.GetTextStyle().SetTextColor(declaration.ParseColor(value));
                declaration.SetFocusTextColor(declaration.ParseColor(value));
            } },
        { DOM_INPUT_CURSOR_COLOR,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                declaration.SetCursorColor(declaration.ParseColor(value));
            } },
        { DOM_INPUT_FONT_FAMILY,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                std::vector<std::string> fontFamilies;
                std::stringstream sstr(value);
                std::string fontFamily;
                while (getline(sstr, fontFamily, ',')) {
                    fontFamilies.emplace_back(fontFamily);
                }
                declaration.GetTextStyle().SetFontFamilies(fontFamilies);
            } },
        { DOM_INPUT_FONT_SIZE,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                declaration.GetTextStyle().SetFontSize(declaration.ParseDimension(value));
            } },
        { DOM_INPUT_FONT_WEIGHT,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                declaration.GetTextStyle().SetFontWeight(ConvertStrToFontWeight(value));
            } },
        { DOM_PADDING,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                Edge padding;
                if (Edge::FromString(value, padding)) {
                    declaration.GetDecoration()->SetPadding(padding);
                }
            } },
        { DOM_PADDING_BOTTOM,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                auto padding = declaration.GetDecoration()->GetPadding();
                padding.SetBottom(declaration.ParseDimension(value));
                declaration.GetDecoration()->SetPadding(padding);
            } },
        { DOM_PADDING_END,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                auto padding = declaration.GetDecoration()->GetPadding();
                declaration.IsRightToLeft() ? padding.SetLeft(declaration.ParseDimension(value))
                                            : padding.SetRight(declaration.ParseDimension(value));
                declaration.GetDecoration()->SetPadding(padding);
            } },
        { DOM_PADDING_LEFT,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                auto padding = declaration.GetDecoration()->GetPadding();
                padding.SetLeft(declaration.ParseDimension(value));
                declaration.GetDecoration()->SetPadding(padding);
            } },
        { DOM_PADDING_RIGHT,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                auto padding = declaration.GetDecoration()->GetPadding();
                padding.SetRight(declaration.ParseDimension(value));
                declaration.GetDecoration()->SetPadding(padding);
            } },
        { DOM_PADDING_START,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                auto padding = declaration.GetDecoration()->GetPadding();
                declaration.IsRightToLeft() ? padding.SetRight(declaration.ParseDimension(value))
                                            : padding.SetLeft(declaration.ParseDimension(value));
                declaration.GetDecoration()->SetPadding(padding);
            } },
        { DOM_PADDING_TOP,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                auto padding = declaration.GetDecoration()->GetPadding();
                padding.SetTop(declaration.ParseDimension(value));
                declaration.GetDecoration()->SetPadding(padding);
            } },
        { DOM_INPUT_PLACEHOLDER_COLOR,
            [](TextFieldDeclaration& declaration, const std::string& value) {
                declaration.SetPlaceholderColor(declaration.ParseColor(value));
                declaration.SetFocusPlaceholderColor(declaration.ParseColor(value));
            } },
    };
    if (IsRadiusStyle(style.first)) {
        hasBoxRadius_ = true;
    }
    auto operatorIter = BinarySearchFindIndex(styleOperators, ArraySize(styleOperators), style.first.c_str());
    if (operatorIter != -1) {
        styleOperators[operatorIter].value(*this, style.second);
        return true;
    }
    return false;
}

bool TextFieldDeclaration::SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event)
{
    static const LinearMapNode<void (*)(TextFieldDeclaration&, const EventMarker&)> eventOperators[] = {
        { DOM_CATCH_BUBBLE_CLICK,
            [](TextFieldDeclaration& declaration, const EventMarker& event) {
                EventMarker eventMarker(event);
                eventMarker.SetCatchMode(true);
                declaration.SetOnTap(eventMarker);
            } },
        { DOM_CHANGE,
            [](TextFieldDeclaration& declaration, const EventMarker& event) { declaration.SetOnTextChange(event); } },
        { DOM_CLICK, [](TextFieldDeclaration& declaration, const EventMarker& event) { declaration.SetOnTap(event); } },
        { DOM_INPUT_EVENT_ENTERKEYCLICK,
            [](TextFieldDeclaration& declaration, const EventMarker& event) { declaration.SetOnFinishInput(event); } },
        { DOM_LONG_PRESS,
            [](TextFieldDeclaration& declaration, const EventMarker& event) { declaration.SetOnLongPress(event); } },
        { DOM_INPUT_EVENT_OPTION_SELECT,
            [](TextFieldDeclaration& declaration, const EventMarker& event) { declaration.SetOnOptionsClick(event); } },
        { DOM_INPUT_EVENT_SEARCH,
            [](TextFieldDeclaration& declaration, const EventMarker& event) { declaration.SetOnSearch(event); } },
        { DOM_INPUT_EVENT_SELECT_CHANGE,
            [](TextFieldDeclaration& declaration, const EventMarker& event) { declaration.SetOnSelectChange(event); } },
        { DOM_INPUT_EVENT_SHARE,
            [](TextFieldDeclaration& declaration, const EventMarker& event) { declaration.SetOnShare(event); } },
        { DOM_INPUT_EVENT_TRANSLATE,
            [](TextFieldDeclaration& declaration, const EventMarker& event) { declaration.SetOnTranslate(event); } },
    };
    auto operatorIter = BinarySearchFindIndex(eventOperators, ArraySize(eventOperators), event.c_str());
    if (operatorIter != -1) {
        eventOperators[operatorIter].value(*this, EventMarker(eventId, event, pageId));
        return true;
    }
    return false;
}

void TextFieldDeclaration::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    if (!textFieldController_) {
        return;
    }
    if (method == DOM_INPUT_METHOD_SHOW_ERROR) {
        std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
        if (!argsValue || !argsValue->IsArray() || argsValue->GetArraySize() != METHOD_SHOW_ERROR_ARGS_SIZE) {
            LOGE("parse args error");
            return;
        }

        std::unique_ptr<JsonValue> error = argsValue->GetArrayItem(0)->GetValue("error");
        if (!error || !error->IsString()) {
            LOGE("get error text failed");
            return;
        }
        std::string errorText = error->GetString();
        textFieldController_->ShowError(errorText);
    } else if (method == DOM_INPUT_METHOD_DELETE) {
        textFieldController_->Delete();
    }
}

void TextFieldDeclaration::OnRequestFocus(bool shouldFocus)
{
    if (textFieldController_) {
        textFieldController_->Focus(shouldFocus);
    }
}

} // namespace OHOS::Ace
