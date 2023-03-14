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

#include "napi_bluetooth_host.h"
#include "bluetooth_host.h"
#include "bluetooth_log.h"
#include "napi_bluetooth_host_observer.h"
#include "napi_bluetooth_remote_device_observer.h"
#include "napi_bluetooth_utils.h"

namespace OHOS {
namespace Bluetooth {
namespace {
NapiBluetoothHostObserver g_bluetoothHostObserver;
NapiBluetoothRemoteDeviceObserver g_bluetoothRemoteDevice;
std::string g_RemoteDeviceAddr;
}  // namespace

napi_value BluetoothHostInit(napi_env env, napi_value exports)
{
    HILOGI("start");
    RegisterObserverToHost();
    PropertyValueInit(env, exports);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getState", GetState),
        DECLARE_NAPI_FUNCTION("getBtConnectionState", GetBtConnectionState),
        DECLARE_NAPI_FUNCTION("pairDevice", PairDevice),
        DECLARE_NAPI_FUNCTION("cancelPairedDevice", CancelPairedDevice),
        DECLARE_NAPI_FUNCTION("getRemoteDeviceName", GetRemoteDeviceName),
        DECLARE_NAPI_FUNCTION("getRemoteDeviceClass", GetRemoteDeviceClass),
        DECLARE_NAPI_FUNCTION("enableBluetooth", EnableBluetooth),
        DECLARE_NAPI_FUNCTION("disableBluetooth", DisableBluetooth),
        DECLARE_NAPI_FUNCTION("getLocalName", GetLocalName),
        DECLARE_NAPI_FUNCTION("getPairedDevices", GetPairedDevices),
        DECLARE_NAPI_FUNCTION("getProfileConnState", GetProfileConnState),
        DECLARE_NAPI_FUNCTION("setDevicePairingConfirmation", SetDevicePairingConfirmation),
        DECLARE_NAPI_FUNCTION("setLocalName", SetLocalName),
        DECLARE_NAPI_FUNCTION("setBluetoothScanMode", SetBluetoothScanMode),
        DECLARE_NAPI_FUNCTION("getBluetoothScanMode", GetBluetoothScanMode),
        DECLARE_NAPI_FUNCTION("startBluetoothDiscovery", StartBluetoothDiscovery),
        DECLARE_NAPI_FUNCTION("stopBluetoothDiscovery", StopBluetoothDiscovery),
        DECLARE_NAPI_FUNCTION("on", RegisterObserver),
        DECLARE_NAPI_FUNCTION("off", DeregisterObserver),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    HILOGI("end");
    return exports;
}

void RegisterObserverToHost()
{
    HILOGI("start");
    BluetoothHost *host = &BluetoothHost::GetDefaultHost();
    host->RegisterObserver(g_bluetoothHostObserver);
    host->RegisterRemoteDeviceObserver(g_bluetoothRemoteDevice);
}

napi_value EnableBluetooth(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    BluetoothHost *host = &BluetoothHost::GetDefaultHost();
    bool enabled = host->EnableBle();
    if (enabled) {
        SetCurrentAppOperate(true);
    }
    napi_value result = nullptr;
    napi_get_boolean(env, enabled, &result);
    HILOGI("end");
    return result;
}

napi_value DisableBluetooth(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    BluetoothHost *host = &BluetoothHost::GetDefaultHost();
    bool enabled = false;
    // only ble
    if (host->GetBtState() == BTStateID::STATE_TURN_OFF) {
        enabled = host->DisableBle();
    } else {
        enabled = host->DisableBt();
    }

    if (enabled) {
        SetCurrentAppOperate(true);
    }
    napi_value result = nullptr;
    napi_get_boolean(env, enabled, &result);
    HILOGI("end");
    return result;
}

napi_value SetLocalName(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc == 0) {
        HILOGE("The argc is 0");
        return NapiGetBooleanFalse(env);
    }
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, argv[PARAM0], nullptr, 0, &bufferSize);
    HILOGI("local name writeChar bufferSize = %{public}d", (int)bufferSize);
    if (bufferSize == 0) {
        return NapiGetBooleanFalse(env);
    }
    char buffer[bufferSize + 1];
    size_t strLength = 0;
    napi_get_value_string_utf8(env, argv[PARAM0], buffer, bufferSize + 1, &strLength);
    std::string localName(buffer);

    BluetoothHost *host = &BluetoothHost::GetDefaultHost();
    bool enabled = host->SetLocalName(localName);
    napi_value result = nullptr;
    napi_get_boolean(env, enabled, &result);
    HILOGI("end");
    return result;
}

napi_value GetLocalName(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    HILOGI("start");
    BluetoothHost *host = &BluetoothHost::GetDefaultHost();
    std::string localName = host->GetLocalName();
    napi_create_string_utf8(env, localName.c_str(), localName.size(), &result);
    HILOGI("end");
    return result;
}

