/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_LITE_GRAPHIC_NEON_UTILS_H
#define GRAPHIC_LITE_GRAPHIC_NEON_UTILS_H

#include "graphic_config.h"
#ifdef ARM_NEON_OPT
#include <arm_neon.h>

#include "gfx_utils/color.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/graphic_types.h"

namespace OHOS {
#define BASEMSB 128
#define NEON_STEP_4 4
#define NEON_STEP_8 8
#define NEON_STEP_32 32
#define NEON_A 3
#define NEON_R 2
#define NEON_G 1
#define NEON_B 0

static inline uint8x8_t Multipling(uint8x8_t a, uint8x8_t b)
{
    uint16x8_t calcType = vmlal_u8(vdupq_n_u16(BASEMSB), a, b);
    uint8x8_t result = vshrn_n_u16(calcType, NEON_STEP_8);
    return vshrn_n_u16(vaddq_u16(vmovl_u8(result), calcType), NEON_STEP_8);
}

static inline uint8x8_t NeonPreLerp(uint8x8_t p, uint8x8_t q, uint8x8_t a)
{
    uint16x8_t calcType = vaddl_u8(p, q);
    return vsub_u8(vshrn_n_u16(calcType, NEON_STEP_8), Multipling(p, a));
}

static inline uint8x8_t NeonLerp(uint8x8_t p, uint8x8_t q, uint8x8_t alpha)
{
    uint16x8_t mulRes = vmlal_u8(vdupq_n_u16(BASEMSB), alpha, vsub_u8(p, q));
    uint8x8_t result = vshrn_n_u16(mulRes, NEON_STEP_8);

    return vqadd_u8(p, vshrn_n_u16(vaddq_u16(vmovl_u8(result), mulRes), NEON_STEP_8));
}
// return vIn / 255
static inline uint8x8_t NeonFastDiv255(uint16x8_t vIn)
{
    // 257: 2^8 + 1; 8: number of shifts
    return vmovn_u16(vshrq_n_u16(vIn + vshrq_n_u16(vIn + vdupq_n_u16(257), 8), 8));
}

// return a * b / 255
static inline uint8x8_t NeonMulDiv255(uint8x8_t a, uint8x8_t b)
{
    return NeonFastDiv255(vmull_u8(a, b));
}

// return a / b
// a, b and result are floating-point numbers.
static inline float32x4_t NeonDiv(float32x4_t a, float32x4_t b)
{
    float32x4_t reciprocal = vrecpeq_f32(b);
    reciprocal = vmulq_f32(vrecpsq_f32(b, reciprocal), reciprocal);
    reciprocal = vmulq_f32(vrecpsq_f32(b, reciprocal), reciprocal);
    return vmulq_f32(a, reciprocal);
}

// return a / b
// a is a 16-bits integer, b and result are 8-bits integers.
static inline uint8x8_t NeonDivInt(uint16x8_t a, uint8x8_t b)
{
    float32x4_t low = NeonDiv(vcvtq_f32_u32(vmovl_u16(vget_low_u16(a))),
                              vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(b)))));
    float32x4_t high = NeonDiv(vcvtq_f32_u32(vmovl_u16(vget_high_u16(a))),
                               vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(b)))));
    return vmovn_u16(vcombine_u16(vmovn_u32(vcvtq_u32_f32(low)), vmovn_u32(vcvtq_u32_f32(high))));
}

static void NeonMemcpy(void* dst, int32_t dstSize, const void* src, int32_t srcSize)
{
    int32_t sz = MATH_MIN(dstSize, srcSize);
    // 64-bytes aligned
    int32_t mod = sz % 64;
    if (mod) {
        if (memcpy_s(dst, mod, src, mod) != EOK) {
            return;
        }
        sz -= mod;
        if (sz == 0) {
            return;
        }
        dst = (uint8_t*)dst + mod;
        src = (uint8_t*)src + mod;
    }

    asm volatile(
        "NEONCopyPLD: \n"
        " PLD [%[src], #0xC0] \n"
        " VLDM %[src]!, {d0-d7} \n"
        " VSTM %[dst]!, {d0-d7} \n"
        " SUBS %[sz], %[sz], #0x40 \n"
        " BGT NEONCopyPLD \n"
        : [dst] "+r"(dst), [src] "+r"(src), [sz] "+r"(sz)
        :
        : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "cc", "memory");
}

