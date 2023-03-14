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
 * @file fill_interpolator.h
 * @brief Defines Scan line interpolator
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_FILL_INTERPOLATOR_H
#define GRAPHIC_LITE_FILL_INTERPOLATOR_H

#include "gfx_utils/color.h"
#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/trans_affine.h"
#include "gfx_utils/diagram/vertexprimitive/geometry_dda_line.h"
#include "gfx_utils/graphic_math.h"
namespace OHOS {
const uint8_t SUB_PIXEL_SHIFT = 8;
/**
 * Gradient color interpolator
 */
struct ColorInterpolator {
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
public:

    ColorInterpolator(const Rgba8T& color1, const Rgba8T& color2, uint32_t distance)
        : colorStart_(color1), colorEnd_(color2), len_(distance), place_(0) {}

    /**
     * @brief overwrite ++
     */
    void operator++()
    {
        ++place_;
    }

    /**
     * @brief Returns the color at count during the transition from colorstart to colorend
     * @return
     */
    Rgba8T GetColor() const
    {
        return colorStart_.Gradient(colorEnd_, float(place_) / len_);
    }

private:
    Rgba8T colorStart_;
    Rgba8T colorEnd_;
    uint32_t len_;
    uint32_t place_;
#endif
};

/**
 * Linear scan line inserter
 */
class FillInterpolator {
public:
    enum SubpixelScale {
        SUBPIXEL_SHIFT = SUB_PIXEL_SHIFT,
        SUBPIXEL_SCALE = 1 << SUBPIXEL_SHIFT
    };
    FillInterpolator() {}
    FillInterpolator(TransAffine& trans) : transType_(&trans) {}
    FillInterpolator(TransAffine& trans,
                           float x, float y, uint32_t len) : transType_(&trans)
    {
        Begin(x, y, len);
    }

    const TransAffine& GetTransformer() const
    {
        return *transType_;
    }
    void SetTransformer(TransAffine& trans)
    {
        transType_ = &trans;
    }

    /*
     * Update and set dda2lineinterpolatorx and dda2lineinterpolatory properties again
     */
    void Begin(float x, float y, uint32_t len)
    {
        float tx;
        float ty;

        tx = x;
        ty = y;

        transType_->Transform(&tx, &ty);
        int32_t x1 = MATH_ROUND32(tx * SUBPIXEL_SCALE);
        int32_t y1 = MATH_ROUND32(ty * SUBPIXEL_SCALE);

        tx = x + len;
        ty = y;
        transType_->Transform(&tx, &ty);
        int32_t x2 = MATH_ROUND32(tx * SUBPIXEL_SCALE);
        int32_t y2 = MATH_ROUND32(ty * SUBPIXEL_SCALE);

        dda2LineInterpolatorX_ = GeometryDdaLine(x1, x2, len);
        dda2LineInterpolatorY_ = GeometryDdaLine(y1, y2, len);
    }

    /**
     * @brief Overloading + + operators
     */
    void operator++()
    {
        ++dda2LineInterpolatorX_;
        ++dda2LineInterpolatorY_;
    }

    void Coordinates(int32_t* x, int32_t* y) const
    {
        *x = dda2LineInterpolatorX_.GetCoordinate();
        *y = dda2LineInterpolatorY_.GetCoordinate();
    }

private:
    TransAffine* transType_;
    GeometryDdaLine dda2LineInterpolatorX_;
    GeometryDdaLine dda2LineInterpolatorY_;
};
} // namespace OHOS
#endif
