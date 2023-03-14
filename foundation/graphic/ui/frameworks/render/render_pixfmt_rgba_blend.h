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

#ifndef GRAPHIC_LITE_RENDER_PIXFMT_RGBA_BLEND_H
#define GRAPHIC_LITE_RENDER_PIXFMT_RGBA_BLEND_H

#include "draw/draw_utils.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "gfx_utils/heap_base.h"
#include "gfx_utils/graphic_log.h"
#include "render/render_buffer.h"

#ifdef ARM_NEON_OPT
#include "graphic_neon_pipeline.h"
#endif
namespace OHOS {
const uint8_t NUM_COMPONENTS = 4;
const uint8_t PIX_STEP = 4;

struct RgbaBlender {
#ifdef ARM_NEON_OPT
    /**
     * @brief Mix the pixels with the color component.
     * @param color color,red,green,blue,alpha Color Component,cover Coverage.
     * @since 1.0
     * @version 1.0
     */
    static inline void NeonBlendPix(
        uint8_t* color, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t* covers)
    {
        DrawUtils::GetInstance()->BlendLerpPix(color, red, green, blue, alpha, covers);
    }

    /**
     * @brief Mix the pixels with the color component.
     * @param color color,red,green,blue,alpha Color Component,cover Coverage.
     * @since 1.0
     * @version 1.0
     */
    static inline void NeonBlendPix(
        uint8_t* color, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t cover)
    {
        DrawUtils::GetInstance()->BlendLerpPix(color, red, green, blue, alpha, cover);
    }

    /**
     * @brief Mix the pixels with the color component.
     * @param color color,red,green,blue,alpha Color Component
     * @since 1.0
     * @version 1.0
     */
    static inline void NeonBlendPix(
        uint8_t* color, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
        DrawUtils::GetInstance()->BlendLerpPix(color, red, green, blue, alpha);
    }

    static inline void NeonBlendPix(uint8_t* dstColors, uint8_t* srcColors, uint8_t srcCover)
    {
        DrawUtils::GetInstance()->BlendLerpPix(dstColors, srcColors, srcCover);
    }

    static inline void NeonBlendPix(uint8_t* dstColors, uint8_t* srcColors, uint8_t* srcCovers)
    {
        DrawUtils::GetInstance()->BlendLerpPix(dstColors, srcColors, srcCovers);
    }
#endif
    /**
     * @brief Mix the pixels with the color component.
     * @param color color,red,green,blue,alpha Color Component,cover Coverage.
     * @since 1.0
     * @version 1.0
     */
    static inline void BlendPix(
        uint8_t* color, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t cover)
    {
        BlendPix(color, red, green, blue, Rgba8T::MultCover(alpha, cover));
    }

    /**
     * @brief Mix the pixels with the color component.
     * @param color color,red,green,blue,alpha Color Component
     * @since 1.0
     * @version 1.0
     */
    static inline void BlendPix(
        uint8_t* color, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
        color[OrderBgra::RED] = Rgba8T::Lerp(color[OrderBgra::RED], red, alpha);
        color[OrderBgra::GREEN] = Rgba8T::Lerp(color[OrderBgra::GREEN], green, alpha);
        color[OrderBgra::BLUE] = Rgba8T::Lerp(color[OrderBgra::BLUE], blue, alpha);
        color[OrderBgra::ALPHA] = Rgba8T::Prelerp(color[OrderBgra::ALPHA], alpha, alpha);
    }
};

struct PixelColorType {
    uint8_t colors[NUM_COMPONENTS];

    /**
     * @brief Set Colors.
     * @param r,g,b,a Color Component.
     * @since 1.0
     * @version 1.0
     */
    void SetPixelColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
        colors[OrderBgra::RED] = red;
        colors[OrderBgra::GREEN] = green;
        colors[OrderBgra::BLUE] = blue;
        colors[OrderBgra::ALPHA] = alpha;
    }

