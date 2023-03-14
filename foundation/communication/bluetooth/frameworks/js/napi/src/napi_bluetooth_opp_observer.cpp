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
#include "napi_bluetooth_opp_observer.h"

namespace OHOS {
namespace Bluetooth {
void NapiBluetoothOppObserver::OnReceiveIncomingFileChanged(const BluetoothOppTransferInformation &transferInformation)
{
    HILOGI("NapiBluetoothOppObserver::OnReceiveIncomingFileChanged called");
    if (!callbackInfos_[STR_BT_OPP_OBSERVER_RECEIVE_INCOMING_FILE]) {
        HILOGW("NapiBluetoothOppObserver::OnReceiveIncomingFileChanged: This callback is not registered by ability.");
        return;
    }
    HILOGI("NapiBluetoothOppObserver::OnReceiveIncomingFileChanged: %{public}s is registered by ability",
        STR_BT_OPP_OBSERVER_RECEIVE_INCOMING_FILE.c_str());
    std::shared_ptr<TransforInformationCallbackInfo> callbackInfo =
        std::static_pointer_cast<TransforInformationCallbackInfo>(
            callbackInfos_[STR_BT_OPP_OBSERVER_RECEIVE_INCOMING_FILE]);
    callbackInfo->deviceId_ = transferInformation.GetDeviceAddress();
    callbackInfo->information_ = transferInformation;
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(callbackInfo->env_, &loop);
    uv_work_t *work = new uv_work_t;
    work->data = (void*)callbackInfo.get();

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            TransforInformationCallbackInfo *callbackInfo = (TransforInformationCallbackInfo *)work->data;
            napi_value result = nullptr;
            napi_create_object(callbackInfo->env_, &result);
            ConvertOppTransferInformationToJS(callbackInfo->env_, result, callbackInfo->information_);
            napi_value callback = nullptr;
            napi_value undefined = nullptr;
            napi_value callResult = nullptr;
            napi_get_undefined(callbackInfo->env_, &undefined);
            napi_get_reference_value(callbackInfo->env_, callbackInfo->callback_, &callback);
            napi_call_function(callbackInfo->env_, undefined, callback, ARGS_SIZE_ONE, &result, &callResult);
            delete work;
            work = nullptr;
        });
}

void NapiBluetoothOppObserver::OnTransferStateChanged(const BluetoothOppTransferInformation &transferInformation)
{
    HILOGI("NapiBluetoothOppObserver::OnTransferStateChanged called");
    if (!callbackInfos_[STR_BT_OPP_OBSERVER_TRANSFER_STATE_CHANGE]) {
        HILOGW("NapiBluetoothOppObserver::OnTransferStateChanged: This callback is not registered by ability.");
        return;
    }
    HILOGI("NapiBluetoothOppObserver::OnTransferStateChanged: %{public}s is registered by ability",
        STR_BT_OPP_OBSERVER_TRANSFER_STATE_CHANGE.c_str());
    std::shared_ptr<TransforInformationCallbackInfo> callbackInfo =
        std::static_pointer_cast<TransforInformationCallbackInfo>(
            callbackInfos_[STR_BT_OPP_OBSERVER_TRANSFER_STATE_CHANGE]);

    callbackInfo->deviceId_ = transferInformation.GetDeviceAddress();
    callbackInfo->information_ = transferInformation;

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(callbackInfo->env_, &loop);
    uv_work_t *work = new uv_work_t;
    work->data = (void*)callbackInfo.get();

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            TransforInformationCallbackInfo *callbackInfo = (TransforInformationCallbackInfo *)work->data;
            napi_value result = nullptr;
            napi_create_object(callbackInfo->env_, &result);
            ConvertOppTransferInformationToJS(callbackInfo->env_, result, callbackInfo->information_);
            napi_value callback = nullptr;
            napi_value undefined = nullptr;
            napi_value callResult = nullptr;
            napi_get_undefined(callbackInfo->env_, &undefined);
            napi_get_reference_value(callbackInfo->env_, callbackInfo->callback_, &callback);
            napi_call_function(callbackInfo->env_, undefined, callback, ARGS_SIZE_ONE, &result, &callResult);
            delete work;
            work = nullptr;
        });
}
} // namespace Bluetooth
} // namespace OHOS