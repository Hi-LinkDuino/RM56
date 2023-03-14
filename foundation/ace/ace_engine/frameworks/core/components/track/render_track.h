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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRACK_RENDER_TRACK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRACK_RENDER_TRACK_H

#include <vector>

#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/components/progress/progress_theme.h"
#include "core/components/track/track_component.h"
#include "core/pipeline/base/render_component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

struct RenderRingInfo {
    double radius = 0.0;
    Gradient gradient;
    Color color;
    Offset center;
    double startDegree = 0.0;
    double sweepDegree = 0.0;
    double thickness = 0.0;
    double clockwise = 1.0;

    double scaleStrokeWidth = 0.0;
    int32_t totalScaleNumber = 0;
};

class RenderTrack : public RenderNode {
    DECLARE_ACE_TYPE(RenderTrack, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;

    void PerformLayout() override;

    void UpdateAnimation();

    virtual Size Measure();

    Color GetSelectColor() const
    {
        return selectColor_;
    }

    void SetSelectColor(const Color& color)
    {
        selectColor_ = color;
    }

    Color GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    void SetBackgroundColor(const Color& color)
    {
        backgroundColor_ = color;
    }

    Color GetCachedColor() const
    {
        return cachedColor_;
    }

    void SetCachedColor(const Color& color)
    {
        cachedColor_ = color;
    }

    const Gradient& GetGradient() const
    {
        return selectGradient_;
    }

    double GetTotalRatio() const
    {
        return totalRatio_;
    }

    void SetTotalRatio(const double ratio)
    {
        if (!playAnimation_) {
            totalRatio_ = ratio;
        } else if (!NearEqual(ratio, prevousPercentValue_)) {
            animationDuring_ = std::chrono::steady_clock::now() - prevoiusUpdateTime_;
            percentChange_ = ratio - prevousPercentValue_;
            prevousPercentValue_ = ratio;
            prevoiusUpdateTime_ = std::chrono::steady_clock::now();
            needUpdateAnimation_ = true;
        }
    }

    double GetCachedRatio() const
    {
        return cachedRatio_;
    }

    void SetCachedRatio(const double ratio)
    {
        cachedRatio_ = ratio;
    }

    const RenderRingInfo& GetRingTrackInfo() const
    {
        return paintData_;
    }

    double GetTrackThickness() const
    {
        return paintData_.thickness;
    }

    SliderMode GetSliderMode() const
    {
        return sliderMode_;
    }

    void SetSliderMode(SliderMode mode)
    {
        sliderMode_ = mode;
    }

    double GetSliderSteps() const
    {
        return sliderSteps_;
    }

    void SetSliderSteps(double steps)
    {
        sliderSteps_ = steps;
    }

    void SetAnimationPlay(bool playAnimation)
    {
        playAnimation_ = playAnimation;
    }

    void Dump() override;

protected:
    std::string markedText_;
    Color markedTextColor_;
    RenderRingInfo paintData_;
    bool showIndicator_ = false;
    std::vector<Color> colors_;
    std::vector<double> weights_;
    bool leftToRight_ = true;
    RefPtr<ProgressTheme> theme_;
    SliderMode sliderMode_ = SliderMode::OUTSET;
    double sliderSteps_ = 0.0;

    RefPtr<Animator> progressTransitionController_;
    RefPtr<Animator> scanHaloController_;
    double prevousPercentValue_ = 0.0;
    double percentChange_ = 0.0;
    std::chrono::steady_clock::time_point prevoiusUpdateTime_ = std::chrono::steady_clock::now();
    std::chrono::duration<double> animationDuring_;
    bool needUpdateAnimation_ = false;
    bool playAnimation_ = false;
    bool isReverse_ = false;
    double scanHighLightValue_ = 0.0;

    Color selectColor_;
    Gradient selectGradient_;
    Color backgroundColor_;
    Color cachedColor_;
    double totalRatio_ = 0.0;
    double cachedRatio_ = 0.0;
    Dimension thickness_;
    Dimension scaleStrokeWidth_;
    Axis direction_ = Axis::HORIZONTAL;
};

class RenderCircleTrack : public RenderTrack {
    DECLARE_ACE_TYPE(RenderCircleTrack, RenderTrack);

public:
    static RefPtr<RenderNode> Create();

    Size Measure() override;

    void Dump() override;
};

class RenderScaleRingTrack : public RenderTrack {
    DECLARE_ACE_TYPE(RenderScaleRingTrack, RenderTrack);

public:
    static RefPtr<RenderNode> Create();

    Size Measure() override;

    void Dump() override;
};

class RenderArcTrack : public RenderTrack {
    DECLARE_ACE_TYPE(RenderArcTrack, RenderTrack);

public:
    static RefPtr<RenderNode> Create();

    Size Measure() override;

    void Dump() override;
};

class RenderMoonTrack : public RenderTrack {
    DECLARE_ACE_TYPE(RenderMoonTrack, RenderTrack);

public:
    static RefPtr<RenderNode> Create();

    Size Measure() override;

    void Dump() override;
};

class RenderCapsuleTrack : public RenderTrack {
    DECLARE_ACE_TYPE(RenderCapsuleTrack, RenderTrack);

public:
    static RefPtr<RenderNode> Create();

    Size Measure() override;

    void Dump() override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRACK_RENDER_TRACK_H
