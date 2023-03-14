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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_TOOL_BAR_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_TOOL_BAR_THEME_H

#include <vector>

#include "core/components/common/properties/edge.h"
#include "core/components/common/properties/text_style.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

/**
 * ToolBarTheme defines color and styles of ToolBar. ToolBarTheme should be built
 * using ToolBarTheme::Builder.
 */
class ToolBarTheme : public virtual Theme {
    DECLARE_ACE_TYPE(ToolBarTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<ToolBarTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<ToolBarTheme> theme = AceType::Claim(new ToolBarTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->textStyle_.SetFontSize(themeConstants->GetDimension(THEME_TOOL_BAR_TEXT_FONTSIZE));
            theme->textStyle_.SetFontFamilies({ themeConstants->GetString(THEME_OHOS_TEXT_FONT_FAMILY_MEDIUM) });
            theme->textStyle_.SetAdaptTextSize(themeConstants->GetDimension(THEME_TOOL_BAR_TEXT_FONTSIZE),
                themeConstants->GetDimension(THEME_TOOL_BAR_TEXT_MIN_FONTSIZE));
            theme->iconSize_ = Size(themeConstants->GetDimension(THEME_TOOL_BAR_ITEM_ICON_WIDTH).Value(),
                themeConstants->GetDimension(THEME_TOOL_BAR_ITEM_ICON_HEIGHT).Value());
            theme->imageEdge_ = Edge(themeConstants->GetDimension(THEME_TOOL_BAR_ITEM_PADDING_ICON_LEFT),
                themeConstants->GetDimension(THEME_TOOL_BAR_ITEM_PADDING_ICON_TOP),
                themeConstants->GetDimension(THEME_TOOL_BAR_ITEM_PADDING_ICON_RIGHT),
                themeConstants->GetDimension(THEME_TOOL_BAR_ITEM_PADDING_ICON_BOTTOM));
            theme->textEdge_ = Edge(themeConstants->GetDimension(THEME_TOOL_BAR_ITEM_PADDING_TEXT_LEFT),
                themeConstants->GetDimension(THEME_TOOL_BAR_ITEM_PADDING_TEXT_TOP),
                themeConstants->GetDimension(THEME_TOOL_BAR_ITEM_PADDING_TEXT_RIGHT),
                themeConstants->GetDimension(THEME_TOOL_BAR_ITEM_PADDING_TEXT_BOTTOM));
            theme->toolBarBgColor_ = themeConstants->GetColor(THEME_TOOL_BAR_BG_COLOR);
            theme->focusColor_ = themeConstants->GetColor(THEME_TOOL_BAR_ITEM_FOCUS_COLOR);
            theme->hoverColor_ = themeConstants->GetColor(THEME_TOOL_BAR_ITEM_HOVER_COLOR);
            theme->toolBarItemBgColor_ = themeConstants->GetColor(THEME_TOOL_BAR_ITEM_BACKGROUND_COLOR);
            theme->pressColor_ = themeConstants->GetColor(THEME_OHOS_CONTROL_NORMAL);
            theme->radius_ = themeConstants->GetDimension(THEME_TOOL_BAR_ITEM_RADIUS);
            theme->iconMoreColor_ = themeConstants->GetColor(THEME_TOOL_BAR_ENDITEM_COLOR);
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }

    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<ToolBarTheme>& theme) const
        {
            if (!themeStyle) {
                return;
            }
            theme->textStyle_.SetTextColor(themeStyle->GetAttr<Color>(THEME_ATTR_TOOLBAR_TEXT_COLOR, Color::BLACK));
            theme->iconColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_TOOLBAR_ICON_COLOR, Color());
            theme->toolBarBgColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_TOOLBAR_BG_COLOR, Color());
            theme->toolBarItemBgColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_TOOLBAR_SUB_BG_COLOR, Color());

            auto toolbarPattern = themeStyle->GetAttr<RefPtr<ThemeStyle>>(THEME_PATTERN_TOOLBAR, nullptr);
            if (!toolbarPattern) {
                LOGI("ToolbarPattern is null");
                return;
            }
            theme->iconMoreColor_ = toolbarPattern->GetAttr<Color>(TOOLBAR_ICON_MORE_COLOR, Color());
        }
    };

    ~ToolBarTheme() override = default;

    const TextStyle& GetToolBarTextStyle() const
    {
        return textStyle_;
    }

    const Size& GetIconSize() const
    {
        return iconSize_;
    }

    const Edge& GetIconEdge() const
    {
        return imageEdge_;
    }

    const Edge& GetTextEdge() const
    {
        return textEdge_;
    }

    const Color& GetToolBarBgColor() const
    {
        return toolBarBgColor_;
    }

    const Color& GetItemBackgroundColor() const
    {
        return toolBarItemBgColor_;
    }

    const Color& GetIconColor() const
    {
        return iconColor_;
    }

    const Color& GetFocusColor() const
    {
        return focusColor_;
    }

    const Color& GetHoverColor() const
    {
        return hoverColor_;
    }

    const Color& GetPressColor() const
    {
        return pressColor_;
    }

    const Dimension& GetRadius() const
    {
        return radius_;
    }

    const Color& GetIconMoreColor() const
    {
        return iconMoreColor_;
    }

protected:
    ToolBarTheme() = default;

private:
    Edge imageEdge_;
    Edge textEdge_;
    TextStyle textStyle_;
    Size iconSize_;
    Color toolBarBgColor_;
    Color iconColor_;
    Color focusColor_;
    Color hoverColor_;
    Color toolBarItemBgColor_;
    Color pressColor_;
    Dimension radius_;
    Color iconMoreColor_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_TOOL_BAR_THEME_H
