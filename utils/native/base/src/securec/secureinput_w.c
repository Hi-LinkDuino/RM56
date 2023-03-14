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
#ifdef __STDC_WANT_SECURE_LIB__
#undef __STDC_WANT_SECURE_LIB__
#endif
#ifdef _CRTIMP_ALTERNATIVE
#undef _CRTIMP_ALTERNATIVE
#endif
#define __STDC_WANT_SECURE_LIB__ 0
#define _CRTIMP_ALTERNATIVE     //comment microsoft *_s function
#endif

/* if some platforms don't have wchar.h, don't include it */
#if !(defined(SECUREC_VXWORKS_PLATFORM))
/* This header file is placed below secinput.h, which will cause tool alarm,
  * but  If there is no macro above, it will cause compiling alarm */
#include <wchar.h>
#endif
#include "secinput.h"

#ifndef SECUREC_FOR_WCHAR
#define SECUREC_FOR_WCHAR
#endif

#ifndef WEOF
#define WEOF ((wchar_t)-1)
#endif

#if defined(SECUREC_VXWORKS_PLATFORM) && !defined(__WINT_TYPE__)
typedef wchar_t wint_t;
#endif

typedef wint_t SecInt;
typedef wint_t SecUnsignedInt;
typedef wchar_t SecChar;
typedef wchar_t SecUnsignedChar;

#include "input.inl"

