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

#ifndef KERNEL_MATH_UTILS
#define KERNEL_MATH_UTILS

#include <float.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int Equalf(float a, float b)
{
    return fabsf(a - b) <= FLT_EPSILON;
}

static inline int Equal(double a, double b)
{
    return fabs(a - b) <= DBL_EPSILON;
}

static inline int Equall(long double a, long double b)
{
    return fabsl(a - b) <= LDBL_EPSILON;
}

#ifdef __cplusplus
}
#endif

#endif