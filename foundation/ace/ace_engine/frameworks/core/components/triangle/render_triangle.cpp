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

#include "core/components/triangle/render_triangle.h"

#include "core/components/triangle/triangle_component.h"

namespace OHOS::Ace {

bool TriangleRotationCalculator::Calculate(double xOffset, double yOffset)
{
    // calculate bottom angle
    const double PI = 3.14;
    double width = x2_ - x1_;
    double height = y3_ - y1_;
    if (NearZero(height) || NearZero(width)) {
        return false;
    }
    double bottomAngle = PI - std::atan(width / 2.0 / height) * 2.0;
    double topAngle = PI - std::atan(height * 2.0 / width);
    // calculate out arc 1
    Point centerPoint;
    centerPoint.SetX(x1_ + radius_ * std::tan(topAngle / 2.0));
    centerPoint.SetY(y1_ + radius_);
    double endAngle = PI + PI / 2.0;
    double startAngle = endAngle - topAngle;
    outArc1_.SetCenterPoint(centerPoint);
    outArc1_.SetRadius(radius_);
    outArc1_.SetStartAngle(startAngle);
    outArc1_.SetEndAngle(endAngle);
    // calculate out arc 2
    centerPoint.SetX(x2_ - radius_ * std::tan(topAngle / 2.0));
    centerPoint.SetY(y2_ + radius_);
    startAngle = PI + PI / 2.0;
    endAngle = startAngle + topAngle;
    outArc2_.SetCenterPoint(centerPoint);
    outArc2_.SetRadius(radius_);
    outArc2_.SetStartAngle(startAngle);
    outArc2_.SetEndAngle(endAngle);
    // calculate out arc3
    centerPoint.SetX(x3_);
    centerPoint.SetY(y3_ - radius_ / std::cos(bottomAngle / 2.0));
    startAngle = PI / 2.0 - bottomAngle / 2.0;
    endAngle = startAngle + bottomAngle;
    outArc3_.SetCenterPoint(centerPoint);
    outArc3_.SetRadius(radius_);
    outArc3_.SetStartAngle(startAngle);
    outArc3_.SetEndAngle(endAngle);
    // move with inner diff
    double topDiff = radius_ * std::tan(topAngle / 2.0) - radius_;
    double rate = height * 2.0 / width;
    double newHeight = (width / 2.0 + topDiff) * rate;
    double bottomDiff = newHeight - height;
    outArc1_.Move(0.0 - topDiff, 0.0);
    outArc2_.Move(topDiff, 0.0);
    outArc3_.Move(0.0, bottomDiff);
    // rotate with center point of triangle
    centerPoint.SetX(x3_);
    centerPoint.SetY((y1_ + y3_ + bottomDiff) / 2.0);
    outArc1_.Rotate(centerPoint, angle_);
    outArc2_.Rotate(centerPoint, angle_);
    outArc3_.Rotate(centerPoint, angle_);
    // move with offset
    outArc1_.Move(xOffset, yOffset);
    outArc2_.Move(xOffset, yOffset);
    outArc3_.Move(xOffset, yOffset);
    return true;
}

bool TriangleRotationCalculator::IsDown() const
{
    return NearZero(angle_);
}

void RenderTriangle::Update(const RefPtr<Component>& component)
{
    auto triangle = AceType::DynamicCast<TriangleComponent>(component);
    if (!triangle) {
        LOGE("input component is incorrect type or null.");
        return;
    }

    width_ = triangle->GetWidth();
    height_ = triangle->GetHeight();
    color_ = triangle->GetColor();
    padding_ = triangle->GetPadding();
    MarkNeedLayout();
}

void RenderTriangle::PerformLayout()
{
    CalculateTrianglePoints();
    SetLayoutSize(Size(NormalizeToPx(width_), NormalizeToPx(height_)));
}

void RenderTriangle::CalculateTrianglePoints()
{
    double width = NormalizeToPx(width_);
    double height = NormalizeToPx(height_);
    double padding = NormalizeToPx(padding_);
    double x1 = padding;
    double y1 = padding;
    double x2 = width - padding;
    double y2 = padding;
    double x3 = (x1 + x2) / 2.0f; // the average
    double y3 = height - padding;
    data_.SetPointOne(x1, y1);
    data_.SetPointTwo(x2, y2);
    data_.SetPointThree(x3, y3);
}

void RenderTriangle::HandleClick()
{
    if (!onClick_) {
        return; // for select that no need to change direction
    }

    bool down = data_.IsDown();
    down = !down;
    onClick_(down);
}

void RenderTriangle::OnTouchTestHit(const Offset&, const TouchRestrict&, TouchTestResult& result)
{
    if (!click_) {
        click_ = AceType::MakeRefPtr<ClickRecognizer>();
        auto weak = WeakClaim(this);
        click_->SetOnClick([weak = WeakClaim(this)](const ClickInfo&) {
            auto refPtr = weak.Upgrade();
            if (refPtr) {
                refPtr->HandleClick();
            }
        });
    }
    if (onClick_) {
        click_->SetIsExternalGesture(true);
        result.emplace_back(click_);
    }
}

} // namespace OHOS::Ace
