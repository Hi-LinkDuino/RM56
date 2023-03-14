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

#include "unicode_ex.h"

#include <climits>
#include <cstdio>
#include <cstdlib>

#include "utils_log.h"
using namespace std;
/***************************************UTF8 and UTF16 unicode**********************************************
UTF8
Unicode                                 utf8
U + 0000~U + 007F                    0???????
U + 0080~U + 07FF                    110????? 10??????
U + 0800~U + FFFF                    1110???? 10?????? 10??????
U + 10000~U + 10FFFF                 11110??? 10?????? 10?????? 10??????

UTF16
Unicode                           utf16 code

U + 000~U + FFFF                    2 Byte save, same with Unicode
U + 10000~U + 10FFFF                4 Byte save Unicode 0x10000
**************************************UTF8 and UTF16 unicode**********************************************/
namespace OHOS {
namespace {
constexpr char32_t ONE_BYTE_UTF8 = 0x00000080;
constexpr char32_t TWO_BYTES_UTF8 = 0x00000800;
constexpr char32_t THREE_BYTES_UTF8 = 0x00010000;


constexpr char32_t UNICODE_RESERVED_START = 0x0000D800;
constexpr char32_t UNICODE_RESERVED_END = 0x0000DFFF;
constexpr char32_t UNICODE_MAX_NUM = 0x0010FFFF;
constexpr unsigned int UTF8_OFFSET = 6;

constexpr char32_t UTF8_BYTE_MASK = 0x000000BF;
constexpr char32_t UTF8_BYTE_MARK = 0x00000080;
constexpr char32_t UTF8_FIRST_BYTE_MARK[] = {
    0x00000000, 0x00000000, 0x000000C0, 0x000000E0, 0x000000F0
};
}

// inner func and dstP is not nullptr
void Utf32CodePointToUtf8(uint8_t* dstP, char32_t srcChar, size_t bytes)
{
    dstP += bytes;
    if (bytes >= 4) {
        *--dstP = (uint8_t)((srcChar | UTF8_BYTE_MARK) & UTF8_BYTE_MASK);
        srcChar >>= UTF8_OFFSET;
    }

    if (bytes >= 3) {
        *--dstP = (uint8_t)((srcChar | UTF8_BYTE_MARK) & UTF8_BYTE_MASK);
        srcChar >>= UTF8_OFFSET;
    }

    if (bytes >= 2) {
        *--dstP = (uint8_t)((srcChar | UTF8_BYTE_MARK) & UTF8_BYTE_MASK);
        srcChar >>= UTF8_OFFSET;
    }

    if (bytes >= 1) {
        *--dstP = (uint8_t)(srcChar | UTF8_FIRST_BYTE_MARK[bytes]);
    }
}

size_t Utf32CodePointUtf8Length(char32_t srcChar)
{
    if (srcChar < ONE_BYTE_UTF8) {
        return 1;
    } else if (srcChar < TWO_BYTES_UTF8) {
        return 2;
    } else if (srcChar < THREE_BYTES_UTF8) {
        if ((srcChar < UNICODE_RESERVED_START) || (srcChar > UNICODE_RESERVED_END)) {
            return 3;
        } else {
            // Surrogates are invalid UTF-32 characters.
            return 0;
        }
    } else if (srcChar <= UNICODE_MAX_NUM) {
        // Max code point for Unicode is 0x0010FFFF.
        return 4;
    } else {
        // Invalid UTF-32 character.
        return 0;
    }
}

// get the length of utf8 from utf16
int Utf16ToUtf8Length(const char16_t* str16, size_t str16Len)
{
    if (str16 == nullptr || str16Len == 0) {
        return -1;
    }

    const char16_t* const str16End = str16 + str16Len;
    int utf8Len = 0;
    while (str16 < str16End) {
        int charLen = 0;
        if (((*str16 & 0xFC00) == 0xD800) && ((str16 + 1) < str16End)
            && ((*(str16 + 1) & 0xFC00) == 0xDC00)) {
            // surrogate pairs are always 4 bytes.
            charLen = 4;
            str16 += 2;
        } else {
            charLen = Utf32CodePointUtf8Length((char32_t)* str16++);
        }

        if (utf8Len > (INT_MAX - charLen)) {
            return -1;
        }
        utf8Len += charLen;
    }
    return utf8Len;
}

// inner function, utf8Str and utf16Str is not nullptr
void StrncpyStr16ToStr8(const char16_t* utf16Str, size_t str16Len, char* utf8Str, size_t str8Len)
{
    const char16_t* curUtf16 = utf16Str;
    const char16_t* const endUtf16 = utf16Str + str16Len;
    char* cur = utf8Str;
    while (curUtf16 < endUtf16) {
        char32_t utf32;
        // surrogate pairs
        if (((*curUtf16 & 0xFC00) == 0xD800) && ((curUtf16 + 1) < endUtf16)
            && (((*(curUtf16 + 1) & 0xFC00)) == 0xDC00)) {
            utf32 = (*curUtf16++ - 0xD800) << 10;
            utf32 |= *curUtf16++ - 0xDC00;
            utf32 += 0x10000;
        } else {
            utf32 = (char32_t)* curUtf16++;
        }
        const size_t len = Utf32CodePointUtf8Length(utf32);
        if (str8Len < len) {
            break;
        }

        Utf32CodePointToUtf8(reinterpret_cast<uint8_t*>(cur), utf32, len);
        cur += len;
        str8Len -= len;
    }
    *cur = '\0';
}

// inner function and str16 is not null
char* Char16ToChar8(const char16_t* str16, size_t str16Len)
{
    char* str8 = nullptr;
    int utf8Len = Utf16ToUtf8Length(str16, str16Len);
    if (utf8Len < 0) {
        return nullptr;
    }

    // Allow for closing '\0'
    utf8Len += 1;
    str8 = reinterpret_cast<char*>(calloc(utf8Len, sizeof(char)));
    if (str8 == nullptr) {
        return nullptr;
    }

    StrncpyStr16ToStr8(str16, str16Len, str8, utf8Len);
    return str8;
}

bool String16ToString8(const u16string& str16, string& str8)
{
    size_t str16Len = str16.length();
    if (str16Len < 1) {
        return false;
    }

    char* str8Temp = Char16ToChar8(str16.c_str(), str16Len);
    if (str8Temp == nullptr) {
        UTILS_LOGE("Str16 to str8 failed, because str8Temp is nullptr!");
        return false;
    }

    str8 = str8Temp;
    free(str8Temp);
    str8Temp = nullptr;
    return true;
}

/**
* return 1-4 by first byte
* 1111xxxx : 4
* 1110xxxx : 3
* 110xxxxx : 2
* 10xxxxxx : 1
* 0xxxxxxx : 1
*/
static inline size_t Utf8CodePointLen(uint8_t ch)
{
    return ((0xe5000000 >> ((ch >> 3) & 0x1e)) & 3) + 1;
}

static inline void Utf8ShiftAndMask(uint32_t* codePoint, const uint8_t byte)
{
    *codePoint <<= 6;
    *codePoint |= 0x3F & byte;
}

uint32_t Utf8ToUtf32CodePoint(const char* src, size_t length)
{
    uint32_t unicode = 0;

    switch (length) {
        case 1:
            return src[0];
        case 2:
            unicode = src[0] & 0x1f;
            Utf8ShiftAndMask(&unicode, src[1]);
            return unicode;
        case 3:
            unicode = src[0] & 0x0f;
            Utf8ShiftAndMask(&unicode, src[1]);
            Utf8ShiftAndMask(&unicode, src[2]);
            return unicode;
        case 4:
            unicode = src[0] & 0x07;
            Utf8ShiftAndMask(&unicode, src[1]);
            Utf8ShiftAndMask(&unicode, src[2]);
            Utf8ShiftAndMask(&unicode, src[3]);
            return unicode;
        default:
            return 0xffff;
    }
}

int Utf8ToUtf16Length(const char* str8, size_t str8Len)
{
    const char* const str8end = str8 + str8Len;
    int utf16len = 0;
    while (str8 < str8end) {
        utf16len++;
        int u8charlen = Utf8CodePointLen(*str8);
        if (str8 + u8charlen - 1 >= str8end) {
            UTILS_LOGE("Get str16 length failed because str8 unicode is illegal!");
            return -1;
        }
        uint32_t codepoint = Utf8ToUtf32CodePoint(str8, u8charlen);
        if (codepoint > 0xFFFF) {
            utf16len++; // this will be a surrogate pair in utf16
        }
        str8 += u8charlen;
    }
    if (str8 != str8end) {
        UTILS_LOGE("Get str16 length failed because str8length is illegal!");
        return -1;
    }
    return utf16len;
}

char16_t* Utf8ToUtf16(const char* utf8Str, size_t u8len, char16_t* u16str, size_t u16len)
{
    if (u16len == 0) {
        return u16str;
    }
    const char* const u8end = utf8Str + u8len;
    const char* u8cur = utf8Str;
    const char16_t* const u16end = u16str + u16len;
    char16_t* u16cur = u16str;

    while ((u8cur < u8end) && (u16cur < u16end)) {
        size_t len = Utf8CodePointLen(*u8cur);
        uint32_t codepoint = Utf8ToUtf32CodePoint(u8cur, len);
        // Convert the UTF32 codepoint to one or more UTF16 codepoints
        if (codepoint <= 0xFFFF) {
            // Single UTF16 character
            *u16cur++ = (char16_t)codepoint;
        } else {
            // Multiple UTF16 characters with surrogates
            codepoint = codepoint - 0x10000;
            *u16cur++ = (char16_t)((codepoint >> 10) + 0xD800);
            if (u16cur >= u16end) {
                // Ooops...  not enough room for this surrogate pair.
                return u16cur - 1;
            }
            *u16cur++ = (char16_t)((codepoint & 0x3FF) + 0xDC00);
        }

        u8cur += len;
    }
    return u16cur;
}

void StrncpyStr8ToStr16(const char* utf8Str, size_t u8len, char16_t* u16str, size_t u16len)
{
    char16_t* result = Utf8ToUtf16(utf8Str, u8len, u16str, u16len - 1);
    *result = 0;
    return;
}

// inner function and str8 is not null
char16_t* Char8ToChar16(const char* str8, size_t str8Len)
{
    char16_t* str16 = nullptr;
    int utf16Len = Utf8ToUtf16Length(str8, str8Len);
    if (utf16Len < 0) {
        UTILS_LOGE("Get str16 length failed,length is: %{public}d", utf16Len);
        return nullptr;
    }

    // Allow for closing 0
    utf16Len = utf16Len + 1;
    str16 = reinterpret_cast<char16_t*>(calloc(utf16Len, sizeof(char16_t)));
    if (str16 == nullptr) {
        UTILS_LOGE("Str16 malloc memory failed!");
        return nullptr;
    }

    StrncpyStr8ToStr16(str8, str8Len, str16, utf16Len);
    return str16;
}

bool String8ToString16(const string& str8, u16string& str16)
{
    size_t str8len = str8.length();
    if (str8len < 1) {
        return false;
    }

    char16_t* str16Temp = Char8ToChar16(str8.c_str(), str8len);
    if (str16Temp == nullptr) {
        UTILS_LOGE("str8 to str16 failed, str16Temp is nullptr!");
        return false;
    }

    str16 = str16Temp;
    free(str16Temp);
    str16Temp = nullptr;
    return true;
}
} // namespace OHOS
