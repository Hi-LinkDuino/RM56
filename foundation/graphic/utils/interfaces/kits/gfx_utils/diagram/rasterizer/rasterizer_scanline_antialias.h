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
 * @file rasterizer_scanline_antialias.h
 * @brief Defines Rasterization and scanline process stage processing
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_RASTERIZER_SCANLINE_ANTIALIAS_H
#define GRAPHIC_LITE_RASTERIZER_SCANLINE_ANTIALIAS_H

#include "rasterizer_cells_antialias.h"
#include "rasterizer_scanline_clip.h"
#include "gfx_utils/diagram/scanline/geometry_scanline.h"
namespace OHOS {
/**
 * @class RasterizerScanlineAntiAlias
 * @brief Polygon rasterization is used for high-quality rendering of filled polygons,
 * The int32_t coordinates of this class include the format of 24.8,
 * and 24 bits are used for the int32_t part of the coordinates,
 * 8 bits are used for the shift offset part of the sub-pixel, and then move_ to(x, y) / line_ to(x, y)
 * When constructing polygons, the scanline is rasterized
 * @since 1.0
 * @version 1.0
 */
class RasterizerScanlineAntialias {
    /**
     * @brief Build scanline status enumeration
     * @since 1.0
     * @version 1.0
     */
    enum RasterizerStatus {
        STATUS_INITIAL,
        STATUS_MOVE_TO,
        STATUS_LINE_TO,
        STATUS_CLOSED
    };

public:
    enum AntialiasScale {
        AA_SHIFT = 8,
        AA_SCALE = 1 << AA_SHIFT,
        AA_MASK = AA_SCALE - 1,
        AA_SCALE2 = AA_SCALE * 2,
        AA_MASK2 = AA_SCALE2 - 1
    };

    /**
     * Construction of rasterized scanline antialiasing constructor
     * @brief It mainly includes the allocation quota of cell block and cutter
     * Filling rules, automatic closing, starting position, etc.
     * @since 1.0
     * @version 1.0
     */
    RasterizerScanlineAntialias(uint32_t cell_block_limit = (1 << (AA_SHIFT + 2)))
        : outline_(cell_block_limit),
          clipper_(),
          fillingRule_(FILL_NON_ZERO),
          autoClose_(true),
          startX_(0),
          startY_(0),
          status_(STATUS_INITIAL)
    {
        for (int32_t coverIndex = 0; coverIndex < AA_SCALE; coverIndex++) {
            gammar_[coverIndex] = coverIndex;
        }
    }

    /**
     * @brief Reset the cell array for building contour lines,
     * Reset the scan line status value, etc.
     * @since 1.0
     * @version 1.0
     */
    void Reset();

    /**
     * @brief Reset the clipping range and clipping flag of the clipper.
     * @since 1.0
     * @version 1.0
     */
    void ResetClipping();
    void ClipBox(float x1, float y1, float x2, float y2);

    void AutoClose(bool flag)
    {
        autoClose_ = flag;
    }

    /**
     * @brief Set the starting position of the element according to the of 1 / 256 pixel unit.
     * @since 1.0
     * @version 1.0
     */
    void MoveTo(int32_t x, int32_t y);
    /**
     * @brief The moving position of the element is set according to the 1 / 256 pixel unit.
     * @since 1.0
     * @version 1.0
     */
    void LineTo(int32_t x, int32_t y);
    /**
     * @brief Set the starting position of the element according to the of 1 pixel unit.
     * @since 1.0
     * @version 1.0
     */
    void MoveToByfloat(float x, float y);
    /**
     * @brief Set the moving position of the element according to the pixel unit.
     * @since 1.0
     * @version 1.0
     */
    void LineToByfloat(float x, float y);

    /**
     * @brief Closed polygon processing.
     * @since 1.0
     * @version 1.0
     */
    void ClosePolygon();
    void AddVertex(float x, float y, uint32_t cmd);

    /**
     * @brief Obtain the vertex information coordinates from the vertex source and follow the scanning process
     * Sets the procedure for adding an array of cells.
     * @since 1.0
     * @version 1.0
     */
    template <typename VertexSource>
    void AddPath(VertexSource& vs, uint32_t pathId = 0)
    {
        float x;
        float y;

        uint32_t cmd;
        vs.Rewind(pathId);
        if (outline_.GetSorted()) {
            Reset();
        }
        while (!IsStop(cmd = vs.GenerateVertex(&x, &y))) {
            AddVertex(x, y, cmd);
        }
    }

    /**
     * @brief The range boundary value of the contour line.
     * @since 1.0
     * @version 1.0
     */
    int32_t GetMinX() const
    {
        return outline_.GetMinX();
    }
    int32_t GetMinY() const
    {
        return outline_.GetMinY();
    }
    int32_t GetMaxX() const
    {
        return outline_.GetMaxX();
    }
    int32_t GetMaxY() const
    {
        return outline_.GetMaxY();
    }

    /**
     * @brief The cells in the contour line are sorted from left to right and from top to bottom.
     * @since 1.0
     * @version 1.0
     */
    void Sort();
    bool RewindScanlines();

    /**
     * @brief Convert area cover to gamma cover value to calculate alpha.
     * @since 1.0
     * @version 1.0
     */
    uint32_t CalculateAlpha(int32_t area) const;
    /**
     * @brief Scan line that gets a y value from rasterizer stage
     * And iterate the cell array of the current scan line to obtain area->cover，
     * Use both to calculate delta area as area cover and convert it into gamma cover
     * The color information is obtained successfully,
     * and then the alpha information of color is calculated by gamma function
     * Fill in the new scanline and have subsequent render.
     * @since 1.0
     * @version 1.0
     */
    bool SweepScanline(GeometryScanline& sl);

private:
    // Disable copying
    RasterizerScanlineAntialias(const RasterizerScanlineAntialias&);
    const RasterizerScanlineAntialias& operator=(const RasterizerScanlineAntialias&);

    RasterizerCellsAntiAlias outline_;
    RasterizerScanlineClip clipper_;
    int32_t gammar_[AA_SCALE];
    FillingRule fillingRule_;
    bool autoClose_;
    int32_t startX_;
    int32_t startY_;
    uint32_t status_;
    int32_t scanY_;
};
} // namespace OHOS
#endif
