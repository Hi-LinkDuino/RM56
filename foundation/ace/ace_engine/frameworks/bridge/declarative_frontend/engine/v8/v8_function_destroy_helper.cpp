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

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_function_destroy_helper.h"

namespace OHOS::Ace::Framework {

std::unordered_map<v8::Isolate*,
    std::vector<std::function<void(v8::Isolate*)>>> V8FunctionDestroyCallbackHelper::destroyCallbacks_;

std::mutex V8FunctionDestroyCallbackHelper::helperMutex_;

void V8FunctionDestroyCallbackHelper::AddDestroyCallback(std::function<void(v8::Isolate*)> callback,
    v8::Isolate* isolate)
{
    std::lock_guard<std::mutex> lock(helperMutex_);
    if (destroyCallbacks_.find(isolate) != destroyCallbacks_.end()) {
        destroyCallbacks_[isolate].emplace_back(callback);
    } else {
        std::vector<std::function<void(v8::Isolate*)>> destroyCallbacks;
        destroyCallbacks.emplace_back(callback);
        destroyCallbacks_[isolate] = destroyCallbacks;
    }
}

void V8FunctionDestroyCallbackHelper::DeleteFunctionTemplate(v8::Isolate* isolate)
{
    LOGI("Erase function template, destroy callback size %{public}d", static_cast<int32_t>(destroyCallbacks_.size()));
    std::lock_guard<std::mutex> lock(helperMutex_);
    if (destroyCallbacks_.find(isolate) == destroyCallbacks_.end()) {
        return;
    }
    auto& destroyCallbacks = destroyCallbacks_[isolate];
    for (auto&& destroyCallback : destroyCallbacks) {
        if (destroyCallback) {
            destroyCallback(isolate);
        }
    }
    destroyCallbacks_.erase(isolate);
}

} // namespace OHOS::Ace::Framework
