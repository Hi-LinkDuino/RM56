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

#include "core/components/declaration/search/search_declaration.h"

#include "base/utils/string_utils.h"
#include "base/utils/system_properties.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/core/components/search/search_theme.h"
#include "frameworks/core/components/text_field/textfield_theme.h"

namespace OHOS::Ace {

using namespace Framework;

SearchDeclaration::SearchDeclaration()
{
    textFieldDeclaration_ = AceType::MakeRefPtr<TextFieldDeclaration>();
    textFieldDeclaration_->BindPipelineContext(pipelineContext_);
    textFieldDeclaration_->Init();
}

void SearchDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SEARCH_ATTR);
    AddSpecializedStyle(DeclarationConstants::DEFAULT_SEARCH_STYLE);
    AddSpecializedEvent(DeclarationConstants::DEFAULT_SEARCH_EVENT);
    AddSpecializedMethod(DeclarationConstants::DEFAULT_SEARCH_METHOD);
}

void SearchDeclaration::InitializeStyle()
{
    textFieldDeclaration_->InitializeStyle();

    auto textFieldTheme = GetTheme<TextFieldTheme>();
    auto searchTheme = GetTheme<SearchTheme>();
    if (!textFieldTheme || !searchTheme) {
        // theme is null, set default decoration to text field component
        RefPtr<Decoration> decoration = AceType::MakeRefPtr<Decoration>();
        textFieldDeclaration_->SetDecoration(decoration);
        LOGE("textFieldTheme or searchTheme is null");
        return;
    }

    auto& paddingStyle = static_cast<CommonPaddingStyle&>(GetStyle(StyleTag::COMMON_PADDING_STYLE));
    if (paddingStyle.IsValid()) {
        paddingStyle.padding = Edge();
    }

    auto& sizeStyle = static_cast<CommonSizeStyle&>(GetStyle(StyleTag::COMMON_SIZE_STYLE));
    if (sizeStyle.IsValid()) {
        sizeStyle.height = searchTheme->GetHeight();
    }

    SetBackDecoration(nullptr);

    textFieldDeclaration_->SetIconSize(searchTheme->GetIconSize());
    textFieldDeclaration_->SetIconHotZoneSize(searchTheme->GetCloseIconHotZoneSize());
    Edge decorationPadding;
    Dimension leftPadding = searchTheme->GetLeftPadding();
    Dimension rightPadding = searchTheme->GetRightPadding();
    if (IsRightToLeft()) {
        decorationPadding = Edge(rightPadding.Value(), 0.0, leftPadding.Value(), 0.0, leftPadding.Unit());
    } else {
        decorationPadding = Edge(leftPadding.Value(), 0.0, rightPadding.Value(), 0.0, leftPadding.Unit());
    }
    auto textFieldDecoration = textFieldDeclaration_->GetDecoration();
    if (textFieldDecoration) {
        textFieldDecoration->SetPadding(decorationPadding);
        textFieldDecoration->SetBorderRadius(searchTheme->GetBorderRadius());
        textFieldDeclaration_->SetOriginBorder(textFieldDecoration->GetBorder());
    }
    textFieldDeclaration_->SetAction(TextInputAction::SEARCH);
    textFieldDeclaration_->SetWidthReserved(searchTheme->GetTextFieldWidthReserved());
    textFieldDeclaration_->SetTextColor(searchTheme->GetTextColor());
    textFieldDeclaration_->SetFocusTextColor(searchTheme->GetFocusTextColor());
    textFieldDeclaration_->SetPlaceholderColor(searchTheme->GetPlaceholderColor());
    textFieldDeclaration_->SetFocusPlaceholderColor(searchTheme->GetFocusPlaceholderColor());
    textFieldDeclaration_->SetBlockRightShade(searchTheme->GetBlockRightShade());

    std::function<void(const std::string&)> submitEvent;
    SetSubmitEvent(submitEvent);
    SetTextEditController(textFieldDeclaration_->GetTextEditController());
    SetCloseIconSize(searchTheme->GetCloseIconSize());
    SetCloseIconHotZoneHorizontal(searchTheme->GetCloseIconHotZoneSize());
    SetHoverColor(textFieldTheme->GetHoverColor());
    SetPressColor(textFieldTheme->GetPressColor());
    isPaddingChanged_ = true;
}

