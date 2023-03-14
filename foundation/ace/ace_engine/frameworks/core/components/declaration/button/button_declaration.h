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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_BUTTON_BUTTON_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_BUTTON_BUTTON_DECLARATION_H

#include "frameworks/core/components/declaration/button/button_progress_controller.h"
#include "frameworks/core/components/declaration/common/declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace {

struct ButtonAttribute : Attribute {
    std::string buttonType;
    std::string iconSrc;
    std::string placement;
    std::string textData;
    bool isAutoFocus = false;
    bool isDisabled = false;
    bool isWaiting = false;
};

struct ButtonStyle : Style {
    TextStyle textStyle;
    BorderEdge borderEdge;
    Edge padding;
    Dimension diameter;
    Dimension minWidth;
    Dimension width = -1.0_vp;
    Dimension height = -1.0_vp;
    Dimension radius;
    Dimension iconWidth;
    Dimension iconHeight;
    Dimension innerLeftPadding;
    Color focusColor;
    Color backgroundColor;
    Color clickedColor;
    Color disabledColor;
    Color disabledTextColor;
    Color focusAnimationColor;
    Color hoverColor;
    Color progressColor;
    Color progressFocusColor;
    Color textFocusColor;
    bool bgColorDefined = false;
    bool matchTextDirection = false;
    bool heightDefined = false;
    bool fontSizeDefined = false;
    bool textColorDefined = false;
    bool radiusDefined = false;
    double blendOpacity = 0.0;
};

struct ButtonEvent : Event {
    EventMarker clickEventId;
};

struct ButtonMethod : Method {
    void SetProgress(const RefPtr<ButtonProgressController>& controller, uint32_t progress) const
    {
        controller->SetProgress(progress);
    }
};

class ButtonDeclaration : public Declaration {
    DECLARE_ACE_TYPE(ButtonDeclaration, Declaration);

public:
    ButtonDeclaration() = default;
    ~ButtonDeclaration() override = default;

    void InitializeStyle() override;

    const std::string& GetType() const
    {
        auto& attr = static_cast<ButtonAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attr.buttonType;
    }

    const std::string& GetIconSrc() const
    {
        auto& attr = static_cast<ButtonAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attr.iconSrc;
    }

    const std::string& GetPlacement() const
    {
        auto& attr = static_cast<ButtonAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attr.placement;
    }

    const std::string& GetTextData() const
    {
        auto& attr = static_cast<ButtonAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attr.textData;
    }

    bool GetAutoFocusState() const
    {
        auto& attr = static_cast<ButtonAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attr.isAutoFocus;
    }

    bool GetDisabledState() const
    {
        auto& attr = static_cast<ButtonAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attr.isDisabled;
    }

    bool GetWaitingState() const
    {
        auto& attr = static_cast<ButtonAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attr.isWaiting;
    }

