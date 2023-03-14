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

#include "clip_utils.h"
#include "draw_utils.h"
#include "gfx_utils/diagram/depiction/depict_curve.h"
#include "gfx_utils/diagram/rasterizer/rasterizer_scanline_antialias.h"
#include "gfx_utils/diagram/scanline/geometry_scanline.h"
#include "gfx_utils/diagram/spancolorfill/fill_base.h"
#include "gfx_utils/diagram/spancolorfill/fill_pattern_rgba.h"
#include "gfx_utils/graphic_log.h"
#include "render/render_base.h"
#include "render/render_pixfmt_rgba_blend.h"

namespace OHOS {
using UICanvasPath = DepictCurve;
using PathTransform = DepictTransform<UICanvasPath>;

ClipPath& ClipPath::MoveTo(const PointF& point)
{
    vertices_->MoveTo(point.x, point.y);
    return *this;
}

ClipPath& ClipPath::LineTo(const PointF& point)
{
    if (vertices_->GetTotalVertices() != 0) {
        vertices_->LineTo(point.x, point.y);
    } else {
        vertices_->MoveTo(point.x, point.y);
    }
    return *this;
}

ClipPath& ClipPath::CurveTo(const PointF& control1, const PointF& control2, const PointF& end)
{
    vertices_->CubicBezierCurve(control1.x, control1.y, control2.x, control2.y, end.x, end.y);
    return *this;
}

ClipPath& ClipPath::Arc(const PointF& center, float radius, int16_t startAngle, int16_t endAngle)
{
    if (startAngle == endAngle) {
        return *this;
    }
    float sinma = radius * Sin(startAngle);
    float cosma = radius * Sin(QUARTER_IN_DEGREE - startAngle);
    if (vertices_->GetTotalVertices() != 0) {
        vertices_->LineTo(float(center.x + sinma), float(center.y - cosma));
    } else {
        vertices_->MoveTo(float(center.x + sinma), float(center.y - cosma));
    }
    if (MATH_ABS(startAngle - endAngle) < CIRCLE_IN_DEGREE) {
        sinma = radius * Sin(endAngle);
        cosma = radius * Sin(QUARTER_IN_DEGREE - endAngle);
    } else {
        Circle(center, radius);
        return *this;
    }

    int16_t angle = endAngle - startAngle;
    bool largeArcFlag = false;
    if (angle > SEMICIRCLE_IN_DEGREE || angle <= 0) {
        largeArcFlag = true;
    }
    vertices_->ArcTo(radius, radius, angle, largeArcFlag, 1, center.x + sinma, center.y - cosma);
    return *this;
}

ClipPath& ClipPath::Circle(const PointF& center, float radius)
{
    if (radius <= 0) {
        return *this;
    }
    vertices_->RemoveAll();
#if GRAPHIC_ENABLE_BEZIER_ARC_FLAG
    BezierArc arc(center.x, center.y, radius, radius, 0, TWO_TIMES * PI);
    vertices_->ConcatPath(arc, 0);
#endif
    return *this;
}

UICanvasVertices& ClipUtils::CloseVertices(const ClipPath& path)
{
    UICanvasVertices& vertices = path.GetVertices();
    vertices.ClosePolygon();
    return vertices;
}

void ClipUtils::PerformScan(const ClipPath& path, const ImageInfo* imageInfo)
{
    RasterizerScanlineAntialias rasterizer;
    GeometryScanline scanline;
    TransAffine transform;
    UICanvasVertices& vertices = CloseVertices(path);
    UICanvasPath canvasPath(vertices);
    PathTransform pathTransform(canvasPath, transform);
    rasterizer.Reset();
    rasterizer.AddPath(pathTransform);
    if (!rasterizer.RewindScanlines()) {
        for (int32_t i = 0; i < imageInfo->header.height; i++) {
            DrawHorLine(0, i, imageInfo->header.width, OPA_TRANSPARENT, imageInfo);
        }
        return;
    }
    scanline.Reset(rasterizer.GetMinX(), rasterizer.GetMaxX());

    bool first = true;
    int16_t y = 0;
    while (rasterizer.SweepScanline(scanline)) {
        y = scanline.GetYLevel();
        if (first) {
            for (int32_t i = 0; i < y; i++) {
                DrawHorLine(0, i, imageInfo->header.width, OPA_TRANSPARENT, imageInfo);
            }
            first = false;
        }
        uint32_t numSpans = scanline.NumSpans();
        GeometryScanline::ConstIterator span = scanline.Begin();
        int16_t index = 0;
        while (true) {
            int32_t x = span->x;
            int32_t len = span->spanLength;
            const uint8_t* covers = span->covers;

            if (len < 0) {
                len = -len;
            }
            DrawHorLine(index, y, x - index - 1, OPA_TRANSPARENT, imageInfo);
            for (int16_t i = x; i < x + len; i++, covers++) {
                DrawPixel(i, y, *covers, imageInfo);
            }
            index = x + len;
            if (--numSpans == 0) {
                break;
            }
            ++span;
        }
        DrawHorLine(index, y, imageInfo->header.width - index, OPA_TRANSPARENT, imageInfo);
    }

    for (int32_t i = y + 1; i < imageInfo->header.height; i++) {
        DrawHorLine(0, i, imageInfo->header.width, OPA_TRANSPARENT, imageInfo);
    }
}

void ClipUtils::DrawPixel(int16_t x, int16_t y, uint8_t opa, const ImageInfo* imageInfo)
{
    if (x < 0 || x > imageInfo->header.width - 1 || y < 0 || y > imageInfo->header.height - 1) {
        return;
    }

    int32_t offset = imageInfo->header.width * y + x;
    switch (imageInfo->header.colorMode) {
        case ARGB8888: {
            Color32* buffer = reinterpret_cast<Color32*>(const_cast<uint8_t*>(imageInfo->data));
            buffer[offset].alpha = buffer[offset].alpha * opa / OPA_OPAQUE;
            break;
        }
        default: {
            GRAPHIC_LOGE("Only images in ARGB8888 format are supported!");
            break;
        }
    }
}

void ClipUtils::DrawHorLine(int16_t x, int16_t y, int16_t len, uint8_t opa, const ImageInfo* imageInfo)
{
    for (int32_t i = x; i <= x + len; i++) {
        DrawPixel(i, y, opa, imageInfo);
    }
}
};
