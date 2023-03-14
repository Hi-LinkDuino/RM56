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

#include "secureprintoutput.h"
#ifndef __MINGW32__
static int SecVswprintfImpl(wchar_t *string, size_t sizeInWchar, const wchar_t *format, va_list arglist)
{
    SecPrintfStream str;
    int retVal;                 /* If initialization causes  e838 */

    str.cur = (char *)string;
    str.count = (int)(sizeInWchar * sizeof(wchar_t));

    retVal = SecOutputSW(&str, format, arglist);
    if ((retVal >= 0) && SecPutWcharStrEndingZero(&str, (int)sizeof(wchar_t))) {
        return (retVal);
    } else if (str.count < 0) {
        /* the buffer was too small; we return truncation */
        string[sizeInWchar - 1] = 0;
        return SECUREC_PRINTF_TRUNCATE;
    }
    return -1;

}

/*******************************************************************************
 * <FUNCTION DESCRIPTION>
 *    The  vswprintf_s  function  is  the  wide-character  equivalent  of the vsprintf_s function
 *
 * <INPUT PARAMETERS>
 *    strDest                  Storage location for the output.
 *    destMax                Size of strDest
 *    format                  Format specification.
 *    arglist                   pointer to list of arguments
 *
 * <OUTPUT PARAMETERS>
 *    strDest                 is updated
 *
 * <RETURN VALUE>
 *    return  the number of wide characters stored in strDest, not  counting the terminating null wide character.
 *    return -1  if an error occurred.
 *
 * If there is a runtime-constraint violation, strDest[0] will be set to the '\0' when strDest and destMax valid
 *******************************************************************************
 */

int vswprintf_s(wchar_t *strDest, size_t destMax, const wchar_t *format, va_list arglist)
{
    int retVal;               /* If initialization causes  e838 */

    if (format == NULL || strDest == NULL || destMax == 0 || destMax > (SECUREC_WCHAR_STRING_MAX_LEN)) {
        if (strDest != NULL && destMax > 0) {
            strDest[0] = '\0';
        }
        SECUREC_ERROR_INVALID_PARAMTER("vswprintf_s");
        return -1;
    }

    retVal = SecVswprintfImpl(strDest, destMax, format, arglist);

    if (retVal < 0) {
        strDest[0] = '\0';
        if (retVal == SECUREC_PRINTF_TRUNCATE) {
            /* Buffer too small */
            SECUREC_ERROR_INVALID_RANGE("vswprintf_s");
        }
        SECUREC_ERROR_INVALID_PARAMTER("vswprintf_s");
        return -1;
    }

    return retVal;
}
#endif

