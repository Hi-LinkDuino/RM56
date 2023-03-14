/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "utils/utils.h"

#include <cctype>
#include <cstddef>
#include <cstring>

#include "utils/common.h"

namespace OHOS {
namespace Global {
namespace Resource {
bool Utils::IsAlphaString(const char *s, int32_t len)
{
    if (s == nullptr) {
        return false;
    }
    int32_t i;
    for (i = 0; i < len; i++) {
        char c = *(s + i);
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
            return false;
        }
    }
    return true;
}

bool Utils::IsNumericString(const char *s, int32_t len)
{
    if (s == nullptr) {
        return false;
    }
    int32_t i;
    for (i = 0; i < len; i++) {
        char c = *(s + i);
        if (!(c >= '0' && c <= '9')) {
            return false;
        }
    }

    return true;
}

/**
 * @brief decode 32 bits as script array.
 * 31-24 bits is script[0]
 * 23-16 bits is script[1]
 * 15-8 bits is script[2]
 * 0-7 bits is script[3]
 *
 * @param encodeScript
 * @param outValue
 */
void Utils::DecodeScript(uint32_t encodeScript, char *outValue)
{
    if (outValue == nullptr) {
        return;
    }
    outValue[0] = (encodeScript & 0xFF000000) >> 24;
    outValue[1] = (encodeScript & 0x00FF0000) >> 16;
    outValue[2] = (encodeScript & 0x0000FF00) >> 8;
    outValue[3] = (encodeScript & 0x000000FF);
}

bool Utils::IsStrEmpty(const char *s)
{
    return (s == nullptr || *s == '\0');
}

size_t Utils::StrLen(const char *s)
{
    if (s == nullptr) {
        return 0;
    }
    return strlen(s);
}

uint16_t Utils::EncodeLanguage(const char *language)
{
    if (Utils::IsStrEmpty(language)) {
        return NULL_LANGUAGE;
    }
    return Utils::EncodeLanguageOrRegion(language, 'a');
}

/**
 * @brief  locale compose of language,script and region,encode as 64bits.
 * 63-48 bits represent language,detail format see EncodeLanguageOrRegion method
 * 47-16 bits represent script,detail format see EncodeScript method
 * 15-0 bits represent region,detail format see EncodeLanguageOrRegion method
 *
 * @param language
 * @param script
 * @param region
 * @return uint64_t
 */
uint64_t Utils::EncodeLocale(const char *language,
                             const char *script,
                             const char *region)
{
    uint16_t languageData = Utils::EncodeLanguage(language);
    uint32_t scriptData = Utils::EncodeScript(script);
    uint16_t regionData = Utils::EncodeRegion(region);

    return (uint64_t)(0xffff000000000000 & (((uint64_t)languageData) << 48)) |
           (0x0000ffffffff0000 & (((uint64_t)scriptData) << 16)) | (0x000000000000ffff & (uint64_t)(regionData));
}

uint16_t Utils::EncodeRegionByResLocale(const ResLocale *locale)
{
    if (locale == nullptr) {
        return NULL_REGION;
    }
    return Utils::EncodeRegion(locale->GetRegion());
}

uint16_t Utils::EncodeLanguageByResLocale(const ResLocale *locale)
{
    if (locale == nullptr) {
        return NULL_LANGUAGE;
    }
    return Utils::EncodeLanguage(locale->GetLanguage());
}

uint32_t Utils::EncodeScriptByResLocale(const ResLocale *locale)
{
    if (locale == nullptr) {
        return NULL_SCRIPT;
    }
    return Utils::EncodeScript(locale->GetScript());
}

uint16_t Utils::EncodeRegion(const char *region)
{
    if (Utils::IsStrEmpty(region)) {
        return NULL_REGION;
    }
    if (region[0] >= '0' && region[0] <= '9') {
        return Utils::EncodeLanguageOrRegion(region, '0');
    }
    return Utils::EncodeLanguageOrRegion(region, 'A');
}

/**
 * @brief script is four letter array.encode script array as four bytes.Encode format.
 * 31-24 bits represent script[0]
 * 23-16 bits represent script[1]
 * 15-8 bits represent script[2]
 * 0-7 bits represent script[3]
 *
 * @param script
 * @return uint32_t
 */
uint32_t Utils::EncodeScript(const char *script)
{
    if (Utils::IsStrEmpty(script)) {
        return NULL_SCRIPT;
    }
    return ((uint8_t)script[0] << 24) | ((uint8_t)script[1] << 16) | ((uint8_t)script[2] << 8) | (uint8_t)script[3];
}

/**
 * @brief encode language or region str as two byte.
 * language is two or three lowercase.
 * region is two capital  letter or three digit.
 *
 * two char,encode format
 * 15-8 bits is the first char
 * 7-0 bits is the second char
 *
 * three chars,encode format
 * 15 bit is 1
 * 14-10 bits represent the value of  the first char subtract base char,
 * 9-5 bits represent the value of the second char subtract base char  .
 * 4-0 bits represent the value of the third char subtract base char.
 * base char is 'a','A','0'.
 * example when base is 'a',max value('z' - 'a') is 26,so five bits can represent a char.
 *
 * @param str
 * @param base is '0' or 'a' or 'A'
 * @return uint16_t
 */
uint16_t Utils::EncodeLanguageOrRegion(const char *str, char base)
{
    if (str[2] == 0 || str[2] == '-' || str[2] == '_') {
        return ((uint8_t)str[0] << 8) | ((uint8_t)str[1]);
    }
    uint8_t first = ((uint8_t)(str[0] - base)) & 0x7f;
    uint8_t second = ((uint8_t)(str[1] - base)) & 0x7f;
    uint8_t third = ((uint8_t)(str[2] - base)) & 0x7f;
    return ((0x80 | (first << 2) | (second >> 3)) << 8) | ((second << 5) | third);
};

bool Utils::StrCompare(const char *left, const char *right, size_t len, bool isCaseSensitive)
{
    if (left == nullptr && right == nullptr) {
        return true;
    }
    if (left == nullptr || right == nullptr) {
        return false;
    }
    int rc;
    unsigned char c1, c2;
    while (len--) {
        c1 = (unsigned char)*left;
        c2 = (unsigned char)*right;
        if (c1 == 0 && c2 == 0) {
            return true;
        }
        if (c1 == 0 || c2 == 0) {
            return false;
        }
        if (isCaseSensitive) {
            rc = (int)(c1) - (int)(c2);
        } else {
            rc = tolower(c1) - tolower(c2);
        }
        if (rc != 0) {
            return false;
        }
        ++left;
        ++right;
    }
    return true;
}

/**
 * @brief convert hex char as int value
 *
 * @param c
 * @param state
 * @return uint32_t
 */
static uint32_t ParseHex(char c, RState &state)
{
    if (c >= '0' && c <= '9') {
        return (c - '0');
    } else if (c >= 'a' && c <= 'f') {
        return (c - 'a' + 0xa);
    } else if (c >= 'A' && c <= 'F') {
        return (c - 'A' + 0xa);
    }
    state = INVALID_FORMAT;

    return -1;
}

/**
 * @brief  convert color string to 32 bits value 0xaarrggbb.
 * color string format is
 * #rgb  red (0-f) greed(0-f) blue(0-f)
 * #argb transparency(0-f)  red (0-f) greed(0-f) blue(0-f)
 * #rrggbb red (00-ff) greed(00-ff) blue(00-ff)
 * #aarrggbb transparency(00-ff) red (00-ff) greed(00-ff) blue(00-ff)
 *
 * @param s
 * @param outValue
 * @return RState
 */
RState Utils::ConvertColorToUInt32(const char *s, uint32_t &outValue)
{
    if (s == nullptr) {
        return INVALID_FORMAT;
    }
    uint32_t color = 0;
    RState parseState = SUCCESS;
    size_t len = strlen(s);
    if (*s == '#') {
        if (len == 4) { // 4 means #rgb
            color |= 0xFF000000;
            color |= ParseHex(s[1], parseState) << 20;
            color |= ParseHex(s[1], parseState) << 16;
            color |= ParseHex(s[2], parseState) << 12;
            color |= ParseHex(s[2], parseState) << 8;
            color |= ParseHex(s[3], parseState) << 4;
            color |= ParseHex(s[3], parseState);
        } else if (len == 5) { // 5 means #argb
            color |= ParseHex(s[1], parseState) << 28;
            color |= ParseHex(s[1], parseState) << 24;
            color |= ParseHex(s[2], parseState) << 20;
            color |= ParseHex(s[2], parseState) << 16;
            color |= ParseHex(s[3], parseState) << 12;
            color |= ParseHex(s[3], parseState) << 8;
            color |= ParseHex(s[4], parseState) << 4;
            color |= ParseHex(s[4], parseState);
        } else if (len == 7) { // 7 means #rrggbb
            color |= 0xFF000000;
            color |= ParseHex(s[1], parseState) << 20;
            color |= ParseHex(s[2], parseState) << 16;
            color |= ParseHex(s[3], parseState) << 12;
            color |= ParseHex(s[4], parseState) << 8;
            color |= ParseHex(s[5], parseState) << 4;
            color |= ParseHex(s[6], parseState);
        } else if (len == 9) { // 9 means #aarrggbb
            color |= ParseHex(s[1], parseState) << 28;
            color |= ParseHex(s[2], parseState) << 24;
            color |= ParseHex(s[3], parseState) << 20;
            color |= ParseHex(s[4], parseState) << 16;
            color |= ParseHex(s[5], parseState) << 12;
            color |= ParseHex(s[6], parseState) << 8;
            color |= ParseHex(s[7], parseState) << 4;
            color |= ParseHex(s[8], parseState);
        }
    } else {
        parseState = INVALID_FORMAT;
    }
    outValue = color;
    return parseState;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
