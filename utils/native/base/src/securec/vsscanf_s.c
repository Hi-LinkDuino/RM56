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
#if defined(SECUREC_VXWORKS_PLATFORM) && !defined(SECUREC_SYSAPI4VXWORKS)
#include <ctype.h>
#endif

/*******************************************************************************
 * <NAME>
 *    vsscanf_s
 *
 *
 * <FUNCTION DESCRIPTION>
 *    The vsscanf_s function is equivalent to sscanf_s, with the variable argument list replaced by arglist
 *    The vsscanf_s function reads data from buffer into the location given by
 *    each argument. Every argument must be a pointer to a variable with a type
 *    that corresponds to a type specifier in format. The format argument controls
 *    the interpretation of the input fields and has the same form and function
 *    as the format argument for the scanf function.
 *    If copying takes place between strings that overlap, the behavior is undefined.
 *
 * <INPUT PARAMETERS>
 *    buffer                Stored data
 *    format                Format control string, see Format Specifications.
 *    arglist               pointer to list of arguments
 *
 * <OUTPUT PARAMETERS>
 *    arglist               the converted value stored in user assigned address
 *
 * <RETURN VALUE>
 *    Each of these functions returns the number of fields successfully converted
 *    and assigned; the return value does not include fields that were read but
 *    not assigned. A return value of 0 indicates that no fields were assigned.
 *    return -1 if an error occurs.
 *******************************************************************************
 */
#ifndef __MINGW32__
int vsscanf_s(const char *buffer, const char *format, va_list arglist)
{
    SecFileStream fStr = SECUREC_INIT_SEC_FILE_STREAM;
    int retVal;                 /* If initialization causes  e838 */
    size_t count;               /* If initialization causes  e838 */

    /* validation section */
    if (buffer == NULL || format == NULL) {
        SECUREC_ERROR_INVALID_PARAMTER("vsscanf_s");
        return SECUREC_SCANF_EINVAL;
    }
    count = strlen(buffer);
    if (count == 0 || count > SECUREC_STRING_MAX_LEN) {
        SecClearDestBuf(buffer, format, arglist);
        SECUREC_ERROR_INVALID_PARAMTER("vsscanf_s");
        return SECUREC_SCANF_EINVAL;
    }
#ifdef SECUREC_VXWORKS_PLATFORM
    if (isspace((int)buffer[0]) && isspace((int)buffer[count - 1])) {
        SecClearDestBuf(buffer, format, arglist);
    }
#endif
    fStr.flag = SECUREC_MEM_STR_FLAG;
    fStr.cur = (const char *)buffer;
    fStr.count = (int)count;
    retVal = SecInputS(&fStr, format, arglist);
    if (retVal < 0) {
        SECUREC_ERROR_INVALID_PARAMTER("vsscanf_s");
        return SECUREC_SCANF_EINVAL;
    }

    return retVal;
}
#endif

