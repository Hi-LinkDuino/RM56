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
 * @file fill_gradient.h
 * @brief Defines Scanline gradient mode
 * @since 1.0
 * @version 1.0
 */


#ifndef GRAPHIC_LITE_FILL_GRADIENT_H
#define GRAPHIC_LITE_FILL_GRADIENT_H


#include "gfx_utils/color.h"
#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/diagram/spancolorfill/fill_interpolator.h"
#include "gfx_utils/diagram/spancolorfill/fill_gradient_lut.h"
#include "gfx_utils/graphic_math.h"
#include "fill_base.h"
namespace OHOS {
class Gradient {
public:
    virtual int16_t  Calculate(int16_t x, int16_t y, int16_t startRadius, int16_t endRadius, int16_t size) = 0;
};

/**
 * Gradient scanline fill
 * @ColorT Color source type, Interpolator span interpolator
 * @GradientFgradient_function The function of
 * the corresponding mode to calculate the position of the current point
 * @ColorF Color array
 */
class FillGradient : public SpanBase {
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
public:
    FillGradient() {}
    /**
     * @brief SpanGradient Constructor for scanline gradient
     * @param inter Interpolator
     * @param GradientFunction
     * The function of the corresponding mode to calculate the position of the current point
     * @param ColorFunction Gradient array
     * @param distance1 The parameter content is determined according to the mode:
     * it is the starting circle radius when radiating the gradient
     * @param distance2 Determine the parameter content according to the mode:
     * the radius of the end circle in the case of radial gradient and
     * the distance of the starting and ending points in the case of linear gradient
     */
    FillGradient(FillInterpolator& inter, Gradient& GradientFunction,
                 FillGradientLut& ColorFunction, float distance1, float distance2)
        : interpolator_(&inter),
          gradientFunction_(&GradientFunction),
          colorFunction_(&ColorFunction),
          distance1_(distance1 * GRADIENT_SUBPIXEL_SCALE),
          distance2_(distance2 * GRADIENT_SUBPIXEL_SCALE) {}

    void Prepare() {}

    /**
     * @brief Generate From colorfunction_ Remove the rgba from the span
     * @param span First address of scan line to be filled
     * @param x coordinate-x
     * @param y coordinate-y
     * @param len Scan line length
     */
    void Generate(Rgba8T* span, int32_t x, int32_t y, uint32_t len)
    {
        int32_t downscaleShift = FillInterpolator::SUBPIXEL_SHIFT - GRADIENT_SUBPIXEL_SHIFT;
        interpolator_->Begin(x, y, len);
        for (; len; --len, ++(*interpolator_), *span++) {
            interpolator_->Coordinates(&x, &y);
            int32_t index = gradientFunction_->Calculate(x >> downscaleShift, y >> downscaleShift,
                                                         distance1_, distance2_, colorFunction_->GetSize());
            span->red = (*colorFunction_)[index].red;
            span->green = (*colorFunction_)[index].green;
            span->blue = (*colorFunction_)[index].blue;
            span->alpha = (*colorFunction_)[index].alpha;
        }
    }
private:
    FillInterpolator* interpolator_;
    Gradient* gradientFunction_;
    FillGradientLut* colorFunction_;
    int32_t distance1_;
    int32_t distance2_;
};


/**
 * GradientFunction
 * @brief The subscript of the current (x, y) ColorFunction array when calculating the radial gradient
 * @since 1.0
 * @version 1.0
 */
class GradientRadialCalculate : public virtual Gradient {
public:
    GradientRadialCalculate()
        : endRadius_(HUNDRED_TIMES * GRADIENT_SUBPIXEL_SCALE), dx_(0), dy_(0)
    {
        UpdateValues();
    }

    /**
     * @brief GradientRadialCalculate Constructor arguments
     * @param endRadius End circle radius
     * @param dx In the x-axis direction, the distance from the end circle center to the start circle center
     * @param dy In the y-axis direction, the distance from the end circle center to the start circle center
     */
    GradientRadialCalculate(float endRadius, float dx, float dy)
        : endRadius_(endRadius * GRADIENT_SUBPIXEL_SCALE),
          dx_(dx * GRADIENT_SUBPIXEL_SCALE),
          dy_(dy * GRADIENT_SUBPIXEL_SCALE)
    {
        UpdateValues();
    }

    /**
     * @brief calculate The subscript of the current (x, y) ColorFunction array
     * when calculating the radial gradient
     * @param x coordinate x
     * @param y coordinate y
     * @param startRadius Start circle radius
     * @param endRadius End circle radius
     * @param size colorFunction_µÄsize
     * @return
     */
    int16_t  Calculate(int16_t x, int16_t y, int16_t startRadius, int16_t endRadius, int16_t size)
    {
        float dx = x - dx_;
        float dy = y - dy_;
        float distanceRadius = dx * dy_ - dy * dx_;
        float radiusDistance = endRadiusSquare_ * (dx * dx + dy * dy)
                - distanceRadius * distanceRadius;
        float deltaRadius = endRadius - startRadius; // Difference of radius
        if (deltaRadius < 1) {
            deltaRadius = 1;
        }
        int16_t index = (((dx * dx_ + dy * dy_ +
                        Sqrt(fabs(radiusDistance)))
                        * mul_ - startRadius) * size) / deltaRadius;
        if (index < 0) {
            index = 0;
        }
        if (index >= size) {
            index = size - 1;
        }
        return index;
    }

private:
    /**
     * @brief update mul_
     */
    void UpdateValues()
    {
        endRadiusSquare_ = float(endRadius_) * float(endRadius_);
        float dxSquare_ = float(dx_) * float(dx_);
        float dySquare_ = float(dy_) * float(dy_);
        float dRadius = (endRadiusSquare_ - (dxSquare_ + dySquare_));
        if (dRadius == 0.0f) {
            if (dx_) {
                if (dx_ < 0.0f) {
                    ++dx_;
                } else {
                    --dx_;
                }
            }
            if (dy_) {
                if (dy_ < 0.0f) {
                    ++dy_;
                } else {
                    --dy_;
                }
            }
            dxSquare_ = float(dx_) * float(dx_);
            dySquare_ = float(dy_) * float(dy_);
            dRadius = (endRadiusSquare_ - (dxSquare_ + dySquare_));
        }
        mul_ = endRadius_ / dRadius;
    }

    int32_t endRadius_;
    /** In the x-axis direction, the distance from the end circle center to the start circle center */
    int32_t dx_;
    /** In the y-axis direction, the distance from the end circle center to the start circle center */
    int32_t dy_;
    float endRadiusSquare_;
    float mul_;
};

/**
 * @brief The subscript of the current (x, y) ColorFunction array when calculating a linear gradient
 * @since 1.0
 * @version 1.0
 */
class GradientLinearCalculate : public virtual Gradient {
public:
    /**
     * @brief The subscript of the current (x, y) ColorFunction array when calculating a linear gradient
     * @param x coordinate x
     * @param distance Distance between start and end of linear gradient
     * @param size color_functionµÄsize
     * @return
     */
    int16_t Calculate(int16_t x, int16_t, int16_t, int16_t distance, int16_t size)
    {
        if (distance < 1) {
            distance = 1;
        }
        int16_t index = (x * size) / distance;
        if (index < 0) {
            index = 0;
        }
        if (index >= size) {
            index = size - 1;
        }
        return index;
    }
#endif
};
} // namespace OHOS
#endif
