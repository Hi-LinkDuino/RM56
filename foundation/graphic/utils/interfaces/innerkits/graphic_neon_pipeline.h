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

#ifndef GRAPHIC_LITE_GRAPHIC_NEON_PIPELINE_H
#define GRAPHIC_LITE_GRAPHIC_NEON_PIPELINE_H

#include "graphic_config.h"
#ifdef ARM_NEON_OPT
#include <arm_neon.h>

#include "gfx_utils/color.h"
#include "graphic_neon_utils.h"

namespace OHOS {
using LoadBuf = void (*)(uint8_t* buf, uint8x8_t& r, uint8x8_t& g, uint8x8_t& b, uint8x8_t& a);
using LoadBufA = void (*)(uint8_t* buf, uint8x8_t& r, uint8x8_t& g, uint8x8_t& b, uint8x8_t& a, uint8_t opa);
using NeonBlend = void (*)(uint8x8_t& r1, uint8x8_t& g1, uint8x8_t& b1, uint8x8_t& a1,
                           uint8x8_t r2, uint8x8_t g2, uint8x8_t b2, uint8x8_t a2);
using StoreBuf = void (*)(uint8_t* buf, uint8x8_t& r, uint8x8_t& g, uint8x8_t& b, uint8x8_t& a);

struct {
    ColorMode dm;
    LoadBuf loadDstFunc;
    NeonBlend blendFunc;
    StoreBuf storeDstFunc;
}
g_dstFunc[] = {
    {ARGB8888, LoadBuf_ARGB8888, NeonBlendRGBA, StoreBuf_ARGB8888},
    {RGB888, LoadBuf_RGB888, NeonBlendRGB, StoreBuf_RGB888},
    {RGB565, LoadBuf_RGB565, NeonBlendRGB, StoreBuf_RGB565}
};

struct {
    ColorMode sm;
    LoadBufA loadSrcFunc;
}
g_srcFunc[] = {
    {ARGB8888, LoadBufA_ARGB8888},
    {RGB888, LoadBufA_RGB888},
    {RGB565, LoadBufA_RGB565}
};

class NeonBlendPipeLine {
public:
    NeonBlendPipeLine() {}
    ~NeonBlendPipeLine() {}

    void Construct(ColorMode dm, ColorMode sm, void* srcColor = nullptr, uint8_t opa = OPA_OPAQUE)
    {
        int16_t dstNum = sizeof(g_dstFunc) / sizeof(g_dstFunc[0]);
        for (int16_t i = 0; i < dstNum; ++i) {
            if (g_dstFunc[i].dm == dm) {
                loadDstFunc_ = g_dstFunc[i].loadDstFunc;
                blendFunc_ = g_dstFunc[i].blendFunc;
                storeDstFunc_ = g_dstFunc[i].storeDstFunc;
                break;
            }
        }
        int16_t srcNum = sizeof(g_srcFunc) / sizeof(g_srcFunc[0]);
        for (int16_t i = 0; i < srcNum; ++i) {
            if (g_srcFunc[i].sm == sm) {
                loadSrcFunc_ = g_srcFunc[i].loadSrcFunc;
                break;
            }
        }
        if (srcColor != nullptr) {
            ConstructSrcColor(sm, srcColor, opa, r2_, g2_, b2_, a2_);
        }
    }

    void Invoke(uint8_t* dst, uint8_t* src, uint8_t opa)
    {
        loadDstFunc_(dst, r1_, g1_, b1_, a1_);
        loadSrcFunc_(src, r2_, g2_, b2_, a2_, opa);
        blendFunc_(r1_, g1_, b1_, a1_, r2_, g2_, b2_, a2_);
        storeDstFunc_(dst, r1_, g1_, b1_, a1_);
    }

    void Invoke(uint8_t* dst)
    {
        loadDstFunc_(dst, r1_, g1_, b1_, a1_);
        blendFunc_(r1_, g1_, b1_, a1_, r2_, g2_, b2_, a2_);
        storeDstFunc_(dst, r1_, g1_, b1_, a1_);
    }

    void Invoke(uint8_t* dst, uint8x8_t& r, uint8x8_t& g, uint8x8_t& b, uint8x8_t& a)
    {
        loadDstFunc_(dst, r1_, g1_, b1_, a1_);
        blendFunc_(r1_, g1_, b1_, a1_, r, g, b, a);
        storeDstFunc_(dst, r1_, g1_, b1_, a1_);
    }

