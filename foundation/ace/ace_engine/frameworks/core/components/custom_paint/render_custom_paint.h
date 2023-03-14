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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_RENDER_CUSTOM_PAINT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_RENDER_CUSTOM_PAINT_H

#include "core/components/common/properties/paint_state.h"
#include "core/components/custom_paint/canvas_render_context_base.h"
#include "core/components/custom_paint/custom_paint_component.h"
#include "core/components/custom_paint/offscreen_canvas.h"

#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderOffscreenCanvas : public OffscreenCanvas {
    DECLARE_ACE_TYPE(RenderOffscreenCanvas, OffscreenCanvas);
public:
    static RefPtr<OffscreenCanvas> Create(const WeakPtr<PipelineContext>& context, int32_t width, int32_t height);
};

class RenderCustomPaint : public RenderNode {
    DECLARE_ACE_TYPE(RenderCustomPaint, RenderNode);

public:
    ~RenderCustomPaint() override = default;

    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    void PushTask(const TaskFunc& func);

    bool HasTask() const
    {
        return !tasks_.empty();
    }

    const std::list<TaskFunc>& GetTasks() const
    {
        return tasks_;
    }

    void SetTasks(const std::list<TaskFunc>& tasks)
    {
        tasks_ = tasks;
    }

    void FlushPipelineImmediately();

    virtual void TransferFromImageBitmap(const RefPtr<OffscreenCanvas>& offscreenCanvas) = 0;
    virtual void DrawBitmapMesh(const RefPtr<OffscreenCanvas>& offscreenCanvas,
        const std::vector<double>& mesh, int32_t column, int32_t row) = 0;
    virtual std::string ToDataURL(const std::string& args) = 0;
    virtual void SetAntiAlias(bool isEnabled) = 0;
    virtual void FillRect(const Offset& offset, const Rect& rect) = 0;
    virtual void StrokeRect(const Offset& offset, const Rect& rect) = 0;
    virtual void ClearRect(const Offset& offset, const Rect& rect) = 0;

    virtual void FillText(const Offset& offset, const std::string& text, double x, double y) = 0;
    virtual void StrokeText(const Offset& offset, const std::string& text, double x, double y) = 0;
    virtual double MeasureText(const std::string& text, const PaintState& state) = 0;
    virtual double MeasureTextHeight(const std::string& text, const PaintState& state) = 0;

    virtual void MoveTo(const Offset& offset, double x, double y) = 0;
    virtual void LineTo(const Offset& offset, double x, double y) = 0;
    virtual void BezierCurveTo(const Offset& offset, const BezierCurveParam& param) = 0;
    virtual void QuadraticCurveTo(const Offset& offset, const QuadraticCurveParam& param) = 0;
    virtual void Arc(const Offset& offset, const ArcParam& param) = 0;
    virtual void ArcTo(const Offset& offset, const ArcToParam& param) = 0;
    virtual void Ellipse(const Offset& offset, const EllipseParam& param) = 0;
    virtual void AddRect(const Offset& offset, const Rect& rect) = 0;

    virtual void Fill(const Offset& offset) = 0;
    virtual void Stroke(const Offset& offset) = 0;
    virtual void Stroke(const Offset& offset, const RefPtr<CanvasPath2D>& path) = 0;
    virtual void Clip() = 0;
    virtual void BeginPath() = 0;
    virtual void ClosePath() = 0;

    // in render node, restore and save are used for clip and matrix operation
    virtual void Restore() = 0;
    virtual void Save() = 0;

    virtual void Rotate(double angle) = 0;
    virtual void Scale(double x, double y) = 0;
    virtual void SetTransform(const TransformParam& param) = 0;
    virtual void Transform(const TransformParam& param) = 0;
    virtual void Translate(double x, double y) = 0;
    virtual void DrawImage(const Offset& offset, const CanvasImage& image, double width, double height) = 0;
    virtual void DrawPixelMap(RefPtr<PixelMap> pixelMap, const CanvasImage& canvasImage) = 0;
    virtual void PutImageData(const Offset& offset, const ImageData& imageData) = 0;
    virtual std::unique_ptr<ImageData> GetImageData(double left, double top, double width, double height) = 0;
    virtual std::string GetJsonData(const std::string& path) = 0;

    virtual void WebGLInit(CanvasRenderContextBase* context) = 0;
    virtual void WebGLUpdate() = 0;

    bool IsRepaintBoundary() const override
    {
        return true;
    }

    void SetFillColor(const Color& color)
    {
        fillState_.SetColor(color);
        fillState_.SetTextColor(color);
    }

    void SetStrokeColor(const Color& color)
    {
        strokeState_.SetColor(color);
    }

