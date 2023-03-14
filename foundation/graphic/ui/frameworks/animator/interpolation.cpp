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

#include "animator/interpolation.h"

#include "gfx_utils/graphic_math.h"

namespace OHOS {
/* B(t) = P0*(1-t)^3 + 3*P1*t*(1-t)^2 + 3*P2*t^2*(1-t) + P3*t^3 */
int16_t Interpolation::GetBezierInterpolation(int16_t t, int16_t u0, int16_t u1, int16_t u2, int16_t u3)
{
    int64_t invT = 1024 - t; // Intergerlize the standard equation, 1.0f is divided into 1024 parts
    int64_t invT2 = invT * invT;
    int64_t invT3 = invT2 * invT;
    int64_t t2 = t * t;
    int64_t t3 = t2 * t;

    int64_t ret = invT3 * u0;
    ret += BEZIER_COEFFICIENT * invT2 * t * u1;
    ret += BEZIER_COEFFICIENT * invT * t2 * u2;
    ret += t3 * u3;

    uint64_t uret = (ret < 0) ? (-ret) : ret;
    int16_t value = static_cast<int16_t>(uret >> 30); // 30: cubic shift
    return (ret < 0) ? (-value) : value;
}

/* B(t) = P0*(1-t)^3 + 3*P1*t*(1-t)^2 + 3*P2*t^2*(1-t) + P3*t^3 */
float Interpolation::GetBezierInterpolation(float t, float u0, float u1, float u2, float u3)
{
    float invT = 1 - t;
    float invT2 = invT * invT;
    float invT3 = invT2 * invT;
    float t2 = t * t;
    float t3 = t2 * t;

    float ret = invT3 * u0;
    ret += BEZIER_COEFFICIENT * invT2 * t * u1;
    ret += BEZIER_COEFFICIENT * invT * t2 * u2;
    ret += t3 * u3;
    return ret;
}

/* B(t) = 3(P1-P0)(1-t)^2 + 6(P2-P1)t(1-t) + 3(P3-P2)t^2 */
float Interpolation::GetBezierDerivative(float t, float u0, float u1, float u2, float u3)
{
    float invT = 1 - t;
    float d0 = u1 - u0;
    float d1 = u2 - u1;
    float d2 = u3 - u2;
    constexpr int8_t BESSEL_SQUARE_COEFFICIENT = (BEZIER_COEFFICIENT - 1) * BEZIER_COEFFICIENT;

    float ret = BEZIER_COEFFICIENT * d0 * invT * invT;
    ret += BESSEL_SQUARE_COEFFICIENT * d1 * invT * t;
    ret += BEZIER_COEFFICIENT * d2 * t * t;
    return ret;
}

float Interpolation::GetBezierY(float x, float x1, float y1, float x2, float y2)
{
    /* P={x,y}; P0={0,0}; P1={x1,y1}; P2={x2,y2}; P3={1,1}
     * P = P0*(1-t)^3 + 3*P1*t*(1-t)^2 + 3*P2*t^2*(1-t) + P3*t^3
     */
    float t = x;
    float xt = GetBezierInterpolation(t, 0, x1, x2, 1);
    /* Attention: precision must be carefully selected
     * too small may lead to misconvergence and a decrease of performance
     * too large may cause the curve rugged even make some points outlier */
    constexpr float PRECISION = 0.05f; // 0.05f make several outliers near inflection point
    int8_t iterationCnt = 10;          // iterate at most 10 times

    /* Newton Method to solve t from x */
    while ((MATH_ABS(xt - x) > PRECISION) && (iterationCnt-- > 0)) {
        t = t + (x - xt) / GetBezierDerivative(t, 0, x1, x2, 1);
        xt = GetBezierInterpolation(t, 0, x1, x2, 1);
    }
    return GetBezierInterpolation(t, 0, y1, y2, 1);
}
} // namespace OHOS