    void NeonPreLerpARGB8888(uint8_t* buf, uint8_t r, uint8_t g, uint8_t b, uint8_t a, uint8_t* covers)
    {
        uint8x8x4_t vBuf = vld4_u8(buf);
        uint8x8_t r0 = vBuf.val[NEON_R];
        uint8x8_t g0 = vBuf.val[NEON_G];
        uint8x8_t b0 = vBuf.val[NEON_B];
        uint8x8_t a0 = vBuf.val[NEON_A];

        uint8x8_t r1 = Multipling(vdup_n_u8(r), vld1_u8(covers));
        uint8x8_t g1 = Multipling(vdup_n_u8(g), vld1_u8(covers));
        uint8x8_t b1 = Multipling(vdup_n_u8(b), vld1_u8(covers));
        uint8x8_t a1 = Multipling(vdup_n_u8(a), vld1_u8(covers));

        uint8x8_t rs = NeonLerp(r0, r1, a1);
        uint8x8_t gs = NeonLerp(g0, g1, a1);
        uint8x8_t bs = NeonLerp(b0, b1, a1);
        uint8x8_t as = NeonPreLerp(a0, a1, a1);

        StoreBuf_ARGB8888(buf, rs, gs, bs, as);
    }
    void NeonPrelerpARGB8888(uint8_t* buf, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
        uint8x8x4_t vBuf = vld4_u8(buf);
        uint8x8_t r0 = vBuf.val[NEON_R];
        uint8x8_t g0 = vBuf.val[NEON_G];
        uint8x8_t b0 = vBuf.val[NEON_B];
        uint8x8_t a0 = vBuf.val[NEON_A];

        uint8x8_t r1 = vdup_n_u8(red);
        uint8x8_t g1 = vdup_n_u8(green);
        uint8x8_t b1 = vdup_n_u8(blue);
        uint8x8_t a1 = vdup_n_u8(alpha);

        uint8x8_t rs = NeonPreLerp(r0, r1, a1);
        uint8x8_t gs = NeonPreLerp(g0, g1, a1);
        uint8x8_t bs = NeonPreLerp(b0, b1, a1);
        uint8x8_t as = NeonPreLerp(a0, a1, a1);

        StoreBuf_ARGB8888(buf, rs, gs, bs, as);
    }

    void NeonPrelerpARGB8888(uint8_t* buf, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t cover)
    {
        uint8x8x4_t vBuf = vld4_u8(buf);
        uint8x8_t r0 = vBuf.val[NEON_R];
        uint8x8_t g0 = vBuf.val[NEON_G];
        uint8x8_t b0 = vBuf.val[NEON_B];
        uint8x8_t a0 = vBuf.val[NEON_A];

        uint8x8_t r1 = Multipling(vdup_n_u8(red), vdup_n_u8(cover));
        uint8x8_t g1 = Multipling(vdup_n_u8(green), vdup_n_u8(cover));
        uint8x8_t b1 = Multipling(vdup_n_u8(blue), vdup_n_u8(cover));
        uint8x8_t a1 = Multipling(vdup_n_u8(alpha), vdup_n_u8(cover));

        uint8x8_t rs = NeonPreLerp(r0, r1, a1);
        uint8x8_t gs = NeonPreLerp(g0, g1, a1);
        uint8x8_t bs = NeonPreLerp(b0, b1, a1);
        uint8x8_t as = NeonPreLerp(a0, a1, a1);

        StoreBuf_ARGB8888(buf, rs, gs, bs, as);
    }

    void NeonPrelerpARGB8888(uint8_t* dstBuffer, uint8_t* srcBuffer, uint8_t cover)
    {
        uint8x8x4_t vDstBuf = vld4_u8(dstBuffer);
        uint8x8_t r0 = vDstBuf.val[NEON_R];
        uint8x8_t g0 = vDstBuf.val[NEON_G];
        uint8x8_t b0 = vDstBuf.val[NEON_B];
        uint8x8_t a0 = vDstBuf.val[NEON_A];
        uint8x8x4_t vSrcBuf = vld4_u8(srcBuffer);
        uint8x8_t r1 = vSrcBuf.val[NEON_R];
        uint8x8_t g1 = vSrcBuf.val[NEON_G];
        uint8x8_t b1 = vSrcBuf.val[NEON_B];
        uint8x8_t a1 = vSrcBuf.val[NEON_A];

        r1 = Multipling(r1, vdup_n_u8(cover));
        g1 = Multipling(g1, vdup_n_u8(cover));
        b1 = Multipling(b1, vdup_n_u8(cover));
        a1 = Multipling(a1, vdup_n_u8(cover));

        uint8x8_t rs = NeonPreLerp(r0, r1, a1);
        uint8x8_t gs = NeonPreLerp(g0, g1, a1);
        uint8x8_t bs = NeonPreLerp(b0, b1, a1);
        uint8x8_t as = NeonPreLerp(a0, a1, a1);

        StoreBuf_ARGB8888(dstBuffer, rs, gs, bs, as);
    }

