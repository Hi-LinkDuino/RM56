/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "print_log.h"

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LINE_BYTE_COUNT 16
#define FORMAT_PRINT_BUFFER LOGD

#define CHAR_ZERO '0'
#define CHAR_A 'A'
#define NUM_ZERO 0
#define NUM_ONE 1
#define NUM_TWO 2
#define NUM_THREE 3
#define NUM_FOUR 4
#define NUM_NINE 9
#define NUM_TEN 10
#define NUM_FIFTEEN 15
#define HALF_BYTE_MASK 0x0F
#define NUM_PRINTABLE_BEGIN 0x20
#define NUM_PRINTABLE_END 0x7E
#define BITS_PER_BYTE 8
#define BITS_PER_HEX 4

static char Int2char(uint8_t c)
{
    // c >= NUM_ZERO comparison is always true due to limited range of data type
    if (c <= NUM_NINE) {
        return (char)(c + CHAR_ZERO);
    } else if (c >= NUM_TEN && c <= NUM_FIFTEEN) {
        return (char)(c - NUM_TEN + CHAR_A);
    } else {
        return 0;
    }
}

static void PrintLine(const uint8_t *ptr, int32_t len, uint32_t offset)
{
    int i;
    char x1;
    char x2;
    char str[NUM_NINE + LINE_BYTE_COUNT * NUM_FOUR + NUM_ONE] = {0};
    char *out = str + NUM_NINE;
    uint32_t t;

    for (i = 0; i < BITS_PER_BYTE; i++) {
        t = offset << (i * BITS_PER_HEX);
        t = t >> ((BITS_PER_BYTE - 1) * BITS_PER_HEX);
        str[i] = Int2char(t);
    }
    str[BITS_PER_BYTE] = ' ';
    for (i = 0; i < len; i++) {
        uint8_t c = ptr[i];
        x1 = Int2char(c >> NUM_FOUR);
        x2 = Int2char(c & HALF_BYTE_MASK);
        out[i * NUM_THREE] = x1;
        out[i * NUM_THREE + NUM_ONE] = x2;
        out[i * NUM_THREE + NUM_TWO] = ' ';
    }
    for (i = len; i < LINE_BYTE_COUNT; i++) {
        out[i * NUM_THREE] = out[i * NUM_THREE + NUM_ONE] = out[i * NUM_THREE + NUM_TWO] = ' ';
    }
    for (i = 0; i < len; i++) {
        if (ptr[i] >= NUM_PRINTABLE_BEGIN && ptr[i] <= NUM_PRINTABLE_END) {
            out[LINE_BYTE_COUNT * NUM_THREE + i] = ptr[i];
        } else {
            out[LINE_BYTE_COUNT * NUM_THREE + i] = '.';
        }
    }
    FORMAT_PRINT_BUFFER("%s", str);
}

void PrintBuffer(const char *tag, const uint8_t *ptr, int32_t len)
{
    int i = 0;
    const uint8_t *linePtr = ptr;
    uint32_t offset = 0;

    FORMAT_PRINT_BUFFER("PrintBuffer tag = %s, len = %d, 0x%x", tag, len, len);
    while (i < len) {
        int currentLineByteCount = (int)(len - i);
        if (currentLineByteCount >= LINE_BYTE_COUNT) {
            PrintLine(linePtr, LINE_BYTE_COUNT, offset);
            i += LINE_BYTE_COUNT;
        } else {
            PrintLine(linePtr, currentLineByteCount, offset);
            i += currentLineByteCount;
        }
        linePtr += LINE_BYTE_COUNT;
        offset += LINE_BYTE_COUNT;
    }
}

#ifdef __cplusplus
}
#endif