    /**
     * @brief Set Colors.
     * @param color Colors.
     * @since 1.0
     * @version 1.0
     */
    void SetPixelColor(const Rgba8T& color)
    {
        SetPixelColor(color.red, color.green, color.blue, color.alpha);
    }

    /**
     * @brief Get Colors.
     * @param r,g,b,a Color Component.
     * @since 1.0
     * @version 1.0
     */
    void GetPixelColor(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha) const
    {
        red = colors[OrderBgra::RED];
        green = colors[OrderBgra::GREEN];
        blue = colors[OrderBgra::BLUE];
        alpha = colors[OrderBgra::ALPHA];
    }

    /**
     * @brief Get Colors.
     * @return Colors
     * @since 1.0
     * @version 1.0
     */
    Rgba8T GetPixelColor() const
    {
        return Rgba8T(colors[OrderBgra::RED], colors[OrderBgra::GREEN],
                      colors[OrderBgra::BLUE], colors[OrderBgra::ALPHA]);
    }

    /**
    * @brief Gets the color component of the next pixel.
    *
    * @since 1.0
    * @version 1.0
    */
    PixelColorType* Next()
    {
        return reinterpret_cast<PixelColorType*>((colors + PIX_STEP));
    }

    /**
    * @brief Gets the first address of the color component of the next pixel.
    *
    * @since 1.0
    * @version 1.0
    */
    const PixelColorType* Next() const
    {
        return reinterpret_cast<const PixelColorType*>(colors + PIX_STEP);
    }

    /**
    * @brief Gets the first address of the color component of the nth pixe.
    * @param pixelIndex pixel offset
    * @since 1.0
    * @version 1.0
    */
    PixelColorType* Advance(int32_t pixelIndex)
    {
        return reinterpret_cast<PixelColorType*>(colors + pixelIndex * PIX_STEP);
    }

    /**
    * @brief Gets the first address of the color component of the nth pixe.
    * @param pixelIndex pixel offset
    * @since 1.0
    * @version 1.0
    */
    const PixelColorType* Advance(int32_t pixelIndex) const
    {
        return reinterpret_cast<const PixelColorType*>(colors + pixelIndex * PIX_STEP);
    }
};

class RenderPixfmtRgbaBlend : public HeapBase {
public:
    const uint8_t PIX_WIDTH = sizeof(uint8_t) * PIX_STEP;
    RenderPixfmtRgbaBlend() : rBuf_(0) {}
    explicit RenderPixfmtRgbaBlend(RenderBuffer& rBuf) :  rBuf_(&rBuf) {}

    /**
    * @brief Attach pixels to the drawing area.
    *
    * @since 1.0
    * @version 1.0
    */
    virtual void Attach(RenderBuffer& rBuf)
    {
        rBuf_ = &rBuf;
    }

    bool Attach(RenderPixfmtRgbaBlend& pixf, int32_t x1, int32_t y1, int32_t x2, int32_t y2);

    /**
    * @brief Get the size of each screen (draw buffer).
    *
    * @since 1.0
    * @version 1.0
    */
    virtual inline uint32_t GetWidth() const
    {
        return rBuf_->GetWidth();
    }

    virtual inline uint32_t GetHeight() const
    {
        return rBuf_->GetHeight();
    }

    /**
      * @brief Returns the number of bytes in a row of the window.
      *
      * @since 1.0
      * @version 1.0
      */
    virtual inline int32_t GetStride() const
    {
        return rBuf_->GetStride();
    }

    /**
      * @brief Returns the row address of a window row.
      *
      * @since 1.0
      * @version 1.0
      */
    virtual inline uint8_t* GetRowPtr(int32_t y)
    {
        return rBuf_->GetRowPtr(y);
    }

    /**
      * @brief Returns the row address of a window row.
      *
      * @since 1.0
      * @version 1.0
      */
    virtual inline const uint8_t* GetRowPtr(int32_t y) const
    {
        return rBuf_->GetRowPtr(y);
    }