    void NeonPrelerpARGB8888(uint8_t* dstBuffer, uint8_t* srcBuffer, uint8_t* covers)
    {
        uint8x8x4_t vDstBuf = vld4_u8(dstBuffer);
        uint8x8_t r0 = vDstBuf.val[NEON_R];
        uint8x8_t g0 = vDstBuf.val[NEON_G];
        uint8x8_t b0 = vDstBuf.val[NEON_B];
        uint8x8_t a0 = vDstBuf.val[NEON_A];

        uint8x8x4_t vSrcBuf = vld4_u8(srcBuffer);

        uint8x8_t r1 = Multipling(vSrcBuf.val[NEON_R], vld1_u8(covers));
        uint8x8_t g1 = Multipling(vSrcBuf.val[NEON_G], vld1_u8(covers));
        uint8x8_t b1 = Multipling(vSrcBuf.val[NEON_B], vld1_u8(covers));
        uint8x8_t a1 = Multipling(vSrcBuf.val[NEON_A], vld1_u8(covers));

        uint8x8_t rs = NeonPreLerp(r0, r1, a1);
        uint8x8_t gs = NeonPreLerp(g0, g1, a1);
        uint8x8_t bs = NeonPreLerp(b0, b1, a1);
        uint8x8_t as = NeonPreLerp(a0, a1, a1);

        StoreBuf_ARGB8888(dstBuffer, rs, gs, bs, as);
    }
    void NeonLerpARGB8888(uint8_t* buf, uint8_t r, uint8_t g, uint8_t b, uint8_t a,
                           uint8_t* covers)
    {
        uint8x8x4_t vBuf = vld4_u8(buf);
        uint8x8_t r0 = vBuf.val[NEON_R];
        uint8x8_t g0 = vBuf.val[NEON_G];
        uint8x8_t b0 = vBuf.val[NEON_B];
        uint8x8_t a0 = vBuf.val[NEON_A];

        uint8x8_t r1 = Multipling(vdup_n_u8(r), vld1_u8(covers));
        uint8x8_t g1 = Multipling(vdup_n_u8(g), vld1_u8(covers));
        uint8x8_t b1 = Multipling(vdup_n_u8(b), vld1_u8(covers));
        uint8x8_t a1 = Multipling(vdup_n_u8(a), vld1_u8(covers));

        uint8x8_t rs = NeonLerp(r0, r1, a1);
        uint8x8_t gs = NeonLerp(g0, g1, a1);
        uint8x8_t bs = NeonLerp(b0, b1, a1);
        uint8x8_t as = NeonPreLerp(a0, a1, a1);

        StoreBuf_ARGB8888(buf, rs, gs, bs, as);
    }
    void NeonLerpARGB8888(uint8_t* buf, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        uint8x8x4_t vBuf = vld4_u8(buf);
        uint8x8_t r0 = vBuf.val[NEON_R];
        uint8x8_t g0 = vBuf.val[NEON_G];
        uint8x8_t b0 = vBuf.val[NEON_B];
        uint8x8_t a0 = vBuf.val[NEON_A];

        uint8x8_t r1 = vdup_n_u8(r);
        uint8x8_t g1 = vdup_n_u8(g);
        uint8x8_t b1 = vdup_n_u8(b);
        uint8x8_t a1 = vdup_n_u8(a);

        uint8x8_t rs = NeonLerp(r0, r1, a1);
        uint8x8_t gs = NeonLerp(g0, g1, a1);
        uint8x8_t bs = NeonLerp(b0, b1, a1);
        uint8x8_t as = NeonPreLerp(a0, a1, a1);

        StoreBuf_ARGB8888(buf, rs, gs, bs, as);
    }

    void NeonLerpARGB8888(uint8_t* buf, uint8_t r, uint8_t g, uint8_t b, uint8_t a, uint8_t cover)
    {
        uint8x8x4_t vBuf = vld4_u8(buf);
        uint8x8_t r0 = vBuf.val[NEON_R];
        uint8x8_t g0 = vBuf.val[NEON_G];
        uint8x8_t b0 = vBuf.val[NEON_B];
        uint8x8_t a0 = vBuf.val[NEON_A];

        uint8x8_t r1 = Multipling(vdup_n_u8(r), vdup_n_u8(cover));
        uint8x8_t g1 = Multipling(vdup_n_u8(g), vdup_n_u8(cover));
        uint8x8_t b1 = Multipling(vdup_n_u8(b), vdup_n_u8(cover));
        uint8x8_t a1 = Multipling(vdup_n_u8(a), vdup_n_u8(cover));

        uint8x8_t rs = NeonLerp(r0, r1, a1);
        uint8x8_t gs = NeonLerp(g0, g1, a1);
        uint8x8_t bs = NeonLerp(b0, b1, a1);
        uint8x8_t as = NeonPreLerp(a0, a1, a1);

        StoreBuf_ARGB8888(buf, rs, gs, bs, as);
    }

