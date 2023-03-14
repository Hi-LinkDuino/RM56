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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_FIELD_TEXTFIELD_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_FIELD_TEXTFIELD_THEME_H

#include "base/geometry/dimension.h"
#include "core/components/common/properties/border.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/edge.h"
#include "core/components/common/properties/radius.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/**
 * TextFieldTheme defines color and styles of SliderComponent. TextFieldTheme should be built
 * using TextFieldTheme::Builder.
 */
class TextFieldTheme : public virtual Theme {
    DECLARE_ACE_TYPE(TextFieldTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<TextFieldTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<TextFieldTheme> theme = AceType::Claim(new TextFieldTheme());
            if (!themeConstants) {
                return theme;
            }
            // init theme from global data
            theme->padding_ = Edge(themeConstants->GetDimension(THEME_TEXTFIELD_PADDING_HORIZONTAL),
                themeConstants->GetDimension(THEME_TEXTFIELD_PADDING_VERTICAL),
                themeConstants->GetDimension(THEME_TEXTFIELD_PADDING_HORIZONTAL),
                themeConstants->GetDimension(THEME_TEXTFIELD_PADDING_VERTICAL));
            theme->height_ = themeConstants->GetDimension(THEME_TEXTFIELD_HEIGHT);
            theme->fontSize_ = themeConstants->GetDimension(THEME_TEXTFIELD_FONT_SIZE);
            theme->fontWeight_ = FontWeight(themeConstants->GetInt(THEME_TEXTFIELD_FONT_WEIGHT));
            theme->borderRadius_ = Radius(themeConstants->GetDimension(THEME_TEXTFIELD_BORDER_RADIUS));

            theme->bgColor_ = themeConstants->GetColor(THEME_TEXTFIELD_BACKGROUND_COLOR);
            theme->placeholderColor_ = themeConstants->GetColor(THEME_TEXTFIELD_PLACEHOLDER_COLOR);
            theme->textColor_ = themeConstants->GetColor(THEME_TEXTFIELD_TEXT_COLOR);
            theme->selectedColor_ = themeConstants->GetColor(THEME_TEXTFIELD_TEXT_SELECTED_COLOR);
            theme->hoverColor_ = themeConstants->GetColor(THEME_TEXTFIELD_HOVER_COLOR);
            theme->pressColor_ = themeConstants->GetColor(THEME_TEXTFIELD_PRESS_COLOR);

            theme->focusBgColor_ = themeConstants->GetColor(THEME_TEXTFIELD_FOCUS_BACKGROUND_COLOR);
            theme->focusPlaceholderColor_ = themeConstants->GetColor(THEME_TEXTFIELD_FOCUS_PLACEHOLDER_COLOR);
            theme->focusTextColor_ = themeConstants->GetColor(THEME_TEXTFIELD_FOCUS_TEXT_COLOR);

            theme->disableOpacityRatio_ = themeConstants->GetDouble(THEME_TEXTFIELD_DISABLE_OPACITY_RATIO);
            theme->disableTextColor_ = themeConstants->GetColor(THEME_TEXTFIELD_TEXT_COLOR_DISABLE);

            theme->overHideLength_ = themeConstants->GetDimension(THEME_TEXTFIELD_OVER_HIDE_LENGTH);

            theme->cursorColor_ = themeConstants->GetColor(THEME_TEXTFIELD_CURSOR_COLOR);
            theme->cursorRadius_ = themeConstants->GetDimension(THEME_TEXTFIELD_CURSOR_RADIUS);
            theme->needFade_ = static_cast<bool>(themeConstants->GetInt(THEME_TEXTFIELD_NEED_FADE));

            theme->iconSize_ = themeConstants->GetDimension(THEME_TEXTFIELD_ICON_SIZE);
            theme->iconHotZoneSize_ = themeConstants->GetDimension(THEME_TEXTFIELD_ICON_HOT_ZONE_SIZE);

            theme->showEllipsis_ = static_cast<bool>(themeConstants->GetInt(THEME_TEXTFIELD_SHOW_ELLIPSIS));

            theme->errorSpacing_ = themeConstants->GetDimension(THEME_TEXTFIELD_ERROR_SPACING);
            theme->errorIsInner_ = static_cast<bool>(themeConstants->GetInt(THEME_TEXTFIELD_ERROR_IS_INNER));
            theme->errorBorderWidth_ = themeConstants->GetDimension(THEME_TEXTFIELD_ERROR_BORDER_WIDTH);
            theme->errorBorderColor_ = themeConstants->GetColor(THEME_TEXTFIELD_ERROR_BORDER_COLOR);
            theme->errorTextStyle_.SetTextColor(themeConstants->GetColor(THEME_TEXTFIELD_ERROR_COLOR));
            theme->errorTextStyle_.SetFontSize(themeConstants->GetDimension(THEME_TEXTFIELD_ERROR_FONT_SIZE));
            theme->errorTextStyle_.SetFontWeight(FontWeight(themeConstants->GetInt(THEME_TEXTFIELD_ERROR_FONT_WEIGHT)));

            theme->countTextStyle_.SetTextColor(themeConstants->GetColor(THEME_TEXTFIELD_COUNT_TEXT_COLOR));
            theme->countTextStyle_.SetFontSize(themeConstants->GetDimension(THEME_TEXTFIELD_COUNT_FONT_SIZE));
            theme->countTextStyle_.SetFontWeight(
                FontWeight(themeConstants->GetInt(THEME_TEXTFIELD_COUNT_FONT_WEIGHT)));
            theme->overCountStyle_.SetTextColor(themeConstants->GetColor(THEME_TEXTFIELD_OVER_COUNT_TEXT_COLOR));
            theme->overCountStyle_.SetFontSize(themeConstants->GetDimension(THEME_TEXTFIELD_OVER_COUNT_FONT_SIZE));
            theme->overCountStyle_.SetFontWeight(
                FontWeight(themeConstants->GetInt(THEME_TEXTFIELD_OVER_COUNT_FONT_WEIGHT)));

            theme->countTextStyleOuter_.SetTextColor(themeConstants->GetColor(THEME_TEXTFIELD_COUNT_TEXT_COLOR));
            theme->countTextStyleOuter_.SetFontSize(
                themeConstants->GetDimension(THEME_TEXTFIELD_COUNT_FONT_SIZE_OUTER));
            theme->countTextStyleOuter_.SetFontWeight(
                FontWeight(themeConstants->GetInt(THEME_TEXTFIELD_COUNT_FONT_WEIGHT)));
            theme->overCountStyleOuter_.SetTextColor(themeConstants->GetColor(THEME_TEXTFIELD_OVER_COUNT_TEXT_COLOR));
            theme->overCountStyleOuter_.SetFontSize(
                themeConstants->GetDimension(THEME_TEXTFIELD_COUNT_FONT_SIZE_OUTER));
            theme->overCountStyleOuter_.SetFontWeight(
                FontWeight(themeConstants->GetInt(THEME_TEXTFIELD_OVER_COUNT_FONT_WEIGHT)));

            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }

    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<TextFieldTheme>& theme) const
        {
            if (!themeStyle || !theme) {
                return;
            }
            theme->fontSize_ = themeStyle->GetAttr<Dimension>(TEXTFIELD_FONT_SIZE, 0.0_fp);
            theme->textColor_ = themeStyle->GetAttr<Color>(TEXTFIELD_TEXT_COLOR, Color());
            theme->focusTextColor_ = themeStyle->GetAttr<Color>(TEXTFIELD_TEXT_COLOR, Color());
            theme->placeholderColor_ = themeStyle->GetAttr<Color>(TEXTFIELD_HINT_TEXT_COLOR, Color());
            theme->focusPlaceholderColor_ = themeStyle->GetAttr<Color>(TEXTFIELD_HINT_TEXT_COLOR, Color());
            theme->bgColor_ = themeStyle->GetAttr<Color>(TEXTFIELD_BACKGROUND_COLOR, Color());
            theme->focusBgColor_ = themeStyle->GetAttr<Color>(TEXTFIELD_BACKGROUND_COLOR, Color());
            // color of error border blend 60% opacity
            theme->errorBorderColor_ = themeStyle->GetAttr<Color>(TEXTFIELD_ERROR_COLOR, Color()).BlendOpacity(0.6);
            theme->errorTextStyle_.SetTextColor(themeStyle->GetAttr<Color>(TEXTFIELD_ERROR_COLOR, Color()));
            theme->errorTextStyle_.SetFontSize(themeStyle->GetAttr<Dimension>(TEXTFIELD_ERROR_TEXT_FONT_SIZE, 0.0_fp));

            theme->countTextStyle_.SetTextColor(themeStyle->GetAttr<Color>(TEXTFIELD_COUNT_TEXT_COLOR, Color()));
            theme->countTextStyle_.SetFontSize(themeStyle->GetAttr<Dimension>(TEXTFIELD_COUNT_TEXT_FONT_SIZE, 0.0_fp));
            theme->overCountStyle_.SetTextColor(themeStyle->GetAttr<Color>(TEXTFIELD_ERROR_COLOR, Color()));
            theme->overCountStyle_.SetFontSize(themeStyle->GetAttr<Dimension>(TEXTFIELD_COUNT_TEXT_FONT_SIZE, 0.0_fp));

            theme->countTextStyleOuter_.SetTextColor(themeStyle->GetAttr<Color>(TEXTFIELD_COUNT_TEXT_COLOR, Color()));
            theme->countTextStyleOuter_.SetFontSize(
                themeStyle->GetAttr<Dimension>(TEXTFIELD_COUNT_TEXT_FONT_SIZE_OUTER, 0.0_fp));
            theme->overCountStyleOuter_.SetTextColor(themeStyle->GetAttr<Color>(TEXTFIELD_ERROR_COLOR, Color()));
            theme->overCountStyleOuter_.SetFontSize(
                themeStyle->GetAttr<Dimension>(TEXTFIELD_COUNT_TEXT_FONT_SIZE_OUTER, 0.0_fp));

            theme->selectedColor_ = themeStyle->GetAttr<Color>(TEXTFIELD_SELECTED_COLOR, Color());
            theme->disableTextColor_ = themeStyle->GetAttr<Color>(TEXTFIELD_TEXT_COLOR_DISABLE, Color());
            theme->cursorColor_ = themeStyle->GetAttr<Color>(TEXTFIELD_CURSOR_COLOR, Color());
            theme->hoverColor_ = themeStyle->GetAttr<Color>(TEXTFIELD_HOVER_COLOR, Color());
            theme->pressColor_ = themeStyle->GetAttr<Color>(TEXTFIELD_PRESS_COLOR, Color());
        }
    };

    ~TextFieldTheme() override = default;

    const Edge& GetPadding() const
    {
        return padding_;
    }

    const Dimension& GetHeight() const
    {
        return height_;
    }

    const Dimension& GetFontSize() const
    {
        return fontSize_;
    }

    const FontWeight& GetFontWeight() const
    {
        return fontWeight_;
    }

    const Radius& GetBorderRadius() const
    {
        return borderRadius_;
    }

    const Color& GetBgColor() const
    {
        return bgColor_;
    }

    const Color& GetPlaceholderColor() const
    {
        return placeholderColor_;
    }

    const Color& GetFocusBgColor() const
    {
        return focusBgColor_;
    }

    const Color& GetFocusPlaceholderColor() const
    {
        return focusPlaceholderColor_;
    }

    const Color& GetFocusTextColor() const
    {
        return focusTextColor_;
    }

    const Color& GetTextColor() const
    {
        return textColor_;
    }

    const Color& GetDisableTextColor() const
    {
        return disableTextColor_;
    }

    const Color& GetSelectedColor() const
    {
        return selectedColor_;
    }

    const Color& GetHoverColor() const
    {
        return hoverColor_;
    }

    const Color& GetPressColor() const
    {
        return pressColor_;
    }

    double GetDisableOpacityRatio() const
    {
        return disableOpacityRatio_;
    }

    const Dimension& GetOverHideLength() const
    {
        return overHideLength_;
    }

    const Color& GetCursorColor() const
    {
        return cursorColor_;
    }

    const Dimension& GetCursorRadius() const
    {
        return cursorRadius_;
    }

    bool NeedFade() const
    {
        return needFade_;
    }

    const Dimension& GetIconSize() const
    {
        return iconSize_;
    }

    const Dimension& GetIconHotZoneSize() const
    {
        return iconHotZoneSize_;
    }

    bool ShowEllipsis() const
    {
        return showEllipsis_;
    }

    const Dimension& GetErrorSpacing() const
    {
        return errorSpacing_;
    }

    bool GetErrorIsInner() const
    {
        return errorIsInner_;
    }

    const Dimension& GetErrorBorderWidth() const
    {
        return errorBorderWidth_;
    }

    const Color& GetErrorBorderColor() const
    {
        return errorBorderColor_;
    }

    const TextStyle& GetErrorTextStyle() const
    {
        return errorTextStyle_;
    }

    const TextStyle& GetCountTextStyle() const
    {
        return countTextStyle_;
    }

    const TextStyle& GetOverCountStyle() const
    {
        return overCountStyle_;
    }

    const TextStyle& GetCountTextStyleOuter() const
    {
        return countTextStyleOuter_;
    }

    const TextStyle& GetOverCountStyleOuter() const
    {
        return overCountStyleOuter_;
    }

