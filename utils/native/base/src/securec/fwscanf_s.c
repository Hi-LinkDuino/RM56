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

#include "securec.h"

/*******************************************************************************
 * <FUNCTION DESCRIPTION>
 *    The fwscanf_s function  is  the  wide-character  equivalent  of the fscanf_s function
 *    The fwscanf_s function reads data from the current position of stream into
 *    the locations given by argument (if any). Each argument must be a pointer
 *    to a variable of a type that corresponds to a type specifier in format.
 *    format controls the interpretation of the input fields and has the same
 *    form and function as the format argument for scanf; see scanf for a
 *    description of format.
 *
 * <INPUT PARAMETERS>
 *    stream                   Pointer to FILE structure.
 *    format                   Format control string, see Format Specifications.
 *    ...                      Optional arguments.
 *
 * <OUTPUT PARAMETERS>
 *    ...                      The converted value stored in user assigned address
 *
 * <RETURN VALUE>
 *    Each of these functions returns the number of fields successfully converted
 *    and assigned; the return value does not include fields that were read but
 *    not assigned. A return value of 0 indicates that no fields were assigned.
 *    A value of -1 indicates that an error has occurred.
 *******************************************************************************
 */
int fwscanf_s(FILE *stream, const wchar_t *format, ...)
{
    int ret;                    /* If initialization causes  e838 */
    va_list arglist;

    va_start(arglist, format);
    ret = vfwscanf_s(stream, format, arglist);
    va_end(arglist);
    (void)arglist;              /* to clear e438 last value assigned not used , the compiler will optimize this code */

    return ret;
}

