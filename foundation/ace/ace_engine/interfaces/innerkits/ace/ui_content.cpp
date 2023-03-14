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

#include "interfaces/innerkits/ace/ui_content.h"

#include <dlfcn.h>

namespace OHOS::Ace {

using CreateFunc = UIContent* (*)(void*, void*);
constexpr char UI_CONTENT_CREATE_FUNC[] = "OHOS_ACE_CreateUIContent";

UIContent* CreateUIContent(void* context, void* runtime)
{
    void* handle = dlopen("libace.z.so", RTLD_LAZY);
    if (handle == nullptr) {
        return nullptr;
    }

    auto entry = reinterpret_cast<CreateFunc>(dlsym(handle, UI_CONTENT_CREATE_FUNC));
    if (entry == nullptr) {
        dlclose(handle);
        return nullptr;
    }

    auto content = entry(context, runtime);
    if (content == nullptr) {
        dlclose(handle);
    }

    return content;
}

std::unique_ptr<UIContent> UIContent::Create(OHOS::AbilityRuntime::Context* context, NativeEngine* runtime)
{
    std::unique_ptr<UIContent> content;
    content.reset(CreateUIContent(reinterpret_cast<void*>(context), reinterpret_cast<void*>(runtime)));
    return content;
}

} // namespace OHOS::Ace
