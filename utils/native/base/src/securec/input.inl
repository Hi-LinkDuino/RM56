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


/* [Standardize-exceptions] Use unsafe function: Performance-sensitive
 * [reason] Always used in the performance critical path,
 *          and sufficient input validation is performed before calling
 */

#ifndef __INPUT_INL__5D13A042_DC3F_4ED9_A8D1_882811274C27
#define __INPUT_INL__5D13A042_DC3F_4ED9_A8D1_882811274C27

#if !defined(SECUREC_SYSAPI4VXWORKS)
#include <ctype.h>
#ifdef SECUREC_FOR_WCHAR
#include <wctype.h>             /* for iswspace */
#endif
#endif

#define SECUREC_NUM_WIDTH_SHORT       0
#define SECUREC_NUM_WIDTH_INT         1
#define SECUREC_NUM_WIDTH_LONG        2
#define SECUREC_NUM_WIDTH_LONG_LONG   3 /* also long double */

#define SECUREC_BUF_EXT_MUL (2)
#define SECUREC_BUFFERED_BLOK_SIZE 1024

#if defined(SECUREC_SYSAPI4VXWORKS)
#define SECUREC_DECIMAL_POINT_PTR  "."
#else
#include <locale.h>             /* if this file NOT exist, you can remove it */
#define SECUREC_DECIMAL_POINT_PTR (localeconv()->decimal_point)
#endif

#if defined(SECUREC_VXWORKS_PLATFORM) && !defined(va_copy) && !defined(__va_copy)
/* the name is the same as system macro. */
#define __va_copy(d,s) do { \
                            size_t size_of_d = (size_t)sizeof(d); \
                            size_t size_of_s = (size_t)sizeof(s); \
                            if (size_of_d != size_of_s) { \
                                (void)memcpy((d), (s), sizeof(va_list)); \
                            } else { \
                                (void)memcpy(&(d), &(s), sizeof(va_list)); \
                            } \
                         } SECUREC_WHILE_ZERO
#endif

#define SECUREC_MUL10(x) ((((x) << 2) + (x)) << 1)
#define SECUREC_MULTI_BYTE_MAX_LEN (6)
#define SECUREC_INT_MAX_DIV_TEN       21474836

/* Compatibility macro name cannot be modifie */
#ifndef UNALIGNED
#if !(defined(_M_IA64)) && !(defined(_M_AMD64))
#define UNALIGNED
#else
#define UNALIGNED __unaligned
#endif
#endif

#if (defined(SECUREC_COMPATIBLE_LINUX_FORMAT) && !(defined(SECUREC_ON_UNIX)))
#define SECUREC_MAX_64BITS_VALUE                18446744073709551615ULL
#define SECUREC_MAX_64BITS_VALUE_DIV_TEN        1844674407370955161ULL
#define SECUREC_MAX_64BITS_VALUE_CUT_LAST_DIGIT 18446744073709551610ULL
#define SECUREC_MIN_64BITS_NEG_VALUE            9223372036854775808ULL
#define SECUREC_MAX_64BITS_POS_VALUE            9223372036854775807ULL
#define SECUREC_MIN_32BITS_NEG_VALUE            2147483648ULL
#define SECUREC_MAX_32BITS_POS_VALUE            2147483647ULL
#define SECUREC_MAX_32BITS_VALUE                4294967295ULL
#define SECUREC_MAX_32BITS_VALUE_INC            4294967296ULL
#define SECUREC_MAX_32BITS_VALUE_DIV_TEN        429496729ULL
#define SECUREC_LONG_BIT_NUM            ((unsigned int)(sizeof(long) << 3U))
#endif

#define SECUREC_CHAR(x) (x)
#define SECUREC_BRACE ('{')     /* [ to { */

#ifdef SECUREC_FOR_WCHAR
#define SECUREC_EOF WEOF
#define SECUREC_MB_LEN 16       /* max. # bytes in multibyte char  ,see MB_LEN_MAX */
#define SECUREC_GET_CHAR() (++charCount, SecGetCharW(stream))
/* un get char marco do not set char count ,The reason is to avoid warning that variables are not used */
#define SECUREC_UN_GET_CHAR(chr) (--charCount,SecUnGetCharW((chr), stream))
#define SECUREC_IS_DIGIT(chr) (!((chr) & 0xff00) && isdigit(((chr) & 0x00ff)))
#define SECUREC_IS_XDIGIT(chr) (!((chr) & 0xff00) && isxdigit(((chr) & 0x00ff)))
static void SecUnGetCharW(SecInt chr, SecFileStream *str);
static SecInt SecGetCharW(SecFileStream *str);
#else
#define SECUREC_EOF EOF
#define SECUREC_GET_CHAR() (++charCount, SecGetChar(stream))
#define SECUREC_UN_GET_CHAR(chr) (--charCount,SecUnGetChar((chr), stream))
#define SECUREC_IS_DIGIT(chr) isdigit((unsigned char)(chr)& 0x00ff)
#define SECUREC_IS_XDIGIT(chr) isxdigit((unsigned char)(chr)& 0x00ff)
static SecInt SecGetChar(SecFileStream *str);
static void SecUnGetChar(SecInt chr, SecFileStream *str);
#endif

#define SECUREC_SKIP_SPACE_CHAR() SecSkipSpaceChar(&charCount, stream)

static SecInt SecSkipSpaceChar(int *, SecFileStream *);

typedef struct {
#if (defined(SECUREC_COMPATIBLE_LINUX_FORMAT) && !(defined(SECUREC_ON_UNIX)))
    SecUnsignedInt64 number64As;
    int beyondMax;
#endif
    SecUnsignedInt64 number64;
    unsigned long number;
    int negative;
} SecNumberSpce;

static int SecCheckFloatDecPoint(char dec_point)
{
    /* don't support multi-language decimal point */
    if (dec_point == '.') {
        return 1;
    }
    return 0;
}

static int SecIs64BitPtr(size_t ptrSize)
{
    /* point size is 4 or 8 , Under the 64 bit system, the value not 0 */
    /* to clear e778 */
    return (int)(ptrSize & sizeof(SecInt64));
}

static void SecAssignFloat(int numberWidth, void *argument, const char *number)
{
    char *endptr = NULL;
    double d;
#if SECUREC_SUPPORT_STRTOLD
    if (numberWidth == SECUREC_NUM_WIDTH_LONG_LONG) {
        long double d2 = strtold(number, &endptr);
        *(long double UNALIGNED *)argument = d2;
        return;
    }
#endif
    d = strtod(number, &endptr);
    if (numberWidth > SECUREC_NUM_WIDTH_INT) {
        *(double UNALIGNED *)argument = (double)d;
    } else {
        *(float UNALIGNED *)argument = (float)d;
    }
}

static int SecUpdateFloatString(size_t usedLen,
                                size_t *floatStrSize,
                                SecChar **floatStr, const SecChar *floatStrBuf, SecChar **allocFlag)
{
    if (usedLen == (*floatStrSize)) {
        if ((*floatStr) == floatStrBuf) {
            /* add 1 to clear ZERO LENGTH ALLOCATIONS warning */
            size_t oriBufSize = (*floatStrSize) * SECUREC_BUF_EXT_MUL * sizeof(SecChar) + 1;
            void *tmpPointer = (void *)SECUREC_MALLOC(oriBufSize);
            if (tmpPointer == NULL) {
                return 0;
            }
            if (memcpy_s(tmpPointer, oriBufSize, (*floatStr), (*floatStrSize) * sizeof(SecChar)) != EOK) {
                SECUREC_FREE(tmpPointer);   /* This is a dead code, just to meet the coding requirements */
                return 0;
            }
            (*floatStr) = (SecChar *) (tmpPointer);
            (*allocFlag) = (SecChar *) (tmpPointer); /* use to clear free on stack warning */
            (*floatStrSize) *= SECUREC_BUF_EXT_MUL; /* this is OK, oriBufSize plus 1 just clear warning */
        } else {
            /* LSD 2014.3.6 fix, replace realloc to malloc to avoid heap injection */
            size_t oriBufSize = (*floatStrSize) * sizeof(SecChar);
            size_t nextSize = oriBufSize * SECUREC_BUF_EXT_MUL + 1; /* add 1 to clear ZERO LENGTH ALLOCATIONS warning */
            /* Prevents integer overflow when calculating the wide character length.
             * The maximum length of 2G/2 is enough
             */
            if (nextSize <= (SECUREC_WCHAR_MEM_MAX_LEN) / 2) {
                void *tmpPointer = (void *)SECUREC_MALLOC(nextSize);
                if (tmpPointer == NULL) {
                    return 0;
                }
                if (memcpy_s(tmpPointer, nextSize, (*floatStr), oriBufSize) != EOK) {
                    SECUREC_FREE(tmpPointer);   /* This is a dead code, just to meet the coding requirements */
                    return 0;
                }
                if (memset_s((*floatStr), oriBufSize, 0, oriBufSize) != EOK) {
                    SECUREC_FREE(tmpPointer);   /* This is a dead code, just to meet the coding requirements */
                    return 0;
                }
                SECUREC_FREE((*floatStr));

                (*floatStr) = (SecChar *) (tmpPointer);
                (*allocFlag) = (SecChar *) (tmpPointer);    /* use to clear free on stack warning */
                (*floatStrSize) *= SECUREC_BUF_EXT_MUL; /* this is OK, oriBufSize plus 1 just clear warning */
                return 1;
            }
            return 0;
        }
    }
    return 1;
}

#ifndef SECUREC_FOR_WCHAR