napi_value GetRemoteDeviceName(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc == 0) {
        return NapiGetNull(env);
    }
    napi_value result = nullptr;
    ParseString(env, g_RemoteDeviceAddr, argv[PARAM0]);
    std::string name =
        BluetoothHost::GetDefaultHost().GetRemoteDevice(g_RemoteDeviceAddr, BT_TRANSPORT_BREDR).GetDeviceName();
    napi_create_string_utf8(env, name.c_str(), name.size(), &result);
    HILOGI("end");
    return result;
}

napi_value GetRemoteDeviceClass(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc == 0) {
        return NapiGetNull(env);
    }
    ParseString(env, g_RemoteDeviceAddr, argv[PARAM0]);
    BluetoothDeviceClass deviceClass =
        BluetoothHost::GetDefaultHost().GetRemoteDevice(g_RemoteDeviceAddr, BT_TRANSPORT_BREDR).GetDeviceClass();
    int tmpCod = deviceClass.GetClassOfDevice();
    int tmpMajorClass = deviceClass.GetMajorClass();
    int tmpMajorMinorClass = deviceClass.GetMajorMinorClass();
    if (tmpCod == 0) {
        HILOGI("cod = %{public}d", tmpCod);
        tmpCod = MajorClass::MAJOR_UNCATEGORIZED;
        tmpMajorClass = MajorClass::MAJOR_UNCATEGORIZED;
        tmpMajorMinorClass = MajorClass::MAJOR_UNCATEGORIZED;
    }
    HILOGI("cod = %{public}d, majorClass = %{public}d, majorMinorClass = %{public}d",
        tmpCod, tmpMajorClass, tmpMajorMinorClass);
    napi_value result = nullptr;
    napi_create_object(env, &result);
    napi_value majorClass = 0;
    napi_create_int32(env, tmpMajorClass, &majorClass);
    napi_set_named_property(env, result, "majorClass", majorClass);
    napi_value majorMinorClass = 0;
    napi_create_int32(env, tmpMajorMinorClass, &majorMinorClass);
    napi_set_named_property(env, result, "majorMinorClass", majorMinorClass);
    napi_value cod = 0;
    napi_create_int32(env, tmpCod, &cod);
    napi_set_named_property(env, result, "classOfDevice", cod);
    HILOGI("end");
    return result;
}

napi_value SetBluetoothScanMode(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc == 0) {
        return NapiGetNull(env);
    }
    napi_valuetype valuetype = napi_undefined;

    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
    int32_t mode = 0;
    napi_get_value_int32(env, argv[PARAM0], &mode);
    HILOGI("mode = %{public}d", mode);
    NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
    int32_t duration = 0;
    napi_get_value_int32(env, argv[PARAM1], &duration);
    HILOGI("duration = %{public}d", duration);

    BluetoothHost *host = &BluetoothHost::GetDefaultHost();
    bool enabled = host->SetBtScanMode(mode, duration);
    host->SetBondableMode(BT_TRANSPORT_BREDR, 1);
    napi_value result = nullptr;
    napi_get_boolean(env, enabled, &result);
    HILOGI("end");
    return result;
}

napi_value GetBluetoothScanMode(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    BluetoothHost *host = &BluetoothHost::GetDefaultHost();
    int32_t scanMode = host->GetBtScanMode();
    napi_value result = nullptr;
    napi_create_uint32(env, scanMode, &result);
    HILOGI("end");
    return result;
}

napi_value StartBluetoothDiscovery(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    BluetoothHost *host = &BluetoothHost::GetDefaultHost();
    bool isStart = host->StartBtDiscovery();
    napi_value result = nullptr;
    napi_get_boolean(env, isStart, &result);
    HILOGI("end");
    return result;
}

napi_value StopBluetoothDiscovery(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    BluetoothHost *host = &BluetoothHost::GetDefaultHost();
    bool isStart = host->CancelBtDiscovery();
    napi_value result = nullptr;
    napi_get_boolean(env, isStart, &result);
    HILOGI("end");
    return result;
}

napi_value GetState(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    BluetoothHost *host = &BluetoothHost::GetDefaultHost();
    int32_t state = host->GetBtState();
    int32_t status = static_cast<int32_t>(BluetoothState::STATE_OFF);
    switch (state) {
        case BTStateID::STATE_TURNING_ON:
            HILOGI("STATE_TURNING_ON(1)");
            status = static_cast<int32_t>(BluetoothState::STATE_TURNING_ON);
            break;
        case BTStateID::STATE_TURN_ON:
            HILOGI("STATE_ON(2)");
            status = static_cast<int32_t>(BluetoothState::STATE_ON);
            break;
        case BTStateID::STATE_TURNING_OFF:
            HILOGI("STATE_TURNING_OFF(3)");
            status = static_cast<int32_t>(BluetoothState::STATE_TURNING_OFF);
            break;
        case BTStateID::STATE_TURN_OFF:
            HILOGI("STATE_OFF(0)");
            status = static_cast<int32_t>(BluetoothState::STATE_OFF);
            break;
        default:
            HILOGE("get state failed");
            break;
    }

    bool enableBle = host->IsBleEnabled();
    if (enableBle && (state == BTStateID::STATE_TURN_OFF)) {
        HILOGI("BR off and BLE on, STATE_BLE_ON(5)");
        status = static_cast<int32_t>(BluetoothState::STATE_BLE_ON);
    } else if (!enableBle && (state == BTStateID::STATE_TURN_OFF)) {
        status = static_cast<int32_t>(BluetoothState::STATE_OFF);
    }

    napi_value result = nullptr;
    napi_create_int32(env, status, &result);
    return result;
}

