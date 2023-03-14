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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRACK_TRACK_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRACK_TRACK_COMPONENT_H

#include <vector>

#include "base/geometry/dimension.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/decoration.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/render_component.h"

namespace OHOS::Ace {

enum class SliderMode {
    OUTSET, // block on track, track is thin
    INSET,  // block inside track, track is rough
    CAPSULE, // capsule slider.
};

inline const Dimension TRACK_HEIGHT_DP = Dimension(2.0, DimensionUnit::VP);

class RingTrackInfo : public virtual AceType {
    DECLARE_ACE_TYPE(RingTrackInfo, AceType);

public:
    RingTrackInfo() = default;
    ~RingTrackInfo() override = default;

    void SetStartDegree(double startDegree)
    {
        startDegree_ = startDegree;
    }

    void SetSweepDegree(double sweepDegree)
    {
        sweepDegree_ = sweepDegree;
    }

    void SetClockwise(bool clockwise)
    {
        if (clockwise) {
            clockwise_ = 1.0;
        } else {
            clockwise_ = -1.0;
        }
    }

    void SetDimension(const Dimension& thickness)
    {
        trackThickness_ = thickness;
    }

    void SetScaleNumber(int32_t number)
    {
        totalScaleNumber_ = number;
    }

    void SetScaleWidth(const Dimension& width)
    {
        scaleWidth_ = width;
    }

    void SetRadius(double radius)
    {
        radius_ = radius;
    }

    void SetX(double x)
    {
        coordinateX_ = x;
    }

    void SetY(double y)
    {
        coordinateY_ = y;
    }

    double GetRadius() const
    {
        return radius_;
    }

    double GetX() const
    {
        return coordinateX_;
    }

    double GetY() const
    {
        return coordinateY_;
    }

    double GetStartDegree() const
    {
        return startDegree_;
    }

    double GetSweepDegree() const
    {
        return sweepDegree_;
    }

    const Dimension& GetTrackThickness() const
    {
        return trackThickness_;
    }

    double GetClockwiseValue() const
    {
        return clockwise_;
    }

    int32_t GetScaleNumber() const
    {
        return totalScaleNumber_;
    }

    const Dimension& GetScaleWidth() const
    {
        return scaleWidth_;
    }

private:
    Dimension trackThickness_;

    // degree from 0 to 360
    double startDegree_ = 0.0;
    double sweepDegree_ = 360.0;
    // clockwise is 1.0 and counter-clockwise is -1.0
    double clockwise_ = 1.0;

    double coordinateX_ = -1.0;
    double coordinateY_ = -1.0;
    double radius_ = -1.0;

    // the number of scale in ring. default number is 120
    int32_t totalScaleNumber_ = 120;
    Dimension scaleWidth_;
};

class TrackComponent : public RenderComponent {
    DECLARE_ACE_TYPE(TrackComponent, RenderComponent);

public:
    TrackComponent() = default;
    ~TrackComponent() override = default;

    RefPtr<Element> CreateElement() override
    {
        // never use element to create element tree.
        return nullptr;
    }

    const Color& GetSelectColor() const
    {
        return selectColor_;
    }

    void SetSelectColor(const Color& color)
    {
        selectColor_ = color;
    }

    const Gradient& GetSelectGradient() const
    {
        return selectGradient_;
    }

    void SetSelectGradient(const Gradient& color)
    {
        selectGradient_ = color;
    }

    const Color& GetCachedColor() const
    {
        return cachedColor_;
    }

