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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_RRECT_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_RRECT_H

#include <algorithm>
#include <cstdint>
#include <cstring>

#include "base/geometry/rect.h"
#include "core/components/common/properties/radius.h"

namespace OHOS::Ace {

struct Corner {
    Radius topLeftRadius;
    Radius topRightRadius;
    Radius bottomRightRadius;
    Radius bottomLeftRadius;
};

class RRect final {
public:
    RRect() = default;
    ~RRect() = default;

    explicit RRect(const Rect& rect) : rect_(rect) {}
    RRect(double x, double y, double width, double height)
    {
        rect_.SetRect(x, y, width, height);
    }

    static RRect MakeRect(const Rect& rect)
    {
        RRect rrect;
        rrect.SetRect(rect);
        return rrect;
    }

    static RRect MakeRRect(const Rect& rect, const Radius& radius)
    {
        RRect rrect;
        rrect.SetRectWithSimpleRadius(rect, radius.GetX().Value(), radius.GetY().Value());
        return rrect;
    }

    static RRect MakeRRect(const Rect& rect, double x, double y)
    {
        RRect rrect;
        rrect.SetRectWithSimpleRadius(rect, x, y);
        return rrect;
    }

    static RRect MakeOval(const Rect& rect)
    {
        RRect rrect;
        rrect.SetOval(rect);
        return rrect;
    }

    const Rect& GetRect() const
    {
        return rect_;
    }

    const Corner& GetCorner() const
    {
        return corner_;
    }

    void SetRect(const Rect& rect)
    {
        rect_ = rect;
    }

    void SetCorner(const Corner& corner)
    {
        corner_ = corner;
    }

    void SetRectWithSimpleRadius(const Rect& rect, double xRadius, double yRadius)
    {
        rect_ = rect;
        if (xRadius <= 0 || yRadius <= 0) {
            return;
        }
        if (rect_.Width() < (xRadius + xRadius) || rect.Height() < (yRadius + yRadius)) {
            double scale = std::min(rect.Width() / (xRadius + xRadius), rect.Height() / (yRadius + yRadius));
            xRadius *= scale;
            yRadius *= scale;
        }
        auto radius = Radius(xRadius, yRadius);
        corner_ = { radius, radius, radius, radius };
    }

    void ApplyScaleAndRound(float scale)
    {
        rect_.ApplyScaleAndRound(Size(scale, scale));
        corner_.topLeftRadius.ApplyScaleAndRound(scale);
        corner_.topRightRadius.ApplyScaleAndRound(scale);
        corner_.bottomRightRadius.ApplyScaleAndRound(scale);
        corner_.bottomLeftRadius.ApplyScaleAndRound(scale);
    }

    void SetOval(const Rect& rect)
    {
        rect_ = rect;
        double xRadius = rect.Width() / 2;
        double yRadius = rect.Height() / 2;
        auto radius = Radius(xRadius, yRadius);
        corner_ = { radius, radius, radius, radius };
    }

    double Width() const
    {
        return rect_.Width();
    }

    double Height() const
    {
        return rect_.Height();
    }

    bool operator==(const RRect& rrect) const
    {
        return rrect.GetCorner().bottomLeftRadius == corner_.bottomLeftRadius &&
               rrect.GetCorner().bottomRightRadius == corner_.bottomRightRadius &&
               rrect.GetCorner().topLeftRadius == corner_.topLeftRadius &&
               rrect.GetCorner().topRightRadius == corner_.topRightRadius && rrect.GetRect() == rect_;
    }

    RRect& operator+=(const Offset& offset)
    {
        rect_ += offset;
        return *this;
    }

private:
    Corner corner_ { { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 } };
    Rect rect_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_RRECT_H
