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
 * @addtogroup GraphicGeometry
 * @{
 *
 * @brief Defines Graphic operation
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file common_basics.h
 *
 * @brief Defines Basic data types and general functions of graphic operations
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_COMMON_BASICS_H
#define GRAPHIC_LITE_COMMON_BASICS_H

#include "gfx_utils/graphic_math.h"
#include "graphic_config.h"
namespace OHOS {
enum VertexGenerateFlags {
    GENERATE_DASH,
    GENERATE_STROKE
};

/**
 * @brief Fill rule
 * @since 1.0
 * @version 1.0
 */
enum FillingRule {
    FILL_NON_ZERO,
    FILL_EVEN_ODD
};


/**
 * repeat Spread the whole canvas
 * repeat-x Repeat on the X axis of the canvas
 * repeat-y Repeat on the Y axis of the canvas
 * no-repeat Do not repeat on canvas
 */
enum PatternRepeatMode {
    REPEAT,
    REPEAT_X,
    REPEAT_Y,
    NO_REPEAT,
};

/**
 * @brief Sub - pixel offset and mask flag
 * @since 1.0
 * @version 1.0
 */
enum PolySubpixelScale {
    POLY_SUBPIXEL_SHIFT = 8,
    POLY_SUBPIXEL_SCALE = 1 << POLY_SUBPIXEL_SHIFT,
    POLY_SUBPIXEL_MASK = POLY_SUBPIXEL_SCALE - 1
};

enum GradientSubpixelScale {
    GRADIENT_SUBPIXEL_SHIFT = 4,
    GRADIENT_SUBPIXEL_SCALE = 1 << GRADIENT_SUBPIXEL_SHIFT,
    GRADIENT_SUBPIXEL_MASK = GRADIENT_SUBPIXEL_SCALE - 1
};
/**
 * @brief Elastic treatment of coverage
 * @since 1.0
 * @version 1.0
 */
enum CoverScale {
    COVER_SHIFT = 8,
    COVER_SIZE = 1 << COVER_SHIFT,
    COVER_MASK = COVER_SIZE - 1,
    COVER_NONE = 0,
    COVER_FULL = COVER_MASK
};

enum PathCommands {
    PATH_CMD_STOP = 0,
    PATH_CMD_MOVE_TO = 1,
    PATH_CMD_LINE_TO = 2,
    PATH_CMD_CURVE3 = 3,      // Conic command
    PATH_CMD_CURVE4 = 4,      // Cubic curve command
    PATH_CMD_CURVEN = 5,      // Curve command
    PATH_CMD_CARROM = 6,      // Conic command
    PATH_CMD_UBSPLINE = 7,    // Curve command
    PATH_CMD_END_POLY = 0x0F, // Polygon closure
    PATH_CMD_MASK = 0x0F
};

enum PathFlags {
    PATH_FLAGS_NONE = 0,
    PATH_FLAGS_CLOCKWISE = 0x10,      // Clockwise
    PATH_FLAGS_ANTI_CLOCKWISE = 0x20, // anti-clockwise
    PATH_FLAGS_CLOSE = 0x40,
    PATH_FLAGS_MASK = 0xF0
};

/**
 * @brief Four cases of collinearity
 * @since 1.0
 * @version 1.0
 */
enum Recursive {
    COLLINEAR = 0,
    COLLINEAR1,
    COLLINEAR2,
    COLLINEAR3
};
enum ClippingFlags {
    CLIPPING_FLAGS_X1_CLIPPED = 4,
    CLIPPING_FLAGS_X2_CLIPPED = 1,
    CLIPPING_FLAGS_Y1_CLIPPED = 8,
    CLIPPING_FLAGS_Y2_CLIPPED = 2,
    CLIPPING_FLAGS_X_CLIPPED = CLIPPING_FLAGS_X1_CLIPPED | CLIPPING_FLAGS_X2_CLIPPED,
    CLIPPING_FLAGS_Y_CLIPPED = CLIPPING_FLAGS_Y1_CLIPPED | CLIPPING_FLAGS_Y2_CLIPPED
};

enum GlobalCompositeOperation {
    /**
     * Default. Displays the source image on the target image.
     */
    SOURCE_OVER,
    /**
     * Displays the source image at the top of the target image.
     * The part of the source image outside the target image is invisible.
     */
    SOURCE_ATOP,
    /**
     * Displays the source image in the target image.
     * Only the source image part within the target image will be displayed, and the target image is transparent.
     */
    SOURCE_IN,
    /**
     * Displays the source image outside the target image.
     * Only the source image part other than the target image will be displayed,and the target image is transparent
     */
    SOURCE_OUT,
    /**
     * Displays the target image on the source image.
     */
    DESTINATION_OVER,
    /**
     * Displays the target image at the top of the source image.
     * The part of the target image outside the source image is invisible.
     */
    DESTINATION_ATOP,
    /**
     * Displays the target image in the source image.
     * Only the target image part within the source image will be displayed, and the source image is transparent.
     */
    DESTINATION_IN,
    /**
     * Displays the target image outside the source image.
     * Only the part of the target image other than the source image will be displayed,
     * and the source image is transparent.
     */
    DESTINATION_OUT,
    /**
     * Display source image + target image.
     */
    LIGHTER,
    /**
     * Displays the source image. Ignore the target image.
     */
    COPY,
    /**
     * Use the XOR operation to combine the source image with the target image.
     */
    XOR
};

const float COEFFICIENT = 0.7f;
const float ALPHA_HALF = 0.5f;
const float VERTEX_DIST_EPSILON = 1e-14;
const float INTERSECTIONEPSILON = 1.0e-30;
/**
 * @brief PI
 */
const float PI = 3.1415926f;
/**
 * @brief Semicircle arc angle
 */
const float RAD_BASE = 180.0f;
const float ROUND_BASE = 0.5f;

/**
 * @brief Degree of flat angle
 */
const float BOXER = 180.0f;

/**
 * @brief BUF_SIZE
 */
const float BUF_SIZE = 1024.0f;
/**
 * @brief Maximum color
 */
const uint32_t MAX_COLOR_NUM = 255;
/**
 * @brief Maximum array length of colors
 */
const uint32_t MAX_COLOR_SIZE = 256;

/**
 * @brief Half the maximum value of the color
 */
const uint32_t HALF_COLOR_NUM = 128;
/**
 * @brief One eighth of the maximum value of the color
 */
const uint32_t EIGHTH_COLOR_NUM = 32;
/**
 * @brief One thirty second of the maximum value of the color
 */
const uint32_t THIRTY_TWO_COLOR_NUM = 8;

/**
 * @brief Default maximum miter
 */
const uint32_t DEFAULTMITERLIMIT = 10;
/**
 * @brief Length of one byte
 */
const int32_t BYTE_LENGTH = 8;
const int32_t CELLS_SIZE = 16;
/**
 * @brief Maximum length of dash array
 */
const int32_t MAX_DASHES = 32;
/**
 * @brief Default block offset
 */
const int32_t BLOCK_SHIFT_SIZE = 6;
/**
 * @brief The number of vertices required to draw the ellipse
 */
const int32_t ELLIPSE_VERTEX_NUM = 4;
/**
 * The general stride is four times the width
 */
const int32_t STRIDE_FACTOR = 4;
const int32_t PXSIZE2STRIDE_FACTOR = 3;
/**
 * @brief 1 / √ 2 is 1 divided by √ 2
 */
const float SIN45 = 0.7071068f;
/**
 * @brief 1 / √ 2 is 1 divided by √ 2
 */
const float COS45 = 0.7071068f;
/**
 * @brief twice as much
 */
const int32_t TWO_TIMES = 2;
/**
 * @brief triple
 */
const int32_t THREE_TIMES = 3;
/**
 * @brief fourfold
 */
const int32_t FOUR_TIMES = 4;
/**
 * @brief Sixfold
 */
const int32_t SIX_TIMES = 6;
/**
 * @brief hundredfold
 */
const int32_t HUNDRED_TIMES = 100;
/**
 * @brief Take two steps at a time
 */
const int32_t TWO_STEP = 2;
const float HALFNUM = 0.5f;
const float FLOATNUM = 2.0f;
const float RADDALETAELPS = 0.125f;

const int32_t CONSTITUTION = 16384;
/**
 * @brief Directly below
 */
const int32_t DIRECTLY_BELOW = 10;

/**
 * @brief Determines whether the value is a vertex
 * @since 1.0
 * @version 1.0
 */
inline bool IsVertex(uint32_t val)
{
    return val < PATH_CMD_END_POLY && val >= PATH_CMD_MOVE_TO;
}

/**
 * @brief Judge whether the current status is MOVE_TO.
 * @since 1.0
 * @version 1.0
 */
inline bool IsMoveTo(uint32_t val)
{
    return PATH_CMD_MOVE_TO == val;
}

/**
 * @brief Judge whether the current status is stop status
 * @since 1.0
 * @version 1.0
 */
inline bool IsStop(uint32_t val)
{
    return PATH_CMD_STOP == val;
}

/**
 * @brief Judge whether the current state is the state of drawing the end of the curve
 * @since 1.0
 * @version 1.0
 */
inline bool IsEndPoly(uint32_t val)
{
    return PATH_CMD_END_POLY == (val & PATH_CMD_MASK);
}

/**
 * @brief Judge whether the current state is the closed state of drawing curve
 * @since 1.0
 * @version 1.0
 */
inline bool IsClose(uint32_t val)
{
    return (val & ~(PATH_FLAGS_ANTI_CLOCKWISE | PATH_FLAGS_CLOCKWISE)) == (PATH_CMD_END_POLY | PATH_FLAGS_CLOSE);
}

/**
 * @brief Get closed state
 * @since 1.0
 * @version 1.0
 */
inline uint32_t GetCloseFlag(uint32_t val)
{
    return val & PATH_FLAGS_CLOSE;
}

template <class T>
struct GeometryArrayAllocator {
    /**
     * @brief Array memory allocation
     * @since 1.0
     * @version 1.0
     */
    static T* Allocate(uint32_t num)
    {
        if (num < 1) {
            num = 1;
        }
        return new T[num];
    }
    /**
     * @brief Array memory free
     * @since 1.0
     * @version 1.0
     */
    static void Deallocate(T* ptr, uint32_t)
    {
        delete[] ptr;
    }
};

struct RowData {
    int32_t x1;
    int32_t x2;
    const uint8_t* ptr;
    RowData() {}
    RowData(int32_t x1_, int32_t x2_, const uint8_t* ptr_)
        : x1(x1_), x2(x2_), ptr(ptr_) {}
};
/* Indicates a point in float */
struct PointF {
    float x;
    float y;
    PointF() {}
    PointF(float x_, float y_) : x(x_), y(y_) {}
};
} // namespace OHOS
#endif
