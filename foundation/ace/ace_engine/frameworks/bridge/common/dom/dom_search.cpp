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

#include "frameworks/bridge/common/dom/dom_search.h"

#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "core/common/ime/text_selection.h"
#include "core/components/common/properties/radius.h"
#include "core/components/search/search_theme.h"
#include "core/components/text_field/textfield_theme.h"
#include "frameworks/bridge/common/dom/input/dom_textfield_util.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

struct StyleParseHolder {
    const DOMSearch& node;
    RefPtr<SearchComponent>& search;
    RefPtr<TextFieldComponent>& textField;
    TextStyle& textStyle;
    bool& isPaddingChanged;
};

} // namespace

DOMSearch::DOMSearch(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    searchChild_ = AceType::MakeRefPtr<SearchComponent>();
    textFieldComponent_ = AceType::MakeRefPtr<TextFieldComponent>();
    DOMTextFieldUtil::InitController(textFieldComponent_);
}

void DOMSearch::ResetInitializedStyle()
{
    InitializeStyle();
}

void DOMSearch::InitializeStyle()
{
    auto textFieldTheme = GetTheme<TextFieldTheme>();
    auto searchTheme = GetTheme<SearchTheme>();
    if (!textFieldTheme || !searchTheme) {
        // theme is null, set default decoration to text field component
        RefPtr<Decoration> decoration = AceType::MakeRefPtr<Decoration>();
        textFieldComponent_->SetDecoration(decoration);
        LOGE("textFieldTheme or searchTheme is null");
        return;
    }

    DOMTextFieldUtil::InitDefaultValue(boxComponent_, textFieldComponent_, textFieldTheme);
    boxComponent_->SetBackDecoration(nullptr);
    boxComponent_->SetPadding(Edge());
    textFieldComponent_->SetIconSize(searchTheme->GetIconSize());
    textFieldComponent_->SetIconHotZoneSize(searchTheme->GetCloseIconHotZoneSize());
    Edge decorationPadding;
    Dimension leftPadding = searchTheme->GetLeftPadding();
    Dimension rightPadding = searchTheme->GetRightPadding();
    if (IsRightToLeft()) {
        decorationPadding = Edge(rightPadding.Value(), 0.0, leftPadding.Value(), 0.0, leftPadding.Unit());
    } else {
        decorationPadding = Edge(leftPadding.Value(), 0.0, rightPadding.Value(), 0.0, leftPadding.Unit());
    }
    auto textFieldDecoration = textFieldComponent_->GetDecoration();
    if (textFieldDecoration) {
        textFieldDecoration->SetPadding(decorationPadding);
        textFieldDecoration->SetBorderRadius(searchTheme->GetBorderRadius());
        textFieldComponent_->SetOriginBorder(textFieldDecoration->GetBorder());
    }
    textFieldComponent_->SetAction(TextInputAction::SEARCH);
    textFieldComponent_->SetWidthReserved(searchTheme->GetTextFieldWidthReserved());
    textFieldComponent_->SetTextColor(searchTheme->GetTextColor());
    textFieldComponent_->SetFocusTextColor(searchTheme->GetFocusTextColor());
    textFieldComponent_->SetPlaceholderColor(searchTheme->GetPlaceholderColor());
    textFieldComponent_->SetFocusPlaceholderColor(searchTheme->GetFocusPlaceholderColor());
    textFieldComponent_->SetBlockRightShade(searchTheme->GetBlockRightShade());
    textStyle_ = textFieldComponent_->GetTextStyle();

    std::function<void(const std::string&)> submitEvent;
    searchChild_->SetSubmitEvent(submitEvent);
    searchChild_->SetChild(textFieldComponent_);
    searchChild_->SetTextEditController(textFieldComponent_->GetTextEditController());
    searchChild_->SetCloseIconSize(searchTheme->GetCloseIconSize());
    searchChild_->SetCloseIconHotZoneHorizontal(searchTheme->GetCloseIconHotZoneSize());
    searchChild_->SetHoverColor(textFieldTheme->GetHoverColor());
    searchChild_->SetPressColor(textFieldTheme->GetPressColor());
    SetHeight(searchTheme->GetHeight());
    isPaddingChanged_ = true;
}

