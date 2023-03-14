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

#include "core/components/custom_paint/custom_paint_component.h"

#include "core/components/custom_paint/custom_paint_element.h"
#include "core/components/custom_paint/render_custom_paint.h"

namespace OHOS::Ace {

RefPtr<RenderNode> CustomPaintComponent::CreateRenderNode()
{
    return RenderCustomPaint::Create();
}

RefPtr<Element> CustomPaintComponent::CreateElement()
{
    return AceType::MakeRefPtr<CustomPaintElement>();
}

void CanvasTaskPool::SetRenderNode(const WeakPtr<RenderCustomPaint>& paint)
{
    renderNode_ = paint;
}

std::string CanvasTaskPool::ToDataURL(const std::string& args)
{
    auto paint = renderNode_.Upgrade();
    if (!paint) {
        return args;
    }
    if (paint->HasTask()) {
        paint->FlushPipelineImmediately();
    }
    return paint->ToDataURL(args);
}
void CanvasTaskPool::TransferFromImageBitmap(const RefPtr<OffscreenCanvas>& offscreenCanvas)
{
    auto task = [offscreenCanvas](RenderCustomPaint& interface, const Offset&) {
        interface.TransferFromImageBitmap(offscreenCanvas);
    };
    PushTask(task);
}

void CanvasTaskPool::DrawBitmapMesh(const RefPtr<OffscreenCanvas>& offscreenCanvas,
    const std::vector<double>& mesh, int32_t column, int32_t row)
{
    auto task = [offscreenCanvas, mesh, column, row](RenderCustomPaint& interface, const Offset&) {
        interface.DrawBitmapMesh(offscreenCanvas, mesh, column, row);
    };
    PushTask(task);
}

void CanvasTaskPool::SetWebGLInstance(CanvasRenderContextBase* context)
{
    auto paint = renderNode_.Upgrade();
    if (!paint) {
        LOGE("CanvasTaskPool::SetWebGLInstance invalid renderNode");
        return;
    }
    paint->SetWebGLInstance(context);
}

void CanvasTaskPool::WebGLInit(CanvasRenderContextBase* context)
{
    auto paint = renderNode_.Upgrade();
    if (!paint) {
        LOGE("CanvasTaskPool::SetWebGLInstance invalid renderNode");
        return;
    }
    paint->WebGLInit(context);
}

void CanvasTaskPool::WebGLUpdate()
{
    auto task = [](RenderCustomPaint& interface, const Offset& offset) { interface.WebGLUpdate(); };
    PushTask(task);
}

void CanvasTaskPool::SetAntiAlias(bool isEnabled)
{
    auto task = [isEnabled](RenderCustomPaint& interface, const Offset&) { interface.SetAntiAlias(isEnabled); };
    PushTask(task);
}

void CanvasTaskPool::FillRect(const Rect& rect)
{
    auto task = [rect](RenderCustomPaint& interface, const Offset& offset) { interface.FillRect(offset, rect); };
    PushTask(task);
}

void CanvasTaskPool::StrokeRect(const Rect& rect)
{
    auto task = [rect](RenderCustomPaint& interface, const Offset& offset) { interface.StrokeRect(offset, rect); };
    PushTask(task);
}

void CanvasTaskPool::ClearRect(const Rect& rect)
{
    auto task = [rect](RenderCustomPaint& interface, const Offset& offset) { interface.ClearRect(offset, rect); };
    PushTask(task);
}

void CanvasTaskPool::FillText(const std::string& text, const Offset& textOffset)
{
    auto task = [text, textOffset](RenderCustomPaint& interface, const Offset& offset) {
        interface.FillText(offset, text, textOffset.GetX(), textOffset.GetY());
    };
    PushTask(task);
}

void CanvasTaskPool::StrokeText(const std::string& text, const Offset& textOffset)
{
    auto task = [text, textOffset](RenderCustomPaint& interface, const Offset& offset) {
        interface.StrokeText(offset, text, textOffset.GetX(), textOffset.GetY());
    };
    PushTask(task);
}

double CanvasTaskPool::MeasureText(const std::string& text, const PaintState& state)
{
    auto paint = renderNode_.Upgrade();
    if (!paint) {
        return 0.0;
    }
    return paint->MeasureText(text, state);
}

double CanvasTaskPool::MeasureTextHeight(const std::string& text, const PaintState& state)
{
    auto paint = renderNode_.Upgrade();
    if (!paint) {
        return 0.0;
    }
    return paint->MeasureTextHeight(text, state);
}

void CanvasTaskPool::MoveTo(double x, double y)
{
    auto task = [x, y](RenderCustomPaint& interface, const Offset& offset) { interface.MoveTo(offset, x, y); };
    PushTask(task);
}

void CanvasTaskPool::LineTo(double x, double y)
{
    auto task = [x, y](RenderCustomPaint& interface, const Offset& offset) { interface.LineTo(offset, x, y); };
    PushTask(task);
}

void CanvasTaskPool::BezierCurveTo(const BezierCurveParam& param)
{
    auto task = [param](RenderCustomPaint& interface, const Offset& offset) { interface.BezierCurveTo(offset, param); };
    PushTask(task);
}

void CanvasTaskPool::QuadraticCurveTo(const QuadraticCurveParam& param)
{
    auto task = [param](
                    RenderCustomPaint& interface, const Offset& offset) { interface.QuadraticCurveTo(offset, param); };
    PushTask(task);
}

void CanvasTaskPool::Arc(const ArcParam& param)
{
    auto task = [param](RenderCustomPaint& interface, const Offset& offset) { interface.Arc(offset, param); };
    PushTask(task);
}

void CanvasTaskPool::AddRect(const Rect& rect)
{
    auto task = [rect](RenderCustomPaint& interface, const Offset& offset) { interface.AddRect(offset, rect); };
    PushTask(task);
}

void CanvasTaskPool::ArcTo(const OHOS::Ace::ArcToParam& param)
{
    auto task = [param](RenderCustomPaint& interface, const Offset& offset) { interface.ArcTo(offset, param); };
    PushTask(task);
}

void CanvasTaskPool::Ellipse(const OHOS::Ace::EllipseParam& param)
{
    auto task = [param](RenderCustomPaint& interface, const Offset& offset) { interface.Ellipse(offset, param); };
    PushTask(task);
}

void CanvasTaskPool::Fill()
{
    auto task = [](RenderCustomPaint& interface, const Offset& offset) { interface.Fill(offset); };
    PushTask(task);
}

void CanvasTaskPool::Stroke()
{
    auto task = [](RenderCustomPaint& interface, const Offset& offset) { interface.Stroke(offset); };
    PushTask(task);
}

void CanvasTaskPool::Stroke(const RefPtr<CanvasPath2D>& path)
{
    auto task = [path](RenderCustomPaint& interface, const Offset& offset) { interface.Stroke(offset, path); };
    PushTask(task);
}

void CanvasTaskPool::Clip()
{
    auto task = [](RenderCustomPaint& interface, const Offset& offset) { interface.Clip(); };
    PushTask(task);
}

void CanvasTaskPool::BeginPath()
{
    auto task = [](RenderCustomPaint& interface, const Offset& offset) { interface.BeginPath(); };
    PushTask(task);
}

void CanvasTaskPool::ClosePath()
{
    auto task = [](RenderCustomPaint& interface, const Offset& offset) { interface.ClosePath(); };
    PushTask(task);
}

void CanvasTaskPool::Save()
{
    auto task = [](RenderCustomPaint& interface, const Offset& offset) { interface.Save(); };
    PushTask(task);
}

void CanvasTaskPool::Restore()
{
    auto task = [](RenderCustomPaint& interface, const Offset& offset) { interface.Restore(); };
    PushTask(task);
}

void CanvasTaskPool::Rotate(double angle)
{
    auto task = [angle](RenderCustomPaint& interface, const Offset& offset) { interface.Rotate(angle); };
    PushTask(task);
}

void CanvasTaskPool::Scale(double x, double y)
{
    auto task = [x, y](RenderCustomPaint& interface, const Offset& offset) { interface.Scale(x, y); };
    PushTask(task);
}

void CanvasTaskPool::SetTransform(const TransformParam& param)
{
    auto task = [param](RenderCustomPaint& interface, const Offset& offset) { interface.SetTransform(param); };
    PushTask(task);
}

void CanvasTaskPool::Transform(const TransformParam& param)
{
    auto task = [param](RenderCustomPaint& interface, const Offset& offset) { interface.Transform(param); };
    PushTask(task);
}

void CanvasTaskPool::Translate(double x, double y)
{
    auto task = [x, y](RenderCustomPaint& interface, const Offset& offset) { interface.Translate(x, y); };
    PushTask(task);
}

void CanvasTaskPool::DrawImage(const CanvasImage& image, double width, double height)
{
    auto task = [image, width, height](RenderCustomPaint& interface, const Offset& offset) {
        interface.DrawImage(offset, image, width, height);
    };
    PushTask(task);
}

void CanvasTaskPool::DrawPixelMap(RefPtr<PixelMap> pixelMap, const CanvasImage& image)
{
    auto task = [pixelMap, image](RenderCustomPaint& interface, const Offset& offset) {
        interface.DrawPixelMap(pixelMap, image);
    };
    PushTask(task);
}

void CanvasTaskPool::PutImageData(const ImageData& imageData)
{
    auto task = [imageData](
                    RenderCustomPaint& interface, const Offset& offset) { interface.PutImageData(offset, imageData); };
    PushTask(task);
}

std::unique_ptr<ImageData> CanvasTaskPool::GetImageData(double left, double top, double width, double height)
{
    auto paint = renderNode_.Upgrade();
    if (!paint) {
        std::unique_ptr<ImageData> data = std::make_unique<ImageData>();
        data->dirtyWidth = width;
        data->dirtyHeight = height;
        return data;
    }
    if (paint->HasTask()) {
        paint->FlushPipelineImmediately();
    }
    return paint->GetImageData(left, top, width, height);
}

std::string CanvasTaskPool::GetJsonData(const std::string& path)
{
    auto paint = renderNode_.Upgrade();
    if (!paint) {
        return "";
    }

    return paint->GetJsonData(path);
}

double CanvasTaskPool::GetWidth() const
{
    auto paint = renderNode_.Upgrade();
    if (!paint) {
        return 0.0;
    }

    return paint->GetLayoutSize().Width();
}

double CanvasTaskPool::GetHeight() const
{
    auto paint = renderNode_.Upgrade();
    if (!paint) {
        return 0.0;
    }

    return paint->GetLayoutSize().Height();
}

void CanvasTaskPool::UpdateFillColor(const Color& color)
{
    auto task = [color](RenderCustomPaint& interface, const Offset& offset) {
        interface.SetFillColor(color);
        interface.SetFillPattern(Pattern());
        interface.SetFillGradient(Gradient());
    };
    PushTask(task);
}

void CanvasTaskPool::UpdateStrokeColor(const Color& color)
{
    auto task = [color](RenderCustomPaint& interface, const Offset& offset) {
        interface.SetStrokeColor(color);
        interface.SetStrokePattern(Pattern());
        interface.SetStrokeGradient(Gradient());
    };
    PushTask(task);
}

void CanvasTaskPool::UpdateFillGradient(const Gradient& gradient)
{
    auto task = [gradient](RenderCustomPaint& interface, const Offset& offset) {
        interface.SetFillGradient(gradient);
        interface.SetFillColor(Color());
        interface.SetFillPattern(Pattern());
    };
    PushTask(task);
}

void CanvasTaskPool::UpdateStrokeGradient(const Gradient& grad)
{
    auto task = [grad](RenderCustomPaint& interface, const Offset& offset) {
        interface.SetStrokeGradient(grad);
        interface.SetStrokeColor(Color());
        interface.SetStrokePattern(Pattern());
    };
    PushTask(task);
}

void CanvasTaskPool::UpdateFillPattern(const Pattern& pattern)
{
    auto task = [pattern](RenderCustomPaint& interface, const Offset& offset) {
        interface.SetFillPattern(pattern);
        interface.SetFillGradient(Gradient());
        interface.SetFillColor(Color());
    };
    PushTask(task);
}

void CanvasTaskPool::UpdateStrokePattern(const Pattern& pattern)
{
    auto task = [pattern](RenderCustomPaint& interface, const Offset& offset) {
        interface.SetStrokePattern(pattern);
        interface.SetStrokeGradient(Gradient());
        interface.SetStrokeColor(Color());
    };
    PushTask(task);
}

void CanvasTaskPool::UpdateLineWidth(double width)
{
    auto task = [width](RenderCustomPaint& interface, const Offset& offset) { interface.SetLineWidth(width); };
    PushTask(task);
}

void CanvasTaskPool::UpdateLineCap(LineCapStyle cap)
{
    auto task = [cap](RenderCustomPaint& interface, const Offset& offset) { interface.SetLineCap(cap); };
    PushTask(task);
}

void CanvasTaskPool::UpdateLineJoin(LineJoinStyle join)
{
    auto task = [join](RenderCustomPaint& interface, const Offset& offset) { interface.SetLineJoin(join); };
    PushTask(task);
}

void CanvasTaskPool::UpdateMiterLimit(double limit)
{
    auto task = [limit](RenderCustomPaint& interface, const Offset& offset) { interface.SetMiterLimit(limit); };
    PushTask(task);
}

void CanvasTaskPool::UpdateTextBaseline(TextBaseline baseline)
{
    auto task = [baseline](RenderCustomPaint& interface, const Offset& offset) { interface.SetTextBaseline(baseline); };
    PushTask(task);
}

void CanvasTaskPool::UpdateTextAlign(TextAlign align)
{
    auto task = [align](RenderCustomPaint& interface, const Offset& offset) { interface.SetTextAlign(align); };
    PushTask(task);
}

void CanvasTaskPool::UpdateFontSize(const Dimension& size)
{
    auto task = [size](RenderCustomPaint& interface, const Offset& offset) { interface.SetFontSize(size); };
    PushTask(task);
}

void CanvasTaskPool::UpdateFontFamilies(const std::vector<std::string>& families)
{
    auto task = [families](RenderCustomPaint& interface, const Offset& offset) { interface.SetFontFamilies(families); };
    PushTask(task);
}

void CanvasTaskPool::UpdateFontWeight(FontWeight weight)
{
    auto task = [weight](RenderCustomPaint& interface, const Offset& offset) { interface.SetFontWeight(weight); };
    PushTask(task);
}

void CanvasTaskPool::UpdateFontStyle(FontStyle style)
{
    auto task = [style](RenderCustomPaint& interface, const Offset& offset) { interface.SetFontStyle(style); };
    PushTask(task);
}

void CanvasTaskPool::UpdateGlobalAlpha(double alpha)
{
    auto task = [alpha](RenderCustomPaint& interface, const Offset& offset) { interface.SetAlpha(alpha); };
    PushTask(task);
}

void CanvasTaskPool::UpdateLineDashOffset(double dash)
{
    auto task = [dash](RenderCustomPaint& interface, const Offset& offset) { interface.SetLineDashOffset(dash); };
    PushTask(task);
}

void CanvasTaskPool::UpdateLineDash(const std::vector<double>& segments)
{
    auto task = [segments](RenderCustomPaint& interface, const Offset& offset) { interface.SetLineDash(segments); };
    PushTask(task);
}

void CanvasTaskPool::UpdateShadowBlur(double blur)
{
    auto task = [blur](RenderCustomPaint& interface, const Offset& offset) { interface.SetShadowBlur(blur); };
    PushTask(task);
}

void CanvasTaskPool::UpdateShadowColor(const Color& color)
{
    auto task = [color](RenderCustomPaint& interface, const Offset& offset) { interface.SetShadowColor(color); };
    PushTask(task);
}

void CanvasTaskPool::UpdateShadowOffsetX(double offsetX)
{
    auto task = [offsetX](RenderCustomPaint& interface, const Offset& offset) { interface.SetShadowOffsetX(offsetX); };
    PushTask(task);
}

void CanvasTaskPool::UpdateShadowOffsetY(double offsetY)
{
    auto task = [offsetY](RenderCustomPaint& interface, const Offset& offset) { interface.SetShadowOffsetY(offsetY); };
    PushTask(task);
}

void CanvasTaskPool::UpdateCompositeOperation(CompositeOperation type)
{
    auto task = [type](RenderCustomPaint& interface, const Offset& offset) { interface.SetCompositeType(type); };
    PushTask(task);
}

void CanvasTaskPool::UpdateSmoothingEnabled(bool enabled)
{
    auto task =
        [enabled](RenderCustomPaint& interface, const Offset& offset) { interface.SetSmoothingEnabled(enabled); };
    PushTask(task);
}

void CanvasTaskPool::UpdateSmoothingQuality(const std::string& quality)
{
    auto task =
        [quality](RenderCustomPaint& interface, const Offset& offset) { interface.SetSmoothingQuality(quality); };
    PushTask(task);
}

} // namespace OHOS::Ace
