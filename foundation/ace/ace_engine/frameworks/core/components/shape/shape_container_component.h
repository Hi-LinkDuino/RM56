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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHARP_SHAPE_CONTAINER_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHARP_SHAPE_CONTAINER_COMPONENT_H

#include "frameworks/core/components/common/properties/svg_paint_state.h"
#include "frameworks/core/pipeline/base/component_group.h"

namespace OHOS::Ace {

class ShapeViewBox final {
public:
    ShapeViewBox() = default;
    ShapeViewBox(const AnimatableDimension& x, const AnimatableDimension& y,
                 const AnimatableDimension& width, const AnimatableDimension& height)
        : x_(x), y_(y), width_(width), height_(height) {}
    ~ShapeViewBox() = default;

    void SetContextAndCallback(const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
    {
        x_.SetContextAndCallback(context, callback);
        y_.SetContextAndCallback(context, callback);
        width_.SetContextAndCallback(context, callback);
        height_.SetContextAndCallback(context, callback);
    }

    const AnimatableDimension& Left() const
    {
        return x_;
    }

    const AnimatableDimension& Top() const
    {
        return y_;
    }

    const AnimatableDimension& Width() const
    {
        return width_;
    }

    const AnimatableDimension& Height() const
    {
        return height_;
    }

    void SetLeft(const AnimatableDimension& left)
    {
        x_ = left;
    }

    void SetLeft(const Dimension& left, const AnimationOption& option = AnimationOption())
    {
        x_ = AnimatableDimension(left, option);
    }

    void SetTop(const AnimatableDimension& top)
    {
        y_ = top;
    }

    void SetTop(const Dimension& top, const AnimationOption& option = AnimationOption())
    {
        y_ = AnimatableDimension(top, option);
    }

    void SetWidth(const AnimatableDimension& width)
    {
        width_ = width;
    }

    void SetWidth(const Dimension& width, const AnimationOption& option = AnimationOption())
    {
        width_ = AnimatableDimension(width, option);
    }

    void SetHeight(const AnimatableDimension& height)
    {
        height_ = height;
    }

    void SetHeight(const Dimension& height, const AnimationOption& option = AnimationOption())
    {
        height_ = AnimatableDimension(height, option);
    }

    bool IsValid() const
    {
        return width_.Value() > 0.0 && height_.Value() > 0.0;
    }
private:
    AnimatableDimension x_ = AnimatableDimension(-1.0);
    AnimatableDimension y_ = AnimatableDimension(-1.0);
    AnimatableDimension width_ = AnimatableDimension(-1.0);
    AnimatableDimension height_ = AnimatableDimension(-1.0);
};

class ACE_EXPORT ShapeContainerComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(ShapeContainerComponent, ComponentGroup);

public:
    explicit ShapeContainerComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children) {}
    ~ShapeContainerComponent() override = default;
    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;
    void InheritShapeStyle(const FillState& fillState, const StrokeState& strokeState, bool antiAlias);

    void SetViewBox(const ShapeViewBox& viewBox)
    {
        viewBox_ = viewBox;
    }

    ShapeViewBox GetViewBox() const
    {
        return viewBox_;
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

    void SetBitmapMesh(const std::vector<double>& mesh, int32_t column, int32_t row)
    {
        mesh_ = mesh;
        column_ = column;
        row_ = row;
    }

    int32_t GetMeshColumn() const
    {
        return column_;
    }

    int32_t GetMeshRow() const
    {
        return row_;
    }

    const std::vector<double> GetMesh() const
    {
        return mesh_;
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

    void SetWidthFlag(bool flag)
    {
        hasDefineWidth_ = flag;
    }

    bool GetWidthFlag()
    {
        return hasDefineWidth_;
    }

    void SetHeightFlag(bool flag)
    {
        hasDefineHeight_ = flag;
    }

    bool GetHeightFlag()
    {
        return hasDefineHeight_;
    }

private:
    ShapeViewBox viewBox_;
    FillState fillState_;
    StrokeState strokeState_;
    std::pair<bool, bool> antiAlias_ = std::make_pair(false, true);
    bool hasDefineWidth_ = false;
    bool hasDefineHeight_ = false;
    int32_t column_ = 0;
    int32_t row_ = 0;
    std::vector<double> mesh_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHARP_SHAPE_CONTAINER_COMPONENT_H
