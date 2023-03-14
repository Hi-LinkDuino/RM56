/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "frameworks/bridge/declarative_frontend/engine/jsi/modules/jsi_timer_module.h"

#include "base/log/log.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_declarative_engine.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"
#include "frameworks/bridge/js_frontend/frontend_delegate.h"

namespace OHOS::Ace::Framework {

namespace {

shared_ptr<JsValue> SetTimeoutOrInterval(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc, bool isInterval)
{
    if (argc < 2) {
        LOGE("argc should be greater than or equal to 2");
        return runtime->NewNull();
    }
    if (!argv[0]->IsFunction(runtime) || !argv[1]->IsNumber(runtime)) {
        LOGW("argv[0] is not IsFunction or argv[1] is not number");
        return runtime->NewNull();
    }

    auto instance = static_cast<JsiDeclarativeEngineInstance*>(runtime->GetEmbedderData());
    if (instance == nullptr) {
        LOGE("get jsi engine instance failed");
        return runtime->NewNull();
    }
    auto delegate = instance->GetDelegate();
    if (!delegate) {
        LOGE("get frontend delegate failed");
        return runtime->NewNull();
    }

    std::vector<shared_ptr<JsValue>> callBackParams;
    for (int i = 2; i < argc; ++i) {
        callBackParams.emplace_back(argv[i]);
    }
    uint32_t callbackId = JsiTimerModule::GetInstance()->AddCallBack(argv[0], callBackParams);
    uint32_t delay = static_cast<uint32_t>(argv[1]->ToInt32(runtime));
    delegate->WaitTimer(std::to_string(callbackId), std::to_string(delay), isInterval, true);
    return runtime->NewInt32(callbackId);
}

void ClearTimeoutOrInterval(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc < 1) {
        LOGE("argc is invalid");
        return;
    }
    if (!argv[0]->IsNumber(runtime)) {
        LOGE("argv[0] is not number");
        return;
    }

    auto instance = static_cast<JsiDeclarativeEngineInstance*>(runtime->GetEmbedderData());
    if (instance == nullptr) {
        LOGE("get jsi engine instance failed");
        return;
    }
    auto delegate = instance->GetDelegate();
    if (!delegate) {
        LOGE("get frontend delegate failed");
        return;
    }

    uint32_t callbackId = static_cast<uint32_t>(argv[0]->ToInt32(runtime));
    JsiTimerModule::GetInstance()->RemoveCallBack(callbackId);
    delegate->ClearTimer(std::to_string(callbackId));
}

}

shared_ptr<JsValue> SetTimeout(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return SetTimeoutOrInterval(runtime, thisObj, argv, argc, false);
}

shared_ptr<JsValue> SetInterval(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return SetTimeoutOrInterval(runtime, thisObj, argv, argc, true);
}

shared_ptr<JsValue> ClearTimeout(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    ClearTimeoutOrInterval(runtime, thisObj, argv, argc);
    return runtime->NewNull();
}

shared_ptr<JsValue> ClearInterval(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    ClearTimeoutOrInterval(runtime, thisObj, argv, argc);
    return runtime->NewNull();
}

JsiTimerModule* JsiTimerModule::GetInstance()
{
    static JsiTimerModule instance;
    return &instance;
}

uint32_t JsiTimerModule::AddCallBack(const shared_ptr<JsValue>& func, const std::vector<shared_ptr<JsValue>>& params)
{
    ++callBackId_;
    callBackFuncMap_[callBackId_] = func;
    callBackParamsMap_[callBackId_] = params;
    return callBackId_;
}

void JsiTimerModule::RemoveCallBack(uint32_t callBackId)
{
    if (callBackFuncMap_.find(callBackId) != callBackFuncMap_.end()) {
        callBackFuncMap_.erase(callBackId);
    }
    if (callBackParamsMap_.find(callBackId) != callBackParamsMap_.end()) {
        callBackParamsMap_.erase(callBackId);
    }
}

bool JsiTimerModule::GetCallBack(uint32_t callBackId, shared_ptr<JsValue>& func,
    std::vector<shared_ptr<JsValue>>& params)
{
    auto iterFunc = callBackFuncMap_.find(callBackId);
    auto iterParams = callBackParamsMap_.find(callBackId);
    if (iterFunc == callBackFuncMap_.end()) {
        LOGE("find callback function failed, callbackId = %{public}u", callBackId);
        return false;
    }
    if (iterParams == callBackParamsMap_.end()) {
        LOGE("find callback parameters failed, callbackId = %{public}u", callBackId);
        return false;
    }
    func = iterFunc->second;
    params = iterParams->second;
    return true;
}

void JsiTimerModule::InitTimerModule(const shared_ptr<JsRuntime>& runtime, shared_ptr<JsValue>& moduleObj)
{
    moduleObj->SetProperty(runtime, SET_TIMEOUT, runtime->NewFunction(SetTimeout));
    moduleObj->SetProperty(runtime, SET_INTERVAL, runtime->NewFunction(SetInterval));
    moduleObj->SetProperty(runtime, CLEAR_TIMEOUT, runtime->NewFunction(ClearTimeout));
    moduleObj->SetProperty(runtime, CLEAR_INTERVAL, runtime->NewFunction(ClearInterval));
}

} // namespace OHOS::Ace::Framework