/* LSD only multi-bytes string need isleadbyte() function */
static int SecIsleadbyte(SecInt ch)
{
    unsigned int c = (unsigned int)ch;
#if !(defined(_WIN32) || defined(_INC_WCTYPE))
    return (int)(c & 0x80);
#else
    return (int)isleadbyte((int)(c & 0xff));
#endif
}

#endif

static void SecUpdateWcharFlagByType(SecUnsignedChar chr, signed char *isWchar)
{
#if defined(SECUREC_FOR_WCHAR) && (defined(SECUREC_COMPATIBLE_WIN_FORMAT))
    signed char flagForUpperType = -1;
    signed char flagForLowerType =  1;
#else
    signed char flagForUpperType =  1;
    signed char flagForLowerType = -1;
#endif

    if ((*isWchar) == 0) {
        if ((chr == SECUREC_CHAR('C')) || (chr == SECUREC_CHAR('S'))) {
            (*isWchar) = flagForUpperType;
        } else {
            (*isWchar) = flagForLowerType;
        }
    }
    return;
}

#define SECUREC_FLOAT_BUFSIZE (309+40)  /* digits in max. dp value + slop */
#ifdef SECUREC_FOR_WCHAR
#define SECUREC_BRACKET_TABLE_SIZE    (32 * 256)
#else
#define SECUREC_BRACKET_TABLE_SIZE    (32)
#endif

#ifdef SECUREC_FOR_WCHAR
#define SECUREC_GETC fgetwc
#define SECUREC_UN_GETC ungetwc
#define SECUREC_CHAR_MASK 0xffff
#else
#define SECUREC_GETC fgetc
#define SECUREC_UN_GETC ungetc
#define SECUREC_CHAR_MASK 0xff
#endif

/* LSD 2014 1 24 add to protect NULL pointer access */
#define SECUREC_CHECK_INPUT_ADDR(p) do { \
                                        if ((p) == NULL) { \
                                            paraIsNull = 1; \
                                            goto ERR_RET; \
                                        } \
                                    } SECUREC_WHILE_ZERO

#ifdef SECUREC_FOR_WCHAR
void SecClearDestBufW(const wchar_t *buffer, const wchar_t *cformat, va_list arglist)
#else
void SecClearDestBuf(const char *buffer, const char *cformat, va_list arglist)
#endif
{
    const SecUnsignedChar *fmt = (const SecUnsignedChar *)cformat;
    void *pDestBuf = NULL;
    va_list arglistSave;        /* backup for arglist value, this variable don't need initialized */
    size_t bufSize = 0;
    int spec = 0;
    signed char isWchar = 0;
    char doneFlag = 0;

    if (fmt != NULL) {
        while (*fmt) {
            if (*fmt == SECUREC_CHAR('%')) {
                doneFlag = 0;
                isWchar = 0;

                while (doneFlag == 0) {
                    spec = *(++fmt);

                    if (SECUREC_IS_DIGIT((SecUnsignedChar) spec)) {
                        continue;
                    } else if (spec == SECUREC_CHAR('h')) {
                        isWchar = -1;
                        continue;
                    } else if (spec == SECUREC_CHAR('l') || spec == SECUREC_CHAR('w')) {
                        isWchar = 1;
                        continue;
                    }
                    doneFlag = 1;
                }

                /* if no  l or h flag */
                SecUpdateWcharFlagByType(*fmt, &isWchar);

                spec = *fmt | (SECUREC_CHAR('a') - SECUREC_CHAR('A'));

                if (!(spec == SECUREC_CHAR('c') || spec == SECUREC_CHAR('s') || spec == SECUREC_BRACE)) {
                    return;     /* first argument is not a string type */
                }

                if ((buffer != NULL) && (*buffer != SECUREC_CHAR('\0')) && (spec != SECUREC_CHAR('s'))) {
                    /* when buffer not empty just clear %s.
                     * example call sscanf by  argument of (" \n","%s",str,sizeof(str))
                     */
                    return;
                }

                if (spec == SECUREC_BRACE) {
#if !(defined(SECUREC_COMPATIBLE_WIN_FORMAT))
                    if (*fmt == SECUREC_CHAR('{')) {
                        return;
                    }
#endif
                    ++fmt;

                    if (*fmt == SECUREC_CHAR('^')) {
                        ++fmt;
                    }

                    if (*fmt == SECUREC_CHAR(']')) {
                        ++fmt;
                    }

                    while ((*fmt != SECUREC_CHAR('\0')) && (*fmt != SECUREC_CHAR(']'))) {
                        ++fmt;
                    }
                    if (*fmt == SECUREC_CHAR('\0')) {
                        return; /* trunc'd format string */
                    }
                }

                (void)memset(&arglistSave, 0, sizeof(arglistSave)); /* to clear e530 arglistSave not initialized */
#if defined(va_copy)
                va_copy(arglistSave, arglist);
#elif defined(__va_copy)        /* for vxworks */
                __va_copy(arglistSave, arglist);
#else
                arglistSave = arglist;
#endif
                pDestBuf = (void *)va_arg(arglistSave, void *);
                /* Get the next argument - size of the array in characters */
                bufSize = ((size_t)(va_arg(arglistSave, size_t))) & 0xFFFFFFFFUL;

                va_end(arglistSave);
                /* to clear e438 last value assigned not used , the compiler will optimize this code */
                (void)arglistSave;

                if (bufSize == 0 || bufSize > SECUREC_STRING_MAX_LEN || pDestBuf == NULL) {
                    return;
                }

                *(char *)pDestBuf = '\0';

                if (isWchar > 0 && bufSize >= sizeof(wchar_t)) {
                    *(wchar_t UNALIGNED *)pDestBuf = L'\0';
                }

                return;
            }
            ++fmt;              /* skip to next char */
        }
    }
    return;
}

static void SecAssignNumber(void *argPtr, const int numberWidth, const unsigned long number)
{
    if (numberWidth > SECUREC_NUM_WIDTH_INT) {
        *(long UNALIGNED *)argPtr = (long)number;   /* take number as unsigned number */
    } else if (numberWidth == SECUREC_NUM_WIDTH_INT) {
        *(int UNALIGNED *)argPtr = (int)number;
    } else if (numberWidth == SECUREC_NUM_WIDTH_SHORT) {
        *(short UNALIGNED *)argPtr = (short)number; /* take number as unsigned number */
    } else {                    /* < 0 for hh format modifier */
        *(char UNALIGNED *)argPtr = (char)number;   /* take number as unsigned number */
    }
}

#if (defined(SECUREC_COMPATIBLE_LINUX_FORMAT) && !(defined(SECUREC_ON_UNIX)))
static int SecIsLongBitEqual(const int bitNum)
{
    return (unsigned int)bitNum == SECUREC_LONG_BIT_NUM;
}
#endif
static SecInt SecHex2Dec(const SecInt ch)
{
    /* use isdigt Causing tool false alarms */
    return (SecInt) ((ch >= '0' && ch <= '9') ? (unsigned char)ch :
                     ((((unsigned char)ch | (unsigned char)('a' - 'A')) - ('a')) + 10 + '0'));
}

static char SecDecodeNumber(const int comChr, const SecInt ch, SecNumberSpce *spec)
{
    char doneFlag = 0;
#if (defined(SECUREC_COMPATIBLE_LINUX_FORMAT) && !(defined(SECUREC_ON_UNIX)))
    unsigned long decimalEdge = SECUREC_MAX_32BITS_VALUE_DIV_TEN;
#endif
    if (comChr == SECUREC_CHAR('x') || comChr == SECUREC_CHAR('p')) {
        if (SECUREC_IS_XDIGIT(ch)) {
            SecInt ch2;
#if (defined(SECUREC_COMPATIBLE_LINUX_FORMAT) && !(defined(SECUREC_ON_UNIX)))
            if ((spec->number >> (SECUREC_LONG_BIT_NUM - 4)) > 0) {
                spec->beyondMax = 1;
            }
#endif
            spec->number = (spec->number << 4);
            ch2 = SecHex2Dec(ch);
            spec->number += (unsigned long)((SecUnsignedInt) ch2 - SECUREC_CHAR('0'));
        } else {
            doneFlag = 1;
        }
    } else if (SECUREC_IS_DIGIT(ch)) {

        if (comChr == SECUREC_CHAR('o')) {
            if (ch < SECUREC_CHAR('8')) {
#if (defined(SECUREC_COMPATIBLE_LINUX_FORMAT) && !(defined(SECUREC_ON_UNIX)))
                if ((spec->number >> (SECUREC_LONG_BIT_NUM - 3)) > 0) {
                    spec->beyondMax = 1;
                }
#endif
                spec->number = (spec->number << 3);
                spec->number += (unsigned long)((SecUnsignedInt) ch - SECUREC_CHAR('0'));
            } else {
                doneFlag = 1;
            }
        } else {                /* SECUREC_CHAR('d') == comChr */
#if (defined(SECUREC_COMPATIBLE_LINUX_FORMAT) && !(defined(SECUREC_ON_UNIX)))
#ifdef SECUREC_ON_64BITS
            if (SecIsLongBitEqual(64)) {
                decimalEdge = (unsigned long)SECUREC_MAX_64BITS_VALUE_DIV_TEN;
            }
#else
            if (SecIsLongBitEqual(32)) {
                decimalEdge = SECUREC_MAX_32BITS_VALUE_DIV_TEN;
            }
#endif
            if (spec->number > decimalEdge) {
                spec->beyondMax = 1;
            }
#endif
            spec->number = SECUREC_MUL10(spec->number);
#if (defined(SECUREC_COMPATIBLE_LINUX_FORMAT) && !(defined(SECUREC_ON_UNIX)))
            if (spec->number == SECUREC_MUL10(decimalEdge)) {
                spec->number64As = (unsigned long)SECUREC_MAX_64BITS_VALUE - spec->number;
                if (spec->number64As < (SecUnsignedInt64)((SecUnsignedInt) ch - SECUREC_CHAR('0'))) {
                    spec->beyondMax = 1;
                }
            }
#endif
            spec->number += (unsigned long)((SecUnsignedInt) ch - SECUREC_CHAR('0'));
        }
    } else {
        doneFlag = 1;
    }
    return doneFlag;
}

