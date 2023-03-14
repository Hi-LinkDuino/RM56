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

/* Avoid duplicate header files,not include securecutil.h */
#include "securecutil.h"
#if SECUREC_IN_KERNEL == 0
#include "secureprintoutput.h"
#endif


#if SECUREC_IN_KERNEL == 0
/* put a char to output */
#define SECUREC_PUTC(_c, _stream)    ((--(_stream)->count >= 0) ? ((*(_stream)->cur++ = (char)(_c)) & 0xff) : EOF)
/* to clear e835 */
#define SECUREC_PUTC_ZERO(_stream)    ((--(_stream)->count >= 0) ? ((*(_stream)->cur++ = (char)('\0'))) : EOF)

int SecPutWcharStrEndingZero(SecPrintfStream *str, int zeroNum)
{
    int succeed = 0;
    int i;

    for (i = 0; i < zeroNum && (SECUREC_PUTC_ZERO(str) != EOF); ++i) {
    }
    if (i == zeroNum) {
        succeed = 1;
    }
    return succeed;
}

int SecVsnprintfImpl(char *string, size_t count, const char *format, va_list arglist)
{
    SecPrintfStream str;
    int retVal;

    str.count = (int)count;     /* this count include \0 character */
    str.cur = string;

    retVal = SecOutputS(&str, format, arglist);
    if ((retVal >= 0) && (SECUREC_PUTC_ZERO(&str) != EOF)) {
        return (retVal);
    } else if (str.count < 0) {
        /* the buffer was too small; we return truncation */
        string[count - 1] = 0;
        return SECUREC_PRINTF_TRUNCATE;
    }

    return -1;
}

void SecWriteMultiChar(char ch, int num, SecPrintfStream *f, int *pnumwritten)
{
    int count = num;
    while (count-- > 0) {
        if (SECUREC_PUTC(ch, f) == EOF) {
            *pnumwritten = -1;
            break;
        } else {
            ++(*pnumwritten);
        }
    }
}

void SecWriteString(const char *string, int len, SecPrintfStream *f, int *pnumwritten)
{
    const char *str = string;
    int count = len;
    while (count-- > 0) {
        if (SECUREC_PUTC(*str, f) == EOF) {
            *pnumwritten = -1;
            break;
        } else {
            ++(*pnumwritten);
            ++str;
        }
    }
}
#endif

/* Following function "U64Div32" realized the operation of division between an unsigned 64-bits
 *     number and an unsigned 32-bits number.
 * these codes are contributed by Dopra team in syslib.
 */
#if defined(SECUREC_VXWORKS_VERSION_5_4)

#define SECUREC_MAX_SHIFT_NUM           32
#define SECUREC_MASK_BIT_ALL            0xFFFFFFFF
#define SECUREC_MASK_BIT_32             0x80000000
#define SECUREC_MASK_BIT_01             0x00000001
#define SECUREC_MASK_HI_NBITS(x)        (SECUREC_MASK_BIT_ALL << (SECUREC_MAX_SHIFT_NUM - (x)))

typedef enum {
    SEC_BIT64_GREAT,
    SEC_BIT64_EQUAL,
    SEC_BIT64_LESS
} SecCompareResult;

static void SecBigIntSub(SecUnsignedInt32 *aHi, SecUnsignedInt32 *aLo, const SecUnsignedInt32 bHi,
                         const SecUnsignedInt32 bLo)
{
    if (*aLo < bLo) {
        *aHi -= (bHi + 1);
    } else {
        *aHi -= (bHi);
    }
    *aLo -= bLo;
}

static SecCompareResult SecBigIntCompare(const SecUnsignedInt32 aHi, const SecUnsignedInt32 aLo,
                                         const SecUnsignedInt32 bHi, const SecUnsignedInt32 bLo)
{
    if (aHi > bHi) {
        return SEC_BIT64_GREAT;
    } else if ((aHi == bHi) && (aLo > bLo)) {
        return SEC_BIT64_GREAT;
    } else if ((aHi == bHi) && (aLo == bLo)) {
        return SEC_BIT64_EQUAL;
    } else {
        return SEC_BIT64_LESS;
    }
}

static void SecU64Div64Ret(SecUnsignedInt32 tmpQuoHi, SecUnsignedInt32 tmpQuoLo,
                           SecUnsignedInt32 tmpDividendHi, SecUnsignedInt32 tmpDividendLo,
                           SecUnsignedInt32 *pQuotientHigh, SecUnsignedInt32 *pQuotientLow,
                           SecUnsignedInt32 *pRemainderHigh, SecUnsignedInt32 *pRemainderLow)
{
    *pQuotientHigh = tmpQuoHi;
    *pQuotientLow = tmpQuoLo;

    if ((pRemainderHigh != NULL)
        && (pRemainderLow != NULL)) {
        *pRemainderHigh = tmpDividendHi;
        *pRemainderLow = tmpDividendLo;
    }
    return;
}

