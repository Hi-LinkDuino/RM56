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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_THEME_H

#include "core/components/common/properties/text_style.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/**
 * TabTheme defines color and styles of tab. TabTheme should be built
 * using TabTheme::Builder.
 */
class TabTheme : public virtual Theme {
    DECLARE_ACE_TYPE(TabTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<TabTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<TabTheme> theme = AceType::Claim(new TabTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->labelPadding_ = themeConstants->GetDimension(THEME_TAB_LABEL_PADDING);
            theme->padding_ = themeConstants->GetDimension(THEME_TAB_PADDING);
            theme->gradientWidth_ = themeConstants->GetDimension(THEME_TAB_GRADIENT_WIDTH);
            theme->defaultHeight_ = themeConstants->GetDimension(THEME_TAB_DEFAULT_HEIGHT);
            theme->defaultWidth_ = themeConstants->GetDimension(THEME_TAB_DEFAULT_WIDTH);
            theme->defaultItemHeight_ = themeConstants->GetDimension(THEME_TAB_DEFAULT_ITEM_HEIGHT);
            theme->backgroundColor_ = themeConstants->GetColor(THEME_TAB_BACKGROUND_COLOR);
            theme->activeIndicatorColor_ = themeConstants->GetColor(THEME_TAB_ACTIVE_INDICATOR_COLOR);
            theme->activeIndicatorWidth_ = themeConstants->GetDimension(THEME_TAB_ACTIVE_INDICATOR_WIDTH);
            theme->activeIndicatorMinWidth_ = themeConstants->GetDimension(THEME_TAB_ACTIVE_INDICATOR_MIN_WIDTH);
            theme->activeIndicatorPadding_ = themeConstants->GetDimension(THEME_TAB_ACTIVE_INDICATOR_PADDING);
            theme->focusIndicatorColor_ = themeConstants->GetColor(THEME_TAB_FOCUS_INDICATOR_COLOR);
            theme->focusIndicatorRadius_ = themeConstants->GetDimension(THEME_TAB_FOCUS_INDICATOR_RADIUS);
            theme->focusIndicatorHorizontalPadding_ =
                themeConstants->GetDimension(THEME_TAB_FOCUS_INDICATOR_HORIZONTAL_PADDING);
            theme->focusIndicatorVerticalPadding_ =
                themeConstants->GetDimension(THEME_TAB_FOCUS_INDICATOR_VERTICAL_PADDING);
            auto themeStyle = themeConstants->GetThemeStyle();
            if (!themeStyle) {
                return theme;
            }
            theme->backgroundColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_TAB_BACKGROUND, Color::WHITE);
            theme->activeIndicatorColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_SUBTAB_LINE_ON, Color::WHITE);
            theme->focusIndicatorColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_SUBTAB_LINE_ON, Color::WHITE);
            theme->focusIndicatorRadius_  = themeStyle->GetAttr<Dimension>(THEME_ATTR_CLICK_CORNER_RADIUS, 0.0_vp);
            return theme;
        }
    };

    ~TabTheme() override = default;

    const Dimension& GetLabelPadding() const
    {
        return labelPadding_;
    }

    const Dimension& GetPadding() const
    {
        return padding_;
    }

    const Dimension& GetGradientWidth() const
    {
        return gradientWidth_;
    }

    const Dimension& GetDefaultHeight() const
    {
        return defaultHeight_;
    }

    const Dimension& GetDefaultWidth() const
    {
        return defaultWidth_;
    }

    const Dimension& GetDefaultItemHeight() const
    {
        return defaultItemHeight_;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    const Color& GetActiveIndicatorColor() const
    {
        return activeIndicatorColor_;
    }

    const Dimension& GetActiveIndicatorWidth() const
    {
        return activeIndicatorWidth_;
    }

    const Dimension& GetActiveIndicatorMinWidth() const
    {
        return activeIndicatorMinWidth_;
    }

    const Dimension& GetActiveIndicatorPadding() const
    {
        return activeIndicatorPadding_;
    }

    const Color& GetFocusIndicatorColor() const
    {
        return focusIndicatorColor_;
    }

    const Dimension& GetFocusIndicatorRadius() const
    {
        return focusIndicatorRadius_;
    }

    const Dimension& GetFocusIndicatorHorizontalPadding() const
    {
        return focusIndicatorHorizontalPadding_;
    }

    const Dimension& GetFocusIndicatorVerticalPadding() const
    {
        return focusIndicatorVerticalPadding_;
    }

protected:
    TabTheme() = default;

private:
    Dimension labelPadding_;
    Dimension padding_;
    Dimension gradientWidth_;
    Dimension defaultHeight_;
    Dimension defaultWidth_;
    Dimension defaultItemHeight_;
    Color backgroundColor_;
    Color activeIndicatorColor_;
    Dimension activeIndicatorWidth_;
    Dimension activeIndicatorMinWidth_;
    Dimension activeIndicatorPadding_;
    Color focusIndicatorColor_;
    Dimension focusIndicatorRadius_;
    Dimension focusIndicatorHorizontalPadding_;
    Dimension focusIndicatorVerticalPadding_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_THEME_H
