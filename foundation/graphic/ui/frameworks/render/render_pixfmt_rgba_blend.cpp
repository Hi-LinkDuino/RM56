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

#include "render_pixfmt_rgba_blend.h"

namespace OHOS {
bool RenderPixfmtRgbaBlend::Attach(RenderPixfmtRgbaBlend& pixf, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    Rect32 r(x1, y1, x2, y2);
    if (r.Intersect(r, Rect32(0, 0, pixf.GetWidth() - 1, pixf.GetHeight() - 1))) {
        int32_t stride = pixf.GetStride();
        rBuf_->Attach(pixf.PixPtr(r.GetLeft(), stride < 0 ? r.GetBottom() : r.GetTop()),
                      (r.GetRight() - r.GetLeft()) + 1, (r.GetBottom() - r.GetTop()) + 1, stride);
        return true;
    }
    return false;
}

void RenderPixfmtRgbaBlend::CopyHLine(int32_t x, int32_t y,
                                      uint32_t len,
                                      const Rgba8T& color)
{
    PixelColorType vPixelValue;
    vPixelValue.SetPixelColor(color);
    PixelColorType* pixelPtr = PixValuePtr(x, y);
#ifdef NEON_ARM_OPT
    int16_t step = NEON_STEP_8 * PIX_STEP;
    while (len >= NEON_STEP_8) {
        SetPixelColor_ARGB8888(pixelPtr->colors, color->red,
                               colors->green, colors->blue,
                               colors->alpha);
        pixelPtr = pixelPtr->colors + step;
        len -= NEON_STEP_8;
    };
#endif
    for (int16_t iPixel = 0; iPixel < len; ++iPixel) {
        *pixelPtr = vPixelValue;
        pixelPtr = pixelPtr->Next();
    }
}

void RenderPixfmtRgbaBlend::BlendHLine(int32_t x, int32_t y,
                                       uint32_t len,
                                       const Rgba8T& color,
                                       uint8_t cover)
{
    if (!color.IsTransparent()) {
        PixelColorType* pPixel = PixValuePtr(x, y);
#ifdef NEON_ARM_OPT
        int16_t step = NEON_STEP_8 * PIX_STEP;
        while (len >= NEON_STEP_8) {
            NeonBlendPix(pixelPtr, color, cover);
            pixelPtr = pixelPtr->colors + step;
            len -= NEON_STEP_8;
        };
#endif
        if (color.IsOpaque() && cover == COVER_MASK) {
            for (int16_t iPixel = 0; iPixel < len; ++iPixel) {
                PixelColorType pixelValue;
                pixelValue.SetPixelColor(color);
                *pPixel = pixelValue;
                pPixel = pPixel->Next();
            }
        } else {
            if (cover == COVER_MASK) {
                for (int16_t iPixel = 0; iPixel < len; ++iPixel) {
                    BlendPix(pPixel, color);
                    pPixel = pPixel->Next();
                }
            } else {
                for (int16_t iPixel = 0; iPixel < len; ++iPixel) {
                    BlendPix(pPixel, color, cover);
                    pPixel = pPixel->Next();
                }
            }
        }
    }
}

void RenderPixfmtRgbaBlend::BlendSolidHSpan(int32_t x, int32_t y,
                                            uint32_t len,
                                            const Rgba8T& color,
                                            const uint8_t* covers)
{
    if (!color.IsTransparent()) {
        PixelColorType* pixelPtr = PixValuePtr(x, y);

#ifdef NEON_ARM_OPT
        int16_t step = NEON_STEP_8 * PIX_STEP;
        while (len >= NEON_STEP_8) {
            NeonBlendPix(pixelPtr->colors, color, covers);
            pixelPtr = pixelPtr->colors + step;
            covers += NEON_STEP_8;
            len -= NEON_STEP_8;
        };
#endif

        for (int16_t iPixel = 0; iPixel < len; ++iPixel) {
            if (color.IsOpaque() && *covers == COVER_MASK) {
                pixelPtr->SetPixelColor(color);
            } else {
                BlendPix(pixelPtr, color, *covers);
            }
            pixelPtr = pixelPtr->Next();
            ++covers;
        }
    }
}

void RenderPixfmtRgbaBlend::BlendSolidVSpan(int32_t x, int32_t y,
                                            uint32_t len,
                                            const Rgba8T& color,
                                            const uint8_t* covers)
{
    if (!color.IsTransparent()) {
        do {
            PixelColorType* pixelPtr = PixValuePtr(x, y++);
            if (color.IsOpaque() && *covers == COVER_MASK) {
                pixelPtr->SetPixelColor(color);
            } else {
                BlendPix(pixelPtr, color, *covers);
            }
            ++covers;
        } while (--len);
    }
}

void RenderPixfmtRgbaBlend::CopyColorHSpan(int32_t x, int32_t y,
                                           uint32_t len,
                                           const Rgba8T* colors)
{
    PixelColorType* pixelPtr = PixValuePtr(x, y);
#ifdef NEON_ARM_OPT
    int16_t step = NEON_STEP_8 * PIX_STEP;
    const int16_t NEON_STEP_COMPONENTS = NEON_STEP_8 * NUM_COMPONENTS;
    uint8_t mColors[NEON_STEP_COMPONENTS];
    while (len >= NEON_STEP_8) {
        if (memset_s(mColors, size_t(NEON_STEP_COMPONENTS), 0, size_t(NEON_STEP_COMPONENTS)) != EOK) {
            return GRAPHIC_LOGE("CopyColorHSpan faile");
        }
        NeonMemcpy(mColors, NEON_STEP_COMPONENTS, colors, NEON_STEP_COMPONENTS);

        SetPixelColor_ARGB8888(pixelPtr->colors, mColors);
        pixelPtr = pixelPtr->colors + step;
        colors += NEON_STEP_8;
        len -= NEON_STEP_8;
    };
#endif
    for (int16_t iPixel = 0; iPixel < len; ++iPixel) {
        pixelPtr->SetPixelColor(*colors++);
        pixelPtr = pixelPtr->Next();
    }
}

void RenderPixfmtRgbaBlend::CopyColorVSpan(int32_t x, int32_t y,
                                           uint32_t len,
                                           const Rgba8T* colors)
{
    do {
        PixValuePtr(x, y++)->SetPixelColor(*colors++);
    } while (--len);
}

void RenderPixfmtRgbaBlend::BlendColorHSpan(int32_t x, int32_t y,
                                            uint32_t len,
                                            const Rgba8T* colors,
                                            const uint8_t* covers,
                                            uint8_t cover)
{
    PixelColorType* pixelPtr = PixValuePtr(x, y);
    if (covers) {
#ifdef NEON_ARM_OPT
        int16_t step = NEON_STEP_8 * PIX_STEP;
        const int16_t NEON_STEP_COMPONENTS = NEON_STEP_8 * NUM_COMPONENTS;
        uint8_t mColors[NEON_STEP_COMPONENTS];
        while (len >= NEON_STEP_8) {
            if (memset_s(mColors, size_t(NEON_STEP_COMPONENTS), 0, size_t(NEON_STEP_COMPONENTS)) != EOK) {
                GRAPHIC_LOGE("BlendColorHSpan fail");
                return;
            }
            NeonMemcpy(mColors, NEON_STEP_COMPONENTS, colors, NEON_STEP_COMPONENTS);

            NeonBlendPix(pixelPtr->colors, mColors, covers);
            pixelPtr = pixelPtr->colors + step;
            colors += NEON_STEP_8;
            covers += NEON_STEP_8;
            len -= NEON_STEP_8;
        };
#endif
        for (int16_t iPixel = 0; iPixel < len; ++iPixel) {
            CopyOrBlendPix(pixelPtr, *colors++, *covers++);
            pixelPtr = pixelPtr->Next();
        }
    } else {
#ifdef NEON_ARM_OPT
        int16_t step = NEON_STEP_8 * PIX_STEP;
        const int16_t NEON_STEP_COMPONENTS = NEON_STEP_8 * NUM_COMPONENTS;
        uint8_t mColors[NEON_STEP_COMPONENTS];
        while (len >= NEON_STEP_8) {
            if (memset_s(mColors, size_t(NEON_STEP_COMPONENTS), 0, size_t(NEON_STEP_COMPONENTS)) != EOK) {
                GRAPHIC_LOGE("BlendColorHSpan fail");
                return;
            }
            NeonMemcpy(mColors, NEON_STEP_COMPONENTS, colors, NEON_STEP_COMPONENTS);

            NeonBlendPix(pixelPtr->colors, mColors, cover);
            pixelPtr = pixelPtr->colors + step;
            colors += NEON_STEP_8;
            len -= NEON_STEP_8;
        };
#endif
        if (cover == COVER_MASK) {
            for (int16_t iPixel = 0; iPixel < len; ++iPixel) {
                cover == COVER_MASK ? CopyOrBlendPix(pixelPtr, *colors++) :
                                      CopyOrBlendPix(pixelPtr, *colors++, cover);
                pixelPtr = pixelPtr->Next();
            }
        }
    }
}
} // namespace OHOS
