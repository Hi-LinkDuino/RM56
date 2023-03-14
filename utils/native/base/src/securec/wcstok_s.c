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

static wchar_t *SecFindBeginW(wchar_t *strToken, const wchar_t *strDelimit)
{
    /* Find beginning of token (skip over leading delimiters). Note that
     * there is no token if this loop sets string to point to the terminal null.
     */
    wchar_t *token = strToken;
    while (*token != 0) {
        const wchar_t *ctl = strDelimit;
        while (*ctl != 0 && *ctl != *token) {
            ++ctl;
        }
        if (*ctl == 0) {
            break;
        }
        ++token;
    }
    return token;
}

static wchar_t *SecFindRestW(wchar_t *strToken, const wchar_t *strDelimit)
{

    /* Find the end of the token. If it is not the end of the string,
     * put a null there.
     */
    wchar_t *token = strToken;
    while (*token != 0) {
        const wchar_t *ctl = strDelimit;
        while (*ctl != 0 && *ctl != *token) {
            ++ctl;
        }
        if (*ctl != 0) {
            *token++ = 0;
            break;
        }
        ++token;
    }
    return token;
}

/*******************************************************************************
 * <NAME>
 *    wcstok_s
 *
 *
 * <FUNCTION DESCRIPTION>
 *   The  wcstok_s  function  is  the  wide-character  equivalent  of the strtok_s function
 *
 * <INPUT PARAMETERS>
 *    strToken               String containing token or tokens.
 *    strDelimit             Set of delimiter characters.
 *    context                Used to store position information between calls to
 *                               wcstok_s.
 *
 * <OUTPUT PARAMETERS>
 *    context               is updated
 * <RETURN VALUE>
 *    Returns a pointer to the next token found in strToken.
 *    They return NULL when no more tokens are found.
 *    Each call modifies strToken by substituting a NULL character for the first
 *    delimiter that occurs after the returned token.
 *
 *    return value          condition
 *    NULL                  context is NULL, strDelimit is NULL, strToken is NULL
 *                          and  (*context) is NULL, or no token is found.
 *******************************************************************************
 */
wchar_t *wcstok_s(wchar_t *strToken, const wchar_t *strDelimit, wchar_t **context)
{
    wchar_t *orgToken = strToken;
    /* validation section */
    if (context == NULL || strDelimit == NULL) {
        return NULL;
    }
    if (orgToken == NULL && (*context) == NULL) {
        return NULL;
    }

    /* If string==NULL, continue with previous string */
    if (orgToken == NULL) {
        orgToken = *context;
    }

    orgToken = SecFindBeginW(orgToken, strDelimit);
    {
        wchar_t *token = orgToken;

        orgToken = SecFindRestW(orgToken, strDelimit);

        /* Update the context */
        *context = orgToken;

        /* Determine if a token has been found. */
        if (token == orgToken) {
            token = NULL;
        }

        return token;
    }
}


