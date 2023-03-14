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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_BAR_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_BAR_THEME_H

#include "core/components/common/properties/scroll_bar.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/**
 * ScrollBarTheme defines styles of scrollBar. ScrollBarTheme should be built
 * using ScrollBarTheme::Builder.
 */
class ScrollBarTheme : public virtual Theme {
    DECLARE_ACE_TYPE(ScrollBarTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<ScrollBarTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<ScrollBarTheme> theme = AceType::Claim(new ScrollBarTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->shapeMode_ = ShapeMode(themeConstants->GetInt(THEME_SCROLL_BAR_SHAPE_MODE));
            theme->normalWidth_ = themeConstants->GetDimension(THEME_SCROLL_BAR_NORMAL_WIDTH);
            theme->activeWidth_ = themeConstants->GetDimension(THEME_SCROLL_BAR_ACTIVE_WIDTH);
            theme->minHeight_ = themeConstants->GetDimension(THEME_SCROLL_BAR_MIN_HEIGHT);
            theme->minDynamicHeight_ = themeConstants->GetDimension(THEME_SCROLL_BAR_MIN_DYNAMIC_HEIGHT);
            theme->reservedHeight_ = themeConstants->GetDimension(THEME_SCROLL_BAR_RESERVED_HEIGHT);
            theme->touchWidth_ = themeConstants->GetDimension(THEME_SCROLL_BAR_TOUCH_WIDTH);
            theme->backgroundColor_ = themeConstants->GetColor(THEME_SCROLL_BAR_BACKGROUND_COLOR);
            theme->foregroundColor_ = themeConstants->GetColor(THEME_SCROLL_BAR_FOREGROUND_COLOR);
            Dimension paddingRight = themeConstants->GetDimension(THEME_SCROLL_BAR_PADDING_RIGHT);
            theme->padding_ = Edge(0.0, 0.0, paddingRight.Value(), 0.0, paddingRight.Unit());
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }

    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<ScrollBarTheme>& theme) const
        {
            if (!themeStyle) {
                LOGI("scroll bar theme style is null");
                return;
            }
            theme->foregroundColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_FOREGROUND,
                Color::TRANSPARENT).BlendOpacity(0.4);
        }
    };

    ~ScrollBarTheme() override = default;

    const Dimension& GetNormalWidth() const
    {
        return normalWidth_;
    }

    const Dimension& GetActiveWidth() const
    {
        return activeWidth_;
    }

    const Dimension& GetMinHeight() const
    {
        return minHeight_;
    }

    const Dimension& GetMinDynamicHeight() const
    {
        return minDynamicHeight_;
    }

    const Dimension& GetReservedHeight() const
    {
        return reservedHeight_;
    }

    const Dimension& GetTouchWidth() const
    {
        return touchWidth_;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    const Color& GetForegroundColor() const
    {
        return foregroundColor_;
    }

    ShapeMode GetShapeMode() const
    {
        return shapeMode_;
    }

    const Edge& GetPadding() const
    {
        return padding_;
    }

protected:
    ScrollBarTheme() = default;

private:
    ShapeMode shapeMode_ = ShapeMode::DEFAULT;
    Dimension normalWidth_;
    Dimension activeWidth_;
    Dimension minHeight_;
    Dimension minDynamicHeight_;
    Dimension reservedHeight_;
    Dimension touchWidth_;
    Color backgroundColor_;
    Color foregroundColor_;
    Edge padding_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_BAR_THEME_H
