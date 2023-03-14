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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_ARC_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_ARC_H

#include "base/geometry/point.h"
#include "base/utils/utils.h"

namespace OHOS::Ace {

class Arc {
public:
    Arc() = default;
    ~Arc() = default;

    const Point& GetCenterPoint() const
    {
        return centerPoint_;
    }

    void SetCenterPoint(const Point& point)
    {
        centerPoint_ = point;
    }

    double GetRadius() const
    {
        return radius_;
    }

    void SetRadius(double radius)
    {
        radius_ = radius;
    }

    double GetStartAngle() const
    {
        return startAngle_;
    }

    void SetStartAngle(double angle)
    {
        startAngle_ = angle;
    }

    double GetEndAngle() const
    {
        return endAngle_;
    }

    void SetEndAngle(double angle)
    {
        endAngle_ = angle;
    }

    double GetSweepAngle() const
    {
        return endAngle_ - startAngle_;
    }

    void Rotate(const Point& point, double angle)
    {
        centerPoint_.Rotate(point, angle);
        startAngle_ += angle;
        endAngle_ += angle;
    }

    void Move(double xOffset, double yOffset)
    {
        centerPoint_.SetX(centerPoint_.GetX() + xOffset);
        centerPoint_.SetY(centerPoint_.GetY() + yOffset);
    }

    void GetPointByAngle(double angle, Point& out) const
    {
        out.SetX(centerPoint_.GetX() + radius_);
        out.SetY(centerPoint_.GetY());
        out.Rotate(centerPoint_, angle);
    }

    void GetStartPoint(Point& out) const
    {
        GetPointByAngle(startAngle_, out);
    }

    void GetEndPoint(Point& out) const
    {
        GetPointByAngle(endAngle_, out);
    }

    Point GetStartPoint() const
    {
        Point startPoint;
        GetStartPoint(startPoint);
        return startPoint;
    }

    Point GetEndPoint() const
    {
        Point endPoint;
        GetEndPoint(endPoint);
        return endPoint;
    }

    double GetLeft() const
    {
        return centerPoint_.GetX() - radius_;
    }

    double GetRight() const
    {
        return centerPoint_.GetX() + radius_;
    }

    double GetTop() const
    {
        return centerPoint_.GetY() - radius_;
    }

    double GetBottom() const
    {
        return centerPoint_.GetY() + radius_;
    }

private:
    Point centerPoint_;
    double radius_ = 0.0;
    double startAngle_ = 0.0;
    double endAngle_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_ARC_H
