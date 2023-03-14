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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_DIMENSION_OFFSET_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_DIMENSION_OFFSET_H

#include <cmath>
#include <limits>

#include "base/geometry/dimension.h"
#include "base/geometry/offset.h"

namespace OHOS::Ace {

class DimensionOffset {
public:
    DimensionOffset() = default;
    ~DimensionOffset() = default;
    DimensionOffset(Dimension deltaX, Dimension deltaY) : deltaX_(deltaX), deltaY_(deltaY) {}
    DimensionOffset(Offset offset) : deltaX_(Dimension(offset.GetX(), DimensionUnit::PX)),
        deltaY_(Dimension(offset.GetY(), DimensionUnit::PX)) {}

    const Dimension& GetX() const
    {
        return deltaX_;
    }

    const Dimension& GetY() const
    {
        return deltaY_;
    }

    void SetX(Dimension& x)
    {
        deltaX_ = x;
    }

    void SetY(Dimension& y)
    {
        deltaY_ = y;
    }

    DimensionOffset operator+(const DimensionOffset& dimensionOffset) const
    {
        return DimensionOffset(deltaX_ + dimensionOffset.deltaX_, deltaY_ + dimensionOffset.deltaY_);
    }

    DimensionOffset operator-(const DimensionOffset& dimensionOffset) const
    {
        return DimensionOffset(deltaX_ - dimensionOffset.deltaX_, deltaY_ - dimensionOffset.deltaY_);
    }

    DimensionOffset operator*(double value) const
    {
        return DimensionOffset(deltaX_ * value, deltaY_ * value);
    }

private:
    Dimension deltaX_;
    Dimension deltaY_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_DIMENSION_OFFSET_H
