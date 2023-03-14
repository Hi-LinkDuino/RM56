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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOAST_TOAST_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOAST_TOAST_THEME_H

#include "core/components/common/properties/color.h"
#include "core/components/common/properties/edge.h"
#include "core/components/common/properties/text_style.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/**
 * ToastTheme defines color and styles of Toast. ToastTheme should be built
 * using ToastTheme::Builder.
 */
class ToastTheme : public virtual Theme {
    DECLARE_ACE_TYPE(ToastTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<ToastTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<ToastTheme> theme = AceType::Claim(new ToastTheme());
            if (!themeConstants) {
                return theme;
            }
            // init theme from global data
            theme->padding_ = Edge(themeConstants->GetDimension(THEME_TOAST_PADDING_HORIZONTAL).Value(),
                themeConstants->GetDimension(THEME_TOAST_PADDING_VERTICAL).Value(),
                themeConstants->GetDimension(THEME_TOAST_PADDING_HORIZONTAL).Value(),
                themeConstants->GetDimension(THEME_TOAST_PADDING_VERTICAL).Value(),
                themeConstants->GetDimension(THEME_TOAST_PADDING_VERTICAL).Unit());
            theme->maxWidth_ = themeConstants->GetDimension(THEME_TOAST_CONTENT_MAX_WIDTH);
            theme->minWidth_ = themeConstants->GetDimension(THEME_TOAST_CONTENT_MIN_WIDTH);
            theme->minHeight_ = themeConstants->GetDimension(THEME_TOAST_CONTENT_MIN_HEIGHT);
            theme->backgroundColor_ = themeConstants->GetColor(THEME_TOAST_BACKGROUND_COLOR);
            theme->textStyle_.SetFontWeight(FontWeight(themeConstants->GetInt(THEME_TOAST_TEXT_TEXT_FONTWEIGHT)));
            theme->textStyle_.SetTextColor(themeConstants->GetColor(THEME_TOAST_TEXT_COLOR));
            theme->textStyle_.SetFontSize(themeConstants->GetDimension(THEME_TOAST_TEXT_FONTSIZE));
            theme->radius_ = Radius(
                themeConstants->GetDimension(THEME_TOAST_RADIUS), themeConstants->GetDimension(THEME_TOAST_RADIUS));
            theme->bottom_ = themeConstants->GetDimension(THEME_TOAST_BOTTOM);
            theme->marging_ = Edge(themeConstants->GetDimension(THEME_TOAST_MARGIN).Value(), 0.0,
                themeConstants->GetDimension(THEME_TOAST_MARGIN).Value(), 0.0,
                themeConstants->GetDimension(THEME_TOAST_MARGIN).Unit());
            theme->minFontSize_ = themeConstants->GetDimension(THEME_TOAST_TEXT_FONTSIZE_MIN);
            auto textMaxLines = themeConstants->GetInt(THEME_TOAST_TEXT_MAX_LINES);
            theme->textMaxLines_ = textMaxLines < 0 ? theme->textMaxLines_ : static_cast<uint32_t>(textMaxLines);
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }
    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<ToastTheme>& theme) const
        {
            if (!themeStyle) {
                return;
            }
            auto toastPattern = themeStyle->GetAttr<RefPtr<ThemeStyle>>(THEME_PATTERN_TOAST, nullptr);
            if (!toastPattern) {
                return;
            }
            theme->textStyle_.SetFontSize(toastPattern->GetAttr<Dimension>(TOAST_FONT_SIZE, 0.0_vp));
            theme->textStyle_.SetTextColor(toastPattern->GetAttr<Color>(TOAST_TEXT_COLOR, Color()));
            theme->backgroundColor_ = toastPattern->GetAttr<Color>(TOAST_BACKGROUND_COLOR, Color());
        }
    };

    ~ToastTheme() override = default;

    const Edge& GetPadding() const
    {
        return padding_;
    }

    const Dimension& GetMaxWidth() const
    {
        return maxWidth_;
    }

    const Dimension& GetMinWidth() const
    {
        return minWidth_;
    }

    const Dimension& GetMinHeight() const
    {
        return minHeight_;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    const TextStyle& GetTextStyle() const
    {
        return textStyle_;
    }

    const Radius& GetRadius() const
    {
        return radius_;
    }

    const Dimension& GetBottom() const
    {
        return bottom_;
    }

    const Dimension& GetMinFontSize() const
    {
        return minFontSize_;
    }

    uint32_t GetTextMaxLines() const
    {
        return textMaxLines_;
    }

    const Edge& GetMarging() const
    {
        return marging_;
    }

protected:
    ToastTheme() = default;

private:
    Edge padding_;
    Dimension maxWidth_;
    Dimension minWidth_;
    Dimension minHeight_;
    Color backgroundColor_;
    TextStyle textStyle_;
    Radius radius_;
    Dimension bottom_;
    Dimension minFontSize_;
    uint32_t textMaxLines_ = 1;
    Edge marging_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOAST_TOAST_THEME_H
