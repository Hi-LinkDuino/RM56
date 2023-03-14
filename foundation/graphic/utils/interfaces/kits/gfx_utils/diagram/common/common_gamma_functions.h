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
 * @file common_gamma_functions.h
 *
 * @brief Gamma correction method
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_COMMON_GAMMA_FUNCTIONS_H
#define GRAPHIC_LITE_COMMON_GAMMA_FUNCTIONS_H

#include "gfx_utils/diagram/common/common_basics.h"

namespace OHOS {
const float LINEAR_VALUE = 0.0031308f;
const float SRGB_VALUE = 0.04045f;
const float CARDINAL_NUMBER = 12.92f;
const float BENCH_MARK = 0.055f;
const float BASE_VALUE = 1.055f;
const float POW_VALUE = 2.4f;

/**
 * @brief From linear type to sRGB type
 * @param Linearvalue is of linear type
 * @return Returns a color value of type sRGB
 * @since 1.0
 * @version 1.0
 */
inline float LinearToSrgb(float linearValue)
{
    return (linearValue <= LINEAR_VALUE) ?
               (linearValue * CARDINAL_NUMBER) :
               (BASE_VALUE * pow(linearValue, 1 / POW_VALUE) - BENCH_MARK);
}

/**
 * @brief From sRGB type to linear type
 * @param Srgbvalue is sRGB type
 * @return Returns a color value of type linear
 * @since 1.0
 * @version 1.0
 */
inline float SrgbToLinear(float srgbValue)
{
    return (srgbValue <= SRGB_VALUE) ?
               (srgbValue / CARDINAL_NUMBER) :
               pow((srgbValue + BENCH_MARK) / (BASE_VALUE), POW_VALUE);
}
} // namespace OHOS
#endif