bool DOMSearch::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SearchComponent&, TextFieldComponent&, TextStyle&)>
        searchAttrOperators[] = {
            { DOM_AUTO_FOCUS,
                [](const std::string& val, SearchComponent& searchComponent, TextFieldComponent& textFieldComponent,
                    TextStyle& textStyle) { textFieldComponent.SetAutoFocus(StringToBool(val)); } },
            { DOM_SEARCH_HINT,
                [](const std::string& val, SearchComponent& searchComponent, TextFieldComponent& textFieldComponent,
                    TextStyle& textStyle) { textFieldComponent.SetPlaceholder(val); } },
            { DOM_SEARCH_ICON,
                [](const std::string& val, SearchComponent& searchComponent, TextFieldComponent& textFieldComponent,
                    TextStyle& textStyle) {
                    if (SystemProperties::GetDeviceType() == DeviceType::TV) {
                        return;
                    }
                    textFieldComponent.SetIconImage(val);
                } },
            { DOM_SEARCH_BUTTON,
                [](const std::string& val, SearchComponent& searchComponent, TextFieldComponent& textFieldComponent,
                    TextStyle& textStyle) { searchComponent.SetSearchText(val); } },
            { DOM_INPUT_SELECTED_END,
                [](const std::string& val, SearchComponent& searchComponent, TextFieldComponent& textFieldComponent,
                    TextStyle& textStyle) { textFieldComponent.SetSelectedEnd(StringToInt(val)); } },
            { DOM_INPUT_SELECTED_START,
                [](const std::string& val, SearchComponent& searchComponent, TextFieldComponent& textFieldComponent,
                    TextStyle& textStyle) { textFieldComponent.SetSelectedStart(StringToInt(val)); } },
            { DOM_INPUT_SOFT_KEYBOARD_ENABLED,
                [](const std::string& val, SearchComponent& searchComponent, TextFieldComponent& textFieldComponent,
                    TextStyle& textStyle) { textFieldComponent.SetSoftKeyboardEnabled(StringToBool(val)); } },
            { DOM_SEARCH_VALUE,
                [](const std::string& val, SearchComponent& searchComponent, TextFieldComponent& textFieldComponent,
                    TextStyle& textStyle) {
                    textFieldComponent.SetValue(val);
                    textFieldComponent.SetResetToStart(false);
                } },
        };
    auto operatorIter = BinarySearchFindIndex(searchAttrOperators, ArraySize(searchAttrOperators), attr.first.c_str());
    if (operatorIter != -1) {
        searchAttrOperators[operatorIter].value(attr.second, *searchChild_, *textFieldComponent_, textStyle_);
        return true;
    }
    return false;
}

