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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_OPTION_OPTION_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_OPTION_OPTION_COMPONENT_H

#include "core/accessibility/accessibility_manager.h"
#include "core/components/box/box_component.h"
#include "core/components/common/properties/color.h"
#include "core/components/flex/flex_component.h"
#include "core/components/image/image_component.h"
#include "core/components/select/select_theme.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace {

class ACE_EXPORT OptionComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(OptionComponent, ComponentGroup);

public:
    OptionComponent() = default;
    explicit OptionComponent(const RefPtr<SelectTheme>& theme);
    ~OptionComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void InitTheme(const RefPtr<ThemeManager>& themeManager);

    const RefPtr<ImageComponent>& GetIcon() const
    {
        return icon_;
    }
    void SetIcon(const RefPtr<ImageComponent>& icon)
    {
        icon_ = icon;
    }

    const RefPtr<TextComponent>& GetText() const
    {
        return text_;
    }
    void SetText(const RefPtr<TextComponent>& text)
    {
        text_ = text;
        CheckOptionModify();
    }

    void SetTheme(const RefPtr<SelectTheme>& theme)
    {
        SetClickedColor(theme->GetClickedColor());
        SetSelectedColor(theme->GetSelectedColor());
        SetFontColor(theme->GetFontColor());
        SetFontSize(theme->GetFontSize());
        SetFontWeight(theme->GetFontWeight());
        SetFontFamily(theme->GetFontFamily());
        SetTextDecoration(theme->GetTextDecoration());
        SetAllowScale(theme->IsAllowScale());
    }

    bool GetSelected() const
    {
        if (!selectable_) {
            return false;
        }
        return selected_;
    }
    void SetSelected(bool selected)
    {
        if (!selectable_) {
            selected_ = false;
        } else {
            selected_ = selected;
        }
    }

    bool GetClicked() const
    {
        return clicked_;
    }
    void SetClicked(bool clicked)
    {
        clicked_ = clicked;
    }

    bool GetFocused() const
    {
        return focused_;
    }
    void SetFocused(bool value)
    {
        focused_ = value;
    }

    bool GetFocusable() const
    {
        return focusable_;
    }
    void SetFocusable(bool value)
    {
        focusable_ = value;
    }

    bool GetVisible() const
    {
        return visible_;
    }
    void SetVisible(bool value)
    {
        visible_ = value;
    }

    std::string GetValue() const
    {
        return value_;
    }
    void SetValue(const std::string& value)
    {
        value_ = value;
    }

    const Color& GetClickedColor() const
    {
        return theme_->GetClickedColor();
    }
    void SetClickedColor(const Color& clickedColor)
    {
        theme_->SetClickedColor(clickedColor);
    }

    const Color& GetSelectedColor() const
    {
        return theme_->GetSelectedColor();
    }
    void SetSelectedColor(const Color& selectedColor)
    {
        theme_->SetSelectedColor(selectedColor);
    }

    const Color& GetFontColor() const
    {
        return theme_->GetFontColor();
    }
    void SetFontColor(const Color& fontColor)
    {
        theme_->SetFontColor(fontColor);
    }

    FontWeight GetFontWeight() const
    {
        return theme_->GetFontWeight();
    }
    void SetFontWeight(FontWeight fontWeight)
    {
        theme_->SetFontWeight(fontWeight);
    }

    const std::string& GetFontFamily() const
    {
        return theme_->GetFontFamily();
    }
    void SetFontFamily(const std::string& fontFamily)
    {
        theme_->SetFontFamily(fontFamily);
    }

    TextDecoration GetTextDecoration() const
    {
        return theme_->GetTextDecoration();
    }
    void SetTextDecoration(TextDecoration textDecoration)
    {
        theme_->SetTextDecoration(textDecoration);
    }

    const Dimension& GetFontSize() const
    {
        return theme_->GetFontSize();
    }
    void SetFontSize(const Dimension& fontSize)
    {
        theme_->SetFontSize(fontSize);
    }

    const std::function<void(std::size_t)>& GetClickedCallback() const
    {
        return clickedCallback_;
    }

    void SetClickedCallback(const std::function<void(std::size_t)>& clickedCallback)
    {
        clickedCallback_ = clickedCallback;
    }

    void SetModifiedCallback(const std::function<void(std::size_t)>& value)
    {
        modifiedCallback_ = value;
    }

    std::size_t GetIndex() const
    {
        return index_;
    }
    void SetIndex(std::size_t index)
    {
        index_ = index;
    }

    bool GetSelectable() const
    {
        return selectable_;
    }
    void SetSelectable(bool selectable)
    {
        selectable_ = selectable;
        if (!selectable) {
            selected_ = false;
        }
    }

    ShowInNavigationBar GetShowInNavigationBar() const
    {
        return showInNavigationBar_;
    }

    void SetShowInNavigationBar(ShowInNavigationBar showInNavigationBar)
    {
        showInNavigationBar_ = showInNavigationBar;
    }

    bool Initialize(const RefPtr<AccessibilityManager>& manager);

    const ComposeId& GetId() const
    {
        return id_;
    }

    void SetId(const ComposeId& id)
    {
        id_ = id;
    }

    bool IsAllowScale() const
    {
        return theme_->IsAllowScale();
    }

    void SetAllowScale(bool allowScale)
    {
        theme_->SetAllowScale(allowScale);
    }

    const RefPtr<SelectTheme>& GetTheme() const
    {
        return theme_;
    }

    const RefPtr<AccessibilityNode> GetNode() const
    {
        return node_;
    }

    void SetNode(const RefPtr<AccessibilityNode>& value)
    {
        node_ = value;
    }

    int32_t GetParentId() const
    {
        return parentId_;
    }

    void SetParentId(int32_t value)
    {
        parentId_ = value;
    }

    void SetDisabled(bool disable)
    {
        disabled_ = disable;
    }

    bool GetDisabled() const
    {
        return disabled_;
    }

    void SetClickEvent(const EventMarker& clickEvent)
    {
        clickEvent_ = clickEvent;
    }

    const EventMarker& GetClickEvent() const
    {
        return clickEvent_;
    }

    void SetClickEventForToolBarItem(const EventMarker& clickEventForToolBarItem)
    {
        clickEventForToolBarItem_ = clickEventForToolBarItem;
    }

    const EventMarker& GetClickEventForToolBarItem() const
    {
        return clickEventForToolBarItem_;
    }

    // used for inspector node in PC preview
    void SetAttr(const std::vector<std::pair<std::string, std::string>>& attrs)
    {
        attrs_ = attrs;
    }

    // used for inspector node in PC preview
    void SetStyle(const std::vector<std::pair<std::string, std::string>>& styles)
    {
        styles_ = styles;
    }

    const std::function<void()>& GetCustomizedCallback()
    {
        return customizedCallback_;
    }

    void SetCustomizedCallback(const std::function<void()>& onClickFunc)
    {
        customizedCallback_ = onClickFunc;
    }

    void SetCustomComponent(const RefPtr<Component>& component)
    {
        customComponent_ = component;
    }

    const RefPtr<Component>& GetCustomComponent() const
    {
        return customComponent_;
    }

    const TextStyle& GetTextStyle() const
    {
        return textStyle_;
    }
    void SetTextStyle(const TextStyle& style)
    {
        textStyle_ = style;
    }

    const TextStyle& GetSelectedTextStyle() const
    {
        return selectedTextStyle_;
    }
    void SetSelectedTextStyle(const TextStyle& style)
    {
        selectedTextStyle_ = style;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }
    void SetBackgroundColor(const Color& backgroundColor)
    {
        backgroundColor_ = backgroundColor;
    }

    const Color& GetSelectedBackgroundColor() const
    {
        return selectedBackgroundColor_;
    }
    void SetSelectedBackgroundColor(const Color& backgroundColor)
    {
        selectedBackgroundColor_ = backgroundColor;
    }

