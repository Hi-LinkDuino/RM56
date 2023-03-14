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
#include "napi_bluetooth_host_observer.h"

#include "bluetooth_host.h"
#include "bluetooth_log.h"
#include "bluetooth_utils.h"

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include <uv.h>

namespace OHOS {
namespace Bluetooth {
void NapiBluetoothHostObserver::UvQueueWorkOnStateChanged(uv_work_t *work, BluetoothState &state)
{
    HILOGI("start");

    if (work == nullptr) {
        HILOGE("work is null");
        return;
    }
    auto callbackData = (AfterWorkCallbackData<NapiBluetoothHostObserver,
        decltype(&NapiBluetoothHostObserver::UvQueueWorkOnStateChanged),
        BluetoothState> *)work->data;
    if (callbackData == nullptr) {
        HILOGE("callbackData is null");
        return;
    }

    napi_value result = 0;
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_value callResult = 0;

    napi_get_undefined(callbackData->env, &undefined);
    HILOGD("state is %{public}d", state);
    napi_create_int32(callbackData->env, static_cast<int32_t>(state), &result);
    napi_get_reference_value(callbackData->env, callbackData->callback, &callback);
    napi_call_function(callbackData->env, undefined, callback, ARGS_SIZE_ONE, &result, &callResult);
}

void NapiBluetoothHostObserver::OnStateChanged(const int transport, const int status)
{
    HILOGD("start");
    BluetoothState state = BluetoothState::STATE_OFF;
    if (!DealStateChange(transport, status, state)) {
        return;
    }

    std::shared_ptr<BluetoothCallbackInfo> callbackInfo = GetCallbackInfoByType(REGISTER_STATE_CHANGE_TYPE);
    if (callbackInfo == nullptr) {
        HILOGI("This callback is not registered by ability.");
        return;
    }
    
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(callbackInfo->env_, &loop);
    if (loop == nullptr) {
        HILOGE("loop instance is nullptr");
        return;
    }

    auto callbackData = new (std::nothrow) AfterWorkCallbackData<NapiBluetoothHostObserver,
        decltype(&NapiBluetoothHostObserver::UvQueueWorkOnStateChanged),
        BluetoothState>();
    if (callbackData == nullptr) {
        HILOGE("new callbackData failed");
        return;
    }

    callbackData->object = this;
    callbackData->function = &NapiBluetoothHostObserver::UvQueueWorkOnStateChanged;
    callbackData->env = callbackInfo->env_;
    callbackData->callback = callbackInfo->callback_;
    callbackData->data = state;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        HILOGE("new work failed");
        delete callbackData;
        callbackData = nullptr;
        return;
    }
    work->data = (void *)callbackData;
    int ret = uv_queue_work(
        loop, work, [](uv_work_t *work) {}, AfterWorkCallback<decltype(callbackData)>);
    if (ret != 0) {
        delete callbackData;
        callbackData = nullptr;
        delete work;
        work = nullptr;
    }
}

void NapiBluetoothHostObserver::OnDiscoveryStateChanged(int status)
{
    switch (status) {
        case DISCOVERY_STARTED:
            HILOGD("DISCOVERY_STARTED(1)");
            break;
        case DISCOVERYING:
            HILOGD("DISCOVERYING(2)");
            break;
        case DISCOVERY_STOPED:
            HILOGD("DISCOVERY_STOPED(3)");
            break;
        default:
            HILOGE("invaild status is %{public}d", status);
            break;
    }
}

void NapiBluetoothHostObserver::UvQueueWorkOnDiscoveryResult(
    uv_work_t *work, std::shared_ptr<BluetoothRemoteDevice> &device)
{
    HILOGI("start");

    if (work == nullptr) {
        HILOGE("work is null");
        return;
    }
    auto callbackData = (AfterWorkCallbackData<NapiBluetoothHostObserver,
        decltype(&NapiBluetoothHostObserver::UvQueueWorkOnDiscoveryResult),
        std::shared_ptr<BluetoothRemoteDevice>> *)work->data;
    if (callbackData == nullptr) {
        HILOGE("callbackData is null");
        return;
    }

    napi_value result = 0;
    napi_value value = 0;
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_value callResult = 0;
    napi_get_undefined(callbackData->env, &undefined);
    HILOGD("deviceId is %{public}s", GetEncryptAddr(device->GetDeviceAddr()).c_str());
    napi_create_array(callbackData->env, &result);
    napi_create_string_utf8(callbackData->env, device->GetDeviceAddr().c_str(), device->GetDeviceAddr().size(), &value);
    napi_set_element(callbackData->env, result, 0, value);

    napi_get_reference_value(callbackData->env, callbackData->callback, &callback);
    napi_call_function(callbackData->env, undefined, callback, ARGS_SIZE_ONE, &result, &callResult);
}

void NapiBluetoothHostObserver::OnDiscoveryResult(const BluetoothRemoteDevice &device)
{
    HILOGI("start");
    std::shared_ptr<BluetoothCallbackInfo> callbackInfo = GetCallbackInfoByType(REGISTER_DEVICE_FIND_TYPE);
    if (callbackInfo == nullptr) {
        HILOGI("This callback is not registered by ability.");
        return;
    }
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(callbackInfo->env_, &loop);
    if (loop == nullptr) {
        HILOGE("loop instance is nullptr");
        return;
    }

    auto callbackData = new (std::nothrow) AfterWorkCallbackData<NapiBluetoothHostObserver,
        decltype(&NapiBluetoothHostObserver::UvQueueWorkOnDiscoveryResult),
        std::shared_ptr<BluetoothRemoteDevice>>();
    if (callbackData == nullptr) {
        HILOGE("new callbackData failed");
        return;
    }

    callbackData->object = this;
    callbackData->function = &NapiBluetoothHostObserver::UvQueueWorkOnDiscoveryResult;
    callbackData->env = callbackInfo->env_;
    callbackData->callback = callbackInfo->callback_;
    callbackData->data = std::make_shared<BluetoothRemoteDevice>(device);

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        HILOGE("new work failed");
        delete callbackData;
        callbackData = nullptr;
        return;
    }

