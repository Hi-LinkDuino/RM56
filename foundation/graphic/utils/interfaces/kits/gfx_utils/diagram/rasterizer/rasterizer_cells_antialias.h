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
 * @file rasterizer_cells_antialias.h
 * @brief Defines Grating cell (anti aliasing)
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_RASTERIZER_CELLS_ANTIALIAS_H
#define GRAPHIC_LITE_RASTERIZER_CELLS_ANTIALIAS_H

#include "gfx_utils/diagram/common/common_math.h"
#include "gfx_utils/vector.h"

namespace OHOS {
// There is no constructor defined for pixel cells,
// which is to avoid the additional overhead of allocating cell arrays
struct CellBuildAntiAlias {
    int32_t x;
    int32_t y;
    int32_t cover;
    int32_t area;

    void Initial()
    {
        x = INT32_MAX;
        y = INT32_MAX;
        cover = 0;
        area = 0;
    }

    void Style(const CellBuildAntiAlias&) {}

    int32_t NotEqual(int32_t ex, int32_t ey, const CellBuildAntiAlias&) const
    {
        return (static_cast<uint32_t>(ex) - static_cast<uint32_t>(x))
                | (static_cast<uint32_t>(ey) - static_cast<uint32_t>(y));
    }
};

class RasterizerCellsAntiAlias {
    struct SortedYLevel {
        uint32_t start;
        uint32_t num;
    };

    /**
     * @brief Build the offset of 'cell unit', mask mask, cell pool capacity, etc
     * @since 1.0
     * @version 1.0
     */
    enum CellBlockScale {
        CELL_BLOCK_SHIFT = 12,
        CELL_BLOCK_SIZE = 1 << CELL_BLOCK_SHIFT,
        CELL_BLOCK_MASK = CELL_BLOCK_SIZE - 1,
        CELL_BLOCK_POOL = 256
    };

    enum DxLimit {
        DX_LIMIT = CONSTITUTION << POLY_SUBPIXEL_SHIFT
    };

public:
    ~RasterizerCellsAntiAlias();

    /**
     * @brief RasterizerCellsAntiAlias Class constructor
     * initialization numBlocks_,maxBlocks_,currBlock_ etc
     * @since 1.0
     * @version 1.0
     */
    RasterizerCellsAntiAlias(uint32_t cellBlockLimit = 1024);

    /**
     * Reinitialize settings numBlocks_,maxBlocks_,currBlock_ etc。
     * @since 1.0
     * @version 1.0
     */
    void Reset();
    void SetStyle(const CellBuildAntiAlias& styleCell);

