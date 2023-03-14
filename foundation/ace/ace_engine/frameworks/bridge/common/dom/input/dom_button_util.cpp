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

#include "frameworks/bridge/common/dom/input/dom_button_util.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr uint32_t WATCH_BACKGROUND_COLOR = 0xff007dff;
constexpr uint32_t WATCH_TEXT_COLOR = 0xffffffff;
constexpr Dimension BOX_HOVER_RADIUS = 8.0_vp;

} // namespace

void DOMButtonUtil::InitDefaultValue(
    const RefPtr<ButtonComponent>& component, const RefPtr<TextComponent>& textChild, const RefPtr<ButtonTheme>& theme)
{
    // set default properties
    component->SetLayoutFlag(LAYOUT_FLAG_EXTEND_TO_PARENT);
    component->SetBackgroundColor(theme->GetBgColor());
    component->SetFocusColor(theme->GetBgFocusColor());
    component->SetFocusAnimationColor(theme->GetBgFocusColor());
    component->SetHoverColor(theme->GetHoverColor());
    textChild->SetFocusColor(theme->GetTextFocusColor());
    // set text styles
    auto textStyle = theme->GetTextStyle();
    textStyle.SetAdaptTextSize(textStyle.GetFontSize(), theme->GetMinFontSize());
    textStyle.SetTextAlign(TextAlign::CENTER);
    textStyle.SetMaxLines(theme->GetTextMaxLines());
    textStyle.SetTextOverflow(TextOverflow::ELLIPSIS);
    // set default background color and text color in watch
    if (SystemProperties::GetDeviceType() == DeviceType::WATCH) {
        component->SetBackgroundColor(Color(WATCH_BACKGROUND_COLOR));
        textStyle.SetTextColor(Color(WATCH_TEXT_COLOR));
        textChild->SetFocusColor(Color(WATCH_TEXT_COLOR));
    }
    textChild->SetTextStyle(textStyle);
}

RefPtr<ButtonComponent> DOMButtonUtil::CreateComponentAndSetChildAttr(
    const std::map<std::string, std::string>& attrs, DOMInput& node)
{
    std::list<RefPtr<Component>> buttonChildren;
    RefPtr<ButtonComponent> component = AceType::MakeRefPtr<ButtonComponent>(buttonChildren);
    RefPtr<ButtonTheme> theme = node.GetTheme<ButtonTheme>();
    if (!theme) {
        return component;
    }
    RefPtr<TextComponent> textChild = AceType::MakeRefPtr<TextComponent>("");
    RefPtr<PaddingComponent> padding = AceType::MakeRefPtr<PaddingComponent>();
    padding->SetPadding(theme->GetPadding());
    padding->SetChild(textChild);
    component->AppendChild(padding);
    InitDefaultValue(component, textChild, theme);

    auto boxComponent = node.GetBoxComponent();
    if (boxComponent) {
        if (!boxComponent->GetBackDecoration()) {
            RefPtr<Decoration> backDecoration = AceType::MakeRefPtr<Decoration>();
            backDecoration->SetBorderRadius(Radius(BOX_HOVER_RADIUS));
            boxComponent->SetBackDecoration(backDecoration);
        }
    } else {
        LOGE("boxComponent is null");
        return component;
    }
    if (LessOrEqual(node.GetHeight().Value(), 0.0)) {
        node.SetHeight(theme->GetHeight());
        component->SetHeight(theme->GetHeight());
        boxComponent->SetHeight(theme->GetHeight());
    } else {
        component->SetHeight(boxComponent->GetHeightDimension());
    }
    if (GreatNotEqual(boxComponent->GetWidthDimension().Value(), 0.0)) {
        padding->SetPadding(Edge());
        component->SetWidth(boxComponent->GetWidthDimension());
    }
    if (component->GetHeight().Unit() == DimensionUnit::PERCENT) {
        component->SetInputButton(true);
    } else {
        component->SetRectRadius(component->GetHeight() / 2);
    }
    SetChildAttr(component, attrs, theme);
    return component;
}