napi_value GetBtConnectionState(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    BluetoothHost *host = &BluetoothHost::GetDefaultHost();
    int32_t state = host->GetBtConnectionState();
    HILOGI("start state %{public}d", state);
    napi_value result = nullptr;
    napi_create_int32(env, GetProfileConnectionState(state), &result);
    HILOGI("end");
    return result;
}

napi_value PairDevice(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc == 0) {
        return NapiGetNull(env);
    }
    ParseString(env, g_RemoteDeviceAddr, argv[PARAM0]);
    BluetoothRemoteDevice remoteDevice = BluetoothRemoteDevice(g_RemoteDeviceAddr, BT_TRANSPORT_BREDR);
    bool isSuccess = remoteDevice.StartPair();
    napi_value result = nullptr;
    napi_get_boolean(env, isSuccess, &result);
    HILOGI("end");
    return result;
}

napi_value CancelPairedDevice(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc == 0) {
        return NapiGetNull(env);
    }
    ParseString(env, g_RemoteDeviceAddr, argv[PARAM0]);
    BluetoothRemoteDevice remoteDevice = BluetoothRemoteDevice(g_RemoteDeviceAddr, BT_TRANSPORT_BREDR);
    BluetoothHost *host = &BluetoothHost::GetDefaultHost();
    bool isSuccess = host->RemovePair(remoteDevice);
    napi_value result = nullptr;
    napi_get_boolean(env, isSuccess, &result);
    HILOGI("end");
    return result;
}

napi_value GetPairedDevices(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    BluetoothHost *host = &BluetoothHost::GetDefaultHost();
    std::vector<BluetoothRemoteDevice> remoteDeviceLists = host->GetPairedDevices(BT_TRANSPORT_BREDR);
    napi_value result = nullptr;
    int count = 0;
    napi_create_array(env, &result);
    for (auto vec : remoteDeviceLists) {
        napi_value remoteDeviceResult;
        napi_create_string_utf8(env, vec.GetDeviceAddr().c_str(), vec.GetDeviceAddr().size(), &remoteDeviceResult);
        napi_set_element(env, result, count, remoteDeviceResult);
        count++;
    }
    HILOGI("end");
    return result;
}

napi_value SetDevicePairingConfirmation(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc == 0) {
        return NapiGetNull(env);
    }
    ParseString(env, g_RemoteDeviceAddr, argv[PARAM0]);
    napi_valuetype valuetype = napi_undefined;

    NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Boolean expected.");
    bool accept = false;
    napi_get_value_bool(env, argv[PARAM1], &accept);
    HILOGI("SetDevicePairingConfirmation::accept = %{public}d", accept);
    bool isSuccess = BluetoothHost::GetDefaultHost()
        .GetRemoteDevice(g_RemoteDeviceAddr, BT_TRANSPORT_BREDR)
        .SetDevicePairingConfirmation(accept);
    napi_value result = nullptr;
    napi_get_boolean(env, isSuccess, &result);
    HILOGI("end");
    return result;
}

static void SetCallback(const napi_env &env, const napi_ref &callbackIn, const int &errorCode, const napi_value &result)
{
    HILOGI("enter");
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);

    napi_value callback = nullptr;
    napi_value resultout = nullptr;
    napi_get_reference_value(env, callbackIn, &callback);
    napi_value results[ARGS_SIZE_TWO] = {nullptr};
    results[PARAM0] = GetCallbackErrorValue(env, errorCode);
    results[PARAM1] = result;
    NAPI_CALL_RETURN_VOID(
        env, napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, &results[PARAM0], &resultout));
    HILOGI("end");
}

static void SetPromise(const napi_env &env, const napi_deferred &deferred, const napi_value &result)
{
    HILOGI("enter");
    napi_resolve_deferred(env, deferred, result);
    HILOGI("end");
}

static void PaddingCallbackPromiseInfo(
    const napi_env &env, const napi_ref &callback, CallbackPromiseInfo &info, napi_value &promise)
{
    HILOGI("enter");
    if (callback) {
        info.callback = callback;
        info.isCallback = true;
    } else {
        napi_deferred deferred = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_promise(env, &deferred, &promise));
        info.deferred = deferred;
        info.isCallback = false;
    }
    HILOGI("end");
}