    work->data = (void *)callbackData;

    int ret = uv_queue_work(
        loop, work, [](uv_work_t *work) {}, AfterWorkCallback<decltype(callbackData)>);
    if (ret != 0) {
        delete callbackData;
        callbackData = nullptr;
        delete work;
        work = nullptr;
    }
}

void NapiBluetoothHostObserver::OnPairRequested(const BluetoothRemoteDevice &device)
{
    HILOGI("start");
    BluetoothRemoteDevice remoteDevice;
    if (device.GetTransportType() == BT_TRANSPORT_BREDR) {
        remoteDevice = BluetoothHost::GetDefaultHost().GetRemoteDevice(device.GetDeviceAddr(), BT_TRANSPORT_BREDR);
    } else if (device.GetTransportType() == BT_TRANSPORT_BLE) {
        remoteDevice = BluetoothHost::GetDefaultHost().GetRemoteDevice(device.GetDeviceAddr(), BT_TRANSPORT_BLE);
    }
    remoteDevice.PairRequestReply(true);
}

void NapiBluetoothHostObserver::OnPairConfirmed(const BluetoothRemoteDevice &device, int reqType, int number)
{
    std::string remoteAddr = device.GetDeviceAddr();
    int transportType = device.GetTransportType();
    HILOGI("remote deviceId is %{public}s, transportType is %{public}d",
        GetEncryptAddr(remoteAddr).c_str(), transportType);
    if (transportType == BT_TRANSPORT_BREDR) {
        DealBredrPairComfirmed(remoteAddr, reqType, number);
    } else if (transportType == BT_TRANSPORT_BLE) {
        DealBlePairComfirmed(remoteAddr, reqType, number);
    }
}

void NapiBluetoothHostObserver::OnScanModeChanged(int mode)
{
    HILOGI("mode is %{public}d", mode);
}

void NapiBluetoothHostObserver::OnDeviceNameChanged(const std::string &deviceName)
{
    HILOGI("name is %{public}s", deviceName.c_str());
}

void NapiBluetoothHostObserver::OnDeviceAddrChanged(const std::string &address)
{
    HILOGI("address is %{public}s", GetEncryptAddr(address).c_str());
}