static inline void NeonBlendRGBA(uint8x8_t& r1, uint8x8_t& g1, uint8x8_t& b1, uint8x8_t& a1,
                                 uint8x8_t r2, uint8x8_t g2, uint8x8_t b2, uint8x8_t a2)
{
    uint8x8_t da = NeonMulDiv255(a1, vdup_n_u8(OPA_OPAQUE) - a2);
    a1 = a1 - NeonMulDiv255(a2, a1) + a2;
    uint16x8_t r = vmull_u8(r2, a2) + vmull_u8(r1, da);
    uint16x8_t g = vmull_u8(g2, a2) + vmull_u8(g1, da);
    uint16x8_t b = vmull_u8(b2, a2) + vmull_u8(b1, da);
    r1 = NeonDivInt(r, a1);
    g1 = NeonDivInt(g, a1);
    b1 = NeonDivInt(b, a1);
}

static inline void NeonBlendRGB(uint8x8_t& r1, uint8x8_t& g1, uint8x8_t& b1, uint8x8_t& a1,
                                uint8x8_t r2, uint8x8_t g2, uint8x8_t b2, uint8x8_t a2)
{
    uint8x8_t da = vdup_n_u8(OPA_OPAQUE) - a2;
    r1 = NeonMulDiv255(r2, a2) + NeonMulDiv255(r1, da);
    g1 = NeonMulDiv255(g2, a2) + NeonMulDiv255(g1, da);
    b1 = NeonMulDiv255(b2, a2) + NeonMulDiv255(b1, da);
}

static inline void LoadBuf_ARGB8888(uint8_t* buf, uint8x8_t& r, uint8x8_t& g, uint8x8_t& b, uint8x8_t& a)
{
    uint8x8x4_t vBuf = vld4_u8(buf);
    r = vBuf.val[NEON_R];
    g = vBuf.val[NEON_G];
    b = vBuf.val[NEON_B];
    a = vBuf.val[NEON_A];
}

static inline void LoadBuf_RGB888(uint8_t* buf, uint8x8_t& r, uint8x8_t& g, uint8x8_t& b, uint8x8_t& a)
{
    uint8x8x3_t vBuf = vld3_u8(buf);
    r = vBuf.val[NEON_R];
    g = vBuf.val[NEON_G];
    b = vBuf.val[NEON_B];
}

static inline void LoadBuf_RGB565(uint8_t* buf, uint8x8_t& r, uint8x8_t& g, uint8x8_t& b, uint8x8_t& a)
{
    uint16x8_t vBuf = vld1q_u16(reinterpret_cast<uint16_t*>(buf));
    // 3: RRRRRGGG|GGGBBBBB => RRGGGGGG|BBBBB000
    b = vmovn_u16(vshlq_n_u16(vBuf, 3));
    // 5, 2: RRRRRGGG|GGGBBBBB => XXXRRRRR|GGGGGG00
    g = vshl_n_u8(vshrn_n_u16(vBuf, 5), 2);
    // 11, 3: RRRRRGGG|GGGBBBBB => XXXXXXXX|RRRRR000
    r = vmovn_u16(vshlq_n_u16(vshrq_n_u16(vBuf, 11), 3));
}

static inline void LoadBufA_ARGB8888(uint8_t* buf,
                                     uint8x8_t& r,
                                     uint8x8_t& g,
                                     uint8x8_t& b,
                                     uint8x8_t& a,
                                     uint8_t opa)
{
    uint8x8x4_t vBuf = vld4_u8(buf);
    r = vBuf.val[NEON_R];
    g = vBuf.val[NEON_G];
    b = vBuf.val[NEON_B];
    a = NeonMulDiv255(vBuf.val[NEON_A], vdup_n_u8(opa));
}

static inline void LoadBufA_RGB888(uint8_t* buf,
                                   uint8x8_t& r,
                                   uint8x8_t& g,
                                   uint8x8_t& b,
                                   uint8x8_t& a,
                                   uint8_t opa)
{
    uint8x8x3_t vBuf = vld3_u8(buf);
    r = vBuf.val[NEON_R];
    g = vBuf.val[NEON_G];
    b = vBuf.val[NEON_B];
    a = vdup_n_u8(opa);
}

