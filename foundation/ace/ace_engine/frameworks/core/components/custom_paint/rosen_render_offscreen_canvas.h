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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_ROSEN_RENDER_OFFSCREEN_CANVAS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_ROSEN_RENDER_OFFSCREEN_CANVAS_H

#include "experimental/svg/model/SkSVGDOM.h"
#include "flutter/lib/ui/painting/path.h"
#include "flutter/third_party/txt/src/txt/paragraph.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPath.h"

#include "core/components/custom_paint/offscreen_canvas.h"
#include "core/components/custom_paint/render_custom_paint.h"
#include "core/image/image_source_info.h"
#include "core/image/image_object.h"
#include "core/image/image_provider.h"
#include "core/pipeline/base/scoped_canvas_state.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/offscreen_canvas_bridge.h"

namespace OHOS::Ace {
using setColorFunc = std::function<void (const std::string&)>;
class RosenRenderOffscreenCanvas : public RenderOffscreenCanvas {
    DECLARE_ACE_TYPE(RosenRenderOffscreenCanvas, RenderOffscreenCanvas);

public:
    RosenRenderOffscreenCanvas(const WeakPtr<PipelineContext>& context, int32_t width, int32_t height);
    ~RosenRenderOffscreenCanvas() override = default;
    void SetAntiAlias(bool isEnabled) override;
    void FillRect(Rect rect) override;
    void ClearRect(Rect rect) override;
    void StrokeRect(Rect rect) override;
    void FillText(const std::string& text, double x, double y, const PaintState& state) override;
    void StrokeText(const std::string& text, double x, double y, const PaintState& state) override;
    double MeasureText(const std::string& text, const PaintState& state) override;
    double MeasureTextHeight(const std::string& text, const PaintState& state) override;
    std::string ToDataURL(const std::string& type, const double quality) override;
    std::unique_ptr<ImageData> GetImageData(double left, double top, double width, double height) override;
    void BeginPath() override;
    void Arc(const ArcParam& param) override;
    void Stroke() override;
    void Stroke(const RefPtr<CanvasPath2D>& path) override;
    void ArcTo(const ArcToParam& param) override;
    void MoveTo(double x, double y) override;
    void ClosePath() override;
    void Rotate(double angle) override;
    void Scale(double x, double y) override;
    void AddRect(const Rect& rect) override;
    void Fill() override;
    void Clip() override;
    void PutImageData(const ImageData& imageData) override;
    void DrawImage(const CanvasImage& canvasImage, double width, double height) override;
    void DrawPixelMap(RefPtr<PixelMap> pixelMap, const CanvasImage& image) override;
    void LineTo(double x, double y) override;
    void BezierCurveTo(const BezierCurveParam& param) override;
    void QuadraticCurveTo(const QuadraticCurveParam& param) override;
    void Ellipse(const EllipseParam& param) override;
    void SetTransform(const TransformParam& param) override;
    void Transform(const TransformParam& param) override;
    void Translate(double x, double y) override;
    void Restore() override;
    void Save() override;
    bool IsPointInStroke(double x, double y) override;
    bool IsPointInStroke(const RefPtr<CanvasPath2D>& path, double x, double y) override;
    bool IsPointInPath(double x, double y) override;
    bool IsPointInPath(const RefPtr<CanvasPath2D>& path, double x, double y) override;
    void ResetTransform() override;
private:
    void InitImagePaint();
    void InitCachePaint();
    bool antiAlias_ = true;
    SkPaint GetStrokePaint();
    WeakPtr<PipelineContext> pipelineContext_;
    SkBitmap skBitmap_;
    SkPath skPath_;
    SkPath strokePath_;
    SkPaint imagePaint_;
    SkPaint cachePaint_;
    SkBitmap cacheBitmap_;
    std::unique_ptr<SkCanvas> cacheCanvas_;
    std::unique_ptr<SkCanvas> skCanvas_;
    std::map<std::string, setColorFunc> filterFunc_;
    RefPtr<FlutterRenderTaskHolder> renderTaskHolder_;
    ImageSourceInfo loadingSource_;
    ImageSourceInfo currentSource_;
    ImageObjSuccessCallback imageObjSuccessCallback_;
    UploadSuccessCallback uploadSuccessCallback_;
    FailedCallback failedCallback_;
    OnPostBackgroundTask onPostBackgroundTask_;
    sk_sp<SkSVGDOM> skiaDom_ = nullptr;
    CanvasImage canvasImage_;

    void UpdatePaintShader(SkPaint& paint, const Gradient& gradient);
    void UpdatePaintShader(const Pattern& pattern, SkPaint& paint);
    void PaintText(const std::string& text, double x, double y, bool isStroke, bool hasShadow = false);
    double GetBaselineOffset(TextBaseline baseline);
    std::unique_ptr<txt::Paragraph> paragraph_;
    bool HasShadow() const;
    bool HasImageShadow() const;
    void UpdateTextStyleForeground(bool isStroke, txt::TextStyle& style, bool hasShadow);
    double GetAlignOffset(const std::string& text, TextAlign align);
    TextDirection GetTextDirection(const std::string& text);
    bool UpdateOffParagraph(const std::string& text, bool isStroke, const PaintState& state, bool hasShadow = false);
    void UpdateLineDash(SkPaint& paint);
    void Path2DAddPath(const PathArgs& args);
    void Path2DSetTransform(const PathArgs& args);
    void Path2DMoveTo(const PathArgs& args);
    void Path2DLineTo(const PathArgs& args);
    void Path2DArc(const PathArgs& args);
    void Path2DArcTo(const PathArgs& args);
    void Path2DQuadraticCurveTo(const PathArgs& args);
    void Path2DBezierCurveTo(const PathArgs& args);
    void Path2DEllipse(const PathArgs& args);
    void Path2DRect(const PathArgs& args);
    void Path2DClosePath(const PathArgs& args);
    void Path2DStroke();
    void TranspareCmdToPath(const RefPtr<CanvasPath2D>& path);
    bool IsPointInPathByColor(double x, double y, SkPath& path, SkColor colorMatch);
    void SetPaintImage();
    void InitFliterFunc();
    bool GetFilterType(std::string& filterType, std::string& filterParam);
    void SetGrayFilter(const std::string& percent);
    void SetSepiaFilter(const std::string& percent);
    void SetInvertFilter(const std::string& percent);
    void SetOpacityFilter(const std::string& percent);
    void SetBrightnessFilter(const std::string& percent);
    void SetContrastFilter(const std::string& percent);
    void SetBlurFilter(const std::string& percent);
    void SetDropShadowFilter(const std::string& percent);
    void SetSaturateFilter(const std::string& percent);
    void SetHueRotateFilter(const std::string& percent);
    double PxStrToDouble(const std::string& str);
    double BlurStrToDouble(const std::string& str);
    bool IsPercentStr(std::string& percentStr);
    void SetColorFilter(float matrix[20]);
    void InitImageCallbacks();
    void ImageObjReady(const RefPtr<ImageObject>& imageObj);
    void ImageObjFailed();
    void DrawSvgImage(const CanvasImage& canvasImage);
};
} // namespace OHOS::Ace

#endif // !FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_ROSEN_RENDER_OFFSCREEN_CANVAS_H