    /**
      * @brief Returns the row data of one row of the window.
      *
      * @since 1.0
      * @version 1.0
      */
    virtual inline RowData GetRow(int32_t y) const
    {
        return rBuf_->GetRow(y);
    }

    virtual inline uint8_t* PixPtr(int32_t x, int32_t y)
    {
        return rBuf_->GetRowPtr(y) + sizeof(uint8_t) * (x * PIX_STEP);
    }

    virtual inline const uint8_t* PixPtr(int32_t x, int32_t y) const
    {
        return rBuf_->GetRowPtr(y) + sizeof(uint8_t) * (x * PIX_STEP);
    }

    /**
     * @brief Pointer to pixel type pointer.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual inline PixelColorType* PixValuePtr(int32_t x, int32_t y)
    {
        return reinterpret_cast<PixelColorType*>(rBuf_->GetRowPtr(y) + sizeof(uint8_t) * (x * PIX_STEP));
    }

    /**
     * @brief Gets the pixel address of the specified left.
     *
     * @since 1.0
     * @version 1.0.
     */
    virtual inline const PixelColorType* PixValuePtr(int32_t x, int32_t y) const
    {
        uint8_t* pixelPtr = rBuf_->GetRowPtr(y);
        return pixelPtr ? reinterpret_cast<PixelColorType*>(pixelPtr + sizeof(uint8_t) * (x * PIX_STEP)) : nullptr;
    }

    /**
     * @brief Pointer to pixel type pointer.
     *
     * @since 1.0
     * @version 1.0
     */
    inline static PixelColorType* PixValuePtr(void* pixelPtr)
    {
        return static_cast<PixelColorType*>(pixelPtr);
    }

    /**
     * @brief Pointer to pixel type pointer.
     *
     * @since 1.0
     * @version 1.0
     */
    inline static const PixelColorType* PixValuePtr(const void* pixelPtr)
    {
        return static_cast<const PixelColorType*>(pixelPtr);
    }

    /**
     * @brief Gets the color of a pixel.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual inline Rgba8T Pixel(int32_t x, int32_t y) const
    {
        const PixelColorType* pixelPtr = PixValuePtr(x, y);
        if (pixelPtr != nullptr) {
            return pixelPtr->GetPixelColor();
        }
        return Rgba8T::NoColor();
    }

    /**
     * @brief Copy color to pixel.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual inline void CopyPixel(int32_t x, int32_t y, const Rgba8T& color)
    {
        PixValuePtr(x, y)->SetPixelColor(color);
    }

    /**
    * @brief Mix pixels (x, y) with the transparency of cover (coverage = transparency) with color color.
    *
    * @since 1.0
    * @version 1.0
    */
    virtual inline void BlendPixel(int32_t x, int32_t y, const Rgba8T& color, uint8_t cover)
    {
        CopyOrBlendPix(PixValuePtr(x, y), color, cover);
    }