static void SecFinishNumber(const int comChr, const int numberWidth, SecNumberSpce *spec)
{

#if (defined(SECUREC_COMPATIBLE_LINUX_FORMAT) && !(defined(SECUREC_ON_UNIX)))
    if (spec->negative != 0) {
        if (numberWidth == SECUREC_NUM_WIDTH_INT) {
            if ((comChr == SECUREC_CHAR('d')) || (comChr == SECUREC_CHAR('i'))) {
#ifdef SECUREC_ON_64BITS
                if (SecIsLongBitEqual(64)) {
                    if ((spec->number > SECUREC_MIN_64BITS_NEG_VALUE)) {
                        spec->number = 0;
                    } else {
                        spec->number = (unsigned int)(-(int)spec->number);
                    }
                }
#else
                if (SecIsLongBitEqual(32)) {
                    if ((spec->number > SECUREC_MIN_32BITS_NEG_VALUE)) {
                        spec->number = SECUREC_MIN_32BITS_NEG_VALUE;
                    } else {
                        spec->number = (unsigned int)(-(int)spec->number);
                    }
                }
#endif
                if (spec->beyondMax) {
#ifdef SECUREC_ON_64BITS
                    if (SecIsLongBitEqual(64)) {
                        spec->number = 0;
                    }
#else
                    if (SecIsLongBitEqual(32)) {
                        spec->number = SECUREC_MIN_32BITS_NEG_VALUE;
                    }
#endif
                }
            } else {            /* o, u, x, X ,p */
#ifdef SECUREC_ON_64BITS
                if (spec->number > SECUREC_MAX_32BITS_VALUE_INC) {
                    spec->number = SECUREC_MAX_32BITS_VALUE;
                } else {
                    spec->number = (unsigned int)(-(int)spec->number);
                }
#else
                 spec->number = (unsigned int)(-(int)spec->number);
#endif
                if (spec->beyondMax) {
                    spec->number |= (unsigned long)0xffffffffffffffffULL;
                }
            }
        } else {
            if ((comChr == SECUREC_CHAR('d')) || (comChr == SECUREC_CHAR('i'))) {
                if (spec->number > (unsigned long)(1ULL << (SECUREC_LONG_BIT_NUM - 1))) {
                    spec->number = (unsigned long)(1ULL << (SECUREC_LONG_BIT_NUM - 1));
                } else {
                    spec->number = (unsigned long)(-(long)spec->number);
                }
            } else {
                spec->number = (unsigned long)(-(long)spec->number);
                if (spec->beyondMax) {
                    spec->number |= (unsigned long)0xffffffffffffffffULL;
                }
            }
        }
        if ((comChr == SECUREC_CHAR('d')) || (comChr == SECUREC_CHAR('i'))) {
            if (((spec->beyondMax) && (numberWidth < SECUREC_NUM_WIDTH_SHORT)) ||
                ((spec->beyondMax) && (numberWidth == SECUREC_NUM_WIDTH_SHORT)) ||
                ((spec->beyondMax) && (numberWidth == SECUREC_NUM_WIDTH_INT) && (SecIsLongBitEqual(64)))) {
                spec->number = 0;
            }
            if ((spec->beyondMax) && (numberWidth == SECUREC_NUM_WIDTH_LONG)) {
                spec->number = ((unsigned long)(1UL << (SECUREC_LONG_BIT_NUM - 1)));
            }
        } else {                /* o, u, x, X, p */
            if (spec->beyondMax) {
                spec->number |= (unsigned long)0xffffffffffffffffULL;
            }
        }
    } else {
        if (numberWidth == SECUREC_NUM_WIDTH_INT) {
            if ((comChr == SECUREC_CHAR('d')) || (comChr == SECUREC_CHAR('i'))) {
#ifdef SECUREC_ON_64BITS
                if (SecIsLongBitEqual(64)) {
                    if (spec->number > SECUREC_MAX_64BITS_POS_VALUE) {
                        spec->number |= (unsigned long)0xffffffffffffffffULL;
                    }
                }
                if ((spec->beyondMax) && (SecIsLongBitEqual(64))) {
                    spec->number |= (unsigned long)0xffffffffffffffffULL;
                }
#else
                if (SecIsLongBitEqual(32)) {
                    if (spec->number > SECUREC_MAX_32BITS_POS_VALUE) {
                        spec->number = SECUREC_MAX_32BITS_POS_VALUE;
                    }
                }
                if ((spec->beyondMax) && (SecIsLongBitEqual(32))) {
                    spec->number = SECUREC_MAX_32BITS_POS_VALUE;
                }
#endif
            } else {            /* o,u,x,X,p */
                if (spec->beyondMax) {
                    spec->number = SECUREC_MAX_32BITS_VALUE;
                }
            }

        } else {
            if ((comChr == SECUREC_CHAR('d')) || (comChr == SECUREC_CHAR('i'))) {
                if (spec->number > ((unsigned long)(1UL << (SECUREC_LONG_BIT_NUM - 1)) - 1)) {
                    spec->number = ((unsigned long)(1UL << (SECUREC_LONG_BIT_NUM - 1)) - 1);
                }
                if (((spec->beyondMax) && (numberWidth < SECUREC_NUM_WIDTH_SHORT)) ||
                    ((spec->beyondMax) && (numberWidth == SECUREC_NUM_WIDTH_SHORT))) {
                    spec->number |= (unsigned long)0xffffffffffffffffULL;
                }
                if ((spec->beyondMax) && (numberWidth == SECUREC_NUM_WIDTH_LONG)) {
                    spec->number = ((unsigned long)(1UL << (SECUREC_LONG_BIT_NUM - 1)) - 1);
                }
            } else {
                if (spec->beyondMax) {
                    spec->number |= (unsigned long)0xffffffffffffffffULL;
                }
            }
        }
    }
#else
    if (spec->negative != 0) {
#if defined(__hpux)
        if (comChr != SECUREC_CHAR('p')) {
            spec->number = (unsigned long)(-(long)spec->number);
        }
#else
        spec->number = (unsigned long)(-(long)spec->number);
#endif
    }
#endif

    (void)numberWidth;          /* clear compile warning */
    (void)comChr;               /* clear compile warning */
    return;
}

static char SecDecodeNumber64(const int comChr, const SecInt ch, SecNumberSpce *spec)
{
    char doneFlag = 0;

    if (comChr == SECUREC_CHAR('x') || comChr == SECUREC_CHAR('p')) {
        if (SECUREC_IS_XDIGIT(ch)) {
            SecInt ch2;
#if (defined(SECUREC_COMPATIBLE_LINUX_FORMAT) && !(defined(SECUREC_ON_UNIX)))
            if ((spec->number64 >> 60) > 0) {
                spec->beyondMax = 1;
            }
            spec->number64As = 16;
#endif
            spec->number64 <<= 4;
            ch2 = SecHex2Dec(ch);
            spec->number64 += (SecUnsignedInt64)((SecUnsignedInt) ch2 - SECUREC_CHAR('0'));
        } else {
            doneFlag = 1;
        }
    } else if (SECUREC_IS_DIGIT(ch)) {
        if (comChr == SECUREC_CHAR('o')) {
            if (ch < SECUREC_CHAR('8')) {
#if (defined(SECUREC_COMPATIBLE_LINUX_FORMAT) && !(defined(SECUREC_ON_UNIX)))
                if ((spec->number64 >> 61) > 0) {
                    spec->beyondMax = 1;
                }
                spec->number64As = 8;
#endif
                spec->number64 <<= 3;
                spec->number64 += (SecUnsignedInt64)((SecUnsignedInt) ch - SECUREC_CHAR('0'));
            } else {
                doneFlag = 1;
            }
        } else {                /* SECUREC_CHAR('d') == comChr */
#if (defined(SECUREC_COMPATIBLE_LINUX_FORMAT) && !(defined(SECUREC_ON_UNIX)))
            if (spec->number64 > SECUREC_MAX_64BITS_VALUE_DIV_TEN) {
                spec->beyondMax = 1;
            }
#endif
            spec->number64 = SECUREC_MUL10(spec->number64);
#if (defined(SECUREC_COMPATIBLE_LINUX_FORMAT) && !(defined(SECUREC_ON_UNIX)))
            if (spec->number64 == SECUREC_MAX_64BITS_VALUE_CUT_LAST_DIGIT) {
                spec->number64As = SECUREC_MAX_64BITS_VALUE - spec->number64;
                if (spec->number64As < (SecUnsignedInt64)((SecUnsignedInt) ch - SECUREC_CHAR('0'))) {
                    spec->beyondMax = 1;
                }
            }
#endif
            spec->number64 += (SecUnsignedInt64)((SecUnsignedInt) ch - SECUREC_CHAR('0'));
        }

    } else {
        doneFlag = 1;
    }

    return doneFlag;
}

