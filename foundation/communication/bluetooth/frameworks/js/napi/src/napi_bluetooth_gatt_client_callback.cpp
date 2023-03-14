/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include <unistd.h>
#include "bluetooth_log.h"
#include "napi_bluetooth_event.h"
#include "napi_bluetooth_gatt_client.h"
#include "napi_bluetooth_gatt_client_callback.h"

namespace OHOS {
namespace Bluetooth {
void NapiGattClientCallback::OnCharacteristicChanged(const GattCharacteristic &characteristic)
{
    HILOGI("enter");
    std::unique_lock<std::mutex> guard(g_gattClientCallbackInfosMutex);
    if (!callbackInfos_[STR_BT_GATT_CLIENT_CALLBACK_BLE_CHARACTERISTIC_CHANGE]) {
        HILOGI("This callback is not registered by ability.");
        return;
    }
    std::shared_ptr<GattCharacteristicCallbackInfo> callbackInfo =
        std::static_pointer_cast<GattCharacteristicCallbackInfo>(
            callbackInfos_[STR_BT_GATT_CLIENT_CALLBACK_BLE_CHARACTERISTIC_CHANGE]);

    HILOGI("uuid is %{public}s", characteristic.GetUuid().ToString().c_str());
    callbackInfo->characteristic_ = characteristic;
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(callbackInfo->env_, &loop);
    uv_work_t *work = new uv_work_t;
    if (work == nullptr) {
        HILOGI("uv_work_t work is null.");
        return;
    }
    work->data = (void*)callbackInfo.get();
    uint32_t refCount = INVALID_REF_COUNT;
    napi_reference_ref(callbackInfo->env_, callbackInfo->callback_, &refCount);
    HILOGI("increments the reference count, refCount: %{public}d", refCount);

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            GattCharacteristicCallbackInfo *callbackInfo = (GattCharacteristicCallbackInfo *)work->data;
            napi_value result = nullptr;
            napi_create_object(callbackInfo->env_, &result);
            ConvertBLECharacteristicToJS(callbackInfo->env_, result, callbackInfo->characteristic_);
            napi_value callback = nullptr;
            napi_value undefined = nullptr;
            napi_value callResult = nullptr;
            napi_get_undefined(callbackInfo->env_, &undefined);
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
            delete work;
            work = nullptr;
        }
    );
}

void NapiGattClientCallback::OnCharacteristicReadResult(const GattCharacteristic &characteristic, int ret)
{
    HILOGI("enter");
    ReadCharacteristicValueCallbackInfo *callbackInfo = client_->readCharacteristicValueCallbackInfo_;
    if (!callbackInfo) {
        HILOGE("CallbackInfo does not exist");
        return;
    }
    HILOGI("asyncState: %{public}d", callbackInfo->asyncState_);
    if (callbackInfo->asyncState_ == ASYNC_START) {
        callbackInfo->ret = ret;
        callbackInfo->outputCharacteristic_ = &characteristic;
        callbackInfo->asyncState_ = ASYNC_DONE;
    }
}

void NapiGattClientCallback::OnDescriptorReadResult(const GattDescriptor &descriptor, int ret)
{
    HILOGI("enter");
    ReadDescriptorValueCallbackInfo *callbackInfo = client_->readDescriptorValueCallbackInfo_;
    if (!callbackInfo) {
        HILOGE("CallbackInfo does not exist");
        return;
    }
    HILOGI("asyncState: %{public}d", callbackInfo->asyncState_);
    if (callbackInfo->asyncState_ == ASYNC_START) {
        callbackInfo->ret = ret;
        callbackInfo->outputDescriptor_ = &descriptor;
        callbackInfo->asyncState_ = ASYNC_DONE;
    }
}

void NapiGattClientCallback::OnConnectionStateChanged(int connectionState, int ret)
{
    HILOGI("enter");
    std::unique_lock<std::mutex> guard(g_gattClientCallbackInfosMutex);
    std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>>::iterator it =
    callbackInfos_.find(STR_BT_GATT_CLIENT_CALLBACK_BLE_CONNECTIION_STATE_CHANGE);
    if (it == callbackInfos_.end() || it->second == nullptr) {
        HILOGI("This callback is not registered by ability.");
        return;
    }
    std::shared_ptr<BluetoothCallbackInfo> callbackInfo = it->second;
    HILOGI("connectionState:%{public}d, ret:%{public}d", connectionState, ret);
    callbackInfo->state_ = connectionState;
    callbackInfo->deviceId_ = client_->GetDevice()->GetDeviceAddr();
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(callbackInfo->env_, &loop);
    uv_work_t *work = new uv_work_t;
    if (work == nullptr) {
        HILOGI("uv_work_t work is null.");
        return;
    }
    work->data = (void*)callbackInfo.get();
    uint32_t refCount = INVALID_REF_COUNT;
    napi_reference_ref(callbackInfo->env_, callbackInfo->callback_, &refCount);
    HILOGI("increments the reference count, refCount: %{public}d", refCount);
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
                HILOGI("napi_call_function called");
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

void NapiGattClientCallback::OnServicesDiscovered(int status)
{
    HILOGI("enter");
}
} // namespace Bluetooth
} // namespace OHOS