static void ReturnCallbackPromise(const napi_env &env, const CallbackPromiseInfo &info, const napi_value &result)
{
    HILOGI("enter");
    if (info.isCallback) {
        SetCallback(env, info.callback, info.errorCode, result);
    } else {
        SetPromise(env, info.deferred, result);
    }
    HILOGI("end");
}

static napi_value JSParaError(const napi_env &env, const napi_ref &callback)
{
    if (callback) {
        return NapiGetNull(env);
    } else {
        napi_value promise = nullptr;
        napi_deferred deferred = nullptr;
        napi_create_promise(env, &deferred, &promise);
        SetPromise(env, deferred, NapiGetNull(env));
        return promise;
    }
}

static napi_value GetCallback(const napi_env &env, const napi_value &value, CallbackPromiseInfo &params)
{
    HILOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
    napi_create_reference(env, value, 1, &params.callback);
    HILOGI("end");
    return NapiGetNull(env);
}

static napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, CallbackPromiseInfo &params)
{
    HILOGI("enter");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argv[PARAM0] != nullptr) {
        GetCallback(env, argv[PARAM0], params);
    }
    HILOGI("end");
    return NapiGetNull(env);
}

static void GetDeviceNameSyncWorkStart(GattGetDeviceNameCallbackInfo *asynccallbackinfo)
{
    HILOGI("enter");
    std::string deviceId = GetGattClientDeviceId();
    asynccallbackinfo->deviceId =
        BluetoothHost::GetDefaultHost().GetRemoteDevice(deviceId, BT_TRANSPORT_BLE).GetDeviceName();
    if (asynccallbackinfo->deviceId.empty()) {
        asynccallbackinfo->promise.errorCode = CODE_FAILED;
    } else {
        asynccallbackinfo->promise.errorCode = CODE_SUCCESS;
    }
}

void AsyncCompleteCallbackGetDeviceName(napi_env env, napi_status status, void *data)
{
    HILOGI("napi_create_async_work complete start");
    if (!data) {
        HILOGE("Invalid async callback data");
        return;
    }
    GattGetDeviceNameCallbackInfo *asynccallbackinfo = (GattGetDeviceNameCallbackInfo *)data;
    napi_value result = nullptr;
    if (asynccallbackinfo->promise.errorCode != CODE_SUCCESS) {
        HILOGI("failed.");
        result = NapiGetNull(env);
    } else {
        HILOGI("success.");
        napi_create_string_utf8(env, asynccallbackinfo->deviceId.c_str(), asynccallbackinfo->deviceId.size(), &result);
    }
    ReturnCallbackPromise(env, asynccallbackinfo->promise, result);
    if (asynccallbackinfo->promise.callback != nullptr) {
        napi_delete_reference(env, asynccallbackinfo->promise.callback);
    }
    napi_delete_async_work(env, asynccallbackinfo->asyncWork);
    delete asynccallbackinfo;
    asynccallbackinfo = nullptr;
    HILOGI("napi_create_async_work complete end");
}

napi_value GetDeviceName(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    napi_ref callback = nullptr;
    GattGetDeviceNameCallbackInfo *asynccallbackinfo =
        new (std::nothrow) GattGetDeviceNameCallbackInfo {.env = env, .asyncWork = nullptr};
    if (!asynccallbackinfo) {
        return JSParaError(env, callback);
    }
    ParseParameters(env, info, asynccallbackinfo->promise);
    napi_value promise = nullptr;
    PaddingCallbackPromiseInfo(env, asynccallbackinfo->promise.callback, asynccallbackinfo->promise, promise);
    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "getDeviceName", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            HILOGI("napi_create_async_work start");
            GattGetDeviceNameCallbackInfo *asynccallbackinfo = (GattGetDeviceNameCallbackInfo *)data;
            GetDeviceNameSyncWorkStart(asynccallbackinfo);
        },
        AsyncCompleteCallbackGetDeviceName,
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);
    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));
    if (asynccallbackinfo->promise.isCallback) {
        return NapiGetNull(env);
    } else {
        return promise;
    }
    HILOGI("end");
}

static void GetRssiValueSyncWorkStart(GattGetRssiValueCallbackInfo *asynccallbackinfo)
{
    HILOGI("start");
    std::string deviceId = GetGattClientDeviceId();
    bool isResult = BluetoothHost::GetDefaultHost().GetRemoteDevice(deviceId, BT_TRANSPORT_BLE).ReadRemoteRssiValue();
    if (!isResult) {
        asynccallbackinfo->promise.errorCode = CODE_FAILED;
    } else {
        std::unique_lock<std::mutex> lock(asynccallbackinfo->mutexRssi);
        std::shared_ptr<GattGetRssiValueCallbackInfo> callbackInfo(asynccallbackinfo,
            [](GattGetRssiValueCallbackInfo *) {});
        SetRssiValueCallbackInfo(callbackInfo);
        if (asynccallbackinfo->cvfull.wait_for(lock, std::chrono::seconds(THREAD_WAIT_TIMEOUT)) ==
            std::cv_status::timeout) {
            HILOGI("ReadRemoteRssi timeout!");
        }
    }
}

