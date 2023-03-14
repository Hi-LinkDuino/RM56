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

static inline int SecVsnprintfPImpl(char *string, size_t count, int priv, const char *format, va_list arglist);

/*******************************************************************************
 * <FUNCTION DESCRIPTION>
 *    The vsnprintf_s function is equivalent to the vsnprintf function
 *     except for the parameter destMax/count and the explicit runtime-constraints violation
 *    The vsnprintf_s function takes a pointer to an argument list, then formats
 *    and writes up to count characters of the given data to the memory pointed
 *    to by strDest and appends a terminating null.
 *
 * <INPUT PARAMETERS>
 *    strDest                  Storage location for the output.
 *    destMax                The size of the strDest for output.
 *    count                    Maximum number of character to write(not including
 *                                the terminating NULL)
 *    priv_on               whether print <private> for not-public args
 *    format                   Format-control string.
 *    arglist                     pointer to list of arguments.
 *
 * <OUTPUT PARAMETERS>
 *    strDest                is updated
 *
 * <RETURN VALUE>
 *    return  the number of characters written, not including the terminating null
 *    return -1 if an  error occurs.
 *    return -1 if count < destMax and the output string  has been truncated
 *
 * If there is a runtime-constraint violation, strDest[0] will be set to the '\0' when strDest and destMax valid
 *******************************************************************************
 */
int vsnprintfp_s(char *strDest, size_t destMax, size_t count, int priv,  const char *format, va_list arglist)
{
    int retVal;

    if (format == NULL || strDest == NULL || destMax == 0 || destMax > SECUREC_STRING_MAX_LEN ||
        (count > (SECUREC_STRING_MAX_LEN - 1) && count != (size_t)-1)) {
        if (strDest != NULL && destMax > 0) {
            strDest[0] = '\0';
        }
        SECUREC_ERROR_INVALID_PARAMTER("vsnprintfp_s");
        return -1;
    }

    if (destMax > count) {
        retVal = SecVsnprintfPImpl(strDest, count + 1, priv, format, arglist);
        if (retVal == SECUREC_PRINTF_TRUNCATE) {  /* lsd add to keep dest buffer not destroyed 2014.2.18 */
            /* the string has been truncated, return  -1 */
            return -1;          /* to skip error handler,  return strlen(strDest) or -1 */
        }
    } else {                    /* destMax <= count */
        retVal = SecVsnprintfPImpl(strDest, destMax, priv, format, arglist);
#ifdef SECUREC_COMPATIBLE_WIN_FORMAT
        if (retVal == SECUREC_PRINTF_TRUNCATE && count == (size_t)-1) {
            return -1;
        }
#endif
    }

    if (retVal < 0) {
        strDest[0] = '\0';      /* empty the dest strDest */

        if (retVal == SECUREC_PRINTF_TRUNCATE) {
            /* Buffer too small */
            SECUREC_ERROR_INVALID_RANGE("vsnprintfp_s");
        }

        SECUREC_ERROR_INVALID_PARAMTER("vsnprintfp_s");
        return -1;
    }

    return retVal;
}

#ifdef SECUREC_FOR_WCHAR
#undef SECUREC_FOR_WCHAR
#endif

typedef char SecChar;
#define SECUREC_CHAR(x) x

#define SECUREC_WRITE_MULTI_CHAR  SecWriteMultiChar
#define SECUREC_WRITE_STRING      SecWriteString
#include "output_p.inl"

/* put a char to output */
#define SECUREC_PUTC(_c, _stream)    ((--(_stream)->count >= 0) ? ((*(_stream)->cur++ = (char)(_c)) & 0xff) : EOF)
/* to clear e835 */
#define SECUREC_PUTC_ZERO(_stream)    ((--(_stream)->count >= 0) ? ((*(_stream)->cur++ = (char)('\0'))) : EOF)

static inline int SecVsnprintfPImpl(char *string, size_t count, int priv, const char *format, va_list arglist)
{
    SecPrintfStream str;
    int retVal;

    str.count = (int)count;     /* this count include \0 character */
    str.cur = string;

    retVal = SecOutputPS(&str, priv, format, arglist);
    if ((retVal >= 0) && (SECUREC_PUTC_ZERO(&str) != EOF)) {
        return (retVal);
    } else if (str.count < 0) {
        /* the buffer was too small; we return truncation */
        string[count - 1] = 0;
        return SECUREC_PRINTF_TRUNCATE;
    }

    return -1;
}
