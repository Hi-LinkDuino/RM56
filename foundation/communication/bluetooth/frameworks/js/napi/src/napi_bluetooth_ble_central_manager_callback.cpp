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
#include "napi_bluetooth_ble_central_manager_callback.h"

#include "bluetooth_log.h"
#include "bluetooth_utils.h"
#include "napi_bluetooth_utils.h"
#include "securec.h"

namespace OHOS {
namespace Bluetooth {
namespace {
struct SysBLEStartScanCallbackData {
    napi_env env;
    napi_ref callbackSuccess = nullptr;
    napi_ref callbackFail = nullptr;
    napi_ref callbackComplete = nullptr;
    int32_t resultCode = 0;
};

struct SysBLEDeviceFoundCallbackData {
    napi_env env;
    napi_ref callbackSuccess = nullptr;
    napi_ref callbackFail = nullptr;
    std::shared_ptr<BleScanResult> result;
};

void ConvertScanResult(const std::vector<BleScanResult> &results, const napi_env &env, napi_value &scanResultArray,
    bool isSysInterface = false)
{
    HILOGI("enter");
    napi_create_array(env, &scanResultArray);
    size_t count = 0;
    for (auto bleScanResult : results) {
        napi_value result = nullptr;
        napi_value value = nullptr;
        napi_create_object(env, &result);
        napi_create_string_utf8(
            env, bleScanResult.GetPeripheralDevice().GetDeviceAddr().c_str(), NAPI_AUTO_LENGTH, &value);
        std::string propertyName = "deviceId";
        if (isSysInterface) {
            propertyName = "addr";
        }
        napi_set_named_property(env, result, propertyName.c_str(), value);
        napi_create_int32(env, bleScanResult.GetRssi(), &value);
        napi_set_named_property(env, result, "rssi", value);
        if (isSysInterface) {
            HILOGI("ConvertScanResult isSysInterface");
            napi_create_string_utf8(env, "random", NAPI_AUTO_LENGTH, &value);
            napi_set_named_property(env, result, "addrType", value);
            napi_create_int32(env, 1, &value);
            napi_set_named_property(env, result, "txpower", value);
        }
        uint8_t *native = nullptr;
        napi_value buffer = nullptr;
        napi_create_arraybuffer(env, bleScanResult.GetPayload().size(), reinterpret_cast<void **>(&native), &buffer);
        if (memcpy_s(native, bleScanResult.GetPayload().size(), bleScanResult.GetPayload().data(),
            bleScanResult.GetPayload().size()) != EOK) {
            HILOGE("ConvertScanResult memcpy_s fail");
            return;
        }
        napi_create_typedarray(env, napi_uint8_array, bleScanResult.GetPayload().size(), buffer, 0, &value);
        napi_set_named_property(env, result, "data", value);
        napi_set_element(env, scanResultArray, count, result);
        ++count;
    }
}

void AfterWorkCallbackToSysBLEScan(uv_work_t *work, int status)
{
    if (work == nullptr) {
        return;
    }

    std::unique_ptr<SysBLEStartScanCallbackData> data(static_cast<SysBLEStartScanCallbackData *>(work->data));

    const napi_env &env = data->env;

    napi_value funcComplete = nullptr;
    napi_value callbackResult = nullptr;
    napi_value undefine = nullptr;
    napi_get_undefined(env, &undefine);
    napi_get_reference_value(env, data->callbackComplete, &funcComplete);
    if (data->resultCode == 0) {
        napi_value funcSuccess = nullptr;
        napi_get_reference_value(env, data->callbackSuccess, &funcSuccess);
        napi_call_function(env, undefine, funcSuccess, 0, nullptr, &callbackResult);
    } else {
        napi_value funcFail = nullptr;
        napi_get_reference_value(env, data->callbackFail, &funcFail);
        napi_value callbackValue[] {nullptr, nullptr};
        napi_create_string_utf8(env, "sys startBLEScan fail", NAPI_AUTO_LENGTH, &callbackValue[0]);
        napi_create_int32(env, data->resultCode, &callbackValue[1]);
        napi_call_function(env, undefine, funcFail, ARGS_SIZE_TWO, callbackValue, &callbackResult);
    }
    napi_call_function(env, undefine, funcComplete, 0, nullptr, &callbackResult);
}

void AfterWorkCallbackToSysBLEDeviceFound(uv_work_t *work, int status)
{
    if (work == nullptr) {
        return;
    }

    std::unique_ptr<SysBLEDeviceFoundCallbackData> data(static_cast<SysBLEDeviceFoundCallbackData *>(work->data));
    const napi_env &env = data->env;

    napi_value result = nullptr;
    std::vector<BleScanResult> scanResults;
    scanResults.push_back(*(data->result));
    ConvertScanResult(scanResults, env, result, true);

    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_set_named_property(env, object, "devices", result);
    napi_value callbackResult = nullptr;
    napi_value undefine = nullptr;
    napi_get_undefined(env, &undefine);
    napi_value funcSuccess = nullptr;
    napi_get_reference_value(env, data->callbackSuccess, &funcSuccess);
    napi_call_function(env, undefine, funcSuccess, ARGS_SIZE_ONE, &object, &callbackResult);
}

void SysOnScanCallBack(sysBLEMap &observers, const BleScanResult &result)
{
    auto callbackInfos = observers[REGISTER_SYS_BLE_FIND_DEVICE_TYPE];
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(callbackInfos[PARAM0]->env_, &loop);
    if (loop == nullptr) {
        HILOGE("loop instance is nullptr");
        return;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        HILOGE("create uv_work_t failed!");
        return;
    }

    SysBLEDeviceFoundCallbackData *data = new (std::nothrow) SysBLEDeviceFoundCallbackData();
    if (data == nullptr) {
        HILOGE("create SysBLECallbackData failed!");
        delete work;
        return;
    }

    data->env = callbackInfos[PARAM0]->env_;
    data->callbackSuccess = callbackInfos[PARAM0]->callback_;
    data->callbackFail = callbackInfos[PARAM1]->callback_;
    data->result = std::make_shared<BleScanResult>(result);
    work->data = static_cast<void *>(data);
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, AfterWorkCallbackToSysBLEDeviceFound);
}
} // namespace

void NapiBluetoothBleCentralManagerCallback::UvQueueWorkOnScanCallback(
    uv_work_t *work, std::shared_ptr<BleScanResult> &result)
{
    HILOGI("enter");
    if (work == nullptr) {
        HILOGE("work is null");
        return;
    }
    auto callbackData = (AfterWorkCallbackData<NapiBluetoothBleCentralManagerCallback,
        decltype(&NapiBluetoothBleCentralManagerCallback::UvQueueWorkOnScanCallback),
	std::shared_ptr<BleScanResult>> *)work->data;
    if (callbackData == nullptr) {
        HILOGE("callbackData is null");
        return;
    }
    napi_value napiResult = 0;
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_value callResult = 0;
    napi_get_undefined(callbackData->env, &undefined);
    std::vector<BleScanResult> results;
    results.push_back(*result);
    ConvertScanResult(results, callbackData->env, napiResult);
    napi_get_reference_value(callbackData->env, callbackData->callback, &callback);
    napi_call_function(callbackData->env, undefined, callback, ARGS_SIZE_ONE, &napiResult, &callResult);
}

static void OnSysScanCallback(const BleScanResult &result, const std::string type)
{
    std::lock_guard<std::mutex> lock(g_sysBLEObserverMutex);
        auto sysObservers = GetSysBLEObserver();
        if (sysObservers.find(type) != sysObservers.end()) {
            SysOnScanCallBack(sysObservers, result);
        }
}

void NapiBluetoothBleCentralManagerCallback::OnScanCallback(const BleScanResult &result)
{
    HILOGI("enter, remote device address: %{public}s", GET_ENCRYPT_ADDR(result.GetPeripheralDevice()));
    OnSysScanCallback(result, REGISTER_SYS_BLE_FIND_DEVICE_TYPE);
    std::shared_ptr<BluetoothCallbackInfo> callbackInfo = GetCallbackInfoByType(REGISTER_BLE_FIND_DEVICE_TYPE);
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

    auto callbackData = new (std::nothrow) AfterWorkCallbackData<NapiBluetoothBleCentralManagerCallback,
        decltype(&NapiBluetoothBleCentralManagerCallback::UvQueueWorkOnScanCallback),
        std::shared_ptr<BleScanResult>>();
    if (callbackData == nullptr) {
        HILOGE("new callbackData failed");
        return;
    }

    callbackData->object = this;
    callbackData->function = &NapiBluetoothBleCentralManagerCallback::UvQueueWorkOnScanCallback;
    callbackData->env = callbackInfo->env_;
    callbackData->callback = callbackInfo->callback_;
    callbackData->data = std::make_shared<BleScanResult>(result);

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

void NapiBluetoothBleCentralManagerCallback::UvQueueWorkOnBleBatchScanResultsEvent(
    uv_work_t *work, const std::vector<BleScanResult> &results)
{
    HILOGI("enter");

    if (work == nullptr) {
        HILOGE("work is null");
        return;
    }
    auto callbackData = (AfterWorkCallbackData<NapiBluetoothBleCentralManagerCallback,
        decltype(&NapiBluetoothBleCentralManagerCallback::UvQueueWorkOnBleBatchScanResultsEvent),
        std::vector<BleScanResult>> *)work->data;
    if (callbackData == nullptr) {
        HILOGE("callbackData is null");
        return;
    }

    napi_value result = 0;
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_value callResult = 0;
    napi_get_undefined(callbackData->env, &undefined);
    ConvertScanResult(results, callbackData->env, result);
    napi_get_reference_value(callbackData->env, callbackData->callback, &callback);
    napi_call_function(callbackData->env, undefined, callback, ARGS_SIZE_ONE, &result, &callResult);
}

void NapiBluetoothBleCentralManagerCallback::OnBleBatchScanResultsEvent(const std::vector<BleScanResult> &results)
{
    HILOGI("enter, scan result size: %{public}zu", results.size());
    std::shared_ptr<BluetoothCallbackInfo> callbackInfo = GetCallbackInfoByType(REGISTER_BLE_FIND_DEVICE_TYPE);
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

    auto callbackData = new (std::nothrow) AfterWorkCallbackData<NapiBluetoothBleCentralManagerCallback,
        decltype(&NapiBluetoothBleCentralManagerCallback::UvQueueWorkOnBleBatchScanResultsEvent),
        std::vector<BleScanResult>>();
    if (callbackData == nullptr) {
        HILOGE("new callbackData failed");
        return;
    }

    callbackData->object = this;
    callbackData->function = &NapiBluetoothBleCentralManagerCallback::UvQueueWorkOnBleBatchScanResultsEvent;
    callbackData->env = callbackInfo->env_;
    callbackData->callback = callbackInfo->callback_;
    callbackData->data = results;

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

void NapiBluetoothBleCentralManagerCallback::OnStartOrStopScanEvent(int resultCode, bool isStartScan)
{
    HILOGI("resultCode: %{public}d, isStartScan: %{public}d", resultCode, isStartScan);
    
    std::array<std::shared_ptr<BluetoothCallbackInfo>, ARGS_SIZE_THREE> callbackInfos;
    {
        std::lock_guard<std::mutex> lock(g_sysBLEObserverMutex);
        auto observers = GetSysBLEObserver();
        if (observers.find(REGISTER_SYS_BLE_SCAN_TYPE) == observers.end()) {
            HILOGE("sys BEL callback is not registered by ability.");
            return;
        }
        callbackInfos = observers[REGISTER_SYS_BLE_SCAN_TYPE];
    }
    
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(callbackInfos[PARAM0]->env_, &loop);
    if (loop == nullptr) {
        HILOGE("loop instance is nullptr");
        return;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        HILOGE("create uv_work_t failed!");
        return;
    }

    SysBLEStartScanCallbackData *data = new (std::nothrow) SysBLEStartScanCallbackData();
    if (data == nullptr) {
        HILOGE("create SysBLECallbackData failed!");
        delete work;
        return;
    }

    data->resultCode = resultCode;
    data->env = callbackInfos[PARAM0]->env_;
    data->callbackSuccess = callbackInfos[PARAM0]->callback_;
    data->callbackFail = callbackInfos[PARAM1]->callback_;
    data->callbackComplete = callbackInfos[PARAM2]->callback_;
    work->data = static_cast<void *>(data);
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, AfterWorkCallbackToSysBLEScan);
}
}  // namespace Bluetooth
}  // namespace OHOS