void AsyncCompleteCallbackGetRssiValue(napi_env env, napi_status status, void *data)
{
    HILOGI("napi_create_async_work complete start");
    if (!data) {
        HILOGE("Invalid async callback data");
        return;
    }
    GattGetRssiValueCallbackInfo *asynccallbackinfo = (GattGetRssiValueCallbackInfo *)data;
    napi_value result = nullptr;
    if (asynccallbackinfo->promise.errorCode != CODE_SUCCESS) {
        HILOGE("failed.");
        result = NapiGetNull(env);
    } else {
        HILOGI("success.");
        napi_create_int32(env, asynccallbackinfo->rssi, &result);
    }

    ReturnCallbackPromise(env, asynccallbackinfo->promise, result);
    if (asynccallbackinfo->promise.callback != nullptr) {
        napi_delete_reference(env, asynccallbackinfo->promise.callback);
    }

    napi_delete_async_work(env, asynccallbackinfo->asyncWork);

    delete asynccallbackinfo;
    asynccallbackinfo = nullptr;
    std::shared_ptr<GattGetRssiValueCallbackInfo> callbackInfo = nullptr;
    SetRssiValueCallbackInfo(callbackInfo);
    HILOGI("napi_create_async_work complete end");
}

napi_value GetRssiValue(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    napi_ref callback = nullptr;
    GattGetRssiValueCallbackInfo *asynccallbackinfo =
        new (std::nothrow) GattGetRssiValueCallbackInfo {.env = env, .asyncWork = nullptr};
    if (!asynccallbackinfo) {
        return JSParaError(env, callback);
    }
    ParseParameters(env, info, asynccallbackinfo->promise);
    napi_value promise = nullptr;
    PaddingCallbackPromiseInfo(env, asynccallbackinfo->promise.callback, asynccallbackinfo->promise, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "getRssiValue", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            HILOGI("napi_create_async_work start");
            GattGetRssiValueCallbackInfo *asynccallbackinfo = (GattGetRssiValueCallbackInfo *)data;
            GetRssiValueSyncWorkStart(asynccallbackinfo);
        },
        AsyncCompleteCallbackGetRssiValue,
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);
    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));
    if (asynccallbackinfo->promise.isCallback) {
        return NapiGetNull(env);
    } else {
        return promise;
    }
    HILOGI("end");
}

napi_value PropertyValueInit(napi_env env, napi_value exports)
{
    HILOGI("start");
    napi_value scanDutyObject = ScanDutyInit(env);
    napi_value matchModeObject = MatchModeInit(env);
    napi_value stateObj = StateChangeInit(env);
    napi_value profileStateObj = ProfileStateInit(env);
    napi_value scanModeObj = ScanModeInit(env);
    napi_value bondStateObj = BondStateInit(env);
    napi_value majorClassObj = MajorClassOfDeviceInit(env);
    napi_value majorMinorClassObj = MajorMinorClassOfDeviceInit(env);
    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("ScanDuty", scanDutyObject),
        DECLARE_NAPI_PROPERTY("MatchMode", matchModeObject),
        DECLARE_NAPI_PROPERTY("BluetoothState", stateObj),
        DECLARE_NAPI_PROPERTY("ProfileConnectionState", profileStateObj),
        DECLARE_NAPI_PROPERTY("ScanMode", scanModeObj),
        DECLARE_NAPI_PROPERTY("BondState", bondStateObj),
        DECLARE_NAPI_PROPERTY("MajorClass", majorClassObj),
        DECLARE_NAPI_PROPERTY("MajorMinorClass", majorMinorClassObj),
    };
    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    HILOGI("end");
    return exports;
}

napi_value ScanDutyInit(napi_env env)
{
    HILOGI("enter");
    napi_value scanDuty = nullptr;
    napi_create_object(env, &scanDuty);
    SetNamedPropertyByInteger(env, scanDuty, static_cast<int>(ScanDuty::SCAN_MODE_LOW_POWER), "SCAN_MODE_LOW_POWER");
    SetNamedPropertyByInteger(env, scanDuty, static_cast<int>(ScanDuty::SCAN_MODE_BALANCED), "SCAN_MODE_BALANCED");
    SetNamedPropertyByInteger(env, scanDuty, static_cast<int>(ScanDuty::SCAN_MODE_LOW_LATENCY),
        "SCAN_MODE_LOW_LATENCY");
    return scanDuty;
}

napi_value MatchModeInit(napi_env env)
{
    HILOGI("enter");
    napi_value matchMode = nullptr;
    napi_create_object(env, &matchMode);
    SetNamedPropertyByInteger(env, matchMode, static_cast<int>(MatchMode::MATCH_MODE_AGGRESSIVE),
        "MATCH_MODE_AGGRESSIVE");
    SetNamedPropertyByInteger(env, matchMode, static_cast<int>(MatchMode::MATCH_MODE_STICKY), "MATCH_MODE_STICKY");
    return matchMode;
}