static inline void LoadBufA_RGB565(uint8_t* buf,
                                   uint8x8_t& r,
                                   uint8x8_t& g,
                                   uint8x8_t& b,
                                   uint8x8_t& a,
                                   uint8_t opa)
{
    uint16x8_t vBuf = vld1q_u16(reinterpret_cast<uint16_t*>(buf));
    // 3: RRRRRGGG|GGGBBBBB => RRGGGGGG|BBBBB000
    b = vmovn_u16(vshlq_n_u16(vBuf, 3));
    // 5, 2: RRRRRGGG|GGGBBBBB => XXXRRRRR|GGGGGG00
    g = vshl_n_u8(vshrn_n_u16(vBuf, 5), 2);
    // 11, 3: RRRRRGGG|GGGBBBBB => XXXXXXXX|RRRRR000
    r = vmovn_u16(vshlq_n_u16(vshrq_n_u16(vBuf, 11), 3));
    a = vdup_n_u8(opa);
}
static inline void SetPixelColor_ARGB8888(uint8_t* buf,
                                          const uint8_t& r,
                                          const uint8_t& g,
                                          const uint8_t& b,
                                          const uint8_t& a)
{
    uint8x8x4_t vBuf;
    vBuf.val[NEON_R] = vdup_n_u8(r);
    vBuf.val[NEON_G] = vdup_n_u8(g);
    vBuf.val[NEON_B] = vdup_n_u8(b);
    vBuf.val[NEON_A] = vdup_n_u8(a);
    vst4_u8(buf, vBuf);
}
static inline void SetPixelColor_ARGB8888(uint8_t* dstBuf, uint8_t* srcBuf)
{
    uint8x8x4_t vSrcBuf = vld4_u8(srcBuf);
    uint8x8x4_t vDstBuf;
    vDstBuf.val[NEON_R] = vSrcBuf.val[NEON_R];
    vDstBuf.val[NEON_G] = vSrcBuf.val[NEON_G];
    vDstBuf.val[NEON_B] = vSrcBuf.val[NEON_B];
    vDstBuf.val[NEON_A] = vSrcBuf.val[NEON_A];
    vst4_u8(dstBuf, vDstBuf);
}
static inline void StoreBuf_ARGB8888(uint8_t* buf, uint8x8_t& r, uint8x8_t& g, uint8x8_t& b, uint8x8_t& a)
{
    uint8x8x4_t vBuf;
    vBuf.val[NEON_R] = r;
    vBuf.val[NEON_G] = g;
    vBuf.val[NEON_B] = b;
    vBuf.val[NEON_A] = a;
    vst4_u8(buf, vBuf);
}

static inline void StoreBuf_RGB888(uint8_t* buf, uint8x8_t& r, uint8x8_t& g, uint8x8_t& b, uint8x8_t& a)
{
    uint8x8x3_t vBuf;
    vBuf.val[NEON_R] = r;
    vBuf.val[NEON_G] = g;
    vBuf.val[NEON_B] = b;
    vst3_u8(buf, vBuf);
}

static inline void StoreBuf_RGB565(uint8_t* buf, uint8x8_t& r, uint8x8_t& g, uint8x8_t& b, uint8x8_t& a)
{
    /* red left shift 8 bit.
     * vBuf => RRRRRXXX|XXXXXXXX
     */
    uint16x8_t vBuf = vshll_n_u8(r, 8);
    /* Keep the first 5 digits of vBuf, and splice it with vshll_n_u8(g, 8).
     * vBuf => RRRRRGGG|GGGXXXXX
     */
    vBuf = vsriq_n_u16(vBuf, vshll_n_u8(g, 8), 5);
    /* Keep the first 11 digits of vBuf, and splice it with vshll_n_u8(b, 8).
     * vBuf => RRRRRGGG|GGGBBBBB
     */
    vBuf = vsriq_n_u16(vBuf, vshll_n_u8(b, 8), 11);
    vst1q_u16(reinterpret_cast<uint16_t*>(buf), vBuf);
}
} // namespace OHOS
#endif
#endif
