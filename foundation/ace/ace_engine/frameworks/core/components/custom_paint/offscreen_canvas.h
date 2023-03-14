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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_OFFSCREEN_CANVAS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_OFFSCREEN_CANVAS_H

#include <stack>
#include "base/log/log.h"
#include "base/memory/ace_type.h"
#include "base/geometry/rect.h"
#include "core/components/common/properties/paint_state.h"

namespace OHOS::Ace {
class OffscreenCanvas : public virtual AceType {
    DECLARE_ACE_TYPE(OffscreenCanvas, AceType);

public:
    int32_t GetWidth()
    {
        return width_;
    }
    int32_t GetHeight()
    {
        return height_;
    }
    // priority set
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

    void SetFilterParam(const std::string& quality)
    {
        filterParam_ = quality;
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

    // method
    virtual std::unique_ptr<ImageData> GetImageData(double left, double top, double width, double height) = 0;
    virtual std::string ToDataURL(const std::string& type, const double quality) = 0;
    virtual void SetAntiAlias(bool isEnabled) = 0;
    virtual void FillRect(Rect rect) = 0;
    virtual void ClearRect(Rect rect) = 0;
    virtual void StrokeRect(Rect rect) = 0;
    virtual void BeginPath() = 0;
    virtual void Arc(const ArcParam& param) = 0;
    virtual void Stroke() = 0;
    virtual void Stroke(const RefPtr<CanvasPath2D>& path) = 0;
    virtual void ArcTo(const ArcToParam& param) = 0;
    virtual void MoveTo(double x, double y) = 0;
    virtual void ClosePath() = 0;
    virtual void Rotate(double angle) = 0;
    virtual void Scale(double x, double y) = 0;
    virtual void FillText(const std::string& text, double x, double y, const PaintState& state) = 0;
    virtual void StrokeText(const std::string& text, double x, double y, const PaintState& state) = 0;
    virtual double MeasureText(const std::string& text, const PaintState& state) = 0;
    virtual double MeasureTextHeight(const std::string& text, const PaintState& state) = 0;
    virtual void AddRect(const Rect& rect) = 0;
    virtual void Fill() = 0;
    virtual void Clip() = 0;
    virtual void PutImageData(const ImageData& imageData) = 0;
    virtual void DrawImage(const CanvasImage& image, double width, double height) = 0;
    virtual void DrawPixelMap(RefPtr<PixelMap> pixelMap, const CanvasImage& image) = 0;
    virtual void LineTo(double x, double y) = 0;
    virtual void BezierCurveTo(const BezierCurveParam& param) = 0;
    virtual void QuadraticCurveTo(const QuadraticCurveParam& param) = 0;
    virtual void Ellipse(const EllipseParam& param) = 0;
    virtual void SetTransform(const TransformParam& param) = 0;
    virtual void Transform(const TransformParam& param) = 0;
    virtual void Translate(double x, double y) = 0;
    virtual void Restore() = 0;
    virtual void Save() = 0;
    virtual bool IsPointInStroke(double x, double y) = 0;
    virtual bool IsPointInStroke(const RefPtr<CanvasPath2D>& path, double x, double y) = 0;
    virtual bool IsPointInPath(double x, double y) = 0;
    virtual bool IsPointInPath(const RefPtr<CanvasPath2D>& path, double x, double y) = 0;
    virtual void ResetTransform() = 0;
protected:
    int32_t width_;
    int32_t height_;
    PaintState fillState_;
    StrokePaintState strokeState_;
    GlobalPaintState globalState_;
    Shadow shadow_;
    Shadow imageShadow_;
    bool smoothingEnabled_ = true;
    std::string smoothingQuality_ = "low";
    std::string filterParam_ = "";

    // PaintHolder includes fillState, strokeState, globalState and shadow for save
    std::stack<PaintHolder> saveStates_;
};
} // namespace OHOS::Ace

#endif  // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_OFFSCREEN_CANVAS_H

