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

#include "securecutil.h"

#ifndef SECUREC_STRCOPY_THRESHOLD_SIZE
#define SECUREC_STRCOPY_THRESHOLD_SIZE (32UL)
#endif

/* The purpose of converting to void is to clean up the alarm */
#define SECUREC_STRCPY_BY_STRUCT(num) \
        case num: \
            *(SecStrBuf##num *)(void *)strDest = *(const SecStrBuf##num *)(const void *)strSrc; \
            break;

#define SECUREC_SMALL_STR_COPY do { \
        if (SECUREC_ADDR_ALIGNED_8(strDest) && SECUREC_ADDR_ALIGNED_8(strSrc)) { \
            /* use struct assignment */ \
            switch (srcStrLen) { \
                SECUREC_STRCPY_BY_STRUCT(1) \
                SECUREC_STRCPY_BY_STRUCT(2) \
                SECUREC_STRCPY_BY_STRUCT(3) \
                SECUREC_STRCPY_BY_STRUCT(4) \
                SECUREC_STRCPY_BY_STRUCT(5) \
                SECUREC_STRCPY_BY_STRUCT(6) \
                SECUREC_STRCPY_BY_STRUCT(7) \
                SECUREC_STRCPY_BY_STRUCT(8) \
                SECUREC_STRCPY_BY_STRUCT(9) \
                SECUREC_STRCPY_BY_STRUCT(10) \
                SECUREC_STRCPY_BY_STRUCT(11) \
                SECUREC_STRCPY_BY_STRUCT(12) \
                SECUREC_STRCPY_BY_STRUCT(13) \
                SECUREC_STRCPY_BY_STRUCT(14) \
                SECUREC_STRCPY_BY_STRUCT(15) \
                SECUREC_STRCPY_BY_STRUCT(16) \
                SECUREC_STRCPY_BY_STRUCT(17) \
                SECUREC_STRCPY_BY_STRUCT(18) \
                SECUREC_STRCPY_BY_STRUCT(19) \
                SECUREC_STRCPY_BY_STRUCT(20) \
                SECUREC_STRCPY_BY_STRUCT(21) \
                SECUREC_STRCPY_BY_STRUCT(22) \
                SECUREC_STRCPY_BY_STRUCT(23) \
                SECUREC_STRCPY_BY_STRUCT(24) \
                SECUREC_STRCPY_BY_STRUCT(25) \
                SECUREC_STRCPY_BY_STRUCT(26) \
                SECUREC_STRCPY_BY_STRUCT(27) \
                SECUREC_STRCPY_BY_STRUCT(28) \
                SECUREC_STRCPY_BY_STRUCT(29) \
                SECUREC_STRCPY_BY_STRUCT(30) \
                SECUREC_STRCPY_BY_STRUCT(31) \
                SECUREC_STRCPY_BY_STRUCT(32) \
                default:break; \
            } /* END switch */ \
        }  else { \
            char *tmpStrDest = (char *)strDest; \
            const char *tmpStrSrc =  (const char *)strSrc; \
            switch (srcStrLen) { \
                case 32: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 31: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 30: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 29: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 28: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 27: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 26: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 25: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 24: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 23: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 22: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 21: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 20: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 19: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 18: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 17: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 16: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 15: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 14: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 13: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 12: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 11: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 10: *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 9:  *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 8:  *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 7:  *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 6:  *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 5:  *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 4:  *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 3:  *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 2:  *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                case 1:  *tmpStrDest++ = *tmpStrSrc++; /* fall-through */ /* FALLTHRU */ \
                default:break; \
            } \
        } \
    } SECUREC_WHILE_ZERO

static errno_t CheckSrcRange(char *strDest, size_t destMax, const char *strSrc)
{
    size_t tmpDestMax = destMax;
    const char *tmpSrc = strSrc;
    /* use destMax as boundary checker and destMax must be greater than zero */
    while (*(tmpSrc) != '\0' && tmpDestMax > 0) {
        ++tmpSrc;
        --tmpDestMax;
    }
    if (tmpDestMax == 0) {
        strDest[0] = '\0';
        SECUREC_ERROR_INVALID_RANGE("strcpy_s");
        return ERANGE_AND_RESET;
    }
    return EOK;
}

