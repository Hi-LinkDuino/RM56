/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

/**
* @brief Defines scanline renderer
* @since 1.0
* @version 1.0
*/

#include "render_scanline.h"

namespace OHOS {
void RenderScanlinesAntiAliasSolid(RasterizerScanlineAntialias& raster, GeometryScanline& scanline,
                                   RenderBase& renBase, const Rgba8T& color)
{
    if (!raster.RewindScanlines()) {
        return;
    }
    scanline.Reset(raster.GetMinX(), raster.GetMaxX());
    while (raster.SweepScanline(scanline)) {
        int32_t y = scanline.GetYLevel();
        uint32_t numSpans = scanline.NumSpans();
        GeometryScanline::ConstIterator span = scanline.Begin();
        while (true) {
            int32_t x = span->x;
            if (span->spanLength > 0) {
                renBase.BlendSolidHSpan(x, y, static_cast<uint32_t>(span->spanLength),
                                        color, span->covers);
            } else {
                renBase.BlendHLine(x, y, static_cast<uint32_t>(x - span->spanLength - 1),
                                   color, *(span->covers));
            }
            if (--numSpans == 0) {
                break;
            }
            ++span;
        }
    }
}

void RenderScanlinesAntiAlias(RasterizerScanlineAntialias& raster, GeometryScanline& scanline,
                              RenderBase& renBase, FillBase& alloc, SpanBase& spanGen)
{
    if (!raster.RewindScanlines()) {
        return;
    }
    scanline.Reset(raster.GetMinX(), raster.GetMaxX());
    spanGen.Prepare(); // Segment generator preparation
    alloc.Resize(raster.GetMaxX()-raster.GetMinX());
    while (raster.SweepScanline(scanline)) {
        int32_t y = scanline.GetYLevel();
        uint32_t numSpans = scanline.NumSpans();
        GeometryScanline::ConstIterator span = scanline.Begin();
        while (true) {
            int32_t x = span->x;
            int32_t len = span->spanLength;
            const uint8_t* covers = span->covers;

            if (len < 0) {
                len = -len;
            }

            spanGen.Generate(alloc.GetSpanPtr(), x, y, len);
            renBase.BlendColorHSpan(x, y, len, alloc.GetSpanPtr(),
                                    (span->spanLength < 0) ? 0 : covers, *covers);

            if (--numSpans == 0) {
                break;
            }
            ++span;
        }
    }
}

void BlendScanLine(GlobalCompositeOperation op, RasterizerScanlineAntialias& raster1,
                   RasterizerScanlineAntialias& raster2, GeometryScanline& sl1, GeometryScanline& sl2,
                   RenderBase& renBase, FillBase& alloc, SpanBase& spanGen1, SpanBase& spanGen2)
{
    switch (op) {
        case SOURCE_OVER:
            BlendSourceOver(raster2, raster1, sl2, sl1, renBase, alloc, spanGen2, spanGen1);
            break;
        case SOURCE_ATOP:
            BlendSourceAtop(raster1, raster2, sl1, sl2, renBase, alloc, spanGen1, spanGen2);
            break;
        case SOURCE_IN:
            BlendSourceIn(raster1, raster2, sl1, sl2, renBase, alloc, spanGen1);
            break;
        case SOURCE_OUT:
            BlendSourceOut(raster1, raster2, sl1, sl2, renBase, alloc, spanGen1);
            break;
        case DESTINATION_OVER:
            BlendSourceOver(raster1, raster2, sl1, sl2, renBase, alloc, spanGen1, spanGen2);
            break;
        case DESTINATION_ATOP:
            BlendSourceAtop(raster2, raster1, sl2, sl1, renBase, alloc, spanGen2, spanGen1);
            break;
        case DESTINATION_IN:
            BlendSourceIn(raster2, raster1, sl2, sl1, renBase, alloc, spanGen2);
            break;
        case DESTINATION_OUT:
            BlendSourceOut(raster2, raster1, sl2, sl1, renBase, alloc, spanGen2);
            break;
        case LIGHTER:
            BlendLIGHTER(raster1, raster2, sl1, sl2, renBase, alloc, spanGen1, spanGen2);
            break;
        case COPY:
            RenderScanlinesAntiAlias(raster1, sl1, renBase, alloc, spanGen1);
            break;
        case XOR:
            BlendXOR(raster1, raster2, sl1, sl2, renBase, alloc, spanGen1, spanGen2);
            break;
        default:
            break;
    }
}

void BlendSourceAtop(RasterizerScanlineAntialias& raster1, RasterizerScanlineAntialias& raster2,
                     GeometryScanline& scanline1, GeometryScanline& scanline2, RenderBase& renBase,
                     FillBase& alloc, SpanBase& spanGen1, SpanBase& spanGen2)
{
    if ((!raster1.RewindScanlines()) || (!raster2.RewindScanlines())) {
        return;
    }
    scanline1.Reset(raster1.GetMinX(), raster1.GetMaxX());
    scanline2.Reset(raster2.GetMinX(), raster2.GetMaxX());
    int16_t allocMinx = (raster1.GetMinX() > raster2.GetMinX()) ? raster2.GetMinX() : raster1.GetMinX();
    int16_t allocMaxX = (raster1.GetMaxX() > raster2.GetMaxX()) ? raster1.GetMaxX() : raster2.GetMaxX();
    alloc.Resize(allocMaxX - allocMinx);
    int32_t y1 = 0;
    if (raster1.SweepScanline(scanline1)) {
        y1 = scanline1.GetYLevel();
        spanGen1.Prepare(); // Segment generator preparation
    }
    while (raster2.SweepScanline(scanline2)) {
        spanGen2.Prepare();
        int32_t y2 = scanline2.GetYLevel();
        uint32_t numSpans2 = scanline2.NumSpans();
        GeometryScanline::ConstIterator span2 = scanline2.Begin();
        uint32_t numSpans1;
        GeometryScanline::ConstIterator span1;
        if (y1 == y2) {
            numSpans1 = scanline1.NumSpans();
            span1 = scanline1.Begin();
        }
        if (y2 > y1) {
            while (raster1.SweepScanline(scanline1)) {
                y1 = scanline1.GetYLevel();
                if (y1 == y2) {
                    numSpans1 = scanline1.NumSpans();
                    span1 = scanline1.Begin();
                    break;
                }
            }
        }
        while (true) {
            int32_t x2 = span2->x;
            if (y1 == y2) {
                int32_t x1 = span1->x;
                if (span2->spanLength > 0) {
                    if (span1->spanLength > 0) {
                        spanGen2.Generate(alloc.GetSpanPtr(), x2, y2, span2->spanLength);
                        renBase.BlendColorHSpan(x2, y2, (uint32_t)span2->spanLength,
                                                alloc.GetSpanPtr(), span2->covers);
                        if ((x1 <= x2) && ((x2 + span2->spanLength) < (x1 + span1->spanLength))) {
                            spanGen1.Generate(alloc.GetSpanPtr(), x2, y2, span2->spanLength);
                            renBase.BlendColorHSpan(x2, y2, (uint32_t)span2->spanLength,
                                                    alloc.GetSpanPtr(),
                                                    span2->covers);
                        } else if ((x1 >= x2) && ((x1 + span1->spanLength) <= (x2 + span2->spanLength))) {
                            spanGen1.Generate(alloc.GetSpanPtr(), x1, y2, span1->spanLength);
                            renBase.BlendColorHSpan(x1, y2, (uint32_t)span1->spanLength,
                                                    alloc.GetSpanPtr(),
                                                    span1->covers);
                        } else if ((x1 > x2) && (x1 < (x2 + span2->spanLength)) &&
                                   ((x1 + span1->spanLength) >= (x2 + span2->spanLength))) {
                            GeometryScanline scanline3;
                            CalcinterScanline(scanline3, x1, x2, span1, span2);
                            GeometryScanline::ConstIterator span3 = scanline3.Begin();
                            spanGen1.Generate(alloc.GetSpanPtr(), x1, y2, span3->spanLength);
                            renBase.BlendColorHSpan(x1, y2, (uint32_t)span3->spanLength,
                                                    alloc.GetSpanPtr(),
                                                    span3->covers);
                        } else if ((x1 < x2) && ((x1 + span1->spanLength) > x2) &&
                                   ((x1 + span1->spanLength) <= (x2 + span2->spanLength))) {
                            GeometryScanline scanline3;
                            CalcinterScanline(scanline3, x2, x1, span2, span1);
                            GeometryScanline::ConstIterator span3 = scanline3.Begin();
                            spanGen1.Generate(alloc.GetSpanPtr(), x2, y2, span3->spanLength);
                            renBase.BlendColorHSpan(x2, y2, (uint32_t)span3->spanLength,
                                                    alloc.GetSpanPtr(),
                                                    span3->covers);
                        }
                    }
                }
                ++span1;
                } else {
                    if (span2->spanLength > 0) {
                        spanGen2.Generate(alloc.GetSpanPtr(), x2, y2, span2->spanLength);
                        renBase.BlendColorHSpan(x2, y2, (uint32_t)span2->spanLength,
                                                alloc.GetSpanPtr(),
                                                span2->covers);
                    }
                }
            if (--numSpans2 == 0) {
                break;
            }
            ++span2;
        }
        if (y1 == y2) {
            raster1.SweepScanline(scanline1);
            y1 = scanline1.GetYLevel();
            numSpans1 = scanline1.NumSpans();
            span1 = scanline1.Begin();
        }
    }
}

void BlendSourceIn(RasterizerScanlineAntialias& raster1, RasterizerScanlineAntialias& raster2,
                   GeometryScanline& scanline1, GeometryScanline& scanline2, RenderBase& renBase,
                   FillBase& alloc, SpanBase& spanGen1)
{
    if (!raster1.RewindScanlines() || !raster2.RewindScanlines()) {
        return;
    }

    scanline1.Reset(raster1.GetMinX(), raster1.GetMaxX());
    scanline2.Reset(raster2.GetMinX(), raster2.GetMaxX());

    int16_t allocMinx = (raster1.GetMinX() > raster2.GetMinX()) ? raster2.GetMinX() : raster1.GetMinX();
    int16_t allocMaxX = (raster1.GetMaxX() > raster2.GetMaxX()) ? raster1.GetMaxX() : raster2.GetMaxX();
    alloc.Resize(allocMaxX-allocMinx);
    int32_t y1 = 0;
    if (raster1.SweepScanline(scanline1)) {
        y1 = scanline1.GetYLevel();
        spanGen1.Prepare();
    }
    while (raster2.SweepScanline(scanline2)) {
        BlendSourceInLoop(raster1, scanline1, scanline2, spanGen1, renBase, alloc, y1);
    }
}

void BlendSourceOut(RasterizerScanlineAntialias& raster1, RasterizerScanlineAntialias& raster2,
                    GeometryScanline& scanline1, GeometryScanline& scanline2,
                    RenderBase& renBase, FillBase& alloc, SpanBase& spanGen1)
{
    if (!raster1.RewindScanlines() || !raster2.RewindScanlines()) {
        return;
    }
    scanline1.Reset(raster1.GetMinX(), raster1.GetMaxX());
    scanline2.Reset(raster2.GetMinX(), raster2.GetMaxX());
    int16_t allocMinx = (raster1.GetMinX() > raster2.GetMinX()) ? raster2.GetMinX() : raster1.GetMinX();
    int16_t allocMaxX = (raster1.GetMaxX() > raster2.GetMaxX()) ? raster1.GetMaxX() : raster2.GetMaxX();
    alloc.Resize(allocMaxX-allocMinx);
    int32_t y1 = 0;
    if (raster1.SweepScanline(scanline1)) {
        y1 = scanline1.GetYLevel();
        spanGen1.Prepare();
    }
    if (raster1.GetMinY() < raster2.GetMinY()) {
        do {
            y1 = scanline1.GetYLevel();
            if (y1 == raster2.GetMinY()) {
                break;
            }
            BlendSourceOutWhile(y1, scanline1, renBase, alloc, spanGen1);
        } while (raster1.SweepScanline(scanline1));
    }
    while (raster2.SweepScanline(scanline2)) {
        int32_t y2 = scanline2.GetYLevel();
        uint32_t numSpans2 = scanline2.NumSpans();
        GeometryScanline::ConstIterator span2 = scanline2.Begin();
        uint32_t numSpans1;
        GeometryScanline::ConstIterator span1;
        if (y1 == y2) {
            numSpans1 = scanline1.NumSpans();
            span1 = scanline1.Begin();
        }
        while (true) {
            int32_t x2 = span2->x;
            if (y1 == y2) {
                int32_t x1 = span1->x;
                if (span2->spanLength > 0) {
                    if (span1->spanLength > 0) {
                        if ((x1 < x2) && (x2 + span2->spanLength) < (x1 + span1->spanLength)) {
                            GeometryScanline scanline3;
                            CalcOutScanlineAll(scanline3, x2, x1, span2, span1);
                            GeometryScanline::ConstIterator span3 = scanline3.Begin();
                            spanGen1.Generate(alloc.GetSpanPtr(), x1, y2, span3->spanLength);
                            renBase.BlendColorHSpan(x1, y2, (uint32_t)span3->spanLength,
                                                    alloc.GetSpanPtr(),
                                                    span3->covers);
                        } else if ((x1 >= x2) && x1 < x2 + span2->spanLength &&
                                   ((x1 + span1->spanLength) >= (x2 + span2->spanLength))) {
                            GeometryScanline scanline4;
                            CalcOutScanlineLeft(scanline4, x1, x2, span1, span2);
                            GeometryScanline::ConstIterator span4 = scanline4.Begin();
                            spanGen1.Generate(alloc.GetSpanPtr(), x1, y2, span4->spanLength);
                            renBase.BlendColorHSpan(x1, y2, (uint32_t)span4->spanLength,
                                                    alloc.GetSpanPtr(),
                                                    span4->covers);
                        } else if ((x1 <= x2) && (x1 + span1->spanLength) >= x2 &&
                                   ((x1 + span1->spanLength) <= (x2 + span2->spanLength))) {
                            GeometryScanline scanline3;
                            CalcOutScanlineRight(scanline3, x2, x1, span2, span1);
                            GeometryScanline::ConstIterator span3 = scanline3.Begin();
                            spanGen1.Generate(alloc.GetSpanPtr(), x1, y2, span3->spanLength);
                            renBase.BlendColorHSpan(x1, y2, (uint32_t)span3->spanLength,
                                                    alloc.GetSpanPtr(), span3->covers);
                        } else if (((x1 + span1->spanLength) < x2) || ((x2 + span2->spanLength) < x1)) {
                            spanGen1.Generate(alloc.GetSpanPtr(), x1, y2, span1->spanLength);
                            renBase.BlendColorHSpan(x1, y2, (uint32_t)span1->spanLength,
                                                    alloc.GetSpanPtr(),
                                                    span1->covers);
                        }
                    }
                }
                ++span1;
            }
            if (--numSpans2 == 0) {
                break;
            }
            ++span2;
        }
        if (y1 == y2 && y1 < raster2.GetMaxY() - 1) {
            if (raster1.SweepScanline(scanline1)) {
                y1 = scanline1.GetYLevel();
                numSpans1 = scanline1.NumSpans();
                span1 = scanline1.Begin();
            }
        }
    }
    BlendSourceOutDrawResetRaster(raster1, scanline1, renBase, alloc, spanGen1);
}

void BlendSourceOutWhile(int32_t y1, GeometryScanline& scanline1, RenderBase& renBase,
                         FillBase& alloc, SpanBase& spanGen1)
{
    uint32_t numSpans = scanline1.NumSpans();
    typename GeometryScanline::ConstIterator span = scanline1.Begin();
    while (true) {
        int32_t x = span->x;
        if (span->spanLength > 0) {
            spanGen1.Generate(alloc.GetSpanPtr(), x, y1, span->spanLength);
            renBase.BlendColorHSpan(x, y1, (uint32_t)span->spanLength,
                                    alloc.GetSpanPtr(),
                                    span->covers);
        }
        if (--numSpans == 0) {
            break;
        }
        ++span;
    }
}

void BlendSourceOutDrawResetRaster(RasterizerScanlineAntialias& raster1, GeometryScanline& scanline1,
                                   RenderBase& renBase, FillBase& alloc, SpanBase& spanGen1)
{
    while (raster1.SweepScanline(scanline1)) {
        int32_t y = scanline1.GetYLevel();
        uint32_t numSpans = scanline1.NumSpans();
        GeometryScanline::ConstIterator span = scanline1.Begin();
        while (true) {
            int32_t x = span->x;
            if (span->spanLength > 0) {
                spanGen1.Generate(alloc.GetSpanPtr(), x, y, span->spanLength);
                renBase.BlendColorHSpan(x, y, (uint32_t)span->spanLength,
                                        alloc.GetSpanPtr(),
                                        span->covers);
            }
            if (--numSpans == 0) {
                break;
            }
            ++span;
        }
    }
}

void BlendSourceOver(RasterizerScanlineAntialias &raster1,
                     RasterizerScanlineAntialias &raster2,
                     GeometryScanline &scanline1,
                     GeometryScanline &scanline2,
                     RenderBase &renBase,
                     FillBase &alloc,
                     SpanBase&&spanGen1,
                     SpanBase &spanGen2)
{
    RenderScanlinesAntiAlias(raster1, scanline1, renBase, alloc, spanGen1);
    RenderScanlinesAntiAlias(raster2, scanline2, renBase, alloc, spanGen2);
}

void BlendXOR(RasterizerScanlineAntialias& raster1, RasterizerScanlineAntialias& raster2,
              GeometryScanline& scanline1, GeometryScanline& scanline2,
              RenderBase& renBase, FillBase& alloc, SpanBase& spanGen1, SpanBase& spanGen2)
{
    if (!raster1.RewindScanlines() || !raster2.RewindScanlines()) {
        return;
    }
    scanline1.Reset(raster1.GetMinX(), raster1.GetMaxX());
    scanline2.Reset(raster2.GetMinX(), raster2.GetMaxX());
    int16_t allocMinx = raster1.GetMinX() > raster2.GetMinX() ? raster2.GetMinX() : raster1.GetMinX();
    int16_t allocMaxX = raster1.GetMaxX() > raster2.GetMaxX() ? raster1.GetMaxX() : raster2.GetMaxX();
    alloc.Resize(allocMaxX-allocMinx);
    int32_t y1 = 0;
    if (raster1.SweepScanline(scanline1)) {
        y1 = scanline1.GetYLevel();
        spanGen1.Prepare();
    }
    BlendXORColorHspan(y1, raster1, raster2, scanline1, renBase, alloc, spanGen1);
    while (raster2.SweepScanline(scanline2)) {
        int32_t y2 = scanline2.GetYLevel();
        spanGen2.Prepare();
        uint32_t numSpans2 = scanline2.NumSpans();
        GeometryScanline::ConstIterator span2 = scanline2.Begin();
        uint32_t numSpans1;
        GeometryScanline::ConstIterator span1;
        if (y1 == y2) {
            numSpans1 = scanline1.NumSpans();
            span1 = scanline1.Begin();
        }
        while (true) {
            int32_t x2 = span2->x;
            if (y1 == y2) {
                {
                int32_t x1 = span1->x;
                if (span2->spanLength > 0) {
                    if (span1->spanLength > 0) {
                        if ((x1 >= x2) && ((x1 + span1->spanLength) <= (x2 + span2->spanLength))) {
                            GeometryScanline scanline3;
                            CalcOutScanlineAll(scanline3, x1, x2, span1, span2);
                            GeometryScanline::ConstIterator span3 = scanline3.Begin();
                            spanGen2.Generate(alloc.GetSpanPtr(), x2, y2, span3->spanLength);
                            renBase.BlendColorHSpan(x2, y2, (uint32_t)span3->spanLength,
                                                    alloc.GetSpanPtr(),
                                                    span3->covers);
                        } else if (((x1 < x2) && ((x2 + span2->spanLength) < (x1 + span1->spanLength)))) {
                            GeometryScanline scanline3;
                            CalcOutScanlineAll(scanline3, x2, x1, span2, span1);
                            GeometryScanline::ConstIterator span3 = scanline3.Begin();
                            spanGen1.Generate(alloc.GetSpanPtr(), x1, y2, span3->spanLength);
                            renBase.BlendColorHSpan(x1, y2, (uint32_t)span3->spanLength,
                                                    alloc.GetSpanPtr(),
                                                    span3->covers);
                        } else if ((x1 >= x2) && (x1 < x2 + span2->spanLength) &&
                            (x1 + span1->spanLength >= x2 + span2->spanLength)) {
                            GeometryScanline scanline3;
                            CalcOutScanlineRight(scanline3, x1, x2, span1, span2);
                            GeometryScanline::ConstIterator span3 = scanline3.Begin();
                            spanGen2.Generate(alloc.GetSpanPtr(), x2, y2, span3->spanLength);
                            renBase.BlendColorHSpan(x2, y2, (uint32_t)span3->spanLength,
                                                    alloc.GetSpanPtr(),
                                                    span3->covers);
                            GeometryScanline scanline4;
                            CalcOutScanlineLeft(scanline4, x1, x2, span1, span2);
                            GeometryScanline::ConstIterator span4 = scanline4.Begin();
                            spanGen1.Generate(alloc.GetSpanPtr(), x1, y2, span4->spanLength);
                            renBase.BlendColorHSpan(x1, y2, (uint32_t)span4->spanLength,
                                                    alloc.GetSpanPtr(),
                                                    span4->covers);
                        } else if ((x1 <= x2) && (x1 + span1->spanLength >= x2) &&
                            (x1 + span1->spanLength <= x2 + span2->spanLength)) {
                            GeometryScanline scanline3;
                            CalcOutScanlineRight(scanline3, x2, x1, span2, span1);
                            GeometryScanline::ConstIterator span3 = scanline3.Begin();
                            spanGen1.Generate(alloc.GetSpanPtr(), x1, y2, span3->spanLength);
                            renBase.BlendColorHSpan(x1, y2, (uint32_t)span3->spanLength,
                                                    alloc.GetSpanPtr(), span3->covers);
                            GeometryScanline scanline4;
                            CalcOutScanlineLeft(scanline4, x2, x1, span2, span1);
                            GeometryScanline::ConstIterator span4 = scanline4.Begin();
                            spanGen2.Generate(alloc.GetSpanPtr(), x2, y2, span4->spanLength);
                            renBase.BlendColorHSpan(x2, y2, (uint32_t)span4->spanLength,
                                                    alloc.GetSpanPtr(), span4->covers);
                        } else if ((x1 + span1->spanLength < x2)|| (x2+span2->spanLength < x1)) {
                            spanGen1.Generate(alloc.GetSpanPtr(), x1, y2, span1->spanLength);
                            renBase.BlendColorHSpan(x1, y2, (uint32_t)span1->spanLength,
                                                    alloc.GetSpanPtr(), span1->covers);
                            spanGen2.Generate(alloc.GetSpanPtr(), x2, y2, span2->spanLength);
                            renBase.BlendColorHSpan(x2, y2, (uint32_t)span2->spanLength,
                                                    alloc.GetSpanPtr(), span2->covers);
                        }
                    }
                }
                    ++span1;
                }
            } else if (span2->spanLength > 0) {
                spanGen2.Generate(alloc.GetSpanPtr(), x2, y2, span2->spanLength);
                renBase.BlendColorHSpan(x2, y2, (uint32_t)span2->spanLength, alloc.GetSpanPtr(), span2->covers);
            }
            if (--numSpans2 == 0) {
                break;
            }
            ++span2;
        }
        if (y1 == y2 && y1 < raster2.GetMaxY() - 1) {
            if (raster1.SweepScanline(scanline1)) {
                y1 = scanline1.GetYLevel();
                numSpans1 = scanline1.NumSpans();
                span1 = scanline1.Begin();
            }
        }
    }
    // Draw the rest of raster1
    BlendXORDrawResetRaster(raster1, scanline1, renBase, alloc, spanGen1);
}

void BlendXORDrawResetRaster(RasterizerScanlineAntialias& raster1, GeometryScanline& scanline1,
                             RenderBase& renBase, FillBase& alloc, SpanBase& spanGen1)

{
    while (raster1.SweepScanline(scanline1)) {
        int32_t y = scanline1.GetYLevel();
        uint32_t numSpans = scanline1.NumSpans();
        GeometryScanline::ConstIterator span = scanline1.Begin();
        while (true) {
            int32_t x = span->x;
            if (span->spanLength > 0) {
                spanGen1.Generate(alloc.GetSpanPtr(), x, y, span->spanLength);
                renBase.BlendColorHSpan(x, y, (uint32_t)span->spanLength, alloc.GetSpanPtr(), span->covers);
            }
            if (--numSpans == 0) {
                break;
            }
            ++span;
        }
    }
}

void BlendXORColorHspan(int32_t& y1, RasterizerScanlineAntialias& raster1, RasterizerScanlineAntialias& raster2,
                        GeometryScanline& scanline1, RenderBase& renBase, FillBase& alloc, SpanBase& spanGen1)
{
    if (raster1.GetMinY() > raster2.GetMinY()) {
        return;
    }
    do {
        y1 = scanline1.GetYLevel();
        if (y1 == raster2.GetMinY()) {
            break;
        }
        uint32_t numSpans = scanline1.NumSpans();
        GeometryScanline::ConstIterator span = scanline1.Begin();
        while (true) {
            int32_t x = span->x;
            if (span->spanLength > 0) {
                spanGen1.Generate(alloc.GetSpanPtr(), x, y1, span->spanLength);
                renBase.BlendColorHSpan(x, y1, (uint32_t)span->spanLength,
                                        alloc.GetSpanPtr(), span->covers);
            }
            if (--numSpans == 0) {
                break;
            }
            ++span;
        }
    } while (raster1.SweepScanline(scanline1));
}

void BlendLIGHTER(RasterizerScanlineAntialias& raster1, RasterizerScanlineAntialias& raster2,
                  GeometryScanline& scanline1, GeometryScanline& scanline2,
                  RenderBase& renBase, FillBase& alloc, SpanBase& spanGen1, SpanBase& spanGen2)
{
    BlendXOR(raster1, raster2, scanline1, scanline2, renBase, alloc, spanGen1, spanGen2);
    SpanBlendColor spanBlendColor(spanGen1, spanGen2);
    BlendSourceIn(raster1, raster2, scanline1, scanline2, renBase, alloc, spanBlendColor);
}

void BlendSourceInLoop(RasterizerScanlineAntialias& raster1, GeometryScanline& scanline1, GeometryScanline& scanline2,
                       SpanBase& spanGen1, RenderBase& renBase, FillBase& alloc, int32_t& y1)
{
    int32_t y2 = scanline2.GetYLevel();
    uint32_t numSpans2 = scanline2.NumSpans();
    GeometryScanline::ConstIterator span2 = scanline2.Begin();

    uint32_t numSpans1;
    GeometryScanline::ConstIterator span1;

    if (y1 == y2) {
        numSpans1 = scanline1.NumSpans();
        span1 = scanline1.Begin();
    }

    if (y2 > y1) {
        while (raster1.SweepScanline(scanline1)) {
            y1 = scanline1.GetYLevel();
            if (y1 == y2) {
                numSpans1 = scanline1.NumSpans();
                span1 = scanline1.Begin();
                break;
            }
        }
    }
    while (true) {
        int32_t x2 = span2->x;
        if (y1 == y2) {
            int32_t x1 = span1->x;
            if (span2->spanLength > 0) {
                if (span1->spanLength > 0) {
                    if (x1<=x2 && x2+span2->spanLength < x1+span1->spanLength) {
                        spanGen1.Generate(alloc.GetSpanPtr(), x2, y2, span2->spanLength);
                        renBase.BlendColorHSpan(x2, y2, (uint32_t)span2->spanLength,
                                                alloc.GetSpanPtr(), span2->covers);
                    } else if ((x1 >= x2) && (x1 + span1->spanLength <= x2+span2->spanLength)) {
                        spanGen1.Generate(alloc.GetSpanPtr(), x1, y2, span1->spanLength);
                        renBase.BlendColorHSpan(x1, y2, (uint32_t)span1->spanLength,
                                                alloc.GetSpanPtr(), span1->covers);
                    } else if ((x1 > x2) && (x1 < x2 + span2->spanLength) &&
                        (x1 + span1->spanLength >= x2 + span2->spanLength)) {
                        GeometryScanline scanline3;
                        CalcinterScanline(scanline3, x1, x2, span1, span2);
                        GeometryScanline::ConstIterator span3 = scanline3.Begin();
                        spanGen1.Generate(alloc.GetSpanPtr(), x1, y2, span3->spanLength);
                        renBase.BlendColorHSpan(x1, y2, (uint32_t)span3->spanLength,
                                                alloc.GetSpanPtr(), span3->covers);
                    } else if ((x1 < x2) && (x1 + span1->spanLength > x2) &&
                        (x1 + span1->spanLength <= x2 + span2->spanLength)) {
                        GeometryScanline scanline3;
                        CalcinterScanline(scanline3, x2, x1, span2, span1);
                        GeometryScanline::ConstIterator span3 = scanline3.Begin();
                        spanGen1.Generate(alloc.GetSpanPtr(), x2, y2, span3->spanLength);
                        renBase.BlendColorHSpan(x2, y2, (uint32_t)span3->spanLength,
                                                alloc.GetSpanPtr(), span3->covers);
                    }
                }
            }
            ++span1;
        }
        if (--numSpans2 == 0) {
            break;
        }
        ++span2;
    }
    if (y1 == y2) {
        raster1.SweepScanline(scanline1);
        y1 = scanline1.GetYLevel();
        numSpans1 = scanline1.NumSpans();
        span1 = scanline1.Begin();
    }
}

void BlendSourceOver(RasterizerScanlineAntialias& raster1, RasterizerScanlineAntialias& raster2,
                     GeometryScanline& scanline1, GeometryScanline& scanline2,
                     RenderBase& renBase, FillBase& alloc, SpanBase& spanGen1, SpanBase& spanGen2)
{
    RenderScanlinesAntiAlias(raster1, scanline1, renBase, alloc, spanGen1);
    RenderScanlinesAntiAlias(raster2, scanline2, renBase, alloc, spanGen2);
}

void CalcinterScanline(GeometryScanline& scanline3, int32_t x1, int32_t x2,
                       GeometryScanline::ConstIterator span1, GeometryScanline::ConstIterator span2)
{
    scanline3.Reset(x1, x2 + span2->spanLength);
    uint32_t len3 = x2 + span2->spanLength - x1;
    uint8_t* cover1 = span1->covers;
    uint8_t* cover2 = span2->covers + (x1 - x2);
    int32_t x3 = x1;
    for (uint32_t i = 0; i < len3; i++, cover1++, cover2++) {
        if (*(cover1) != COVER_FULL) {
            scanline3.AddCell(x3++, *cover1);
        } else {
            scanline3.AddCell(x3++, *cover2);
        }
    }
}

void CalcOutScanlineRight(GeometryScanline& scanline, int32_t x1, int32_t x2,
                          GeometryScanline::ConstIterator span1, GeometryScanline::ConstIterator span2)
{
    uint8_t* cover1 = span1->covers;
    uint8_t* cover2 = span2->covers;
    scanline.Reset(x2, x2 + span2->spanLength);
    for (int32_t i = 1; i <= span2->spanLength; i++, cover2++) {
        if (x1 == x2) {
            scanline.AddCell(x2++, COVER_FULL - (*cover1));
            x1++;
            cover1++;
        } else {
            scanline.AddCell(x2++, *cover2);
        }
    }
}

void CalcOutScanlineLeft(GeometryScanline& scanline, int32_t x1, int32_t x2,
                         GeometryScanline::ConstIterator span1, GeometryScanline::ConstIterator span2)
{
    uint8_t* cover2 = span2->covers + (x1 - x2);
    scanline.Reset(x1, x1 + span1->spanLength);
    int32_t len1 = span2->spanLength - (x1 - x2); // Length of intersection
    int32_t len2 = span1->spanLength - len1;      // Length of intersection
    uint8_t* cover1 = span1->covers + len1;
    for (int32_t i = 0; i < len1; i++, cover2++) {
        scanline.AddCell(x1++, COVER_FULL - (*cover2));
    }

    for (int32_t i = 0; i < len2; i++, cover1++) {
        scanline.AddCell(x1++, *cover1);
    }
}

void CalcOutScanlineAll(GeometryScanline& scanline, int32_t x1, int32_t x2,
                        GeometryScanline::ConstIterator span1, GeometryScanline::ConstIterator span2)
{
    uint8_t* cover1 = span1->covers;
    uint8_t* cover2 = span2->covers;
    int32_t x3 = x1;
    scanline.Reset(x2, x2 + span2->spanLength);
    for (int32_t i = 0; i < span2->spanLength; i++, cover2++) {
        if (x3 == x2 && x3 <= x1 + span1->spanLength - 1) {
            scanline.AddCell(x2++, COVER_FULL - (*cover1));
            x3++;
            cover1++;
        } else {
            scanline.AddCell(x2++, *cover2);
        }
    }
}
} // namespace OHOS