static int SecU64Div64(SecUnsignedInt32 dividendHigh, SecUnsignedInt32 dividendLow,
                       SecUnsignedInt32 divisorHigh, SecUnsignedInt32 divisorLow,
                       SecUnsignedInt32 *pQuotientHigh, SecUnsignedInt32 *pQuotientLow,
                       SecUnsignedInt32 *pRemainderHigh, SecUnsignedInt32 *pRemainderLow)
{
    signed char scShiftNumHi = 0;
    signed char scShiftNumLo = 0;
    SecUnsignedInt32 tmpQuoHi;
    SecUnsignedInt32 tmpQuoLo;
    SecUnsignedInt32 tmpDividendHi;
    SecUnsignedInt32 tmpDividendLo;
    SecUnsignedInt32 tmpDivisorHi;
    SecUnsignedInt32 tmpDivisorLo;
    SecCompareResult etmpResult;

    if ((pQuotientHigh == NULL) || (pQuotientLow == NULL)) {
        return -1;
    }

    if (divisorHigh == 0) {
        if (divisorLow == 0) {
            return -1;
        } else if (divisorLow == 1) {
            *pQuotientHigh = dividendHigh;
            *pQuotientLow = dividendLow;

            if (pRemainderHigh != NULL && pRemainderLow != NULL) {
                *pRemainderHigh = 0;
                *pRemainderLow = 0;
            }

            return 0;
        }
    }

    tmpQuoHi = tmpQuoLo = 0;
    tmpDividendHi = dividendHigh;
    tmpDividendLo = dividendLow;

    /* if divisor is larger than dividend, quotient equals to zero,
     * remainder equals to dividends */
    etmpResult = SecBigIntCompare(dividendHigh, dividendLow, divisorHigh, divisorLow);

    if (etmpResult == SEC_BIT64_LESS) {
        SecU64Div64Ret(tmpQuoHi, tmpQuoLo,
                       tmpDividendHi, tmpDividendLo, pQuotientHigh, pQuotientLow, pRemainderHigh, pRemainderLow);
        return 0;
    }

    else if (etmpResult == SEC_BIT64_EQUAL) {
        *pQuotientHigh = 0;
        *pQuotientLow = 1;

        if ((pRemainderHigh != NULL) && (pRemainderLow != NULL)) {
            *pRemainderHigh = 0;
            *pRemainderLow = 0;
        }

        return 0;
    }

    /* get shift number to implement divide arithmetic */
    if (divisorHigh > 0) {
        for (scShiftNumHi = 0; scShiftNumHi < SECUREC_MAX_SHIFT_NUM; scShiftNumHi++) {
            if ((divisorHigh << (SecUnsignedInt32)(unsigned char)scShiftNumHi) & SECUREC_MASK_BIT_32) {
                break;
            }
        }
    } else {
        for (scShiftNumLo = 0; scShiftNumLo < SECUREC_MAX_SHIFT_NUM; scShiftNumLo++) {
            if ((divisorLow << scShiftNumLo) & SECUREC_MASK_BIT_32) {
                break;
            }
        }
    }

    if (divisorHigh > 0) {
        /* divisor's high 32 bits doesn't equal to zero */

        for (; scShiftNumHi >= 0; scShiftNumHi--) {

            if (scShiftNumHi == 0) {
                tmpDivisorHi = divisorHigh;
            } else {
                tmpDivisorHi = (divisorHigh << (SecUnsignedInt32)(unsigned char)scShiftNumHi)
                    | (divisorLow >> (SECUREC_MAX_SHIFT_NUM - scShiftNumHi));
            }

            tmpDivisorLo = divisorLow << (SecUnsignedInt32)(unsigned char)scShiftNumHi;

            etmpResult = SecBigIntCompare(tmpDividendHi, tmpDividendLo, tmpDivisorHi, tmpDivisorLo);

            if (etmpResult != SEC_BIT64_LESS) {
                SecBigIntSub(&tmpDividendHi, &tmpDividendLo, tmpDivisorHi, tmpDivisorLo);

                tmpQuoLo |= (SecUnsignedInt32)(1 << (SecUnsignedInt32)(unsigned char)scShiftNumHi);

                if ((tmpDividendHi == 0) && (tmpDividendLo == 0)) {
                    SecU64Div64Ret(tmpQuoHi, tmpQuoLo,
                                   tmpDividendHi, tmpDividendLo,
                                   pQuotientHigh, pQuotientLow, pRemainderHigh, pRemainderLow);
                    return 0;
                }
            }
            if (scShiftNumHi == 0) {
                break;
            }
        }

    } else {
        /* divisor's high 32 bits equals to zero */

        scShiftNumHi = scShiftNumLo;

        for (; scShiftNumHi >= 0; scShiftNumHi--) {
            tmpDivisorHi = divisorLow << (SecUnsignedInt32)(unsigned char)scShiftNumHi;
            etmpResult = SecBigIntCompare(tmpDividendHi, tmpDividendLo, tmpDivisorHi, 0);

            if (etmpResult != SEC_BIT64_LESS) {
                SecUnsignedInt32 tmp = 0;
                SecBigIntSub(&tmpDividendHi, &tmpDividendLo, tmpDivisorHi, tmp);

                tmpQuoHi |= (SecUnsignedInt32)(1 << (SecUnsignedInt32)(unsigned char)scShiftNumHi);

                if ((tmpDividendHi == 0) && (tmpDividendLo == 0)) {
                    SecU64Div64Ret(tmpQuoHi, tmpQuoLo, tmpDividendHi, tmpDividendLo,
                                   pQuotientHigh, pQuotientLow, pRemainderHigh, pRemainderLow);
                    return 0;
                }
            }
            if (scShiftNumHi == 0) {
                break;
            }
        }

        for (scShiftNumHi = SECUREC_MAX_SHIFT_NUM - 1; scShiftNumHi >= 0; scShiftNumHi--) {
            if (scShiftNumHi == 0) {
                tmpDivisorHi = 0;
            } else {
                tmpDivisorHi = divisorLow >> (SECUREC_MAX_SHIFT_NUM - scShiftNumHi);
            }

            tmpDivisorLo = divisorLow << (SecUnsignedInt32)(unsigned char)scShiftNumHi;

            etmpResult = SecBigIntCompare(tmpDividendHi, tmpDividendLo, tmpDivisorHi, tmpDivisorLo);

            if (etmpResult != SEC_BIT64_LESS) {
                SecBigIntSub(&tmpDividendHi, &tmpDividendLo, tmpDivisorHi, tmpDivisorLo);

                tmpQuoLo |= (SecUnsignedInt32)(1 << (SecUnsignedInt32)(unsigned char)scShiftNumHi);

                if ((tmpDividendHi == 0) && (tmpDividendLo == 0)) {
                    SecU64Div64Ret(tmpQuoHi, tmpQuoLo, tmpDividendHi, tmpDividendLo,
                                   pQuotientHigh, pQuotientLow, pRemainderHigh, pRemainderLow);
                    return 0;
                }
            }
            if (scShiftNumHi == 0) {
                break;
            }
        }

    }

    SecU64Div64Ret(tmpQuoHi, tmpQuoLo,
                   tmpDividendHi, tmpDividendLo, pQuotientHigh, pQuotientLow, pRemainderHigh, pRemainderLow);
    return 0;
}

