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

#include "gfx_utils/graphic_math.h"
#if ENABLE_CMATH
#include <cmath>
#endif

namespace OHOS {
#if !ENABLE_CMATH
static float g_sinValues[] = {
    0.000000, 0.017452, 0.034899, 0.052336, 0.069756, 0.087156, 0.104528, 0.121869, 0.139173, 0.156434, 0.173648,
    0.190809, 0.207912, 0.224951, 0.241922, 0.258819, 0.275637, 0.292372, 0.309017, 0.325568, 0.342020, 0.358368,
    0.374607, 0.390731, 0.406737, 0.422618, 0.438371, 0.453990, 0.469472, 0.484810, 0.500000, 0.515038, 0.529919,
    0.544639, 0.559193, 0.573576, 0.587785, 0.601815, 0.615661, 0.629320, 0.642788, 0.656059, 0.669131, 0.681998,
    0.694658, 0.707107, 0.719340, 0.731354, 0.743145, 0.754710, 0.766044, 0.777146, 0.788011, 0.798636, 0.809017,
    0.819152, 0.829038, 0.838671, 0.848048, 0.857167, 0.866025, 0.874620, 0.882948, 0.891007, 0.898794, 0.906308,
    0.913545, 0.920505, 0.927184, 0.933580, 0.939693, 0.945519, 0.951057, 0.956305, 0.961262, 0.965926, 0.970296,
    0.974370, 0.978148, 0.981627, 0.984808, 0.987688, 0.990268, 0.992546, 0.994522, 0.996195, 0.997564, 0.998630,
    0.999391, 0.999848, 1.000000
};
#endif

float Sin(float angle)
{
#if ENABLE_CMATH
    float radian =  angle / RADIAN_TO_ANGLE;
    return sin(radian);
#else
    int16_t degree = static_cast<int16_t>(MATH_ROUND(angle));
    degree = degree % CIRCLE_IN_DEGREE;
    if (degree < 0) {
        degree = CIRCLE_IN_DEGREE + degree;
    }

    if (degree <= QUARTER_IN_DEGREE) {
        return g_sinValues[degree];
    } else if (degree <= SEMICIRCLE_IN_DEGREE) {
        return g_sinValues[SEMICIRCLE_IN_DEGREE - degree];
    } else if (degree <= THREE_QUARTER_IN_DEGREE) {
        return -g_sinValues[degree - SEMICIRCLE_IN_DEGREE];
    } else {
        return -g_sinValues[CIRCLE_IN_DEGREE - degree];
    }
#endif
}
float Fmod(float X, float Y)
{
    return X - (int)(X / Y) * Y;
}
float Cos(float angle)
{
#if ENABLE_CMATH
    return cos(angle / RADIAN_TO_ANGLE);
#else
    Sin(QUARTER_IN_DEGREE - angle);
#endif
}

float Acos(float value)
{
#if ENABLE_CMATH
    return acos(value);
#else
    float result, l, r;
    l = 0;
    r = UI_PI;
    result = (l + r) / 2;
    while (MATH_ABS(Sin(QUARTER_IN_DEGREE - result * RADIAN_TO_ANGLE) - value) > UI_FLT_EPSILON) {
        if (Sin(QUARTER_IN_DEGREE - result * RADIAN_TO_ANGLE) - value > UI_FLT_EPSILON) {
           l = result;
        } else {
           r = result;
        }
        result = (l + r) / 2;
    }
    return result;
#endif
}

/* arctan(x) = x - p3 * x^3 + p5 * x^5 - p7 * x^7 */
uint16_t FastAtan2(int16_t x, int16_t y)
{
    if (x == 0 && y == 0) {
        return 0;
    }

    int16_t absX = MATH_ABS(x);
    int16_t absY = MATH_ABS(y);
    float t;
    float t2;
    uint16_t angle;
    if (absX <= absY) {
        t = static_cast<float>(absX) / absY;
        t2 = t * t;
        angle = static_cast<uint16_t>(t * (1 + t2 * (ATAN2_P3 + t2 * (ATAN2_P5 + t2 * ATAN2_P7))) * RADIAN_TO_ANGLE);
    } else {
        t = static_cast<float>(absY) / absX;
        t2 = t * t;
        angle = QUARTER_IN_DEGREE -
            static_cast<uint16_t>(t * (1 + t2 * (ATAN2_P3 + t2 * (ATAN2_P5 + t2 * ATAN2_P7))) * RADIAN_TO_ANGLE);
    }

    if (y < 0) {
        if (x < 0) {
            angle = SEMICIRCLE_IN_DEGREE + angle;
        } else {
            angle = SEMICIRCLE_IN_DEGREE - angle;
        }
    } else if (x < 0) {
        angle = CIRCLE_IN_DEGREE - angle;
    }

    return angle;
}

float FastAtan2F(float y, float x)
{
    float absX = MATH_ABS(x);
    float absY = MATH_ABS(y);
    if (absY < UI_FLT_EPSILON && absX < UI_FLT_EPSILON){
        return 0;
    }
    float t;
    float t2;
    float angle;
    if (absX <= absY) {
        t = absX / absY;
        t2 = t * t;
        angle = UI_PI / 2 - (t * (1 + t2 * (ATAN2_P3 + t2 * (ATAN2_P5 + t2 * ATAN2_P7))));
    } else {
        t = (absY) / absX;
        t2 = t * t;
        angle = (t * (1 + t2 * (ATAN2_P3 + t2 * (ATAN2_P5 + t2 * ATAN2_P7))) );
    }

    if (y < 0) {
        if (x < 0) {
            angle = - UI_PI + angle;
        } else {
            angle = - angle;
        }
    } else if (x < 0) {
        angle =  UI_PI - angle;
    }

    return angle;
}

float Sqrt(float x)
{
    const float xhalf = 0.5f * x;
    int32_t i = *reinterpret_cast<int32_t*>(&x);
    i = 0x5f375a86 - (i / 2); // 0x5f375a86 : Initial value of Newton Iterator. 2 : initial parameter for iterator.
    float y = *reinterpret_cast<float*>(&i);
    y = y * (1.5f - (xhalf * y * y));
    y = y * (1.5f - (xhalf * y * y));
    y = y * (1.5f - (xhalf * y * y));
    return x * y;
}

bool IsIdentity(Matrix3<float>& matrix)
{
    // check m00 m11 m22
    if (!FloatEqual(matrix[0][0], 1) || !FloatEqual(matrix[1][1], 1) || !FloatEqual(matrix[2][2], 1)) {
        return false;
    }
    // check others
    if (!FloatEqual(matrix[0][1], 0) || !FloatEqual(matrix[0][2], 0) ||
        !FloatEqual(matrix[2][0], 0) || !FloatEqual(matrix[2][1], 0) ||
        !FloatEqual(matrix[1][0], 0) || !FloatEqual(matrix[1][2], 0)) {
        return false;
    }
    return true;
}

bool IsIdentity(Matrix4<float>& matrix)
{
    for (int16_t row = 0; row < ORDER_MATRIX_4; row++) {
        for (int16_t col = 0; col < ORDER_MATRIX_4; col++) {
            bool flag = (row == col) ? FloatEqual(matrix[row][col], 1) : FloatEqual(matrix[row][col], 0);
            if (!flag) {
                return false;
            }
        }
    }
    return true;
}
} // namespace OHOS
