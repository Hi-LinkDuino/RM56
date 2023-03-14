/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "soft_blit.h"
#include <string>
#include <unordered_map>
#include <algorithm>
#include "display_test.h"

namespace {
const int MAX_FLOAT_TO_INT32 = 2147483520;
const int MIN_FLOAT_TO_INT32 = -2147483520;

using BlendFunction = void (*)(ColorRGBAf &src, ColorRGBAf &dst);

static inline uint32_t RgbaGetA(uint32_t color)
{
    return ((color >> 0) & 0xFF);
}

static inline uint32_t RgbaGetR(uint32_t color)
{
    const uint32_t shift = 24;
    const uint32_t mask = 0xFF;
    return ((color >> shift) & mask);
}

static inline uint32_t RgbaGetG(uint32_t color)
{
    const uint32_t shift = 16;
    const uint32_t mask = 0xFF;
    return ((color >> shift) & mask);
}

static inline uint32_t RgbaGetB(uint32_t color)
{
    const uint32_t shift = 8;
    const uint32_t mask = 0xFF;
    return ((color >> shift) & mask);
}

static constexpr inline uint32_t ColorSetARGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    const uint32_t shiftR = 24;
    const uint32_t shiftG = 16;
    const uint32_t shiftB = 8;
    return (r << shiftR) | (g << shiftG) | (b << shiftB) | (a << 0);
}
// blend none
/*
fs: sa      fd: 1.0-sa
pixel = (foreground x fs + background x fd)
*/
static inline void BlendNone(ColorRGBAf &src, ColorRGBAf &dst)
{
    dst.mR = src.mR * src.mA + dst.mR * (1 - src.mA);
    dst.mG = src.mG * src.mA + dst.mG * (1 - src.mA);
    dst.mB = src.mB * src.mA + dst.mB * (1 - src.mA);
    dst.mA = src.mA * src.mA + dst.mA * (1 - src.mA);
}

// blend clear
static inline void BlendClear(ColorRGBAf &src, ColorRGBAf &dst)
{
    dst.mA = 0;
    dst.mR = 0;
    dst.mG = 0;
    dst.mB = 0;
}

// blend src
static inline void BlendSrc(ColorRGBAf &src, ColorRGBAf &dst)
{
    dst = src;
}

// blend src over
static inline void BlendSrcOver(ColorRGBAf &src, ColorRGBAf &dst)
{
    // the display_gfx has Premultiplied
    src.mR *= src.mA;
    src.mG *= src.mA;
    src.mB *= src.mA;

    dst.mR = src.mR + (1 - src.mA) * dst.mR;
    dst.mG = src.mG + (1 - src.mA) * dst.mG;
    dst.mB = src.mB + (1 - src.mA) * dst.mB;
    dst.mA = src.mA + (1 - src.mA) * dst.mA;
}

// blend src in
static inline void BlendSrcIn(ColorRGBAf &src, ColorRGBAf &dst)
{
    dst.mR = src.mR * dst.mA;
    dst.mG = src.mG * dst.mA;
    dst.mB = src.mB * dst.mA;
    dst.mA = src.mA * dst.mA;
}

// blend src out
static inline void BlendSrcOut(ColorRGBAf &src, ColorRGBAf &dst)
{
    dst.mR = (1 - dst.mA) * src.mR;
    dst.mG = (1 - dst.mA) * src.mG;
    dst.mB = (1 - dst.mA) * src.mB;
    dst.mA = (1 - dst.mA) * src.mA;
}

// blend src Atop
static inline void BlendAtop(ColorRGBAf &src, ColorRGBAf &dst)
{
    dst.mR = dst.mA * src.mR + (1 - src.mA) * dst.mR;
    dst.mG = dst.mA * src.mG + (1 - src.mA) * dst.mG;
    dst.mB = dst.mA * src.mB + (1 - src.mA) * dst.mB;
    dst.mA = dst.mA;
}

// blend dst
static inline void BlendDst(ColorRGBAf &src, ColorRGBAf &dst)
{
    (void)src;
    (void)dst;
}

// blend dst atop
static inline void BlendDstAtop(ColorRGBAf &src, ColorRGBAf &dst)
{
    dst.mR = src.mA * dst.mR + (1 - dst.mA) * src.mR;
    dst.mG = src.mA * dst.mG + (1 - dst.mA) * src.mG;
    dst.mB = src.mA * dst.mB + (1 - dst.mA) * src.mB;
    dst.mA = src.mA;
}

