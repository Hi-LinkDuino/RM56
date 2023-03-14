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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_PAINT_STATE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_PAINT_STATE_H

#include "base/memory/ace_type.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/common/properties/text_style.h"

namespace OHOS::Ace {

struct BezierCurveParam {
    double cp1x = 0.0; // first bezier point x
    double cp1y = 0.0; // first bezier point y
    double cp2x = 0.0; // second bezier point x
    double cp2y = 0.0; // second bezier point y
    double x = 0.0;    // end point x
    double y = 0.0;    // end point y
};

struct QuadraticCurveParam {
    double cpx = 0.0; // bezier point x
    double cpy = 0.0; // bezier point y
    double x = 0.0;   // end point x
    double y = 0.0;   // end point y
};

struct ArcParam {
    double x = 0.0;             // point x of the circle
    double y = 0.0;             // point y of the circle
    double radius = 0.0;        // radius of the circle
    double startAngle = 0.0;    // start angle of the circle
    double endAngle = 0.0;      // end angle of the circle
    bool anticlockwise = false; // is draw clock wise or not
};

struct ArcToParam {
    double x1 = 0.0;     // start point x
    double y1 = 0.0;     // start point y
    double x2 = 0.0;     // end point x
    double y2 = 0.0;     // end point y
    double radius = 0.0; // radius of the circle
};

struct EllipseParam {
    double x = 0.0;             // point x of the ellipse
    double y = 0.0;             // point y of the ellipse
    double radiusX = 0.0;       // x axis radius of the ellipse
    double radiusY = 0.0;       // y axis radius of the ellipse
    double rotation = 0.0;      // rotation angle of the ellipse
    double startAngle = 0.0;    // start angle of the ellipse
    double endAngle = 0.0;      // end angle of the ellipse
    bool anticlockwise = false; // is draw clock wise or not
};

struct TransformParam {
    double scaleX = 0.0;
    double skewX = 0.0;
    double skewY = 0.0;
    double scaleY = 0.0;
    double translateX = 0.0;
    double translateY = 0.0;
};

struct LineDashParam {
    std::vector<double> lineDash;
    double dashOffset = 0.0;
};

struct CanvasImage {
    int32_t flag = 0;
    double sx = 0.0;
    double sy = 0.0;
    double sWidth = 0.0;
    double sHeight = 0.0;
    double dx = 0.0;
    double dy = 0.0;
    double dWidth = 0.0;
    double dHeight = 0.0;
    std::string src;
};

struct ImageData {
    int32_t x = 0;
    int32_t y = 0;
    int32_t dirtyX = 0;
    int32_t dirtyY = 0;
    int32_t dirtyWidth = 0;
    int32_t dirtyHeight = 0;
    std::vector<Color> data;
};

enum class ContextType {
    RENDER_2D,
    RENDER_3D,
};

// following the definition in skPaint
enum class LineCapStyle {
    BUTT = 0,
    ROUND,
    SQUARE,
};

enum class LineJoinStyle {
    MITER = 0,
    ROUND,
    BEVEL,
};

enum class CompositeOperation {
    SOURCE_OVER = 0,
    SOURCE_ATOP,
    SOURCE_IN,
    SOURCE_OUT,
    DESTINATION_OVER,
    DESTINATION_ATOP,
    DESTINATION_IN,
    DESTINATION_OUT,
    LIGHTER,
    COPY,
    XOR,
};

class PaintState {
public:
    const Color& GetColor() const
    {
        return color_;
    }

    void SetColor(const Color& color)
    {
        color_ = color;
    }

    const Gradient& GetGradient() const
    {
        return gradient_;
    }

    void SetGradient(const Gradient& gradient)
    {
        gradient_ = gradient;
    }

    const TextStyle& GetTextStyle() const
    {
        return textStyle_;
    }

    void SetTextStyle(const TextStyle& textStyle)
    {
        textStyle_ = textStyle;
    }