void NapiBluetoothHostObserver::EnableBt()
{
    HILOGI("start");
    BluetoothHost *host = &BluetoothHost::GetDefaultHost();
    bool enabled = host->EnableBt();
    if (!enabled) {
        HILOGE("failed");
    }
    SetCurrentAppOperate(false);
}

void NapiBluetoothHostObserver::DisableBle()
{
    HILOGI("start");
    BluetoothHost *host = &BluetoothHost::GetDefaultHost();
    bool enabled = host->DisableBle();
    if (!enabled) {
        HILOGE("failed");
    }
    SetCurrentAppOperate(false);
}

bool NapiBluetoothHostObserver::DealStateChange(const int transport, const int status, BluetoothState &state)
{
    HILOGD("transport is %{public}d, status is %{public}d", transport, status);
    bool isCallback = true;
    if (transport == BT_TRANSPORT_BREDR) {
        switch (status) {
            case BTStateID::STATE_TURNING_ON:
                HILOGD("STATE_TURNING_ON(1)");
                state = BluetoothState::STATE_TURNING_ON;
                break;
            case BTStateID::STATE_TURN_ON:
                HILOGD("STATE_ON(2)");
                state = BluetoothState::STATE_ON;
                break;
            case BTStateID::STATE_TURNING_OFF:
                HILOGD("STATE_TURNING_OFF(3)");
                state = BluetoothState::STATE_TURNING_OFF;
                break;
            case BTStateID::STATE_TURN_OFF: {
                HILOGD("STATE_OFF(0)");
                isCallback = false;
                BluetoothHost *host = &BluetoothHost::GetDefaultHost();
                if (host->IsBleEnabled() && GetCurrentAppOperate()) {
                    DisableBle();
                }
                break;
            }
            default:
                break;
        }
    } else if (transport == BT_TRANSPORT_BLE) {
        switch (status) {
            case BTStateID::STATE_TURNING_ON:
                HILOGD("STATE_BLE_TURNING_ON(4)");
                state = BluetoothState::STATE_BLE_TURNING_ON;
                break;
            case BTStateID::STATE_TURN_ON:
                HILOGD("STATE_BLE_ON(5)");
                state = BluetoothState::STATE_BLE_ON;
                if (GetCurrentAppOperate()) {
                    EnableBt();
                }
                break;
            case BTStateID::STATE_TURNING_OFF:
                HILOGD("STATE_BLE_TURNING_OFF(6)");
                state = BluetoothState::STATE_BLE_TURNING_OFF;
                break;
            case BTStateID::STATE_TURN_OFF:
                HILOGD("STATE_OFF(0)");
                state = BluetoothState::STATE_OFF;
                break;
            default:
                break;
        }
    }
    return isCallback;
}

void NapiBluetoothHostObserver::DealBredrPairComfirmed(const std::string &addr, const int reqType, const int number)
{
    switch (reqType) {
        case PAIR_CONFIRM_TYPE_PIN_CODE:
            HILOGD("PAIR_CONFIRM_TYPE_PIN_CODE(1)");
            break;
        case PAIR_CONFIRM_TYPE_PASSKEY_DISPLAY:
            HILOGD("PAIR_CONFIRM_TYPE_PASSKEY_DISPLAY(2)");
            OnPairConfirmedCallBack(addr, number);
            break;
        case PAIR_CONFIRM_TYPE_PASSKEY_INPUT:
            HILOGD("PAIR_CONFIRM_TYPE_PASSKEY_INPUT(3)");
            break;
        case PAIR_CONFIRM_TYPE_NUMERIC:
            HILOGD("PAIR_CONFIRM_TYPE_NUMERIC(4)");
            OnPairConfirmedCallBack(addr, number);
            break;
        case PAIR_CONFIRM_TYPE_CONSENT:
            HILOGD("PAIR_CONFIRM_TYPE_CONSENT(5)");
            OnPairConfirmedCallBack(addr, number);
            break;
        default:
            HILOGE("Invalid reqType is %{public}d", reqType);
            break;
    }
}

