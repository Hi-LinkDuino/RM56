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

#include "gfx_utils/diagram/rasterizer/rasterizer_scanline_antialias.h"

namespace OHOS {
/**
 * @brief Scan line that gets a y value from rasterizer stage
 * And iterate the cell array of the current scan line to obtain area->cover，
 * Use both to calculate delta area as area cover and convert it into gamma cover
 * the color information is obtained successfully,
 * and then the alpha information of color is calculated by gamma function
 * Fill in the new scanline and have subsequent render.
 * @since 1.0
 * @version 1.0
 */
bool RasterizerScanlineAntialias::SweepScanline(GeometryScanline& sl)
{
    while (true) {
        if (scanY_ > outline_.GetMaxY()) {
            return false;
        }
        sl.ResetSpans();
        uint32_t numCells = outline_.GetScanlineNumCells(scanY_);
        const CellBuildAntiAlias * const *cells = outline_.GetScanlineCells(scanY_);
        int32_t cover = 0;

        while (numCells) {
            const CellBuildAntiAlias* curCell = *cells;
            int32_t x = curCell->x;
            int32_t area = curCell->area;
            uint32_t alpha;

            cover += curCell->cover;
            // accumulate all cells with the same X
            while (--numCells && (curCell = *++cells) && (curCell->x == x)) {
                area += curCell->area;
                cover += curCell->cover;
            }
            if (area) {
                // Span interval from area to  (cover << (POLY_SUBPIXEL_SHIFT + 1))
                // Cover can be understood as a delta mask with an area of 1
                alpha = CalculateAlpha((cover << (POLY_SUBPIXEL_SHIFT + 1)) - area);
                if (alpha) {
                    sl.AddCell(x, alpha);
                }
                x++;
            }
            if (numCells && curCell->x > x) {
                // At this time, area is 0, that is, 0 to cover << (POLY_SUBPIXEL_SHIFT + 1)
                alpha = CalculateAlpha(cover << (POLY_SUBPIXEL_SHIFT + 1));
                if (alpha) {
                    sl.AddSpan(x, curCell->x - x, alpha);
                }
            }
        }

        if (sl.NumSpans()) {
            break;
        }
        ++scanY_;
    }

    sl.Finalize(scanY_);
    ++scanY_;
    return true;
}

/**
 * @brief Convert area cover to gamma cover value to calculate alpha.
 * @since 1.0
 * @version 1.0
 */
uint32_t RasterizerScanlineAntialias::CalculateAlpha(int32_t area) const
{
    int32_t cover = area >> (POLY_SUBPIXEL_SHIFT * 2 + 1 - AA_SHIFT);

    if (cover < 0) {
        cover = -cover;
    }
    if (fillingRule_ == FILL_EVEN_ODD) {
        cover &= AA_MASK2;
        if (cover > AA_SCALE) {
            cover = AA_SCALE2 - cover;
        }
    }
    if (cover > AA_MASK) {
        cover = AA_MASK;
    }
    return gammar_[cover];
}

void RasterizerScanlineAntialias::Reset()
{
    outline_.Reset();
    status_ = STATUS_INITIAL;
}

void RasterizerScanlineAntialias::ClipBox(float x1, float y1, float x2, float y2)
{
    Reset();
    clipper_.ClipBox(RasterDepictInt::UpScale(x1), RasterDepictInt::UpScale(y1),
                     RasterDepictInt::UpScale(x2), RasterDepictInt::UpScale(y2));
}

void RasterizerScanlineAntialias::ResetClipping()
{
    Reset();
    clipper_.ResetClipping();
}

void RasterizerScanlineAntialias::ClosePolygon()
{
    if (status_ == STATUS_LINE_TO) {
        clipper_.LineTo(outline_, startX_, startY_);
        status_ = STATUS_CLOSED;
    }
}

void RasterizerScanlineAntialias::MoveTo(int32_t x, int32_t y)
{
    if (outline_.GetSorted()) {
        Reset();
    }
    if (autoClose_) {
        ClosePolygon();
    }
    clipper_.MoveTo(startX_ = RasterDepictInt::DownScale(x),
                    startY_ = RasterDepictInt::DownScale(y));
    status_ = STATUS_MOVE_TO;
}

void RasterizerScanlineAntialias::LineTo(int32_t x, int32_t y)
{
    clipper_.LineTo(outline_, RasterDepictInt::DownScale(x), RasterDepictInt::DownScale(y));
    status_ = STATUS_LINE_TO;
}

void RasterizerScanlineAntialias::MoveToByfloat(float x, float y)
{
    if (outline_.GetSorted()) {
        Reset();
    }
    if (autoClose_) {
        ClosePolygon();
    }
    clipper_.MoveTo(startX_ = RasterDepictInt::UpScale(x),
                    startY_ = RasterDepictInt::UpScale(y));
    status_ = STATUS_MOVE_TO;
}

void RasterizerScanlineAntialias::LineToByfloat(float x, float y)
{
    clipper_.LineTo(outline_, RasterDepictInt::UpScale(x), RasterDepictInt::UpScale(y));
    status_ = STATUS_LINE_TO;
}

void RasterizerScanlineAntialias::AddVertex(float x, float y, uint32_t cmd)
{
    if (IsMoveTo(cmd)) {
        MoveToByfloat(x, y);
    } else if (IsVertex(cmd)) {
        LineToByfloat(x, y);
    } else if (IsClose(cmd)) {
        ClosePolygon();
    }
}

void RasterizerScanlineAntialias::Sort()
{
    if (autoClose_) {
        ClosePolygon();
    }
    outline_.SortAllCells();
}

bool RasterizerScanlineAntialias::RewindScanlines()
{
    if (autoClose_) {
        ClosePolygon();
    }
    outline_.SortAllCells();
    if (outline_.GetTotalCells() == 0) {
        return false;
    }
    scanY_ = outline_.GetMinY();
    return true;
}
} // namespace OHOS
