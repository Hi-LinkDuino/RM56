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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_BADGE_BADGE_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_BADGE_BADGE_DECLARATION_H

#include "core/components/declaration/common/declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace {

struct BadgeAttribute : Attribute {
    bool showMessage = true;
    BadgePosition badgePosition { BadgePosition::RIGHT_TOP };
    int64_t messageCount = 0;
    int64_t maxCount = 99;
    std::string badgeLabel;
};

struct BadgeStyle : Style {
    Color badgeColor;
    Color badgeTextColor;
    Edge padding;
    Dimension badgeFontSize;
    Dimension badgeCircleSize;
    bool badgeCircleSizeDefined = false;
};

struct BadgeEvent : Event {
    EventMarker clickEvent;
};

class BadgeDeclaration : public Declaration {
    DECLARE_ACE_TYPE(BadgeDeclaration, Declaration);

public:
    BadgeDeclaration() = default;
    ~BadgeDeclaration() override = default;

    void InitializeStyle() override;

    BadgePosition GetBadgePosition() const
    {
        auto& attribute = static_cast<BadgeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.badgePosition;
    }

    const std::string& GetBadgeLabel() const
    {
        auto& attribute = static_cast<BadgeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.badgeLabel;
    }

    int64_t GetMessageCount() const
    {
        auto& attribute = static_cast<BadgeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.messageCount;
    }

    bool IsShowMessage() const
    {
        auto& attribute = static_cast<BadgeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.showMessage;
    }

    int64_t GetMaxCount() const
    {
        auto& attribute = static_cast<BadgeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.maxCount;
    }

    const Color& GetBadgeColor() const
    {
        auto& style = static_cast<BadgeStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.badgeColor;
    }

    const Color& GetBadgeTextColor() const
    {
        auto& style = static_cast<BadgeStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.badgeTextColor;
    }

    const Dimension& GetBadgeFontSize() const
    {
        auto& style = static_cast<BadgeStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.badgeFontSize;
    }

    const Edge& GetPadding() const
    {
        auto& style = static_cast<BadgeStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.padding;
    }

    const Dimension& GetBadgeCircleSize() const
    {
        auto& style = static_cast<BadgeStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.badgeCircleSize;
    }

    bool IsBadgeCircleSizeDefined() const
    {
        auto& style = static_cast<BadgeStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.badgeCircleSizeDefined;
    }

    const EventMarker& GetClickEvent() const
    {
        auto& event = static_cast<BadgeEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.clickEvent;
    }

    void SetMaxCount(int64_t maxCount)
    {
        auto& attribute = MaybeResetAttribute<BadgeAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.maxCount = maxCount;
    }

    void SetShowMessage(bool showMessage)
    {
        auto& attribute = MaybeResetAttribute<BadgeAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.showMessage = showMessage;
    }

    void SetMessageCount(int64_t messageCount)
    {
        auto& attribute = MaybeResetAttribute<BadgeAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.messageCount = messageCount;
    }

    void SetBadgePosition(BadgePosition badgePostion)
    {
        auto& attribute = MaybeResetAttribute<BadgeAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.badgePosition = badgePostion;
    }

    void SetBadgeCircleSizeDefined(bool badgeCircleSizeDefined)
    {
        auto& style = MaybeResetStyle<BadgeStyle>(StyleTag::SPECIALIZED_STYLE);
        style.badgeCircleSizeDefined = badgeCircleSizeDefined;
    }

    void SetPadding(const Edge& padding)
    {
        auto& style = MaybeResetStyle<BadgeStyle>(StyleTag::SPECIALIZED_STYLE);
        style.padding = padding;
    }

    void SetBadgeTextColor(const Color& badgeTextColor)
    {
        auto& style = MaybeResetStyle<BadgeStyle>(StyleTag::SPECIALIZED_STYLE);
        style.badgeTextColor = badgeTextColor;
    }

    void SetBadgeFontSize(const Dimension& badgeFontSize)
    {
        auto& style = MaybeResetStyle<BadgeStyle>(StyleTag::SPECIALIZED_STYLE);
        style.badgeFontSize = badgeFontSize;
    }

    void SetBadgeColor(const Color& color)
    {
        auto& style = MaybeResetStyle<BadgeStyle>(StyleTag::SPECIALIZED_STYLE);
        style.badgeColor = color;
    }

    void SetBadgeCircleSize(const Dimension& badgeCircleSize)
    {
        auto& style = MaybeResetStyle<BadgeStyle>(StyleTag::SPECIALIZED_STYLE);
        style.badgeCircleSize = badgeCircleSize;
        style.badgeCircleSizeDefined = true;
    }

    void SetBadgeLabel(const std::string& badgeLabel)
    {
        auto& attribute = MaybeResetAttribute<BadgeAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.badgeLabel = badgeLabel;
    }

    void SetClickEvent(const EventMarker& event)
    {
        auto& badgeEvent = MaybeResetEvent<BadgeEvent>(EventTag::SPECIALIZED_EVENT);
        badgeEvent.clickEvent = event;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_BADGE_BADGE_DECLARATION_H
