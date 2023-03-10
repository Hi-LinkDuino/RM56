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
#include "dynamic_module.h"

#include <string>

#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace OHOS {
namespace HiviewDFX {
DynamicModule LoadModule(const std::string &moduleName)
{
#if defined(_WIN32)
return LoadLibraryA(moduleName.c_str());
#else
return dlopen(moduleName.c_str(), RTLD_GLOBAL);
#endif
}

void UnloadModule(DynamicModule module)
{
#if defined(_WIN32)
    FreeLibrary(module);
#else
    dlclose(module);
#endif
}
}  // namespace HiviewDFX
}  // namespace OHOS
