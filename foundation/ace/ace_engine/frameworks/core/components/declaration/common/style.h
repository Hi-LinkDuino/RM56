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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_STYLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_STYLE_H

#include "base/geometry/dimension.h"
#include "base/geometry/calc_dimension.h"

#include "core/animation/animation_pub.h"
#include "core/animation/shared_transition_effect.h"
#include "core/components/common/properties/border.h"
#include "core/components/common/properties/clip_path.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/common/properties/radius.h"
#include "core/components/common/properties/shadow.h"
#include "core/components/common/properties/tween_option.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

enum class StyleTag {
    COMMON_STYLE = 0,
    COMMON_SIZE_STYLE,
    COMMON_MARGIN_STYLE,
    COMMON_PADDING_STYLE,
    COMMON_BORDER_STYLE,
    COMMON_BACKGROUND_STYLE,
    COMMON_FLEX_STYLE,
    COMMON_POSITION_STYLE,
    COMMON_OPACITY_STYLE,
    COMMON_VISIBILITY_STYLE,
    COMMON_DISPLAY_STYLE,
    COMMON_SHADOW_STYLE,
    COMMON_OVERFLOW_STYLE,
    COMMON_FILTER_STYLE,
    COMMON_ANIMATION_STYLE,
    COMMON_SHARE_TRANSITION_STYLE,
    COMMON_CARD_TRANSITION_STYLE,
    COMMON_PAGE_TRANSITION_STYLE,
    COMMON_CLIP_PATH_STYLE,
    COMMON_MASK_STYLE,
    COMMON_IMAGE_STYLE,
    COMMON_PATTERN_STYLE,
    SPECIALIZED_STYLE,
    UNKNOWN,
    DEFAULT,
};

struct Style {
    bool IsValid() const
    {
        return tag != StyleTag::UNKNOWN;
    }

    bool IsShared() const
    {
        return isShared;
    }

    bool isShared = true;
    StyleTag tag = StyleTag::DEFAULT;
};

struct CommonStyle : Style {
    std::string alignSelf;
    bool layoutInBox = false;
    int32_t zIndex = 0;
};

struct CommonSizeStyle : Style {
    CalcDimension width = Dimension(-1.0, DimensionUnit::PX);
    CalcDimension height = Dimension(-1.0, DimensionUnit::PX);
    CalcDimension minWidth = Dimension(0.0);
    CalcDimension minHeight = Dimension(0.0);
    CalcDimension maxWidth = Dimension(Size::INFINITE_SIZE);
    CalcDimension maxHeight = Dimension(Size::INFINITE_SIZE);
    double aspectRatio = 0.0;
    BoxSizing boxSizing = BoxSizing::BORDER_BOX;
};

struct CommonMarginStyle : Style {
    Edge margin;
};

struct CommonPaddingStyle : Style {
    Edge padding;
};

struct CommonBorderStyle : Style {
    Border border;
};

struct CommonFlexStyle : Style {
    double flex = 0.0;
    double flexGrow = 0.0;
    double flexShrink = 1.0;
    Dimension flexBasis = 0.0_px;
    double flexWeight = 0.0;
    int32_t displayIndex = 1;
};

struct CommonPositionStyle : Style {
    PositionType position { PositionType::RELATIVE };
    Dimension left;
    Dimension top;
    Dimension right;
    Dimension bottom;
};

struct CommonBackgroundStyle : Style {
    Gradient gradient;
    Gradient gradientBorderImage;
    Color backgroundColor;
    std::string backgroundSize;
    RefPtr<BackgroundImage> backgroundImage;
    RefPtr<BorderImage> borderImage;
    ImageRepeat backgroundRepeat;
    std::string backgroundPosition;
};

struct CommonShadowStyle : Style {
    Shadow shadow;
};

struct CommonClipPathStyle : Style {
    RefPtr<ClipPath> clipPath;
};

struct CommonOverflowStyle : Style {
    Overflow overflow = Overflow::OBSERVABLE;
    std::pair<bool, Color> scrollBarColor;
    std::pair<bool, Dimension> scrollBarWidth;
    EdgeEffect edgeEffect = EdgeEffect::NONE;
};

struct CommonFilterStyle : Style {
    Dimension filter;
    Dimension backdropFilter;
    Dimension windowFilter;
};

struct CommonOpacityStyle : Style {
    double opacity = 1.0;
    int32_t appearingDuration = 0;
};

struct CommonVisibilityStyle : Style {
    VisibilityType visibility { VisibilityType::NO_SETTING };
};

struct CommonDisplayStyle : Style {
    DisplayType display { DisplayType::NO_SETTING };
};

struct CommonAnimationStyle : Style {
    std::string transform;
    std::string transformOrigin;
    std::string animationName;
    int32_t animationDelay = 0;
    int32_t animationDuration = 0;
    int32_t iteration = 1;
    RefPtr<Curve> curve;
    FillMode fillMode = FillMode::NONE;

    Dimension transformOriginX;
    Dimension transformOriginY;
    TweenOption tweenOption;

    AnimationOperation animationOperation = AnimationOperation::PLAY;
};

struct CommonShareTransitionStyle : Style {
    std::string transitionName;
    RefPtr<SharedTransitionEffect> sharedEffect;
    RefPtr<Curve> curve;

    TweenOption sharedTransitionOption;
};

struct CommonCardTransitionStyle : Style {
    TransitionEffect transitionEffect = TransitionEffect::NONE;
};

struct CommonPageTransitionStyle : Style {
    TweenOption transitionEnterOption;
    TweenOption transitionExitOption;
    int32_t transitionDuration = 0;
    RefPtr<Curve> curve;
};

struct CommonMaskStyle : Style {
    std::string maskImage;
    std::string maskPosition;
    std::string maskSize;
};

struct CommonImageStyle : Style {
    std::optional<Color> imageFill;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_STYLE_H