    /**
     * @brief Set the pixels of len length in horizontal order starting from (x, y).
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void CopyHLine(int32_t x, int32_t y,
                           uint32_t len,
                           const Rgba8T& color);

    /**
     * @brief Pixels of len length are mixed horizontally from (x, y).
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void BlendHLine(int32_t x, int32_t y,
                            uint32_t len,
                            const Rgba8T& color,
                            uint8_t cover);

    /**
     * @brief Mix a series of colors of len length in horizontal order starting from (x, y).
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void BlendSolidHSpan(int32_t x, int32_t y,
                                 uint32_t len,
                                 const Rgba8T& color,
                                 const uint8_t* covers);

    /**
     * @brief Mix a series of colors of len length in vertical order starting from (x, y).
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void BlendSolidVSpan(int32_t x, int32_t y,
                                 uint32_t len,
                                 const Rgba8T& color,
                                 const uint8_t* covers);

    /**
     * @brief Set the color of len length in horizontal order starting from (x, y).
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void CopyColorHSpan(int32_t x, int32_t y,
                                uint32_t len,
                                const Rgba8T* colors);

    /**
     * @brief Set the color of len length in vertical order starting from (x, y).
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void CopyColorVSpan(int32_t x, int32_t y,
                                uint32_t len,
                                const Rgba8T* colors);

    /**
     * @brief Mix a series of colors of len length in horizontal order starting from (x, y).
     * @param x         x coordinate.
     * @param y         x coordinate
     * @param len       length of line
     * @param colors    colors array
     * @param covers    transition value
     * @param cover     cover value
     * @since 1.0
     * @version 1.0
     */
    void BlendColorHSpan(int32_t x, int32_t y,
                         uint32_t len,
                         const Rgba8T* colors,
                         const uint8_t* covers,
                         uint8_t cover);

protected:
#ifdef ARM_NEON_OPT
    virtual inline void NeonBlendPix(PixelColorType* pixelColors, const Rgba8T& color)
    {
        blender_.NeonBlendPix(pixelColors->colors, color.red, color.green,
                              color.blue, color.alpha);
    }

    virtual inline void NeonBlendPix(PixelColorType* pixelColors, const Rgba8T& color, uint8_t cover)
    {
        blender_.NeonBlendPix(pixelColors->colors, color.red, color.green,
                              color.blue, color.alpha, cover);
    }

    virtual inline void NeonBlendPix(uint8_t* dstColors, uint8_t* srcColors, uint8_t srcCover)
    {
        blender_.NeonBlendPix(dstColors, srcColors, srcCover);
    }

    virtual inline void NeonBlendPix(uint8_t* dstColors, uint8_t* srcColors, uint8_t* srcCovers)
    {
        blender_.NeonBlendPix(dstColors, srcColors, srcCovers);
    }

    virtual inline void NeonBlendPix(uint8_t* dstColors, const Rgba8T& srcColors, uint8_t* srcCovers)
    {
        blender_.NeonBlendPix(dstColors, srcColors.red, srcColors.green, srcColors.blue,
                              srcColors.alpha, srcCovers);
    }
#endif

    /**
     * @brief Blend color and coverage to specified pixels.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual inline void BlendPix(PixelColorType* pixelPtr, const Rgba8T& color, uint32_t cover)
    {
        blender_.BlendPix(pixelPtr->colors, color.red, color.green, color.blue, color.alpha, cover);
    }

    /**
     * @brief Blend colors to specified pixels.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual inline void BlendPix(PixelColorType* pixelPtr, const Rgba8T& color)
    {
        blender_.BlendPix(pixelPtr->colors, color.red, color.green, color.blue, color.alpha);
    }

    /**
     * @brief Set or blend to specified pixels with color and coverage.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual inline void CopyOrBlendPix(PixelColorType* pixelPtr, const Rgba8T& color, uint32_t cover)
    {
        if (!color.IsTransparent()) {
            if (color.IsOpaque() && cover == COVER_MASK) {
                pixelPtr->SetPixelColor(color.red, color.green, color.blue, color.alpha);
            } else {
                blender_.BlendPix(pixelPtr->colors, color.red, color.green,
                                  color.blue, color.alpha, cover);
            }
        }
    }

    /**
     * @brief Sets or blends the color to the specified pixel.
     *
     * @since 1.0
     * @version 1.0.
     */
    virtual inline void CopyOrBlendPix(PixelColorType* pixelPtr, const Rgba8T& color)
    {
        if (!color.IsTransparent()) {
            if (color.IsOpaque()) {
                pixelPtr->SetPixelColor(color);
            } else {
                blender_.BlendPix(pixelPtr->colors, color.red, color.green,
                                  color.blue, color.alpha);
            }
        }
    }

    RenderBuffer* rBuf_;
    RgbaBlender blender_;
};
} // namespace OHOS
#endif
