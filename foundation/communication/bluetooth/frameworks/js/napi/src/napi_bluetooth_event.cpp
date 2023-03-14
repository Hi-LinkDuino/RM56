/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include <uv.h>
#include "bluetooth_utils.h"
#include "napi_bluetooth_event.h"

namespace OHOS {
namespace Bluetooth {
void NapiEvent::EventNotify(AsyncEventData *asyncEvent)
{
    HILOGI("Enter bluetooth event notify");
    HILOGI("NEW");
    
    if (asyncEvent == nullptr || asyncEvent->callback_ == nullptr) {
        HILOGI("asyncEvent callback_ is null.");
        return;
    }
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(asyncEvent->env_, &loop);

    uv_work_t *work = new uv_work_t;
    if (work == nullptr) {
        HILOGI("uv_work_t work is null.");
        delete asyncEvent;
        asyncEvent = nullptr;
        return;
    }
    
    napi_value callback = nullptr;
    napi_status status = napi_get_reference_value(asyncEvent->env_, asyncEvent->callback_, &callback);
    if (status != napi_ok || callback == nullptr) {
        HILOGI("reference_value is null.");
        return;
    }
    uint32_t refCount = INVALID_REF_COUNT;
    napi_reference_ref(asyncEvent->env_, asyncEvent->callback_, &refCount);
    HILOGI("increments the reference count, refCount: %{public}d", refCount);
    work->data = asyncEvent;

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            AsyncEventData *callbackInfo = static_cast<AsyncEventData*>(work->data);
            napi_value result = nullptr;
            napi_value callback = nullptr;
            napi_value undefined = nullptr;
            napi_value callResult = nullptr;
            napi_get_undefined(callbackInfo->env_, &undefined);
            result = callbackInfo->packResult();
            napi_get_reference_value(callbackInfo->env_, callbackInfo->callback_, &callback);
            if (callback != nullptr) {
                HILOGI("napi_call_function called");
                napi_call_function(callbackInfo->env_, undefined, callback, ARGS_SIZE_ONE, &result, &callResult);
            }
            uint32_t refCount = INVALID_REF_COUNT;
            napi_reference_unref(callbackInfo->env_, callbackInfo->callback_, &refCount);
            HILOGI("uv_queue_work unref, refCount: %{public}d", refCount);
            if (refCount == 0) {
                napi_delete_reference(callbackInfo->env_, callbackInfo->callback_);
            }
            delete callbackInfo;
            delete work;
            work = nullptr;
        }
    );
}

napi_value NapiEvent::CreateResult(const std::shared_ptr<BluetoothCallbackInfo> &cb, int value)
{
    napi_value result = nullptr;
    napi_create_object(cb->env_, &result);
    ConvertStateChangeParamToJS(cb->env_, result, cb->deviceId_, value);
    return result;
}

napi_value NapiEvent::CreateResult(const std::shared_ptr<BluetoothCallbackInfo> &cb,
    BluetoothOppTransferInformation &information)
{
    napi_value result = nullptr;
    napi_create_object(cb->env_, &result);
    ConvertOppTransferInformationToJS(cb->env_, result, information);
    return result;
}

napi_value NapiEvent::CreateResult(const std::shared_ptr<BluetoothCallbackInfo> &cb,
    GattCharacteristic &characteristic)
{
    napi_value result = nullptr;
    napi_create_object(cb->env_, &result);
    ConvertBLECharacteristicToJS(cb->env_, result, characteristic);
    return result;
}

napi_value NapiEvent::OnEvent(napi_env env, napi_callback_info info,
    std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>> &callbackInfos)
{
    size_t expectedArgsCount = ARGS_SIZE_TWO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    napi_value thisVar = nullptr;

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 2 argument.");
        return ret;
    }
    std::string type;
    if (!ParseString(env, type, argv[PARAM0])) {
        HILOGE("string expected.");
        return ret;
    }
    std::shared_ptr<BluetoothCallbackInfo> callbackInfo = std::make_shared<BluetoothCallbackInfo>();
    callbackInfo->env_ = env;

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[PARAM1], &valueType);
    if (valueType != napi_function) {
        HILOGE("Wrong argument type. Function expected.");
        return ret;
    }
    napi_create_reference(env, argv[PARAM1], 1, &callbackInfo->callback_);
    callbackInfos[type] = callbackInfo;
    HILOGI("%{public}s is registered", type.c_str());
    return ret;
}

napi_value NapiEvent::OffEvent(napi_env env, napi_callback_info info,
    std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>> &callbackInfos)
{
    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value thisVar = nullptr;

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 1 argument.");
        return ret;
    }
    std::string type;
    if (!ParseString(env, type, argv[PARAM0])) {
        HILOGE("string expected.");
        return ret;
    }
    auto it = callbackInfos.find(type);
    if (it == callbackInfos.end() || it->second == nullptr) {
        HILOGE("type %{public}s callbackInfos isn't exist.", type.c_str());
        return ret;
    }
    uint32_t refCount = INVALID_REF_COUNT;
    napi_reference_unref(env, it->second->callback_, &refCount);
    HILOGI("decrements the refernce count, refCount: %{public}d", refCount);
    if (refCount == 0) {
        napi_delete_reference(env, it->second->callback_);
    }
    it->second = nullptr;
    HILOGI("%{public}s is unregistered", type.c_str());
    return ret;
}
}  // namespace Bluetooth
}  // namespace OHOS