int SecU64Div32(SecUnsignedInt32 dividendHigh, SecUnsignedInt32 dividendLow, SecUnsignedInt32 divisor,
                SecUnsignedInt32 *pQuotientHigh, SecUnsignedInt32 *pQuotientLow, SecUnsignedInt32 *puiRemainder)
{
    SecUnsignedInt32 tmpRemainderHi = 0;
    SecUnsignedInt32 tmpRemainderLo = 0;
    SecUnsignedInt32 tmpDividendHigh = dividendHigh;
    SecUnsignedInt32 tmpDividendLow = dividendLow;
    SecUnsignedInt32 tmpDivisor = divisor;
    int ret = 0;

    if ((pQuotientHigh == NULL) || (pQuotientLow == NULL) || tmpDivisor == 0 || puiRemainder == NULL) {
        return -1;
    }

    tmpDividendHigh &= SECUREC_MASK_BIT_ALL;
    tmpDividendLow &= SECUREC_MASK_BIT_ALL;
    tmpDivisor &= SECUREC_MASK_BIT_ALL;
    *pQuotientHigh = 0;
    *pQuotientLow = 0;
    *puiRemainder = 0;

    ret = SecU64Div64(tmpDividendHigh,
                      tmpDividendLow, 0, tmpDivisor, pQuotientHigh, pQuotientLow, &tmpRemainderHi, &tmpRemainderLo);
    if (ret != 0) {
        return ret;
    }

    if (tmpRemainderHi != 0) {
        return -1;
    }
    *puiRemainder = tmpRemainderLo;

    return 0;
}
#endif

