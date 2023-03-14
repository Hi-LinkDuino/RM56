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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_MODULES_JSI_SYSCAP_MODULE_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_MODULES_JSI_SYSCAP_MODULE_H

#include <string>
#include <unordered_map>

#include "base/utils/noncopyable.h"
#include "frameworks/bridge/js_frontend/engine/jsi/js_runtime.h"
#include "frameworks/bridge/js_frontend/engine/jsi/js_value.h"

namespace OHOS::Ace::Framework {
class JsiSyscapModule {
public:
    JsiSyscapModule() = default;
    ~JsiSyscapModule() = default;
    static JsiSyscapModule* GetInstance();
    void InitSyscapModule(const shared_ptr<JsRuntime>& runtime, shared_ptr<JsValue>& moduleObj);
    uint32_t AddCallBack(const shared_ptr<JsValue>& func, const std::vector<shared_ptr<JsValue>>& params);
    void RemoveCallBack(uint32_t callBackId);
    bool GetCallBack(uint32_t callBackId, shared_ptr<JsValue>& func, std::vector<shared_ptr<JsValue>>& params);

private:
    ACE_DISALLOW_COPY_AND_MOVE(JsiSyscapModule);

    uint32_t callBackId_ = 0;
    std::unordered_map<uint32_t, shared_ptr<JsValue>> callBackFuncMap_;
    std::unordered_map<uint32_t, std::vector<shared_ptr<JsValue>>> callBackParamsMap_;
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_MODULES_JSI_SYSCAP_MODULE_H