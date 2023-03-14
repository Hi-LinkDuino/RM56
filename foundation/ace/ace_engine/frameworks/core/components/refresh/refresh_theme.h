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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_REFRESH_REFRESH_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_REFRESH_REFRESH_THEME_H

#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/**
 * RefreshTheme should be built using RefreshTheme::Builder.
 */
class RefreshTheme : public virtual Theme {
    DECLARE_ACE_TYPE(RefreshTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<RefreshTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<RefreshTheme> theme = AceType::Claim(new RefreshTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->loadingDistance_ = themeConstants->GetDimension(THEME_REFRESH_DEFAULT_LOADING_DISTANCE);
            theme->refreshDistance_ = themeConstants->GetDimension(THEME_REFRESH_DEFAULT_REFRESHING_DISTANCE);
            theme->maxDistance_ = themeConstants->GetDimension(THEME_REFRESH_DEFAULT_MAX_DISTANCE);
            theme->progressDistance_ = themeConstants->GetDimension(THEME_REFRESH_DEFAULT_PROGRESS_DISTANCE);
            theme->progressDiameter_ = themeConstants->GetDimension(THEME_REFRESH_DEFAULT_PROGRESS_DIAMETER);
            theme->showTimeDistance_ = themeConstants->GetDimension(THEME_REFRESH_DEFAULT_SHOWTIME_DISTANCE);
            theme->textStyle_.SetFontSize(themeConstants->GetDimension(THEME_REFRESH_DEFAULT_FONTSIZE));
            theme->textStyle_.SetTextColor(themeConstants->GetColor(THEME_REFRESH_DEFAULT_TEXT_COLOR));
            theme->textStyle_.SetFontWeight(FontWeight(themeConstants->GetInt(THEME_TEXTFIELD_FONT_WEIGHT)));
            theme->progressColor_ = themeConstants->GetColor(THEME_REFRESH_DEFAULT_PROGRESS_COLOR);
            theme->backgroundColor_ = themeConstants->GetColor(THEME_REFRESH_DEFAULT_BACKGROUND_COLOR);
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }
    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<RefreshTheme>& theme) const
        {
            if (!themeStyle) {
                return;
            }
            theme->textStyle_.SetFontSize(themeStyle->GetAttr<Dimension>(THEME_ATTR_TEXT_SIZE_BODY2, 0.0_vp));
            theme->textStyle_.SetTextColor(themeStyle->GetAttr<Color>(THEME_ATTR_TEXT_COLOR_SECONDARY, Color::BLACK));
            theme->progressColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_PROGRESS, Color::BLACK);
            theme->backgroundColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_BG_COLOR, Color::WHITE);
        }
    };

    ~RefreshTheme() override = default;

    const Dimension& GetLoadingDistance() const
    {
        return loadingDistance_;
    }

    const Dimension& GetRefreshDistance() const
    {
        return refreshDistance_;
    }

    const Dimension& GetProgressDistance() const
    {
        return progressDistance_;
    }

    const Dimension& GetShowTimeDistance() const
    {
        return showTimeDistance_;
    }

    const Dimension& GetMaxDistance() const
    {
        return maxDistance_;
    }

    const Dimension& GetProgressDiameter() const
    {
        return progressDiameter_;
    }

    const TextStyle& GetTextStyle() const
    {
        return textStyle_;
    }

    const Color& GetProgressColor() const
    {
        return progressColor_;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

protected:
    RefreshTheme() = default;

private:
    Dimension loadingDistance_;
    Dimension progressDistance_;
    Dimension refreshDistance_;
    Dimension maxDistance_;
    Dimension progressDiameter_;
    Dimension showTimeDistance_;
    TextStyle textStyle_;
    Color progressColor_;
    Color backgroundColor_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_REFRESH_REFRESH_THEME_H
