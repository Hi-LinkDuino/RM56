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

static errno_t CheckSrcCountRange(char *strDest, size_t destMax, const char *strSrc, size_t count)
{
    size_t tmpDestMax = destMax;
    size_t tmpCount = count;
    const char *endPos = strSrc;

    /* use destMax and  count as boundary checker and destMax must be greater than zero */
    while (*(endPos) != '\0' && tmpDestMax > 0 && tmpCount > 0) {
        ++endPos;
        --tmpCount;
        --tmpDestMax;
    }
    if (tmpDestMax == 0) {
        strDest[0] = '\0';
        SECUREC_ERROR_INVALID_RANGE("strncpy_s");
        return ERANGE_AND_RESET;
    }
    return EOK;
}

errno_t strncpy_error(char *strDest, size_t destMax, const char *strSrc, size_t count)
{
    if (destMax == 0 || destMax > SECUREC_STRING_MAX_LEN) {
        SECUREC_ERROR_INVALID_RANGE("strncpy_s");
        return ERANGE;
    } else if (strDest == NULL || strSrc == NULL) {
        SECUREC_ERROR_INVALID_PARAMTER("strncpy_s");
        if (strDest != NULL) {
            strDest[0] = '\0';
            return EINVAL_AND_RESET;
        }
        return EINVAL;
    } else if (count > SECUREC_STRING_MAX_LEN) {
        strDest[0] = '\0';      /* clear dest string */
        SECUREC_ERROR_INVALID_RANGE("strncpy_s");
        return ERANGE_AND_RESET;
    } else if (count == 0) {
        strDest[0] = '\0';
        return EOK;
    }

    return CheckSrcCountRange(strDest, destMax, strSrc, count);
}

/* The function compiler will be inlined and not placed in other files */
static size_t SecMinStrLen(const char *str, size_t maxLen)
{
#if SECUREC_HAVE_STRNLEN
    size_t len = strnlen(str, maxLen);  /* no ending terminator */
#else
    size_t len = 0;
    /* use maxLen  as boundary checker */
    const char *strEnd = str;
    while (len < maxLen && *(strEnd) != '\0') {
        ++strEnd;
        ++len;                  /* no ending terminator */
    }
#endif
    return len;
}

/*******************************************************************************
 * <FUNCTION DESCRIPTION>
 *   The strncpy_s function copies not more than n successive characters (not including the terminating null character)
  *                     from the array pointed to by strSrc to the array pointed to by strDest.
 *
 * <INPUT PARAMETERS>
 *    strDest                          Destination string.
 *    destMax                          The size of the destination string, in characters.
 *    strSrc                            Source string.
 *    count                              Number of characters to be copied.
 *
 * <OUTPUT PARAMETERS>
 *    strDest                           is updated
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
errno_t strncpy_s(char *strDest, size_t destMax, const char *strSrc, size_t count)
{
#ifdef  SECUREC_COMPATIBLE_WIN_FORMAT
    if ((destMax > 0 && destMax <= SECUREC_STRING_MAX_LEN && strDest != NULL && strSrc != NULL &&
         (count <= SECUREC_STRING_MAX_LEN || count == ((size_t)-1)) && count > 0)) {
#else
    if ((destMax > 0 && destMax <= SECUREC_STRING_MAX_LEN && strDest != NULL && strSrc != NULL &&
         count <= SECUREC_STRING_MAX_LEN && count > 0)) {
#endif
        size_t minCpLen;        /* use it to store the maxi length limit */

        if (count < destMax) {
            minCpLen = SecMinStrLen(strSrc, count); /* no ending terminator */
        } else {
            minCpLen = SecMinStrLen(strSrc, destMax);
            if (minCpLen == destMax) {
#ifdef  SECUREC_COMPATIBLE_WIN_FORMAT
                if (count == ((size_t)-1)) {
                    /* Compatible with windows security function features */
                    minCpLen = destMax - 1;
                } else {
                    strDest[0] = '\0';
                    SECUREC_ERROR_BUFFER_OVERLAP("strncpy_s");
                    return ERANGE_AND_RESET;
                }
#else
                strDest[0] = '\0';
                SECUREC_ERROR_BUFFER_OVERLAP("strncpy_s");
                return ERANGE_AND_RESET;
#endif
            }
        }

        if ((strDest < strSrc && strDest + minCpLen < strSrc) ||
            (strSrc < strDest && strSrc + minCpLen < strDest) || strDest == strSrc) {
            /* Not overlap */
            (void)memcpy(strDest, strSrc, minCpLen);    /* copy string without terminator */
            strDest[minCpLen] = '\0';
            return EOK;
        } else {
            strDest[0] = '\0';
            SECUREC_ERROR_BUFFER_OVERLAP("strncpy_s");
            return EOVERLAP_AND_RESET;
        }
    }
    return strncpy_error(strDest, destMax, strSrc, count);
}

#if SECUREC_IN_KERNEL
EXPORT_SYMBOL(strncpy_s);
#endif

