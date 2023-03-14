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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_RECT_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_RECT_H

#include <algorithm>

#include "base/geometry/offset.h"
#include "base/geometry/point.h"
#include "base/geometry/size.h"

namespace OHOS::Ace {

class Rect {
public:
    Rect() = default;
    ~Rect() = default;

    Rect(double x, double y, double width, double height)
    {
        SetRect(x, y, width, height);
    }

    Rect(const Offset& offset, const Size& size)
    {
        SetOffset(offset);
        SetSize(size);
    }

    void SetRect(double x, double y, double width, double height)
    {
        x_ = x;
        y_ = y;
        width_ = width;
        height_ = height;
    }

    void SetRect(const Offset& offset, const Size& size)
    {
        SetOffset(offset);
        SetSize(size);
    }

    void ApplyScale(double scale)
    {
        x_ *= scale;
        y_ *= scale;
        width_ *= scale;
        height_ *= scale;
    }

    void ApplyScaleAndRound(const Size& scale)
    {
        x_ = round(x_ * scale.Width());
        y_ = round(y_ * scale.Height());
        width_ = round(width_ * scale.Width());
        height_ = round(height_ * scale.Height());
    }

    double Left() const
    {
        return GreatNotEqual(width_, 0.0) ? x_ : x_ + width_;
    }

    double Top() const
    {
        return GreatNotEqual(height_, 0.0) ? y_ : y_ + height_;
    }

    double Right() const
    {
        return GreatNotEqual(width_, 0.0) ? x_ + width_ : x_;
    }

    double Bottom() const
    {
        return GreatNotEqual(height_, 0.0) ? y_ + height_ : y_;
    }

    double Width() const
    {
        return width_;
    }

    double Height() const
    {
        return height_;
    }

    void SetSize(const Size& size)
    {
        width_ = size.Width();
        height_ = size.Height();
    }

    Size GetSize() const
    {
        return Size(width_, height_);
    }

    void SetOffset(const Offset& offset)
    {
        x_ = offset.GetX();
        y_ = offset.GetY();
    }

    Offset GetOffset() const
    {
        return Offset(x_, y_);
    }

    void SetLeft(double left)
    {
        x_ = left;
    }

    void SetTop(double top)
    {
        y_ = top;
    }

    void SetWidth(double width)
    {
        width_ = width;
    }

    void SetHeight(double height)
    {
        height_ = height;
    }

    bool IsInRegion(const Point& point) const
    {
        return (point.GetX() >= x_) && (point.GetX() < (x_ + width_)) && (point.GetY() >= y_) &&
               (point.GetY() < (y_ + height_));
    }

    bool IsWrappedBy(const Rect& other) const
    {
        return (Left() >= other.Left()) && (Right() <= other.Right()) && (Top() >= other.Top()) &&
               (Bottom() <= other.Bottom());
    }

    bool IsValid() const
    {
        return width_ > 0.0 && height_ > 0.0;
    }

    Rect Constrain(const Rect& other)
    {
        double right = Right();
        double bottom = Bottom();
        double left = std::clamp(x_, other.Left(), other.Right());
        double top = std::clamp(y_, other.Top(), other.Bottom());
        right = std::clamp(right, other.Left(), other.Right()) - left;
        bottom = std::clamp(bottom, other.Top(), other.Bottom()) - top;
        return Rect(left, top, right, bottom);
    }

    Rect& operator+=(const Offset& offset)
    {
        x_ += offset.GetX();
        y_ += offset.GetY();
        return *this;
    }

    Rect& operator-=(const Offset& offset)
    {
        x_ -= offset.GetX();
        y_ -= offset.GetY();
        return *this;
    }

    Rect& operator+=(const Size& size)
    {
        width_ += size.Width();
        height_ += size.Height();
        return *this;
    }

    Rect& operator-=(const Size& size)
    {
        width_ -= size.Width();
        height_ -= size.Height();
        return *this;
    }

    Rect operator+(const Offset& offset) const
    {
        return Rect(x_ + offset.GetX(), y_ + offset.GetY(), width_, height_);
    }

    Rect operator-(const Offset& offset) const
    {
        return Rect(x_ - offset.GetX(), y_ - offset.GetY(), width_, height_);
    }

    Rect operator+(const Size& size) const
    {
        return Rect(x_, y_, width_ + size.Width(), height_ + size.Height());
    }

    Rect operator-(const Size& size) const
    {
        return Rect(x_, y_, width_ - size.Width(), height_ - size.Height());
    }

    Rect operator*(double scale) const
    {
        return Rect(x_ * scale, y_ * scale, width_ * scale, height_ * scale);
    }

    bool operator==(const Rect& rect) const
    {
        return (GetOffset() == rect.GetOffset()) && (GetSize() == rect.GetSize());
    }

    bool operator!=(const Rect& rect) const
    {
        return !operator==(rect);
    }

    bool IsIntersectWith(const Rect& other) const
    {
        return !(other.Right() < Left() || other.Left() > Right() || other.Bottom() < Top() || other.Top() > Bottom());
    }

    bool IsIntersectByCommonSideWith(const Rect& other) const
    {
        return !(
            other.Right() <= Left() || other.Left() >= Right() || other.Bottom() <= Top() || other.Top() >= Bottom());
    }

