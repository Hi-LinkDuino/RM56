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

#include "gfx_utils/diagram/rasterizer/rasterizer_cells_antialias.h"
#include "gfx_utils/graphic_log.h"
#include "securec.h"

namespace OHOS {
RasterizerCellsAntiAlias::~RasterizerCellsAntiAlias()
{
    if (numBlocks_) {
        CellBuildAntiAlias** ptr = cells_ + numBlocks_ - 1;
        while (numBlocks_--) {
            GeometryArrayAllocator<CellBuildAntiAlias>::Deallocate(*ptr, CELL_BLOCK_SIZE);
            ptr--;
        }
        GeometryArrayAllocator<CellBuildAntiAlias*>::Deallocate(cells_, maxBlocks_);
        GeometryArrayAllocator<CellBuildAntiAlias*>::Deallocate(sortedCells_, numCells_ + CELLS_SIZE);
        GeometryArrayAllocator<SortedYLevel>::Deallocate(sortedY_, maxY_ - minY_ + 1 + CELLS_SIZE);
    }
}

/**
 * @brief RasterizerCellsAntiAlias Class constructor
 * initialization numBlocks_,maxBlocks_,currBlock_ Other attributes
 * @since 1.0
 * @version 1.0
 */
RasterizerCellsAntiAlias::RasterizerCellsAntiAlias(uint32_t cellBlockLimit)
    : numBlocks_(0),
      maxBlocks_(0),
      currBlock_(0),
      numCells_(0),
      cellBlockLimit_(cellBlockLimit),
      cells_(0),
      currCellPtr_(0),
      sortedCells_(nullptr),
      sortedY_(nullptr),
      minX_(INT32_MAX),
      minY_(INT32_MAX),
      maxX_(INT32_MIN),
      maxY_(INT32_MIN),
      sorted_(false)
{
    styleCell_.Initial();
    currCell_.Initial();
}

/**
 * Reinitialize settings numBlocks_,maxBlocks_,currBlock_ and other attributes.
 * @since 1.0
 * @version 1.0
 */
void RasterizerCellsAntiAlias::Reset()
{
    numCells_ = 0;
    currBlock_ = 0;
    currCell_.Initial();
    styleCell_.Initial();
    sorted_ = false;
    minX_ = INT32_MAX;
    minY_ = INT32_MAX;
    maxX_ = INT32_MIN;
    maxY_ = INT32_MIN;
}

/**
 * @brief Add the current cell during rasterization.
 * @since 1.0
 * @version 1.0
 */
void RasterizerCellsAntiAlias::AddCurrentCell()
{
    bool areaCoverFlags = currCell_.area | currCell_.cover;
    if (areaCoverFlags) {
        // Reach CELL_BLOCK_MASK After the number of mask, re allocate memory
        if ((numCells_ & CELL_BLOCK_MASK) == 0) {
            // Exceeds the memory block size limit. The default is 1024 limit
            if (numBlocks_ >= cellBlockLimit_) {
                return;
            }
            AllocateBlock();
        }
        *currCellPtr_++ = currCell_;
        ++numCells_;
    }
}

/**
 * @brief Set the current cell during rasterization.
 * @since 1.0
 * @version 1.0
 */
inline void RasterizerCellsAntiAlias::SetCurrentCell(int32_t x, int32_t y)
{
    if (currCell_.NotEqual(x, y, styleCell_)) {
        AddCurrentCell();
        currCell_.Style(styleCell_);
        currCell_.x = x;
        currCell_.y = y;
        currCell_.cover = 0;
        currCell_.area = 0;
    }
}

void RasterizerCellsAntiAlias::OutLineLegal(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    /**
     * outline range
     */
    if (x1 < minX_) {
        minX_ = x1;
    }
    if (x1 > maxX_) {
        maxX_ = x1;
    }
    if (y1 < minY_) {
        minY_ = y1;
    }
    if (y1 > maxY_) {
        maxY_ = y1;
    }
    if (x2 < minX_) {
        minX_ = x2;
    }
    if (x2 > maxX_) {
        maxX_ = x2;
    }
    if (y2 < minY_) {
        minY_ = y2;
    }
    if (y2 > maxY_) {
        maxY_ = y2;
    }
}

/**
 * @brief In the rasterization process, according to the coordinate height value of ey,
 * x1 to x2 in 1 / 256 pixel horizontally,
 * The filling process of cell cells longitudinally from sub-pixel mask y1 to sub-pixel mask y2.
 * @since 1.0
 * @version 1.0
 */
void RasterizerCellsAntiAlias::RenderHorizonline(
    int32_t ey, int32_t x1, int32_t polySubpixelMaskY1, int32_t x2, int32_t polySubpixelMaskY2)
{
    /**
     * Take out the mask value of the last 8 bits, namely the color mask,
     * from the points in units of 1 / 256 pixels
     */
    int32_t submaskFlagsX1 = x1 & POLY_SUBPIXEL_MASK;
    int32_t submaskFlagsX2 = x2 & POLY_SUBPIXEL_MASK;
    /**
     * The coordinates of the first 24 bits are extracted from the points in units of 1 / 256 pixels
     */
    int32_t pixelX1 = x1 >> POLY_SUBPIXEL_SHIFT;
    int32_t pixelX2 = x2 >> POLY_SUBPIXEL_SHIFT;

    int32_t delta, deltayMask, first;
    int64_t dx;
    int32_t increase, liftDxMask, modDxMask, remDxMask;
    /**
     * The color mask of the two points is the same. Add the settings directly and return.
     */
    if (polySubpixelMaskY2 == polySubpixelMaskY1) {
        SetCurrentCell(pixelX2, ey);
        return;
    }

    // The pixel coordinates of the two points are the same and are directly calculated as a cell.
    if (pixelX1 == pixelX2) {
        delta = polySubpixelMaskY2 - polySubpixelMaskY1;
        currCell_.cover += delta;
        currCell_.area += (submaskFlagsX1 + submaskFlagsX2) * delta;
        return;
    }
    // hline At the beginning of the process, the cells area adjacent to the same organization is rendered
    first = POLY_SUBPIXEL_SCALE;
    increase = 1;
    /**  Convert from submaskFlagsX1 to POLY_SUBPIXEL_SCALE to calculate deltax * deltay */
    deltayMask = (POLY_SUBPIXEL_SCALE - submaskFlagsX1) * (polySubpixelMaskY2 - polySubpixelMaskY1);
    dx = (long long)x2 - (long long)x1;
    if (dx < 0) {
        first = 0;
        increase = -1;
        dx = -dx;
        deltayMask = submaskFlagsX1 * (polySubpixelMaskY2 - polySubpixelMaskY1);
    }
    delta = static_cast<int32_t>(deltayMask / dx);
    modDxMask = static_cast<int32_t>(deltayMask % dx);
    if (modDxMask < 0) {
        modDxMask += dx;
        delta--;
    }
    /* submaskFlagsX1+ (0->first) */
    currCell_.area += (submaskFlagsX1 + first) * delta;
    currCell_.cover += delta;
    pixelX1 += increase;
    SetCurrentCell(pixelX1, ey);
    polySubpixelMaskY1 += delta;
    if (pixelX1 != pixelX2) {
        /* delta_subpixel x（ 0 to POLY_SUBPIXEL_SCALE）  to ( delta_subpixel_scale_y + delta) */
        deltayMask = POLY_SUBPIXEL_SCALE * (polySubpixelMaskY2 - polySubpixelMaskY1 + delta);
        remDxMask = static_cast<int32_t>(deltayMask % dx);
        liftDxMask = static_cast<int32_t>(deltayMask / dx);
        if (remDxMask < 0) {
            liftDxMask--;
            remDxMask += dx;
        }
        modDxMask -= dx;
        while (pixelX1 != pixelX2) {
            delta = liftDxMask;
            modDxMask += remDxMask;
            if (modDxMask >= 0) {
                modDxMask -= dx;
                delta++;
            }
            currCell_.area += POLY_SUBPIXEL_SCALE * delta;
            currCell_.cover += delta;
            polySubpixelMaskY1 += delta;
            pixelX1 += increase;
            SetCurrentCell(pixelX1, ey);
        }
    }
    delta = polySubpixelMaskY2 - polySubpixelMaskY1;
    currCell_.cover += delta;
    /* From first to POLY_SUBPIXEL_SCALE procedure */
    currCell_.area += (submaskFlagsX2 + POLY_SUBPIXEL_SCALE - first) * delta;
}

inline void RasterizerCellsAntiAlias::SetStyle(const CellBuildAntiAlias& styleCell)
{
    styleCell_.Style(styleCell);
}

/**
 * @brief According to the incoming 2 coordinate points (both with sub pixels),
 * The process of constructing rasterized cell points is from y to X.
 * @since 1.0
 * @version 1.0
 */
void RasterizerCellsAntiAlias::LineOperate(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    int64_t dx = static_cast<int64_t>(x2) - static_cast<int64_t>(x1);
    /**
     * If dx exceeds the limit, a compromise is adopted to calculate the line.
     */
    if (dx >= DX_LIMIT || dx <= -DX_LIMIT) {
        int32_t cx = static_cast<int32_t>(((int64_t)x1 + (int64_t)x2) >> 1);
        int32_t cy = static_cast<int32_t>(((int64_t)y1 + (int64_t)y2) >> 1);
        LineOperate(x1, y1, cx, cy);
        LineOperate(cx, cy, x2, y2);
        return;
    }
    /**
     * The coordinates of the first 24 bits are extracted from the points in units of 1 / 256 pixels
     */
    int64_t dy = (int64_t)y2 - (int64_t)y1;
    int32_t ex1 = x1 >> POLY_SUBPIXEL_SHIFT;
    int32_t ex2 = x2 >> POLY_SUBPIXEL_SHIFT;
    int32_t ey1 = y1 >> POLY_SUBPIXEL_SHIFT;
    int32_t ey2 = y2 >> POLY_SUBPIXEL_SHIFT;
    /**
     * Take out the mask value of the last 8 bits from
     * the points with 1 / 256 pixel as the unit, that is, the color mask
     */
    int32_t submaskFlagsY1 = y1 & POLY_SUBPIXEL_MASK;
    int32_t submaskFlagsY2 = y2 & POLY_SUBPIXEL_MASK;

    int32_t xFrom;
    int32_t modDyMask, delta, first, increase;
    int64_t deltaxMask;

    OutLineLegal(ex1, ey1, ex2, ey2);
    SetCurrentCell(ex1, ey1);

    /**
     * If the Y values of the two points are the same, they will be directly rendered horizontally,
     * The horizontal coordinate spacing is from X1 - > x2 in 1 / 256 pixels,
     * Color mask spacing is from submaskFlagsY1 to submaskFlagsY2
     */
    if (ey1 == ey2) {
        RenderHorizonline(ey1, x1, submaskFlagsY1, x2, submaskFlagsY2);
        return;
    }
    /**
     * For the processing of vertical lines, start - > end cells are calculated, and then the line is calculated
     * The above general attribute area - > cover is for each y value,
     * There is only one cell, so it is no longer called RenderHorizonline()
     */
    increase = 1;
    if (dx == 0) {
        RenderVerticalLine(x1, ex1, dy, first, increase, xFrom, submaskFlagsY1, submaskFlagsY2, ey1, ey2, delta);
        return;
    }
    /**
     * The color mask is from the color mask to submaskFlagsY1 to POLY_SUBPIXEL_SCALE Process of scale
     */
    deltaxMask = (POLY_SUBPIXEL_SCALE - submaskFlagsY1) * dx;
    first = POLY_SUBPIXEL_SCALE;
    if (dy < 0) {
        deltaxMask = submaskFlagsY1 * dx;
        first = 0;
        increase = -1;
        dy = -dy;
    }
    delta = static_cast<int32_t>(deltaxMask / dy);
    modDyMask = static_cast<int32_t>(deltaxMask % dy);
    if (modDyMask < 0) {
        delta--;
        modDyMask += dy;
    }
    xFrom = x1 + delta;
    RenderHorizonline(ey1, x1, submaskFlagsY1, xFrom, first);
    ey1 += increase;
    SetCurrentCell(xFrom >> POLY_SUBPIXEL_SHIFT, ey1);
    if (ey1 != ey2) {
        RenderObliqueLine(dx, dy, first, increase, xFrom, deltaxMask, ey1, ey2, delta);
    }
    RenderHorizonline(ey1, xFrom, POLY_SUBPIXEL_SCALE - first, x2, submaskFlagsY2);
}

void RasterizerCellsAntiAlias::RenderVerticalLine(int32_t& x1, int32_t& ex1, int64_t& dy, int32_t& first, int32_t& increase, int32_t& xFrom,
                                                  int32_t& submaskFlagsY1, int32_t& submaskFlagsY2, int32_t& ey1, int32_t& ey2, int32_t& delta)
{
    /**
     * The coordinates of the first 24 bits are extracted from the points in units of 1 / 256 pixels
     */
    /* Take out the number of decimal points and occupy 2 spaces */
    int32_t twoFx = (x1 - (ex1 << POLY_SUBPIXEL_SHIFT)) << 1;
    int32_t area;
    /* 256 */
    first = POLY_SUBPIXEL_SCALE;
    if (dy < 0) {
        first = 0;
        increase = -1;
    }
    xFrom = x1;
    /* From submaskFlagsY1 to first process */
    /* The color mask is from submaskFlagsY1->first */
    delta = first - submaskFlagsY1;
    currCell_.cover += delta;
    currCell_.area += twoFx * delta;
    ey1 += increase;
    SetCurrentCell(ex1, ey1);
    /* The color mask is from (poly_subpixel_scale - first) -> first */
    delta = first + first - POLY_SUBPIXEL_SCALE;
    area = twoFx * delta;
    while (ey1 != ey2) {
        /* from poly_subpixel_scale - first to  first */
        currCell_.cover = delta;
        currCell_.area = area;
        ey1 += increase;
        SetCurrentCell(ex1, ey1);
    }
    /* The color mask is from poly_subpixel_scale - first to  submaskFlagsY2 */
    delta = submaskFlagsY2 - POLY_SUBPIXEL_SCALE + first;
    currCell_.cover += delta;
    currCell_.area += twoFx * delta;
}

void RasterizerCellsAntiAlias::RenderObliqueLine(int64_t& dx, int64_t& dy, int32_t& first, int32_t& increase, int32_t& xFrom,
                                                 int64_t& deltaxMask, int32_t& ey1, int32_t& ey2, int32_t& delta)
{
    int32_t xTo;
    int32_t remDyMask, liftDyMask;
    deltaxMask = POLY_SUBPIXEL_SCALE * dx;
    liftDyMask = static_cast<int32_t>(deltaxMask / dy);
    remDyMask = static_cast<int32_t>(deltaxMask % dy);
    if (remDyMask < 0) {
        liftDyMask--;
        remDyMask += dy;
    }
    int32_t modDyMask = -dy;
    while (ey1 != ey2) {
        delta = liftDyMask;
        modDyMask += remDyMask;
        if (modDyMask >= 0) {
            modDyMask -= dy;
            delta++;
        }
        xTo = xFrom + delta;
        RenderHorizonline(ey1, xFrom, POLY_SUBPIXEL_SCALE - first, xTo, first);
        xFrom = xTo;
        ey1 += increase;
        SetCurrentCell(xFrom >> POLY_SUBPIXEL_SHIFT, ey1);
    }
}

/**
 * @brief Allocate array space for cells during rasterization.
 * @since 1.0
 * @version 1.0
 */
void RasterizerCellsAntiAlias::AllocateBlock()
{
    if (currBlock_ >= numBlocks_) {
        if (numBlocks_ >= maxBlocks_) {
            CellBuildAntiAlias** newCells =
                GeometryArrayAllocator<CellBuildAntiAlias*>::Allocate(maxBlocks_ +
                                                            CELL_BLOCK_POOL);
            if (cells_) {
                if (memcpy_s(newCells, maxBlocks_ * sizeof(CellBuildAntiAlias*),
                             cells_, maxBlocks_ * sizeof(CellBuildAntiAlias*)) != EOK) {
                    GRAPHIC_LOGE("RasterizerCellsAntiAlias::AllocateBlock memcpy_s fail\n");
                    return;
                }
                GeometryArrayAllocator<CellBuildAntiAlias*>::Deallocate(cells_, maxBlocks_);
            }
            cells_ = newCells;
            maxBlocks_ += CELL_BLOCK_POOL;
        }
        cells_[numBlocks_++] = GeometryArrayAllocator<CellBuildAntiAlias>::Allocate(CELL_BLOCK_SIZE);
    }

    currCellPtr_ = cells_[currBlock_++];
}
/**
 * @brief In the rasterization process, all cells are rasterized according to
 * Sort from left to right and from top to bottom.
 * @since 1.0
 * @version 1.0
 */
void RasterizerCellsAntiAlias::SortAllCells()
{
    if (sorted_) {
        return; // Perform sort only the first time.
    }

    AddCurrentCell();
    currCell_.x = INT32_MAX;
    currCell_.y = INT32_MAX;
    currCell_.cover = 0;
    currCell_.area = 0;
    if (numCells_ == 0) {
        return;
    }

    // Allocate the array of cell pointers
    sortedCells_ = GeometryArrayAllocator<CellBuildAntiAlias*>::Allocate(numCells_ + CELLS_SIZE);

    // Allocate and zero the Y array
    uint32_t sortedYSize = maxY_ - minY_ + 1;
    sortedY_ = GeometryArrayAllocator<SortedYLevel>::Allocate(sortedYSize + CELLS_SIZE);
    if (memset_s(sortedY_, sizeof(SortedYLevel) * sortedYSize, 0, sizeof(SortedYLevel) * sortedYSize) != EOK) {
        GRAPHIC_LOGE("CleanData fail");
    }

    // Create the Y-histogram (count the numbers of cells for each Y)
    CellBuildAntiAlias** blockPtr = cells_;
    CellBuildAntiAlias* cellPtr = nullptr;
    uint32_t nb = numCells_;
    uint32_t i = 0;
    while (nb) {
        cellPtr = *blockPtr++;
        i = (nb > CELL_BLOCK_SIZE) ? uint32_t(CELL_BLOCK_SIZE) : nb;
        nb -= i;
        while (i--) {
            sortedY_[cellPtr->y - minY_].start++;
            ++cellPtr;
        }
    }

    // Convert the Y-histogram into the array of starting indexes
    uint32_t start = 0;
    for (i = 0; i < sortedYSize; i++) {
        uint32_t v = sortedY_[i].start;
        sortedY_[i].start = start;
        start += v;
    }

    // Fill the cell pointer array sorted by Y
    blockPtr = cells_;
    nb = numCells_;
    while (nb) {
        cellPtr = *blockPtr++;
        i = (nb > CELL_BLOCK_SIZE) ? uint32_t(CELL_BLOCK_SIZE) : nb;
        nb -= i;
        while (i--) {
            SortedYLevel& currY = sortedY_[cellPtr->y - minY_];
            sortedCells_[currY.start + currY.num] = cellPtr;
            ++currY.num;
            ++cellPtr;
        }
    }

    // Finally arrange the X-arrays
    for (i = 0; i < sortedYSize; i++) {
        const SortedYLevel& currY = sortedY_[i];
        if (currY.num) {
            QsortCells(sortedCells_ + currY.start, currY.num);
        }
    }
    sorted_ = true;
}

void QsortCellsSweep(CellBuildAntiAlias*** base, CellBuildAntiAlias*** iIndex, CellBuildAntiAlias*** jIndex)
{
    /**
     * Sorting guarantees the value of * i < = * the value of base < = * the value of j
     */
    if ((**jIndex)->x < (**iIndex)->x) {
        SwapCells(*iIndex, *jIndex);
    }

    if ((**base)->x < (**iIndex)->x) {
        SwapCells(*base, *iIndex);
    }

    if ((**jIndex)->x < (**base)->x) {
        SwapCells(*base, *jIndex);
    }

    while (true) {
        int32_t x = (**base)->x;
        do {
            (*iIndex)++;
        } while ((**iIndex)->x < x);
        do {
            (*jIndex)--;
        } while (x < (**jIndex)->x);

        if ((*iIndex) > (*jIndex)) {
            break;
        }
        SwapCells(*iIndex, *jIndex);
    }
}

void QsortCells(CellBuildAntiAlias** start, uint32_t num)
{
    const int32_t QSORT_THRESHOLD = 9;
    const int32_t stackSize = 80;
    CellBuildAntiAlias** stack[stackSize];
    CellBuildAntiAlias*** top;
    CellBuildAntiAlias** limit;
    CellBuildAntiAlias** base;

    limit = start + num;
    base = start;
    top = stack;

    while (true) {
        int32_t len = int32_t(limit - base);

        CellBuildAntiAlias** iIndex;
        CellBuildAntiAlias** jIndex;
        CellBuildAntiAlias** pivot;

        if (len > QSORT_THRESHOLD) {
            /**
             * First exchange base + len / 2 as the pivot
             */
            pivot = base + len / TWO_TIMES;
            SwapCells(base, pivot);

            iIndex = base + 1;
            jIndex = limit - 1;

            QsortCellsSweep(&base, &iIndex, &jIndex);
            SwapCells(base, jIndex);
            if (jIndex - base > limit - iIndex) {
                top[0] = base;
                top[1] = jIndex;
                base = iIndex;
            } else {
                top[0] = iIndex;
                top[1] = limit;
                limit = jIndex;
            }
            top += TWO_STEP;
        } else {
            /**
             * When the sub-array becomes smaller, insert sort is performed using
             */
            jIndex = base;
            iIndex = jIndex + 1;
            QsortCellsFor(&iIndex, &jIndex, &limit, &base);
            if (top > stack) {
                top -= TWO_STEP;
                base = top[0];
                limit = top[1];
            } else {
                break;
            }
        }
    }
}

void QsortCellsFor(CellBuildAntiAlias*** iIndex, CellBuildAntiAlias*** jIndex,
                   CellBuildAntiAlias*** limit, CellBuildAntiAlias*** base)
{
    for (; *iIndex < *limit; (*iIndex)++) {
        for (; (*jIndex)[1]->x < (**jIndex)->x; (*jIndex)--) {
            SwapCells((*jIndex) + 1, *jIndex);
            if ((*jIndex) == (*base)) {
                break;
            }
        }
        *jIndex = *iIndex;
    }
}
} // namespace OHOS
