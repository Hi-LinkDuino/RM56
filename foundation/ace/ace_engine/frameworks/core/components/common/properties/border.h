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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_BORDER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_BORDER_H

#include "base/geometry/offset.h"
#include "base/geometry/size.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/border_edge.h"
#include "core/components/common/properties/border_image_edge.h"
#include "core/components/common/properties/radius.h"

namespace OHOS::Ace {

// Border of a box, contains four borderEdges: left, top, right, bottom.
// And four radius: topLeftRadius, topRightRadius, bottomLeftRadius, bottomRightRadius.
// Each borderEdge is a BorderEdge object and each radius is a Radius object.
class ACE_EXPORT Border final {
public:
    Border() = default;
    explicit Border(const BorderEdge& edge) : Border(edge, edge, edge, edge) {}

    explicit Border(const BorderImageEdge& edge) : Border(edge, edge, edge, edge) {}

    Border(const BorderEdge& edge, const Radius& radius) : Border(edge, edge, edge, edge)
    {
        SetBorderRadius(radius);
    }
    Border(const BorderEdge& left, const BorderEdge& top, const BorderEdge& right, const BorderEdge& bottom);

    Border(const BorderImageEdge& leftImage, const BorderImageEdge& topImage,
        const BorderImageEdge& rightImage, const BorderImageEdge& bottomImage);

    ~Border() = default;

    bool IsAllEqual() const;
    bool HasValue() const;
    bool HasRadius() const;
    Offset GetOffset(double dipScale) const;
    double HorizontalWidth(double dipScale) const;
    double VerticalWidth(double dipScale) const;
    Size GetLayoutSize(double dipScale) const;
    BorderEdge GetValidEdge() const;

    bool IsValid() const
    {
        return left_.IsValid() && right_.IsValid() && bottom_.IsValid() && top_.IsValid() && topLeftRadius_.IsValid() &&
               bottomLeftRadius_.IsValid() && topRightRadius_.IsValid() && bottomRightRadius_.IsValid();
    }
    bool BorderImageIsValid() const
    {
        return borderImageLeft_.IsValid() && borderImageRight_.IsValid() &&
               borderImageBottom_.IsValid() && borderImageTop_.IsValid();
    }

    bool IsAllSolidStyle() const
    {
        return left_.GetBorderStyle() == BorderStyle::SOLID && bottom_.GetBorderStyle() == BorderStyle::SOLID &&
               right_.GetBorderStyle() == BorderStyle::SOLID && top_.GetBorderStyle() == BorderStyle::SOLID;
    }

    void SetBorderRadius(const Radius& radius)
    {
        topLeftRadius_ = radius;
        topRightRadius_ = radius;
        bottomLeftRadius_ = radius;
        bottomRightRadius_ = radius;
    }

    void SetBorderEdge(const BorderEdge& borderEdge)
    {
        top_ = borderEdge;
        left_ = borderEdge;
        right_ = borderEdge;
        bottom_ = borderEdge;
    }

    void SetBorderImageEdge(const BorderImageEdge& borderImageEdge)
    {
        borderImageLeft_ = borderImageEdge;
        borderImageTop_ = borderImageEdge;
        borderImageRight_ = borderImageEdge;
        borderImageBottom_ = borderImageEdge;
    }

    void SetTopLeftRadius(const Radius& topLeftRadius)
    {
        topLeftRadius_ = topLeftRadius;
    }

    void SetTopRightRadius(const Radius& topRightRadius)
    {
        topRightRadius_ = topRightRadius;
    }

    void SetBottomLeftRadius(const Radius& bottomLeftRadius)
    {
        bottomLeftRadius_ = bottomLeftRadius;
    }

    void SetBottomRightRadius(const Radius& bottomRightRadius)
    {
        bottomRightRadius_ = bottomRightRadius;
    }

    const Radius& TopLeftRadius() const
    {
        return topLeftRadius_;
    }

    const Radius& TopRightRadius() const
    {
        return topRightRadius_;
    }

    const Radius& BottomLeftRadius() const
    {
        return bottomLeftRadius_;
    }

    const Radius& BottomRightRadius() const
    {
        return bottomRightRadius_;
    }

    const BorderEdge& Left() const
    {
        return left_;
    }

    const BorderEdge& Top() const
    {
        return top_;
    }

    const BorderEdge& Right() const
    {
        return right_;
    }

    const BorderEdge& Bottom() const
    {
        return bottom_;
    }

    bool operator==(const Border& border) const
    {
        return (border.Left() == left_) && (border.Top() == top_) && (border.Right() == right_) &&
               (border.Bottom() == bottom_);
    }

    void SetLeftEdge(const BorderEdge& edge)
    {
        left_ = edge;
    }

    void SetTopEdge(const BorderEdge& edge)
    {
        top_ = edge;
    }

    void SetRightEdge(const BorderEdge& edge)
    {
        right_ = edge;
    }

    void SetBottomEdge(const BorderEdge& edge)
    {
        bottom_ = edge;
    }

    void SetWidth(const Dimension& width, const AnimationOption& option = AnimationOption())
    {
        SetLeftWidth(width, option);
        SetTopWidth(width, option);
        SetRightWidth(width, option);
        SetBottomWidth(width, option);
    }

    void SetLeftWidth(const Dimension& width, const AnimationOption& option = AnimationOption())
    {
        left_.SetWidth(width, option);
    }

