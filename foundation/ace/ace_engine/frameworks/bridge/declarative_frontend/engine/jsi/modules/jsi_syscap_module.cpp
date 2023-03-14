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

#include "frameworks/bridge/declarative_frontend/engine/jsi/modules/jsi_syscap_module.h"

#include "base/log/log.h"
#include "frameworks/bridge/declarative_frontend/engine/jsi/jsi_declarative_engine.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"
#include "frameworks/bridge/js_frontend/frontend_delegate.h"

namespace OHOS::Ace::Framework {
JsiSyscapModule* JsiSyscapModule::GetInstance()
{
    static JsiSyscapModule instance;
    return &instance;
}

uint32_t JsiSyscapModule::AddCallBack(const shared_ptr<JsValue>& func, const std::vector<shared_ptr<JsValue>>& params)
{
    ++callBackId_;
    callBackFuncMap_[callBackId_] = func;
    callBackParamsMap_[callBackId_] = params;
    return callBackId_;
}

void JsiSyscapModule::RemoveCallBack(uint32_t callBackId)
{
    if (callBackFuncMap_.find(callBackId) != callBackFuncMap_.end()) {
        callBackFuncMap_.erase(callBackId);
    }
    if (callBackParamsMap_.find(callBackId) != callBackParamsMap_.end()) {
        callBackParamsMap_.erase(callBackId);
    }
}

shared_ptr<JsValue> CanIUse(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& thisObj,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 1) {
        LOGE("argc should be 1");
        return runtime->NewNull();
    }
    if (!argv[0]->IsString(runtime)) {
        LOGW("argv[0] is not IsString");
        return runtime->NewNull();
    }

    std::string syscapString = argv[0]->ToString(runtime);

    bool ret = Ace::SystemProperties::IsSyscapExist(syscapString.c_str());
    return runtime->NewBoolean(ret);
}

bool JsiSyscapModule::GetCallBack(uint32_t callBackId, shared_ptr<JsValue>& func,
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

void JsiSyscapModule::InitSyscapModule(const shared_ptr<JsRuntime>& runtime, shared_ptr<JsValue>& moduleObj)
{
    moduleObj->SetProperty(runtime, CAN_IUSE, runtime->NewFunction(CanIUse));
}
} // namespace OHOS::Ace::Framework