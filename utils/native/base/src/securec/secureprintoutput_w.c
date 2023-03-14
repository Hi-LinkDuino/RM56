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

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#ifdef _CRTIMP_ALTERNATIVE
#undef _CRTIMP_ALTERNATIVE
#endif
#ifdef __STDC_WANT_SECURE_LIB__
#undef __STDC_WANT_SECURE_LIB__
#endif
#define _CRTIMP_ALTERNATIVE     //comment microsoft *_s function
#define __STDC_WANT_SECURE_LIB__ 0
#endif

/* if some platforms don't have wchar.h, don't include it */
#if !(defined(SECUREC_VXWORKS_PLATFORM))
/* This header file is placed below secinput.h, which will cause tool alarm,
  * but  If there is no macro above, it will cause compiling alarm */
#include <wchar.h>
#endif
#include "secureprintoutput.h"

#ifndef WEOF
#define WEOF ((wchar_t)-1)
#endif

#ifndef SECUREC_FOR_WCHAR
#define SECUREC_FOR_WCHAR
#endif

typedef wchar_t SecChar;
#define SECUREC_CHAR(x) L ## x
#define SECUREC_WRITE_CHAR       SecWriteCharW
#define SECUREC_WRITE_MULTI_CHAR SecWriteMultiCharW
#define SECUREC_WRITE_STRING     SecWriteStringW


/* put a wchar to output stream */
/* LSD change "unsigned short" to wchar_t */

static wchar_t SecPutCharW(wchar_t ch, SecPrintfStream *f)
{
    wchar_t wcRet = 0;
    if (((f)->count -= (int)sizeof(wchar_t)) >= 0) {
        *(wchar_t *)(void *)(f->cur) = ch;
        f->cur += sizeof(wchar_t);
        wcRet = ch;
    } else {
        wcRet = (wchar_t)WEOF;
    }
    return wcRet;
}

static void SecWriteCharW(wchar_t ch, SecPrintfStream *f, int *pnumwritten)
{
    if (SecPutCharW(ch, f) == (wchar_t)WEOF) {
        *pnumwritten = -1;
    } else {
        ++(*pnumwritten);
    }
}

static void SecWriteMultiCharW(wchar_t ch, int num, SecPrintfStream *f, int *pnumwritten)
{
    int count = num;
    while (count-- > 0) {
        SecWriteCharW(ch, f, pnumwritten);
        if (*pnumwritten == -1) {
            break;
        }
    }
}

static void SecWriteStringW(const wchar_t *string, int len, SecPrintfStream *f, int *pnumwritten)
{
    const wchar_t *str = string;
    int count = len;
    while (count-- > 0) {
        SecWriteCharW(*str++, f, pnumwritten);
        if (*pnumwritten == -1) {
            break;
        }
    }
}

#include "output.inl"

