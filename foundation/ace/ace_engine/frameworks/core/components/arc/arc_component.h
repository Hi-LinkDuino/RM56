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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ARC_ARC_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ARC_ARC_COMPONENT_H

#include "base/geometry/dimension.h"
#include "base/log/log.h"
#include "core/components/common/properties/color.h"
#include "core/pipeline/base/render_component.h"

namespace OHOS::Ace {

enum class ArcFlex {
    FLEX_X,
    FLEX_Y,
    FLEX_XY,
};

class ArcComponent : public RenderComponent {
    DECLARE_ACE_TYPE(ArcComponent, RenderComponent);

public:
    ArcComponent() = default;
    ~ArcComponent() override = default;
    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    ArcComponent(double outerRadius, double width)
    {
        SetOuterRadius(outerRadius);
        SetWidth(width);
    }

    ArcComponent(double outerRadius, double width, double startAngle, double sweepAngle)
    {
        SetOuterRadius(outerRadius);
        SetWidth(width);
        startAngle_ = startAngle;
        sweepAngle_ = sweepAngle;
    }

    ArcComponent(double outerRadius, double width, double startAngle, double sweepAngle, const Color& color)
    {
        SetOuterRadius(outerRadius);
        SetWidth(width);
        startAngle_ = startAngle;
        sweepAngle_ = sweepAngle;
        color_ = color;
    }

    const Dimension& GetOuterRadius() const
    {
        return outerRadius_;
    }

    void SetOuterRadius(const Dimension& outerRadius)
    {
        if (!outerRadius.IsValid()) {
            LOGE("Invalid outerRadius:%{public}lf", outerRadius.Value());
            return;
        }
        outerRadius_ = outerRadius;
    }

    void SetOuterRadius(double outerRadius)
    {
        if (outerRadius < 0.0) {
            LOGE("Invalid outerRadius:%{public}lf", outerRadius);
            return;
        }
        outerRadius_ = Dimension(outerRadius);
    }

    const Dimension& GetWidth() const
    {
        return width_;
    }

    void SetWidth(const Dimension& width)
    {
        if (!width.IsValid()) {
            LOGE("Invalid width:%{public}lf", width.Value());
            return;
        }
        width_ = width;
    }

    void SetWidth(double width)
    {
        if (width < 0.0) {
            LOGE("Invalid width:%{public}lf", width);
            return;
        }
        width_ = Dimension(width);
    }

    double GetStartAngle() const
    {
        return startAngle_;
    }

    void SetStartAngle(double startAngle)
    {
        startAngle_ = startAngle;
    }

    double GetSweepAngle() const
    {
        return sweepAngle_;
    }

    void SetSweepAngle(double sweepAngle)
    {
        sweepAngle_ = sweepAngle;
    }

    const Color& GetColor() const
    {
        return color_;
    }

    void SetColor(const Color& color)
    {
        color_ = color;
    }

    ArcFlex GetArcFlex() const
    {
        return flex_;
    }

    void SetArcFlex(ArcFlex flex)
    {
        flex_ = flex;
    }

    const Dimension& GetShadowWidth() const
    {
        return arcShadowWidth_;
    }

    void SetShadowWidth(const Dimension& width)
    {
        arcShadowWidth_ = width;
    }

    bool IsAcrValid() const
    {
        return !NearZero(sweepAngle_);
    }

    bool IsArcShow() const
    {
        return color_.GetAlpha() != 0;
    }

private:
    Dimension outerRadius_;
    Dimension width_;
    Dimension arcShadowWidth_;
    ArcFlex flex_ = ArcFlex::FLEX_X;
    Color color_ = Color::BLACK;
    double startAngle_ = 0.0;
    double sweepAngle_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ARC_ARC_COMPONENT_H
