/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "interfaces/innerkits/ace/declarative_module_preloader.h"

#include <dlfcn.h>

namespace OHOS::Ace {

using CreateFunc = void (*)(void*);
constexpr char PRE_INIT_ACE_MODULE_FUNC[] = "OHOS_ACE_PreloadAceModule";

void InitAceModule(void* runtime)
{
    void* handle = dlopen("libace_engine_declarative_ark.z.so", RTLD_LAZY);
    if (handle == nullptr) {
        return;
    }

    auto entry = reinterpret_cast<CreateFunc>(dlsym(handle, PRE_INIT_ACE_MODULE_FUNC));
    if (entry == nullptr) {
        dlclose(handle);
        return;
    }

    entry(runtime);
}

void DeclarativeModulePreloader::Preload(NativeEngine& runtime)
{
    InitAceModule(reinterpret_cast<void*>(&runtime));
}

} // namespace OHOS::Ace