bool DOMSearch::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    // static linear map must be sorted by key.
    static const LinearMapNode<void (*)(const std::string&, StyleParseHolder&)> searchStyleSize[] = {
        { DOM_TEXT_ALLOW_SCALE, [](const std::string& val,
                                    StyleParseHolder& holder) { holder.textStyle.SetAllowScale(StringToBool(val)); } },
        { DOM_BACKGROUND_COLOR,
            [](const std::string& val, StyleParseHolder& holder) {
                holder.textField->SetBgColor(holder.node.ParseColor(val));
                holder.textField->SetFocusBgColor(holder.node.ParseColor(val));
            } },
        { DOM_CARET_COLOR,
            [](const std::string& val, StyleParseHolder& holder) {
                holder.textField->SetCursorColor(holder.node.ParseColor(val));
            } },
        { DOM_COLOR,
            [](const std::string& val, StyleParseHolder& holder) {
                auto color = holder.node.ParseColor(val);
                holder.textStyle.SetTextColor(color);
                holder.textField->SetTextColor(color);
                holder.textField->SetFocusTextColor(color);
            } },
        { DOM_TEXT_FONT_FAMILY,
            [](const std::string& val, StyleParseHolder& holder) {
                holder.textStyle.SetFontFamilies(holder.node.ParseFontFamilies(val));
            } },
        { DOM_TEXT_FONT_SIZE,
            [](const std::string& val, StyleParseHolder& holder) {
                holder.textStyle.SetFontSize(holder.node.ParseDimension(val));
            } },
        { DOM_TEXT_FONT_WEIGHT,
            [](const std::string& val, StyleParseHolder& holder) {
                holder.textStyle.SetFontWeight(ConvertStrToFontWeight(val));
            } },
        { DOM_PADDING,
            [](const std::string& val, StyleParseHolder& holder) {
                Edge padding;
                if (Edge::FromString(val, padding)) {
                    holder.textField->GetDecoration()->SetPadding(padding);
                    holder.isPaddingChanged = true;
                }
            } },
        { DOM_PADDING_BOTTOM,
            [](const std::string& val, StyleParseHolder& holder) {
                auto padding = holder.textField->GetDecoration()->GetPadding();
                padding.SetBottom(holder.node.ParseDimension(val));
                holder.textField->GetDecoration()->SetPadding(padding);
                holder.isPaddingChanged = true;
            } },
        { DOM_PADDING_END,
            [](const std::string& val, StyleParseHolder& holder) {
                auto padding = holder.textField->GetDecoration()->GetPadding();
                (holder.search->GetTextDirection() == TextDirection::RTL)
                    ? padding.SetLeft(holder.node.ParseDimension(val))
                    : padding.SetRight(holder.node.ParseDimension(val));
                holder.textField->GetDecoration()->SetPadding(padding);
                holder.isPaddingChanged = true;
            } },
        { DOM_PADDING_LEFT,
            [](const std::string& val, StyleParseHolder& holder) {
                auto padding = holder.textField->GetDecoration()->GetPadding();
                padding.SetLeft(holder.node.ParseDimension(val));
                holder.textField->GetDecoration()->SetPadding(padding);
                holder.isPaddingChanged = true;
            } },
        { DOM_PADDING_RIGHT,
            [](const std::string& val, StyleParseHolder& holder) {
                auto padding = holder.textField->GetDecoration()->GetPadding();
                padding.SetRight(holder.node.ParseDimension(val));
                holder.textField->GetDecoration()->SetPadding(padding);
                holder.isPaddingChanged = true;
            } },
        { DOM_PADDING_START,
            [](const std::string& val, StyleParseHolder& holder) {
                auto padding = holder.textField->GetDecoration()->GetPadding();
                (holder.search->GetTextDirection() == TextDirection::RTL)
                    ? padding.SetRight(holder.node.ParseDimension(val))
                    : padding.SetLeft(holder.node.ParseDimension(val));
                holder.textField->GetDecoration()->SetPadding(padding);
                holder.isPaddingChanged = true;
            } },
        { DOM_PADDING_TOP,
            [](const std::string& val, StyleParseHolder& holder) {
                auto padding = holder.textField->GetDecoration()->GetPadding();
                padding.SetTop(holder.node.ParseDimension(val));
                holder.textField->GetDecoration()->SetPadding(padding);
                holder.isPaddingChanged = true;
            } },
        { DOM_INPUT_PLACEHOLDER_COLOR,
            [](const std::string& val, StyleParseHolder& holder) {
                auto color = holder.node.ParseColor(val);
                holder.textField->SetPlaceholderColor(color);
                holder.textField->SetFocusPlaceholderColor(color);
            } },
    };
    auto operatorIter = BinarySearchFindIndex(searchStyleSize, ArraySize(searchStyleSize), style.first.c_str());
    if (operatorIter != -1) {
        StyleParseHolder holder = { .node = *this,
            .search = searchChild_,
            .textField = textFieldComponent_,
            .textStyle = textStyle_,
            .isPaddingChanged = isPaddingChanged_ };
        searchStyleSize[operatorIter].value(style.second, holder);
        return true;
    }
    if (DOMTextFieldUtil::IsRadiusStyle(style.first)) {
        hasBoxRadius_ = true;
    }
    return false;
}