    void SetFillGradient(const Gradient& gradient)
    {
        fillState_.SetGradient(gradient);
    }

    void SetStrokeGradient(const Gradient& gradient)
    {
        strokeState_.SetGradient(gradient);
    }

    void SetFillPattern(const Pattern& pattern)
    {
        fillState_.SetPattern(pattern);
    }

    void SetStrokePattern(const Pattern& pattern)
    {
        strokeState_.SetPattern(pattern);
    }

    void SetShadowBlur(double blur)
    {
        shadow_.SetBlurRadius(blur);
    }

    void SetShadowOffsetX(double x)
    {
        shadow_.SetOffsetX(x);
    }

    void SetShadowOffsetY(double y)
    {
        shadow_.SetOffsetY(y);
    }

    void SetSmoothingEnabled(bool enabled)
    {
        smoothingEnabled_ = enabled;
    }

    void SetSmoothingQuality(const std::string& quality)
    {
        smoothingQuality_ = quality;
    }

    void SetShadowColor(const Color& color)
    {
        shadow_.SetColor(color);
    }

    void SetAlpha(double alpha)
    {
        globalState_.SetAlpha(alpha);
    }

    void SetCompositeType(CompositeOperation operation)
    {
        globalState_.SetType(operation);
    }

    void SetLineDashOffset(double offset)
    {
        strokeState_.SetLineDashOffset(offset);
    }

    void SetLineDash(const std::vector<double>& segments)
    {
        strokeState_.SetLineDash(segments);
    }

    void SetTextAlign(TextAlign align)
    {
        fillState_.SetTextAlign(align);
        strokeState_.SetTextAlign(align);
    }

    void SetTextBaseline(TextBaseline baseline)
    {
        fillState_.SetTextBaseline(baseline);
        strokeState_.SetTextBaseline(baseline);
    }

    void SetFontWeight(FontWeight weight)
    {
        fillState_.SetFontWeight(weight);
        strokeState_.SetFontWeight(weight);
    }

    void SetFontFamilies(const std::vector<std::string>& fontFamilies)
    {
        fillState_.SetFontFamilies(fontFamilies);
        strokeState_.SetFontFamilies(fontFamilies);
    }

    void SetFontStyle(FontStyle style)
    {
        fillState_.SetFontStyle(style);
        strokeState_.SetFontStyle(style);
    }

    void SetFontSize(const Dimension& size)
    {
        fillState_.SetFontSize(size);
        strokeState_.SetFontSize(size);
    }

    void SetTextStyle(const TextStyle& style)
    {
        fillState_.SetTextStyle(style);
        strokeState_.SetTextStyle(style);
    }

    void SetLineWidth(double width)
    {
        strokeState_.SetLineWidth(width);
    }

    void SetLineCap(LineCapStyle style)
    {
        strokeState_.SetLineCap(style);
    }

    void SetLineJoin(LineJoinStyle style)
    {
        strokeState_.SetLineJoin(style);
    }

    void SetMiterLimit(double limit)
    {
        strokeState_.SetMiterLimit(limit);
    }

    void SaveStates()
    {
        PaintHolder holder;
        holder.shadow = shadow_;
        holder.fillState = fillState_;
        holder.globalState = globalState_;
        holder.strokeState = strokeState_;
        saveStates_.push(holder);
    }

    void RestoreStates()
    {
        if (saveStates_.empty()) {
            return;
        }
        auto saveState = saveStates_.top();
        shadow_ = saveState.shadow;
        fillState_ = saveState.fillState;
        strokeState_ = saveState.strokeState;
        globalState_ = saveState.globalState;
        saveStates_.pop();
    }

    void SetWebGLInstance(CanvasRenderContextBase* context)
    {
        webGLContext_ = context;
    }

    CanvasRenderContextBase* GetWebGLInstance() const
    {
        return webGLContext_;
    }

protected:
    RenderCustomPaint();

    PaintState fillState_;
    StrokePaintState strokeState_;

    // save alpha and compositeType in GlobalPaintState
    GlobalPaintState globalState_;
    Shadow shadow_;

    bool smoothingEnabled_ = true;
    std::string smoothingQuality_ = "low";

    // PaintHolder includes fillState, strokeState, globalState and shadow for save
    std::stack<PaintHolder> saveStates_;

    RefPtr<CanvasTaskPool> pool_;
    std::list<TaskFunc> tasks_;

    ContextType type_ = ContextType::RENDER_2D;
    CanvasRenderContextBase* webGLContext_ = nullptr;

private:
    Dimension width_;
    Dimension height_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_RENDER_CUSTOM_PAINT_H
