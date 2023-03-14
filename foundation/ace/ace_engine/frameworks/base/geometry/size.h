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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_SIZE_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_SIZE_H

#include <iomanip>
#include <limits>
#include <sstream>
#include <string>

#include "base/utils/utils.h"

namespace OHOS::Ace {

class Size {
public:
    static constexpr double INFINITE_SIZE = std::numeric_limits<double>::max();
    Size() = default;
    ~Size() = default;
    Size(double width, double height) : width_(width), height_(height) {}

    double Width() const
    {
        return width_;
    }

    double Height() const
    {
        return height_;
    }

    void SetWidth(double width)
    {
        width_ = width;
    }

    void SetHeight(double height)
    {
        height_ = height;
    }

    void SetSize(const Size& size)
    {
        width_ = size.Width();
        height_ = size.Height();
    }

    bool IsWidthInfinite() const
    {
        return NearEqual(width_, INFINITE_SIZE);
    }

    bool IsHeightInfinite() const
    {
        return NearEqual(height_, INFINITE_SIZE);
    }

    bool IsInfinite() const
    {
        return NearEqual(width_, INFINITE_SIZE) || NearEqual(height_, INFINITE_SIZE);
    }

    bool IsEmpty() const
    {
        return NearEqual(width_, 0.0) || NearEqual(height_, 0.0);
    }

    Size& AddHeight(double height)
    {
        height_ += height;
        return *this;
    }

    Size& AddWidth(double value)
    {
        width_ += value;
        return *this;
    }

    Size& MinusHeight(double height)
    {
        height_ -= height;
        return *this;
    }

    Size& MinusWidth(double width)
    {
        width_ -= width;
        return *this;
    }

    bool IsValid() const
    {
        return width_ > 0.0 && height_ > 0.0;
    }

    Size operator*(double value) const
    {
        return Size(width_ * value, height_ * value);
    }

    bool operator==(const Size& size) const
    {
        return NearEqual(width_, size.width_) && NearEqual(height_, size.height_);
    }

    bool operator!=(const Size& size) const
    {
        return !operator==(size);
    }

    Size operator+(const Size& size) const
    {
        return Size(width_ + size.Width(), height_ + size.Height());
    }

    Size operator+=(const Size& size)
    {
        width_ += size.Width();
        height_ += size.Height();
        return *this;
    }

    Size operator-(const Size& size) const
    {
        return Size(width_ - size.Width(), height_ - size.Height());
    }

    Size operator-=(const Size& size)
    {
        width_ -= size.Width();
        height_ -= size.Height();
        return *this;
    }

    void ApplyScale(double scale)
    {
        width_ *= scale;
        height_ *= scale;
    }

    /*
     * Please make sure that two sizes are both valid.
     * You can use IsValid() to see if a size is valid.
     */
    bool operator>(const Size& size) const
    {
        if (IsValid() && size.IsValid()) {
            return GreatOrEqual(width_, size.width_) && GreatOrEqual(height_, size.height_);
        } else {
            return false;
        }
    }

    /*
     * Please make sure that two sizes are both valid.
     * You can use IsValid() to see if a size is valid.
     */
    bool operator<(const Size& size) const
    {
        if (IsValid() && size.IsValid()) {
            return LessOrEqual(width_, size.width_) && LessOrEqual(height_, size.height_);
        } else {
            return false;
        }
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
        if (NearEqual(width_, INFINITE_SIZE)) {
            ss << "INFINITE";
        } else {
            ss << width_;
        }
        ss << " x ";
        if (NearEqual(height_, INFINITE_SIZE)) {
            ss << "INFINITE";
        } else {
            ss << height_;
        }
        ss << "]";
        std::string output = ss.str();
        return output;
    }

private:
    double width_ = 0.0;
    double height_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_SIZE_H
