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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_PROGRESS_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_PROGRESS_THEME_H

#include "base/geometry/dimension.h"
#include "core/components/common/properties/color.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/**
 * ProgressTheme defines color and styles of ProgressComponent. ProgressTheme should be built
 * using ProgressTheme::Builder.
 */
class ProgressTheme : public virtual Theme {
    DECLARE_ACE_TYPE(ProgressTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<ProgressTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<ProgressTheme> theme = AceType::Claim(new ProgressTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->trackThickness_ = themeConstants->GetDimension(THEME_PROGRERSS_THICKNESS);
            theme->trackWidth_ = themeConstants->GetDimension(THEME_PROGRESS_DEFAULT_WIDTH);
            theme->ringThickness_ = themeConstants->GetDimension(THEME_PROGRESS_RING_THICKNESS);
            theme->ringDiameter_ = themeConstants->GetDimension(THEME_PROGRESS_DEFAULT_DIAMETER);
            theme->trackBgColor_ = themeConstants->GetColor(THEME_PROGRESS_BG_COLOR);
            theme->trackSelectedColor_ = themeConstants->GetColor(THEME_PROGRESS_COLOR);
            theme->trackCachedColor_ = themeConstants->GetColor(THEME_PROGRESS_CACHED_COLOR);
            theme->loadingColor_ = themeConstants->GetColor(THEME_LOADING_COLOR);
            theme->loadingDiameter_ = themeConstants->GetDimension(THEME_PROGRESS_DEFAULT_DIAMETER);
            theme->scaleNumber_ = themeConstants->GetInt(THEME_PROGRESS_SCALE_NUMBER);
            theme->scaleWidth_ = themeConstants->GetDimension(THEME_PROGRESS_SCALE_WIDTH);
            theme->scaleLength_ = themeConstants->GetDimension(THEME_PROGRESS_STROKE_WIDTH);
            theme->scaleRingDiameter_ = themeConstants->GetDimension(THEME_SCALE_PROGRESS_DEFAULT_DIAMETER);

            // For moon progress
            theme->moonDiameter_ = themeConstants->GetDimension(THEME_MOON_PROGRESS_DIAMETER);
            theme->moonTrackBackgroundColor_ = themeConstants->GetColor(THEME_MOON_BACKGROUDN_COLOR);
            theme->moonFrontColor_ = themeConstants->GetColor(THEME_MOON_FRONT_COLOR);

            // For loading progress in cycle type.
            theme->progressColor_ = themeConstants->GetColor(THEME_LOADING_PROGRESS_COLOR);
            theme->moveRatio_ = themeConstants->GetDouble(THEME_LOADING_PROGRESS_MOVE_RATIO);
            theme->ringRadius_ = themeConstants->GetDimension(THEME_LOADING_PROGRESS_RING_RADIUS);
            theme->orbitRadius_ = themeConstants->GetDimension(THEME_LOADING_PROGRESS_ORBIT_RADIUS);
            theme->cometTailLen_ = themeConstants->GetDouble(THEME_LOADING_PROGRESS_COMET_TAIL_LEN);

            theme->bubbleRadius_ = themeConstants->GetDimension(THEME_BUBBLE_PROGRESS_RADIUS);
            theme->bubbleDiameter_ = themeConstants->GetDimension(THEME_BUBBLE_PROGRESS_DIAMETER);
            // Read style from system.
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }

        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<ProgressTheme>& theme) const
        {
            if (!themeStyle) {
                LOGI("progress theme style is null");
                return;
            }
            theme->trackBgColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_COMPONENT_NORMAL, Color::RED);
            theme->trackSelectedColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_EMPHASIZE, Color::RED);
            theme->trackCachedColor_ =
                themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_EMPHASIZE, Color::RED).BlendOpacity(0.4);
            theme->loadingColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_PROGRESS, Color::RED).BlendOpacity(0.6);
            theme->progressColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_PROGRESS, Color::RED);
            theme->moonFrontColor_ =
                themeStyle->GetAttr<Color>(TEXT_OVERLAY_HANDLE_COLOR, Color::RED)
                    .BlendOpacity(themeStyle->GetAttr<double>(THEME_ATTR_HIGHLIGHT_BACKGROUND_ALPHA, 1.0));
            theme->moonTrackBackgroundColor_ =
                themeStyle->GetAttr<Color>(TEXT_OVERLAY_HANDLE_COLOR, Color::RED)
                    .BlendOpacity(themeStyle->GetAttr<double>(THEME_ATTR_HIGHLIGHT_BACKGROUND_ALPHA, 1.0))
                    .BlendOpacity(themeStyle->GetAttr<double>(THEME_ATTR_DISABLED_ALPHA, 1.0));
        }
    };

    ~ProgressTheme() override = default;

    const Dimension& GetTrackThickness() const
    {
        return trackThickness_;
    }

    const Dimension& GetTrackWidth() const
    {
        return trackWidth_;
    }

    const Dimension& GetRingThickness() const
    {
        return ringThickness_;
    }

    const Dimension& GetRingDiameter() const
    {
        return ringDiameter_;
    }

    const Color& GetTrackBgColor() const
    {
        return trackBgColor_;
    }

    const Color& GetTrackSelectedColor() const
    {
        return trackSelectedColor_;
    }

    Color GetTrackCachedColor() const
    {
        return trackCachedColor_;
    }

    const Dimension& GetLoadingDiameter() const
    {
        return loadingDiameter_;
    }

    const Color& GetLoadingColor() const
    {
        return loadingColor_;
    }

    const Dimension& GetScaleWidth() const
    {
        return scaleWidth_;
    }

    int32_t GetScaleNumber() const
    {
        return scaleNumber_;
    }

    const Dimension& GetScaleLength() const
    {
        return scaleLength_;
    }

    const Color& GetProgressColor() const
    {
        return progressColor_;
    }

    double GetMoveRatio() const
    {
        return moveRatio_;
    }

    const Dimension& GetRingRadius() const
    {
        return ringRadius_;
    }

    const Dimension& GetOrbitRadius() const
    {
        return orbitRadius_;
    }

    double GetCometTailLen() const
    {
        return cometTailLen_;
    }

    const Dimension& GetScaleRingDiameter() const
    {
        return scaleRingDiameter_;
    }

    const Dimension& GetMoonDiameter() const
    {
        return moonDiameter_;
    }

    const Color& GetMoonBackgroundColor() const
    {
        return moonTrackBackgroundColor_;
    }

    const Color& GetMoonFrontColor() const
    {
        return moonFrontColor_;
    }

    const Dimension& GetBubbleDiameter() const
    {
        return bubbleDiameter_;
    }

    const Dimension& GetBubbleRadius() const
    {
        return bubbleRadius_;
    }

protected:
    ProgressTheme() = default;

private:
    Dimension trackThickness_;
    Dimension trackWidth_;
    Color trackBgColor_;
    Color trackSelectedColor_;
    Color trackCachedColor_;

    Dimension ringThickness_;
    Dimension ringDiameter_;
    Dimension bubbleDiameter_;
    Dimension bubbleRadius_;

    Dimension loadingDiameter_;
    Color loadingColor_;

    Dimension scaleWidth_;
    int32_t scaleNumber_ = 0;
    Dimension scaleLength_;
    Dimension scaleRingDiameter_;

    Dimension moonDiameter_;
    Color moonTrackBackgroundColor_;
    Color moonFrontColor_;

    // For loading progress in cycle type.
    Color progressColor_;
    double moveRatio_ = 0.0;
    Dimension ringRadius_;
    Dimension orbitRadius_;
    double cometTailLen_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_PROGRESS_THEME_H
