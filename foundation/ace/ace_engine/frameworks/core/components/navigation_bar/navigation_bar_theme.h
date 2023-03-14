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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_BAR_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_BAR_THEME_H

#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

class NavigationBarTheme : public virtual Theme {
    DECLARE_ACE_TYPE(NavigationBarTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<NavigationBarTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<NavigationBarTheme> theme = AceType::Claim(new NavigationBarTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->titleColor_ = themeConstants->GetColor(THEME_NAVIGATION_BAR_TITLE_COLOR);
            theme->titleFontSize_ = Dimension(
                themeConstants->GetDimension(THEME_NAVIGATION_BAR_TITLE_FONT_SIZE).Value(), DimensionUnit::VP);
            theme->titleFontSizeBig_ = themeConstants->GetDimension(THEME_NAVIGATION_BAR_TITLE_FONT_SIZE_BIG);
            theme->subTitleColor_ = themeConstants->GetColor(THEME_NAVIGATION_BAR_SUBTITLE_COLOR);
            theme->subTitleFontSize_ = Dimension(
                themeConstants->GetDimension(THEME_NAVIGATION_BAR_SUBTITLE_FONT_SIZE).Value(), DimensionUnit::VP);
            theme->height_ = themeConstants->GetDimension(THEME_NAVIGATION_BAR_HEIGHT);
            theme->heightEmphasize_ = themeConstants->GetDimension(THEME_NAVIGATION_BAR_HEIGHT_EMPHASIZE);
            theme->backResourceId_ = themeConstants->GetResourceId(THEME_NAVIGATION_BAR_RESOURCE_ID_BACK);
            theme->moreResourceId_ = themeConstants->GetResourceId(THEME_NAVIGATION_BAR_RESOURCE_ID_MORE);
            theme->menuZoneSize_ = themeConstants->GetDimension(THEME_NAVIGATION_BAR_MENU_ZONE_SIZE);
            theme->menuIconSize_ = themeConstants->GetDimension(THEME_NAVIGATION_BAR_MENU_ICON_SIZE);
            theme->logoIconSize_ = themeConstants->GetDimension(THEME_NAVIGATION_BAR_LOGO_ICON_SIZE);
            theme->buttonNormalColor_ = themeConstants->GetColor(THEME_NAVIGATION_BAR_BUTTON_NORMAL_COLOR);
            theme->buttonPressedColor_ = themeConstants->GetColor(THEME_NAVIGATION_BAR_BUTTON_PRESSED_COLOR);
            theme->buttonFocusColor_ = themeConstants->GetColor(THEME_NAVIGATION_BAR_BUTTON_FOCUS_COLOR);
            theme->buttonCornerRadius_ = themeConstants->GetDimension(THEME_NAVIGATION_BAR_BUTTON_CORNER_RADIUS);
            theme->maxPaddingStart_ = themeConstants->GetDimension(THEME_NAVIGATION_BAR_MAX_PADDING_START);
            theme->maxPaddingEnd_ = themeConstants->GetDimension(THEME_NAVIGATION_BAR_MAX_PADDING_END);
            theme->defaultPaddingStart_ = themeConstants->GetDimension(THEME_NAVIGATION_BAR_DEFAULT_PADDING_START);
            theme->defaultPaddingEnd_ = themeConstants->GetDimension(THEME_NAVIGATION_BAR_DEFAULT_PADDING_END);
            theme->menuItemPadding_ = themeConstants->GetDimension(THEME_NAVIGATION_BAR_MENU_ITEM_PADDING);
            theme->titleMinPadding_ = themeConstants->GetDimension(THEME_NAVIGATION_BAR_TITLE_MIN_PADDING);
            auto menuCount = themeConstants->GetInt(THEME_NAVIGATION_BAR_MOST_MENU_ITEM_COUNT_IN_BAR);
            theme->mostMenuItemCountInBar_ =
                menuCount < 0 ? theme->mostMenuItemCountInBar_ : static_cast<uint32_t>(menuCount);

            auto themeStyle = themeConstants->GetThemeStyle();
            if (!themeStyle) {
                return theme;
            }
            theme->titleColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_APPBAR_TITLE, Color::WHITE);
            theme->titleFontSize_  = themeStyle->GetAttr<Dimension>(THEME_ATTR_TEXT_SIZE_HEADLINE7, 0.0_vp);
            theme->titleFontSizeBig_  = themeStyle->GetAttr<Dimension>(THEME_ATTR_TEXT_SIZE_HEADLINE6, 0.0_vp);
            theme->subTitleColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_APPBAR_SUBTITLE, Color::WHITE);
            theme->subTitleFontSize_  = themeStyle->GetAttr<Dimension>(THEME_ATTR_TEXT_SIZE_OVERLINE, 0.0_vp);
            theme->menuIconColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_APPBAR_ICON, Color::WHITE);
            theme->buttonPressedColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_CLICK_EFFECT, Color::WHITE);
            theme->buttonFocusColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_FOCUSED, Color::WHITE);
            theme->buttonHoverColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_HOVER, Color::WHITE);
            theme->buttonCornerRadius_  = themeStyle->GetAttr<Dimension>(THEME_ATTR_CLICK_CORNER_RADIUS, 0.0_vp);

            return theme;
        }
    };

    ~NavigationBarTheme() override = default;

    const Color& GetTitleColor() const
    {
        return titleColor_;
    }

    const Color& GetSubTitleColor() const
    {
        return subTitleColor_;
    }
    const Dimension& GetTitleFontSizeBig() const
    {
        return titleFontSizeBig_;
    }

    const Dimension& GetTitleFontSize() const
    {
        return titleFontSize_;
    }

    const Dimension& GetSubTitleFontSize() const
    {
        return subTitleFontSize_;
    }
    const Dimension& GetHeight() const
    {
        return height_;
    }
    const Dimension& GetHeightEmphasize() const
    {
        return heightEmphasize_;
    }
    InternalResource::ResourceId GetBackResourceId() const
    {
        return backResourceId_;
    }
    InternalResource::ResourceId GetMoreResourceId() const
    {
        return moreResourceId_;
    }
    const Dimension& GetMenuZoneSize() const
    {
        return menuZoneSize_;
    }
    const Dimension& GetMenuIconSize() const
    {
        return menuIconSize_;
    }
    const Dimension& GetLogoIconSize() const
    {
        return logoIconSize_;
    }
    const Color& GetMenuIconColor() const
    {
        return menuIconColor_;
    }
    const Color& GetButtonNormalColor() const
    {
        return buttonNormalColor_;
    }
    const Color& GetButtonPressedColor() const
    {
        return buttonPressedColor_;
    }
    const Color& GetButtonFocusColor() const
    {
        return buttonFocusColor_;
    }
    const Color& GetButtonHoverColor() const
    {
        return buttonHoverColor_;
    }
    const Dimension& GetButtonCornerRadius() const
    {
        return buttonCornerRadius_;
    }
    const Dimension& GetMaxPaddingStart() const
    {
        return maxPaddingStart_;
    }
    const Dimension& GetDefaultPaddingStart() const
    {
        return defaultPaddingStart_;
    }
    const Dimension& GetDefaultPaddingEnd() const
    {
        return defaultPaddingEnd_;
    }
    const Dimension& GetMaxPaddingEnd() const
    {
        return maxPaddingEnd_;
    }
    const Dimension& GetMenuItemPadding() const
    {
        return menuItemPadding_;
    }
    const Dimension& GetTitleMinPadding() const
    {
        return titleMinPadding_;
    }
    uint32_t GetMostMenuItemCountInBar() const
    {
        return mostMenuItemCountInBar_;
    }

protected:
    NavigationBarTheme() = default;

private:
    Color titleColor_;
    Color subTitleColor_;
    Dimension titleFontSizeBig_;
    Dimension titleFontSize_;
    Dimension subTitleFontSize_;
    Dimension height_;
    Dimension heightEmphasize_;
    InternalResource::ResourceId backResourceId_ = InternalResource::ResourceId::NO_ID;
    InternalResource::ResourceId moreResourceId_ = InternalResource::ResourceId::NO_ID;
    Dimension menuZoneSize_;
    Dimension menuIconSize_;
    Dimension logoIconSize_;
    Color menuIconColor_;
    Color buttonNormalColor_;
    Color buttonPressedColor_;
    Color buttonFocusColor_;
    Color buttonHoverColor_;
    Dimension buttonCornerRadius_;
    Dimension maxPaddingStart_;
    Dimension maxPaddingEnd_;
    Dimension defaultPaddingStart_;
    Dimension defaultPaddingEnd_;
    Dimension menuItemPadding_;
    Dimension titleMinPadding_;
    uint32_t mostMenuItemCountInBar_ = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_BAR_THEME_H
