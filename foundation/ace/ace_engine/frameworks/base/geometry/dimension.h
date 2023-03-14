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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_DIMENSION_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_DIMENSION_H

#include <string>

#include "base/utils/macros.h"
#include "base/utils/system_properties.h"
#include "base/utils/utils.h"

#define NEAR_ZERO(value) ((value > 0.0) ? ((value - 0.0) <= 0.000001f) : ((0.0 - value) <= 0.000001f))

namespace OHOS::Ace {

enum class DimensionUnit {
    /*
     * Logical pixel used in Ace1.0. It's based on frontend design width.
     * For example, when a frontend with 750px design width running on a
     * device with 1080 pixels width, 1px represents 1.44 pixels.
     */
    PX = 0,
    /*
     * Density independent pixels, one vp is one pixel on a 160 dpi screen.
     */
    VP,
    /*
     * Scale independent pixels. This is like VP but will be scaled by
     * user's font size preference.
     */
    FP,
    /*
     * The percentage of either a value from the element's parent or from
     * another property of the element itself.
     */
    PERCENT,
    /*
     * logic pixels used in ACE2.0 instead of PX, and PX is the physical pixels in ACE2.0
     */
    LPX,
    /*
     * The value is calculated from the element's parent and another property of the element itself.
     */
    AUTO,
    /*
     * The value is expression.
     */
    CALC,
};

/*
 * Dimension contains a value and an unit to represent different
 * scales in one class.
 */
class Dimension {
public:
    constexpr Dimension() = default;
    ~Dimension() = default;
    constexpr explicit Dimension(double value, DimensionUnit unit = DimensionUnit::PX) : value_(value), unit_(unit) {}

    constexpr double Value() const
    {
        return value_;
    }

    void SetValue(double value)
    {
        value_ = value;
    }

    constexpr DimensionUnit Unit() const
    {
        return unit_;
    }

    void SetUnit(DimensionUnit unit)
    {
        unit_ = unit;
    }

    bool IsValid() const
    {
        return (value_ > 0.0) && (!NearZero(value_));
    }

    double ConvertToPx(double dipScale) const
    {
        if (unit_ == DimensionUnit::VP || unit_ == DimensionUnit::FP) {
            return value_ * dipScale;
        } else {
            return value_;
        }
    }

    double ConvertToVp() const
    {
        if (unit_ == DimensionUnit::VP) {
            return value_;
        }
        if (unit_ == DimensionUnit::PX) {
            return SystemProperties::Px2Vp(value_);
        }
        return 0;
    }

    double ConvertToPx() const
    {
        if (unit_ == DimensionUnit::PX) {
            return value_;
        }
        if (unit_ == DimensionUnit::VP) {
            return SystemProperties::Vp2Px(value_);
        }
        return 0;
    }

    constexpr Dimension operator*(double value) const
    {
        return Dimension(value_ * value, unit_);
    }

    constexpr Dimension operator/(double value) const
    {
        // NearZero cannot be used in a constant expression
        if (NEAR_ZERO(value)) {
            return Dimension();
        }
        return Dimension(value_ / value, unit_);
    }

    bool operator==(const Dimension& dimension) const
    {
        return (unit_ == dimension.unit_) && NearEqual(value_, dimension.value_);
    }

    bool operator!=(const Dimension& dimension) const
    {
        return !operator==(dimension);
    }

    /*
     * Add two dimensions using the same unit.
     */
    constexpr Dimension operator+(const Dimension& dimension) const
    {
        if (NEAR_ZERO(dimension.Value())) {
            return *this;
        }
        ACE_DCHECK(unit_ == dimension.unit_);
        return Dimension(value_ + dimension.value_, unit_);
    }

    /*
     * Add a new dimension to itself using same unit.
     */
    Dimension& operator+=(const Dimension& dimension)
    {
        ACE_DCHECK(unit_ == dimension.unit_);
        value_ += dimension.value_;
        return *this;
    }

    /*
     * Minus a dimension using the same unit.
     */
    constexpr Dimension operator-(const Dimension& dimension) const
    {
        if (NEAR_ZERO(dimension.Value())) {
            return *this;
        }
        ACE_DCHECK(unit_ == dimension.unit_);
        return Dimension(value_ - dimension.value_, unit_);
    }

    /*
     * The opposite of dimension.
     */
    constexpr Dimension operator-() const
    {
        return Dimension(-value_, unit_);
    }

    /*
     * Minus a dimension to itself using the same unit.
     */
    Dimension& operator-=(const Dimension& dimension)
    {
        ACE_DCHECK(unit_ == dimension.unit_);
        value_ -= dimension.value_;
        return *this;
    }

    bool operator>(const Dimension& dimension) const
    {
        ACE_DCHECK(unit_ == dimension.unit_);
        return (value_ > dimension.value_);
    }

    bool operator<(const Dimension& dimension) const
    {
        ACE_DCHECK(unit_ == dimension.unit_);
        return (value_ < dimension.value_);
    }

    std::string ToString() const;

private:
    double value_ = 0.0;
    DimensionUnit unit_ = DimensionUnit::PX;
};

// literal operators for dimension
inline constexpr Dimension operator""_vp(long double value)
{
    return Dimension(static_cast<double>(value), DimensionUnit::VP);
}

inline constexpr Dimension operator""_px(long double value)
{
    return Dimension(static_cast<double>(value), DimensionUnit::PX);
}

inline constexpr Dimension operator""_fp(long double value)
{
    return Dimension(static_cast<double>(value), DimensionUnit::FP);
}

inline constexpr Dimension operator""_pct(long double value)
{
    return Dimension(static_cast<double>(value), DimensionUnit::PERCENT);
}

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_DIMENSION_H