    void SetCachedColor(const Color& color)
    {
        cachedColor_ = color;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    void SetBackgroundColor(const Color& color)
    {
        backgroundColor_ = color;
    }

    const Dimension& GetTrackThickness() const
    {
        return trackPaintData_->GetTrackThickness();
    }

    double GetStartDegree() const
    {
        return trackPaintData_->GetStartDegree();
    }

    double GetSweepDegree() const
    {
        return trackPaintData_->GetSweepDegree();
    }

    double GetClockwiseValue() const
    {
        return trackPaintData_->GetClockwiseValue();
    }

    int32_t GetScaleNumber() const
    {
        return trackPaintData_->GetScaleNumber();
    }

    const Dimension& GetScaleWidth() const
    {
        return trackPaintData_->GetScaleWidth();
    }

    void SetScaleNumber(int32_t number)
    {
        trackPaintData_->SetScaleNumber(number);
    }

    void SetScaleWidth(const Dimension& width)
    {
        trackPaintData_->SetScaleWidth(width);
    }

    void SetTrackThickness(const Dimension& thickness)
    {
        trackPaintData_->SetDimension(thickness);
    }

    void SetIndicatorFlag(bool flag)
    {
        showIndicator_ = flag;
    }

    void SetRadius(double radius)
    {
        trackPaintData_->SetRadius(radius);
    }

    void SetCenterX(double x)
    {
        trackPaintData_->SetX(x);
    }

    void SetCenterY(double y)
    {
        trackPaintData_->SetY(y);
    }

    bool GetIndicatorFlag() const
    {
        return showIndicator_;
    }

    void SetSectionsStyle(const std::vector<Color>& colors, const std::vector<double>& weights)
    {
        colors_ = colors;
        weights_ = weights;
    }

    const std::vector<Color>& GetSectionsColors() const
    {
        return colors_;
    }

    const std::vector<double>& GetSectionsWeights() const
    {
        return weights_;
    }

    RefPtr<RingTrackInfo> GetTrackInfo() const
    {
        return trackPaintData_;
    }

    double GetRadius() const
    {
        return trackPaintData_->GetRadius();
    }

    double GetCenterX() const
    {
        return trackPaintData_->GetX();
    }

    double GetCenterY() const
    {
        return trackPaintData_->GetY();
    }

    bool GetShowAnimation() const
    {
        return showAnimation_;
    }

    void SetShowAnimation(bool isAnimation)
    {
        showAnimation_ = isAnimation;
    }

    void SetLableMarkedText(std::string markedText)
    {
        markedText_ = markedText;
    }

    void SetLableMarkedColor(Color markedColor)
    {
        markedTextColor_ = markedColor;
    }

    std::string GetLableMarkedText()
    {
        return markedText_;
    }

    Color GetLableMarkedColor()
    {
        return markedTextColor_;
    }

    Axis GetDirection() const
    {
        return axis_;
    }

    void SetDirection(Axis axis)
    {
        axis_ = axis;
    }

    bool IsReverse() const
    {
        return isReverse_;
    }

    void SetReverse(bool isReverse)
    {
        isReverse_ = isReverse;
    }

private:
    std::string markedText_;
    Color markedTextColor_;
    Color selectColor_;
    Gradient selectGradient_;
    Color cachedColor_;
    Color backgroundColor_;
    // the thickness of the track. when the track is circle, there is no effect.
    RefPtr<RingTrackInfo> trackPaintData_ = AceType::MakeRefPtr<RingTrackInfo>();
    std::vector<Color> colors_;
    std::vector<double> weights_;
    bool showIndicator_ = false;
    bool showAnimation_ = true;
    bool isReverse_ = false;
    Axis axis_ = Axis::HORIZONTAL;
};

class LinearTrack : public TrackComponent {
    DECLARE_ACE_TYPE(LinearTrack, TrackComponent);

public:
    LinearTrack() = default;
    ~LinearTrack() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
};

class CircularTrack : public TrackComponent {
    DECLARE_ACE_TYPE(CircularTrack, TrackComponent);

public:
    CircularTrack() = default;
    ~CircularTrack() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
};

class ScaleRingTrack : public TrackComponent {
    DECLARE_ACE_TYPE(ScaleRingTrack, TrackComponent);

public:
    ScaleRingTrack() = default;
    ~ScaleRingTrack() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
};

class ArcTrack : public TrackComponent {
    DECLARE_ACE_TYPE(ArcTrack, TrackComponent);

public:
    ArcTrack() = default;
    ~ArcTrack() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
};

class MoonTrack : public TrackComponent {
    DECLARE_ACE_TYPE(MoonTrack, TrackComponent);

public:
    MoonTrack() = default;
    ~MoonTrack() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
};

class CapsuleTrack : public TrackComponent {
    DECLARE_ACE_TYPE(CapsuleTrack, TrackComponent);

public:
    CapsuleTrack() = default;
    ~CapsuleTrack() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRACK_TRACK_COMPONENT_H
