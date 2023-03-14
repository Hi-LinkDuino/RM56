/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_MACROS_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_MACROS_H

#define ACE_FORCE_EXPORT __attribute__((visibility("default")))

#ifndef ACE_EXPORT
#ifndef WEARABLE_PRODUCT
#define ACE_EXPORT ACE_FORCE_EXPORT
#else
#define ACE_EXPORT
#endif
#endif

// The macro "ACE_FORCE_EXPORT_WITH_PREVIEW" is used to replace the macro "ACE_FORCE_EXPORT"
// when adapting the napi to the previewer.
#ifndef ACE_FORCE_EXPORT_WITH_PREVIEW
#ifndef WINDOWS_PLATFORM
#define ACE_FORCE_EXPORT_WITH_PREVIEW ACE_FORCE_EXPORT
#else
#define ACE_FORCE_EXPORT_WITH_PREVIEW __declspec(dllexport)
#endif
#endif

// The macro "ACE_EXPORT_WITH_PREVIEW" is used to replace the macro "ACE_EXPORT"
// when adapting the napi to the previewer.
#ifndef ACE_EXPORT_WITH_PREVIEW
#ifndef WINDOWS_PLATFORM
#define ACE_EXPORT_WITH_PREVIEW ACE_EXPORT
#else
#ifndef WEARABLE_PRODUCT
#define ACE_EXPORT_WITH_PREVIEW __declspec(dllexport)
#else
#define ACE_EXPORT_WITH_PREVIEW
#endif
#endif
#endif

#ifdef ACE_DEBUG

#ifdef NDEBUG
#define CANCEL_NDEBUG
#undef NDEBUG
#endif // NDEBUG

#include <cassert>

#ifdef CANCEL_NDEBUG
#define NDEBUG
#undef CANCEL_NDEBUG
#endif // CANCEL_NDEBUG

#define ACE_DCHECK(expr) assert(expr)
#else
#define ACE_DCHECK(expr) ((void)0)

#endif // ACE_DEBUG

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_MACROS_H
