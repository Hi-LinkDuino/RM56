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

static errno_t SecDoWcscat(wchar_t *strDest, size_t destMax, const wchar_t *strSrc)
{
    wchar_t *tmpDest = strDest;
    const wchar_t *tmpSrc = strSrc;
    size_t availableSize = destMax;
    SECUREC_IN_REGISTER const wchar_t *overlapGuard = NULL;

    if (tmpDest < tmpSrc) {
        overlapGuard = tmpSrc;
        while (availableSize > 0 && *tmpDest != '\0') {
            if (tmpDest == overlapGuard) {
                strDest[0] = '\0';
                SECUREC_ERROR_BUFFER_OVERLAP("wcscat_s");
                return EOVERLAP_AND_RESET;
            }
            /* seek to string end */
            ++tmpDest;
            --availableSize;
        }

        /* strDest unterminated, return error. */
        if (availableSize == 0) {
            strDest[0] = '\0';
            SECUREC_ERROR_INVALID_PARAMTER("wcscat_s");
            return EINVAL_AND_RESET;
        }

        /* if 2014-2018 > 0, then execute the strcat operation */
        while ((*tmpDest++ = *tmpSrc++) != '\0' && --availableSize > 0) {
            if (tmpDest == overlapGuard) {
                strDest[0] = '\0';
                SECUREC_ERROR_BUFFER_OVERLAP("wcscat_s");
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
            SECUREC_ERROR_INVALID_PARAMTER("wcscat_s");
            return EINVAL_AND_RESET;
        }
        while ((*tmpDest++ = *tmpSrc++) != '\0' && --availableSize > 0) {
            if (tmpSrc == overlapGuard) {
                strDest[0] = '\0';
                SECUREC_ERROR_BUFFER_OVERLAP("wcscat_s");
                return EOVERLAP_AND_RESET;
            }
        }
    }

    /* strDest have not enough space, return error */
    if (availableSize == 0) {
        strDest[0] = '\0';
        SECUREC_ERROR_INVALID_RANGE("wcscat_s");
        return ERANGE_AND_RESET;
    }
    return EOK;
}

/*******************************************************************************
 * <FUNCTION DESCRIPTION>
 *    The wcscat_s function appends a copy of the wide string pointed to by strSrc
*      (including the terminating null wide character)
 *     to the end of the wide string pointed to by strDest.
 *    The arguments and return value of wcscat_s are wide-character strings.
 *
 *    The wcscat_s function appends strSrc to strDest and terminates the resulting
 *    string with a null character. The initial character of strSrc overwrites the
 *    terminating null character of strDest. wcscat_s will return EOVERLAP_AND_RESET if the
 *    source and destination strings overlap.
 *
 *    Note that the second parameter is the total size of the buffer, not the
 *    remaining size.
 *
 * <INPUT PARAMETERS>
 *    strDest              Null-terminated destination string buffer.
 *    destMax              Size of the destination string buffer.
 *    strSrc               Null-terminated source string buffer.
 *
 * <OUTPUT PARAMETERS>
 *    strDest               is updated
 *
 * <RETURN VALUE>
 *    EOK                   Success
 *    EINVAL                strDest is  NULL and destMax != 0 and destMax <= SECUREC_WCHAR_STRING_MAX_LEN
 *    EINVAL_AND_RESET      (strDest unterminated and all other parameters are valid) or
 *                          (strDest != NULL and strSrc is NULL and destMax != 0
 *                           and destMax <= SECUREC_WCHAR_STRING_MAX_LEN)
 *    ERANGE                destMax > SECUREC_WCHAR_STRING_MAX_LEN or destMax is 0
 *    ERANGE_AND_RESET      strDest have not enough space  and all other parameters are valid  and not overlap
 *    EOVERLAP_AND_RESET     dest buffer and source buffer are overlapped and all  parameters are valid
 *
 *    If there is a runtime-constraint violation, strDest[0] will be set to the '\0' when strDest and destMax valid
 *******************************************************************************
 */
errno_t wcscat_s(wchar_t *strDest, size_t destMax, const wchar_t *strSrc)
{
    if (destMax == 0 || destMax > SECUREC_WCHAR_STRING_MAX_LEN) {
        SECUREC_ERROR_INVALID_RANGE("wcscat_s");
        return ERANGE;
    }

    if (strDest == NULL || strSrc == NULL) {
        SECUREC_ERROR_INVALID_PARAMTER("wcscat_s");
        if (strDest != NULL) {
            strDest[0] = '\0';
            return EINVAL_AND_RESET;
        }
        return EINVAL;
    }

    return SecDoWcscat(strDest, destMax, strSrc);
}


