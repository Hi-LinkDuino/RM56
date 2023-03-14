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
#include "frameworks/bridge/declarative_frontend/jsview/js_local_storage.h"

#include "frameworks/bridge/declarative_frontend/engine/bindings.h"
#include "frameworks/core/common/container.h"

namespace OHOS::Ace::Framework {

thread_local std::unordered_map<int32_t, JSRef<JSObject>> JSLocalStorage::storages_;

JSLocalStorage::JSLocalStorage()
{
    LOGD("JSLocalStorage ctor");
}

void JSLocalStorage::JSBind(BindingTarget globalObj)
{
    JSClass<JSLocalStorage>::Declare("NativeLocalStorage");
    JSClass<JSLocalStorage>::StaticMethod("GetShared", JSLocalStorage::GetShared);
    JSClass<JSLocalStorage>::Bind<>(globalObj);
}

void JSLocalStorage::AddStorage(int32_t key, const JSRef<JSObject>& value)
{
    if (storages_.find(key) != storages_.end()) {
        LOGW("A local storage already exists for key %d", key);
        return;
    }

    storages_.emplace(key, value);
}

void JSLocalStorage::RemoveStorage(int32_t key)
{
    auto it = storages_.find(key);
    if (it != storages_.end()) {
        storages_.erase(it);
    } else {
        LOGW("A local storage with key %{public}d does not exist!", key);
    }
}

void JSLocalStorage::GetShared(const JSCallbackInfo& info)
{
    int32_t currentInstance = Container::CurrentId();
    LOGD("Current ID is %{public}d", currentInstance);
    auto it = storages_.find(currentInstance);
    if (it == storages_.end()) {
        LOGW("LocalStorage with ID %{public}d not found!", currentInstance);
        return;
    }
    LOGI("JSLocalStorage::GetShared find ID:%{public}d", currentInstance);
    info.SetReturnValue(it->second);
}

} // namespace OHOS::Ace::Framework