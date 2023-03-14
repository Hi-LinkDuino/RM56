/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_LITE_SCREEN_DEVICE_H
#define GRAPHIC_LITE_SCREEN_DEVICE_H

#include "gfx_utils/color.h"
#include "gfx_utils/graphic_assert.h"
#include "gfx_utils/rect.h"
#include "graphic_semaphore.h"
#if ENABLE_WINDOW
#include "gfx_utils/pixel_format_utils.h"
#endif

namespace OHOS {
#if ENABLE_WINDOW
struct AllocationInfo {
    uint8_t* virAddr;
    uint8_t* phyAddr;
    uint16_t width;
    uint16_t height;
    uint32_t stride;
    ImagePixelFormat pixelFormat;
};
#endif

#ifndef TRANSFORMOPTION
#define TRANSFORMOPTION
struct TransformOption {
    TransformAlgorithm algorithm;
};
#endif

/** @brief A semaphore for display buffer flushing. */
class FlushSem : public HeapBase {
public:
    /**
     * Constructor
     *
     * @param [in] isFlushing (Optional) True if is flushing, false if not.
     */
    FlushSem(bool isFlushing = false) : sem_(1, 1), isFlushing_(isFlushing) {}

    /**
     * @brief Destructor
     */
    virtual ~FlushSem() {}

    /** Notifies the buffer is flushing end */
    void Notify()
    {
        isFlushing_ = false;
        sem_.Notify();
    }

    /** Waits the buffer is flushing */
    void Wait()
    {
        while (isFlushing_) {
            sem_.Wait();
        }
    }

    /** set the flag as flashing */
    void Flushing()
    {
        isFlushing_ = true;
    }

private:
    GraphicSemaphore sem_;
    bool isFlushing_;
};

/** @brief A display device. */
class ScreenDevice : public HeapBase {
public:
    /**
     * @brief Constructor
     */
    ScreenDevice() {}

    /**
     * @brief Destructor
     */
    virtual ~ScreenDevice() {}

    virtual void Flush(int16_t x1, int16_t y1, int16_t x2, int16_t y2, const uint8_t* buffer, ColorMode mode) {}

    virtual void Fill(int16_t x1, int16_t y1, int16_t x2, int16_t y2, const ColorType& color) {}

    /**
     * @brief Hardware accelerated filling interface implemented by the product platform
     * @param fillArea Indicates the area to be filled
     * @param color Indicates the color to be filled
     * @param opa Indicates the transparency
     * @param dst Indicates the start address of destination memory
     * @param dstStride Indicates the number of bytes in a single row of destination memory
     * @param dstColorMode Indicates the color format of destination memory
     * @return Return true on success, false on failure
     * @since 5.0
     * @version 3.0
     */
    virtual bool HardwareFill(const Rect& fillArea,
                              uint32_t color,
                              OpacityType opa,
                              uint8_t* dst,
                              uint32_t dstStride,
                              ColorMode dstColorMode)
    {
        return false;
    }

    /**
     * @brief Hardware accelerated blending interface implemented by the product platform
     * @param src Indicates the start address of source memory
     * @param srcRect Indicates the area of ​​the source memory for color blending
     * @param srcStride Indicates the number of bytes in a single row of source memory
     * @param srcLineNumber Indicates the number of source memory rows
     * @param srcColorMode Indicates the source memory color format
     * @param color 32-bit XRGB8888 value
     *              (valid when the source memory is in a format with only alpha information such as A1)
     * @param opa Indicates the transparency
     * @param dst Indicates the start address of destination memory
     * @param dstStride Indicates the number of bytes in a single row of destination memory
     * @param dstColorMode Indicates the color format of destination memory
     * @param x The x coordinate of the upper left vertex of the destination memory for color blending
     * @param y The y coordinate of the upper left vertex of the destination memory for color blending
     * @return Return true on success, false on failure
     * @since 5.0
     * @version 3.0
     */
    virtual bool HardwareBlend(const uint8_t* src,
                               const Rect& srcRect,
                               uint32_t srcStride,
                               uint32_t srcLineNumber,
                               ColorMode srcColorMode,
                               uint32_t color,
                               OpacityType opa,
                               uint8_t* dst,
                               uint32_t dstStride,
                               ColorMode dstColorMode,
                               uint32_t x,
                               uint32_t y)
    {
        return false;
    }

    /**
     * @brief Hardware accelerated transformation interface implemented by the product platform
     * @param src Indicates the start address of the source image
     * @param srcColorMode Indicates the color format of the source image
     * @param srcRect Indicates the position of the source image in the destination memory and its width and height
     * @param transformMatrix  Indicates the transformation matrix
     * @param opa Indicates the transparency
     * @param color 32-bit XRGB8888 value
     *              (valid when the source memory is in a format with only alpha information such as A1)
     * @param mask Indicates the masking rectangle, and the content beyond the rectangle is not drawn
     * @param dst Indicates the start address of destination memory
     * @param dstStride Indicates the number of bytes in a single row of destination memory
     * @param dstColorMode Indicates the color format of destination memory
     * @param option Indicates the optional setting items for transformation operation
     * @return Return true on success, false on failure
     * @since 5.0
     * @version 3.0
     */
    virtual bool HardwareTransform(const uint8_t* src,
                                   ColorMode srcColorMode,
                                   const Rect& srcRect,
                                   const Matrix3<float>& transformMatrix,
                                   OpacityType opa,
                                   uint32_t color,
                                   const Rect& mask,
                                   uint8_t* dst,
                                   uint32_t dstStride,
                                   ColorMode dstColorMode,
                                   const TransformOption& option)
    {
        return false;
    }

    virtual void SnapShot(uint32_t len,
                          bool justCopy,
                          uint8_t* dest,
                          const Rect& rect,
                          bool justRender)
    {
    }

    virtual void RenderFinish(const Rect& mask) {}
};
} // namespace OHOS
#endif // GRAPHIC_LITE_SCREEN_DEVICE_H