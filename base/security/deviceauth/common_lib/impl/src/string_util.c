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

#include "string_util.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "securec.h"
#include "clib_error.h"
#include "clib_types.h"

#define OUT_OF_HEX 16
#define NUMBER_9_IN_DECIMAL 9
#define ASCII_CASE_DIFFERENCE_VALUE 32
#define DESENSITIZATION_LEN 4

static const char * const g_base64CharacterTable =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const uint8_t g_base64DecodeTable[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0,
    0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

static char HexToChar(uint8_t hex)
{
    return (hex > NUMBER_9_IN_DECIMAL) ? (hex + 0x37) : (hex + 0x30); /* Convert to the corresponding character */
}

int32_t ByteToHexString(const uint8_t *byte, uint32_t byteLen, char *hexStr, uint32_t hexLen)
{
    if (byte == NULL || hexStr == NULL) {
        return CLIB_ERR_NULL_PTR;
    }
    /* The terminator('\0') needs 1 bit */
    if (hexLen < byteLen * BYTE_TO_HEX_OPER_LENGTH + 1) {
        return CLIB_ERR_INVALID_LEN;
    }

    for (uint32_t i = 0; i < byteLen; i++) {
        hexStr[i * BYTE_TO_HEX_OPER_LENGTH] = HexToChar((byte[i] & 0xF0) >> 4); /* 4: shift right for filling */
        hexStr[i * BYTE_TO_HEX_OPER_LENGTH + 1] = HexToChar(byte[i] & 0x0F); /* get low four bits */
    }
    hexStr[byteLen * BYTE_TO_HEX_OPER_LENGTH] = '\0';

    return CLIB_SUCCESS;
}

static uint8_t CharToHex(char c)
{
    if ((c >= 'A') && (c <= 'F')) {
        return (c - 'A' + DEC);
    } else if ((c >= 'a') && (c <= 'f')) {
        return (c - 'a' + DEC);
    } else if ((c >= '0') && (c <= '9')) {
        return (c - '0');
    } else {
        return OUT_OF_HEX;
    }
}

int32_t HexStringToByte(const char *hexStr, uint8_t *byte, uint32_t byteLen)
{
    if (byte == NULL || hexStr == NULL) {
        return CLIB_ERR_NULL_PTR;
    }
    uint32_t realHexLen = strlen(hexStr);
    /* even number or not */
    if (realHexLen % BYTE_TO_HEX_OPER_LENGTH != 0 || byteLen < realHexLen / BYTE_TO_HEX_OPER_LENGTH) {
        return CLIB_ERR_INVALID_LEN;
    }

    for (uint32_t i = 0; i < realHexLen / BYTE_TO_HEX_OPER_LENGTH; i++) {
        uint8_t high = CharToHex(hexStr[i * BYTE_TO_HEX_OPER_LENGTH]);
        uint8_t low = CharToHex(hexStr[i * BYTE_TO_HEX_OPER_LENGTH + 1]);
        if (high == OUT_OF_HEX || low == OUT_OF_HEX) {
            return CLIB_ERR_INVALID_PARAM;
        }
        byte[i] = high << 4; /* 4: Set the high nibble */
        byte[i] |= low; /* Set the low nibble */
    }
    return CLIB_SUCCESS;
}

int64_t StringToInt64(const char *cp)
{
    if (cp == NULL) {
        return 0;
    }
    return strtoll(cp, NULL, DEC);
}

void ConvertToAnonymousStr(const char *originalStr, char **anonymousStr)
{
    if ((originalStr == NULL) || (anonymousStr == NULL)) {
        return;
    }
    uint32_t desensitizationLen = DESENSITIZATION_LEN;
    uint32_t len = strlen(originalStr);
    if (len <= desensitizationLen) {
        return;
    }
    *anonymousStr = (char *)ClibMalloc(len + 1, 0);
    if ((*anonymousStr) == NULL) {
        return;
    }
    if (memset_s(*anonymousStr, len + 1, '*', len) != EOK) {
        ClibFree(*anonymousStr);
        *anonymousStr = NULL;
        return;
    }
    if (memcpy_s(*anonymousStr, len + 1, originalStr, len - desensitizationLen) != EOK) {
        ClibFree(*anonymousStr);
        *anonymousStr = NULL;
        return;
    }
}

static bool IsInvalidBase64Character(char c)
{
    if (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')) {
        return false;
    }
    if ('0' <= c && c <= '9') {
        return false;
    }
    if (c == '+' || c == '/') {
        return false;
    }
    return true;
}

int32_t Base64StringToByte(const char *base64Str, uint8_t *byte, uint32_t *byteLen)
{
    if (base64Str == NULL || byte == NULL || byteLen == NULL) {
        return CLIB_ERR_NULL_PTR;
    }
    uint32_t strLen = strlen(base64Str);
    if (strLen < BYTE_TO_BASE64_MULTIPLIER) {
        return CLIB_ERR_INVALID_LEN;
    }
    uint32_t len = strLen / BYTE_TO_BASE64_MULTIPLIER * BYTE_TO_BASE64_DIVISOR;
    int j = 0;
    for (int i = 0; i < 2; i++) { // at most two end fillings '='
        if (base64Str[strLen - 1 - i] == '=') {
            j++;
        }
    }
    if (len - j > (*byteLen)) {
        return CLIB_ERR_INVALID_LEN;
    }
    *byteLen = len - j;

    // 6 bits each character(first 2 bits pad 0), 4 characters as a group to decode
    if (IsInvalidBase64Character(base64Str[0]) || IsInvalidBase64Character(base64Str[1]) ||
        IsInvalidBase64Character(base64Str[2])) {
        return CLIB_ERR_INVALID_PARAM;
    }
    for (uint32_t i = 0, j = 0; i < strLen - 2; j += 3, i += 4) {
        if (IsInvalidBase64Character(base64Str[i + 3]) && i + 3 < strLen - j) {
            return CLIB_ERR_INVALID_PARAM;
        }
        /* splice the last 6 bits of the first character's value and the first 2 bits of the second character's value */
        byte[j] = (g_base64DecodeTable[(int)base64Str[i]] << 2) | (g_base64DecodeTable[(int)base64Str[i + 1]] >> 4);
        /* splice the last 4 bits of the second character's value and the first 4 bits of the third character's value */
        byte[j + 1] = (g_base64DecodeTable[(int)base64Str[i + 1]] << 4) |
            (g_base64DecodeTable[(int)base64Str[i + 2]] >> 2);
        /* splice the last 2 bits of the third character's value and the first 6 bits of the forth character's value */
        byte[j + 2] = (g_base64DecodeTable[(int)base64Str[i + 2]] << 6) | (g_base64DecodeTable[(int)base64Str[i + 3]]);
    }
    return CLIB_SUCCESS;
}

int32_t ByteToBase64String(const uint8_t *byte, uint32_t byteLen, char *base64Str, uint32_t strLen)
{
    if (byte == NULL || base64Str == NULL) {
        return CLIB_ERR_NULL_PTR;
    }
    if (byteLen > (UINT32_MAX / BYTE_TO_BASE64_MULTIPLIER - 1) * BYTE_TO_BASE64_DIVISOR) {
        return CLIB_ERR_INVALID_LEN;
    }
    uint32_t len = (byteLen / BYTE_TO_BASE64_DIVISOR + (byteLen % BYTE_TO_BASE64_DIVISOR != 0)) *
        BYTE_TO_BASE64_MULTIPLIER;
    if (len + 1 > strLen) {
        return CLIB_ERR_INVALID_LEN;
    }

    uint32_t i;
    uint32_t j;
    // 3 bytes as a group to encode
    for (i = 0, j = 0; i < len - 2; j += 3, i += 4) {
        /* take the first 6 bits of the first byte to map to base64 character */
        base64Str[i] = g_base64CharacterTable[byte[j] >> 2];
        /*
         * splice the last 2 bits of the first byte and the first 4 bits of the second byte,
         * and map to base64 character
         */
        base64Str[i + 1] = g_base64CharacterTable[((byte[j] & 0x3) << 4) | (byte[j + 1] >> 4)];
        /*
         * splice the last 4 bits of the second byte and the first 2 bits of the third byte,
         * and map to base64 character
         */
        base64Str[i + 2] = g_base64CharacterTable[((byte[j + 1] & 0xf) << 2) | (byte[j + 2] >> 6)];
        /* take the last 6 bits of the third byte to map to base64 character */
        base64Str[i + 3] = g_base64CharacterTable[byte[j + 2] & 0x3f];
    }

    // the lack of position fills '='
    if (byteLen % 3 == 1) {
        base64Str[i - 2] = '=';
        base64Str[i - 1] = '=';
    } else if (byteLen % 3 == 2) {
        base64Str[i - 1] = '=';
    }
    base64Str[len] = '\0';

    return CLIB_SUCCESS;
}

int32_t ToUpperCase(const char *oriStr, char **desStr)
{
    if (oriStr == NULL || desStr == NULL) {
        return CLIB_ERR_NULL_PTR;
    }
    uint32_t len = strlen(oriStr);
    *desStr = ClibMalloc(len + 1, 0);
    if (*desStr == NULL) {
        return CLIB_ERR_BAD_ALLOC;
    }
    for (uint32_t i = 0; i < len; i++) {
        if ((oriStr[i] >= 'a') && (oriStr[i] <= 'f')) {
            (*desStr)[i] = oriStr[i] - ASCII_CASE_DIFFERENCE_VALUE;
        } else {
            (*desStr)[i] = oriStr[i];
        }
    }
    return CLIB_SUCCESS;
}