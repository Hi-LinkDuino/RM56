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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_FLUTTER_RENDER_CUSTOM_PAINT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_FLUTTER_RENDER_CUSTOM_PAINT_H

#include "flutter/fml/memory/ref_ptr.h"
#include "flutter/lib/ui/painting/path.h"
#include "flutter/third_party/txt/src/txt/paragraph.h"
#include "third_party/skia/include/core/SkPath.h"

#include "core/components/custom_paint/offscreen_canvas.h"
#include "core/components/custom_paint/render_custom_paint.h"
#include "core/image/image_provider.h"
#include "core/pipeline/base/scoped_canvas_state.h"
#include "core/pipeline/layers/clip_layer.h"

namespace OHOS::Ace {

class FlutterRenderCustomPaint : public RenderCustomPaint {
    DECLARE_ACE_TYPE(FlutterRenderCustomPaint, RenderCustomPaint);

public:
    FlutterRenderCustomPaint();
    ~FlutterRenderCustomPaint() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;

    void TransferFromImageBitmap(const RefPtr<OffscreenCanvas>& offscreenCanvas) override;
    void DrawBitmapMesh(const RefPtr<OffscreenCanvas>& offscreenCanvas, const std::vector<double>& mesh,
        int32_t column, int32_t row) override;
    void Mesh(SkBitmap& bitmap, int column, int row,
        const float* vertices, const int* colors, const SkPaint* paint);
    std::string ToDataURL(const std::string& args) override;
    void SetAntiAlias(bool isEnabled) override;
    void FillRect(const Offset& offset, const Rect& rect) override;
    void StrokeRect(const Offset& offset, const Rect& rect) override;
    void ClearRect(const Offset& offset, const Rect& rect) override;
    void FillText(const Offset& offset, const std::string& text, double x, double y) override;
    void StrokeText(const Offset& offset, const std::string& text, double x, double y) override;
    double MeasureText(const std::string& text, const PaintState& state) override;
    double MeasureTextHeight(const std::string& text, const PaintState& state) override;
    void MoveTo(const Offset& offset, double x, double y) override;
    void LineTo(const Offset& offset, double x, double y) override;
    void BezierCurveTo(const Offset& offset, const BezierCurveParam& param) override;
    void QuadraticCurveTo(const Offset& offset, const QuadraticCurveParam& param) override;
    void Arc(const Offset& offset, const ArcParam& param) override;
    void ArcTo(const Offset& offset, const ArcToParam& param) override;
    void Ellipse(const Offset& offset, const EllipseParam& param) override;
    void AddRect(const Offset& offset, const Rect& rect) override;
    void Fill(const Offset& offset) override;
    void Stroke(const Offset& offset) override;
    void Stroke(const Offset& offset, const RefPtr<CanvasPath2D>& path) override;
    void Clip() override;
    void BeginPath() override;
    void ClosePath() override;
    void Restore() override;
    void Save() override;
    void Rotate(double angle) override;
    void Scale(double x, double y) override;
    void SetTransform(const TransformParam& param) override;
    void Transform(const TransformParam& param) override;
    void Translate(double x, double y) override;
    void DrawImage(const Offset& offset, const CanvasImage& canvasImage, double width, double height) override;
    void DrawPixelMap(RefPtr<PixelMap> pixelMap, const CanvasImage& canvasImage) override;
    void PutImageData(const Offset& offset, const ImageData& imageData) override;
    std::unique_ptr<ImageData> GetImageData(double left, double top, double width, double height) override;
    std::string GetJsonData(const std::string& path) override;

    void WebGLInit(CanvasRenderContextBase* context) override;
    void WebGLUpdate() override;
    RenderLayer GetRenderLayer() override;

private:
    void InitImagePaint();
    void InitCachePaint();
    bool UpdateParagraph(const Offset& offset, const std::string& text, bool isStroke, bool hasShadow = true);
    void PaintText(const Offset& offset, double x, double y, bool isStroke, bool hasShadow = true);
    double GetAlignOffset(TextAlign align);
    double GetBaselineOffset(TextBaseline baseline);
    SkPaint GetStrokePaint();
    bool HasShadow() const;
    void UpdatePaintShader(const Offset& offset, SkPaint& paint, const Gradient& gradient);
    void UpdatePaintShader(const Pattern& pattern, SkPaint& paint);
    void UpdateLineDash(SkPaint& paint);
    void UpdateTextStyleForeground(const Offset& offset, bool isStroke, txt::TextStyle& style, bool hasShadow);

    void Path2DAddPath(const Offset& offset, const PathArgs& args);
    void Path2DSetTransform(const Offset& offset, const PathArgs& args);
    void Path2DMoveTo(const Offset& offset, const PathArgs& args);
    void Path2DLineTo(const Offset& offset, const PathArgs& args);
    void Path2DArc(const Offset& offset, const PathArgs& args);
    void Path2DArcTo(const Offset& offset, const PathArgs& args);
    void Path2DQuadraticCurveTo(const Offset& offset, const PathArgs& args);
    void Path2DBezierCurveTo(const Offset& offset, const PathArgs& args);
    void Path2DEllipse(const Offset& offset, const PathArgs& args);
    void Path2DRect(const Offset& offset, const PathArgs& args);
    void Path2DClosePath(const Offset& offset, const PathArgs& args);
    void Path2DStroke(const Offset& offset);

    bool antiAlias_ = false;
    std::unique_ptr<txt::Paragraph> paragraph_;
    RefPtr<Flutter::ClipLayer> layer_;
    SkPath skPath_;
    SkPath strokePath_;
    SkPaint imagePaint_;
    SkPaint cachePaint_;
    SkBitmap cacheBitmap_;
    SkBitmap canvasCache_;
    SkBitmap webglBitmap_;
    std::unique_ptr<SkCanvas> skCanvas_;
    std::unique_ptr<SkCanvas> cacheCanvas_;
    RefPtr<FlutterRenderTaskHolder> renderTaskHolder_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_FLUTTER_RENDER_CUSTOM_PAINT_H