void DOMButtonUtil::SetChildAttr(const RefPtr<ButtonComponent>& component,
    const std::map<std::string, std::string>& attrs, const RefPtr<ButtonTheme>& theme)
{
    if (!component) {
        LOGE("fail to set child attr due to button component is null");
        return;
    }
    component->SetType(ButtonType::NORMAL);
    for (const auto& attr : attrs) {
        if (attr.first == DOM_DISABLED) {
            component->SetDisabledState(StringToBool(attr.second));
        } else if (attr.first == DOM_INPUT_AUTO_FOCUS) {
            component->SetAutoFocusState(StringToBool(attr.second));
        }
    }
    // set text data to Text child
    std::list<RefPtr<Component>> children = component->GetChildren();
    RefPtr<PaddingComponent> padding = AceType::DynamicCast<PaddingComponent>(children.front());
    if (!padding) {
        return;
    }
    RefPtr<TextComponent> textChild = AceType::DynamicCast<TextComponent>(padding->GetChild());
    auto inputValue = attrs.find(DOM_INPUT_VALUE);
    if (inputValue != attrs.end()) {
        textChild->SetData(inputValue->second);
    }
    if (component->GetDisabledState()) {
        auto textStyle = textChild->GetTextStyle();
        textStyle.SetTextColor(theme->GetTextDisabledColor());
        textChild->SetTextStyle(textStyle);
    }
}