bool DOMSearch::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    static const LinearMapNode<void (*)(
        const RefPtr<SearchComponent>&, const RefPtr<TextFieldComponent>&, const EventMarker&)>
        eventOperators[] = {
            { DOM_CATCH_BUBBLE_CLICK,
                [](const RefPtr<SearchComponent>& search, const RefPtr<TextFieldComponent>& textField,
                    const EventMarker& event) {
                    EventMarker eventMarker(event);
                    eventMarker.SetCatchMode(true);
                    textField->SetOnTap(eventMarker);
                } },
            { DOM_CHANGE, [](const RefPtr<SearchComponent>& search, const RefPtr<TextFieldComponent>& textField,
                              const EventMarker& event) { search->SetChangeEventId(event); } },
            { DOM_CLICK,
                [](const RefPtr<SearchComponent>& search, const RefPtr<TextFieldComponent>& textField,
                    const EventMarker& event) {
                    EventMarker eventMarker(event);
                    eventMarker.SetCatchMode(false);
                    textField->SetOnTap(eventMarker);
                } },
            { DOM_LONG_PRESS, [](const RefPtr<SearchComponent>& search, const RefPtr<TextFieldComponent>& textField,
                                  const EventMarker& event) { textField->SetOnLongPress(event); } },
            { DOM_INPUT_EVENT_OPTION_SELECT,
                [](const RefPtr<SearchComponent>& search, const RefPtr<TextFieldComponent>& textField,
                    const EventMarker& event) { textField->SetOnOptionsClick(event); } },
            { DOM_INPUT_EVENT_SEARCH,
                [](const RefPtr<SearchComponent>& search, const RefPtr<TextFieldComponent>& textField,
                    const EventMarker& event) { textField->SetOnSearch(event); } },
            { DOM_INPUT_EVENT_SELECT_CHANGE,
                [](const RefPtr<SearchComponent>& search, const RefPtr<TextFieldComponent>& textField,
                    const EventMarker& event) { textField->SetOnSelectChange(event); } },
            { DOM_INPUT_EVENT_SHARE,
                [](const RefPtr<SearchComponent>& search, const RefPtr<TextFieldComponent>& textField,
                    const EventMarker& event) { textField->SetOnShare(event); } },
            { DOM_SUBMIT, [](const RefPtr<SearchComponent>& search, const RefPtr<TextFieldComponent>& textField,
                              const EventMarker& event) { search->SetSubmitEventId(event); } },
            { DOM_INPUT_EVENT_TRANSLATE,
                [](const RefPtr<SearchComponent>& search, const RefPtr<TextFieldComponent>& textField,
                    const EventMarker& event) { textField->SetOnTranslate(event); } },
        };
    auto operatorIter = BinarySearchFindIndex(eventOperators, ArraySize(eventOperators), event.c_str());
    if (operatorIter != -1) {
        eventOperators[operatorIter].value(
            searchChild_, textFieldComponent_, EventMarker(GetNodeIdForEvent(), event, pageId));
        return true;
    }
    return false;
}

void DOMSearch::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    auto textField = AceType::DynamicCast<TextFieldComponent>(textFieldComponent_);
    if (!textField) {
        return;
    }
    auto controller = textField->GetTextFieldController();
    if (!controller) {
        return;
    }
    if (method == DOM_INPUT_METHOD_DELETE) {
        controller->Delete();
    }
}

void DOMSearch::PrepareSpecializedComponent()
{
    Border boxBorder;
    // [boxComponent_] is created when [DomNode] is constructed so it won't be null
    boxComponent_->SetMouseAnimationType(HoverAnimationType::OPACITY);
    if (boxComponent_->GetBackDecoration()) {
        boxBorder = boxComponent_->GetBackDecoration()->GetBorder();
    }
    searchChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    textFieldComponent_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    // [textFieldComponent_] is created when [DomSearch] is constructed so it won't be null
    textFieldComponent_->SetTextStyle(textStyle_);
    textFieldComponent_->SetInputOptions(inputOptions_);
    textFieldComponent_->SetImageFill(GetImageFill());
    DOMTextFieldUtil::UpdateDecorationStyle(boxComponent_, textFieldComponent_, boxBorder, hasBoxRadius_);
    if (GreatOrEqual(boxComponent_->GetHeightDimension().Value(), 0.0)) {
        textFieldComponent_->SetHeight(boxComponent_->GetHeightDimension());
    }
    if (isPaddingChanged_) {
        auto padding = textFieldComponent_->GetDecoration()->GetPadding();
        if (searchChild_->GetTextDirection() == TextDirection::RTL) {
            padding.SetLeft(padding.Left() + searchChild_->GetCloseIconHotZoneHorizontal());
        } else {
            padding.SetRight(padding.Right() + searchChild_->GetCloseIconHotZoneHorizontal());
        }
        textFieldComponent_->GetDecoration()->SetPadding(padding);
        searchChild_->SetDecoration(textFieldComponent_->GetDecoration());
        isPaddingChanged_ = false;
    }
}

void DOMSearch::OnRequestFocus(bool shouldFocus)
{
    if (IsNodeDisabled() || !textFieldComponent_) {
        return;
    }
    auto textFieldController = textFieldComponent_->GetTextFieldController();
    if (!textFieldController) {
        return;
    }
    textFieldController->Focus(shouldFocus);
}

} // namespace OHOS::Ace::Framework
