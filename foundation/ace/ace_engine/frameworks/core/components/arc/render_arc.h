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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ARC_RENDER_ARC_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ARC_RENDER_ARC_H

#include "core/components/arc/arc_component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

constexpr double ARC_RADIUS_TO_DIAMETER = 2.0;

class RenderArc : public RenderNode {
    DECLARE_ACE_TYPE(RenderArc, RenderNode);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    double GetOuterRadius() const;

    double GetWidth() const;

    double GetStartAngle() const;

    double GetSweepAngle() const;

    Color GetColor() const;

    void SetOuterRadius(double outerRadius);

    void SetWidth(double width);

    void SetStartAngle(double startAngle);

    void SetSweepAngle(double sweepAngle);

    void SetColor(const Color& color);

    bool IsAcrValid() const;

    bool IsArcShow() const;

    double GetPositionAngle(const Offset& position, double correctOffset) const;

    bool IsInRegion(const Point& point) const;

    double GetShadowWidth() const;

protected:
    double outerRadius_ = 0.0;
    double width_ = 0.0;
    ArcFlex flex_ = ArcFlex::FLEX_X;

    double startAngle_ = 0.0;
    double sweepAngle_ = 0.0;

    Color color_ = Color::BLACK;

    double arcRadiusX_ = 0.0;
    double arcRadiusY_ = 0.0;
    double arcWidth_ = 0.0;

    double shadowWidth_ = 0.0;
}; // class RenderArc

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ARC_RENDER_ARC_H