void NapiBluetoothHostObserver::DealBlePairComfirmed(const std::string &addr, const int reqType, const int number)
{
    switch (reqType) {
        case PAIR_CONFIRM_TYPE_PIN_CODE:
            HILOGD("PAIR_CONFIRM_TYPE_PIN_CODE(1)");
            break;
        case PAIR_CONFIRM_TYPE_PASSKEY_DISPLAY:
            HILOGD("PAIR_CONFIRM_TYPE_PASSKEY_DISPLAY(2)");
            break;
        case PAIR_CONFIRM_TYPE_PASSKEY_INPUT:
            HILOGD("PAIR_CONFIRM_TYPE_PASSKEY_INPUT(3)");
            break;
        case PAIR_CONFIRM_TYPE_NUMERIC:
            HILOGD("PAIR_CONFIRM_TYPE_NUMERIC(4)");
            OnPairConfirmedCallBack(addr, number);
            break;
        default:
            HILOGE("Invalid reqType is %{public}d", reqType);
            break;
    }
}

void NapiBluetoothHostObserver::UvQueueWorkOnPairConfirmedCallBack(uv_work_t *work, std::pair<std::string, int> &data)
{
    HILOGI("start");
    if (work == nullptr) {
        HILOGE("work is null");
        return;
    }
    auto callbackData = (AfterWorkCallbackData<NapiBluetoothHostObserver,
        decltype(&NapiBluetoothHostObserver::UvQueueWorkOnPairConfirmedCallBack),
        std::pair<std::string, int>> *)work->data;
    if (callbackData == nullptr) {
        HILOGE("callbackData is null");
        return;
    }

    napi_value result = 0;
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_value callResult = 0;
    napi_get_undefined(callbackData->env, &undefined);
    HILOGI("Addr is %{public}s", GetEncryptAddr(deviceAddr).c_str());
    napi_create_object(callbackData->env, &result);
    napi_value device = 0;
    napi_create_string_utf8(
        callbackData->env, callbackData->data.first.data(), callbackData->data.first.size(), &device);
    napi_set_named_property(callbackData->env, result, "deviceId", device);
    napi_value pinCode = 0;
    napi_create_int32(callbackData->env, callbackData->data.second, &pinCode);
    napi_set_named_property(callbackData->env, result, "pinCode", pinCode);

    napi_get_reference_value(callbackData->env, callbackData->callback, &callback);
    napi_call_function(callbackData->env, undefined, callback, ARGS_SIZE_ONE, &result, &callResult);
}

void NapiBluetoothHostObserver::OnPairConfirmedCallBack(const std::string &deviceAddr, const int number)
{
    HILOGI("start");
    std::shared_ptr<BluetoothCallbackInfo> callbackInfo = GetCallbackInfoByType(REGISTER_PIN_REQUEST_TYPE);
    if (callbackInfo == nullptr) {
        HILOGI("This callback is not registered by ability.");
        return;
    }
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(callbackInfo->env_, &loop);
    if (loop == nullptr) {
        HILOGE("loop instance is nullptr");
        return;
    }

    auto callbackData = new (std::nothrow) AfterWorkCallbackData<NapiBluetoothHostObserver,
        decltype(&NapiBluetoothHostObserver::UvQueueWorkOnPairConfirmedCallBack),
        std::pair<std::string, int>>();
    if (callbackData == nullptr) {
        HILOGE("new callbackData failed");
        return;
    }

    callbackData->object = this;
    callbackData->function = &NapiBluetoothHostObserver::UvQueueWorkOnPairConfirmedCallBack;
    callbackData->env = callbackInfo->env_;
    callbackData->callback = callbackInfo->callback_;
    callbackData->data = std::pair<std::string, int>(deviceAddr, number);

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        HILOGE("new work failed");
        delete callbackData;
        callbackData = nullptr;
        return;
    }

    work->data = (void *)callbackData;

    int ret = uv_queue_work(
        loop, work, [](uv_work_t *work) {}, AfterWorkCallback<decltype(callbackData)>);
    if (ret != 0) {
        delete callbackData;
        callbackData = nullptr;
        delete work;
        work = nullptr;
    }
}
}  // namespace Bluetooth
}  // namespace OHOS