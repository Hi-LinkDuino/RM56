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
* @file render_buffer.h
* @brief Defines Renderer buffer
* @since 1.0
* @version 1.0.
*/

#ifndef GRAPHIC_LITE_RENDER_BUFFER_H
#define GRAPHIC_LITE_RENDER_BUFFER_H

#include <cstring>

#include "gfx_utils/diagram/vertexprimitive/geometry_plaindata_array.h"

namespace OHOS {
/**
 * Row accessor.
 */
class RenderBuffer {
public:
    RenderBuffer() : renBuf_(0), start_(0), width_(0), height_(0), bufStride_(0) {}

    /**
     * @brief RowAccessor Row accessor.
     * @param renBuf buffer.
     * @param areaWidth The width of the pixel area in the buffer.
     * @param areaHeight The height of the pixel area in the buffer.
     * @param stride Buffer stride.
     */
    RenderBuffer(uint8_t* renBuf, uint32_t areaWidth, uint32_t areaHeight, int32_t stride)
        : renBuf_(0),
          start_(0),
          width_(0),
          height_(0),
          bufStride_(0)
    {
        Attach(renBuf, areaWidth, areaHeight, stride);
    }

    /**
     * @brief attach Incoming parameters.
     * @param renBuf buffer
     * @param areaWidth The width of the pixel area in the buffer.
     * @param areaHeightThe height of the pixel area in the buffer.
     * @param stride Buffer stride.
     */
    void Attach(uint8_t* renBuf, uint32_t areaWidth, uint32_t areaHeight, int32_t stride)
    {
        renBuf_ = renBuf;
        start_ = renBuf;
        width_ = areaWidth;
        height_ = areaHeight;
        bufStride_ = stride;
        if (stride < 0) {
            start_ = renBuf - static_cast<int32_t>(areaHeight - 1) * stride;
        }
    }

    /**
     * @brief GetBuf Gets a pointer to the render buffer.
     */
    const uint8_t* GetBuf() const
    {
        return renBuf_;
    }

    /**
     * @brief GetBuf Get area width.
     */
    uint32_t GetWidth() const
    {
        return width_;
    }

    /**
     * @brief GetBuf GetBuf Get area height.
     */
    uint32_t GetHeight() const
    {
        return height_;
    }

    int32_t GetStride() const
    {
        return bufStride_;
    }

    /**
     * @brief GetRowPtr Returns a pointer to the beginning of line y.
     */
    uint8_t* GetRowPtr(int32_t y)
    {
        return start_ + y * bufStride_;
    }

    /**
     * @brief GetRowPtr Returns a pointer to the beginning of line y。
     */
    const uint8_t* GetRowPtr(int32_t y) const
    {
        return start_ + y * bufStride_;
    }

    /**
     * @brief GetRow Get row data
     */
    RowData GetRow(int32_t y) const
    {
        return RowData(0, width_ - 1, GetRowPtr(y));
    }

private:
    uint8_t* renBuf_;       // Pointer to render buffer.
    uint8_t* start_;        // Point to the first pixel according to the stride.
    uint32_t width_;  // Area width.
    uint32_t height_; // Area height.
    int32_t bufStride_;   // Number of bytes per line.
};
} // namespace OHOS
#endif
