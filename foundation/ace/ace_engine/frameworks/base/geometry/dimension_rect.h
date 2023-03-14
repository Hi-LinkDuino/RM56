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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_DIMENSION_RECT_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_DIMENSION_RECT_H

#include <cmath>
#include <limits>

#include "base/geometry/dimension.h"
#include "base/geometry/dimension_offset.h"
#include "base/geometry/dimension_size.h"

namespace OHOS::Ace {

class DimensionRect {
public:
    DimensionRect() = default;
    ~DimensionRect() = default;

    DimensionRect(const Dimension& width, const Dimension& height, const DimensionOffset& offset)
        : width_(width), height_(height), offset_(offset)
    {}

    DimensionRect(const Dimension& width, const Dimension& height) : width_(width), height_(height) {}

    const Dimension& GetWidth() const
    {
        return width_;
    }

    const Dimension& GetHeight() const
    {
        return height_;
    }

    const DimensionOffset& GetOffset() const
    {
        return offset_;
    }

    void SetOffset(const DimensionOffset& offset)
    {
        offset_ = offset;
    }

    void SetSize(const DimensionSize& size)
    {
        width_ = size.Width();
        height_ = size.Height();
    }


    void SetWidth(const Dimension& width)
    {
        width_ = width;
    }

    void SetHeight(const Dimension& height)
    {
        height_ = height;
    }


    void Reset()
    {
        width_ = 0.0_vp;
        height_ = 0.0_vp;
        offset_ = DimensionOffset();
    }

private:
    Dimension width_ = 0.0_vp;
    Dimension height_ = 0.0_vp;
    DimensionOffset offset_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_DIMENSION_RECT_H
