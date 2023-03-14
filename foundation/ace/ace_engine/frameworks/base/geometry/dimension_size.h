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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_DIMENSION_SIZE_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_DIMENSION_SIZE_H

#include <iomanip>
#include <limits>
#include <sstream>
#include <string>

#include "base/geometry/dimension.h"
#include "base/utils/utils.h"

namespace OHOS::Ace {

class DimensionSize {
public:
    static constexpr double INFINITE_SIZE = std::numeric_limits<double>::max();
    DimensionSize() = default;
    ~DimensionSize() = default;
    DimensionSize(const Dimension& width, const Dimension& height) : width_(width), height_(height) {}

    const Dimension& Width() const
    {
        return width_;
    }

    const Dimension& Height() const
    {
        return height_;
    }

    void SetWidth(const Dimension& width)
    {
        width_ = width;
    }

    void SetHeight(const Dimension& height)
    {
        height_ = height;
    }

    void SetSize(const DimensionSize& size)
    {
        width_ = size.Width();
        height_ = size.Height();
    }

    template<class T>
    static double CalcRatio(const T& rectangle)
    {
        if (NearZero(static_cast<double>(rectangle.Height()))) {
            return INFINITE_SIZE;
        }
        return static_cast<double>(rectangle.Width()) / static_cast<double>(rectangle.Height());
    }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << "[" << std::fixed << std::setprecision(2);
        if (NearEqual(width_.Value(), INFINITE_SIZE)) {
            ss << "INFINITE";
        } else {
            ss << width_.ToString();
        }
        ss << " x ";
        if (NearEqual(height_.Value(), INFINITE_SIZE)) {
            ss << "INFINITE";
        } else {
            ss << height_.ToString();
        }
        ss << "]";
        std::string output = ss.str();
        return output;
    }

private:
    Dimension width_ = 0.0_vp;
    Dimension height_ = 0.0_vp;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_DIMENSION_SIZE_H
