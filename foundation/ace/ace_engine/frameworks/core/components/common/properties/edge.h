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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_EDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_EDGE_H

#include "base/geometry/animatable_dimension.h"
#include "base/geometry/calc_dimension.h"
#include "base/geometry/dimension.h"
#include "base/geometry/offset.h"
#include "base/geometry/size.h"
#include "base/utils/macros.h"
#include "base/utils/utils.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

// Types of padding and margin. Contains four directions: left, top, right and bottom.
class ACE_EXPORT Edge {
public:
    Edge() = default;
    explicit Edge(double value) : Edge(value, value, value, value) {}
    explicit Edge(const Dimension& value) : Edge(value, value, value, value) {}
    explicit Edge(const std::string& value) : Edge(value, value, value, value) {}
    explicit Edge(const CalcDimension& value) : Edge(value, value, value, value) {}
    Edge(double left, double top, double right, double bottom, DimensionUnit unit = DimensionUnit::PX)
        : left_(Dimension(left, unit)), top_(Dimension(top, unit)), right_(Dimension(right, unit)),
          bottom_(Dimension(bottom, unit)) {}
    Edge(const std::string& left, const std::string& top, const std::string& right, const std::string& bottom,
        DimensionUnit unit = DimensionUnit::CALC) : left_(CalcDimension(left, unit)), top_(CalcDimension(top, unit)),
        right_(CalcDimension(right, unit)), bottom_(CalcDimension(bottom, unit)) {}
    Edge(const CalcDimension& left, const CalcDimension& top, const CalcDimension& right, const CalcDimension& bottom)
        : left_(left), top_(top), right_(right), bottom_(bottom) {}
    Edge(const Dimension& left, const Dimension& top, const Dimension& right, const Dimension& bottom,
         const AnimationOption& option)
        : left_(AnimatableDimension(left, option)), top_(AnimatableDimension(top, option)),
          right_(AnimatableDimension(right, option)), bottom_(AnimatableDimension(bottom, option)) {}
    Edge(const CalcDimension& left, const CalcDimension& top, const CalcDimension& right, const CalcDimension& bottom,
         const AnimationOption& option)
        : left_(AnimatableDimension(left, option)), top_(AnimatableDimension(top, option)),
          right_(AnimatableDimension(right, option)), bottom_(AnimatableDimension(bottom, option)) {}

    virtual ~Edge() = default;

    static const Edge NONE;

    // Parse string to edge, support four formats(value separated by one space):
    // 1. 1px, edge has same value.
    // 2. 1px 2px, top and bottom are 1px, left and right are 2px.
    // 3. 1px 2px 3px, top is 1px, left and right are 2px, bottom is 3px.
    // 4. 1px 2px 3px 4px, top is 1px, right is 2px, bottom is 3px and left is 4px.
    static bool FromString(const std::string& value, Edge& edge);

    bool IsValid() const;
    bool IsEffective() const;
    Size GetLayoutSizeInPx(double dipScale) const;
    Offset GetOffsetInPx(double dipScale) const;
    double HorizontalInPx(double dipScale) const;
    double VerticalInPx(double dipScale) const;

    const AnimatableDimension& Left() const
    {
        return left_;
    }

    virtual void SetLeft(const AnimatableDimension& left)
    {
        left_ = left;
    }

    virtual void SetLeft(const CalcDimension& left)
    {
        left_ = AnimatableDimension(left);
    }


    virtual void SetLeft(const Dimension& left)
    {
        left_ = AnimatableDimension(left);
    }

    const AnimatableDimension& Top() const
    {
        return top_;
    }

    virtual void SetTop(const AnimatableDimension& top)
    {
        top_ = top;
    }

    virtual void SetTop(const CalcDimension& top)
    {
        top_ = AnimatableDimension(top);
    }


    virtual void SetTop(const Dimension& top)
    {
        top_ = AnimatableDimension(top);
    }

    const AnimatableDimension& Right() const
    {
        return right_;
    }

    virtual void SetRight(const AnimatableDimension& right)
    {
        right_ = right;
    }

    virtual void SetRight(const CalcDimension& right)
    {
        right_ = AnimatableDimension(right);
    }

    virtual void SetRight(const Dimension& right)
    {
        right_ = AnimatableDimension(right);
    }

    const AnimatableDimension& Bottom() const
    {
        return bottom_;
    }

    virtual void SetBottom(const AnimatableDimension& bottom)
    {
        bottom_ = bottom;
    }

    virtual void SetBottom(const CalcDimension& bottom)
    {
        bottom_ = AnimatableDimension(bottom);
    }

    virtual void SetBottom(const Dimension& bottom)
    {
        bottom_ = AnimatableDimension(bottom);
    }

    Edge operator+(const Edge& edge) const
    {
        return Edge(left_ + edge.left_, top_ + edge.top_, right_ + edge.right_, bottom_ + edge.bottom_);
    }

