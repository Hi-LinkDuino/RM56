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
 *     The vscanf_s function is equivalent to scanf_s, with the variable argument list replaced by arglist,
 *     The vscanf_s function reads data from the standard input stream stdin and
 *    writes the data into the location that's given by argument. Each argument
 *    must be a pointer to a variable of a type that corresponds to a type specifier
 *    in format. If copying occurs between strings that overlap, the behavior is
 *    undefined.
 *
 * <INPUT PARAMETERS>
 *    format                Format control string.
 *    arglist               pointer to list of arguments
 *
 * <OUTPUT PARAMETERS>
 *    arglist               the converted value stored in user assigned address
 *
 * <RETURN VALUE>
 *    Returns the number of fields successfully converted and assigned;
 *    the return value does not include fields that were read but not assigned.
 *    A return value of 0 indicates that no fields were assigned.
 *    return -1 if an error occurs.
 *******************************************************************************
 */
int vscanf_s(const char *format, va_list arglist)
{
    int retVal;                 /* If initialization causes  e838 */
    SecFileStream fStr = SECUREC_INIT_SEC_FILE_STREAM;
    /*
     * "va_list" has different definition on different platform, so we can't use arglist == NULL
     * to determine it's invalid. If you has fixed platform, you can check some fields to validate it,
     * such as "arglist == NULL" or arglist.xxx != NULL or *(size_t *)&arglist != 0.
     */
    fStr.pf = stdin;
    if (format == NULL || fStr.pf == NULL) {
        SECUREC_ERROR_INVALID_PARAMTER("vscanf_s");
        return SECUREC_SCANF_EINVAL;
    }

    fStr.flag = SECUREC_FROM_STDIN_FLAG;

    SECUREC_LOCK_STDIN(0, fStr.pf);

    retVal = SecInputS(&fStr, format, arglist);

    SECUREC_UNLOCK_STDIN(0, fStr.pf);
    if (retVal < 0) {
        SECUREC_ERROR_INVALID_PARAMTER("vscanf_s");
        return SECUREC_SCANF_EINVAL;
    }
    return retVal;
}


