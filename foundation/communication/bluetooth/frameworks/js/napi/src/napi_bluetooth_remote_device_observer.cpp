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
#include <map>
#include <memory>
#include <string>

#include "bluetooth_log.h"
#include "bluetooth_utils.h"
#include "napi_bluetooth_remote_device_observer.h"
#include "napi_bluetooth_utils.h"

namespace OHOS {
namespace Bluetooth {
void NapiBluetoothRemoteDeviceObserver::UvQueueWorkOnPairStatusChanged(
    uv_work_t *work, std::pair<std::string, int> &data)
{
    HILOGI("uv_work_t start");
    if (work == nullptr) {
        HILOGE("work is null");
        return;
    }
    auto callbackData = (AfterWorkCallbackData<NapiBluetoothRemoteDeviceObserver,
        decltype(&NapiBluetoothRemoteDeviceObserver::UvQueueWorkOnPairStatusChanged),
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
    HILOGI("Status is %{public}d", callbackData->data.second);
    napi_create_object(callbackData->env, &result);
    napi_value device = 0;
    napi_create_string_utf8(
        callbackData->env, callbackData->data.first.data(), callbackData->data.first.size(), &device);
    napi_set_named_property(callbackData->env, result, "deviceId", device);
    napi_value bondState = 0;
    napi_create_int32(callbackData->env, callbackData->data.second, &bondState);
    napi_set_named_property(callbackData->env, result, "state", bondState);
    napi_get_reference_value(callbackData->env, callbackData->callback, &callback);
    napi_call_function(callbackData->env, undefined, callback, ARGS_SIZE_ONE, &result, &callResult);
}

void NapiBluetoothRemoteDeviceObserver::OnPairStatusChanged(const BluetoothRemoteDevice &device, int status)
{
    std::shared_ptr<BluetoothCallbackInfo> callbackInfo = GetCallbackInfoByType(REGISTER_BONE_STATE_TYPE);
    if (callbackInfo == nullptr) {
        HILOGI("This callback is not registered by ability.");
        return;
    }
    HILOGI("addr:%{public}s, status:%{public}d", GET_ENCRYPT_ADDR(device), status);
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(callbackInfo->env_, &loop);
    if (loop == nullptr) {
        HILOGE("loop instance is nullptr");
        return;
    }

    int bondStatus = 0;
    DealPairStatus(status, bondStatus);

    auto callbackData = new (std::nothrow) AfterWorkCallbackData<NapiBluetoothRemoteDeviceObserver,
        decltype(&NapiBluetoothRemoteDeviceObserver::UvQueueWorkOnPairStatusChanged), std::pair<std::string, int>>();
    if (callbackData == nullptr) {
        HILOGE("new callbackData failed");
        return;
    }

    std::string deviceAddr = device.GetDeviceAddr();
    callbackData->object = this;
    callbackData->function = &NapiBluetoothRemoteDeviceObserver::UvQueueWorkOnPairStatusChanged;
    callbackData->env = callbackInfo->env_;
    callbackData->callback = callbackInfo->callback_;
    callbackData->data = std::pair<std::string, int>(deviceAddr, bondStatus);

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

void NapiBluetoothRemoteDeviceObserver ::OnRemoteUuidChanged(
    const BluetoothRemoteDevice &device, const std::vector<ParcelUuid> &uuids)
{
    HILOGI("called");
}

void NapiBluetoothRemoteDeviceObserver ::OnRemoteNameChanged(
    const BluetoothRemoteDevice &device, const std::string &deviceName)
{
    HILOGI("addr:%{public}s, deviceName:%{public}s", GET_ENCRYPT_ADDR(device), deviceName.c_str());
}

void NapiBluetoothRemoteDeviceObserver ::OnRemoteAliasChanged(
    const BluetoothRemoteDevice &device, const std::string &alias)
{
    HILOGI("addr:%{public}s, alias:%{public}s", GET_ENCRYPT_ADDR(device), alias.c_str());
}

void NapiBluetoothRemoteDeviceObserver ::OnRemoteCodChanged(
    const BluetoothRemoteDevice &device, const BluetoothDeviceClass &cod)
{
    HILOGI("addr:%{public}s, cod:%{public}d", GET_ENCRYPT_ADDR(device), cod.GetClassOfDevice());
}

void NapiBluetoothRemoteDeviceObserver ::OnRemoteBatteryLevelChanged(
    const BluetoothRemoteDevice &device, int batteryLevel)
{
    HILOGI("addr:%{public}s, batteryLevel:%{public}d", GET_ENCRYPT_ADDR(device), batteryLevel);
}

void NapiBluetoothRemoteDeviceObserver ::OnReadRemoteRssiEvent(
    const BluetoothRemoteDevice &device, int rssi, int status)
{
    HILOGI("addr:%{public}s, rssi:%{public}d, status is %{public}d",
        GET_ENCRYPT_ADDR(device), rssi, status);
    std::shared_ptr<GattGetRssiValueCallbackInfo> callbackInfo = GetRssiValueCallbackInfo();
    if (callbackInfo == nullptr) {
        return;
    }
    std::unique_lock<std::mutex> lock(callbackInfo->mutexRssi);
    if (status == 0) {
        callbackInfo->promise.errorCode = CODE_SUCCESS;
        callbackInfo->rssi = rssi;
    } else {
        callbackInfo->promise.errorCode = CODE_FAILED;
    }
    callbackInfo->cvfull.notify_all();
}

void NapiBluetoothRemoteDeviceObserver::DealPairStatus(const int &status, int &bondStatus)
{
    HILOGI("status is %{public}d", status);
    switch (status) {
        case PAIR_NONE:
            bondStatus = BondState::BOND_STATE_INVALID;
            break;
        case PAIR_PAIRING:
            bondStatus = BondState::BOND_STATE_BONDING;
            break;
        case PAIR_PAIRED:
            bondStatus = BondState::BOND_STATE_BONDED;
            break;
        default:
            break;
    }
}
}  // namespace Bluetooth
}  // namespace OHOS