protected:
    TextFieldTheme() = default;

private:
    Edge padding_;
    Dimension height_;
    Dimension fontSize_;
    FontWeight fontWeight_ = FontWeight::NORMAL;
    Radius borderRadius_;

    Color bgColor_;
    Color placeholderColor_;
    Color focusBgColor_;
    Color focusPlaceholderColor_;
    Color focusTextColor_;
    Color textColor_;
    Color disableTextColor_;
    Color selectedColor_;
    Color hoverColor_;
    Color pressColor_;
    Dimension errorSpacing_;
    bool errorIsInner_ = false;
    Dimension errorBorderWidth_;
    Color errorBorderColor_;
    TextStyle errorTextStyle_;
    TextStyle countTextStyle_;
    TextStyle overCountStyle_;
    TextStyle countTextStyleOuter_;
    TextStyle overCountStyleOuter_;

    // UX::disable state: opacity is set to 38% of the default
    double disableOpacityRatio_ = 1.0;

    // UX::over length state
    Dimension overHideLength_;

    // UX::cursor state cursor-color=#000000, cursor blur-radius=0.9, cursor-width=2, cursor-height=24, cursor-radius=1
    Color cursorColor_;
    Dimension cursorRadius_;
    bool needFade_ = false;

    // UX::icon size = 24, hotZoneSize = 36
    Dimension iconSize_;
    Dimension iconHotZoneSize_;

    bool showEllipsis_ = true;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_FIELD_TEXTFIELD_THEME_H
