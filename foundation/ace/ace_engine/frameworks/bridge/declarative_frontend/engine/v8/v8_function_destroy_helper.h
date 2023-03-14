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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_FUNCTION_DESTROY_HELPER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_FUNCTION_DESTROY_HELPER_H

#include "third_party/v8/include/v8.h"

#include <mutex>
#include <unordered_map>

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_utils.h"

namespace OHOS::Ace::Framework {

class V8FunctionDestroyCallbackHelper {
public:
    static void AddDestroyCallback(std::function<void(v8::Isolate*)> callback, v8::Isolate* isolate);
    static void DeleteFunctionTemplate(v8::Isolate* isolate);

private:
    static std::unordered_map<v8::Isolate*, std::vector<std::function<void(v8::Isolate*)>>> destroyCallbacks_;
    static std::mutex helperMutex_;
};

}; // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_FUNCTION_DESTROY_HELPER_H
