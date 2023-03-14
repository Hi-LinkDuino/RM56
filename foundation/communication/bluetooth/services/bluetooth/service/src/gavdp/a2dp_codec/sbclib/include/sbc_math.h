/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef SBC_MATH_H
#define SBC_MATH_H

#include <cstddef>
#include <cstdint>

static inline int32_t FABS(int32_t x)
{
    return ((x) < 0 ? -(x) : (x));
}
static inline int32_t ASR(int32_t val, int bits)
{
    return ((-2 >> 1 == -1) ?
        (static_cast<int32_t>(val)) >> (bits) : (static_cast<int32_t>(val)) / (1 << (bits)));
}

#define SCALE_SPROTO4_TBL    12
#define SCALE_SPROTO8_TBL    14
#define SCALE_NPROTO4_TBL    11
#define SCALE_NPROTO8_TBL    11
#define SCALE4_STAGED1_BITS    15
#define SCALE4_STAGED2_BITS    16
#define SCALE8_STAGED1_BITS    15
#define SCALE8_STAGED2_BITS    16

static inline int32_t Scale4Staged1(int32_t src)
{
    return ((-2 >> 1 == -1) ?
        (static_cast<int32_t>(src)) >> (SCALE4_STAGED1_BITS) :
            (static_cast<int32_t>(src)) / (1 << (SCALE4_STAGED1_BITS)));
}
static inline int32_t Scale8Staged1(int32_t src)
{
    return ((-2 >> 1 == -1) ?
        (static_cast<int32_t>(src)) >> (SCALE8_STAGED1_BITS) :
            (static_cast<int32_t>(src)) / (1 << (SCALE8_STAGED1_BITS)));
}

static inline int32_t MUL(int32_t a, int32_t b)
{
    return a * b;
}

static inline int32_t MULA(int32_t a, int32_t b, int32_t res)
{
    return (a * b + res);
}
#endif // SBC_MATH_H