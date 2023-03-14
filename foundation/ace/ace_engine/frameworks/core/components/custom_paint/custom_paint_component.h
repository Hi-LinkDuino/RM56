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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_CUSTOM_PAINT_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_CUSTOM_PAINT_COMPONENT_H

#include <functional>
#include <list>

#include "base/geometry/dimension.h"
#include "base/geometry/rect.h"
#include "base/utils/macros.h"
#include "core/components/common/properties/paint_state.h"
#include "core/components/custom_paint/canvas_render_context_base.h"
#include "core/pipeline/base/render_component.h"
#include "core/pipeline/base/render_context.h"
#include "core/components/custom_paint/offscreen_canvas.h"

namespace OHOS::Ace {

class RenderCustomPaint;

using TaskFunc = std::function<void(RenderCustomPaint&, const Offset&)>;
using PushTaskFunc = std::function<void(const TaskFunc&)>;

class ACE_EXPORT CanvasTaskPool : virtual public AceType {
    DECLARE_ACE_TYPE(CanvasTaskPool, AceType);

public:
    void PushTask(const TaskFunc& task)
    {
        if (pushToRenderNodeFunc_) {
            pushToRenderNodeFunc_(task);
        } else {
            tasks_.emplace_back(task);
        }
    }

    void SetPushToRenderNodeFunc(const PushTaskFunc& pushTaskFunc)
    {
        pushToRenderNodeFunc_ = pushTaskFunc;
    }

    const std::list<TaskFunc>& GetTasks() const
    {
        return tasks_;
    }

    void ClearTasks()
    {
        tasks_.clear();
    }

    void SetRenderNode(const WeakPtr<RenderCustomPaint>& paint);
    std::string ToDataURL(const std::string& args);
    void SetWebGLInstance(CanvasRenderContextBase* context);
    void WebGLInit(CanvasRenderContextBase* context);
    void WebGLUpdate();
    void SetAntiAlias(bool isEnabled);
    void FillRect(const Rect& rect);
    void StrokeRect(const Rect& rect);
    void ClearRect(const Rect& rect);
    void FillText(const std::string& text, const Offset& textOffset);
    void StrokeText(const std::string& text, const Offset& textOffset);
    double MeasureText(const std::string& text, const PaintState& state);
    double MeasureTextHeight(const std::string& text, const PaintState& state);
    void MoveTo(double x, double y);
    void LineTo(double x, double y);
    void BezierCurveTo(const BezierCurveParam& param);
    void QuadraticCurveTo(const QuadraticCurveParam& param);
    void Arc(const ArcParam& param);
    void AddRect(const Rect& rect);
    void ArcTo(const ArcToParam& param);
    void Ellipse(const EllipseParam& param);
    void Fill();
    void Stroke();
    void Stroke(const RefPtr<CanvasPath2D>& path);
    void Clip();
    void BeginPath();
    void ClosePath();
    void Restore();
    void Save();
    void Rotate(double angle);
    void Scale(double x, double y);
    void SetTransform(const TransformParam& param);
    void Transform(const TransformParam& param);
    void Translate(double x, double y);
    void DrawImage(const CanvasImage& image, double width, double height);
    void DrawPixelMap(RefPtr<PixelMap> pixelMap, const CanvasImage& image);
    void PutImageData(const ImageData& imageData);
    std::unique_ptr<ImageData> GetImageData(double left, double top, double width, double height);
    std::string GetJsonData(const std::string& path);
    double GetWidth() const;
    double GetHeight() const;

    void UpdateFillColor(const Color& color);
    void UpdateStrokeColor(const Color& color);
    void UpdateFillGradient(const Gradient& gradient);
    void UpdateFillPattern(const Pattern& pattern);
    void UpdateStrokeGradient(const Gradient& gradient);
    void UpdateStrokePattern(const Pattern& pattern);
    void UpdateLineWidth(double width);
    void UpdateLineCap(LineCapStyle cap);
    void UpdateLineJoin(LineJoinStyle join);
    void UpdateMiterLimit(double miterLimit);
    void UpdateTextBaseline(TextBaseline baseline);
    void UpdateTextAlign(TextAlign align);
    void UpdateFontSize(const Dimension& size);
    void UpdateFontFamilies(const std::vector<std::string>& fontFamilies);
    void UpdateFontWeight(FontWeight weight);
    void UpdateFontStyle(FontStyle style);
    void UpdateGlobalAlpha(double alpha);
    void UpdateLineDashOffset(double offset);
    void UpdateLineDash(const std::vector<double>& segments);
    void UpdateShadowBlur(double blur);
    void UpdateShadowColor(const Color& color);
    void UpdateShadowOffsetX(double offsetX);
    void UpdateShadowOffsetY(double offsetY);
    void UpdateCompositeOperation(CompositeOperation type);
    void UpdateSmoothingEnabled(bool enabled);
    void UpdateSmoothingQuality(const std::string& quality);
    void TransferFromImageBitmap(const RefPtr<OffscreenCanvas>& offscreenCanvas);
    void DrawBitmapMesh(const RefPtr<OffscreenCanvas>& offscreenCanvas,
        const std::vector<double>& mesh, int32_t column, int32_t row);

private:
    PushTaskFunc pushToRenderNodeFunc_;
    std::list<TaskFunc> tasks_;
    WeakPtr<RenderCustomPaint> renderNode_;
};

class ACE_EXPORT CustomPaintComponent : public RenderComponent {
    DECLARE_ACE_TYPE(CustomPaintComponent, RenderComponent);

public:
    CustomPaintComponent() : RenderComponent()
    {
        pool_ = AceType::MakeRefPtr<CanvasTaskPool>();
    }

    ~CustomPaintComponent() override = default;

    RefPtr<Element> CreateElement() override;

    RefPtr<RenderNode> CreateRenderNode() override;

    const Dimension& GetWidth() const
    {
        return width_;
    }

    void SetWidth(const Dimension& width)
    {
        width_ = width;
    }

    const Dimension& GetHeight() const
    {
        return height_;
    }

    void SetHeight(const Dimension& height)
    {
        height_ = height;
    }

    void SetOnReadyEvent(const EventMarker& value, RefPtr<PipelineContext> context)
    {
        std::function<void()> onReady_ = AceAsyncEvent<void()>::Create(value, context);
        if (onReady_) {
            onReady_();
        }
    }

    const RefPtr<CanvasTaskPool>& GetTaskPool() const
    {
        return pool_;
    }

private:
    Dimension width_;
    Dimension height_;

    RefPtr<CanvasTaskPool> pool_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_CUSTOM_PAINT_COMPONENT_H
