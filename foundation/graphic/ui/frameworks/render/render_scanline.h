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
* @file render_scanline.h
* @brief Defines scanline renderer
* @since 1.0
* @version 1.0
*/

#ifndef GRAPHIC_LITE_RENDER_SCANLINE_H
#define GRAPHIC_LITE_RENDER_SCANLINE_H

#include <cstdlib>
#include <cstring>
#include <limits>
#include "gfx_utils/color.h"
#include "gfx_utils/diagram/spancolorfill/fill_base.h"
#include "gfx_utils/diagram/scanline/geometry_scanline.h"
#include "gfx_utils/diagram/rasterizer/rasterizer_scanline_antialias.h"
#include "render/render_base.h"

namespace OHOS {
/**
 * @brief Anti aliasing scanline for rendering solid lines.
 * Via scanline Begin gets the first span, and + + span gets the next span.
 * The corresponding function is called by renbase to draw the color to the
 * canvas where the corresponding span is located.
 * @param raster grating
 * @param scanline Scan line
 * @param renBase Renderer
 * @param color colour
 */
void RenderScanlinesAntiAliasSolid(RasterizerScanlineAntialias& raster, GeometryScanline& scanline,
                                   RenderBase& renBase, const Rgba8T& color);

/**
 * @brief Rendering anti aliased scanlines.
 * Via scanline Begin gets the first span, and + + span gets the next span.
 * Via allocat_ - > Resize (spanlen) allocates a color_type with the same length as span.
 * Through segment generator spangenerate_ - > Generate(colors, x, y, len); Fill the color
 * array to get the color array with values corresponding to the scan line span.
 * Finally, through renbase_ Call the corresponding function to draw the color array
 * to the canvas position of the corresponding span.
 * @param raster grating
 * @param scanline Scan line
 * @param renBase Renderer
 * @param alloc distributor
 * @param spanGen Segment generator
 */
void RenderScanlinesAntiAlias(RasterizerScanlineAntialias& raster, GeometryScanline& scanline,
                              RenderBase& renBase, FillBase& alloc, SpanBase& spanGen);

void BlendScanLine(GlobalCompositeOperation op, RasterizerScanlineAntialias& raster1,
                   RasterizerScanlineAntialias& raster2, GeometryScanline& sl1, GeometryScanline& sl2,
                   RenderBase& renBase, FillBase& alloc, SpanBase& spanGen1, SpanBase& spanGen2);

void CalcinterScanline(GeometryScanline& scanline3, int32_t x1, int32_t x2,
                       GeometryScanline::ConstIterator span1, GeometryScanline::ConstIterator span2);

void CalcOutScanlineRight(GeometryScanline& scanline, int32_t x1, int32_t x2,
                          GeometryScanline::ConstIterator span1, GeometryScanline::ConstIterator span2);

void CalcOutScanlineLeft(GeometryScanline& scanline, int32_t x1, int32_t x2,
                         GeometryScanline::ConstIterator span1, GeometryScanline::ConstIterator span2);

void CalcOutScanlineAll(GeometryScanline& scanline, int32_t x1, int32_t x2,
                        GeometryScanline::ConstIterator span1, GeometryScanline::ConstIterator span2);

void BlendSourceAtop(RasterizerScanlineAntialias& raster1, RasterizerScanlineAntialias& raster2,
                     GeometryScanline& scanline1, GeometryScanline& scanline2, RenderBase& renBase,
                     FillBase& alloc, SpanBase& spanGen1, SpanBase& spanGen2);


void BlendSourceIn(RasterizerScanlineAntialias& raster1, RasterizerScanlineAntialias& raster2,
                   GeometryScanline& scanline1, GeometryScanline& scanline2, RenderBase& renBase,
                   FillBase& alloc, SpanBase& spanGen1);

void BlendSourceInLoop(RasterizerScanlineAntialias& raster1, GeometryScanline& scanline1, GeometryScanline& scanline2,
                       SpanBase& spanGen1, RenderBase& renBase, FillBase& alloc, int32_t& y1);

void BlendSourceOut(RasterizerScanlineAntialias& raster1, RasterizerScanlineAntialias& raster2,
                    GeometryScanline& scanline1, GeometryScanline& scanline2,
                    RenderBase& renBase, FillBase& alloc, SpanBase& spanGen1);

void BlendSourceOutWhile(int32_t y1, GeometryScanline& scanline1, RenderBase& renBase,
                         FillBase& alloc, SpanBase& spanGen1);

void BlendSourceOutDrawResetRaster(RasterizerScanlineAntialias& raster1, GeometryScanline& scanline1,
                                   RenderBase& renBase, FillBase& alloc, SpanBase& spanGen1);

void BlendSourceOver(RasterizerScanlineAntialias& raster1, RasterizerScanlineAntialias& raster2,
                     GeometryScanline& scanline1, GeometryScanline& scanline2,
                     RenderBase& renBase, FillBase& alloc, SpanBase& spanGen1, SpanBase& spanGen2);


void BlendXOR(RasterizerScanlineAntialias& raster1, RasterizerScanlineAntialias& raster2,
              GeometryScanline& scanline1, GeometryScanline& scanline2,
              RenderBase& renBase, FillBase& alloc, SpanBase& spanGen1, SpanBase& spanGen2);

void BlendXORDrawResetRaster(RasterizerScanlineAntialias& raster1, GeometryScanline& scanline1,
                             RenderBase& renBase, FillBase& alloc, SpanBase& spanGen1);

void BlendXORColorHspan(int32_t& y1, RasterizerScanlineAntialias& raster1, RasterizerScanlineAntialias& raster2,
                        GeometryScanline& scanline1, RenderBase& renBase, FillBase& alloc, SpanBase& spanGen1);


void BlendLIGHTER(RasterizerScanlineAntialias& raster1, RasterizerScanlineAntialias& raster2,
                  GeometryScanline& scanline1, GeometryScanline& scanline2,
                  RenderBase& renBase, FillBase& alloc, SpanBase& spanGen1, SpanBase& spanGen2);
} // namespace OHOS
#endif