    Rect IntersectRect(const Rect& other) const
    {
        double left = std::max(Left(), other.Left());
        double right = std::min(Right(), other.Right());
        double top = std::max(Top(), other.Top());
        double bottom = std::min(Bottom(), other.Bottom());
        return Rect(left, top, right - left, bottom - top);
    }

    Rect CombineRect(const Rect& other) const
    {
        double left = std::min(Left(), other.Left());
        double right = std::max(Right(), other.Right());
        double top = std::min(Top(), other.Top());
        double bottom = std::max(Bottom(), other.Bottom());
        return Rect(left, top, right - left, bottom - top);
    }

    /**
     * @brief Magnetically attracted to a "magnetic" rect.
     *
     * Let's show some cases to illustrate how this method works:
     *
     * Case 1 : Inside. Rect won't be move because it is already attracted by magnet.
     * Result: Offset(0, 0)
     *
     * +-----------------------------+
     * |                             |
     * |   Magnetical Rect           |
     * |                             |
     * |          +-------+          |
     * |          |  R    |          |
     * |          |       |          |
     * |          +-------+          |
     * +-----------------------------+
     *
     * Case 2: Outside. R will be attracted to position R'
     * Result: Offset(-12, -3)
     *
     * +-----------------------+
     * |                       |
     * |   Magnetical Rect  +--+
     * |                    |  |
     * |                    |R'|        +--+
     * +-----------------------+        |R |
     *                                  |  |
     *                                  +--+
     *
     * Case 3: Half Inside. R will be moved totally into magnet.
     * Result: Offset(-8, 0)
     *
     * +-----------------------------+                  +-----------------------------+
     * |                             |                  |                             |
     * |   Magnetical Rect           |   +---------->   |   Magnetical Rect           |
     * |                             |                  |                             |
     * |                          +----------+          |                  +----------+
     * |                          |  |   R   |          |                  |   R'     |
     * |                          +----------+          |                  +----------+
     * |                             |                  |                             |
     * +-----------------------------+                  +-----------------------------+
     *
     * Case 4: Outside or Half Outside but space not enough. R will be moved into magnet as more as possible.
     * Result: Offset(0, 1)
     *
     *                    +--+
     *                    |  |                                     +--+
     *                    |R |                                     |  |
     *                    |  |                                     |R'|
     * +---------------------------+            +---------------------------+
     * |                  |  |     |            |                  |  |     |
     * | Magnetical Rect  |  |     |  +----->   | Magnetical Rect  |  |     |
     * |                  +--+     |            |                  |  |     |
     * +---------------------------+            +------------------+--+-----+
     *
     * Case 5: Totally Across magnet. Nothing should happen.
     * Result: Offset(0, 0)
     *
     *                    +--+
     *                    |  |
     *                    |R |
     *                    |  |
     * +---------------------------+
     * |                  |  |     |
     * | Magnetical Rect  |  |     |
     * |                  |  |     |
     * +---------------------------+
     *                    |  |
     *                    +--+
     *
     * @param[in] magnet The magnetical rectangle.
     *
     * @return The offset that this rect need to moving into magnet.
     */
    Offset MagneticAttractedBy(const Rect& magnet)
    {
        Offset offset = Offset::Zero();
        if (IsWrappedBy(magnet)) {
            return Offset::Zero();
        }

        if (Left() < magnet.Left()) {
            offset.SetX(std::max(0.0, std::min(magnet.Left() - Left(), magnet.Right() - Right())));
        } else if (Right() > magnet.Right()) {
            offset.SetX(std::min(0.0, std::max(magnet.Left() - Left(), magnet.Right() - Right())));
        }

        if (Top() < magnet.Top()) {
            offset.SetY(std::max(0.0, std::min(magnet.Top() - Top(), magnet.Bottom() - Bottom())));
        } else if (Bottom() > magnet.Bottom()) {
            offset.SetY(std::min(0.0, std::max(magnet.Top() - Top(), magnet.Bottom() - Bottom())));
        }

        *this += offset;

        return offset;
    }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << "Rect (" << std::fixed << std::setprecision(2) << x_ << ", " << y_ << ") - [";
        if (NearEqual(width_, Size::INFINITE_SIZE)) {
            ss << "INFINITE";
        } else {
            ss << width_;
        }
        ss << " x ";
        if (NearEqual(height_, Size::INFINITE_SIZE)) {
            ss << "INFINITE";
        } else {
            ss << height_;
        }
        ss << "]";
        std::string output = ss.str();
        return output;
    }

    std::string ToBounds() const
    {
        std::stringstream ss;
        ss << "[" << std::fixed << std::setprecision(2) << x_ << ", " << y_ << "][";
        if (NearEqual(width_, Size::INFINITE_SIZE)) {
            ss << "INFINITE";
        } else {
            ss << (x_ + width_);
        }
        ss << ",";
        if (NearEqual(height_, Size::INFINITE_SIZE)) {
            ss << "INFINITE";
        } else {
            ss << (y_ + height_);
        }
        ss << "]";
        std::string output = ss.str();
        return output;
    }

    Offset Center() const
    {
        return Offset(width_ / 2.0 + x_, height_ / 2.0 + y_);
    }

private:
    double x_ = 0.0;
    double y_ = 0.0;
    double width_ = 0.0;
    double height_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_RECT_H
