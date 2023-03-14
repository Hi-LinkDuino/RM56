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
 * @brief Defines ClippingFlagsE, ClipMovePoint, ClipLineSegment.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 *
 * @brief Defines Cutting related methods
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_COMMON_CLIP_OPERATE_H
#define GRAPHIC_LITE_COMMON_CLIP_OPERATE_H

#include "gfx_utils/diagram/common/common_basics.h"

#include <gfx_utils/rect.h>

namespace OHOS {
const int32_t CLIP_LINE_SEGMENT_FULL_VISIB = 0; // Fully visible
const int32_t CLIP_LINE_SEGMENT_FULL_CLIP = 4;  // Full clipping
/**
 *   0110  |  0010  | 0011
 *         |        |
 *  -------+--------+-------- clipBox.y2
 *         |        |
 *   0100  |  0000  | 0001
 *         |        |
 *  -------+--------+-------- clipBox.y1
 *         |        |
 *   1100  |  1000  | 1001
 *         |        |
 * clipBox.x1 clipBox.x2
 * @brief Determine the position of Y and cut it longitudinally
 * @param Y vertical position, Clipbox clipping window
 * @return Return the corresponding area code
 * @since 1.0
 * @version 1.0
 */
template <class T>
inline uint32_t ClippingFlagsY(T y, const CommonRect<T>& clipBox)
{
    return ((y < clipBox.GetTop()) << 0x03) | ((y > clipBox.GetBottom()) << 0x01);
}

/**
 * @brief Determine the position of X and cut horizontally
 * @param X horizontal position, Clipbox clipping window
 * @return Return the corresponding area code
 * @since 1.0
 * @version 1.0
 */
template <class T>
inline uint32_t ClippingFlagsX(T x, const CommonRect<T>& clipBox)
{
    return ((x < clipBox.GetLeft()) << 0x02) | (x > clipBox.GetRight());
}

/**
 * @brief The clipping of vertices is determined according to the position of vertices
 * @param x. y vertex position, Clipbox clipping window
 * @return Return the corresponding area code
 * @since 1.0
 * @version 1.0
 */
template <class T>
inline uint32_t ClippingFlags(T x, T y, const CommonRect<T>& clipBox)
{
    return ((x < clipBox.GetLeft()) << 0x02) | ((y < clipBox.GetTop()) << 0x03) |
     (x > clipBox.GetRight()) | ((y > clipBox.GetBottom()) << 0x01);
}
} // namespace OHOS
#endif
