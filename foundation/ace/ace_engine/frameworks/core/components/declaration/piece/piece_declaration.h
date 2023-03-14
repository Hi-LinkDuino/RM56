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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_PIECE_PIECE_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_PIECE_PIECE_DECLARATION_H

#include "core/components/declaration/common/declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/core/components/piece/piece_theme.h"

namespace OHOS::Ace {

struct PieceAttribute : Attribute {
    bool showDelete = false;
    std::string content;
    std::string icon;
    Dimension interval; // Interval between text and icon.
    Dimension iconSize;
    InternalResource::ResourceId iconResource = InternalResource::ResourceId::NO_ID;
};

struct PieceStyle : Style {
    TextStyle textStyle;
    Edge margin;
    Border border;
    Color hoverColor;
    Color backGroundColor;
};

struct PieceEvent : Event {
    EventMarker onDelete;
};

class PieceDeclaration : public Declaration {
    DECLARE_ACE_TYPE(PieceDeclaration, Declaration);

public:
    PieceDeclaration() = default;
    ~PieceDeclaration() override = default;

    void InitializeStyle(RefPtr<PieceTheme>& theme);
    void InitializeStyle() override;
    const std::string& GetContent() const
    {
        auto& attribute = static_cast<PieceAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.content;
    }
    void SetContent(const std::string& content)
    {
        auto& attribute = MaybeResetAttribute<PieceAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.content = content;
    }

    const std::string& GetIcon() const
    {
        auto& attribute = static_cast<PieceAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.icon;
    }
    void SetIcon(const std::string& icon)
    {
        auto& attribute = MaybeResetAttribute<PieceAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.icon = icon;
    }

    bool ShowDelete() const
    {
        auto& attribute = static_cast<PieceAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.showDelete;
    }
    void SetShowDelete(bool showDelete)
    {
        auto& attribute = MaybeResetAttribute<PieceAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.showDelete = showDelete;
    }

    const Dimension& GetInterval() const
    {
        auto& attribute = static_cast<PieceAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.interval;
    }
    void SetInterval(const Dimension& interval)
    {
        auto& attribute = MaybeResetAttribute<PieceAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.interval = interval;
    }

    InternalResource::ResourceId GetIconResource() const
    {
        auto& attribute = static_cast<PieceAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.iconResource;
    }
    void SetIconResource(InternalResource::ResourceId iconResource)
    {
        auto& attribute = MaybeResetAttribute<PieceAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.iconResource = iconResource;
    }

    const Dimension& GetIconSize() const
    {
        auto& attribute = static_cast<PieceAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.iconSize;
    }
    void SetIconSize(const Dimension& iconSize)
    {
        auto& attribute = MaybeResetAttribute<PieceAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.iconSize = iconSize;
    }

    const TextStyle& GetTextStyle() const
    {
        auto& style = static_cast<PieceStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.textStyle;
    }
    void SetTextStyle(const TextStyle& textStyle)
    {
        auto& style = MaybeResetStyle<PieceStyle>(StyleTag::SPECIALIZED_STYLE);
        style.textStyle = textStyle;
    }

    const Edge& GetMargin() const
    {
        auto& style = static_cast<PieceStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.margin;
    }
    void SetMargin(const Edge& margin)
    {
        auto& style = MaybeResetStyle<PieceStyle>(StyleTag::SPECIALIZED_STYLE);
        style.margin = margin;
    }

    const Border& GetBorder() const
    {
        auto& style = static_cast<PieceStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.border;
    }
    void SetBorder(const Border& border)
    {
        auto& style = MaybeResetStyle<PieceStyle>(StyleTag::SPECIALIZED_STYLE);
        style.border = border;
    }

    const Color& GetHoverColor() const
    {
        auto& style = static_cast<PieceStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.hoverColor;
    }

    void SetHoverColor(const Color& hoverColor)
    {
        auto& style = MaybeResetStyle<PieceStyle>(StyleTag::SPECIALIZED_STYLE);
        style.hoverColor = hoverColor;
    }

    void SetBackGroundColor(const Color& backGroundColor)
    {
        auto& style = MaybeResetStyle<PieceStyle>(StyleTag::SPECIALIZED_STYLE);
        style.backGroundColor = backGroundColor;
    }

    const Color& GetBackGroundColor() const
    {
        auto& style = static_cast<PieceStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.backGroundColor;
    }

    const EventMarker& GetOnDelete() const
    {
        auto& event = static_cast<PieceEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.onDelete;
    }
    void SetOnDelete(const EventMarker& onDelete)
    {
        auto& event = MaybeResetEvent<PieceEvent>(EventTag::SPECIALIZED_EVENT);
        event.onDelete = onDelete;
    }

    bool HasContent() const
    {
        return hasContent_;
    }

    bool HasBackground() const
    {
        return hasBackground_;
    }

    const std::optional<Color>& GetImageFill() const
    {
        auto& imageStyle = static_cast<CommonImageStyle&>(GetStyle(StyleTag::COMMON_IMAGE_STYLE));
        return imageStyle.imageFill;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event) override;

private:
    bool hasContent_ = false;
    bool hasBackground_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_PIECE_PIECE_DECLARATION_H
