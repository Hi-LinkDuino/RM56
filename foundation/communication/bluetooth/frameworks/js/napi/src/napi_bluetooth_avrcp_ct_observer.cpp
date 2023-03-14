/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#include "napi_bluetooth_avrcp_ct_observer.h"

namespace OHOS {
namespace Bluetooth {
void NapiAvrcpControllerObserver::OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state)
{
    HILOGI("enter, remote device address: %{public}s, state: %{public}d", GET_ENCRYPT_ADDR(device), state);
    std::unique_lock<std::shared_mutex> guard(g_avrcpCtCallbackInfosMutex);

    std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>>::iterator it =
        callbackInfos_.find(STR_BT_AVRCP_CT_CONNECTION_STATE_CHANGE);
    if (it == callbackInfos_.end() || it->second == nullptr) {
        HILOGW("This callback is not registered by ability.");
        return;
    }
    HILOGI("%{public}s is registered by ability", STR_BT_AVRCP_CT_CONNECTION_STATE_CHANGE.c_str());
    std::shared_ptr<BluetoothCallbackInfo> callbackInfo = it->second;

    callbackInfo->state_ = state;
    callbackInfo->deviceId_ = device.GetDeviceAddr();
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(callbackInfo->env_, &loop);
    uv_work_t *work = new uv_work_t;
    uint32_t refCount = INVALID_REF_COUNT;
    napi_reference_ref(callbackInfo->env_, callbackInfo->callback_, &refCount);
    HILOGI("increments the reference count, refCount: %{public}d", refCount);
    work->data = (void*)callbackInfo.get();

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            BluetoothCallbackInfo *callbackInfo = (BluetoothCallbackInfo *)work->data;
            napi_value result = nullptr;
            napi_create_object(callbackInfo->env_, &result);
            ConvertStateChangeParamToJS(callbackInfo->env_, result, callbackInfo->deviceId_, callbackInfo->state_);
            napi_value callback = nullptr;
            napi_value undefined = nullptr;
            napi_value callResult = nullptr;
            napi_get_undefined(callbackInfo->env_, &undefined);
            napi_get_reference_value(callbackInfo->env_, callbackInfo->callback_, &callback);
            if (callback != nullptr) {
                HILOGI("avrcp ct napi_call_function called");
                napi_call_function(callbackInfo->env_, undefined, callback, ARGS_SIZE_ONE, &result, &callResult);
            }
            uint32_t refCount = INVALID_REF_COUNT;
            napi_reference_unref(callbackInfo->env_, callbackInfo->callback_, &refCount);
            HILOGI("uv_queue_work unref, refCount: %{public}d", refCount);
            if (refCount == 0) {
                napi_delete_reference(callbackInfo->env_, callbackInfo->callback_);
            }
            delete work;
            work = nullptr;
        }
    );
}

} // namespace Bluetooth
} // namespace OHOS