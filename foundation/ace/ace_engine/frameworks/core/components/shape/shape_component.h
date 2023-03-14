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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHAPE_SHAPE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHAPE_SHAPE_COMPONENT_H

#include "core/pipeline/base/measurable.h"
#include "frameworks/core/components/common/properties/animatable_path.h"
#include "frameworks/core/components/common/properties/radius.h"
#include "frameworks/core/components/common/properties/svg_paint_state.h"
#include "frameworks/core/pipeline/base/render_component.h"

namespace OHOS::Ace {

using ShapePoint = std::pair<Dimension, Dimension>;
using ShapePoints = std::vector<ShapePoint>;

enum class ShapeType {
    RECT = 0,
    CIRCLE,
    ELLIPSE,
    LINE,
    POLYGON,
    POLYLINE,
    PATH,
};

// keep same as the doc
static const char* SHAPE_TYPE_STRINGS[] = { "Rect", "Circle", "Ellipse", "Line", "Polygon", "Polyline", "Path"};

class ACE_EXPORT ShapeComponent : public RenderComponent, public Measurable {
    DECLARE_ACE_TYPE(ShapeComponent, RenderComponent, Measurable);

public:
    explicit ShapeComponent(ShapeType shapeType) : RenderComponent(), shapeType_(shapeType) {}
    ~ShapeComponent() override = default;
    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void SetTopLeftRadius(const Radius& topLeftRadius)
    {
        topLeftRadius_ = topLeftRadius;
    }

    void SetTopRightRadius(const Radius& topRightRadius)
    {
        topRightRadius_ = topRightRadius;
    }

    void SetBottomRightRadius(const Radius& bottomRightRadius)
    {
        bottomRightRadius_ = bottomRightRadius;
    }

    void SetBottomLeftRadius(const Radius& bottomLeftRadius)
    {
        bottomLeftRadius_ = bottomLeftRadius;
    }

    void SetStart(const ShapePoint& start)
    {
        start_ = start;
    }

    void SetEnd(const ShapePoint& end)
    {
        end_ = end;
    }

    void SetPoints(const ShapePoints& points)
    {
        points_ = points;
    }

    void SetPathCmd(const std::string& pathCmd, const AnimationOption& option = AnimationOption())
    {
        pathCmd_ = AnimatablePath(pathCmd, option);
    }

    ShapeType GetShapeType() const
    {
        return shapeType_;
    }

    Radius GetTopLeftRadius() const
    {
        return topLeftRadius_;
    }

    Radius GetTopRightRadius() const
    {
        return topRightRadius_;
    }

    Radius GetBottomRightRadius() const
    {
        return bottomRightRadius_;
    }

    Radius GetBottomLeftRadius() const
    {
        return bottomLeftRadius_;
    }

    void SetRadiusWidth(const Dimension& value, const AnimationOption& option = AnimationOption())
    {
        topLeftRadius_.SetX(value, option);
        topRightRadius_.SetX(value, option);
        bottomRightRadius_.SetX(value, option);
        bottomLeftRadius_.SetX(value, option);
    }

    void SetRadiusHeight(const Dimension& value, const AnimationOption& option = AnimationOption())
    {
        topLeftRadius_.SetY(value, option);
        topRightRadius_.SetY(value, option);
        bottomRightRadius_.SetY(value, option);
        bottomLeftRadius_.SetY(value, option);
    }

    ShapePoint GetStart() const
    {
        return start_;
    }

    ShapePoint GetEnd() const
    {
        return end_;
    }

    const AnimatablePath& GetPathCmd() const
    {
        return pathCmd_;
    }

    const std::vector<ShapePoint>& GetPoints() const
    {
        return points_;
    }

    FillState GetFillState() const
    {
        return fillState_;
    }

    StrokeState GetStrokeState() const
    {
        return strokeState_;
    }

    void SetAntiAlias(bool antiAlias)
    {
        antiAlias_.first = true;
        antiAlias_.second = antiAlias;
    }

    const std::pair<bool, bool>& GetAntiAlias() const
    {
        return antiAlias_;
    }

    void SetStroke(const Color& color, const AnimationOption& option = AnimationOption())
    {
        strokeState_.SetColor(color, true, option);
    }

    void SetFill(const Color& color, const AnimationOption& option = AnimationOption())
    {
        fillState_.SetColor(color, true, option);
    }

    void SetStrokeDashOffset(const Dimension& dashOffset, const AnimationOption& option = AnimationOption())
    {
        strokeState_.SetStrokeDashOffset(dashOffset, true, option);
    }

    void SetStrokeLineCap(LineCapStyle lineCapStyle)
    {
        strokeState_.SetLineCap(lineCapStyle);
    }

    void SetStrokeLineJoin(LineJoinStyle lineJoinStyle)
    {
        strokeState_.SetLineJoin(lineJoinStyle);
    }

    void SetStrokeMiterLimit(double miterLimit)
    {
        strokeState_.SetMiterLimit(miterLimit);
    }

    void SetStrokeOpacity(double opacity, const AnimationOption& option = AnimationOption())
    {
        strokeState_.SetOpacity(std::clamp(opacity, 0.0, 1.0), true, option);
    }

    void SetFillOpacity(double opacity, const AnimationOption& option = AnimationOption())
    {
        fillState_.SetOpacity(std::clamp(opacity, 0.0, 1.0), true, option);
    }

    void SetStrokeWidth(const Dimension& lineWidth, const AnimationOption& option = AnimationOption())
    {
        strokeState_.SetLineWidth(lineWidth, true, option);
    }

    void SetStrokeDashArray(const std::vector<Dimension>& segments)
    {
        strokeState_.SetStrokeDashArray(segments);
    }

    void InheritShapeStyle(const FillState& fillState, const StrokeState& strokeState, bool antiAlias);

private:
    ShapeType shapeType_ = ShapeType::RECT;
    Radius topLeftRadius_ = Radius(-1);
    Radius topRightRadius_ = Radius(-1);
    Radius bottomRightRadius_ = Radius(-1);
    Radius bottomLeftRadius_ = Radius(-1);
    ShapePoint start_;
    ShapePoint end_;
    std::vector<ShapePoint> points_;
    AnimatablePath pathCmd_;
    FillState fillState_;
    StrokeState strokeState_;
    std::pair<bool, bool> antiAlias_ = std::make_pair(false, true);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHAPE_SHAPE_COMPONENT_H