    void SetTopWidth(const Dimension& width, const AnimationOption& option = AnimationOption())
    {
        top_.SetWidth(width, option);
    }

    void SetRightWidth(const Dimension& width, const AnimationOption& option = AnimationOption())
    {
        right_.SetWidth(width, option);
    }

    void SetBottomWidth(const Dimension& width, const AnimationOption& option = AnimationOption())
    {
        bottom_.SetWidth(width, option);
    }

    void SetStyle(BorderStyle style)
    {
        SetLeftStyle(style);
        SetTopStyle(style);
        SetRightStyle(style);
        SetBottomStyle(style);
    }

    void SetLeftStyle(BorderStyle style)
    {
        left_.SetStyle(style);
    }

    void SetTopStyle(BorderStyle style)
    {
        top_.SetStyle(style);
    }

    void SetRightStyle(BorderStyle style)
    {
        right_.SetStyle(style);
    }

    void SetBottomStyle(BorderStyle style)
    {
        bottom_.SetStyle(style);
    }

    void SetColor(const Color& color, const AnimationOption& option = AnimationOption())
    {
        SetLeftColor(color, option);
        SetTopColor(color, option);
        SetRightColor(color, option);
        SetBottomColor(color, option);
    }

    void SetLeftColor(const Color& color, const AnimationOption& option = AnimationOption())
    {
        left_.SetColor(color, option);
    }

    void SetTopColor(const Color& color, const AnimationOption& option = AnimationOption())
    {
        top_.SetColor(color, option);
    }

    void SetRightColor(const Color& color, const AnimationOption& option = AnimationOption())
    {
        right_.SetColor(color, option);
    }

    void SetBottomColor(const Color& color, const AnimationOption& option = AnimationOption())
    {
        bottom_.SetColor(color, option);
    }

    void SetContextAndCallback(const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
    {
        left_.SetContextAndCallback(context, callback);
        top_.SetContextAndCallback(context, callback);
        right_.SetContextAndCallback(context, callback);
        bottom_.SetContextAndCallback(context, callback);

        topLeftRadius_.SetContextAndCallback(context, callback);
        topRightRadius_.SetContextAndCallback(context, callback);
        bottomLeftRadius_.SetContextAndCallback(context, callback);
        bottomRightRadius_.SetContextAndCallback(context, callback);
    }

    void SetBorderImageWidth(const Dimension& width, BorderImageDirection direction)
    {
        if (direction == BorderImageDirection::TOP) {
            borderImageTop_.SetBorderImageWidth(width);
        } else if (direction == BorderImageDirection::LEFT) {
            borderImageLeft_.SetBorderImageWidth(width);
        } else if (direction == BorderImageDirection::RIGHT) {
            borderImageRight_.SetBorderImageWidth(width);
        } else if (direction == BorderImageDirection::BOTTOM) {
            borderImageBottom_.SetBorderImageWidth(width);
        }
    }
    void SetBorderImageSlice(const Dimension& slice, BorderImageDirection direction)
    {
        if (direction == BorderImageDirection::TOP) {
            borderImageTop_.SetBorderImageSlice(slice);
        } else if (direction == BorderImageDirection::LEFT) {
            borderImageLeft_.SetBorderImageSlice(slice);
        } else if (direction == BorderImageDirection::RIGHT) {
            borderImageRight_.SetBorderImageSlice(slice);
        } else if (direction == BorderImageDirection::BOTTOM) {
            borderImageBottom_.SetBorderImageSlice(slice);
        }
    }
    void SetBorderImageOutset(const Dimension& outset, BorderImageDirection direction)
    {
        if (direction == BorderImageDirection::TOP) {
            borderImageTop_.SetBorderImageOutset(outset);
        } else if (direction == BorderImageDirection::LEFT) {
            borderImageLeft_.SetBorderImageOutset(outset);
        } else if (direction == BorderImageDirection::RIGHT) {
            borderImageRight_.SetBorderImageOutset(outset);
        } else if (direction == BorderImageDirection::BOTTOM) {
            borderImageBottom_.SetBorderImageOutset(outset);
        }
    }

    void SetBorderImageRepeat(BorderImageRepeat repeat)
    {
        borderImageTop_.SetBorderImageRepeat(repeat);
        borderImageLeft_.SetBorderImageRepeat(repeat);
        borderImageRight_.SetBorderImageRepeat(repeat);
        borderImageBottom_.SetBorderImageRepeat(repeat);
    }
    const BorderImageEdge& ImageLeftEdge() const
    {
        return borderImageLeft_;
    }

    const BorderImageEdge& ImageTopEdge() const
    {
        return borderImageTop_;
    }

    const BorderImageEdge& ImageRightEdge() const
    {
        return borderImageRight_;
    }

    const BorderImageEdge& ImageBottomEdge() const
    {
        return borderImageBottom_;
    }

private:
    BorderEdge left_;
    BorderEdge top_;
    BorderEdge right_;
    BorderEdge bottom_;
    BorderImageEdge borderImageLeft_;
    BorderImageEdge borderImageTop_;
    BorderImageEdge borderImageRight_;
    BorderImageEdge borderImageBottom_;
    Radius topLeftRadius_;
    Radius topRightRadius_;
    Radius bottomLeftRadius_;
    Radius bottomRightRadius_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_BORDER_H