static void SecFinishNumber64(const int comChr, SecNumberSpce *spec)
{
#if (defined(SECUREC_COMPATIBLE_LINUX_FORMAT) && !(defined(SECUREC_ON_UNIX)))
    if (spec->negative != 0) {
        if (comChr == (SECUREC_CHAR('d')) || (comChr == SECUREC_CHAR('i'))) {
            if (spec->number64 > SECUREC_MIN_64BITS_NEG_VALUE) {
                spec->number64 = SECUREC_MIN_64BITS_NEG_VALUE;
            } else {
                spec->number64 = (SecUnsignedInt64)(-(SecInt64)spec->number64);
            }
            if (spec->beyondMax) {
                spec->number64 = SECUREC_MIN_64BITS_NEG_VALUE;
            }
        } else {                /* o, u, x, X, p */
            spec->number64 = (SecUnsignedInt64)(-(SecInt64)spec->number64);
            if (spec->beyondMax) {
                spec->number64 = SECUREC_MAX_64BITS_VALUE;
            }
        }
    } else {
        if ((comChr == SECUREC_CHAR('d')) || (comChr == SECUREC_CHAR('i'))) {
            if (spec->number64 > SECUREC_MAX_64BITS_POS_VALUE) {
                spec->number64 = SECUREC_MAX_64BITS_POS_VALUE;
            }
            if (spec->beyondMax) {
                spec->number64 = SECUREC_MAX_64BITS_POS_VALUE;
            }
        } else {
            if (spec->beyondMax) {
                spec->number64 = SECUREC_MAX_64BITS_VALUE;
            }
        }
    }
#else
    if (spec->negative != 0) {
#if defined(__hpux)
        if (comChr != SECUREC_CHAR('p')) {
            spec->number64 = (SecUnsignedInt64)(-(SecInt64)spec->number64);
        }
#else
        spec->number64 = (SecUnsignedInt64)(-(SecInt64)spec->number64);
#endif
    }
#endif
    (void)comChr;               /* clear compile warning */
    return;
}

static void SecAdjustStream(SecFileStream *stream)
{
    if (stream != NULL && (stream->flag & SECUREC_FILE_STREAM_FLAG) && stream->base != NULL) {

        if ((0 == stream->count) && feof(stream->pf)) {
            /* file pointer at the end of file, don't need to seek back */
            stream->base[0] = '\0';
        } else {
            /* LSD seek to original position, bug fix 2014 1 21 */

            if (fseek(stream->pf, stream->oriFilePos, SEEK_SET)) {
                /* seek failed, ignore it */
                stream->oriFilePos = 0;
            } else {
                if (stream->fileRealRead > 0) { /* LSD bug fix. when file reach to EOF, don't seek back */
#if (defined(SECUREC_COMPATIBLE_WIN_FORMAT))
                    int loops;
                    for (loops = 0; loops < (stream->fileRealRead / SECUREC_BUFFERED_BLOK_SIZE); ++loops) {
                        if (fread(stream->base, (size_t)1, (size_t)SECUREC_BUFFERED_BLOK_SIZE,
                                  stream->pf) != SECUREC_BUFFERED_BLOK_SIZE) {
                            break;
                        }
                    }
                    if ((stream->fileRealRead % SECUREC_BUFFERED_BLOK_SIZE) != 0) {
                        size_t ret = fread(stream->base,
                                           (size_t)((unsigned int)stream->fileRealRead % SECUREC_BUFFERED_BLOK_SIZE),
                                           (size_t)1, stream->pf);
                        if (ret == 1 || ret == 0) {
                            if (ftell(stream->pf) < stream->oriFilePos + stream->fileRealRead) {
                                (void)fseek(stream->pf, stream->oriFilePos + stream->fileRealRead, SEEK_SET);
                            }
                        }
                    }

#else
                    /* in linux like system */
                    if (fseek(stream->pf, stream->oriFilePos + stream->fileRealRead, SEEK_SET)) {
                        /* seek failed, ignore it */
                        stream->oriFilePos = 0;
                    }
#endif
                }
            }
        }

        SECUREC_FREE(stream->base);
        stream->base = NULL;
    }
    return;
}

