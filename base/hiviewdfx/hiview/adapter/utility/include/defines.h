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
#ifndef HIVIEW_BASE_DEFINES_H
#define HIVIEW_BASE_DEFINES_H

#ifndef __UNUSED

#if defined(_MSC_VER)
#define __UNUSED // Note: actually gcc seems to also supports this syntax.
#else
#if defined(__GNUC__)
#define __UNUSED __attribute__ ((__unused__))
#endif
#endif

#endif

#ifdef _WIN32
#define DllExport __declspec (dllexport)
#else
#define DllExport
#endif // _WIN32

#endif // HIVIEW_BASE_DEFINES_H