napi_value StateChangeInit(napi_env env)
{
    HILOGI("enter");
    napi_value state = nullptr;
    napi_create_object(env, &state);
    SetNamedPropertyByInteger(env, state, static_cast<int>(BluetoothState::STATE_OFF), "STATE_OFF");
    SetNamedPropertyByInteger(env, state, static_cast<int>(BluetoothState::STATE_TURNING_ON), "STATE_TURNING_ON");
    SetNamedPropertyByInteger(env, state, static_cast<int>(BluetoothState::STATE_ON), "STATE_ON");
    SetNamedPropertyByInteger(env, state, static_cast<int>(BluetoothState::STATE_TURNING_OFF), "STATE_TURNING_OFF");
    SetNamedPropertyByInteger(
        env, state, static_cast<int>(BluetoothState::STATE_BLE_TURNING_ON), "STATE_BLE_TURNING_ON");
    SetNamedPropertyByInteger(env, state, static_cast<int>(BluetoothState::STATE_BLE_ON), "STATE_BLE_ON");
    SetNamedPropertyByInteger(
        env, state, static_cast<int>(BluetoothState::STATE_BLE_TURNING_OFF), "STATE_BLE_TURNING_OFF");
    return state;
}

napi_value ProfileStateInit(napi_env env)
{
    HILOGI("enter");
    napi_value profileState = nullptr;
    napi_create_object(env, &profileState);
    SetNamedPropertyByInteger(env, profileState, ProfileConnectionState::STATE_DISCONNECTED, "STATE_DISCONNECTED");
    SetNamedPropertyByInteger(env, profileState, ProfileConnectionState::STATE_CONNECTING, "STATE_CONNECTING");
    SetNamedPropertyByInteger(env, profileState, ProfileConnectionState::STATE_CONNECTED, "STATE_CONNECTED");
    SetNamedPropertyByInteger(env, profileState, ProfileConnectionState::STATE_DISCONNECTING, "STATE_DISCONNECTING");
    return profileState;
}

napi_value ScanModeInit(napi_env env)
{
    HILOGI("enter");
    napi_value scanMode = nullptr;
    napi_create_object(env, &scanMode);
    SetNamedPropertyByInteger(env, scanMode, static_cast<int>(ScanMode::SCAN_MODE_NONE), "SCAN_MODE_NONE");
    SetNamedPropertyByInteger(
        env, scanMode, static_cast<int>(ScanMode::SCAN_MODE_CONNECTABLE), "SCAN_MODE_CONNECTABLE");
    SetNamedPropertyByInteger(
        env, scanMode, static_cast<int>(ScanMode::SCAN_MODE_GENERAL_DISCOVERABLE), "SCAN_MODE_GENERAL_DISCOVERABLE");
    SetNamedPropertyByInteger(env,
        scanMode,
        static_cast<int>(ScanMode::SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE),
        "SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE");
    SetNamedPropertyByInteger(env,
        scanMode,
        static_cast<int>(ScanMode::SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE),
        "SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE");
    return scanMode;
}

napi_value BondStateInit(napi_env env)
{
    HILOGI("enter");
    napi_value bondState = nullptr;
    napi_create_object(env, &bondState);
    SetNamedPropertyByInteger(env, bondState, BondState::BOND_STATE_INVALID, "BOND_STATE_INVALID");
    SetNamedPropertyByInteger(env, bondState, BondState::BOND_STATE_BONDING, "BOND_STATE_BONDING");
    SetNamedPropertyByInteger(env, bondState, BondState::BOND_STATE_BONDED, "BOND_STATE_BONDED");
    return bondState;
}

napi_value MajorClassOfDeviceInit(napi_env env)
{
    HILOGI("enter");
    napi_value majorClass = nullptr;
    napi_create_object(env, &majorClass);
    // MajorClass
    SetNamedPropertyByInteger(env, majorClass, MajorClass::MAJOR_MISC, "MAJOR_MISC");
    SetNamedPropertyByInteger(env, majorClass, MajorClass::MAJOR_COMPUTER, "MAJOR_COMPUTER");
    SetNamedPropertyByInteger(env, majorClass, MajorClass::MAJOR_PHONE, "MAJOR_PHONE");
    SetNamedPropertyByInteger(env, majorClass, MajorClass::MAJOR_NETWORKING, "MAJOR_NETWORKING");
    SetNamedPropertyByInteger(env, majorClass, MajorClass::MAJOR_AUDIO_VIDEO, "MAJOR_AUDIO_VIDEO");
    SetNamedPropertyByInteger(env, majorClass, MajorClass::MAJOR_PERIPHERAL, "MAJOR_PERIPHERAL");
    SetNamedPropertyByInteger(env, majorClass, MajorClass::MAJOR_IMAGING, "MAJOR_IMAGING");
    SetNamedPropertyByInteger(env, majorClass, MajorClass::MAJOR_WEARABLE, "MAJOR_WEARABLE");
    SetNamedPropertyByInteger(env, majorClass, MajorClass::MAJOR_TOY, "MAJOR_TOY");
    SetNamedPropertyByInteger(env, majorClass, MajorClass::MAJOR_HEALTH, "MAJOR_HEALTH");
    SetNamedPropertyByInteger(env, majorClass, MajorClass::MAJOR_UNCATEGORIZED, "MAJOR_UNCATEGORIZED");
    return majorClass;
}

