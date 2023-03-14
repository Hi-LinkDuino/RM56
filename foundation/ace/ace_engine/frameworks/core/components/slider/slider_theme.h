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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_SLIDER_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_SLIDER_THEME_H

#include "base/geometry/dimension.h"
#include "core/components/common/properties/color.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/**
 * SliderTheme defines color and styles of SliderComponent. SliderTheme should be built
 * using SliderTheme::Builder.
 */
class SliderTheme : public virtual Theme {
    DECLARE_ACE_TYPE(SliderTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<SliderTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<SliderTheme> theme = AceType::Claim(new SliderTheme());
            if (!themeConstants) {
                return theme;
            }
            // init theme from global data
            theme->outsetBlockSize_ = themeConstants->GetDimension(THEME_SLIDER_OUTSET_BLOCK_SIZE);
            theme->outsetBlockHotSize_ = themeConstants->GetDimension(THEME_SLIDER_OUTSET_BLOCK_HOT_REGION_SIZE);
            theme->blockColor_ = themeConstants->GetColor(THEME_SLIDER_BLOCK_COLOR);
            theme->outsetTrackThickness_ = themeConstants->GetDimension(THEME_SLIDER_OUTSET_TRACK_THICKNESS);
            theme->insetTrackThickness_ = themeConstants->GetDimension(THEME_SLIDER_INSET_TRACK_THICKNESS);
            theme->trackSelectedColor_ = themeConstants->GetColor(THEME_SLIDER_TRACK_SELECTED);
            theme->trackBgColor_ = themeConstants->GetColor(THEME_SLIDER_TRACK_BG);
            theme->insetBlockSize_ = themeConstants->GetDimension(THEME_SLIDER_INSET_BLOCK_SIZE);
            theme->insetBlockHotSize_ = themeConstants->GetDimension(THEME_SLIDER_INSET_BLOCK_HOT_REGION_SIZE);
            theme->markerSize_ = themeConstants->GetDimension(THEME_SLIDER_MARKER_SIZE);
            theme->markerColor_ = themeConstants->GetColor(THEME_SLIDER_MARKER_COLOR);
            theme->tipColor_ = themeConstants->GetColor(THEME_SLIDER_TIP_COLOR);
            theme->tipTextColor_ = themeConstants->GetColor(THEME_SLIDER_TIP_TEXT_COLOR);
            theme->tipFontSize_ = themeConstants->GetDimension(THEME_SLIDER_TIP_FONT_SIZE);
            theme->tipTextPadding_ = themeConstants->GetDimension(THEME_SLIDER_TIP_TEXT_PADDING_SIZE);
            theme->blockHoverColor_ = themeConstants->GetColor(THEME_SLIDER_BLOCK_HOVER_COLOR);
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }

        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<SliderTheme>& theme) const
        {
            if (!themeStyle) {
                LOGI("progress theme style is null");
                return;
            }
            theme->trackBgColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_COMPONENT_NORMAL, Color::RED);
            theme->trackSelectedColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_EMPHASIZE, Color::RED);
            theme->markerColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_FOREGROUND, Color::RED).BlendOpacity(0.1);
            theme->tipTextColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_TEXT_COLOR_PRIMARY_INVERSE, Color::RED);
            theme->tipColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_TIPS_BG, Color::RED);
            theme->blockHoverColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_HOVER, Color::RED);
        }
    };

    ~SliderTheme() override = default;

    Dimension GetOutsetBlockSize() const
    {
        return outsetBlockSize_;
    }

    Dimension GetOutsetBlockHotSize() const
    {
        return outsetBlockHotSize_;
    }

    Dimension GetInsetBlockSize() const
    {
        return insetBlockSize_;
    }

    Dimension GetInsetBlockHotSize() const
    {
        return insetBlockHotSize_;
    }

    Color GetBlockHoverColor() const
    {
        return blockHoverColor_;
    }

    Color GetBlockColor() const
    {
        return blockColor_;
    }

    Dimension GetInsetTrackThickness() const
    {
        return insetTrackThickness_;
    }

    Dimension GetOutsetTrackThickness() const
    {
        return outsetTrackThickness_;
    }

    Dimension GetMarkerSize() const
    {
        return markerSize_;
    }

    Dimension GetTipFontSize() const
    {
        return tipFontSize_;
    }

    Dimension GetTipTextPadding() const
    {
        return tipTextPadding_;
    }

    Color GetTipColor() const
    {
        return tipColor_;
    }

    Color GetTipTextColor() const
    {
        return tipTextColor_;
    }

    Color GetMarkerColor() const
    {
        return markerColor_;
    }

    Color GetTrackBgColor() const
    {
        return trackBgColor_;
    }

    Color GetTrackSelectedColor() const
    {
        return trackSelectedColor_;
    }

protected:
    SliderTheme() = default;

private:
    // outset slider mode
    Dimension outsetBlockSize_;
    Dimension outsetBlockHotSize_;
    Dimension outsetTrackThickness_;

    // inset slide mode
    Dimension insetBlockSize_;
    Dimension insetBlockHotSize_;
    Dimension insetTrackThickness_;

    // common
    Dimension markerSize_;
    Dimension tipFontSize_;
    Dimension tipTextPadding_;
    Color blockColor_;
    Color blockHoverColor_;
    Color tipColor_;
    Color tipTextColor_;
    Color markerColor_;
    Color trackBgColor_;
    Color trackSelectedColor_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_SLIDER_THEME_H