    void NeonLerpARGB8888(uint8_t* dstBuffer, uint8_t* srcBuffer, uint8_t cover)
    {
        uint8x8x4_t vDstBuf = vld4_u8(dstBuffer);
        uint8x8_t r0 = vDstBuf.val[NEON_R];
        uint8x8_t g0 = vDstBuf.val[NEON_G];
        uint8x8_t b0 = vDstBuf.val[NEON_B];
        uint8x8_t a0 = vDstBuf.val[NEON_A];
        uint8x8x4_t vSrcBuf = vld4_u8(srcBuffer);
        uint8x8_t r1 = vSrcBuf.val[NEON_R];
        uint8x8_t g1 = vSrcBuf.val[NEON_G];
        uint8x8_t b1 = vSrcBuf.val[NEON_B];
        uint8x8_t a1 = vSrcBuf.val[NEON_A];

        r1 = Multipling(r1, vdup_n_u8(cover));
        g1 = Multipling(g1, vdup_n_u8(cover));
        b1 = Multipling(b1, vdup_n_u8(cover));
        a1 = Multipling(a1, vdup_n_u8(cover));

        uint8x8_t rs = NeonLerp(r0, r1, a1);
        uint8x8_t gs = NeonLerp(g0, g1, a1);
        uint8x8_t bs = NeonLerp(b0, b1, a1);
        uint8x8_t as = NeonPreLerp(a0, a1, a1);

        StoreBuf_ARGB8888(dstBuffer, rs, gs, bs, as);
    }

    void NeonLerpARGB8888(uint8_t* dstBuffer, uint8_t* srcBuffer, uint8_t* covers)
    {
        uint8x8x4_t vDstBuf = vld4_u8(dstBuffer);
        uint8x8_t r0 = vDstBuf.val[NEON_R];
        uint8x8_t g0 = vDstBuf.val[NEON_G];
        uint8x8_t b0 = vDstBuf.val[NEON_B];
        uint8x8_t a0 = vDstBuf.val[NEON_A];

        uint8x8x4_t vSrcBuf = vld4_u8(srcBuffer);

        uint8x8_t r1 = Multipling(vSrcBuf.val[NEON_R], vld1_u8(covers));
        uint8x8_t g1 = Multipling(vSrcBuf.val[NEON_G], vld1_u8(covers));
        uint8x8_t b1 = Multipling(vSrcBuf.val[NEON_B], vld1_u8(covers));
        uint8x8_t a1 = Multipling(vSrcBuf.val[NEON_A], vld1_u8(covers));

        uint8x8_t rs = NeonLerp(r0, r1, a1);
        uint8x8_t gs = NeonLerp(g0, g1, a1);
        uint8x8_t bs = NeonLerp(b0, b1, a1);
        uint8x8_t as = NeonPreLerp(a0, a1, a1);

        StoreBuf_ARGB8888(dstBuffer, rs, gs, bs, as);
    }
private:
    void ConstructSrcColor(ColorMode sm, void* srcColor, uint8_t opa,
                           uint8x8_t& r, uint8x8_t& g, uint8x8_t& b, uint8x8_t& a)
    {
        if (sm == ARGB8888) {
            Color32* color = reinterpret_cast<Color32*>(srcColor);
            r = vdup_n_u8(color->red);
            g = vdup_n_u8(color->green);
            b = vdup_n_u8(color->blue);
            a = NeonMulDiv255(vdup_n_u8(opa), vdup_n_u8(color->alpha));
        } else if (sm == RGB888) {
            Color24* color = reinterpret_cast<Color24*>(srcColor);
            r = vdup_n_u8(color->red);
            g = vdup_n_u8(color->green);
            b = vdup_n_u8(color->blue);
            a = vdup_n_u8(opa);
        } else if (sm == RGB565) {
            Color16* color = reinterpret_cast<Color16*>(srcColor);
            r = vdup_n_u8(color->red);
            g = vdup_n_u8(color->green);
            b = vdup_n_u8(color->blue);
            a = vdup_n_u8(opa);
        }
    }

    LoadBuf loadDstFunc_ = nullptr;
    LoadBufA loadSrcFunc_ = nullptr;
    NeonBlend blendFunc_ = nullptr;
    StoreBuf storeDstFunc_ = nullptr;
    uint8x8_t r1_;
    uint8x8_t g1_;
    uint8x8_t b1_;
    uint8x8_t a1_;
    uint8x8_t r2_;
    uint8x8_t g2_;
    uint8x8_t b2_;
    uint8x8_t a2_;
};
} // namespace OHOS
#endif
#endif
