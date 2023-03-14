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

#include "securecutil.h"

#if SECUREC_HAVE_STRNLEN
#define SECUREC_STRCAT_LEN_THRESHOLD 8

#define SECUREC_CALC_STR_LEN(str,maxLen, len) do { \
            if (*((str) + 0) == '\0') { \
                len = 0; \
            } else if (*((str) + 1) == '\0') { \
                len = 1; \
            } else if (*((str) + 2) == '\0') { \
                len = 2; \
            } else if (*((str) + 3) == '\0') { \
                len = 3; \
            } else if (*((str) + 4) == '\0') { \
                len = 4; \
            } else if (*((str) + 5) == '\0') { \
                len = 5; \
            } else if (*((str) + 6) == '\0') { \
                len = 6; \
            } else if (*((str) + 7) == '\0') { \
                len = 7; \
            } else if (*((str) + 8) == '\0') { \
                /* Optimization with a length of 8 */ \
                len = 8; \
            } else { \
                /* The offset is 8 because the performance of 8 byte alignment is high */ \
                len = SECUREC_STRCAT_LEN_THRESHOLD + \
                      strnlen((str) + SECUREC_STRCAT_LEN_THRESHOLD, \
                      (maxLen) - SECUREC_STRCAT_LEN_THRESHOLD); \
            } \
        } SECUREC_WHILE_ZERO

/* The function compiler will be inlined and not placed in other files */
static size_t SecMinStrLenOpt(const char *str, size_t maxLen)
{
    size_t len;
    if (maxLen > SECUREC_STRCAT_LEN_THRESHOLD) {
        /* Just to reduce the code complexity */
        SECUREC_CALC_STR_LEN(str, maxLen, len);
    } else {
        const char *strEnd = str;
        len = 0;
        /* use count  as boundary checker */
        while (len < maxLen && *(strEnd) != '\0') {
            ++strEnd;
            ++len;              /* no ending terminator */
        }
    }
    return len;
}

