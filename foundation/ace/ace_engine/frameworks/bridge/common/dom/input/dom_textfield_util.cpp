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

#include "frameworks/bridge/common/dom/input/dom_textfield_util.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

const TextInputAction INPUT_TEXTINPUTACTION_VALUE_DEFAULT = TextInputAction::UNSPECIFIED;
const std::vector<std::string> INPUT_FONT_FAMILY_VALUE = {
    "sans-serif",
};

constexpr Dimension BOX_HOVER_RADIUS = 18.0_vp;

} // namespace

Radius DOMTextFieldUtil::defaultRadius_;

void DOMTextFieldUtil::InitDefaultValue(const RefPtr<BoxComponent>& boxComponent,
    const RefPtr<TextFieldComponent>& component, const RefPtr<TextFieldTheme>& theme)
{
    if (!boxComponent || !component || !theme) {
        return;
    }

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
    defaultRadius_ = theme->GetBorderRadius();
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

void DOMTextFieldUtil::SetDisableStyle(const RefPtr<TextFieldComponent>& component, const RefPtr<TextFieldTheme>& theme)
{
    if (!component || !theme) {
        return;
    }

    TextStyle textStyle = component->GetTextStyle();
    textStyle.SetTextColor(theme->GetDisableTextColor());
    component->SetTextStyle(textStyle);
    component->SetPlaceholderColor(theme->GetDisableTextColor());
}

void DOMTextFieldUtil::InitController(const RefPtr<TextFieldComponent>& component)
{
    if (component) {
        component->SetTextEditController(AceType::MakeRefPtr<TextEditController>());
        component->SetTextFieldController(AceType::MakeRefPtr<TextFieldController>());
    }
}

RefPtr<TextFieldComponent> DOMTextFieldUtil::CreateComponentAndSetChildAttr(const RefPtr<BoxComponent>& boxComponent,
    const std::string& type, const std::map<std::string, std::string>& attrs, const DOMInput& node)
{
    // create component
    RefPtr<TextFieldComponent> component = AceType::MakeRefPtr<TextFieldComponent>();
    // set default value
    RefPtr<TextFieldTheme> theme = node.GetTheme<TextFieldTheme>();
    InitController(component);
    InitDefaultValue(boxComponent, component, theme);

    SetChildAttr(component, boxComponent, type, attrs);

    if (!component->IsEnabled()) {
        SetDisableStyle(component, theme);
    }

    return component;
}

void DOMTextFieldUtil::SetChildAttr(const RefPtr<TextFieldComponent>& component,
    const RefPtr<BoxComponent>& boxComponent, const std::string& type, const std::map<std::string, std::string>& attrs)
{
    if (!component) {
        return;
    }
    component->SetTextInputType(ConvertStrToTextInputType(type));
    component->SetObscure(type == DOM_INPUT_TYPE_PASSWORD);
    // this static map should be sorted by key.
    static const LinearMapNode<void (*)(const RefPtr<TextFieldComponent>&, const std::string&)> attrOperators[] = {
        { DOM_AUTO_FOCUS, [](const RefPtr<TextFieldComponent>& component,
                              const std::string& value) { component->SetAutoFocus(StringToBool(value)); } },
        { DOM_DISABLED, [](const RefPtr<TextFieldComponent>& component,
                        const std::string& value) { component->SetEnabled(!StringToBool(value)); } },
        { DOM_INPUT_ENTERKEYTYPE,
            [](const RefPtr<TextFieldComponent>& component, const std::string& value) {
                component->SetAction(ConvertStrToTextInputAction(value));
            } },
        { DOM_ICON_SRC, [](const RefPtr<TextFieldComponent>& component,
                        const std::string& value) { component->SetIconImage(value); } },
        { DOM_HIDE_ICON_SRC, [](const RefPtr<TextFieldComponent>& component,
                             const std::string& value) { component->SetHideIconImage(value); } },
        { DOM_INPUT_MAXLENGTH,
            [](const RefPtr<TextFieldComponent>& component, const std::string& value) {
                int32_t tmp = StringUtils::StringToInt(value);
                component->SetMaxLength(tmp >= 0 ? (uint32_t)(tmp) : std::numeric_limits<uint32_t>::max());
            } },
        { DOM_INPUT_OBSCURE, [](const RefPtr<TextFieldComponent>& component,
                             const std::string& value) { component->SetObscure(StringToBool(value)); } },
        { DOM_INPUT_PLACEHOLDER, [](const RefPtr<TextFieldComponent>& component,
                                 const std::string& value) { component->SetPlaceholder(value); } },
        { DOM_INPUT_SELECTED_END, [](const RefPtr<TextFieldComponent>& component,
                                      const std::string& value) { component->SetSelectedEnd(StringToInt(value)); } },
        { DOM_INPUT_SELECTED_START,
            [](const RefPtr<TextFieldComponent>& component, const std::string& value) {
                component->SetSelectedStart(StringToInt(value));
            } },
        { DOM_INPUT_SHOW_COUNTER, [](const RefPtr<TextFieldComponent>& component,
                                  const std::string& value) { component->SetShowCounter(StringToBool(value)); } },
        { DOM_SHOW_ICON_SRC, [](const RefPtr<TextFieldComponent>& component,
                             const std::string& value) { component->SetShowIconImage(value); } },
        { DOM_INPUT_SHOW_PASSWORD_ICON,
            [](const RefPtr<TextFieldComponent>& component, const std::string& value) {
                component->SetShowPasswordIcon(StringToBool(value));
            } },
        { DOM_INPUT_SOFT_KEYBOARD_ENABLED,
            [](const RefPtr<TextFieldComponent>& component, const std::string& value) {
                component->SetSoftKeyboardEnabled(StringToBool(value));
            } },
        { DOM_INPUT_VALUE,
            [](const RefPtr<TextFieldComponent>& component, const std::string& value) { component->SetValue(value); } },
    };
    for (const auto& [key, value] : attrs) {
        auto operatorIter = BinarySearchFindIndex(attrOperators, ArraySize(attrOperators), key.c_str());
        if (operatorIter != -1) {
            attrOperators[operatorIter].value(component, value);
        }
    }

    if (boxComponent) {
        boxComponent->SetDeliverMinToChild(true);
        if (GreatOrEqual(boxComponent->GetHeightDimension().Value(), 0.0)) {
            component->SetHeight(boxComponent->GetHeightDimension());
        }
        boxComponent->SetHeight(-1.0, DimensionUnit::PX);
    }
}

void DOMTextFieldUtil::SetChildStyle(const RefPtr<BoxComponent>& boxComponent,
    const RefPtr<TextFieldComponent>& component, const std::map<std::string, std::string>& styles,
    const Border& boxBorder, const DOMInput& node)
{
    if (!component) {
        return;
    }
    TextStyle textStyle = component->GetTextStyle();
    // static linear map must be sorted by key.
    static const LinearMapNode<void (*)(
        const RefPtr<TextFieldComponent>&, TextStyle&, const std::string&, const DOMInput&)>
        styleOperators[] = {
            { DOM_TEXT_ALLOW_SCALE,
                [](const RefPtr<TextFieldComponent>& component, TextStyle& style, const std::string& value,
                    const DOMInput& node) { style.SetAllowScale(StringToBool(value)); } },
            { DOM_BACKGROUND_COLOR,
                [](const RefPtr<TextFieldComponent>& component, TextStyle& style, const std::string& value,
                    const DOMInput& node) {
                    component->SetBgColor(node.ParseColor(value));
                    component->SetFocusBgColor(node.ParseColor(value));
                } },
            { DOM_CARET_COLOR,
                [](const RefPtr<TextFieldComponent>& component, TextStyle& style, const std::string& value,
                    const DOMInput& node) { component->SetCursorColor(node.ParseColor(value)); } },
            { DOM_INPUT_COLOR,
                [](const RefPtr<TextFieldComponent>& component, TextStyle& style, const std::string& value,
                    const DOMInput& node) {
                    style.SetTextColor(node.ParseColor(value));
                    component->SetFocusTextColor(node.ParseColor(value));
                } },
            { DOM_INPUT_CURSOR_COLOR,
                [](const RefPtr<TextFieldComponent>& component, TextStyle& style, const std::string& value,
                    const DOMInput& node) { component->SetCursorColor(node.ParseColor(value)); } },
            { DOM_INPUT_FONT_FAMILY,
                [](const RefPtr<TextFieldComponent>& component, TextStyle& style, const std::string& value,
                    const DOMInput& node) {
                    std::vector<std::string> fontFamilies;
                    std::stringstream sstr(value);
                    std::string fontFamily;
                    while (getline(sstr, fontFamily, ',')) {
                        fontFamilies.emplace_back(fontFamily);
                    }
                    style.SetFontFamilies(fontFamilies);
                } },
            { DOM_INPUT_FONT_SIZE,
                [](const RefPtr<TextFieldComponent>& component, TextStyle& style, const std::string& value,
                    const DOMInput& node) { style.SetFontSize(node.ParseDimension(value)); } },
            { DOM_INPUT_FONT_WEIGHT,
                [](const RefPtr<TextFieldComponent>& component, TextStyle& style, const std::string& value,
                    const DOMInput& node) { style.SetFontWeight(ConvertStrToFontWeight(value)); } },
            { DOM_PADDING,
                [](const RefPtr<TextFieldComponent>& component, TextStyle& style, const std::string& value,
                    const DOMInput& node) {
                    Edge padding;
                    if (Edge::FromString(value, padding)) {
                        component->GetDecoration()->SetPadding(padding);
                    }
                } },
            { DOM_PADDING_BOTTOM,
                [](const RefPtr<TextFieldComponent>& component, TextStyle& style, const std::string& value,
                    const DOMInput& node) {
                    auto padding = component->GetDecoration()->GetPadding();
                    padding.SetBottom(node.ParseDimension(value));
                    component->GetDecoration()->SetPadding(padding);
                } },
            { DOM_PADDING_END,
                [](const RefPtr<TextFieldComponent>& component, TextStyle& style, const std::string& value,
                    const DOMInput& node) {
                    auto padding = component->GetDecoration()->GetPadding();
                    component->GetTextDirection() == TextDirection::LTR ? padding.SetRight(node.ParseDimension(value))
                                                                        : padding.SetLeft(node.ParseDimension(value));
                    component->GetDecoration()->SetPadding(padding);
                } },
            { DOM_PADDING_LEFT,
                [](const RefPtr<TextFieldComponent>& component, TextStyle& style, const std::string& value,
                    const DOMInput& node) {
                    auto padding = component->GetDecoration()->GetPadding();
                    padding.SetLeft(node.ParseDimension(value));
                    component->GetDecoration()->SetPadding(padding);
                } },
            { DOM_PADDING_RIGHT,
                [](const RefPtr<TextFieldComponent>& component, TextStyle& style, const std::string& value,
                    const DOMInput& node) {
                    auto padding = component->GetDecoration()->GetPadding();
                    padding.SetRight(node.ParseDimension(value));
                    component->GetDecoration()->SetPadding(padding);
                } },
            { DOM_PADDING_START,
                [](const RefPtr<TextFieldComponent>& component, TextStyle& style, const std::string& value,
                    const DOMInput& node) {
                    auto padding = component->GetDecoration()->GetPadding();
                    component->GetTextDirection() == TextDirection::LTR ? padding.SetLeft(node.ParseDimension(value))
                                                                        : padding.SetRight(node.ParseDimension(value));
                    component->GetDecoration()->SetPadding(padding);
                } },
            { DOM_PADDING_TOP,
                [](const RefPtr<TextFieldComponent>& component, TextStyle& style, const std::string& value,
                    const DOMInput& node) {
                    auto padding = component->GetDecoration()->GetPadding();
                    padding.SetTop(node.ParseDimension(value));
                    component->GetDecoration()->SetPadding(padding);
                } },
            { DOM_INPUT_PLACEHOLDER_COLOR,
                [](const RefPtr<TextFieldComponent>& component, TextStyle& style, const std::string& value,
                    const DOMInput& node) {
                    component->SetPlaceholderColor(node.ParseColor(value));
                    component->SetFocusPlaceholderColor(node.ParseColor(value));
                } },
        };
    bool hasBoxRadius = false;
    for (const auto& [key, value] : styles) {
        auto operatorIter = BinarySearchFindIndex(styleOperators, ArraySize(styleOperators), key.c_str());
        if (operatorIter != -1) {
            styleOperators[operatorIter].value(component, textStyle, value, node);
        }
        if (IsRadiusStyle(key)) {
            hasBoxRadius = true;
        }
    }
    component->SetTextStyle(textStyle);
    UpdateDecorationStyle(boxComponent, component, boxBorder, hasBoxRadius);
}

void DOMTextFieldUtil::UpdateDecorationStyle(const RefPtr<BoxComponent>& boxComponent,
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
        border.SetBorderRadius(defaultRadius_);
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
            border.SetBorderRadius(defaultRadius_);
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

void DOMTextFieldUtil::AddChildEvent(const RefPtr<TextFieldComponent>& component, int32_t pageId,
    const std::string& nodeId, const std::vector<std::string>& events)
{
    static const LinearMapNode<void (*)(const RefPtr<TextFieldComponent>&, const EventMarker&)> eventOperators[] = {
        { DOM_CATCH_BUBBLE_CLICK,
            [](const RefPtr<TextFieldComponent>& component, const EventMarker& event) {
                EventMarker eventMarker(event);
                eventMarker.SetCatchMode(true);
                component->SetOnTap(eventMarker);
            } },
        { DOM_CHANGE, [](const RefPtr<TextFieldComponent>& component,
                      const EventMarker& event) { component->SetOnTextChange(event); } },
        { DOM_CLICK,
            [](const RefPtr<TextFieldComponent>& component, const EventMarker& event) {
                EventMarker eventMarker(event);
                eventMarker.SetCatchMode(false);
                component->SetOnTap(eventMarker);
            } },
        { DOM_INPUT_EVENT_ENTERKEYCLICK, [](const RefPtr<TextFieldComponent>& component,
                                         const EventMarker& event) { component->SetOnFinishInput(event); } },
        { DOM_LONG_PRESS, [](const RefPtr<TextFieldComponent>& component,
                          const EventMarker& event) { component->SetOnLongPress(event); } },
        { DOM_INPUT_EVENT_OPTION_SELECT, [](const RefPtr<TextFieldComponent>& component,
                                         const EventMarker& event) { component->SetOnOptionsClick(event); } },
        { DOM_INPUT_EVENT_SEARCH, [](const RefPtr<TextFieldComponent>& component,
                                        const EventMarker& event) { component->SetOnSearch(event); } },
        { DOM_INPUT_EVENT_SELECT_CHANGE, [](const RefPtr<TextFieldComponent>& component,
                                             const EventMarker& event) { component->SetOnSelectChange(event); } },
        { DOM_INPUT_EVENT_SHARE, [](const RefPtr<TextFieldComponent>& component,
                                 const EventMarker& event) { component->SetOnShare(event); } },
        { DOM_INPUT_EVENT_TRANSLATE, [](const RefPtr<TextFieldComponent>& component,
                                const EventMarker& event) { component->SetOnTranslate(event); } },
    };
    for (const auto& event : events) {
        auto operatorIter = BinarySearchFindIndex(eventOperators, ArraySize(eventOperators), event.c_str());
        if (operatorIter != -1) {
            eventOperators[operatorIter].value(component, EventMarker(nodeId, event, pageId));
        }
    }
}

bool DOMTextFieldUtil::IsRadiusStyle(const std::string& style)
{
    return (style == DOM_BORDER_TOP_LEFT_RADIUS || style == DOM_BORDER_TOP_RIGHT_RADIUS ||
            style == DOM_BORDER_BOTTOM_LEFT_RADIUS || style == DOM_BORDER_BOTTOM_RIGHT_RADIUS ||
            style == DOM_BORDER_RADIUS);
}

} // namespace OHOS::Ace::Framework
