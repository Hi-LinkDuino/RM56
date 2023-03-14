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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_LOCAL_STORAGE_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_LOCAL_STORAGE_H

#include "frameworks/bridge/declarative_frontend/engine/bindings_defines.h"
#include "frameworks/bridge/declarative_frontend/engine/js_ref_ptr.h"

namespace OHOS::Ace::Framework {
class JSLocalStorage {
public:
    JSLocalStorage();
    ~JSLocalStorage() = default;

    static void JSBind(BindingTarget globalObj);
    static void GetShared(const JSCallbackInfo& info);

    static void AddStorage(int32_t key, const JSRef<JSObject>& value);
    static void RemoveStorage(int32_t key);
private:
    static thread_local std::unordered_map<int32_t, JSRef<JSObject>> storages_;
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_LOCAL_STORAGE_H
