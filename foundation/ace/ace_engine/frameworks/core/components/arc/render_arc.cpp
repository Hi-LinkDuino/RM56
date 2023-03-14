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

#include "core/components/arc/render_arc.h"

#include "core/components/arc/arc_component.h"

namespace OHOS::Ace {
namespace {

constexpr double ROOT = 0.5;

} // namespace

void RenderArc::PerformLayout()
{
    // get parameters set by user
    arcRadiusX_ = 0.0;
    arcRadiusY_ = 0.0;
    arcWidth_ = 0.0;
    if (!NearZero(outerRadius_)) {
        arcRadiusX_ = outerRadius_;
        arcRadiusY_ = outerRadius_;
    }
    if (!NearZero(width_)) {
        arcWidth_ = width_;
    }

    // calculate self size
    const LayoutParam& layoutSetByParent = GetLayoutParam();
    Size arcMax = layoutSetByParent.GetMaxSize();

    if (NearZero(arcRadiusX_)) {
        switch (flex_) {
            case ArcFlex::FLEX_X:
                arcRadiusX_ = arcMax.Width() / ARC_RADIUS_TO_DIAMETER;
                arcRadiusY_ = arcRadiusX_;
                outerRadius_ = arcRadiusX_;
                break;
            case ArcFlex::FLEX_Y:
                arcRadiusY_ = arcMax.Height() / ARC_RADIUS_TO_DIAMETER;
                arcRadiusX_ = arcRadiusY_;
                outerRadius_ = arcRadiusY_;
                break;
            case ArcFlex::FLEX_XY:
                arcRadiusX_ = arcMax.Width() / ARC_RADIUS_TO_DIAMETER;
                arcRadiusY_ = arcMax.Height() / ARC_RADIUS_TO_DIAMETER;
                outerRadius_ = std::min(arcRadiusX_, arcRadiusY_);
                break;
            default:
                break;
        }
    }

    SetLayoutSize(Size(arcRadiusX_ * ARC_RADIUS_TO_DIAMETER, arcRadiusY_ * ARC_RADIUS_TO_DIAMETER));
    MarkNeedRender();
}

void RenderArc::Update(const RefPtr<Component>& component)
{
    RefPtr<ArcComponent> arcComponent = AceType::DynamicCast<ArcComponent>(component);
    if (arcComponent) {
        startAngle_ = arcComponent->GetStartAngle();
        sweepAngle_ = arcComponent->GetSweepAngle();
        color_ = arcComponent->GetColor();
        flex_ = arcComponent->GetArcFlex();

        auto context = GetContext().Upgrade();
        if (context) {
            outerRadius_ = context->NormalizeToPx(arcComponent->GetOuterRadius());
            width_ = context->NormalizeToPx(arcComponent->GetWidth());
            shadowWidth_ = context->NormalizeToPx(arcComponent->GetShadowWidth());
        } else {
            LOGE("GetContext failed.");
        }
        MarkNeedLayout();
    }
}

double RenderArc::GetOuterRadius() const
{
    return outerRadius_;
}

double RenderArc::GetWidth() const
{
    return width_;
}

double RenderArc::GetStartAngle() const
{
    return startAngle_;
}

double RenderArc::GetSweepAngle() const
{
    return sweepAngle_;
}

Color RenderArc::GetColor() const
{
    return color_;
}

void RenderArc::SetOuterRadius(double outerRadius)
{
    if (outerRadius < 0.0) {
        LOGE("Invalid outerRadius:%{public}lf", outerRadius);
        return;
    }
    outerRadius_ = outerRadius;
}

void RenderArc::SetWidth(double width)
{
    if (width < 0.0) {
        LOGE("Invalid width:%{public}lf", width);
        return;
    }
    width_ = width;
}

void RenderArc::SetStartAngle(double startAngle)
{
    startAngle_ = startAngle;
}

void RenderArc::SetSweepAngle(double sweepAngle)
{
    sweepAngle_ = sweepAngle;
}

void RenderArc::SetColor(const Color& color)
{
    color_ = color;
}

bool RenderArc::IsAcrValid() const
{
    return !NearZero(sweepAngle_);
}

bool RenderArc::IsArcShow() const
{
    return color_.GetAlpha() != 0;
}

double RenderArc::GetPositionAngle(const Offset& position, double correctOffset) const
{
    double angle = atan2(position.GetY() - arcRadiusY_, position.GetX() - arcRadiusX_);
    if (angle < startAngle_ - correctOffset) {
        angle += M_PI * ARC_RADIUS_TO_DIAMETER;
    }
    return angle;
}

bool RenderArc::IsInRegion(const Point& point) const
{
    double distance = pow(pow(point.GetX() - arcRadiusX_, SQUARE) + pow(point.GetY() - arcRadiusY_, SQUARE), ROOT);
    double outArcAngle = width_ / (outerRadius_ - width_ / ARC_RADIUS_TO_DIAMETER) / ARC_RADIUS_TO_DIAMETER;
    double angle = GetPositionAngle(Offset(point.GetX(), point.GetY()), outArcAngle);
    if (distance > outerRadius_ || distance < outerRadius_ - width_) {
        return false;
    }
    if (angle < startAngle_ - outArcAngle || angle > startAngle_ + sweepAngle_ + outArcAngle) {
        return false;
    }
    return true;
}

double RenderArc::GetShadowWidth() const
{
    return shadowWidth_;
}

} // namespace OHOS::Ace