bool SearchDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SearchDeclaration&, TextFieldDeclaration&)>
        searchAttrOperators[] = {
            { DOM_AUTO_FOCUS,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    textFieldDeclaration.SetAutoFocus(StringToBool(val));
                } },
            { DOM_SEARCH_HINT,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) { textFieldDeclaration.SetPlaceholder(val); } },
            { DOM_SEARCH_ICON,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    if (SystemProperties::GetDeviceType() == DeviceType::TV) {
                        return;
                    }
                    textFieldDeclaration.SetIconImage(val);
                } },
            { DOM_SEARCH_BUTTON,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) { searchDeclaration.SetSearchText(val); } },
            { DOM_INPUT_SELECTED_END,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    textFieldDeclaration.SetSelectedEnd(StringToInt(val));
                } },
            { DOM_INPUT_SELECTED_START,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    textFieldDeclaration.SetSelectedStart(StringToInt(val));
                } },
            { DOM_INPUT_SOFT_KEYBOARD_ENABLED,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    textFieldDeclaration.SetSoftKeyboardEnabled(StringToBool(val));
                } },
            { DOM_SEARCH_VALUE,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    textFieldDeclaration.SetValue(val);
                    textFieldDeclaration.SetResetToStart(false);
                } },
        };
    auto operatorIter = BinarySearchFindIndex(searchAttrOperators, ArraySize(searchAttrOperators), attr.first.c_str());
    if (operatorIter != -1) {
        searchAttrOperators[operatorIter].value(attr.second, *this, *textFieldDeclaration_);
        return true;
    }
    return false;
}

bool SearchDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    // static linear map must be sorted by key.
    static const LinearMapNode<void (*)(const std::string&, SearchDeclaration&, TextFieldDeclaration&)>
        searchStyleSize[] = {
            { DOM_TEXT_ALLOW_SCALE,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    textFieldDeclaration.GetTextStyle().SetAllowScale(StringToBool(val));
                } },
            { DOM_BACKGROUND_COLOR,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    textFieldDeclaration.SetBgColor(searchDeclaration.ParseColor(val));
                    textFieldDeclaration.SetFocusBgColor(searchDeclaration.ParseColor(val));
                } },
            { DOM_CARET_COLOR,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    textFieldDeclaration.SetCursorColor(searchDeclaration.ParseColor(val));
                } },
            { DOM_COLOR,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    textFieldDeclaration.SetFocusTextColor(searchDeclaration.ParseColor(val));
                } },
            { DOM_TEXT_FONT_FAMILY,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    textFieldDeclaration.GetTextStyle().SetFontFamilies(searchDeclaration.ParseFontFamilies(val));
                } },
            { DOM_TEXT_FONT_SIZE,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    textFieldDeclaration.GetTextStyle().SetFontSize(searchDeclaration.ParseDimension(val));
                } },
            { DOM_TEXT_FONT_WEIGHT,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    textFieldDeclaration.GetTextStyle().SetFontWeight(ConvertStrToFontWeight(val));
                } },
            { DOM_PADDING,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    Edge padding;
                    if (Edge::FromString(val, padding)) {
                        textFieldDeclaration.GetDecoration()->SetPadding(padding);
                        searchDeclaration.isPaddingChanged_ = true;
                    }
                } },
            { DOM_PADDING_BOTTOM,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    auto padding = textFieldDeclaration.GetDecoration()->GetPadding();
                    padding.SetBottom(searchDeclaration.ParseDimension(val));
                    textFieldDeclaration.GetDecoration()->SetPadding(padding);
                    searchDeclaration.isPaddingChanged_ = true;
                } },
            { DOM_PADDING_END,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    auto padding = textFieldDeclaration.GetDecoration()->GetPadding();
                    searchDeclaration.IsRightToLeft() ? padding.SetLeft(searchDeclaration.ParseDimension(val))
                                                      : padding.SetRight(searchDeclaration.ParseDimension(val));
                    textFieldDeclaration.GetDecoration()->SetPadding(padding);
                    searchDeclaration.isPaddingChanged_ = true;
                } },
            { DOM_PADDING_LEFT,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    auto padding = textFieldDeclaration.GetDecoration()->GetPadding();
                    padding.SetLeft(searchDeclaration.ParseDimension(val));
                    textFieldDeclaration.GetDecoration()->SetPadding(padding);
                    searchDeclaration.isPaddingChanged_ = true;
                } },
            { DOM_PADDING_RIGHT,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    auto padding = textFieldDeclaration.GetDecoration()->GetPadding();
                    padding.SetRight(searchDeclaration.ParseDimension(val));
                    textFieldDeclaration.GetDecoration()->SetPadding(padding);
                    searchDeclaration.isPaddingChanged_ = true;
                } },
            { DOM_PADDING_START,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    auto padding = textFieldDeclaration.GetDecoration()->GetPadding();
                    searchDeclaration.IsRightToLeft() ? padding.SetRight(searchDeclaration.ParseDimension(val))
                                                      : padding.SetLeft(searchDeclaration.ParseDimension(val));
                    textFieldDeclaration.GetDecoration()->SetPadding(padding);
                    searchDeclaration.isPaddingChanged_ = true;
                } },
            { DOM_PADDING_TOP,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    auto padding = textFieldDeclaration.GetDecoration()->GetPadding();
                    padding.SetTop(searchDeclaration.ParseDimension(val));
                    textFieldDeclaration.GetDecoration()->SetPadding(padding);
                    searchDeclaration.isPaddingChanged_ = true;
                } },
            { DOM_INPUT_PLACEHOLDER_COLOR,
                [](const std::string& val, SearchDeclaration& searchDeclaration,
                    TextFieldDeclaration& textFieldDeclaration) {
                    textFieldDeclaration.SetPlaceholderColor(searchDeclaration.ParseColor(val));
                } },
        };
    auto operatorIter = BinarySearchFindIndex(searchStyleSize, ArraySize(searchStyleSize), style.first.c_str());
    if (operatorIter != -1) {
        searchStyleSize[operatorIter].value(style.second, *this, *textFieldDeclaration_);
        return true;
    }
    if (style.first == DOM_BORDER_TOP_LEFT_RADIUS || style.first == DOM_BORDER_TOP_RIGHT_RADIUS ||
        style.first == DOM_BORDER_BOTTOM_LEFT_RADIUS || style.first == DOM_BORDER_BOTTOM_RIGHT_RADIUS ||
        style.first == DOM_BORDER_RADIUS) {
        hasBoxRadius_ = true;
    }
    return false;
}

