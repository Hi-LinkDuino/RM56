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
 * @file fill_base.h
 * @brief Defines Scan line distributor and converter
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_FILL_BASE_H
#define GRAPHIC_LITE_FILL_BASE_H
#include "gfx_utils/color.h"
#include "gfx_utils/diagram/vertexprimitive/geometry_plaindata_array.h"
namespace OHOS {
class FillBase {
public:
    /**
     * @brief Resize Redistribution calculation spans_ length
     * @param span_len Scan line length
     * @return First address of spans
     */
    inline Rgba8T* Resize(uint32_t spanLen)
    {
        if (spanLen > spans_.GetSize()) {
            spans_.Resize(((spanLen + MAX_COLOR_NUM) >> BYTE_LENGTH) << BYTE_LENGTH);
        }
        return &spans_[0];
    }

    inline Rgba8T* GetSpanPtr()
    {
        if (spans_.GetSize() <= 0) {
            spans_.Resize(8);
        }
        return &spans_[0];
    }

private:
    GeometryPlainDataArray<Rgba8T> spans_;
};

class SpanBase {
public:
    virtual void Prepare() = 0;
    virtual  void Generate(Rgba8T* span, int32_t, int32_t, uint32_t len) = 0;
};

class SpanSoildColor : public SpanBase {
public:
    explicit SpanSoildColor(Rgba8T color)
    {
        color_ = color;
    }
    void Prepare() {}
    void Generate(Rgba8T* span, int32_t, int32_t, uint32_t len)
    {
        for (; len; --len) {
            *span++ = color_;
        }
    }
private:
    Rgba8T color_;
};

class SpanBlendColor : public SpanBase {
public:
    SpanBlendColor(SpanBase& spanGenerator1, SpanBase& spanGenerator2)
        : spanGenerator1_(spanGenerator1),
          spanGenerator2_(spanGenerator2)
    {
    }
    void Prepare()
    {
        spanGenerator1_.Prepare();
        spanGenerator2_.Prepare();
    }
    void Generate(Rgba8T* span, int32_t x, int32_t y, uint32_t len)
    {
        FillBase allocator1;
        FillBase allocator2;

        Rgba8T* colors1 = allocator1.Resize(len);
        Rgba8T* colors2 = allocator2.Resize(len);

        spanGenerator1_.Generate(colors1, x, y, len);
        spanGenerator2_.Generate(colors2, x, y, len);

        for (; len; --len, ++span, colors1++, colors2++) {
            span->red = (colors1->red+colors2->red) >= MAX_COLOR_NUM ?
                        MAX_COLOR_NUM:(colors1->red+colors2->red);
            span->green = (colors1->green+colors2->green) >= MAX_COLOR_NUM ?
                        MAX_COLOR_NUM:(colors1->green+colors2->green);
            span->blue = (colors1->blue+colors2->blue) >= MAX_COLOR_NUM ?
                        MAX_COLOR_NUM:(colors1->blue+colors2->blue);
            span->alpha = (colors1->alpha+colors2->alpha) >= MAX_COLOR_NUM ?
                        MAX_COLOR_NUM:(colors1->alpha+colors2->alpha);
        }
    }
private:
    SpanBase& spanGenerator1_;
    SpanBase& spanGenerator2_;
};
} // namespace OHOS
#endif
