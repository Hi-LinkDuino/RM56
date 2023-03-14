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

static errno_t SecDoWcsncpy(wchar_t *strDest, size_t destMax, const wchar_t *strSrc, size_t count)
{
    wchar_t *tmpDest = strDest;
    const wchar_t *tmpSrc = strSrc;
    size_t maxSize = destMax;
    size_t maxCount = count;
    SECUREC_IN_REGISTER const wchar_t *overlapGuard = NULL;

    if (tmpDest < tmpSrc) {
        overlapGuard = tmpSrc;
        while ((*(tmpDest++) = *(tmpSrc++)) != '\0' && --maxSize > 0 && --maxCount > 0) {
            if (tmpDest == overlapGuard) {
                strDest[0] = '\0';
                SECUREC_ERROR_BUFFER_OVERLAP("wcsncpy_s");
                return EOVERLAP_AND_RESET;
            }
        }
    } else {
        overlapGuard = tmpDest;
        while ((*(tmpDest++) = *(tmpSrc++)) != '\0' && --maxSize > 0 && --maxCount > 0) {
            if (tmpSrc == overlapGuard) {
                strDest[0] = '\0';
                SECUREC_ERROR_BUFFER_OVERLAP("wcsncpy_s");
                return EOVERLAP_AND_RESET;
            }
        }
    }
    if (maxCount == 0) {
        *tmpDest = '\0';
    }

    if (maxSize == 0) {
        strDest[0] = '\0';
        SECUREC_ERROR_INVALID_RANGE("wcsncpy_s");
        return ERANGE_AND_RESET;
    }
    return EOK;
}

/*******************************************************************************
 * <FUNCTION DESCRIPTION>
 *    The wcsncpy_s function copies not more than n successive wide characters
 *     (not including the terminating null wide character)
 *     from the array pointed to by strSrc to the array pointed to by strDest
 *
 * <INPUT PARAMETERS>
 *    strDest             Destination string.
 *    destMax             The size of the destination string, in characters.
 *    strSrc              Source string.
 *    count                Number of characters to be copied.
 *
 * <OUTPUT PARAMETERS>
 *    strDest              is updated
 *
 * <RETURN VALUE>
 *    EOK                  Success
 *    EINVAL               strDest is  NULL and destMax != 0 and destMax <= SECUREC_WCHAR_STRING_MAX_LEN
 *    EINVAL_AND_RESET     strDest != NULL and strSrc is NULL and destMax != 0
 *                         and destMax <= SECUREC_WCHAR_STRING_MAX_LEN
 *    ERANGE               destMax > SECUREC_WCHAR_STRING_MAX_LEN or destMax is 0
 *    ERANGE_AND_RESET     count > SECUREC_WCHAR_STRING_MAX_LEN or
 *                         (destMax <= length of strSrc and destMax <= count and strDest != strSrc
 *                          and strDest != NULL and strSrc != NULL and destMax != 0 and
 *                          destMax <= SECUREC_WCHAR_STRING_MAX_LEN and not overlap)
 *    EOVERLAP_AND_RESET     dest buffer and source buffer are overlapped and  all  parameters are valid
 *
 *
 *    If there is a runtime-constraint violation, strDest[0] will be set to the '\0' when strDest and destMax valid
 *******************************************************************************
 */
errno_t wcsncpy_s(wchar_t *strDest, size_t destMax, const wchar_t *strSrc, size_t count)
{
    if (destMax == 0 || destMax > SECUREC_WCHAR_STRING_MAX_LEN) {
        SECUREC_ERROR_INVALID_RANGE("wcsncpy_s");
        return ERANGE;
    }

    if (strDest == NULL || strSrc == NULL) {
        SECUREC_ERROR_INVALID_PARAMTER("wcsncpy_s");
        if (strDest != NULL) {
            strDest[0] = '\0';
            return EINVAL_AND_RESET;
        }
        return EINVAL;
    }
#ifdef  SECUREC_COMPATIBLE_WIN_FORMAT
    if (count > SECUREC_WCHAR_STRING_MAX_LEN && count != (size_t)-1) {
        strDest[0] = '\0';      /* clear dest string */
        SECUREC_ERROR_INVALID_RANGE("wcsncpy_s");
        return ERANGE_AND_RESET;
    }
#else
    if (count > SECUREC_WCHAR_STRING_MAX_LEN) {
        strDest[0] = '\0';      /* clear dest string */
        SECUREC_ERROR_INVALID_RANGE("wcsncpy_s");
        return ERANGE_AND_RESET;
    }
#endif
    if (count == 0) {
        strDest[0] = '\0';
        return EOK;
    }

    return SecDoWcsncpy(strDest, destMax, strSrc, count);
}


