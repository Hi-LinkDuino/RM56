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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COMMON_PROPERTIES_RADIUS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COMMON_PROPERTIES_RADIUS_H

#include "base/geometry/animatable_dimension.h"
#include "base/utils/utils.h"

namespace OHOS::Ace {

// A radius for shapes like rect and etc.
class Radius final {
public:
    Radius() = default;
    explicit Radius(double value) : Radius(value, value) {}
    explicit Radius(const Dimension& value) : Radius(value, value) {}
    explicit Radius(const AnimatableDimension& value) : Radius(value, value) {}
    Radius(double x, double y) : x_(Dimension(x)), y_(Dimension(y)) {}
    Radius(const Dimension& x, const Dimension& y) : x_(x), y_(y) {}
    Radius(const AnimatableDimension& x, const AnimatableDimension& y) : x_(x), y_(y) {}
    ~Radius() = default;

    void SetContextAndCallback(const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
    {
        x_.SetContextAndCallback(context, callback);
        y_.SetContextAndCallback(context, callback);
    }

    void ApplyScaleAndRound(double scale)
    {
        x_.SetValue(round(x_.Value() * scale));
        y_.SetValue(round(y_.Value() * scale));
    }

    bool IsValid() const
    {
        return x_.IsValid() && y_.IsValid();
    }

    bool HasValue() const
    {
        return x_.IsValid() || y_.IsValid();
    }

    const AnimatableDimension& GetX() const
    {
        return x_;
    }

    const AnimatableDimension& GetY() const
    {
        return y_;
    }

    void SetX(const Dimension& x, const AnimationOption& option = AnimationOption())
    {
        x_ = AnimatableDimension(x, option);
    }

    void SetY(const Dimension& y, const AnimationOption& option = AnimationOption())
    {
        y_ = AnimatableDimension(y, option);
    }

    void SetX(const AnimatableDimension& x)
    {
        x_ = x;
    }

    void SetY(const AnimatableDimension& y)
    {
        y_ = y;
    }

    Radius operator+(const Radius& radius) const
    {
        return Radius(x_ + radius.GetX(), y_ + radius.GetY());
    }

    Radius operator-(const Radius& radius) const
    {
        return Radius(x_ - radius.GetX(), y_ - radius.GetY());
    }

    Radius operator*(double scale) const
    {
        return Radius(x_ * scale, y_ * scale);
    }

    bool operator==(const Radius& radius) const
    {
        return (radius.GetX() == x_) && (radius.GetY() == y_);
    }

    std::string ToString() const
    {
        return std::string("x:").append(x_.ToString()).append(", y:").append(y_.ToString());
    }

private:
    AnimatableDimension x_;
    AnimatableDimension y_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COMMON_PROPERTIES_RADIUS_H
