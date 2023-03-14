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

#include "softbus_bitmap.h"
#include <stddef.h>

#define UINT32_BITNUM 32

void SoftbusBitmapSet(uint32_t *bitmap, const uint8_t pos)
{
    if (bitmap == NULL || pos >= UINT32_BITNUM) {
        return;
    }
    *bitmap |= 1U << pos;
}

void SoftbusBitmapClr(uint32_t *bitmap, const uint8_t pos)
{
    if (bitmap == NULL || pos >= UINT32_BITNUM) {
        return;
    }
    *bitmap &= ~(1U << pos);
}

void SoftbusBitmapSetBits(uint32_t *bitmap, const uint8_t start, const uint8_t nums)
{
    if (bitmap == NULL || start > UINT32_BITNUM || (start + nums) > UINT32_BITNUM || nums == 0) {
        return;
    }
    *bitmap |= (UINT32_MAX - ((((1U << ((start + nums - 1U))) - 1U)) & (~((1U << ((start - 1U))) - 1U))));
}

void SoftbusBitmapClrBits(uint32_t *bitmap, const uint8_t start, const uint8_t nums)
{
    if (bitmap == NULL || start > UINT32_BITNUM || (start + nums) > UINT32_BITNUM || nums == 0) {
        return;
    }
    *bitmap &= ((((1U << ((start + nums - 1U))) - 1U)) & (~((1U << ((start - 1U))) - 1U)));
}

uint8_t SoftbusLowBitGet(uint32_t bitmap)
{
    uint8_t pos = 0;
    while (bitmap > 0) {
        pos++;
        if (bitmap & 0X1) {
            return pos;
        }
        bitmap = bitmap >> 1;
    }
    return 0;
}

uint8_t SoftbusHighBitGet(uint32_t bitmap)
{
    uint8_t pos = 0;
    while (bitmap > 0) {
        bitmap = bitmap >> 1U;
        pos++;
    }
    return pos;
}

bool SoftbusIsBitmapSet(const uint32_t *bitmap, const uint8_t pos)
{
    if (bitmap == NULL || pos >= UINT32_BITNUM) {
        return false;
    }
    bool flag = ((1U << pos) & (*bitmap)) ? true : false;
    return flag;
}