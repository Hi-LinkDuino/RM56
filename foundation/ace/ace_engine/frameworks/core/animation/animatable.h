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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATABLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATABLE_H

#include <map>

#include "base/memory/ace_type.h"
#include "core/animation/curve.h"
#include "core/animation/curves.h"

namespace OHOS::Ace {

enum class AnimatableType {
    PROPERTY_WIDTH,
    PROPERTY_HEIGHT,
    PROPERTY_BG_COLOR,
    PROPERTY_OPACITY,
    PROPERTY_MARGIN_LEFT, // margin
    PROPERTY_MARGIN_TOP,
    PROPERTY_MARGIN_RIGHT,
    PROPERTY_MARGIN_BOTTOM,
    PROPERTY_PADDING_LEFT, // padding
    PROPERTY_PADDING_TOP,
    PROPERTY_PADDING_RIGHT,
    PROPERTY_PADDING_BOTTOM,
    PROPERTY_BACKGROUND_POSITION,
    PROPERTY_BACKGROUND_SIZE,
    PROPERTY_BORDER_LEFT_WIDTH, // border
    PROPERTY_BORDER_TOP_WIDTH,
    PROPERTY_BORDER_RIGHT_WIDTH,
    PROPERTY_BORDER_BOTTOM_WIDTH,
    PROPERTY_BORDER_LEFT_COLOR,
    PROPERTY_BORDER_TOP_COLOR,
    PROPERTY_BORDER_RIGHT_COLOR,
    PROPERTY_BORDER_BOTTOM_COLOR,
    PROPERTY_BORDER_TOP_LEFT_RADIUS,
    PROPERTY_BORDER_TOP_RIGHT_RADIUS,
    PROPERTY_BORDER_BOTTOM_LEFT_RADIUS,
    PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS,
    PROPERTY_BORDER_LEFT_STYLE,
    PROPERTY_BORDER_RIGHT_STYLE,
    PROPERTY_BORDER_TOP_STYLE,
    PROPERTY_BORDER_BOTTOM_STYLE,
    PROPERTY_FILTER_BLUR,          // filter
    PROPERTY_BACKDROP_FILTER_BLUR, // backdrop filter
    PROPERTY_WINDOW_FILTER_BLUR,   // window blur
    PROPERTY_BOX_SHADOW,           // shadow
    PROPERTY_POSITION_LEFT,        // position
    PROPERTY_POSITION_TOP,
    PROPERTY_POSITION_RIGHT,
    PROPERTY_POSITION_BOTTOM,
};

class Animatable : public AceType {
    DECLARE_ACE_TYPE(Animatable, AceType);

public:
    void SetTimePoint(float timePoint)
    {
        timePoint_ = std::clamp(timePoint, 0.0f, 1.0f);
    }

    float GetTimePoint() const
    {
        return timePoint_;
    }

    void SetCurve(const RefPtr<Curve>& curve)
    {
        if (curve) {
            curve_ = curve;
        }
    }

    const RefPtr<Curve>& GetCurve() const
    {
        return curve_;
    }

protected:
    float timePoint_ = 0.0f; // between 0.0 to 1.0
    RefPtr<Curve> curve_ = Curves::LINEAR;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATABLE_H