    Edge operator-(const Edge& edge) const
    {
        return Edge(left_ - edge.left_, top_ - edge.top_, right_ - edge.right_, bottom_ - edge.bottom_);
    }

    Edge operator*(const double factor) const
    {
        return Edge(left_ * factor, top_ * factor, right_ * factor, bottom_ * factor);
    }

    Edge operator/(const double factor) const
    {
        if (NearZero(factor)) {
            return NONE;
        }
        return Edge(left_ / factor, top_ / factor, right_ / factor, bottom_ / factor);
    }

    bool operator==(const Edge& edge) const
    {
        return (edge.left_ == left_) && (edge.top_ == top_) && (edge.right_ == right_) && (edge.bottom_ == bottom_);
    }

    void SetContextAndCallback(const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
    {
        left_.SetContextAndCallback(context, callback);
        top_.SetContextAndCallback(context, callback);
        right_.SetContextAndCallback(context, callback);
        bottom_.SetContextAndCallback(context, callback);
    }

protected:
    AnimatableDimension left_;
    AnimatableDimension top_;
    AnimatableDimension right_;
    AnimatableDimension bottom_;
};

class EdgePx : public Edge {
public:
    EdgePx() = default;
    explicit EdgePx(double value) : EdgePx(value, value, value, value) {}
    explicit EdgePx(const std::string& value) : EdgePx(value, value, value, value) {}
    EdgePx(double left, double top, double right, double bottom) : Edge(left, top, right, bottom) {}
    EdgePx(const std::string& left, const std::string& top, const std::string& right, const std::string& bottom)
        : Edge(left, top, right, bottom) {}
    ~EdgePx() override = default;

    double LeftPx() const
    {
        return left_.Value();
    }

    double TopPx() const
    {
        return top_.Value();
    }

    double RightPx() const
    {
        return right_.Value();
    }

    double BottomPx() const
    {
        return bottom_.Value();
    }

    void SetLeft(const AnimatableDimension& left) override
    {
        if (left.Unit() != DimensionUnit::PX && left.Unit() != DimensionUnit::CALC) {
            return;
        }
        left_ = left;
    }

    void SetLeft(const Dimension& left) override
    {
        if (left.Unit() != DimensionUnit::PX) {
            return;
        }
        left_ = AnimatableDimension(left);
    }

    void SetLeft(const CalcDimension& left) override
    {
        if (left.Unit() != DimensionUnit::CALC) {
            return;
        }
        left_ = AnimatableDimension(left);
    }

    void SetTop(const AnimatableDimension& top) override
    {
        if (top.Unit() != DimensionUnit::PX && top.Unit() != DimensionUnit::CALC) {
            return;
        }
        top_ = top;
    }

    void SetTop(const Dimension& top) override
    {
        if (top.Unit() != DimensionUnit::PX) {
            return;
        }
        top_ = AnimatableDimension(top);
    }

    void SetTop(const CalcDimension& top) override
    {
        if (top.Unit() != DimensionUnit::CALC) {
            return;
        }
        top_ = AnimatableDimension(top);
    }

    void SetRight(const AnimatableDimension& right) override
    {
        if (right.Unit() != DimensionUnit::PX && right.Unit() != DimensionUnit::CALC) {
            return;
        }
        right_ = right;
    }

    void SetRight(const Dimension& right) override
    {
        if (right.Unit() != DimensionUnit::PX) {
            return;
        }
        right_ = AnimatableDimension(right);
    }

    void SetRight(const CalcDimension& right) override
    {
        if (right.Unit() != DimensionUnit::CALC) {
            return;
        }
        right_ = AnimatableDimension(right);
    }

    void SetBottom(const AnimatableDimension& bottom) override
    {
        if (bottom.Unit() != DimensionUnit::PX && bottom.Unit() != DimensionUnit::CALC) {
            return;
        }
        bottom_ = bottom;
    }

    void SetBottom(const Dimension& bottom) override
    {
        if (bottom.Unit() != DimensionUnit::PX) {
            return;
        }
        bottom_ = AnimatableDimension(bottom);
    }

    void SetBottom(const CalcDimension& bottom) override
    {
        if (bottom.Unit() != DimensionUnit::CALC) {
            return;
        }
        bottom_ = AnimatableDimension(bottom);
    }


    Size GetLayoutSize() const
    {
        return Size(left_.Value() + right_.Value(), top_.Value() + bottom_.Value());
    }

    Offset GetOffset() const
    {
        return Offset(left_.Value(), top_.Value());
    }

    EdgePx operator+(const EdgePx& edge) const
    {
        return EdgePx(LeftPx() + edge.LeftPx(), TopPx() + edge.TopPx(), RightPx() + edge.RightPx(),
            BottomPx() + edge.BottomPx());
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_EDGE_H
