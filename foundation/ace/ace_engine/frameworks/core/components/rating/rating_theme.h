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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_RATING_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_RATING_THEME_H

#include "base/resource/internal_resource.h"
#include "core/components/common/properties/color.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/**
 * RatingTheme defines color and styles of RatingComponent. RatingTheme should be built
 * using RatingTheme::Builder.
 */
class RatingTheme : public virtual Theme {
    DECLARE_ACE_TYPE(RatingTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<RatingTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<RatingTheme> theme = AceType::Claim(new RatingTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->starNum_ = themeConstants->GetInt(THEME_RATING_STAR_NUM);
            theme->ratingScore_ = themeConstants->GetDouble(THEME_RATING_SCORE);
            theme->ratingMiniScore_ = themeConstants->GetDouble(THEME_RATING_MINI_SCORE);
            theme->stepSize_ = themeConstants->GetDouble(THEME_RATING_STEPSIZE);
            theme->paddingVertical_ = themeConstants->GetDimension(THEME_RATING_PADDING_VERTICAL);
            theme->ratingWidth_ = themeConstants->GetDimension(THEME_RATING_BIG_WIDTH);
            theme->ratingHeight_ = themeConstants->GetDimension(THEME_RATING_BIG_HEIGHT);
            theme->ratingMiniWidth_ = themeConstants->GetDimension(THEME_RATING_MINI_WIDTH);
            theme->ratingMiniHeight_ = themeConstants->GetDimension(THEME_RATING_MINI_HEIGHT);
            theme->foregroundResourceId_ = themeConstants->GetResourceId(THEME_RATING_RESOURCE_ID_BIG_ON);
            theme->secondaryResourceId_ = themeConstants->GetResourceId(THEME_RATING_RESOURCE_ID_BIG_HALF);
            theme->backgroundResourceId_ = themeConstants->GetResourceId(THEME_RATING_RESOURCE_ID_BIG_OFF);
            theme->foregroundMiniResourceId_ = themeConstants->GetResourceId(THEME_RATING_RESOURCE_ID_MINI_ON);
            theme->secondaryMiniResourceId_ = themeConstants->GetResourceId(THEME_RATING_RESOURCE_ID_MINI_HALF);
            theme->backgroundMiniResourceId_ = themeConstants->GetResourceId(THEME_RATING_RESOURCE_ID_MINI_OFF);
            theme->designedStarAspectRatio_ = themeConstants->GetDouble(THEME_RATING_DESIGNED_STAR_ASPECT_RATIO);
            theme->focusBorderWidth_ = themeConstants->GetDimension(THEME_RATING_FOCUS_BORDER_WIDTH);
            theme->focusBorderRadius_ = themeConstants->GetDimension(THEME_RATING_FOCUS_BORDER_RADIUS);
            theme->hoverColor_ = themeConstants->GetColor(THEME_RATING_HOVER_COLOR);
            theme->starColorActive_ = themeConstants->GetColor(THEME_RATING_STAR_COLOR_ACTIVE);
            theme->starColorInactive_ = themeConstants->GetColor(THEME_RATING_STAR_COLOR_INACTIVE);
            auto themeStyle = themeConstants->GetThemeStyle();
            if (themeStyle) {
                theme->hoverColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_HOVER, Color::RED);
                theme->starColorActive_ = themeStyle->GetAttr<Color>(THEME_ATTRPALETTE_COLOR11, Color::RED);
                theme->starColorInactive_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_COMPONENT_NORMAL, Color::RED);
            }
            return theme;
        }
    };

    ~RatingTheme() override = default;

    int32_t GetStarNum() const
    {
        return starNum_;
    }

    const Dimension& GetRatingWidth() const
    {
        return ratingWidth_;
    }

    const Dimension& GetRatingHeight() const
    {
        return ratingHeight_;
    }

    const Dimension& GetRatingMiniWidth() const
    {
        return ratingMiniWidth_;
    }

    const Dimension& GetRatingMiniHeight() const
    {
        return ratingMiniHeight_;
    }

    const Dimension& GetPaddingVertical() const
    {
        return paddingVertical_;
    }

    double GetStepSize() const
    {
        return stepSize_;
    }

    double GetRatingScore() const
    {
        return ratingScore_;
    }

    double GetRatingMiniScore() const
    {
        return ratingMiniScore_;
    }

    const InternalResource::ResourceId& GetForegroundResourceId() const
    {
        return foregroundResourceId_;
    }

    const InternalResource::ResourceId& GetSecondaryResourceId() const
    {
        return secondaryResourceId_;
    }

    const InternalResource::ResourceId& GetBackgroundResourceId() const
    {
        return backgroundResourceId_;
    }

    const InternalResource::ResourceId& GetForegroundMiniResourceId() const
    {
        return foregroundMiniResourceId_;
    }

    const InternalResource::ResourceId& GetSecondaryMiniResourceId() const
    {
        return secondaryMiniResourceId_;
    }

    const InternalResource::ResourceId& GetBackgroundMiniResourceId() const
    {
        return backgroundMiniResourceId_;
    }

    double GetDesignedStarAspectRatio() const
    {
        return designedStarAspectRatio_;
    }

    const Dimension& GetFocusBorderWidth() const
    {
        return focusBorderWidth_;
    }

    const Dimension& GetFocusBorderRadius() const
    {
        return focusBorderRadius_;
    }

    const Color& GetHoverColor() const
    {
        return hoverColor_;
    }

    const Color& GetStarColorActive() const
    {
        return starColorActive_;
    }

    const Color& GetStarColorInactive() const
    {
        return starColorInactive_;
    }

protected:
    RatingTheme() = default;

private:
    int32_t starNum_ = 0;
    Dimension ratingWidth_;
    Dimension ratingHeight_;
    Dimension ratingMiniWidth_;
    Dimension ratingMiniHeight_;
    Dimension paddingVertical_;
    double stepSize_ = 0.0;
    double ratingScore_ = 0.0;
    double ratingMiniScore_ = 0.0;
    double designedStarAspectRatio_ = 1.0;
    InternalResource::ResourceId foregroundResourceId_ = InternalResource::ResourceId::RATE_STAR_BIG_ON_SVG;
    InternalResource::ResourceId secondaryResourceId_ = InternalResource::ResourceId::RATE_STAR_BIG_OFF_SVG;
    InternalResource::ResourceId backgroundResourceId_ = InternalResource::ResourceId::RATE_STAR_BIG_OFF_SVG;
    InternalResource::ResourceId foregroundMiniResourceId_ = InternalResource::ResourceId::RATE_STAR_SMALL_ON_SVG;
    InternalResource::ResourceId secondaryMiniResourceId_ = InternalResource::ResourceId::RATE_STAR_SMALL_ON_SVG;
    InternalResource::ResourceId backgroundMiniResourceId_ = InternalResource::ResourceId::RATE_STAR_SMALL_OFF_SVG;

    // properties for phone platform
    Color hoverColor_;
    Color starColorActive_;
    Color starColorInactive_;
    Dimension focusBorderWidth_;
    Dimension focusBorderRadius_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_RATING_THEME_H
