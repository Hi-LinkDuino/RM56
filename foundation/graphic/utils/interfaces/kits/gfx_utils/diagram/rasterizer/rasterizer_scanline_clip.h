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
 * @file rasterizer_scanline_clip.h
 * @brief Defines Light fence clipping
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_RASTERIZER_SCANLINE_CLIP_H
#define GRAPHIC_LITE_RASTERIZER_SCANLINE_CLIP_H

#include "gfx_utils/diagram/common/common_clip_operate.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/diagram/rasterizer/rasterizer_cells_antialias.h"
namespace OHOS {
/**
 * The PolyMaxCoord enumeration type
 * Defines the value of the maximum coordinate of the polygon
 * @since 1.0
 * @version 1.0
 */
enum PolyMaxCoord {
    POLY_MAX_COORD = (1 << 30) - 1
};

/**
 * @struct RasterDepictInt
 * @brief The RasterDepictInt structure is within the target range
 * When clipping coordinates, for 3 values similar to coordinate spacing, or
 * Upper sampling and lower sampling are processed.
 * @since 1.0
 * @version 1.0
 */
struct RasterDepictInt {
    /**
     * @brief This function is mainly for the input coordinate values
     * Do sampling processing.
     * @since 1.0
     * @version 1.0
     */
    static int32_t UpScale(float vUpscale)
    {
        return MATH_ROUND32(vUpscale * POLY_SUBPIXEL_SCALE);
    }

    /**
     * @brief This function is mainly for the input coordinate values
     * Do the next sampling.
     * @since 1.0
     * @version 1.0
     */
    static int32_t DownScale(int32_t vDownscale)
    {
        return vDownscale;
    }

    static inline int32_t MultDiv(float deltaA, float deltaB, float dealtaC)
    {
        if (dealtaC != 0) {
            return MATH_ROUND32(deltaA * deltaB / dealtaC);
        }
        return 0;
    }

    static int32_t GetXCoordinateValue(int32_t xValue)
    {
        return xValue;
    }
    static int32_t GetYCoordinateValue(int32_t yValue)
    {
        return yValue;
    }
};

/**
 * @class RasterizerScanlineClip
 * @brief Defines In the rasterization stage, when exchanging scan line processing, for
 * Coordinate cutting and processing
 * @since 1.0
 * @version 1.0
 */
class RasterizerScanlineClip {
public:
    /**
     * @brief RasterizerScanlineClip Class constructor
     * Initialize clipping range, clipping flag, etc.
     * @since 1.0
     * @version 1.0
     */
    RasterizerScanlineClip()
        : clipBox_(0, 0, 0, 0),
          x1_(0),
          y1_(0),
          clippingFlags_(0),
          clipping_(false) {}

    void ResetClipping()
    {
        clipping_ = false;
    }

    /**
     * @brief Sets the clipping range.
     * @since 1.0
     * @version 1.0
     */
    void ClipBox(int32_t left, int32_t top, int32_t right, int32_t bottom)
    {
        clipBox_ = Rect32(left, top, right, bottom);
        clipBox_.Normalize();
        clipping_ = true;
    }

    /**
     * @brief In the RASTERIZER process, the starting point of setting is added,
     * And set the flag of clippingFlags_
     * @since 1.0
     * @version 1.0
     */
    void MoveTo(int32_t x1, int32_t y1)
    {
        x1_ = x1;
        y1_ = y1;
        if (clipping_) {
            clippingFlags_ = ClippingFlags(x1, y1, clipBox_);
        }
    }
    /**
     * @brief In the RASTERIZER process, add the set sampling point
     * And set the sampling point, set the related cover and area attributes, etc
     *         |        |
     *   0110  |  0010  | 0011
     *         |        |
     *  -------+--------+-------- clip_box.y2
     *         |        |
     *   0100  |  0000  | 0001
     *         |        |
     *  -------+--------+-------- clip_box.y1
     *         |        |
     *   1100  |  1000  | 1001
     *         |        |
     *   clip_box.x1  clip_box.x2
     * @since 1.0
     * @version 1.0
     */
    void LineTo(RasterizerCellsAntiAlias& ras, int32_t x2, int32_t y2);

private:
    /**
     * @brief In the RASTERIZER process,Judge the mark according to the last clipping range
     * And the cutting range judgment flag this time,
     * add the actual sampling points and set relevant attributes
     * @since 1.0
     * @version 1.0
     */
    inline void LineClipY(RasterizerCellsAntiAlias& ras,
                          int32_t x1, int32_t y1,
                          int32_t x2, int32_t y2,
                          uint32_t clipFlagsOne, uint32_t clipFlagsTwo) const;
    Rect32 clipBox_;
    int32_t x1_;
    int32_t y1_;
    uint32_t clippingFlags_;
    bool clipping_;
};
} // namespace OHOS
#endif