    TextAlign GetTextAlign() const
    {
        return textAlign_;
    }

    void SetTextAlign(TextAlign textAlign)
    {
        textAlign_ = textAlign;
    }

    TextDirection GetOffTextDirection() const
    {
        return textDirection_;
    }

    void SetOffTextDirection(TextDirection textDirection)
    {
        textDirection_ = textDirection;
    }

    void SetTextColor(const Color& color)
    {
        textStyle_.SetTextColor(color);
    }

    void SetFontSize(const Dimension& size)
    {
        textStyle_.SetFontSize(size);
    }

    void SetFontStyle(FontStyle style)
    {
        textStyle_.SetFontStyle(style);
    }

    void SetFontWeight(FontWeight weight)
    {
        textStyle_.SetFontWeight(weight);
    }

    void SetFontFamilies(const std::vector<std::string>& fontFamilies)
    {
        textStyle_.SetFontFamilies(fontFamilies);
    }

    void SetTextBaseline(TextBaseline baseline)
    {
        textStyle_.SetTextBaseline(baseline);
    }

    const Pattern& GetPattern() const
    {
        return pattern_;
    }

    void SetPattern(const Pattern& pattern)
    {
        pattern_ = pattern;
    }

    int32_t GetId() const
    {
        return id_;
    }

    void SetId(int32_t id)
    {
        id_ = id;
    }

protected:
    Color color_ = Color::BLACK;
    Gradient gradient_;
    TextStyle textStyle_;
    TextAlign textAlign_ = TextAlign::LEFT;
    TextDirection textDirection_ = TextDirection::LTR;
    int32_t id_ = 0;

    Pattern pattern_;
};

class StrokePaintState : public PaintState {
public:
    LineCapStyle GetLineCap() const
    {
        return lineCap_;
    }

    void SetLineCap(LineCapStyle lineCap)
    {
        lineCap_ = lineCap;
    }

    LineJoinStyle GetLineJoin() const
    {
        return lineJoin_;
    }

    void SetLineJoin(LineJoinStyle lineJoin)
    {
        lineJoin_ = lineJoin;
    }

    double GetLineWidth() const
    {
        return lineWidth_;
    }

    void SetLineWidth(double lineWidth)
    {
        lineWidth_ = lineWidth;
    }

    double GetMiterLimit() const
    {
        return miterLimit_;
    }

    void SetMiterLimit(double miterLimit)
    {
        miterLimit_ = miterLimit;
    }

    const LineDashParam& GetLineDash() const
    {
        return lineDash_;
    }

    void SetLineDash(const LineDashParam& lineDash)
    {
        lineDash_ = lineDash;
    }

    void SetLineDashOffset(double offset)
    {
        lineDash_.dashOffset = offset;
    }

    void SetLineDash(const std::vector<double>& segments)
    {
        lineDash_.lineDash = segments;
    }

private:
    LineCapStyle lineCap_ = LineCapStyle::BUTT;
    LineJoinStyle lineJoin_ = LineJoinStyle::MITER;

    double lineWidth_ = 1.0; // default lineWidth

    double miterLimit_ = 10.0; // default miterLimit
    LineDashParam lineDash_;
};

class GlobalPaintState {
public:
    double GetAlpha() const
    {
        return alpha_;
    }

    void SetAlpha(double alpha)
    {
        alpha_ = alpha;
    }

    CompositeOperation GetType() const
    {
        return type_;
    }

    void SetType(CompositeOperation type)
    {
        type_ = type;
    }

    bool HasGlobalAlpha() const
    {
        return !NearEqual(alpha_, 1.0);
    }

private:
    double alpha_ = 1.0;
    CompositeOperation type_ = CompositeOperation::SOURCE_OVER;
};

struct PaintHolder {
    PaintState fillState;
    StrokePaintState strokeState;
    GlobalPaintState globalState;
    Shadow shadow;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_PAINT_STATE_H
