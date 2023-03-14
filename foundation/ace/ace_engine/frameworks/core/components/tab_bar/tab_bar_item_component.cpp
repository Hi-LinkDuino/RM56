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

#include "core/components/tab_bar/tab_bar_item_component.h"

#include "core/components/flex/flex_component.h"
#include "core/components/tab_bar/render_tab_bar_item.h"
#include "core/components/tab_bar/tab_bar_item_element.h"

namespace OHOS::Ace {

TabBarItemComponent::TabBarItemComponent(const RefPtr<Component>& child)
{
    SetChild(child);
}

TabBarItemComponent::TabBarItemComponent(const std::string& text, const RefPtr<Component>& imageComponent)
{
    std::list<RefPtr<Component>> children;
    RefPtr<ImageComponent> icon = AceType::DynamicCast<ImageComponent>(imageComponent);
    if (icon) {
        icon_ = icon;
        children.push_back(icon_);
    }
    if (!text.empty()) {
        text_ = AceType::MakeRefPtr<TextComponent>(text);
        TextStyle textStyle;
        textStyle.SetTextAlign(TextAlign::CENTER);
        textStyle.SetMaxLines(1);
        textStyle.SetTextOverflow(TextOverflow::CLIP);
        text_->SetTextStyle(textStyle);
        children.push_back(text_);
    }
    if (!children.empty()) {
        auto flex = AceType::MakeRefPtr<ColumnComponent>(FlexAlign::CENTER, FlexAlign::CENTER, children);
        SetChild(flex);
    } else {
        LOGE("Create Tab err: text == null && icon == null");
    }
}

RefPtr<Element> TabBarItemComponent::CreateElement()
{
    return AceType::MakeRefPtr<TabBarItemElement>();
}

RefPtr<RenderNode> TabBarItemComponent::CreateRenderNode()
{
    return RenderTabBarItem::Create();
}

void FindChildren(const RefPtr<Component>& component, std::stack<RefPtr<Component>>& allChildren)
{
    auto sigleChildGroup = AceType::DynamicCast<SingleChild>(component);
    if (sigleChildGroup) {
        allChildren.push(sigleChildGroup->GetChild());
    }

    auto multiChildGroup = AceType::DynamicCast<ComponentGroup>(component);
    if (multiChildGroup) {
        for (const auto& item : multiChildGroup->GetChildren()) {
            allChildren.push(item);
        }
    }
}

void TabBarItemComponent::UpdateStyle(const TextStyle& textStyle, const Color& color)
{
    std::stack<RefPtr<Component>> allChildren;
    allChildren.push(GetChild());

    while (!allChildren.empty()) {
        auto component = allChildren.top();
        allChildren.pop();
        auto text = AceType::DynamicCast<TextComponent>(component);
        auto image = AceType::DynamicCast<ImageComponent>(component);
        if (text) {
            text->SetTextStyle(textStyle);
        } else if (image) {
            image->SetImageFill(color);
        } else {
            FindChildren(component, allChildren);
        }
    }
}

} // namespace OHOS::Ace