#ifdef SECUREC_FOR_WCHAR
int SecInputSW(SecFileStream *stream, const wchar_t *cformat, va_list arglist)
#else
int SecInputS(SecFileStream *stream, const char *cformat, va_list arglist)
#endif
{
    SecChar floatStrBuffer[SECUREC_FLOAT_BUFSIZE + 1];
    SecChar *pFloatStr = floatStrBuffer;
    SecChar *pAllocedFloatStr = NULL;
    const SecUnsignedChar *format = (const SecUnsignedChar *)cformat;

    size_t arrayWidth = 0;
    size_t floatStrUsedLen = 0;
    size_t floatStrSize = sizeof(floatStrBuffer) / sizeof(floatStrBuffer[0]);

#ifdef SECUREC_FOR_WCHAR
    unsigned char *bracketTable = NULL;
#else
    unsigned char bracketTable[SECUREC_BRACKET_TABLE_SIZE] = { 0 };
#endif

#if (defined(SECUREC_COMPATIBLE_LINUX_FORMAT) && !(defined(SECUREC_ON_UNIX)))
    SecNumberSpce numberSpec = { 0, 0, 0, 0, 0 };
#else
    SecNumberSpce numberSpec = { 0, 0, 0 };
#endif

    void *argPtr = NULL;        /* points to receiving data addr */
    void *startPtr = NULL;

    SecInt ch = 0;
    int charCount;
    int comChr = 0;
    int oricomChr = 0;

    int doneCount;

    int started = 0;
    int width = 0;
    int widthSet = 0;
    int errNoMem = 0;
    int formatError = 0;
    int paraIsNull = 0;
    int numberWidth = SECUREC_NUM_WIDTH_INT;    /* 0 = SHORT, 1 = int, > 1  long or L_DOUBLE */
    int isInt64Arg = 0;         /* 1 for 64-bit integer, 0 otherwise */

    va_list arglistSave;        /* backup for arglist value, this variable don't need initialized */

#if defined(va_copy) || defined(__va_copy)
    int arglistBeenCopied = 0;
#endif

    SecChar decimal;
    SecUnsignedChar expCh;
    SecUnsignedChar last;
    SecUnsignedChar prevChar;

    signed char isWchar;     /* -1/0 not wchar, 1 for wchar */
    unsigned char tableMask;
    char suppress;
    char match;
    char doneFlag;

    doneCount = 0;
    charCount = 0;
    match = 0;
    (void)memset(&arglistSave, 0, sizeof(arglistSave));

    while (format != NULL && *format) {
        if (isspace((SecUnsignedChar) * format)) {
            SecUnsignedChar tch;
            /* eat all space chars and put fist no space char backup */
            SECUREC_UN_GET_CHAR(SECUREC_SKIP_SPACE_CHAR());
            do {
                tch = (SecUnsignedChar) * (++format);
            } while (isspace(tch));
            continue;
        }

        if (*format == SECUREC_CHAR('%')) {
            numberSpec.number = 0;
            numberSpec.negative = 0;
            prevChar = 0;
            width = 0;
            widthSet = 0;
            started = 0;
            arrayWidth = 0;
            errNoMem = 0;
            doneFlag = 0;
            suppress = 0;
            tableMask = 0;
            isWchar = 0;
            numberWidth = SECUREC_NUM_WIDTH_INT;
#if (defined(SECUREC_COMPATIBLE_LINUX_FORMAT) && !(defined(SECUREC_ON_UNIX)))
            numberSpec.beyondMax = 0;
#endif
            isInt64Arg = 0;
            numberSpec.number64 = 0;

            while (doneFlag == 0) {
                comChr = *(++format);
                if (SECUREC_IS_DIGIT((SecUnsignedChar) comChr)) {
                    widthSet = 1;
                    if (width > SECUREC_INT_MAX_DIV_TEN) {
                        formatError = 1;
                        goto ERR_RET;
                    }
                    width = (int)SECUREC_MUL10((unsigned int)width) + (comChr - SECUREC_CHAR('0'));
                } else {
                    switch (comChr) {
                    case SECUREC_CHAR('F'):    /* fall-through */ /* FALLTHRU */
                    case SECUREC_CHAR('N'):
                        break;
                    case SECUREC_CHAR('h'):
                        --numberWidth;  /* h for SHORT , hh for CHAR */
                        isWchar = -1;
                        break;
#ifdef SECUREC_COMPATIBLE_LINUX_FORMAT
                    case SECUREC_CHAR('j'):
                        numberWidth = SECUREC_NUM_WIDTH_LONG_LONG;  /* intmax_t or uintmax_t */
                        isInt64Arg = 1;
                        break;
                    case SECUREC_CHAR('t'):    /* fall-through */ /* FALLTHRU */
#endif
                    case SECUREC_CHAR('z'):
#ifdef SECUREC_ON_64BITS
                        numberWidth = SECUREC_NUM_WIDTH_LONG_LONG;
                        isInt64Arg = 1;
#else
                        numberWidth = SECUREC_NUM_WIDTH_LONG;
#endif
                        break;
                    case SECUREC_CHAR('L'):    /* long double */ /* fall-through */ /* FALLTHRU */
                    case SECUREC_CHAR('q'):
                        numberWidth = SECUREC_NUM_WIDTH_LONG_LONG;
                        isInt64Arg = 1;
                        break;
                    case SECUREC_CHAR('l'):
                        if (*(format + 1) == SECUREC_CHAR('l')) {
                            isInt64Arg = 1;
                            numberWidth = SECUREC_NUM_WIDTH_LONG_LONG;
                            ++format;
                        } else {
                            numberWidth = SECUREC_NUM_WIDTH_LONG;
#ifdef SECUREC_ON_64BITS
#if  !(defined(SECUREC_COMPATIBLE_WIN_FORMAT))  /* on window 64 system sizeof long is 32bit */
                            isInt64Arg = 1;
#endif
#endif
                            isWchar = 1;
                        }
                        break;
                    case SECUREC_CHAR('w'):
                        isWchar = 1;
                        break;

                    case SECUREC_CHAR('*'):
                        suppress = 1;
                        break;

                    case SECUREC_CHAR('I'):
                        if ((*(format + 1) == SECUREC_CHAR('6')) &&
                            (*(format + 2) == SECUREC_CHAR('4'))) {
                            isInt64Arg = 1;
                            format += 2;
                            break;
                        } else if ((*(format + 1) == SECUREC_CHAR('3')) &&
                                   (*(format + 2) == SECUREC_CHAR('2'))) {
                            format += 2;
                            break;
                        } else if ((*(format + 1) == SECUREC_CHAR('d')) ||
                                   (*(format + 1) == SECUREC_CHAR('i')) ||
                                   (*(format + 1) == SECUREC_CHAR('o')) ||
                                   (*(format + 1) == SECUREC_CHAR('x')) ||
                                   (*(format + 1) == SECUREC_CHAR('X'))) {
                            isInt64Arg = SecIs64BitPtr(sizeof(void *));
                            break;
                        }
                        isInt64Arg = SecIs64BitPtr(sizeof(void *));
                        doneFlag = 1;
                        break;
                    default:
                        doneFlag = 1;
                        break;
                    }           /* end of switch (comChr) ... */
                }
            }

            if (suppress == 0) {
                /* LSD change, for gcc compile Assign arglist to   arglistSave */
#if defined(va_copy)
                va_copy(arglistSave, arglist);
#elif defined(__va_copy)        /* for vxworks */
                __va_copy(arglistSave, arglist);
#else
                arglistSave = arglist;
#endif
                argPtr = (void *)va_arg(arglist, void *);
                SECUREC_CHECK_INPUT_ADDR(argPtr);
            } else {
                /* "argPtr = NULL" is safe, in suppress mode we don't use argPtr to store data */
                argPtr = NULL;  /* doesn't matter what value we use here - we're only using it as a flag */
            }

            doneFlag = 0;

            SecUpdateWcharFlagByType(*format, &isWchar);

            comChr = *format | (SECUREC_CHAR('a') - SECUREC_CHAR('A')); /* to lowercase */

            if (comChr != SECUREC_CHAR('n')) {
                if (comChr != SECUREC_CHAR('c') && comChr != SECUREC_BRACE) {
                    ch = SECUREC_SKIP_SPACE_CHAR();
                } else {
                    ch = SECUREC_GET_CHAR();
                }
            }

            if (comChr != SECUREC_CHAR('n')) {
                if (ch == SECUREC_EOF) {
                    goto ERR_RET;
                }
            }

            if (widthSet == 0 || width != 0) {
                if (suppress == 0 && (comChr == SECUREC_CHAR('c') ||
                    comChr == SECUREC_CHAR('s') ||
                    comChr == SECUREC_BRACE)) {

#if defined(va_copy)
                    va_copy(arglist, arglistSave);
                    va_end(arglistSave);
                    arglistBeenCopied = 1;
#elif defined(__va_copy)        /* for vxworks */
                    __va_copy(arglist, arglistSave);
                    va_end(arglistSave);
                    arglistBeenCopied = 1;
#else
                    arglist = arglistSave;
#endif
                    argPtr = (void *)va_arg(arglist, void *);
                    SECUREC_CHECK_INPUT_ADDR(argPtr);

#if defined(va_copy)
                    va_copy(arglistSave, arglist);
#elif defined(__va_copy)        /* for vxworks */
                    __va_copy(arglistSave, arglist);
#else
                    arglistSave = arglist;
#endif
                    /* Get the next argument - size of the array in characters */
#ifdef SECUREC_ON_64BITS
                    arrayWidth = ((size_t)(va_arg(arglist, size_t))) & 0xFFFFFFFFUL;
#else /* !SECUREC_ON_64BITS */
                    arrayWidth = (size_t)va_arg(arglist, size_t);
#endif

                    if (arrayWidth < 1) {

                        if (isWchar > 0) {
                            *(wchar_t UNALIGNED *)argPtr = L'\0';
                        } else {
                            *(char *)argPtr = '\0';
                        }

                        goto ERR_RET;
                    }

                    /* LSD add string maxi width protection */
                    if (isWchar > 0) {
                        if (arrayWidth > SECUREC_WCHAR_STRING_MAX_LEN) {
                            goto ERR_RET;
                        }
                    } else {
                        /* for char *buffer */
                        if (arrayWidth > SECUREC_STRING_MAX_LEN) {
                            goto ERR_RET;
                        }
                    }

                }

                oricomChr = comChr;

                switch (comChr) {
                case SECUREC_CHAR('c'):
                    /* also case 'C' */ /* fall-through */ /* FALLTHRU */
                case SECUREC_CHAR('s'):
                    /* also case 'S': */ /* fall-through */ /* FALLTHRU */
                case SECUREC_BRACE:
                    if (comChr == 'c') {
                        if (widthSet == 0) {
                            widthSet = 1;
                            width = 1;
                        }
                    } else if (comChr == 's') {
                        /* empty */
                    } else {    /* for [ */
                        const SecUnsignedChar *bracketFmtPtr = (const SecUnsignedChar *)(format);
#if !(defined(SECUREC_COMPATIBLE_WIN_FORMAT))
                        if (*format == SECUREC_CHAR('{')) {
                            goto ERR_RET;
                        }
#endif
                        /* for building "table" data */
                        ++bracketFmtPtr;

                        if (*bracketFmtPtr == SECUREC_CHAR('^')) {
                            ++bracketFmtPtr;
                            tableMask = (unsigned char)0xff;
                        }

                        /* malloc  when  first %[ is meet  for wchar version */
#ifdef SECUREC_FOR_WCHAR
                        if (bracketTable == NULL) {
                            /* LSD the table will be freed after ERR_RET label of this function */
                            bracketTable = (unsigned char *)SECUREC_MALLOC(SECUREC_BRACKET_TABLE_SIZE);
                            if (bracketTable == NULL) {
                                goto ERR_RET;
                            }
                        }
#endif
                        (void)memset(bracketTable, 0, (size_t)SECUREC_BRACKET_TABLE_SIZE);

                        if (*bracketFmtPtr == SECUREC_CHAR(']')) {
                            prevChar = SECUREC_CHAR(']');
                            ++bracketFmtPtr;

                            bracketTable[SECUREC_CHAR(']') >> 3] = 1 << (SECUREC_CHAR(']') & 7);

                        }

                        while (*bracketFmtPtr != SECUREC_CHAR('\0') && *bracketFmtPtr != SECUREC_CHAR(']')) {
                            unsigned int tmpIndex;  /* to clear warning */
                            expCh = *bracketFmtPtr++;

                            if (expCh != SECUREC_CHAR('-') || !prevChar ||  /* first char */
                                *bracketFmtPtr == SECUREC_CHAR(']')) {  /* last char */
                                prevChar = expCh;
                                /* only supports  wide characters with a maximum length of two bytes */
                                tmpIndex = (unsigned int)(int)expCh & SECUREC_CHAR_MASK;
                                /* Do not use    |= optimize this code, it will cause compiling warning */
                                bracketTable[(tmpIndex) >> 3] = (unsigned char)(bracketTable[(tmpIndex) >> 3] |
                                                                (unsigned char)(1 << (tmpIndex & 7)));
                            } else {
                                /* for %[a-z] */
                                expCh = *bracketFmtPtr++;   /* get end of range */

                                if (prevChar < expCh) { /* %[a-z] */
                                    last = expCh;
                                } else {

#if (defined(SECUREC_COMPATIBLE_WIN_FORMAT))
                                    /* %[z-a] */
                                    last = prevChar;
                                    prevChar = expCh;
#else
                                    prevChar = expCh;
                                    /* only supports  wide characters with a maximum length of two bytes */
                                    tmpIndex = (unsigned int)(int)expCh & SECUREC_CHAR_MASK;
                                    bracketTable['-' >> 3] |= (unsigned char)(1 << ('-' & 7));
                                    bracketTable[(tmpIndex) >> 3] = (unsigned char)(bracketTable[(tmpIndex) >> 3] |
                                                                    (unsigned char)(1 << (tmpIndex & 7)));
                                    continue;
#endif
                                }
                                /* format %[a-\xff] last is 0xFF, condition (rnch <= last) cause dead loop */
                                for (expCh = prevChar; expCh < last; ++expCh) {
                                    /* only supports  wide characters with a maximum length of two bytes */
                                    tmpIndex = (unsigned int)(int)expCh & SECUREC_CHAR_MASK;
                                    bracketTable[tmpIndex >> 3] = (unsigned char)(bracketTable[tmpIndex >> 3] |
                                                                  (unsigned char)(1 << (tmpIndex & 7)));
                                }
                                /* only supports  wide characters with a maximum length of two bytes */
                                tmpIndex = (unsigned int)(int)last & SECUREC_CHAR_MASK;
                                bracketTable[tmpIndex >> 3] = (unsigned char)(bracketTable[tmpIndex >> 3] |
                                                              (unsigned char)(1 << (tmpIndex & 7)));
                                prevChar = 0;
                            }
                        }

                        if (*bracketFmtPtr == SECUREC_CHAR('\0')) {
                            if (arrayWidth >= sizeof(SecChar) && argPtr) {
                                *(SecChar *) argPtr = SECUREC_CHAR('\0');
                            }
                            goto ERR_RET;   /* trunc'd format string */
                        }
                        format = bracketFmtPtr;
                    }
                    /* scanset completed.  Now read string */

                    startPtr = argPtr;

                    SECUREC_UN_GET_CHAR(ch);

                    /* One element is needed for '\0' for %s & %[ */
                    if (comChr != SECUREC_CHAR('c')) {
                        --arrayWidth;
                    }
                    while (widthSet == 0 || width-- != 0) {

                        ch = SECUREC_GET_CHAR();
                        if ((SECUREC_EOF != ch) &&
                            /* char conditions */
                            ((comChr == SECUREC_CHAR('c')) ||
                             /* string conditions !isspace() */
                             ((comChr == SECUREC_CHAR('s') &&
                               (!(ch >= SECUREC_CHAR('\t') && ch <= SECUREC_CHAR('\r')) && ch != SECUREC_CHAR(' ')))) ||
                             /* BRACKET conditions */
                             ((comChr == SECUREC_BRACE) &&
#ifdef SECUREC_FOR_WCHAR
                              bracketTable &&   /* only supports  wide characters with a maximum length of two bytes */
                              ((bracketTable[((unsigned int)(int)ch & SECUREC_CHAR_MASK) >> 3] ^ tableMask) &
                               (1 << (ch & 7)))
#else
                              ((bracketTable[((unsigned char)ch & 0xff) >> 3] ^ tableMask) & (1 << (ch & 7)))
#endif
                             ))) {
                            if (suppress == 0) {
                                if (arrayWidth == 0) {
                                    /* We have exhausted the user's buffer */

                                    errNoMem = 1;
                                    break;
                                }
                                SECUREC_CHECK_INPUT_ADDR(argPtr);

#ifdef SECUREC_FOR_WCHAR
                                if (isWchar > 0) {
                                    *(wchar_t UNALIGNED *)argPtr = (wchar_t)ch;
                                    argPtr = (wchar_t *)argPtr + 1;
                                    --arrayWidth;
                                } else {
                                    int temp = 0;
                                    if (arrayWidth >= ((size_t)MB_CUR_MAX)) {
                                        SECUREC_MASK_MSVC_CRT_WARNING
                                        temp = wctomb((char *)argPtr, (wchar_t)ch);
                                        SECUREC_END_MASK_MSVC_CRT_WARNING
                                    } else {
                                        char tmpBuf[SECUREC_MB_LEN + 1];
                                        SECUREC_MASK_MSVC_CRT_WARNING temp = wctomb(tmpBuf, (wchar_t)ch);
                                        SECUREC_END_MASK_MSVC_CRT_WARNING
                                        if (temp > 0 && ((size_t)(unsigned int)temp) > arrayWidth) {
                                            errNoMem = 1;
                                            break;
                                        }
                                        if (temp > 0 && ((size_t)(unsigned int)temp) <= sizeof(tmpBuf)) {
                                            if (EOK != memcpy_s(argPtr, arrayWidth,
                                                                tmpBuf, (size_t)(unsigned int)temp)) {
                                                errNoMem = 1;
                                                break;
                                            }
                                        }
                                    }
                                    if (temp > 0) {
                                        /* if wctomb  error, then ignore character */
                                        argPtr = (char *)argPtr + temp;
                                        arrayWidth -= (size_t)(unsigned int)temp;
                                    }
                                }
#else

                                if (isWchar > 0) {
                                    wchar_t tempWchar = L'?';   /* set default char as ? */
                                    char temp[SECUREC_MULTI_BYTE_MAX_LEN + 1];
                                    temp[0] = (char)ch;
                                    temp[1] = '\0';
#if defined(SECUREC_COMPATIBLE_WIN_FORMAT)
                                    if (SecIsleadbyte(ch)) {
                                        temp[1] = (char)SECUREC_GET_CHAR();
                                        temp[2] = '\0';
                                    }
                                    if (mbtowc(&tempWchar, temp, sizeof(temp)) <= 0) {
                                        /* no string termination error for tool */
                                        tempWchar = L'?';
                                    }
#else
                                    if (SecIsleadbyte(ch)) {
                                        int convRes = 0;
                                        int di = 1;
                                        /* in Linux like system, the string is encoded in UTF-8 */
                                        while (di < (int)MB_CUR_MAX && di < SECUREC_MULTI_BYTE_MAX_LEN) {
                                            temp[di++] = (char)SECUREC_GET_CHAR();
                                            temp[di] = '\0';
                                            convRes = mbtowc(&tempWchar, temp, sizeof(temp));
                                            if (convRes > 0) {
                                                break;  /* convert succeed */
                                            }
                                        }
                                        if (convRes <= 0) {
                                            tempWchar = L'?';
                                        }
                                    } else {
                                        if (mbtowc(&tempWchar, temp, sizeof(temp)) <= 0) {
                                            /* no string termination error for tool */
                                            tempWchar = L'?';
                                        }
                                    }
#endif

                                    *(wchar_t UNALIGNED *)argPtr = tempWchar;
                                    /* just copy L'?' if mbtowc fails, errno is set by mbtowc */
                                    argPtr = (wchar_t *)argPtr + 1;
                                    --arrayWidth;

                                 } else {
                                    *(char *)argPtr = (char)ch;
                                    argPtr = (char *)argPtr + 1;
                                    --arrayWidth;
                                 }
#endif
                            } else {
                                /* suppress */
                                /* this is OK ,Used to identify processed data for %* ,use size_t just  clear e613 */
                                startPtr = (SecChar *) (size_t)1 + (size_t)argPtr;
                            }
                        } else {
                            SECUREC_UN_GET_CHAR(ch);
                            break;
                        }
                    }

                    if (errNoMem != 0) {
                        /* In case of error, blank out the input buffer */
                        if (isWchar > 0) {
                            if (startPtr != NULL) {
                                *(wchar_t UNALIGNED *)startPtr = 0;
                            }
                        } else {
                            if (startPtr != NULL) {
                                *(char *)startPtr = 0;
                            }
                        }

                        goto ERR_RET;
                    }

                    if (startPtr != argPtr) {
                        if (suppress == 0) {

                            SECUREC_CHECK_INPUT_ADDR(argPtr);

                            if (comChr != 'c') {
                                /* null-terminate strings */
                                if (isWchar > 0) {
                                    *(wchar_t UNALIGNED *)argPtr = L'\0';
                                } else {
                                    *(char *)argPtr = '\0';
                                }
                            }
                            ++doneCount;
                        }

                    } else {
                        goto ERR_RET;
                    }

                    break;
                case SECUREC_CHAR('p'):
                    /* make %hp same as %p */
                    numberWidth = SECUREC_NUM_WIDTH_INT;
#ifdef SECUREC_ON_64BITS
                    isInt64Arg = 1;
#endif
                    /* fall-through */
                    /* FALLTHRU */
                case SECUREC_CHAR('o'):    /* fall-through */ /* FALLTHRU */
                case SECUREC_CHAR('u'):    /* fall-through */ /* FALLTHRU */
                case SECUREC_CHAR('d'):    /* fall-through */ /* FALLTHRU */
                case SECUREC_CHAR('i'):    /* fall-through */ /* FALLTHRU */
                case SECUREC_CHAR('x'):
                    if (ch == SECUREC_CHAR('+') || ch == SECUREC_CHAR('-')) {
                        if (ch == SECUREC_CHAR('-')) {
                            numberSpec.negative = 1;
                        }
                        if (--width == 0 && widthSet != 0) {
                            doneFlag = 1;
                        } else {
                            ch = SECUREC_GET_CHAR();
                        }
                    }

                    if (comChr == SECUREC_CHAR('x') || comChr == SECUREC_CHAR('i')) {
                        if (comChr == SECUREC_CHAR('i')) {
                            /* i could be d, o, or x, use d as default */
                            comChr = SECUREC_CHAR('d');
                        }
                        if (ch == SECUREC_CHAR('0')) {
                            ch = SECUREC_GET_CHAR();
                            if ((SecChar) (ch) == SECUREC_CHAR('x') || (SecChar) ch == SECUREC_CHAR('X')) {
                                ch = SECUREC_GET_CHAR();
                                if (widthSet != 0) {
                                    width -= 2;
                                    if (width < 1) {
                                        doneFlag = 1;
                                    }
                                }
                                comChr = SECUREC_CHAR('x');
                            } else {
                                started = 1;
                                if (comChr != SECUREC_CHAR('x')) {
                                    if (widthSet != 0 && --width == 0) {
                                        doneFlag = 1;
                                    }
                                    comChr = SECUREC_CHAR('o');
                                } else {
                                    SECUREC_UN_GET_CHAR(ch);
                                    ch = SECUREC_CHAR('0');
                                }
                            }
                        }
                    }

                    /* scanNumber: */
                    if (isInt64Arg != 0) {
                        while (doneFlag == 0) {
                            /* decode ch to number64 */
                            doneFlag = SecDecodeNumber64(comChr, ch, &numberSpec);

                            if (doneFlag == 0) {
                                started = 1;

                                if (widthSet != 0 && --width == 0) {
                                    doneFlag = 1;
                                } else {
                                    ch = SECUREC_GET_CHAR();
                                }
                            } else {
                                SECUREC_UN_GET_CHAR(ch);
                            }

                        }       /* end of WHILE loop */

                        /* Handling integer negative numbers and beyond max */

                        SecFinishNumber64(oricomChr, &numberSpec);

                    }
                    /* do not use else , Otherwise, the vxworks55 arm926ej compiler will crash. */
                    if (isInt64Arg == 0) {
                        while (doneFlag == 0) {
                            /* decode ch to number */
                            doneFlag = SecDecodeNumber(comChr, ch, &numberSpec);
                            if (doneFlag == 0) {
                                started = 1;

                                if (widthSet != 0 && --width == 0) {
                                    doneFlag = 1;
                                } else {
                                    ch = SECUREC_GET_CHAR();
                                }
                            } else {
                                SECUREC_UN_GET_CHAR(ch);
                            }
                        }       /* end of WHILE loop */

                        /* Handling integer negative numbers and beyond max */

                        SecFinishNumber(oricomChr, numberWidth, &numberSpec);

                    }

                    if (comChr == SECUREC_CHAR('F')) {  /* expected ':' in long pointer */
                        started = 0;
                    }

                    if (started != 0) {
                        if (suppress == 0) {
                            SECUREC_CHECK_INPUT_ADDR(argPtr);

                            if (isInt64Arg != 0) {
#if defined(SECUREC_VXWORKS_PLATFORM)
                                /* take number64 as unsigned number */
                                *(SecInt64 UNALIGNED *)argPtr = *(SecUnsignedInt64 *)(&numberSpec.number64);
#else
                                /* take number64 as unsigned number */
                                *(SecInt64 UNALIGNED *)argPtr = (SecInt64)numberSpec.number64;
#endif
                            } else {
                                SecAssignNumber(argPtr, numberWidth, numberSpec.number);
                            }
                            ++doneCount;
                        }
                        /* remove blank else */
                    } else {
                        goto ERR_RET;
                    }
                    break;

                case SECUREC_CHAR('n'):    /* char count */
                    if (suppress == 0) {
                        SECUREC_CHECK_INPUT_ADDR(argPtr);
                        SecAssignNumber(argPtr, numberWidth, (unsigned long)(unsigned int)charCount);
                    }
                    break;

                case SECUREC_CHAR('e'):    /* fall-through */ /* FALLTHRU */
                case SECUREC_CHAR('f'):    /* fall-through */ /* FALLTHRU */
                case SECUREC_CHAR('g'):    /* scan a float */

                    floatStrUsedLen = 0;

                    if (ch == SECUREC_CHAR('-')) {
                        pFloatStr[floatStrUsedLen++] = SECUREC_CHAR('-');
                        --width;
                        ch = SECUREC_GET_CHAR();

                    } else if (ch == SECUREC_CHAR('+')) {
                        --width;
                        ch = SECUREC_GET_CHAR();
                    }

                    if (widthSet == 0) {    /* must care width */
                        width = -1;
                    }

                    /* now get integral part */
                    while (SECUREC_IS_DIGIT(ch) && width-- != 0) {
                        started = 1;
                        pFloatStr[floatStrUsedLen++] = (SecChar) ch;    /* ch must be '0' - '9' */
                        if (SecUpdateFloatString(floatStrUsedLen,
                                                 &floatStrSize, &pFloatStr, floatStrBuffer, &pAllocedFloatStr) == 0) {
                            goto ERR_RET;
                        }
                        ch = SECUREC_GET_CHAR();
                    }

#ifdef SECUREC_FOR_WCHAR
                    /* convert decimal point(.) to wide-char */
                    decimal = L'.';
                    if (mbtowc(&decimal, SECUREC_DECIMAL_POINT_PTR, (size_t)MB_CUR_MAX) <= 0) {
                        decimal = L'.';
                    }
#else
                    decimal = *SECUREC_DECIMAL_POINT_PTR;   /* if locale.h NOT exist, let decimal = '.' */
#endif

                    if (SecCheckFloatDecPoint((char)decimal) != 1) {
                        goto ERR_RET;
                    }

                    /* now check for decimal */
                    if (decimal == (char)ch && width-- != 0) {
                        ch = SECUREC_GET_CHAR();
                        pFloatStr[floatStrUsedLen++] = decimal;
                        if (SecUpdateFloatString(floatStrUsedLen,
                                                 &floatStrSize, &pFloatStr, floatStrBuffer, &pAllocedFloatStr) == 0) {
                            goto ERR_RET;
                        }
                        while (SECUREC_IS_DIGIT(ch) && width-- != 0) {
                            started = 1;
                            pFloatStr[floatStrUsedLen++] = (SecChar) ch;
                            if (SecUpdateFloatString(floatStrUsedLen,
                                                     &floatStrSize,
                                                     &pFloatStr, floatStrBuffer, &pAllocedFloatStr) == 0) {
                                goto ERR_RET;
                            }
                            ch = SECUREC_GET_CHAR();
                        }
                    }

                    /* now check for exponent */

                    if (started != 0 && (ch == SECUREC_CHAR('e') || ch == SECUREC_CHAR('E')) && width-- != 0) {
                        pFloatStr[floatStrUsedLen++] = SECUREC_CHAR('e');
                        if (SecUpdateFloatString(floatStrUsedLen,
                                                 &floatStrSize, &pFloatStr, floatStrBuffer, &pAllocedFloatStr) == 0) {
                            goto ERR_RET;
                        }

                        ch = SECUREC_GET_CHAR();
                        if (ch == SECUREC_CHAR('+') || ch == SECUREC_CHAR('-')) {
                            if (ch == SECUREC_CHAR('-')) {

                                pFloatStr[floatStrUsedLen++] = SECUREC_CHAR('-');
                                if (SecUpdateFloatString(floatStrUsedLen,
                                                         &floatStrSize,
                                                         &pFloatStr, floatStrBuffer, &pAllocedFloatStr) == 0) {
                                    goto ERR_RET;
                                }
                            }

                            if (width != 0) {
                                ch = SECUREC_GET_CHAR();
                                --width;
                            }
                        }

                        while (SECUREC_IS_DIGIT(ch) && width-- != 0) {
                            pFloatStr[floatStrUsedLen++] = (SecChar) ch;
                            if (SecUpdateFloatString(floatStrUsedLen,
                                                     &floatStrSize,
                                                     &pFloatStr, floatStrBuffer, &pAllocedFloatStr) == 0) {
                                goto ERR_RET;
                            }
                            ch = SECUREC_GET_CHAR();
                        }

                    }

                    SECUREC_UN_GET_CHAR(ch);

                    if (started != 0) {
                        if (suppress == 0) {
                            SECUREC_CHECK_INPUT_ADDR(argPtr);

                            /* Make sure  have a string terminator */
                            pFloatStr[floatStrUsedLen] = SECUREC_CHAR('\0');
#ifdef SECUREC_FOR_WCHAR
                            {
                                /* convert float string */
                                size_t mbsLen;
                                size_t tempFloatStrLen = (size_t)(floatStrSize + 1) * sizeof(wchar_t);
                                char *tempFloatStr = (char *)SECUREC_MALLOC(tempFloatStrLen);

                                if (tempFloatStr == NULL) {
                                    goto ERR_RET;
                                }
                                tempFloatStr[0] = '\0';
                                SECUREC_MASK_MSVC_CRT_WARNING
                                mbsLen = wcstombs(tempFloatStr, pFloatStr, tempFloatStrLen - 1);
                                SECUREC_END_MASK_MSVC_CRT_WARNING
                                if (mbsLen != (size_t)-1) {
                                    tempFloatStr[mbsLen] = '\0';
                                    SecAssignFloat(numberWidth, argPtr, tempFloatStr);
                                } else {
                                    SECUREC_FREE(tempFloatStr);
                                    goto ERR_RET;
                                }
                                SECUREC_FREE(tempFloatStr);
                            }
#else
                            SecAssignFloat(numberWidth, argPtr, pFloatStr);
#endif
                            ++doneCount;
                        }
                        /* remove blank else */ /* NULL */
                    } else {
                        goto ERR_RET;
                    }

                    break;
                default:
                    if ((int)*format != (int)ch) {
                        SECUREC_UN_GET_CHAR(ch);
                        /* to clear e438 last value assigned not used , the compiler will optimize this code */
                        (void)charCount;
                        formatError = 1;
                        goto ERR_RET;
                    } else {
                        --match;
                    }

                    if (suppress == 0) {
#if defined(va_copy)
                        va_copy(arglist, arglistSave);
                        arglistBeenCopied = 1;
                        va_end(arglistSave);
#elif defined(__va_copy)        /* for vxworks */
                        __va_copy(arglist, arglistSave);
                        arglistBeenCopied = 1;
                        va_end(arglistSave);
#else
                        arglist = arglistSave;
#endif
                    }
                }

                ++match;

            }

            else {
                /* 0 width in format */
                SECUREC_UN_GET_CHAR(ch);
                /* to clear e438 last value assigned not used , the compiler will optimize this code */
                (void)charCount;
                goto ERR_RET;
            }

            ++format;
        } else {
            ch = SECUREC_GET_CHAR();
            if ((int)*format++ != (int)(ch)) {
                SECUREC_UN_GET_CHAR(ch);
                /* to clear e438 last value assigned not used , the compiler will optimize this code */
                (void)charCount;
                goto ERR_RET;
            }
#ifndef SECUREC_FOR_WCHAR
            /* The purpose of type conversion is to avoid warnings */
            if (SecIsleadbyte(ch)) {
                char temp[SECUREC_MULTI_BYTE_MAX_LEN];
                wchar_t tempWchar = L'\0';
                int ch2 = SECUREC_GET_CHAR();

                if ((int)*format++ != (ch2)) {
                    SECUREC_UN_GET_CHAR(ch2);   /* LSD in console mode, ungetc twice will cause problem */
                    SECUREC_UN_GET_CHAR(ch);
                    /* to clear e438 last value assigned not used , the compiler will optimize this code */
                    (void)charCount;
                    goto ERR_RET;
                }
                if (MB_CUR_MAX > 2 && (((unsigned char)ch & 0xE0) == 0xE0) && (((unsigned char)ch2 & 0x80) == 0x80)) {
                    /* this char is very likely to be a UTF-8 char */
                    int ch3 = SECUREC_GET_CHAR();
                    temp[0] = (char)ch;
                    temp[1] = (char)ch2;
                    temp[2] = (char)ch3;
                    temp[3] = '\0';

                    if (mbtowc(&tempWchar, temp, sizeof(temp)) > 0) {
                        /* succeed */
                        if ((int)*format++ != (int)ch3) {
                            SECUREC_UN_GET_CHAR(ch3);
                            /* to clear e438 last value assigned not used , the compiler will optimize this code */
                            (void)charCount;
                            goto ERR_RET;
                        }
                        --charCount;
                    } else {
                        SECUREC_UN_GET_CHAR(ch3);
                    }
                }
                --charCount;    /* only count as one character read */
            }
#endif /* SECUREC_FOR_WCHAR */
        }

        if ((ch == SECUREC_EOF) && ((*format != SECUREC_CHAR('%')) || (*(format + 1) != SECUREC_CHAR('n')))) {
            break;
        }

    }

ERR_RET:
#ifdef SECUREC_FOR_WCHAR
    if (bracketTable != NULL) {
        SECUREC_FREE(bracketTable);
        bracketTable = NULL;
        (void)bracketTable;     /* to clear e438 last value assigned not used , the compiler will optimize this code */
    }
#endif

#if defined(va_copy) || defined(__va_copy)
    if (arglistBeenCopied != 0) {
        va_end(arglist);
        (void)arglist;          /* to clear e438 last value assigned not used , the compiler will optimize this code */
    }
#endif
    va_end(arglistSave);
    (void)arglistSave;          /* to clear e438 last value assigned not used , the compiler will optimize this code */

    /* LSD 2014.3.6 add, clear the stack data */
    if (memset_s(floatStrBuffer, sizeof(floatStrBuffer), 0, sizeof(floatStrBuffer)) != EOK) {
        doneCount = 0;          /* This is a dead code, just to meet the coding requirements */
    }
    if (pAllocedFloatStr != NULL) {
        /* pFloatStr can be allocated in SecUpdateFloatString function, clear and free it */
        if (memset_s(pAllocedFloatStr, floatStrSize * sizeof(SecChar), 0, floatStrSize * sizeof(SecChar)) != EOK) {
            doneCount = 0;      /* This is a dead code, just to meet the coding requirements */
        }
        SECUREC_FREE(pAllocedFloatStr);
        pAllocedFloatStr = NULL;
        (void)pAllocedFloatStr; /* to clear e438 last value assigned not used , the compiler will optimize this code */
    }

    SecAdjustStream(stream);

    if (ch == SECUREC_EOF) {
        return ((doneCount || match) ? doneCount : EOF);
    } else if (formatError != 0 || paraIsNull != 0) {
        /* Invalid Input Format or parameter */
        return SECUREC_SCANF_ERROR_PARA;
    }

    return doneCount;
}