// blend dst in
static inline void BlendDstIn(ColorRGBAf &src, ColorRGBAf &dst)
{
    dst.mR = dst.mR * src.mA;
    dst.mG = dst.mG * src.mA;
    dst.mB = dst.mB * src.mA;
    dst.mA = src.mA * dst.mA;
}

// blend dst out
static inline void BlendDstOut(ColorRGBAf &src, ColorRGBAf &dst)
{
    dst.mR = (1 - src.mA) * dst.mR;
    dst.mG = (1 - src.mA) * dst.mG;
    dst.mB = (1 - src.mA) * dst.mB;
    dst.mA = (1 - src.mA) * dst.mA;
}

// blend dst over
static inline void BlendDstOver(ColorRGBAf &src, ColorRGBAf &dst)
{
    // the display_gfx has Premultiplied
    dst.mR *= dst.mA;
    dst.mG *= dst.mA;
    dst.mB *= dst.mA;

    dst.mR = dst.mR + (1 - dst.mA) * src.mR;
    dst.mG = dst.mG + (1 - dst.mA) * src.mG;
    dst.mB = dst.mB + (1 - dst.mA) * src.mB;
    dst.mA = dst.mA + (1 - dst.mA) * src.mA;
}

// blend xor
static inline void BlendXor(ColorRGBAf &src, ColorRGBAf &dst)
{
    dst.mR = (1 - dst.mA) * src.mR + (1 - src.mA) * dst.mR;
    dst.mG = (1 - dst.mA) * src.mG + (1 - src.mA) * dst.mG;
    dst.mB = (1 - dst.mA) * src.mB + (1 - src.mA) * dst.mB;
    dst.mA = (1 - dst.mA) * src.mA + (1 - src.mA) * dst.mA;
}

// blend add
static inline void BlendAdd(ColorRGBAf &src, ColorRGBAf &dst)
{
    dst.mR = std::max(0.0f, std::min(src.mR + dst.mR, 1.0f));
    dst.mG = std::max(0.0f, std::min(src.mG + dst.mG, 1.0f));
    dst.mB = std::max(0.0f, std::min(src.mB + dst.mB, 1.0f));
    dst.mA = std::max(0.0f, std::min(src.mA + dst.mA, 1.0f));
}

static BlendFunction GetBlendFunc(BlendType type)
{
    std::unordered_map<BlendType, BlendFunction> maps = {
        { BLEND_CLEAR, BlendClear },     { BLEND_SRC, BlendSrc },         { BLEND_DST, BlendDst },
        { BLEND_SRCOVER, BlendSrcOver }, { BLEND_SRCIN, BlendSrcIn },     { BLEND_SRCOUT, BlendSrcOut },
        { BLEND_SRCATOP, BlendAtop },    { BLEND_DSTIN, BlendDstIn },     { BLEND_DSTOUT, BlendDstOut },
        { BLEND_DSTATOP, BlendDstAtop }, { BLEND_DSTOVER, BlendDstOver }, { BLEND_XOR, BlendXor },
        { BLEND_ADD, BlendAdd },         { BLEND_NONE, BlendNone },
    };
    if (maps.find(type) != maps.end()) {
        return maps[type];
    } else {
        DISPLAY_TEST_LOGE("has no function for blend type %d maps.size() %zd", type, maps.size());
        return nullptr;
    }
}

static inline int32_t ConvertFloatToint(float f)
{
    if (f > MAX_FLOAT_TO_INT32) {
        return MAX_FLOAT_TO_INT32;
    }
    if (f < MIN_FLOAT_TO_INT32) {
        return MIN_FLOAT_TO_INT32;
    }
    return static_cast<int32_t>(f);
}

static int32_t GetPixelFormatBpp(PixelFormat format)
{
    const int32_t bppRgba8888 = 32;
    switch (format) {
        case PIXEL_FMT_RGBA_8888:
            return bppRgba8888;
        default:
            return -1;
    }
}

