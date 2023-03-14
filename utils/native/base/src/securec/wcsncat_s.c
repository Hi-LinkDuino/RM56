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

static errno_t SecDoWcsncat(wchar_t *strDest, size_t destMax, const wchar_t *strSrc, size_t count)
{
    wchar_t *tmpDest = strDest;
    const wchar_t *tmpSrc = strSrc;
    size_t availableSize = destMax;
    size_t maxCount = count;
    SECUREC_IN_REGISTER const wchar_t *overlapGuard = NULL;

    if (tmpDest < tmpSrc) {
        overlapGuard = tmpSrc;
        while (availableSize > 0 && *tmpDest != '\0') {
            if (tmpDest == overlapGuard) {
                strDest[0] = '\0';
                SECUREC_ERROR_BUFFER_OVERLAP("wcsncat_s");
                return EOVERLAP_AND_RESET;
            }
            ++tmpDest;
            --availableSize;
        }
        /* strDestination unterminated, return error. */
        if (availableSize == 0) {
            strDest[0] = '\0';
            SECUREC_ERROR_INVALID_PARAMTER("wcsncat_s");
            return EINVAL_AND_RESET;
        }
        while (maxCount > 0 && (*tmpDest++ = *tmpSrc++) != '\0' && --availableSize > 0) {
            if (tmpDest == overlapGuard) {
                strDest[0] = '\0';
                SECUREC_ERROR_BUFFER_OVERLAP("wcsncat_s");
                return EOVERLAP_AND_RESET;
            }
            --maxCount;
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
            SECUREC_ERROR_INVALID_PARAMTER("wcsncat_s");
            return EINVAL_AND_RESET;
        }
        while (maxCount > 0 && (*tmpDest++ = *tmpSrc++) != '\0' && --availableSize > 0) {
            if (tmpSrc == overlapGuard) {
                strDest[0] = '\0';
                SECUREC_ERROR_BUFFER_OVERLAP("wcsncat_s");
                return EOVERLAP_AND_RESET;
            }
            --maxCount;
        }
    }
    if (maxCount == 0) {
        *tmpDest = '\0';        /* add terminator to strDest */
    }

    /* strDest have no space to store the terminator, return error */
    if (availableSize == 0) {
        strDest[0] = '\0';
        SECUREC_ERROR_INVALID_RANGE("wcsncat_s");
        return ERANGE_AND_RESET;
    }
    return EOK;
}

/*******************************************************************************
 * <FUNCTION DESCRIPTION>
 *    The wcsncat_s function appends not more than n successive wide characters
 *     (not including the terminating null wide character)
 *     from the array pointed to by strSrc to the end of the wide string pointed to by strDest.
 *
 *    The wcsncat_s function try to append the first D characters of strSrc to
 *    the end of strDest, where D is the lesser of count and the length of strSrc.
 *    If appending those D characters will fit within strDest (whose size is
 *    given as destMax) and still leave room for a null terminator, then those
 *    characters are appended, starting at the original terminating null of
 *    strDest, and a new terminating null is appended; otherwise, strDest[0] is
 *    set to the null character.
 *
 * <INPUT PARAMETERS>
 *    strDest               Null-terminated destination string.
 *    destMax               Size of the destination buffer.
 *    strSrc                Null-terminated source string.
 *    count                 Number of character to append, or truncate.
 *
 * <OUTPUT PARAMETERS>
 *    strDest               is updated
 *
 * <RETURN VALUE>
 *    EOK                   Success
 *    EINVAL                strDest is  NULL and destMax != 0 and destMax <= SECUREC_WCHAR_STRING_MAX_LEN
 *    EINVAL_AND_RESET      (strDest unterminated and all other parameters are valid) or
 *                    (strDest != NULL and strSrc is NULL and destMax != 0 and destMax <= SECUREC_WCHAR_STRING_MAX_LEN)
 *    ERANGE                destMax > SECUREC_WCHAR_STRING_MAX_LEN or destMax is 0
 *    ERANGE_AND_RESET      strDest have not enough space  and all other parameters are valid  and not overlap
 *    EOVERLAP_AND_RESET     dest buffer and source buffer are overlapped and all  parameters are valid
 *
 *    If there is a runtime-constraint violation, strDest[0] will be set to the '\0' when strDest and destMax valid
 *******************************************************************************
 */
errno_t wcsncat_s(wchar_t *strDest, size_t destMax, const wchar_t *strSrc, size_t count)
{
    if (destMax == 0 || destMax > SECUREC_WCHAR_STRING_MAX_LEN) {
        SECUREC_ERROR_INVALID_RANGE("wcsncat_s");
        return ERANGE;
    }

    if (strDest == NULL || strSrc == NULL) {
        SECUREC_ERROR_INVALID_PARAMTER("wcsncat_s");
        if (strDest != NULL) {
            strDest[0] = '\0';
            return EINVAL_AND_RESET;
        }
        return EINVAL;
    }
#ifdef  SECUREC_COMPATIBLE_WIN_FORMAT
    if (count > SECUREC_WCHAR_STRING_MAX_LEN && count != ((size_t)-1)) {
        strDest[0] = '\0';
        SECUREC_ERROR_INVALID_RANGE("wcsncat_s");
        return ERANGE_AND_RESET;
    }
#else
    if (count > SECUREC_WCHAR_STRING_MAX_LEN) {
        strDest[0] = '\0';
        SECUREC_ERROR_INVALID_RANGE("wcsncat_s");
        return ERANGE_AND_RESET;
    }
#endif

    return SecDoWcsncat(strDest, destMax, strSrc, count);
}


