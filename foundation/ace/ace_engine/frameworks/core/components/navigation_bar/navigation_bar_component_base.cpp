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

#include "core/components/navigation_bar/navigation_bar_component_base.h"

#include "core/components/box/box_component.h"
#include "core/components/padding/padding_component.h"

namespace OHOS::Ace {

RefPtr<ButtonComponent> NavigationBarComponentBase::BuildIconButton(const RefPtr<NavigationBarTheme>& theme,
    const Dimension& width, const Dimension& height, const IconImage& icon, const EventMarker& clickMarker)
{
    std::list<RefPtr<Component>> buttonChildren;
    buttonChildren.emplace_back(icon.image);
    auto button = AceType::MakeRefPtr<ButtonComponent>(buttonChildren);
    button->SetType(ButtonType::NORMAL);
    button->SetWidth(width);
    button->SetHeight(height);
    button->SetBackgroundColor(theme->GetButtonNormalColor());
    button->SetClickedColor(theme->GetButtonPressedColor());
    button->SetFocusColor(theme->GetButtonFocusColor());
    button->SetHoverColor(theme->GetButtonHoverColor());
    button->SetClickedEventId(clickMarker);
    button->SetRectRadius(theme->GetButtonCornerRadius());
    return button;
}

RefPtr<TextComponent> NavigationBarComponentBase::BuildTitleText(
    const std::string& value, const Color& color, const Dimension& fontSize, const FontWeight& fontWeight)
{
    auto text = AceType::MakeRefPtr<TextComponent>(value);
    TextStyle textStyle;
    textStyle.SetTextColor(color);
    textStyle.SetFontSize(fontSize);
    textStyle.SetAllowScale(false);
    textStyle.SetFontWeight(fontWeight);
    textStyle.SetMaxLines(1);
    textStyle.SetTextOverflow(TextOverflow::ELLIPSIS);
    text->SetTextStyle(textStyle);
    return text;
}

RefPtr<PaddingComponent> NavigationBarComponentBase::BuildPadding(double size, bool isVertical)
{
    auto padding = AceType::MakeRefPtr<PaddingComponent>();
    Edge edge;
    if (isVertical) {
        edge.SetBottom(Dimension(size, DimensionUnit::VP));
    } else {
        edge.SetRight(Dimension(size, DimensionUnit::VP));
    }
    padding->SetPadding(edge);
    return padding;
}

RefPtr<ComposedComponent> NavigationBarComponentBase::GenerateAccessibilityComposed(
    int32_t parentId, const std::string& name, const RefPtr<Component>& child)
{
    const auto& pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        return nullptr;
    }

    const auto& accessibilityManager = pipelineContext->GetAccessibilityManager();
    if (accessibilityManager) {
        auto composedId = accessibilityManager->GenerateNextAccessibilityId();
        auto node = accessibilityManager->CreateSpecializedNode(name, composedId, parentId);
        if (node) {
            node->SetFocusableState(true);
        }
        auto box = AceType::MakeRefPtr<BoxComponent>();
        box->SetChild(child);
        return AceType::MakeRefPtr<ComposedComponent>(std::to_string(composedId), name, box);
    }
    return nullptr;
}

} // namespace OHOS::Ace
