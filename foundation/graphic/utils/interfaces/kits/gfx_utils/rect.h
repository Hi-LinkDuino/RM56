/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

/**
 * @addtogroup UI_Utils
 * @{
 *
 * @brief Defines basic UI utils.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file rect.h
 *
 * @brief Defines a rectangle, including the position data of the four boundaries of the rectangle, and provides
 *        functions for rectangle inclusion, intersection, and aggregation.
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_RECT_H
#define GRAPHIC_LITE_RECT_H

#include "gfx_utils/graphic_math.h"
#include "gfx_utils/graphic_types.h"
#include "gfx_utils/heap_base.h"

namespace OHOS {
/**
 * @brief Defines a rectangle, including the position data of the four boundaries of the rectangle, and provides
 *        functions for rectangle inclusion, intersection, and aggregation.
 * @since 1.0
 * @version 1.0
 */
template <typename T> class CommonRect : public HeapBase {
public:
    /**
     * @brief A constructor used to create a <b>CommonRect</b> instance.
     * @since 1.0
     * @version 1.0
     */
    CommonRect() : left_(0), top_(0), right_(0), bottom_(0) {}

    /**
     * @brief A destructor used to delete the <b>CommonRect</b> instance.
     * @since 1.0
     * @version 1.0
     */
    ~CommonRect() {}

    /**
     * @brief A constructor used to create a <b>CommonRect</b> instance based on the coordinates of the four boundaries.
     * @param left Indicates the coordinate of the left boundary.
     * @param top Indicates the coordinate of the top boundary.
     * @param right Indicates the coordinate of the right boundary.
     * @param bottom Indicates the coordinate of the bottom boundary.
     * @since 1.0
     * @version 1.0
     */
    CommonRect(T left, T top, T right, T bottom)
    {
        left_ = left;
        right_ = right;
        top_ = top;
        bottom_ = bottom;
    }

    /**
     * @brief A constructor used to create a <b>CommonRect</b> instance by copying another rectangle.
     *
     * @param other Indicates the rectangle to copy.
     * @since 1.0
     * @version 1.0
     */
    CommonRect(const CommonRect<T>& other)

    {
        left_ = other.left_;
        right_ = other.right_;
        top_ = other.top_;
        bottom_ = other.bottom_;
    }

    /**
     * @brief A constructor used to create a <b>CommonRect</b> instance by copying another rectangle.
     *
     * @param other Indicates the rectangle to copy.
     * @since 1.0
     * @version 1.0
     */
    CommonRect(const CommonRect<T>&& other)
    {
        left_ = other.left_;
        right_ = other.right_;
        top_ = other.top_;
        bottom_ = other.bottom_;
    }

    /**
     * @brief Sets the coordinates of the four boundaries of a rectangle.
     *
     * @param left Indicates the coordinate of the left boundary.
     * @param top Indicates the coordinate of the top boundary.
     * @param right Indicates the coordinate of the right boundary.
     * @param bottom Indicates the coordinate of the bottom boundary.
     * @since 1.0
     * @version 1.0
     */
    void SetRect(T left, T top, T right, T bottom)
    {
        left_ = left;
        right_ = right;
        top_ = top;
        bottom_ = bottom;
    }

    /**
     * @brief Obtains the rectangle width.
     * @return Returns the rectangle width.
     * @since 1.0
     * @version 1.0
     */
    T GetWidth() const
    {
        return right_ - left_ + 1;
    }

    /**
     * @brief Obtains the rectangle height.
     * @return Returns the rectangle height.
     * @since 1.0
     * @version 1.0
     */
    T GetHeight() const
    {
        return bottom_ - top_ + 1;
    }

    /**
     * @brief Obtains the left boundary coordinate of the rectangle.
     * @return Returns the left boundary coordinate.
     * @since 1.0
     * @version 1.0
     */
    T GetX() const
    {
        return left_;
    }

    /**
     * @brief Obtains the top boundary coordinate of the rectangle.
     * @return Returns the top boundary coordinate.
     * @since 1.0
     * @version 1.0
     */
    T GetY() const
    {
        return top_;
    }

    /**
     * @brief Obtains the left boundary coordinate of the rectangle.
     * @return Returns the left boundary coordinate.
     * @since 1.0
     * @version 1.0
     */
    T GetLeft() const
    {
        return left_;
    }

    /**
     * @brief Obtains the top boundary coordinate of the rectangle.
     * @return Returns the top boundary coordinate.
     * @since 1.0
     * @version 1.0
     */
    T GetTop() const
    {
        return top_;
    }

    /**
     * @brief Obtains the right boundary coordinate of the rectangle.
     * @return Returns the right boundary coordinate.
     * @since 1.0
     * @version 1.0
     */
    T GetRight() const
    {
        return right_;
    }

    /**
     * @brief Obtains the bottom boundary coordinate of the rectangle.
     * @return Returns the bottom boundary coordinate.
     * @since 1.0
     * @version 1.0
     */
    T GetBottom() const
    {
        return bottom_;
    }

    /**
     * @brief Changes the left boundary coordinate of the rectangle without changing the rectangle width.
     * @param x Indicates the coordinate of the left boundary.
     * @since 1.0
     * @version 1.0
     */
    void SetX(T x)
    {
        right_ += x - left_;
        left_ = x;
    }

    /**
     * @brief Changes the top boundary coordinate of the rectangle without changing the rectangle height.
     * @param y Indicates the coordinate of the top boundary.
     * @since 1.0
     * @version 1.0
     */
    void SetY(T y)
    {
        bottom_ += y - top_;
        top_ = y;
    }

    /**
     * @brief Changes the coordinates of the left and top boundaries of the rectangle without changing the rectangle
     *        width and height.
     * @param x Indicates the coordinate of the left boundary.
     * @param y Indicates the coordinate of the top boundary.
     * @since 1.0
     * @version 1.0
     */
    void SetPosition(T x, T y)
    {
        right_ += x - left_;
        bottom_ += y - top_;
        left_ = x;
        top_ = y;
    }

    /**
     * @brief Changes the width of the rectangle without changing the coordinate of the left boundary.
     * @param width Indicates the width of the rectangle.
     * @since 1.0
     * @version 1.0
     */
    void SetWidth(T width)
    {
        right_ = left_ + width - 1;
    }

    /**
     * @brief Changes the height of the rectangle without changing the coordinate of the top boundary.
     * @param height Indicates the height of the rectangle.
     * @since 1.0
     * @version 1.0
     */
    void SetHeight(T height)
    {
        bottom_ = top_ + height - 1;
    }

    /**
     * @brief Sets the coordinate of the left boundary of a rectangle.
     * @param left Indicates the coordinate of the left boundary.
     * @since 1.0
     * @version 1.0
     */
    void SetLeft(T left)
    {
        left_ = left;
    }

    /**
     * @brief Sets the coordinate of the top boundary of a rectangle.
     * @param top Indicates the coordinate of the top boundary.
     * @since 1.0
     * @version 1.0
     */
    void SetTop(T top)
    {
        top_ = top;
    }

    /**
     * @brief Sets the coordinate of the right boundary of a rectangle.
     * @param right Indicates the coordinate of the right boundary.
     * @since 1.0
     * @version 1.0
     */
    void SetRight(T right)
    {
        right_ = right;
    }

    /**
     * @brief Sets the coordinate of the bottom boundary of a rectangle.
     * @param bottom Indicates the coordinate of the bottom boundary.
     * @since 1.0
     * @version 1.0
     */
    void SetBottom(T bottom)
    {
        bottom_ = bottom;
    }

    /**
     * @brief Sets the width and height of a rectangle.
     * @param width Indicates the width of the rectangle.
     * @param height Indicates the height of the rectangle.
     * @since 1.0
     * @version 1.0
     */
    void Resize(T width, T height)
    {
        right_ = left_ + width - 1;
        bottom_ = top_ + height - 1;
    }

    /**
     * @brief Obtains the area of a rectangle.
     * @return Returns the area of the rectangle.
     * @since 1.0
     * @version 1.0
     */
    uint32_t GetSize() const
    {
        return static_cast<uint32_t>(right_ - left_ + 1) * (bottom_ - top_ + 1);
    }

    /**
     * @brief Checks whether two rectangles intersect.
     * @param rect1 Indicates the first rectangle to check.
     * @param rect2 Indicates the second rectangle to check.
     * @return Returns <b>true</b> if the two rectangles intersect; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool Intersect(const CommonRect<T>& rect1, const CommonRect<T>& rect2)
    {
        /* Get the smaller area from 'rect1' and 'rect2' */
        left_ = MATH_MAX(rect1.left_, rect2.left_);
        top_ = MATH_MAX(rect1.top_, rect2.top_);
        right_ = MATH_MIN(rect1.right_, rect2.right_);
        bottom_ = MATH_MIN(rect1.bottom_, rect2.bottom_);
        if ((left_ > right_) || (top_ > bottom_)) {
            return false;
        }

        return true;
    }

    /**
     * @brief Obtains the minimum rectangle that contains another two rectangles.
     * @param rect1 Indicates the first rectangle to contain.
     * @param rect2 Indicates the second rectangle to contain.
     * @since 1.0
     * @version 1.0
     */
    void Join(const CommonRect<T>& rect1, const CommonRect<T>& rect2)
    {
        left_ = MATH_MIN(rect1.left_, rect2.left_);
        top_ = MATH_MIN(rect1.top_, rect2.top_);
        right_ = MATH_MAX(rect1.right_, rect2.right_);
        bottom_ = MATH_MAX(rect1.bottom_, rect2.bottom_);
    }

    /**
     * @brief Checks whether the rectangle contains a coordinate point.
     * @param point Indicates the coordinate point.
     * @return Returns <b>true</b> if the input coordinate point is contained; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool IsContains(const Vector2<T>& point) const
    {
        bool isContains = false;

        if ((point.x_ >= this->left_) && (point.x_ <= this->right_) && (point.y_ >= this->top_) &&
            (point.y_ <= this->bottom_)) {
            isContains = true;
        }

        return isContains;
    }

    /**
     * @brief Checks whether the rectangle contains a coordinate point.
     * @param point Indicates the coordinate point.
     * @return Returns <b>true</b> if the input coordinate point is contained; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool IsContains(const Point& point) const
    {
        bool isContains = false;

        if ((point.x >= this->left_) && (point.x <= this->right_) && (point.y >= this->top_) &&
            (point.y <= this->bottom_)) {
            isContains = true;
        }

        return isContains;
    }

    /**
     * @brief Checks whether the rectangle is adjacent to another rectangle horizontally or vertically.
     * @param other Indicates the rectangle to be used for check.
     * @return Returns <b>true</b> if the rectangle is adjacent to the input rectangle; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool IsExtends(const CommonRect<T>& other) const
    {
        if (left_ == other.left_ && right_ == other.right_) {
            return (top_ == other.bottom_ + 1) || (bottom_ == other.top_ - 1);
        }

        if (top_ == other.top_ && bottom_ == other.bottom_) {
            return (left_ == other.right_ + 1) || (right_ == other.left_ - 1);
        }

        return false;
    }

    /**
     * @brief Checks whether the rectangle intersects with another rectangle.
     * @param other Indicates the rectangle to be used for check.
     * @return Returns <b>true</b> if the two rectangles intersect; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool IsIntersect(const CommonRect<T>& other) const
    {
        if ((this->left_ <= other.right_) && (this->right_ >= other.left_) && (this->top_ <= other.bottom_) &&
            (this->bottom_ >= other.top_)) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * @brief Checks whether the rectangle contains another rectangle.
     *
     * @param other Indicates the rectangle to be used for check.
     * @return Returns <b>true</b> if the input rectangle is contained; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool IsContains(const CommonRect<T>& other) const
    {
        bool isContains = false;

        if (other.left_ >= this->left_ && other.top_ >= this->top_ && other.right_ <= this->right_ &&
            other.bottom_ <= this->bottom_) {
            isContains = true;
        }

        return isContains;
    }

    void Inflate(T delta);
    void operator=(const CommonRect<T>& other)
    {
        left_ = other.left_;
        right_ = other.right_;
        top_ = other.top_;
        bottom_ = other.bottom_;
    }
    void operator=(const CommonRect<T>&& other)
    {
        left_ = other.left_;
        right_ = other.right_;
        top_ = other.top_;
        bottom_ = other.bottom_;
    }
    bool operator==(const CommonRect<T>& other) const
    {
        if (left_ == other.left_ && right_ == other.right_ && top_ == other.top_ && bottom_ == other.bottom_) {
            return true;
        } else {
            return false;
        }
    }

    void Normalize()
    {
        T t;
        if (left_ > right_) {
            t = left_;
            left_ = right_;
            right_ = t;
        }
        if (top_ > bottom_) {
            t = top_;
            top_ = bottom_;
            bottom_ = t;
        }
    }

protected:
    T left_;
    T top_;
    T right_;
    T bottom_;
};
using Rect = CommonRect<int16_t>;
using Rect32 = CommonRect<int32_t>;
} // namespace OHOS
#endif // GRAPHIC_LITE_RECT_H
