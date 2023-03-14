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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_LOADING_PROGRESS_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_LOADING_PROGRESS_COMPONENT_H

#include "base/geometry/dimension.h"
#include "base/geometry/offset.h"
#include "core/components/common/properties/color.h"
#include "core/pipeline/base/render_component.h"

namespace OHOS::Ace {

constexpr int32_t COMET_COUNT = 20;
constexpr double MOVE_RATIO = 0.06; // The Ratio to the Ring diameter.
constexpr double COMET_TAIL_LEN = 60.0;
const Dimension RING_RADIUS = 10.5_vp;
const Dimension ORBIT_RADIUS = 17.0_vp;
const Color PROGRESS_COLOR = Color(0xff666666);
const Color PROGRESS_COLOR_DARK = Color(0x99ffffff);

class ACE_EXPORT LoadingProgressComponent : public RenderComponent {
    DECLARE_ACE_TYPE(LoadingProgressComponent, RenderComponent);

public:
    LoadingProgressComponent() = default;
    explicit LoadingProgressComponent(double diameter) : diameter_(diameter) {};
    ~LoadingProgressComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    const Dimension& GetDiameter() const
    {
        return diameter_;
    }

    void SetDiameter(const Dimension& diameter)
    {
        diameter_ = diameter;
    }

    void SetProgressColor(const Color& color)
    {
        progressColor_ = color;
    }

    const Color& GetProgressColor() const
    {
        return progressColor_;
    }

    void SetMoveRatio(double ratio)
    {
        moveRatio_ = ratio;
    }

    double GetMoveRatio() const
    {
        return moveRatio_;
    }

    void SetRingRadius(const Dimension& radius)
    {
        ringRadius_ = radius;
    }

    const Dimension& GetRingRadius() const
    {
        return ringRadius_;
    }

    void SetOrbitRadius(const Dimension& radius)
    {
        orbitRadius_ = radius;
    }

    const Dimension& GetOrbitRadius() const
    {
        return orbitRadius_;
    }

    void SetCometTailLen(double len)
    {
        cometTailLen_ = len;
    }

    double GetCometTailLen() const
    {
        return cometTailLen_;
    }

private:
    Dimension diameter_;
    double cometTailLen_ = COMET_TAIL_LEN; // Degrees.
    double moveRatio_ = MOVE_RATIO; // The Ratio to the Ring diameter.
    Dimension ringRadius_ = RING_RADIUS;
    Dimension orbitRadius_ = ORBIT_RADIUS;
    Color progressColor_ = PROGRESS_COLOR;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_LOADING_PROGRESS_COMPONENT_H