#ifdef SECUREC_FOR_WCHAR
static SecInt SecGetCharW(SecFileStream *str)
#else
static SecInt SecGetChar(SecFileStream *str)
#endif
{

    SecInt ch = 0;
    int firstReadOnFile = 0;
    do {
        if ((str->flag & SECUREC_FROM_STDIN_FLAG) > 0) {
#if defined(SECUREC_NO_STD_UNGETC)
            if (str->fUnget == 1) {
                ch = (SecInt) str->lastChar;
                str->fUnget = 0;

            } else {
                ch = SECUREC_GETC(str->pf);
                str->lastChar = (unsigned int)ch;

            }
#else
            ch = SECUREC_GETC(str->pf);
#endif
            break;
        } else if ((str->flag & SECUREC_FILE_STREAM_FLAG) > 0 && str->count == 0) {
            /* load file to buffer */
            if (str->base == NULL) {
                str->base = (char *)SECUREC_MALLOC(SECUREC_BUFFERED_BLOK_SIZE + 1);
                if (str->base == NULL) {
                    ch = SECUREC_EOF;
                    break;
                }
                str->base[SECUREC_BUFFERED_BLOK_SIZE] = '\0';   /* for tool Warning string null */
            }
            /* LSD add 2014.3.21 */
            if (str->oriFilePos == SECUREC_UNINITIALIZED_FILE_POS) {
                str->oriFilePos = ftell(str->pf);   /* save original file read position */
                firstReadOnFile = 1;
            }
            str->count = (int)fread(str->base, (size_t)1, (size_t)SECUREC_BUFFERED_BLOK_SIZE, str->pf);
            str->base[SECUREC_BUFFERED_BLOK_SIZE] = '\0';   /* for tool Warning string null */
            if (str->count == 0 || str->count > SECUREC_BUFFERED_BLOK_SIZE) {
                ch = SECUREC_EOF;
                break;
            }
            str->cur = str->base;
            str->flag |= SECUREC_LOAD_FILE_TO_MEM_FLAG;
            if (firstReadOnFile != 0) {
#ifdef SECUREC_FOR_WCHAR

                if (str->count > 1
                    && (((unsigned char)(str->base[0]) == 0xFFU && (unsigned char)(str->base[1]) == 0xFEU)
                        || ((unsigned char)(str->base[0]) == 0xFEU && (unsigned char)(str->base[1]) == 0xFFU))) {
                    /* it's BOM header, UNICODE little endian */
                    str->count -= SECUREC_BOM_HEADER_SIZE;
                    if (memmove_s(str->base, (size_t)SECUREC_BUFFERED_BLOK_SIZE,
                                  str->base + SECUREC_BOM_HEADER_SIZE, (size_t)(unsigned int)str->count) != EOK) {
                        ch = SECUREC_EOF;
                        break;
                    }

                    if (str->count % (int)sizeof(SecChar)) {
                        /* the str->count must be a  multiple of  sizeof(SecChar),
                         * otherwise this function will return SECUREC_EOF when read the last character
                         */
                        int ret = (int)fread(str->base + str->count, (size_t)1,
                                             (size_t)SECUREC_BOM_HEADER_SIZE, str->pf);
                        if (ret > 0 && ret <= SECUREC_BUFFERED_BLOK_SIZE) {
                            str->count += ret;
                        }
                    }
                }

#else
                if (str->count > 2 && (unsigned char)(str->base[0]) == 0xEFU &&
                    (unsigned char)(str->base[1]) == 0xBBU && (unsigned char)(str->base[2]) == 0xBFU) {
                    /* it's BOM header,  little endian */
                    str->count -= SECUREC_UTF8_BOM_HEADER_SIZE;
                    str->cur += SECUREC_UTF8_BOM_HEADER_SIZE;
                }
#endif
            }
        }

        if ((str->flag & SECUREC_MEM_STR_FLAG) > 0 || (str->flag & SECUREC_LOAD_FILE_TO_MEM_FLAG) > 0) {
            /* according  wchar_t has two bytes */
            ch = (SecInt)((str->count -= (int)sizeof(SecChar)) >= 0 ? \
                          (SecInt) (SECUREC_CHAR_MASK & *((const SecChar *)(const void *)str->cur)) : SECUREC_EOF);
            str->cur += sizeof(SecChar);
        }
        /* use break in do-while to skip some code */
    } SECUREC_WHILE_ZERO;


    if (ch != SECUREC_EOF && (str->flag & SECUREC_FILE_STREAM_FLAG) > 0 && str->base) {
        str->fileRealRead += (int)sizeof(SecChar);
    }
    return ch;

}

