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

#include "frameworks/bridge/declarative_frontend/engine/jsi/modules/jsi_module_manager.h"

#include "base/log/log.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/modules/jsi_app_module.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/modules/jsi_curves_module.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/modules/jsi_matrix4_module.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/modules/jsi_router_module.h"

namespace OHOS::Ace::Framework {

ModuleManager* ModuleManager::GetInstance()
{
    static ModuleManager instance;
    return &instance;
}

bool ModuleManager::InitModule(
    const shared_ptr<JsRuntime>& runtime, shared_ptr<JsValue>& thisObj, const std::string& moduleName)
{
    static const std::unordered_map<std::string,
        void (*)(const shared_ptr<JsRuntime>& runtime, shared_ptr<JsValue>& thisObj)>
        MODULE_LIST = {
            { "system.router", [](const shared_ptr<JsRuntime>& runtime,
                                   shared_ptr<JsValue>& thisObj) { InitRouterModule(runtime, thisObj); } },
            { "ohos.router", [](const shared_ptr<JsRuntime>& runtime,
                                 shared_ptr<JsValue>& thisObj) { InitRouterModule(runtime, thisObj); } },
            { "system.app", [](const shared_ptr<JsRuntime>& runtime,
                                shared_ptr<JsValue>& thisObj) { InitAppModule(runtime, thisObj); } },
            { "ohos.app", [](const shared_ptr<JsRuntime>& runtime,
                              shared_ptr<JsValue>& thisObj) { InitAppModule(runtime, thisObj); } },
            { "system.curves", [](const shared_ptr<JsRuntime>& runtime,
                                   shared_ptr<JsValue>& thisObj) { InitCurvesModule(runtime, thisObj); } },
            { "ohos.curves", [](const shared_ptr<JsRuntime>& runtime,
                                 shared_ptr<JsValue>& thisObj) { InitCurvesModule(runtime, thisObj); } },
            { "system.matrix4", [](const shared_ptr<JsRuntime>& runtime,
                                    shared_ptr<JsValue>& thisObj) { InitMatrix4Module(runtime, thisObj); } },
            { "ohos.matrix4", [](const shared_ptr<JsRuntime>& runtime,
                                  shared_ptr<JsValue>& thisObj) { InitMatrix4Module(runtime, thisObj); } },
        };
    auto iter = MODULE_LIST.find(moduleName);
    if (iter != MODULE_LIST.end()) {
        iter->second(runtime, thisObj);
        LOGD("ModuleManager InitModule is %{private}s", moduleName.c_str());
        return true;
    } else {
        LOGE("register module is not found");
        return false;
    }
}

} // namespace OHOS::Ace::Framework