    /**
     * @brief According to the incoming 2 coordinate points (both with sub pixels),
     * The process of constructing rasterized cell points is from y to X.
     * @since 1.0
     * @version 1.0
     */
    void LineOperate(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

    void LineOperateVerticalLine();

    /**
     * @brief The rasterization process builds the coordinate range of the entity.
     * @since 1.0
     * @version 1.0
     */
    int32_t GetMinX() const
    {
        return minX_;
    }
    int32_t GetMinY() const
    {
        return minY_;
    }
    int32_t GetMaxX() const
    {
        return maxX_;
    }
    int32_t GetMaxY() const
    {
        return maxY_;
    }

    /**
     * @brief In the rasterization process, all cells are rasterized according to
     * Sort from left to right and from top to bottom.
     * @since 1.0
     * @version 1.0
     */
    void SortAllCells();

    uint32_t GetTotalCells() const
    {
        return numCells_;
    }

    /**
     * @brief In the process of rasterization, it is calculated according to the coordinate height of Y
     * Total number of cells.
     * @since 1.0
     * @version 1.0
     */
    uint32_t GetScanlineNumCells(uint32_t yLevel)
    {
        return sortedY_[yLevel - minY_].num;
    }

    /**
     * @brief In the process of rasterization, it is calculated according to the coordinate height of Y
     * The first address of the sorted cells array.
     * @since 1.0
     * @version 1.0
     */
    const CellBuildAntiAlias * const *GetScanlineCells(uint32_t yLevel)
    {
        return sortedCells_ + sortedY_[yLevel - minY_].start;
    }

    bool GetSorted() const
    {
        return sorted_;
    }

private:
    RasterizerCellsAntiAlias(const CellBuildAntiAlias&);
    const CellBuildAntiAlias& operator=(const CellBuildAntiAlias&);

    /**
     * @brief Set the current cell during rasterization.
     * @since 1.0
     * @version 1.0
     */
    void SetCurrentCell(int32_t x, int32_t y);

    void OutLineLegal(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

    /**
     * @brief Add the current cell during rasterization.
     * @since 1.0
     * @version 1.0
     */
    void AddCurrentCell();

    /**
     * @brief n the rasterization process, the horizontal direction is
     * from x1 to x2 according to the coordinate height value of ey,
     * The filling process of cell cells longitudinally from sub-pixel mask y1 to sub-pixel mask y2.
     * @since 1.0
     * @version 1.0
     */
    void RenderHorizonline(int32_t ey, int32_t x1, int32_t submaskFlagsY1, int32_t x2, int32_t submaskFlagsY2);

    void RenderVerticalLine(int32_t& x1, int32_t& ex1, int64_t& dy, int32_t& first, int32_t& increase, int32_t& xFrom,
                            int32_t& submaskFlagsY1, int32_t& submaskFlagsY2, int32_t& ey1, int32_t& ey2, int32_t& delta);

    void RenderObliqueLine(int64_t& dx, int64_t& dy, int32_t& first, int32_t& increase, int32_t& xFrom,
                           int64_t& deltaxMask, int32_t& ey1, int32_t& ey2, int32_t& delta);
    /**
     * @brief Allocate array space for cells during rasterization.
     * @since 1.0
     * @version 1.0
     */
    void AllocateBlock();

private:
    uint32_t numBlocks_;
    uint32_t maxBlocks_;
    uint32_t currBlock_;
    uint32_t numCells_;
    uint32_t cellBlockLimit_;
    CellBuildAntiAlias** cells_;
    CellBuildAntiAlias* currCellPtr_;
    CellBuildAntiAlias** sortedCells_;
    SortedYLevel* sortedY_;
    CellBuildAntiAlias currCell_;
    CellBuildAntiAlias styleCell_;
    int32_t minX_;
    int32_t minY_;
    int32_t maxX_;
    int32_t maxY_;
    bool sorted_;
};

class ScanlineHitRegionMeasure {
public:
    ScanlineHitRegionMeasure(int32_t x) : xCoordinate_(x), hitMeasureFlags_(false) {}

    void AddCellInContainer(int32_t x, int32_t)
    {
        if (xCoordinate_ == x) {
            hitMeasureFlags_ = true;
        }
    }
    void AddSpanInContainer(int32_t x, int32_t len, int32_t)
    {
        if (xCoordinate_ >= x && xCoordinate_ < x + len) {
            hitMeasureFlags_ = true;
        }
    }
    uint32_t GetNumberSpans() const
    {
        return 1;
    }
    bool GetHitMeasure() const
    {
        return hitMeasureFlags_;
    }

private:
    int32_t xCoordinate_;
    bool hitMeasureFlags_;
};

/**
 * @brief The exchange of cells in the rasterization process.
 * @since 1.0
 * @version 1.0
 */
template <class T>
inline void SwapCells(T* oneCells, T* twoCells)
{
    T tempCells = *oneCells;
    *oneCells = *twoCells;
    *twoCells = tempCells;
}

void QsortCellsSweep(CellBuildAntiAlias*** base, CellBuildAntiAlias*** iIndex, CellBuildAntiAlias*** jIndex);

/**
 * @brief In the rasterization process, all cells are sorted quickly.
 * @since 1.0
 * @version 1.0
 */
void QsortCells(CellBuildAntiAlias** start, uint32_t num);

void QsortCellsFor(CellBuildAntiAlias*** iIndex, CellBuildAntiAlias*** jIndex,
                   CellBuildAntiAlias*** limit, CellBuildAntiAlias*** base);
} // namespace OHOS
#endif