napi_value MajorMinorClassOfDeviceInit(napi_env env)
{
    HILOGI("enter");
    napi_value majorMinorClass = nullptr;
    napi_create_object(env, &majorMinorClass);
    // MajorMinorClass
    // Computer Major Class
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::COMPUTER_UNCATEGORIZED), "COMPUTER_UNCATEGORIZED");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::COMPUTER_DESKTOP), "COMPUTER_DESKTOP");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::COMPUTER_SERVER), "COMPUTER_SERVER");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::COMPUTER_LAPTOP), "COMPUTER_LAPTOP");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::COMPUTER_HANDHELD_PC_PDA), "COMPUTER_HANDHELD_PC_PDA");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::COMPUTER_PALM_SIZE_PC_PDA), "COMPUTER_PALM_SIZE_PC_PDA");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::COMPUTER_WEARABLE), "COMPUTER_WEARABLE");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::COMPUTER_TABLET), "COMPUTER_TABLET");
    // Phone Major Class
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PHONE_UNCATEGORIZED), "PHONE_UNCATEGORIZED");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PHONE_CELLULAR), "PHONE_CELLULAR");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PHONE_CORDLESS), "PHONE_CORDLESS");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PHONE_SMART), "PHONE_SMART");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PHONE_MODEM_OR_GATEWAY), "PHONE_MODEM_OR_GATEWAY");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PHONE_ISDN), "PHONE_ISDN");
    // LAN/Network Access Point Major Class
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::NETWORK_FULLY_AVAILABLE), "NETWORK_FULLY_AVAILABLE");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::NETWORK_1_TO_17_UTILIZED), "NETWORK_1_TO_17_UTILIZED");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::NETWORK_17_TO_33_UTILIZED), "NETWORK_17_TO_33_UTILIZED");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::NETWORK_33_TO_50_UTILIZED), "NETWORK_33_TO_50_UTILIZED");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::NETWORK_60_TO_67_UTILIZED), "NETWORK_60_TO_67_UTILIZED");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::NETWORK_67_TO_83_UTILIZED), "NETWORK_67_TO_83_UTILIZED");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::NETWORK_83_TO_99_UTILIZED), "NETWORK_83_TO_99_UTILIZED");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::NETWORK_NO_SERVICE), "NETWORK_NO_SERVICE");
    // Audio/Video Major Class
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_UNCATEGORIZED), "AUDIO_VIDEO_UNCATEGORIZED");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_WEARABLE_HEADSET), "AUDIO_VIDEO_WEARABLE_HEADSET");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_HANDSFREE), "AUDIO_VIDEO_HANDSFREE");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_MICROPHONE), "AUDIO_VIDEO_MICROPHONE");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_LOUDSPEAKER), "AUDIO_VIDEO_LOUDSPEAKER");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_HEADPHONES), "AUDIO_VIDEO_HEADPHONES");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_PORTABLE_AUDIO), "AUDIO_VIDEO_PORTABLE_AUDIO");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_CAR_AUDIO), "AUDIO_VIDEO_CAR_AUDIO");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_SET_TOP_BOX), "AUDIO_VIDEO_SET_TOP_BOX");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_HIFI_AUDIO), "AUDIO_VIDEO_HIFI_AUDIO");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_VCR), "AUDIO_VIDEO_VCR");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_VIDEO_CAMERA), "AUDIO_VIDEO_VIDEO_CAMERA");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_CAMCORDER), "AUDIO_VIDEO_CAMCORDER");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_VIDEO_MONITOR), "AUDIO_VIDEO_VIDEO_MONITOR");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_VIDEO_DISPLAY_AND_LOUDSPEAKER),
        "AUDIO_VIDEO_VIDEO_DISPLAY_AND_LOUDSPEAKER");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_VIDEO_CONFERENCING), "AUDIO_VIDEO_VIDEO_CONFERENCING");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::AUDIO_VIDEO_VIDEO_GAMING_TOY), "AUDIO_VIDEO_VIDEO_GAMING_TOY");
    // Peripheral Major Class
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PERIPHERAL_NON_KEYBOARD_NON_POINTING),
        "PERIPHERAL_NON_KEYBOARD_NON_POINTING");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PERIPHERAL_KEYBOARD), "PERIPHERAL_KEYBOARD");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PERIPHERAL_POINTING_DEVICE), "PERIPHERAL_POINTING_DEVICE");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PERIPHERAL_KEYBOARD_POINTING), "PERIPHERAL_KEYBOARD_POINTING");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PERIPHERAL_UNCATEGORIZED), "PERIPHERAL_UNCATEGORIZED");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PERIPHERAL_JOYSTICK), "PERIPHERAL_JOYSTICK");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PERIPHERAL_GAMEPAD), "PERIPHERAL_GAMEPAD");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PERIPHERAL_REMOTE_CONTROL), "PERIPHERAL_REMOTE_CONTROL");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PERIPHERAL_SENSING_DEVICE), "PERIPHERAL_SENSING_DEVICE");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PERIPHERAL_DIGITIZER_TABLET), "PERIPHERAL_DIGITIZER_TABLET");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PERIPHERAL_CARD_READER), "PERIPHERAL_CARD_READER");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PERIPHERAL_DIGITAL_PEN), "PERIPHERAL_DIGITAL_PEN");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PERIPHERAL_SCANNER_RFID), "PERIPHERAL_SCANNER_RFID");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::PERIPHERAL_GESTURAL_INPUT), "PERIPHERAL_GESTURAL_INPUT");
    // Imaging Major Class
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::IMAGING_UNCATEGORIZED), "IMAGING_UNCATEGORIZED");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::IMAGING_DISPLAY), "IMAGING_DISPLAY");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::IMAGING_CAMERA), "IMAGING_CAMERA");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::IMAGING_SCANNER), "IMAGING_SCANNER");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::IMAGING_PRINTER), "IMAGING_PRINTER");
    // Wearable Major Class
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::WEARABLE_UNCATEGORIZED), "WEARABLE_UNCATEGORIZED");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::WEARABLE_WRIST_WATCH), "WEARABLE_WRIST_WATCH");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::WEARABLE_PAGER), "WEARABLE_PAGER");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::WEARABLE_JACKET), "WEARABLE_JACKET");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::WEARABLE_HELMET), "WEARABLE_HELMET");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::WEARABLE_GLASSES), "WEARABLE_GLASSES");
    // Minor Device Class field - Toy Major Class
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::TOY_UNCATEGORIZED), "TOY_UNCATEGORIZED");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::TOY_ROBOT), "TOY_ROBOT");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::TOY_VEHICLE), "TOY_VEHICLE");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::TOY_DOLL_ACTION_FIGURE), "TOY_DOLL_ACTION_FIGURE");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::TOY_CONTROLLER), "TOY_CONTROLLER");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::TOY_GAME), "TOY_GAME");
    // Minor Device Class field - Health
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::HEALTH_UNCATEGORIZED), "HEALTH_UNCATEGORIZED");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::HEALTH_BLOOD_PRESSURE), "HEALTH_BLOOD_PRESSURE");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::HEALTH_THERMOMETER), "HEALTH_THERMOMETER");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::HEALTH_WEIGHING), "HEALTH_WEIGHING");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::HEALTH_GLUCOSE), "HEALTH_GLUCOSE");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::HEALTH_PULSE_OXIMETER), "HEALTH_PULSE_OXIMETER");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::HEALTH_PULSE_RATE), "HEALTH_PULSE_RATE");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::HEALTH_DATA_DISPLAY), "HEALTH_DATA_DISPLAY");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::HEALTH_STEP_COUNTER), "HEALTH_STEP_COUNTER");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::HEALTH_BODY_COMPOSITION_ANALYZER), "HEALTH_BODY_COMPOSITION_ANALYZER");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::HEALTH_PEAK_FLOW_MOITOR), "HEALTH_PEAK_FLOW_MOITOR");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::HEALTH_MEDICATION_MONITOR), "HEALTH_MEDICATION_MONITOR");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::HEALTH_KNEE_PROSTHESIS), "HEALTH_KNEE_PROSTHESIS");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::HEALTH_ANKLE_PROSTHESIS), "HEALTH_ANKLE_PROSTHESIS");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::HEALTH_GENERIC_HEALTH_MANAGER), "HEALTH_GENERIC_HEALTH_MANAGER");
    SetNamedPropertyByInteger(env, majorMinorClass,
        static_cast<int>(MajorMinorClass::HEALTH_PERSONAL_MOBILITY_DEVICE), "HEALTH_PERSONAL_MOBILITY_DEVICE");
    return majorMinorClass;
}

napi_value GetProfileConnState(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
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

    int profileId = 0;
    if (!ParseInt32(env, profileId, argv[PARAM0])) {
        HILOGE("string expected.");
        return ret;
    }

    BluetoothHost *host = &BluetoothHost::GetDefaultHost();
    int state = host->GetBtProfileConnState(GetProfileId(profileId));
    int status = GetProfileConnectionState(state);
    napi_create_int32(env, status, &ret);
    HILOGI("status: %{public}d", status);
    return ret;
}
}  // namespace Bluetooth
}  // namespace OHOS