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

#include "secinput.h"

/*******************************************************************************
 * <FUNCTION DESCRIPTION>
 *    The  vfwscanf_s  function  is  the  wide-character  equivalent  of the vfscanf_s function
 *    The vfwscanf_s function reads data from the current position of stream into
 *    the locations given by argument (if any). Each argument must be a pointer
 *    to a variable of a type that corresponds to a type specifier in format.
 *    format controls the interpretation of the input fields and has the same form
 *    and function as the format argument for scanf; see scanf for a description
 *    of format.
 *
 * <INPUT PARAMETERS>
 *    stream               Pointer to FILE structure.
 *    format               Format control string, see Format Specifications.
 *    arglist              pointer to list of arguments
 *
 * <OUTPUT PARAMETERS>
 *    arglist              the converted value stored in user assigned address
 *
 * <RETURN VALUE>
 *    Each of these functions returns the number of fields successfully converted
 *    and assigned; the return value does not include fields that were read but
 *    not assigned. A return value of 0 indicates that no fields were assigned.
 *    return -1 if an error occurs.
 *******************************************************************************
 */
int vfwscanf_s(FILE *stream, const wchar_t *format, va_list arglist)
{
    int retVal;                 /* If initialization causes  e838 */
    SecFileStream fStr = SECUREC_INIT_SEC_FILE_STREAM;

    if ((stream == NULL) || (format == NULL)) {
        SECUREC_ERROR_INVALID_PARAMTER("vfwscanf_s");
        return SECUREC_SCANF_EINVAL;
    }
    if (stream == stdin) {
        return vwscanf_s(format, arglist);
    }

    SECUREC_LOCK_FILE(stream);

    fStr.pf = stream;
    fStr.flag = SECUREC_FILE_STREAM_FLAG;
    fStr.oriFilePos = SECUREC_UNINITIALIZED_FILE_POS;

    retVal = SecInputSW(&fStr, format, arglist);
    SECUREC_UNLOCK_FILE(stream);
    if (retVal < 0) {
        SECUREC_ERROR_INVALID_PARAMTER("vfwscanf_s");
        return SECUREC_SCANF_EINVAL;
    }
    return retVal;
}