errno_t strcpy_error(char *strDest, size_t destMax, const char *strSrc)
{
    if (destMax == 0 || destMax > SECUREC_STRING_MAX_LEN) {
        SECUREC_ERROR_INVALID_RANGE("strcpy_s");
        return ERANGE;
    } else if (strDest == NULL || strSrc == NULL) {
        SECUREC_ERROR_INVALID_PARAMTER("strcpy_s");
        if (strDest != NULL) {
            strDest[0] = '\0';
            return EINVAL_AND_RESET;
        }
        return EINVAL;
    }
    return CheckSrcRange(strDest, destMax, strSrc);
}

/*******************************************************************************
 * <FUNCTION DESCRIPTION>
 *    The strcpy_s function copies the string pointed to  strSrc
 *          (including the terminating null character) into the array pointed to by strDest
 *    The destination string must be large enough to hold the source string,
 *    including the terminating null character. strcpy_s will return EOVERLAP_AND_RESET
 *    if the source and destination strings overlap.
 *
 * <INPUT PARAMETERS>
 *    strDest                          Location of destination string buffer
 *    destMax                        Size of the destination string buffer.
 *    strSrc                            Null-terminated source string buffer.
 *
 * <OUTPUT PARAMETERS>
 *    strDest                         is updated.
 *
 * <RETURN VALUE>
 *    EOK                               Success
 *    EINVAL                          strDest is  NULL and destMax != 0 and destMax <= SECUREC_STRING_MAX_LEN
 *    EINVAL_AND_RESET       strDest !=  NULL and strSrc is NULL and destMax != 0 and destMax <= SECUREC_STRING_MAX_LEN
 *    ERANGE                         destMax is 0 and destMax > SECUREC_STRING_MAX_LEN
 *    ERANGE_AND_RESET      strDest have not enough space  and all other parameters are valid  and not overlap
 *    EOVERLAP_AND_RESET   dest buffer and source buffer are overlapped and all  parameters are valid
 *
 *    If there is a runtime-constraint violation, strDest[0] will be set to the '\0' when strDest and destMax valid
 *******************************************************************************
 */
errno_t strcpy_s(char *strDest, size_t destMax, const char *strSrc)
{
    if ((destMax > 0 && destMax <= SECUREC_STRING_MAX_LEN && strDest != NULL && strSrc != NULL && strDest != strSrc)) {
#if SECUREC_HAVE_STRNLEN
        size_t srcStrLen = strnlen(strSrc, destMax) + 1;    /* len  include \0 */
#else
        size_t srcStrLen = destMax; /* use it to store the max length limit */
        const char *endPos = strSrc;
        while (*(endPos) != '\0' && srcStrLen > 0) {    /* use srcStrLen as boundary checker */
            ++endPos;
            --srcStrLen;
        }
        srcStrLen = (size_t)(endPos - strSrc) + 1;  /* len  include \0 */
#endif
        /* le is  high performance to lt */
        if (srcStrLen <= destMax) {
            if ((strDest < strSrc && strDest + srcStrLen <= strSrc) ||
                (strSrc < strDest && strSrc + srcStrLen <= strDest)) {
#if SECUREC_IN_KERNEL
                (void)memcpy(strDest, strSrc, srcStrLen);
#else
                if (srcStrLen > SECUREC_STRCOPY_THRESHOLD_SIZE) {
                    (void)memcpy(strDest, strSrc, srcStrLen);
                } else {
                    SECUREC_SMALL_STR_COPY;
                }
#endif
                return EOK;
            } else {
                strDest[0] = '\0';
                SECUREC_ERROR_BUFFER_OVERLAP("strcpy_s");
                return EOVERLAP_AND_RESET;
            }

        }
    }
    return strcpy_error(strDest, destMax, strSrc);
}

#if SECUREC_IN_KERNEL
EXPORT_SYMBOL(strcpy_s);
#endif

