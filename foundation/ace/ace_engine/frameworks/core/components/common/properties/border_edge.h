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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_BORDER_EDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_BORDER_EDGE_H

#include "base/geometry/animatable_dimension.h"
#include "base/geometry/dimension.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/animatable_color.h"
#include "core/components/common/properties/color.h"

namespace OHOS::Ace {

// A edge of a Border.
class ACE_EXPORT BorderEdge final {
public:
    BorderEdge() = default;
    ~BorderEdge() = default;
    BorderEdge(const Color& color, const Dimension& width, BorderStyle style);

    bool HasValue() const;

    const AnimatableColor& GetColor() const
    {
        return color_;
    }

    BorderStyle GetBorderStyle() const
    {
        return style_;
    }

    const AnimatableDimension& GetWidth() const
    {
        return width_;
    }

    double GetWidthInPx(double dipScale) const
    {
        return width_.ConvertToPx(dipScale);
    }

    bool operator==(const BorderEdge& borderEdge) const
    {
        return (borderEdge.GetColor() == color_) && NearEqual(borderEdge.GetWidth().Value(), width_.Value()) &&
               (borderEdge.GetWidth().Unit() == width_.Unit()) && (borderEdge.GetBorderStyle() == style_);
    }

    void SetWidth(const Dimension& width, const AnimationOption& option = AnimationOption())
    {
        if (width.Value() < 0.0) {
            width_.SetValue(0.0);
            return;
        }
        width_ = AnimatableDimension(width, option);
    }

    void SetWidth(const AnimatableDimension& width)
    {
        if (width.Value() < 0.0) {
            width_.SetValue(0.0);
            return;
        }
        width_ = width;
    }

    void SetColor(const Color& color, const AnimationOption& option = AnimationOption())
    {
        color_ = AnimatableColor(color, option);
    }

    void SetColor(const AnimatableColor& color)
    {
        color_ = color;
    }

    void SetStyle(BorderStyle style)
    {
        style_ = style;
    }

    bool IsValid() const
    {
        return width_.IsValid();
    }

    std::string ToString() const
    {
        return std::string("BorderEdge(")
            .append("width:")
            .append(std::to_string(width_.Value()))
            .append(" color:")
            .append(std::to_string(color_.GetValue()))
            .append(" style:")
            .append(std::to_string((int)style_))
            .append(")");
    }

    void SetContextAndCallback(const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
    {
        width_.SetContextAndCallback(context, callback);
        color_.SetContextAndCallback(context, callback);
    }

private:
    AnimatableColor color_;
    AnimatableDimension width_;
    BorderStyle style_ { BorderStyle::NONE };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_BORDER_EDGE_H
