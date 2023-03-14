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
#ifndef HIVIEW_OAL_DYNAMIC_MODULE_LINUX_H
#define HIVIEW_OAL_DYNAMIC_MODULE_LINUX_H

#include <string>
#if defined(_WIN32)
#include <windows.h>
#endif
#include "defines.h"
namespace OHOS {
namespace HiviewDFX {
#if defined(_WIN32)
using DynamicModule = HMODULE;
#define DynamicModuleDefault NULL
#else
using DynamicModule = void *;
#define DynamicModuleDefault nullptr
#endif
DynamicModule LoadModule(const std::string &name);
void UnloadModule(DynamicModule module);
}  // namespace HiviewDFX
}  // namespace OHOS

#endif  // HIVIEW_OAL_DYNAMIC_MODULE_H