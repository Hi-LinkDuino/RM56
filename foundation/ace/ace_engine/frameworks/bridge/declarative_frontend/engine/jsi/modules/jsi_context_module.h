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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_CONTEXT_MODULE_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_CONTEXT_MODULE_H

#include <string>
#include <unordered_map>

#include "base/utils/noncopyable.h"
#include "frameworks/bridge/js_frontend/engine/jsi/js_runtime.h"
#include "frameworks/bridge/js_frontend/engine/jsi/js_value.h"

namespace OHOS::Ace::Framework {

class JsiContextModule {
public:
    JsiContextModule() = default;
    ~JsiContextModule() = default;

    static JsiContextModule* GetInstance();
    void InitContextModule(const std::shared_ptr<JsRuntime>& runtime, std::shared_ptr<JsValue> moduleObj);

    static void AddContext(int32_t key, const std::shared_ptr<JsValue>&);
    static void RemoveContext(int32_t key);
    static std::shared_ptr<JsValue> GetContext(const std::shared_ptr<JsRuntime>& runtime,
        const std::shared_ptr<JsValue>& thisObj, const std::vector<std::shared_ptr<JsValue>>& argv, int32_t argc);

private:
    static thread_local std::unordered_map<int32_t, std::shared_ptr<JsValue>> contexts_;
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_CONTEXT_MODULE_H