    const Edge& GetPadding() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.padding;
    }

    const TextStyle& GetTextStyle() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.textStyle;
    }

    const Dimension& GetWidth() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.width;
    }

    const Dimension& GetHeight() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.height;
    }

    const Dimension& GetMinWidth() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.minWidth;
    }

    const Dimension& GetRectRadius() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.radius;
    }

    const Dimension& GetProgressDiameter() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.diameter;
    }

    const Dimension& GetInnerPadding() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.innerLeftPadding;
    }

    const Dimension& GetIconHeight() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.iconHeight;
    }

    const Dimension& GetIconWidth() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.iconWidth;
    }

    const Color& GetBackgroundColor() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.backgroundColor;
    }

    const Color& GetClickedColor() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.clickedColor;
    }

    const Color& GetDisabledColor() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.disabledColor;
    }

    const Color& GetDisabledTextColor() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.disabledTextColor;
    }

    const Color& GetFocusColor() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.focusColor;
    }

    const Color& GetHoverColor() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.hoverColor;
    }

    const Color& GetProgressColor() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.progressColor;
    }

    const Color& GetProgressFocusColor() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.progressFocusColor;
    }

    const Color& GetFocusAnimationColor() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.focusAnimationColor;
    }

    const Color& GetFocusTextColor() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.textFocusColor;
    }

    const BorderEdge& GetBorderEdge() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.borderEdge;
    }

    bool GetMatchTextDirection() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.matchTextDirection;
    }

    bool GetBgColorState() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.bgColorDefined;
    }

    bool GetTextColorState() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.textColorDefined;
    }

    bool GetFontSizeState() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.fontSizeDefined;
    }

    bool GetHeightState() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.heightDefined;
    }

    bool GetRadiusState() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.radiusDefined;
    }

    double GetBlendOpacity() const
    {
        auto& style = static_cast<ButtonStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.blendOpacity;
    }

    const EventMarker& GetClickedEventId() const
    {
        auto& event = static_cast<ButtonEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.clickEventId;
    }

    const EventMarker& GetRemoteMessageEventId() const
    {
        auto& event = static_cast<ButtonEvent&>(GetEvent(EventTag::SPECIALIZED_REMOTE_MESSAGE_EVENT));
        return event.clickEventId;
    }

    const RefPtr<ButtonProgressController> GetButtonController() const
    {
        return buttonController_;
    }

    void SetType(const std::string& type)
    {
        auto& attribute = MaybeResetAttribute<ButtonAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.buttonType = type;
    }

    void SetIconSrc(const std::string& src)
    {
        auto& attribute = MaybeResetAttribute<ButtonAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.iconSrc = src;
    }

    void SetPlacement(const std::string& placement)
    {
        auto& attribute = MaybeResetAttribute<ButtonAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.placement = placement;
    }

    void SetTextData(const std::string& data)
    {
        auto& attribute = MaybeResetAttribute<ButtonAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.textData = data;
    }

    void SetAutoFocusState(bool state)
    {
        auto& attribute = MaybeResetAttribute<ButtonAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.isAutoFocus = state;
    }

    void SetDisabledState(bool state)
    {
        auto& attribute = MaybeResetAttribute<ButtonAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.isDisabled = state;
    }

    void SetWaitingState(bool state)
    {
        auto& attribute = MaybeResetAttribute<ButtonAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.isWaiting = state;
    }

    void SetPadding(const Edge& edge)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.padding = edge;
    }

    void SetTextStyle(const TextStyle& textStyle)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.textStyle = textStyle;
    }

    void SetMinWidth(const Dimension& width)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.minWidth = width;
    }

    void SetWidth(const Dimension& width)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.width = width;
    }

    void SetHeight(const Dimension& height)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.height = height;
    }

    void SetRectRadius(const Dimension& radius)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.radius = radius;
    }

    void SetProgressDiameter(const Dimension& diameter)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.diameter = diameter;
    }

    void SetInnerPadding(const Dimension& padding)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.innerLeftPadding = padding;
    }

    void SetIconHeight(const Dimension& height)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.iconHeight = height;
    }

    void SetIconWidth(const Dimension& width)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.iconWidth = width;
    }

    void SetBackgroundColor(const Color& color)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.backgroundColor = color;
    }

    void SetClickedColor(const Color& color)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.clickedColor = color;
    }

    void SetDisabledColor(const Color& color)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.disabledColor = color;
    }

    void SetDisabledTextColor(const Color& color)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.disabledTextColor = color;
    }

    void SetFocusColor(const Color& color)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.focusColor = color;
    }

    void SetHoverColor(const Color& color)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.hoverColor = color;
    }

    void SetProgressColor(const Color& color)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.progressColor = color;
    }

    void SetProgressFocusColor(const Color& color)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.progressFocusColor = color;
    }

    void SetFocusAnimationColor(const Color& color)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.focusAnimationColor = color;
    }

    void SetFocusTextColor(const Color& color)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.textFocusColor = color;
    }

    void SetBorderEdge(const BorderEdge& borderEdge)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.borderEdge = borderEdge;
    }

    void SetMatchTextDirection(bool match)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.matchTextDirection = match;
    }

    void SetBgColorState(bool state)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.bgColorDefined = state;
    }

    void SetTextColorState(bool state)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.textColorDefined = state;
    }

    void SetFontSizeState(bool state)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.fontSizeDefined = state;
    }

    void SetHeightState(bool state)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.heightDefined = state;
    }

    void SetRadiusState(bool state)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.radiusDefined = state;
    }

    void SetBlendOpacity(double opacity)
    {
        auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
        style.blendOpacity = opacity;
    }

    void SetClickedEventId(const EventMarker& eventId)
    {
        auto& event = MaybeResetEvent<ButtonEvent>(EventTag::SPECIALIZED_EVENT);
        event.clickEventId = eventId;
    }

    void SetRemoteMessageEventId(const EventMarker& eventId)
    {
        auto& event = MaybeResetEvent<ButtonEvent>(EventTag::SPECIALIZED_REMOTE_MESSAGE_EVENT);
        event.clickEventId = eventId;
    }

    void SetClickedFunction(std::function<void()>&& callback)
    {
        auto& event = MaybeResetEvent<ButtonEvent>(EventTag::SPECIALIZED_EVENT);
        event.clickEventId.SetPreFunction(std::move(callback));
    }

    void SetButtonController(const RefPtr<ButtonProgressController>& controller)
    {
        buttonController_ = controller;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event) override;
    void CallSpecializedMethod(const std::string& method, const std::string& args) override;

private:
    RefPtr<ButtonProgressController> buttonController_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_BUTTON_BUTTON_DECLARATION_H
