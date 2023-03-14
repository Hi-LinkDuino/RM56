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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_RATING_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_RATING_COMPONENT_H

#include "base/resource/internal_resource.h"
#include "base/utils/utils.h"
#include "core/components/image/image_component.h"
#include "core/components/rating/rating_theme.h"
#include "core/pipeline/base/component_group.h"
#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/element.h"
#include "core/components_v2/common/common_def.h"

namespace OHOS::Ace {

class ACE_EXPORT RatingComponent : public RenderComponent {
    DECLARE_ACE_TYPE(RatingComponent, RenderComponent);

public:
    RatingComponent() = default;
    ~RatingComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void SetResIdFromTheme(const RefPtr<RatingTheme>& theme);
    void SetMiniResIdFromTheme(const RefPtr<RatingTheme>& theme);
    void SetThemeStyle(const RefPtr<RatingTheme>& theme);

    int32_t GetStarNum() const
    {
        return starNum_;
    }

    void SetStarNum(int32_t starNum)
    {
        starNum_ = starNum;
    }

    bool GetIndicator() const
    {
        return isIndicator_;
    }

    void SetIndicator(bool indicator)
    {
        isIndicator_ = indicator;
    }

    double GetRatingScore() const
    {
        return ratingScore_;
    }

    void SetRatingScore(double ratingScore)
    {
        ratingScore_ = ratingScore;
    }

    double GetStepSize() const
    {
        return stepSize_;
    }

    void SetStepSize(double stepSize)
    {
        stepSize_ = stepSize;
    }

    const Dimension& GetWidth() const
    {
        return width_;
    }

    void SetWidth(const Dimension& width)
    {
        width_ = width;
    }

    const Dimension& GetHeight() const
    {
        return height_;
    }

    void SetHeight(const Dimension& height)
    {
        height_ = height;
    }
    const Dimension& GetDefaultHeight() const
    {
        return defaultHeight_;
    }

    void SetDefaultHeight(const Dimension& defaultHeight)
    {
        defaultHeight_ = defaultHeight;
    }

    const std::string& GetForegroundSrc() const
    {
        return foregroundSrc_;
    }

    void SetForegroundSrc(const std::string& foregroundSrc)
    {
        foregroundSrc_ = foregroundSrc;
    }

    const std::string& GetSecondarySrc() const
    {
        return secondarySrc_;
    }

    void SetSecondarySrc(const std::string& secondarySrc)
    {
        secondarySrc_ = secondarySrc;
    }

    const std::string& GetBackgroundSrc() const
    {
        return backgroundSrc_;
    }

    void SetBackgroundSrc(const std::string& backgroundSrc)
    {
        backgroundSrc_ = backgroundSrc;
    }

    RefPtr<ImageComponent> GetForegroundImage() const
    {
        return foregroundImage_;
    }

    RefPtr<ImageComponent> GetSecondaryImage() const
    {
        return secondaryImage_;
    }

    RefPtr<ImageComponent> GetBackgroundImage() const
    {
        return backgroundImage_;
    }

    void SetChangeEventId(const EventMarker& changeEventId)
    {
        changeEventId_ = changeEventId;
    }

    const EventMarker& GetChangeEventId() const
    {
        return changeEventId_;
    }

    void SetPaddingHorizontal(const Dimension& paddingHorizontal)
    {
        paddingHorizontal_ = paddingHorizontal;
    }

    const Dimension& GetPaddingHorizontal() const
    {
        return paddingHorizontal_;
    }

    void SetPaddingVertical(const Dimension& paddingVertical)
    {
        paddingVertical_ = paddingVertical;
    }

    const Dimension& GetPaddingVertical() const
    {
        return paddingVertical_;
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

    void SetRtlFlip(bool rtlFlip)
    {
        rtlFlip_ = rtlFlip;
    }

    bool IsRtlFlip() const
    {
        return rtlFlip_;
    }

    void SetDesignedStarAspectRatio(double designedStarAspectRatio)
    {
        designedStarAspectRatio_ = designedStarAspectRatio;
    }

    double GetDesignedStarAspectRatio() const
    {
        return designedStarAspectRatio_;
    }

    void SetFocusBorderWidth(const Dimension& focusBorderWidth)
    {
        focusBorderWidth_ = focusBorderWidth;
    }

    const Dimension& GetFocusBorderWidth() const
    {
        return focusBorderWidth_;
    }

    void SetFocusBorderRadius(const Dimension& focusBorderRadius)
    {
        focusBorderRadius_ = focusBorderRadius;
    }

    const Dimension& GetFocusBorderRadius() const
    {
        return focusBorderRadius_;
    }

    void SetHoverColor(const Color& hoverColor)
    {
        hoverColor_ = hoverColor;
    }

    const Color& GetHoverColor() const
    {
        return hoverColor_;
    }

    void SetStarColorActive(const Color& starColorActive)
    {
        starColorActive_ = starColorActive;
    }

    const Color& GetStarColorActive() const
    {
        return starColorActive_;
    }

    void SetStarColorInactive(const Color& starColorInactive)
    {
        starColorInactive_ = starColorInactive;
    }

    const Color& GetStarColorInactive() const
    {
        return starColorInactive_;
    }

    ACE_DEFINE_COMPONENT_EVENT(OnChange, void(double));

private:
    int32_t starNum_ = 5;
    double ratingScore_ = 0.0;
    double stepSize_ = 0.5;
    double designedStarAspectRatio_ = 1.0;
    Dimension width_;
    Dimension height_;
    Dimension defaultHeight_;
    Dimension paddingHorizontal_;
    Dimension paddingVertical_;
    std::string foregroundSrc_;
    std::string secondarySrc_;
    std::string backgroundSrc_;
    InternalResource::ResourceId foregroundResourceId_ = InternalResource::ResourceId::RATE_STAR_BIG_ON_SVG;
    InternalResource::ResourceId secondaryResourceId_ = InternalResource::ResourceId::RATE_STAR_BIG_OFF_SVG;
    InternalResource::ResourceId backgroundResourceId_ = InternalResource::ResourceId::RATE_STAR_BIG_OFF_SVG;
    bool isIndicator_ = false;
    bool rtlFlip_ = true;
    RefPtr<ImageComponent> foregroundImage_ =
        AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::RATE_STAR_BIG_ON_SVG);
    RefPtr<ImageComponent> secondaryImage_ =
        AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::RATE_STAR_BIG_OFF_SVG);
    RefPtr<ImageComponent> backgroundImage_ =
        AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::RATE_STAR_BIG_OFF_SVG);
    EventMarker changeEventId_;
    Color starColorActive_;
    Color starColorInactive_;

    // properties for phone platform
    Color hoverColor_;
    Dimension focusBorderWidth_;
    Dimension focusBorderRadius_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_RATING_COMPONENT_H