static errno_t SecDoStrcat(char *strDest, size_t destMax, const char *strSrc)
{
    size_t destLen = strnlen(strDest, destMax);
    /* Only optimize strSrc, do not apply this function to strDest */
    size_t srcLen = SecMinStrLenOpt(strSrc, destMax - destLen);

    if ((strDest < strSrc && strDest + destLen + srcLen >= strSrc) || \
        (strSrc < strDest && strSrc + srcLen >= strDest)) {
        strDest[0] = '\0';
        if (strDest + destLen <= strSrc && destLen == destMax) {
            SECUREC_ERROR_INVALID_PARAMTER("strcat_s");
            return EINVAL_AND_RESET;
        }
        SECUREC_ERROR_BUFFER_OVERLAP("strcat_s");
        return EOVERLAP_AND_RESET;
    }
    if (srcLen + destLen >= destMax || strDest == strSrc) {
        strDest[0] = '\0';
        if (destLen == destMax) {
            SECUREC_ERROR_INVALID_PARAMTER("strcat_s");
            return EINVAL_AND_RESET;
        }
        SECUREC_ERROR_INVALID_RANGE("strcat_s");
        return ERANGE_AND_RESET;
    }
    (void)memcpy(strDest + destLen, strSrc, srcLen + 1);    /* copy terminator */
    return EOK;
}
#else
static errno_t SecDoStrcat(char *strDest, size_t destMax, const char *strSrc)
{
    char *tmpDest = strDest;
    const char *tmpSrc = strSrc;
    size_t availableSize = destMax;
    SECUREC_IN_REGISTER const char *overlapGuard = NULL;

    if (tmpDest < tmpSrc) {
        overlapGuard = tmpSrc;
        while (availableSize > 0 && *tmpDest != 0) {
            if (tmpDest == overlapGuard) {
                strDest[0] = '\0';
                SECUREC_ERROR_BUFFER_OVERLAP("strcat_s");
                return EOVERLAP_AND_RESET;
            }
            /* seek to string end */
            ++tmpDest;
            --availableSize;
        }

        /* strDest unterminated, return error. */
        if (availableSize == 0) {
            strDest[0] = '\0';
            SECUREC_ERROR_INVALID_PARAMTER("strcat_s");
            return EINVAL_AND_RESET;
        }
        /* if availableSize > 0, then execute the strcat operation */
        while ((*tmpDest++ = *tmpSrc++) != 0 && --availableSize > 0) {
            if (tmpDest == overlapGuard) {
                strDest[0] = '\0';
                SECUREC_ERROR_BUFFER_OVERLAP("strcat_s");
                return EOVERLAP_AND_RESET;
            }
        }
    } else {
        overlapGuard = tmpDest;
        while (availableSize > 0 && *tmpDest != '\0') {
            /* seek to string end, and no need to check overlap */
            ++tmpDest;
            --availableSize;
        }

        /* strDest unterminated, return error. */
        if (availableSize == 0) {
            strDest[0] = '\0';
            SECUREC_ERROR_INVALID_PARAMTER("strcat_s");
            return EINVAL_AND_RESET;
        }
        while ((*tmpDest++ = *tmpSrc++) != '\0' && --availableSize > 0) {
            if (tmpSrc == overlapGuard) {
                strDest[0] = '\0';
                SECUREC_ERROR_BUFFER_OVERLAP("strcat_s");
                return EOVERLAP_AND_RESET;
            }
        }
    }

    /* strDest have not enough space, return error */
    if (availableSize == 0) {
        strDest[0] = '\0';
        SECUREC_ERROR_INVALID_RANGE("strcat_s");
        return ERANGE_AND_RESET;
    }
    return EOK;
}
#endif
/*******************************************************************************
 * <FUNCTION DESCRIPTION>
 *    The strcat_s function appends a copy of the string pointed to by strSrc (including the terminating null character)
 *    to the end of the  string pointed to by strDest.
 *    The initial character of strSrc overwrites the terminating null character of strDest.
 *    strcat_s will return EOVERLAP_AND_RESET if the source and destination strings overlap.
 *
 *    Note that the second parameter is the total size of the buffer, not the
 *    remaining size.
 *
 * <INPUT PARAMETERS>
 *    strDest             Null-terminated destination string buffer.
 *    destMax             Size of the destination string buffer.
 *    strSrc              Null-terminated source string buffer.
 *
 * <OUTPUT PARAMETERS>
 *    strDest             is updated
 *
 * <RETURN VALUE>
 *    EOK                 Success
 *    EINVAL              strDest is  NULL and destMax != 0 and destMax <= SECUREC_STRING_MAX_LEN
 *    EINVAL_AND_RESET    (strDest unterminated  and all other parameters are valid)or
 *                         (strDest !=  NULL and strSrc is NULL and destMax != 0 and destMax <= SECUREC_STRING_MAX_LEN)
 *    ERANGE              destMax is 0 and destMax > SECUREC_STRING_MAX_LEN
 *    ERANGE_AND_RESET      strDest have not enough space  and all other parameters are valid  and not overlap
 *    EOVERLAP_AND_RESET   dest buffer and source buffer are overlapped and all  parameters are valid
 *
 *    If there is a runtime-constraint violation, strDest[0] will be set to the '\0' when strDest and destMax valid
 *******************************************************************************
 */

errno_t strcat_s(char *strDest, size_t destMax, const char *strSrc)
{

    if (destMax == 0 || destMax > SECUREC_STRING_MAX_LEN) {
        SECUREC_ERROR_INVALID_RANGE("strcat_s");
        return ERANGE;
    }

    if (strDest == NULL || strSrc == NULL) {
        SECUREC_ERROR_INVALID_PARAMTER("strcat_s");
        if (strDest != NULL) {
            strDest[0] = '\0';
            return EINVAL_AND_RESET;
        }
        return EINVAL;
    }

    return SecDoStrcat(strDest, destMax, strSrc);
}

#if SECUREC_IN_KERNEL
EXPORT_SYMBOL(strcat_s);
#endif

