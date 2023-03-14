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

/**
 * @addtogroup UI_Animator
 * @{
 *
 * @brief Defines UI animation effects and provides matched curves.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file interpolation.h
 *
 * @brief Defines the functions for calculating the interpolation in computer graphics.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_INTERPOLATION_H
#define GRAPHIC_LITE_INTERPOLATION_H

#include "gfx_utils/heap_base.h"

namespace OHOS {
/**
 * @brief Calculates the Bezier interpolation.
 *
 * @since 1.0
 * @version 1.0
 */
class Interpolation : public HeapBase {
public:
    /**
     * @brief Obtains the value calculated by the cubic Bezier equation.
     *
     * Use [0, 1024] instead of [0, 1] in the standard Bezier equation. The cubic Bezier equation
     * is <b>B(t) = P0*(1-t)^3 + 3*P1*t*(1-t)^2 + 3*P2*t^2*(1-t) + P3*t^3</b>.
     *
     * @param t  Indicates the current change rate of the cubic Bezier curve, within [0, 1024].
     * @param u0 Indicates the coordinates for the start point of the cubic Bezier curve, within [0, 1024].
     * @param u1 Indicates the coordinates for the first control point of the cubic Bezier curve, within [0, 1024].
     * @param u2 Indicates the coordinates for the second control point of the cubic Bezier curve, within [0, 1024].
     * @param u3 Indicates the coordinates for the end point of the cubic Bezier curve, within [0, 1024].
     *
     * @return Returns the coordinates for the current change rate.
     * @since 3
     */
    static int16_t GetBezierInterpolation(int16_t t, int16_t u0, int16_t u1, int16_t u2, int16_t u3);

    /**
     * @brief Obtains the value calculated by the cubic Bezier equation.
     *
     * The standard cubic Bezier equation is <b>B(t) = P0*(1-t)^3 + 3*P1*t*(1-t)^2 + 3*P2*t^2*(1-t) + P3*t^3</b>.
     *
     * @param t  Indicates the current change rate of the cubic Bezier curve, within [0, 1].
     * @param u0 Indicates the coordinates for the start point of the cubic Bezier curve, within [0, 1].
     * @param u1 Indicates the coordinates for the first control point of the cubic Bezier curve, within [0, 1].
     * @param u2 Indicates the coordinates for the second control point of the cubic Bezier curve, within [0, 1].
     * @param u3 Indicates the coordinates for the end point of the cubic Bezier curve, within [0, 1].
     *
     * @return Returns the coordinates for the current change rate.
     * @since 6
     */
    static float GetBezierInterpolation(float t, float u0, float u1, float u2, float u3);

    /**
     * @brief Obtains the value calculated by the cubic Bezier equation in standard two-dimensional coordinate system.
     *
     * The cubic Bezier equation is <b>B(t) = P0*(1-t)^3 + 3*P1*t*(1-t)^2 + 3*P2*t^2*(1-t) + P3*t^3</b>.
     * <b>P0=(0,0), P3=(1,1)</b>
     *
     * @param x  Indicates the abscissa of P, within [0, 1].
     * @param x1 Indicates the abscissa of the first control point of the cubic Bezier curve, within [0, 1].
     * @param y1 Indicates the ordinate of the first control point of the cubic Bezier curve, within [0, 1].
     * @param x2 Indicates the abscissa of the second control point of the cubic Bezier curve, within [0, 1].
     * @param y2 Indicates the ordinate of the second control point of the cubic Bezier curve, within [0, 1].
     *
     * @return Returns the ordinate of the P.
     * @since 6
     */
    static float GetBezierY(float x, float x1, float y1, float x2, float y2);

private:
    static float GetBezierDerivative(float t, float u0, float u1, float u2, float u3);
    static constexpr uint8_t BEZIER_COEFFICIENT = 3;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_INTERPOLATION_H