private:
    // used for inspector node in PC preview
    const std::vector<std::pair<std::string, std::string>> GetAttr()
    {
        return attrs_;
    }

    // used for inspector node in PC preview
    const std::vector<std::pair<std::string, std::string>> GetStyle()
    {
        return styles_;
    }

    void CheckOptionModify();

    // used for inspector node in PC preview
    std::vector<std::pair<std::string, std::string>> attrs_;
    std::vector<std::pair<std::string, std::string>> styles_;
    RefPtr<ImageComponent> icon_;
    RefPtr<TextComponent> text_;
    RefPtr<Component> customComponent_;
    bool selected_ = false;
    bool clicked_ = false;
    bool focused_ = false;
    bool selectable_ = true;
    std::string value_;
    ShowInNavigationBar showInNavigationBar_ = ShowInNavigationBar::SHOW;
    RefPtr<SelectTheme> theme_;
    std::size_t index_ = SELECT_INVALID_INDEX;
    std::function<void(std::size_t)> clickedCallback_;
    std::function<void(std::size_t)> modifiedCallback_;
    std::function<void()> customizedCallback_;

    std::string lastText_;
    ComposeId id_ = "-1";
    EventMarker clickEvent_;
    EventMarker clickEventForToolBarItem_;

    TextStyle textStyle_;
    TextStyle selectedTextStyle_;
    Color backgroundColor_ = Color::TRANSPARENT;
    Color selectedBackgroundColor_;

    RefPtr<AccessibilityNode> node_;
    int32_t parentId_ = -1;
    bool disabled_ = false;
    bool visible_ = true;
    bool focusable_ = true;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_OPTION_OPTION_COMPONENT_H