bool SearchDeclaration::SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event)
{
    static const LinearMapNode<void (*)(SearchDeclaration&, TextFieldDeclaration&, const EventMarker&)>
        eventOperators[] = {
            { DOM_CHANGE, [](SearchDeclaration& searchDeclaration, TextFieldDeclaration& textFieldDeclaration,
                              const EventMarker& event) { searchDeclaration.SetChangeEventId(event); } },
            { DOM_INPUT_EVENT_OPTION_SELECT,
                [](SearchDeclaration& searchDeclaration, TextFieldDeclaration& textFieldDeclaration,
                    const EventMarker& event) { textFieldDeclaration.SetOnOptionsClick(event); } },
            { DOM_INPUT_EVENT_SEARCH,
                [](SearchDeclaration& searchDeclaration, TextFieldDeclaration& textFieldDeclaration,
                    const EventMarker& event) { textFieldDeclaration.SetOnSearch(event); } },
            { DOM_INPUT_EVENT_SELECT_CHANGE,
                [](SearchDeclaration& searchDeclaration, TextFieldDeclaration& textFieldDeclaration,
                    const EventMarker& event) { textFieldDeclaration.SetOnSelectChange(event); } },
            { DOM_INPUT_EVENT_SHARE,
                [](SearchDeclaration& searchDeclaration, TextFieldDeclaration& textFieldDeclaration,
                    const EventMarker& event) { textFieldDeclaration.SetOnShare(event); } },
            { DOM_SUBMIT, [](SearchDeclaration& searchDeclaration, TextFieldDeclaration& textFieldDeclaration,
                              const EventMarker& event) { searchDeclaration.SetSubmitEventId(event); } },
            { DOM_INPUT_EVENT_TRANSLATE,
                [](SearchDeclaration& searchDeclaration, TextFieldDeclaration& textFieldDeclaration,
                    const EventMarker& event) { textFieldDeclaration.SetOnTranslate(event); } },
        };
    auto operatorIter = BinarySearchFindIndex(eventOperators, ArraySize(eventOperators), event.c_str());
    if (operatorIter != -1) {
        eventOperators[operatorIter].value(*this, *textFieldDeclaration_, EventMarker(eventId, event, pageId));
        return true;
    }
    return false;
}

void SearchDeclaration::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    textFieldDeclaration_->CallSpecializedMethod(method, args);
}

void SearchDeclaration::OnRequestFocus(bool shouldFocus)
{
    textFieldDeclaration_->OnRequestFocus(shouldFocus);
}

} // namespace OHOS::Ace