void DOMButtonUtil::SetChildStyle(const RefPtr<BoxComponent>& boxComponent, const RefPtr<ButtonComponent>& component,
    const std::map<std::string, std::string>& styles, DOMInput& node)
{
    if (!boxComponent || !component) {
        LOGE("fail to set child style due to box and button component is null");
        return;
    }
    // get style which is set by theme
    std::list<RefPtr<Component>> children = component->GetChildren();
    RefPtr<PaddingComponent> padding = AceType::DynamicCast<PaddingComponent>(children.front());
    if (!padding) {
        return;
    }
    RefPtr<TextComponent> textChild = AceType::DynamicCast<TextComponent>(padding->GetChild());
    TextStyle parentStyle = textChild->GetTextStyle();
    static const LinearMapNode<void (*)(
        const std::string&, const DOMInput&, const RefPtr<ButtonComponent>&, TextStyle&)>
        styleOperators[] = {
            { DOM_INPUT_BACKGROUND_COLOR,
                [](const std::string& value, const DOMInput& node, const RefPtr<ButtonComponent>& component,
                    TextStyle& style) { component->SetBackgroundColor(node.ParseColor(value)); } },
            { DOM_INPUT_CLICKED_COLOR,
                [](const std::string& value, const DOMInput& node, const RefPtr<ButtonComponent>& component,
                    TextStyle& style) { component->SetClickedColor(node.ParseColor(value)); } },
            { DOM_INPUT_COLOR,
                [](const std::string& value, const DOMInput& node, const RefPtr<ButtonComponent>& component,
                    TextStyle& style) { style.SetTextColor(node.ParseColor(value)); } },
            { DOM_INPUT_DISABLE_COLOR,
                [](const std::string& value, const DOMInput& node, const RefPtr<ButtonComponent>& component,
                    TextStyle& style) { component->SetDisabledColor(node.ParseColor(value)); } },
            { DOM_INPUT_FOCUS_COLOR,
                [](const std::string& value, const DOMInput& node, const RefPtr<ButtonComponent>& component,
                    TextStyle& style) { component->SetFocusColor(node.ParseColor(value)); } },
            { DOM_INPUT_FONT_FAMILY,
                [](const std::string& value, const DOMInput& node, const RefPtr<ButtonComponent>& component,
                    TextStyle& style) {
                    std::vector<std::string> fontFamilies;
                    std::stringstream sstr(value);
                    std::string fontFamily;
                    while (getline(sstr, fontFamily, ',')) {
                        fontFamilies.emplace_back(fontFamily);
                    }
                    style.SetFontFamilies(fontFamilies);
                } },
            { DOM_INPUT_FONT_SIZE,
                [](const std::string& value, const DOMInput& node, const RefPtr<ButtonComponent>& component,
                    TextStyle& style) { style.SetFontSize(node.ParseDimension(value)); } },
            { DOM_INPUT_FONT_WEIGHT,
                [](const std::string& value, const DOMInput& node, const RefPtr<ButtonComponent>& component,
                    TextStyle& style) { style.SetFontWeight(ConvertStrToFontWeight(value)); } },
            { DOM_INPUT_RECT_RADIUS,
                [](const std::string& value, const DOMInput& node, const RefPtr<ButtonComponent>& component,
                    TextStyle& style) { component->SetRectRadius(node.ParseDimension(value)); } },
        };
    static const LinearMapNode<void (*)(const std::string&, const DOMInput&, const RefPtr<PaddingComponent>&)>
        paddingStyleOperators[] = {
            { DOM_PADDING_BOTTOM,
                [](const std::string& value, const DOMInput& node, const RefPtr<PaddingComponent>& component) {
                    component->SetPaddingBottom(node.ParseDimension(value));
                } },
            { DOM_PADDING_LEFT,
                [](const std::string& value, const DOMInput& node, const RefPtr<PaddingComponent>& component) {
                    component->SetPaddingLeft(node.ParseDimension(value));
                } },
            { DOM_PADDING_RIGHT,
                [](const std::string& value, const DOMInput& node, const RefPtr<PaddingComponent>& component) {
                    component->SetPaddingRight(node.ParseDimension(value));
                } },
            { DOM_PADDING_TOP,
                [](const std::string& value, const DOMInput& node, const RefPtr<PaddingComponent>& component) {
                    component->SetPaddingTop(node.ParseDimension(value));
                } },
        };
    // set text style properties
    for (const auto& [key, value] : styles) {
        auto operatorIter = BinarySearchFindIndex(styleOperators, ArraySize(styleOperators), key.c_str());
        if (operatorIter != -1) {
            styleOperators[operatorIter].value(value, node, component, parentStyle);
            continue;
        }
        auto paddingOperator =
            BinarySearchFindIndex(paddingStyleOperators, ArraySize(paddingStyleOperators), key.c_str());
        if (paddingOperator != -1) {
            paddingStyleOperators[paddingOperator].value(value, node, padding);
        }
    }
    auto theme = node.GetTheme<ButtonTheme>();
    if (theme) {
        component->SetDisabledColor(component->GetBackgroundColor().BlendOpacity(theme->GetBgDisabledAlpha()));
        component->SetClickedColor(component->GetBackgroundColor().BlendColor(theme->GetClickedColor()));
        if (parentStyle.GetFontSize() != theme->GetTextStyle().GetFontSize()) {
            parentStyle.SetAdaptTextSize(parentStyle.GetFontSize(), parentStyle.GetFontSize());
        }
    }
    // set text style to Text child
    if (SystemProperties::GetDeviceType() != DeviceType::TV) {
        textChild->SetFocusColor(parentStyle.GetTextColor());
    }
    textChild->SetTextStyle(parentStyle);

    auto backDecoration = boxComponent->GetBackDecoration();
    if (backDecoration) {
        const auto& border = backDecoration->GetBorder();
        if (node.HasBorderRadiusStyle()) {
            component->SetRectRadius(border.TopLeftRadius().GetX() - border.Top().GetWidth());
            component->SetRadiusState(true);
        } else {
            backDecoration->SetBorderRadius(Radius(component->GetRectRadius()));
        }
        if (node.CheckPseduo(backDecoration)) {
            component->SetType(ButtonType::ICON);
        }
    }
    boxComponent->SetPadding(Edge());
}

void DOMButtonUtil::AddChildEvent(const RefPtr<ButtonComponent>& component, int32_t pageId, const std::string& nodeId,
    const std::vector<std::string>& events)
{
    if (!component) {
        LOGE("fail to add child event due to button component is null");
        return;
    }
    static const LinearMapNode<void (*)(const RefPtr<ButtonComponent>&, EventMarker&)> eventOperators[] = {
        { DOM_CATCH_BUBBLE_CLICK,
            [](const RefPtr<ButtonComponent>& component, EventMarker& event) {
                event.SetCatchMode(true);
                component->SetClickedEventId(event);
            } },
        { DOM_CLICK,
            [](const RefPtr<ButtonComponent>& component, EventMarker& event) {
                event.SetCatchMode(false);
                component->SetClickedEventId(event);
            } },
    };
    for (const auto& event : events) {
        auto operatorIter = BinarySearchFindIndex(eventOperators, ArraySize(eventOperators), event.c_str());
        if (operatorIter != -1) {
            EventMarker eventMarker(nodeId, event, pageId);
            eventOperators[operatorIter].value(component, eventMarker);
        }
    }
}

} // namespace OHOS::Ace::Framework