static uint32_t GetPixelRGBA32(const BufferHandle &handle, int x, int y)
{
    const int32_t pixelBytes = 4;
    int32_t bpp = GetPixelFormatBpp((PixelFormat)handle.format);
    DISPLAY_TEST_CHK_RETURN((bpp <= 0), 0, DISPLAY_TEST_LOGE("CheckPixel do not support format %d", handle.format));
    DISPLAY_TEST_CHK_RETURN((handle.virAddr == nullptr), 0,
        DISPLAY_TEST_LOGE("CheckPixel viraddr is null must map it"));
    DISPLAY_TEST_CHK_RETURN((x < 0 || x >= handle.width), 0,
        DISPLAY_TEST_LOGE("CheckPixel invalid parameter x:%d width:%d", x, handle.width));
    DISPLAY_TEST_CHK_RETURN((y < 0 || y >= handle.height), 0,
        DISPLAY_TEST_LOGE("CheckPixel invalid parameter y:%d height:%d", y, handle.height));

    int32_t position = y * handle.width + x;
    if ((position * pixelBytes) > handle.size) {
        DISPLAY_TEST_LOGE("the pixel postion outside\n");
    }
    uint32_t *pixel = reinterpret_cast<uint32_t *>(handle.virAddr) + position;
    return *pixel;
}

ColorRGBAf::ColorRGBAf(uint32_t r, uint32_t g, uint32_t b, uint32_t a)
{
    const float inv = 1.0f / 255.0f;
    mA = a * inv;
    mR = r * inv;
    mG = g * inv;
    mB = b * inv;
}

bool ColorRGBA32::Compare(ColorRGBA32 &other)
{
    const int endure = 1;
    if ((std::abs(other.mA - mA) > endure) || (std::abs(other.mR - mR) > endure) ||
        (std::abs(other.mG - mG) > endure) || (std::abs(other.mB - mB) > endure)) {
        DISPLAY_TEST_LOGE("compare failed mC %x other.mC %x ", mC, other.mC);
        return false;
    }
    return true;
}

ColorRGBA32::ColorRGBA32(uint32_t pixel)
{
    mA = RgbaGetA(pixel);
    mR = RgbaGetR(pixel);
    mG = RgbaGetG(pixel);
    mB = RgbaGetB(pixel);
    mC = pixel;
}

ColorRGBA32::ColorRGBA32(ColorRGBAf colorF)
{
    const uint8_t rgbMax = 255;
    const float round = 0.5;
    mA = static_cast<uint8_t>(ConvertFloatToint(colorF.mA * rgbMax + round));
    mR = static_cast<uint8_t>(ConvertFloatToint(colorF.mR * rgbMax + round));
    mG = static_cast<uint8_t>(ConvertFloatToint(colorF.mG * rgbMax + round));
    mB = static_cast<uint8_t>(ConvertFloatToint(colorF.mB * rgbMax + round));
    mC = ColorSetARGB(mR, mG, mB, mA);
}
}

bool SoftBlit::RunAndCheck(const BufferHandle &exBuffer)
{
    BlendFunction blendFunc = GetBlendFunc(mType);
    if (blendFunc == nullptr) {
        return false;
    }
    DISPLAY_TEST_LOGD("blend RunAndCheck begin");
    for (int x = mSrcRect.x; x < mSrcRect.w; x++) {
        for (int y = mSrcRect.y; y < mSrcRect.h; y++) {
            uint32_t srcPixel = GetPixelRGBA32(mSrcBuffer, x, y);
            uint32_t dstPixel = GetPixelRGBA32(mDstBuffer, x, y);
            uint32_t exPixel = GetPixelRGBA32(exBuffer, x, y);
            ColorRGBAf srcColorF =
                ColorRGBAf(RgbaGetR(srcPixel), RgbaGetG(srcPixel), RgbaGetB(srcPixel), RgbaGetA(srcPixel));
            ColorRGBAf dstColorF =
                ColorRGBAf(RgbaGetR(dstPixel), RgbaGetG(dstPixel), RgbaGetB(dstPixel), RgbaGetA(dstPixel));
            blendFunc(srcColorF, dstColorF);
            ColorRGBA32 dstColrRGBA = ColorRGBA32(dstColorF);
            ColorRGBA32 exRGBA = ColorRGBA32(exPixel);
            if (!dstColrRGBA.Compare(exRGBA)) {
                DISPLAY_TEST_LOGE(
                    "blend check failed x %d, y %d, srcPixel %x, dstPixel %x , exPixel %x, blendResult %x blendtype %d",
                    x, y, srcPixel, dstPixel, exPixel, dstColrRGBA.mC, mType);
                return false;
            }
        }
    }
    DISPLAY_TEST_LOGD("blend RunAndCheck end");
    return true;
}

SoftBlit::SoftBlit(const BufferHandle &srcBuffer, const IRect &srcRect, const BufferHandle &dstBuffer,
    const IRect &dstRect, const BlendType type)
{
    mSrcRect = srcRect;
    mDstRect = dstRect;
    mSrcBuffer = srcBuffer;
    mDstBuffer = dstBuffer;
    mType = type;
}