static void SecUnGetCharImpl(SecInt chr, SecFileStream *str)
{
    if ((str->flag & SECUREC_FROM_STDIN_FLAG) > 0) {
#if defined(SECUREC_NO_STD_UNGETC)
        str->lastChar = (unsigned int)chr;
        str->fUnget = 1;
#else
        (void)SECUREC_UN_GETC(chr, str->pf);
#endif
    } else if ((str->flag & SECUREC_MEM_STR_FLAG) || (str->flag & SECUREC_LOAD_FILE_TO_MEM_FLAG) > 0) {
        if (str->cur > str->base) {
            str->cur -= sizeof(SecChar);
            str->count += (int)sizeof(SecChar);
        }
    }

    if ((str->flag & SECUREC_FILE_STREAM_FLAG) > 0 && str->base) {
        /* LSD fix, change from -- str->fileRealRead to str->fileRealRead -= sizeof(SecChar). 2014.2.21 */
        str->fileRealRead -= (int)sizeof(SecChar);
    }

}

#ifdef SECUREC_FOR_WCHAR
static void SecUnGetCharW(SecInt chr, SecFileStream *str)
#else
static void SecUnGetChar(SecInt chr, SecFileStream *str)
#endif
{
    if (chr != SECUREC_EOF) {
        SecUnGetCharImpl(chr, str);
    }
}

static SecInt SecSkipSpaceChar(int *counter, SecFileStream *fileptr)
{
    SecInt ch;

    do {
        ++(*counter);
#ifdef SECUREC_FOR_WCHAR
        ch = SecGetCharW(fileptr);
#else
        ch = SecGetChar(fileptr);
#endif
        if (ch == SECUREC_EOF) {
            break;
        }
    }
#ifdef SECUREC_FOR_WCHAR
    while (iswspace((wint_t) ch));
#else
    while (isspace((SecUnsignedChar) ch));
#endif
    return ch;
}

#endif /* __INPUT_INL__5D13A042_DC3F_4ED9_A8D1_882811274C27 */


