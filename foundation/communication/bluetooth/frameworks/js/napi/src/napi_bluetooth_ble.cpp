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
#include "napi_bluetooth_ble.h"

#include "napi_bluetooth_ble_advertise_callback.h"
#include "napi_bluetooth_ble_central_manager_callback.h"
#include "napi_bluetooth_gatt_client.h"
#include "napi_bluetooth_gatt_server.h"
#include "napi_bluetooth_utils.h"

#include "bluetooth_ble_advertiser.h"
#include "bluetooth_ble_central_manager.h"
#include "bluetooth_utils.h"

#include <memory>
namespace OHOS {
namespace Bluetooth {
namespace {
struct SysStopBLEContext {
    napi_async_work work = nullptr;
    napi_ref callbackSuccess = nullptr;
    napi_ref callbackFail = nullptr;
    napi_ref callbackComplete = nullptr;
};

NapiBluetoothBleCentralManagerCallback bleCentralMangerCallback;
NapiBluetoothBleAdvertiseCallback bleAdvertiseCallback;
BleAdvertiser bleAdvertiser;
std::unique_ptr<BleCentralManager> bleCentralManager = std::make_unique<BleCentralManager>(bleCentralMangerCallback);

napi_value GetPropertyValueByNamed(napi_env env, napi_value object, std::string_view propertyName, napi_valuetype type)
{
    napi_value value = nullptr;
    bool hasProperty = false;
    napi_valuetype paraType = napi_undefined;

    NAPI_CALL(env, napi_has_named_property(env, object, propertyName.data(), &hasProperty));
    if (hasProperty) {
        NAPI_CALL(env, napi_get_named_property(env, object, propertyName.data(), &value));
        NAPI_CALL(env, napi_typeof(env, value, &paraType));
        if (paraType != type) {
            return NapiGetNull(env);
        }
    }
    return value;
}

void RegisterBLEObserver(napi_env env, napi_value val, int32_t callbackIndex, const std::string &type)
{
    std::shared_ptr<BluetoothCallbackInfo> pCallbackInfo = std::make_shared<BluetoothCallbackInfo>();
    pCallbackInfo->env_ = env;
    napi_create_reference(env, val, 1, &pCallbackInfo->callback_);
    RegisterSysBLEObserver(pCallbackInfo, callbackIndex, type);
}

bool ParseScanParameters(napi_env env, napi_value arg, ScanOptions &info)
{
    napi_value interval = GetPropertyValueByNamed(env, arg, "interval", napi_number);
    if (interval) {
        napi_get_value_int32(env, interval, &info.interval);
        HILOGI("Scan interval is %{public}d", info.interval);
    } else {
        info.interval = 0;
    }

    std::array<std::string, ARGS_SIZE_THREE> funcArray {"success", "fail", "complete"};

    for (size_t i = 0; i < funcArray.size(); i++) {
        napi_value value = GetPropertyValueByNamed(env, arg, funcArray[i], napi_function);
        if (value) {
            RegisterBLEObserver(env, value, i, REGISTER_SYS_BLE_SCAN_TYPE);
        } else {
            UnregisterSysBLEObserver(REGISTER_SYS_BLE_SCAN_TYPE);
            return false;
        }
    }
    return true;
}

napi_value SysStartBLEScan(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    if (argc != 1) {
        return NapiGetNull(env);
    }

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valueType));
    if (valueType != napi_object) {
        return NapiGetNull(env);
    }
    ScanOptions scanOptions;
    if (!ParseScanParameters(env, argv[PARAM0], scanOptions)) {
        HILOGE("SysStartBLEScan Input parameter parsing failed!");
        return NapiGetNull(env);
    }

    BleScanSettings settinngs;
    settinngs.SetReportDelay(scanOptions.interval);
    settinngs.SetScanMode(static_cast<int32_t>(scanOptions.dutyMode));

    bleCentralManager->StartScan(settinngs);
    return NapiGetNull(env);
}

void SysStopBLEScanExec(napi_env env, void *data)
{
    HILOGI("SysStopBLEScanExec");
    bleCentralManager->StopScan();
    UnregisterSysBLEObserver(REGISTER_SYS_BLE_SCAN_TYPE);
}

void SysStopBLEScanComplete(napi_env env, napi_status status, void *data)
{
    NAPI_CALL_RETURN_VOID(env, (data == nullptr ? napi_invalid_arg : napi_ok));
    std::unique_ptr<SysStopBLEContext> context(static_cast<SysStopBLEContext *>(data));

    napi_value undefine = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefine));
    napi_value funcComplete = nullptr;
    napi_value funcSuccess = nullptr;
    napi_value callbackResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, context->callbackSuccess, &funcSuccess));
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefine, funcSuccess, 0, nullptr, &callbackResult));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, context->callbackComplete, &funcComplete));
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefine, funcComplete, 0, nullptr, &callbackResult));
    NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, context->callbackSuccess));
    NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, context->callbackComplete));
    NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, context->callbackFail));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, context->work));
    HILOGI("SysStopBLEScanComplete end");
}

napi_value SysStopBLEScan(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    if (argc != 1) {
        return NapiGetNull(env);
    }

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valueType));
    if (valueType != napi_object) {
        return NapiGetNull(env);
    }

    std::unique_ptr<SysStopBLEContext> context = std::make_unique<SysStopBLEContext>();

    std::array<std::string, ARGS_SIZE_THREE> funcArray {"success", "fail", "complete"};
    for (size_t i = 0; i < funcArray.size(); i++) {
        napi_value value = GetPropertyValueByNamed(env, argv[PARAM0], funcArray[i], napi_function);
        if (value) {
            napi_create_reference(env, value, 1,
                &(i == PARAM0 ? context->callbackSuccess :
                                (i == PARAM1 ? context->callbackFail : context->callbackComplete)));
        } else {
            HILOGE("SysStopBLEScan Input parameter parsing failed!");
            return NapiGetNull(env);
        }
    }

    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "SysStopBLEScan", NAPI_AUTO_LENGTH, &resourceName));
    SysStopBLEContext *pContext = context.release();
    napi_status status = napi_create_async_work(env, nullptr, resourceName, SysStopBLEScanExec,
        SysStopBLEScanComplete, static_cast<void *>(pContext), &pContext->work);
    if (status != napi_ok) {
        delete pContext;
        return NapiGetNull(env);
    }

    if (napi_queue_async_work(env, pContext->work) != napi_ok) {
        delete pContext;
    }
    return NapiGetNull(env);
}

bool ParseDeviceFoundParameters(napi_env env, napi_value arg)
{
    std::array<std::string, ARGS_SIZE_TWO> funcArray {"success", "fail"};

    for (size_t i = 0; i < funcArray.size(); i++) {
        napi_value value = GetPropertyValueByNamed(env, arg, funcArray[i], napi_function);
        if (value) {
            RegisterBLEObserver(env, value, i, REGISTER_SYS_BLE_FIND_DEVICE_TYPE);
        } else {
            UnregisterSysBLEObserver(REGISTER_SYS_BLE_FIND_DEVICE_TYPE);
            return false;
        }
    }
    return true;
}

napi_value SysSubscribeBLEFound(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    if (argc != 1) {
        return NapiGetNull(env);
    }

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valueType));
    if (valueType != napi_object) {
        return NapiGetNull(env);
    }
    if (!ParseDeviceFoundParameters(env, argv[PARAM0])) {
        HILOGE("SysSubscribeBLEFound Input parameter parsing failed!");
    }
    return NapiGetNull(env);
}

napi_value SysUnsubscribeBLEFound(napi_env env, napi_callback_info info)
{
    UnregisterSysBLEObserver(REGISTER_SYS_BLE_FIND_DEVICE_TYPE);
    return NapiGetNull(env);
}
} // namespace

void DefineSystemBLEInterface(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("startBLEScan", SysStartBLEScan),
        DECLARE_NAPI_FUNCTION("stopBLEScan", SysStopBLEScan),
        DECLARE_NAPI_FUNCTION("subscribeBLEFound", SysSubscribeBLEFound),
        DECLARE_NAPI_FUNCTION("unsubscribeBLEFound", SysUnsubscribeBLEFound),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    HILOGI("DefineSystemBLEInterface init");
}

void DefineBLEJSObject(napi_env env, napi_value exports)
{
    HILOGI("enter");
    napi_value BLEObject = nullptr;
    PropertyInit(env, exports);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("createGattServer", NapiGattServer::CreateGattServer),
        DECLARE_NAPI_FUNCTION("createGattClientDevice", NapiGattClient::CreateGattClientDevice),
        DECLARE_NAPI_FUNCTION("startBLEScan", StartBLEScan),
        DECLARE_NAPI_FUNCTION("stopBLEScan", StopBLEScan),
        DECLARE_NAPI_FUNCTION("on", RegisterObserver),
        DECLARE_NAPI_FUNCTION("off", DeregisterObserver),
        DECLARE_NAPI_FUNCTION("getConnectedBLEDevices", GetConnectedBLEDevices),
    };

    napi_create_object(env, &BLEObject);
    napi_define_properties(env, BLEObject, sizeof(desc) / sizeof(desc[0]), desc);
    napi_set_named_property(env, exports, "BLE", BLEObject);
}

static void ConvertMatchMode(ScanOptions &params, int32_t matchMode)
{
    switch (matchMode) {
        case MatchMode::MATCH_MODE_AGGRESSIVE:
            params.MatchMode = MatchMode::MATCH_MODE_AGGRESSIVE;
            break;
        case MatchMode::MATCH_MODE_STICKY:
            params.MatchMode = MatchMode::MATCH_MODE_STICKY;
            break;
        default:
            break;
    }
}

static void ConvertDutyMode(ScanOptions &params, int32_t dutyMode)
{
    switch (dutyMode) {
        case static_cast<int32_t>(ScanDuty::SCAN_MODE_LOW_POWER):
            params.dutyMode = ScanDuty::SCAN_MODE_LOW_POWER;
            break;
        case static_cast<int32_t>(ScanDuty::SCAN_MODE_BALANCED):
            params.dutyMode = ScanDuty::SCAN_MODE_BALANCED;
            break;
        case static_cast<int32_t>(ScanDuty::SCAN_MODE_LOW_LATENCY):
            params.dutyMode = ScanDuty::SCAN_MODE_LOW_LATENCY;
            break;
        default:
            break;
    }
}

static napi_value ParseScanParameters(
    const napi_env &env, const napi_callback_info &info, const napi_value &scanArg, ScanOptions &params)
{
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc == 0) {
        return NapiGetNull(env);
    }

    bool hasProperty = false;
    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;

    NAPI_CALL(env, napi_has_named_property(env, scanArg, "interval", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, scanArg, "interval", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, result, &params.interval);
        HILOGI("Scan interval is %{public}d", params.interval);
    }

    NAPI_CALL(env, napi_has_named_property(env, scanArg, "dutyMode", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, scanArg, "dutyMode", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        int32_t dutyMode = 0;
        napi_get_value_int32(env, result, &dutyMode);
        HILOGI("Scan dutyMode is %{public}d", dutyMode);
        ConvertDutyMode(params, dutyMode);
    }

    NAPI_CALL(env, napi_has_named_property(env, scanArg, "matchMode", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, scanArg, "matchMode", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        int32_t matchMode = -1;
        napi_get_value_int32(env, result, &matchMode);
        HILOGI("Scan matchMode is %{public}d", matchMode);
        ConvertMatchMode(params, matchMode);
    }
    return NapiGetNull(env);
}

static napi_value ParseScanFilterDeviceIdParameters(
    const napi_env &env, napi_value &scanFilter, BleScanFilter &bleScanFilter)
{
    napi_value result;
    bool hasProperty = false;
    ScanFilter filter;

    NAPI_CALL(env, napi_has_named_property(env, scanFilter, "deviceId", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, scanFilter, "deviceId", &result);
        bool isSuccess = ParseString(env, filter.deviceId, result);
        if (!isSuccess) {
            HILOGE("ParseString faild.");
            return NapiGetBooleanFalse(env);
        }
        bleScanFilter.SetDeviceId(filter.deviceId);
        HILOGI("Scan filter device id is %{public}s", GetEncryptAddr(filter.deviceId).c_str());
    }
    return NapiGetBooleanTrue(env);
}

static napi_value ParseScanFilterLocalNameParameters(
    const napi_env &env, napi_value &scanFilter, BleScanFilter &bleScanFilter)
{
    napi_value result;
    bool hasProperty = false;
    ScanFilter filter;

    NAPI_CALL(env, napi_has_named_property(env, scanFilter, "name", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, scanFilter, "name", &result);
        bool isSuccess = ParseString(env, filter.name, result);
        if (!isSuccess) {
            HILOGE("ParseString faild.");
            return NapiGetBooleanFalse(env);
        }
        bleScanFilter.SetName(filter.name);
        HILOGI("Scan filter name is %{public}s", filter.name.c_str());
    }
    return NapiGetBooleanTrue(env);
}

static napi_value ParseScanFilterServiceUuidParameters(
    const napi_env &env, napi_value &scanFilter, BleScanFilter &bleScanFilter)
{
    napi_value result;
    bool hasProperty = false;
    ScanFilter filter;

    NAPI_CALL(env, napi_has_named_property(env, scanFilter, "serviceUuid", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, scanFilter, "serviceUuid", &result);
        std::string serviceUuid;
        bool isSuccess = ParseString(env, serviceUuid, result);
        if (!isSuccess) {
            HILOGE("ParseString faild.");
            return NapiGetBooleanFalse(env);
        }
        HILOGI("Scan filter serviceUuid is %{public}s", serviceUuid.c_str());
        if (!serviceUuid.empty()) {
            if (!regex_match(serviceUuid, uuidRegex)) {
                HILOGE("match the UUID faild.");
                return NapiGetBooleanFalse(env);
            }
            filter.serviceUuid = ParcelUuid::FromString(serviceUuid);
            bleScanFilter.SetServiceUuid(filter.serviceUuid);
        }
    }

    NAPI_CALL(env, napi_has_named_property(env, scanFilter, "serviceUuidMask", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, scanFilter, "serviceUuidMask", &result);
        std::string serviceUuidMask;
        bool isSuccess = ParseString(env, serviceUuidMask, result);
        if (!isSuccess) {
            HILOGE("ParseString faild.");
            return NapiGetBooleanFalse(env);
        }
        HILOGI("Scan filter serviceUuidMask is %{public}s", serviceUuidMask.c_str());
        if (!serviceUuidMask.empty()) {
            if (!regex_match(serviceUuidMask, uuidRegex)) {
                HILOGE("match the UUID faild.");
                return NapiGetBooleanFalse(env);
            }
            filter.serviceUuidMask = ParcelUuid::FromString(serviceUuidMask);
            bleScanFilter.SetServiceUuidMask(filter.serviceUuidMask);
        }
    }
    return NapiGetBooleanTrue(env);
}

static napi_value ParseScanFilterSolicitationUuidParameters(
    const napi_env &env, napi_value &scanFilter, BleScanFilter &bleScanFilter)
{
    napi_value result;
    bool hasProperty = false;
    ScanFilter filter;

    NAPI_CALL(env, napi_has_named_property(env, scanFilter, "serviceSolicitationUuid", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, scanFilter, "serviceSolicitationUuid", &result);
        std::string serviceSolicitationUuid;
        bool isSuccess = ParseString(env, serviceSolicitationUuid, result);
        if (!isSuccess) {
            HILOGE("ParseString faild.");
            return NapiGetBooleanFalse(env);
        }
        HILOGI("Scan filter serviceSolicitationUuid is %{public}s", serviceSolicitationUuid.c_str());
        if (!serviceSolicitationUuid.empty()) {
            if (!regex_match(serviceSolicitationUuid, uuidRegex)) {
                HILOGE("match the UUID faild.");
                return NapiGetBooleanFalse(env);
            }
            filter.serviceSolicitationUuid = ParcelUuid::FromString(serviceSolicitationUuid);
            bleScanFilter.SetServiceSolicitationUuid(filter.serviceSolicitationUuid);
        }
    }

    NAPI_CALL(env, napi_has_named_property(env, scanFilter, "serviceSolicitationUuidMask", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, scanFilter, "serviceSolicitationUuidMask", &result);
        std::string serviceSolicitationUuidMask;
        bool isSuccess = ParseString(env, serviceSolicitationUuidMask, result);
        if (!isSuccess) {
            HILOGE("ParseString faild.");
            return NapiGetBooleanFalse(env);
        }
        HILOGI("Scan filter serviceSolicitationUuidMask is %{public}s", serviceSolicitationUuidMask.c_str());
        if (!serviceSolicitationUuidMask.empty()) {
            if (!regex_match(serviceSolicitationUuidMask, uuidRegex)) {
                HILOGE("match the UUID faild.");
                return NapiGetBooleanFalse(env);
            }
            filter.serviceSolicitationUuidMask = ParcelUuid::FromString(serviceSolicitationUuidMask);
            bleScanFilter.SetServiceSolicitationUuidMask(filter.serviceSolicitationUuidMask);
        }
    }
    return NapiGetBooleanTrue(env);
}

static napi_value ParseScanFilterServiceDataParameters(
    const napi_env &env, napi_value &scanFilter, BleScanFilter &bleScanFilter)
{
    napi_value result;
    bool hasProperty = false;
    ScanFilter filter;

    NAPI_CALL(env, napi_has_named_property(env, scanFilter, "serviceData", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, scanFilter, "serviceData", &result);
        bool isArrayBuffer = false;
        napi_is_arraybuffer(env, result, &isArrayBuffer);
        if (isArrayBuffer) {
            uint8_t *arrayBufferData = nullptr;
            size_t arrayBufferTotal = 0;
            bool isSuccess = ParseArrayBuffer(env, &arrayBufferData, arrayBufferTotal, result);
            if (!isSuccess) {
                HILOGE("ParseArrayBuffer faild.");
                return NapiGetBooleanFalse(env);
            }
            filter.serviceData = std::vector<uint8_t>(arrayBufferData, arrayBufferData + arrayBufferTotal);
        }
        bleScanFilter.SetServiceData(filter.serviceData);
    }

    NAPI_CALL(env, napi_has_named_property(env, scanFilter, "serviceDataMask", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, scanFilter, "serviceDataMask", &result);
        bool isArrayBuffer = false;
        napi_is_arraybuffer(env, result, &isArrayBuffer);
        if (isArrayBuffer) {
            uint8_t *arrayBufferData = nullptr;
            size_t arrayBufferTotal = 0;
            bool isSuccess = ParseArrayBuffer(env, &arrayBufferData, arrayBufferTotal, result);
            if (!isSuccess) {
                HILOGE("ParseArrayBuffer faild.");
                return NapiGetBooleanFalse(env);
            }
            filter.serviceDataMask = std::vector<uint8_t>(arrayBufferData, arrayBufferData + arrayBufferTotal);
        }
        bleScanFilter.SetServiceDataMask(filter.serviceDataMask);
    }
    return NapiGetBooleanTrue(env);
}

static napi_value ParseScanFilterManufactureDataParameters(
    const napi_env &env, napi_value &scanFilter, BleScanFilter &bleScanFilter)
{
    napi_value result;
    bool hasProperty = false;
    ScanFilter filter;

    NAPI_CALL(env, napi_has_named_property(env, scanFilter, "manufacturerId", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, scanFilter, "manufacturerId", &result);
        uint32_t manufacturerId = 0;
        napi_get_value_uint32(env, result, &manufacturerId);
        filter.manufacturerId = manufacturerId;
        bleScanFilter.SetManufacturerId(filter.manufacturerId);
        HILOGI("Scan filter manufacturerId is %{public}d", filter.manufacturerId);
    }

    NAPI_CALL(env, napi_has_named_property(env, scanFilter, "manufactureData", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, scanFilter, "manufactureData", &result);
        bool isArrayBuffer = false;
        napi_is_arraybuffer(env, result, &isArrayBuffer);
        if (isArrayBuffer) {
            uint8_t *arrayBufferData = nullptr;
            size_t arrayBufferTotal = 0;
            bool isSuccess = ParseArrayBuffer(env, &arrayBufferData, arrayBufferTotal, result);
            if (!isSuccess) {
                HILOGE("ParseArrayBuffer faild.");
                return NapiGetBooleanFalse(env);
            }
            filter.manufactureData = std::vector<uint8_t>(arrayBufferData, arrayBufferData + arrayBufferTotal);
        }
        bleScanFilter.SetManufactureData(filter.manufactureData);
    }

    NAPI_CALL(env, napi_has_named_property(env, scanFilter, "manufactureDataMask", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, scanFilter, "manufactureDataMask", &result);
        bool isArrayBuffer = false;
        napi_is_arraybuffer(env, result, &isArrayBuffer);
        if (isArrayBuffer) {
            uint8_t *arrayBufferData = nullptr;
            size_t arrayBufferTotal = 0;
            bool isSuccess = ParseArrayBuffer(env, &arrayBufferData, arrayBufferTotal, result);
            if (!isSuccess) {
                HILOGE("ParseArrayBuffer faild.");
                return NapiGetBooleanFalse(env);
            }
            filter.manufactureDataMask =
            std::vector<uint8_t>(arrayBufferData, arrayBufferData + arrayBufferTotal);
        }
        bleScanFilter.SetManufactureDataMask(filter.manufactureDataMask);
    }
    return NapiGetBooleanTrue(env);
}

static bool ParseScanFilter(const napi_env &env, napi_value &scanFilter, BleScanFilter &bleScanFilter)
{
    HILOGI("enter");
    bool isSuccess = true;
    napi_get_value_bool(env, ParseScanFilterDeviceIdParameters(env, scanFilter, bleScanFilter), &isSuccess);
    if (!isSuccess) {
        HILOGE("ParseScanFilterDeviceIdParameters faild.");
        return false;
    }
    napi_get_value_bool(env, ParseScanFilterLocalNameParameters(env, scanFilter, bleScanFilter), &isSuccess);
    if (!isSuccess) {
        HILOGE("ParseScanFilterLocalNameParameters faild.");
        return false;
    }
    napi_get_value_bool(env, ParseScanFilterServiceUuidParameters(env, scanFilter, bleScanFilter), &isSuccess);
    if (!isSuccess) {
        HILOGE("ParseScanFilterServiceUuidParameters faild.");
        return false;
    }
    napi_get_value_bool(env, ParseScanFilterSolicitationUuidParameters(env, scanFilter, bleScanFilter), &isSuccess);
    if (!isSuccess) {
        HILOGE("ParseScanFilterSolicitationUuidParameters faild.");
        return false;
    }
    napi_get_value_bool(env, ParseScanFilterServiceDataParameters(env, scanFilter, bleScanFilter), &isSuccess);
    if (!isSuccess) {
        HILOGE("ParseScanFilterServiceDataParameters faild.");
        return false;
    }
    napi_get_value_bool(env, ParseScanFilterManufactureDataParameters(env, scanFilter, bleScanFilter), &isSuccess);
    if (!isSuccess) {
        HILOGE("ParseScanFilterManufactureDataParameters faild.");
        return false;
    }
    return true;
}

static napi_value ParseScanFilterParameters(const napi_env &env, napi_value &args, std::vector<BleScanFilter> &params)
{
    HILOGI("enter");
    if (args == nullptr) {
        return NapiGetBooleanTrue(env);
    }

    bool isArray = false;
    napi_is_array(env, args, &isArray);
    if (isArray) {
        uint32_t length = 0;
        napi_get_array_length(env, args, &length);
        for (size_t i = 0; i < length; i++) {
            napi_value scanFilter;
            napi_valuetype valuetype = napi_undefined;
            napi_get_element(env, args, i, &scanFilter);
            NAPI_CALL(env, napi_typeof(env, scanFilter, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");

            BleScanFilter bleScanFilter;
            bool isSuccess = ParseScanFilter(env, scanFilter, bleScanFilter);
            if (!isSuccess) {
                HILOGE("ParseScanFilter faild.");
                return NapiGetBooleanFalse(env);
            }

            params.push_back(bleScanFilter);
        }
    }
    return NapiGetBooleanTrue(env);
}

napi_value StartBLEScan(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc == 0) {
        HILOGE("Requires 1 or 2 arguments.");
        return NapiGetNull(env);
    }

    std::vector<BleScanFilter> scanfilters;
    if (argv[PARAM0] != nullptr) {
        bool isSuccess = true;
        napi_get_value_bool(env, ParseScanFilterParameters(env, argv[PARAM0], scanfilters), &isSuccess);
        if (!isSuccess) {
            HILOGE("ParseScanFilterParameters faild.");
            return NapiGetNull(env);
        }
    }
    bleCentralManager->ConfigScanFilter(scanfilters);

    BleScanSettings settinngs;
    if (argv[PARAM1] != nullptr) {
        ScanOptions scanOptions;
        ParseScanParameters(env, info, argv[PARAM1], scanOptions);
        settinngs.SetReportDelay(scanOptions.interval);
        settinngs.SetScanMode(static_cast<int32_t>(scanOptions.dutyMode));
    }

    bleCentralManager->StartScan(settinngs);
    return NapiGetNull(env);
}

napi_value StopBLEScan(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    bleCentralManager->StopScan();
    return NapiGetNull(env);
}

static napi_value ParseAdvertisingSettingsParameters(
    const napi_env &env, const napi_callback_info &info, const napi_value &args, BleAdvertiserSettings &settings)
{
    HILOGI("enter");
    bool hasProperty = false;
    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;

    NAPI_CALL(env, napi_has_named_property(env, args, "interval", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, args, "interval", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        int32_t interval = 0;
        napi_get_value_int32(env, result, &interval);
        HILOGI("interval is %{public}d", interval);
        settings.SetInterval(interval);
    }
    NAPI_CALL(env, napi_has_named_property(env, args, "txPower", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, args, "txPower", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        int32_t txPower = 0;
        napi_get_value_int32(env, result, &txPower);
        HILOGI("txPower is %{public}d", txPower);
        settings.SetTxPower(txPower);
    }

    NAPI_CALL(env, napi_has_named_property(env, args, "connectable", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, args, "connectable", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Boolean expected.");
        bool connectable = true;
        napi_get_value_bool(env, result, &connectable);
        HILOGI("connectable is %{public}d", connectable);
        settings.SetConnectable(connectable);
    }
    return NapiGetNull(env);
}

static napi_value ParseServiceUuidParameters(const napi_env &env, const napi_value &args, BleAdvertiserData &data)
{
    HILOGI("enter");
    napi_value result = nullptr;

    napi_get_named_property(env, args, "serviceUuids", &result);
    bool isArray = false;
    napi_is_array(env, result, &isArray);
    NAPI_ASSERT(env, isArray, "Wrong argument type. Array expected.");
    uint32_t length = 0;
    napi_get_array_length(env, result, &length);
    NAPI_ASSERT(env, length > 0, "The array is empty.");
    for (size_t serviceUuidIter = 0; serviceUuidIter < length; ++serviceUuidIter) {
        napi_value uuid = nullptr;
        napi_get_element(env, result, serviceUuidIter, &uuid);
        std::string serviceUuid;
        bool isSuccess = ParseString(env, serviceUuid, uuid);
        if (!isSuccess) {
            HILOGE("ParseString faild.");
            return NapiGetBooleanFalse(env);
        }
        if (!regex_match(serviceUuid, uuidRegex)) {
            HILOGE("Match the UUID faild.");
            return NapiGetBooleanFalse(env);
        }
        data.AddServiceUuid(ParcelUuid::FromString(serviceUuid));
        HILOGI("Service Uuid = %{public}s", serviceUuid.c_str());
    }
    return NapiGetBooleanTrue(env);
}

static napi_value ParseManufactureDataParameters(const napi_env &env, const napi_value &args, BleAdvertiserData &data)
{
    HILOGI("enter");
    napi_value result = nullptr;

    napi_get_named_property(env, args, "manufactureData", &result);
    bool isArray = false;
    napi_is_array(env, result, &isArray);
    NAPI_ASSERT(env, isArray, "Wrong argument type. Array expected.");
    uint32_t length = 0;
    napi_get_array_length(env, result, &length);
    if (length == 0) {
        HILOGE("The array is empty.");
        return NapiGetBooleanTrue(env);
    }
    for (size_t manufactureDataIter = 0; manufactureDataIter < length; ++manufactureDataIter) {
        napi_value manufactureData = nullptr;
        napi_valuetype valuetype = napi_undefined;
        napi_get_element(env, result, manufactureDataIter, &manufactureData);
        NAPI_CALL(env, napi_typeof(env, manufactureData, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        bool hasProperty = false;
        NAPI_CALL(env, napi_has_named_property(env, manufactureData, "manufactureId", &hasProperty));
        NAPI_ASSERT(env, hasProperty, "manufactureId expected.");
        napi_get_named_property(env, manufactureData, "manufactureId", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        int32_t manufactureId = 0;
        napi_get_value_int32(env, result, &manufactureId);
        HILOGI("manufactureId = %{public}d", manufactureId);
        NAPI_CALL(env, napi_has_named_property(env, manufactureData, "manufactureValue", &hasProperty));
        NAPI_ASSERT(env, hasProperty, "manufactureValue expected.");
        napi_get_named_property(env, manufactureData, "manufactureValue", &result);
        bool isArrayBuffer = false;
        napi_is_arraybuffer(env, result, &isArrayBuffer);
        if (isArrayBuffer) {
            uint8_t *arrayBufferData = nullptr;
            size_t arrayBufferTotal = 0;
            bool isSuccess = ParseArrayBuffer(env, &arrayBufferData, arrayBufferTotal, result);
            if (!isSuccess) {
                HILOGE("ParseArrayBuffer faild.");
                return NapiGetBooleanFalse(env);
            }
            std::string manuData(arrayBufferData, arrayBufferData + arrayBufferTotal);
            data.AddManufacturerData(manufactureId, manuData);
        }
    }
    return NapiGetBooleanTrue(env);
}

static napi_value ParseServiceDataParameters(const napi_env &env, const napi_value &args, BleAdvertiserData &data)
{
    HILOGI("enter");
    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    napi_get_named_property(env, args, "serviceData", &result);
    bool isArray = false;
    napi_is_array(env, result, &isArray);
    NAPI_ASSERT(env, isArray, "Wrong argument type. Array expected.");
    uint32_t length = 0;
    napi_get_array_length(env, result, &length);
    if (length == 0) {
        HILOGE("The array is empty.");
        return NapiGetBooleanTrue(env);
    }
    for (size_t serviceDataIter = 0; serviceDataIter < length; ++serviceDataIter) {
        napi_value serviceData = nullptr;
        napi_get_element(env, result, serviceDataIter, &serviceData);
        NAPI_CALL(env, napi_typeof(env, serviceData, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        bool hasProperty = false;
        NAPI_CALL(env, napi_has_named_property(env, serviceData, "serviceUuid", &hasProperty));
        NAPI_ASSERT(env, hasProperty, "serviceUuid expected.");
        napi_get_named_property(env, serviceData, "serviceUuid", &result);
        std::string serviceBuffer;
        bool isSuccess = ParseString(env, serviceBuffer, result);
        if (!isSuccess) {
            HILOGE("ParseString faild.");
            return NapiGetBooleanFalse(env);
        }
        if (!regex_match(serviceBuffer, uuidRegex)) {
            HILOGE("Match the UUID faild.");
            return NapiGetBooleanFalse(env);
        }
        ParcelUuid serviceUuid(ParcelUuid::FromString(serviceBuffer));
        HILOGI("serviceUuid = %{public}s", serviceUuid.ToString().c_str());

        NAPI_CALL(env, napi_has_named_property(env, serviceData, "serviceValue", &hasProperty));
        NAPI_ASSERT(env, hasProperty, "serviceValue expected.");
        napi_get_named_property(env, serviceData, "serviceValue", &result);
        bool isArrayBuffer = false;
        napi_is_arraybuffer(env, result, &isArrayBuffer);
        NAPI_ASSERT(env, isArrayBuffer, "Array buffer expected.");
        uint8_t *arrayBufferData = nullptr;
        size_t arrayBufferTotal;
        napi_get_arraybuffer_info(env, result, reinterpret_cast<void **>(&arrayBufferData), &arrayBufferTotal);
        std::string serviceDataStr(arrayBufferData, arrayBufferData + arrayBufferTotal);
        data.AddServiceData(serviceUuid, serviceDataStr);
    }
    return NapiGetBooleanTrue(env);
}

static napi_value ParseAdvertisDataParameters(
    const napi_env &env, const napi_callback_info &info, const napi_value &args, BleAdvertiserData &data)
{
    HILOGI("enter");
    bool hasProperty = false;
    bool isSuccess = true;
    NAPI_CALL(env, napi_has_named_property(env, args, "serviceUuids", &hasProperty));
    if (hasProperty) {
        napi_get_value_bool(env, ParseServiceUuidParameters(env, args, data), &isSuccess);
        if (!isSuccess) {
            HILOGE("ParseServiceUuidParameters faild.");
            return NapiGetBooleanFalse(env);
        }
    }
    NAPI_CALL(env, napi_has_named_property(env, args, "manufactureData", &hasProperty));
    if (hasProperty) {
        napi_get_value_bool(env, ParseManufactureDataParameters(env, args, data), &isSuccess);
        if (!isSuccess) {
            HILOGE("ParseManufactureDataParameters faild.");
            return NapiGetBooleanFalse(env);
        }
    }
    NAPI_CALL(env, napi_has_named_property(env, args, "serviceData", &hasProperty));
    if (hasProperty) {
        napi_get_value_bool(env, ParseServiceDataParameters(env, args, data), &isSuccess);
        if (!isSuccess) {
            HILOGE("ParseServiceDataParameters faild.");
            return NapiGetBooleanFalse(env);
        }
    }
    return NapiGetBooleanTrue(env);
}

napi_value StartAdvertising(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc == 0) {
        HILOGE("Requires 2 or 3 arguments.");
        return NapiGetNull(env);
    }
    BleAdvertiserSettings settings;
    ParseAdvertisingSettingsParameters(env, info, argv[PARAM0], settings);
    BleAdvertiserData AdvData;
    bool isSuccess = true;
    napi_get_value_bool(env, ParseAdvertisDataParameters(env, info, argv[PARAM1], AdvData), &isSuccess);
    if (!isSuccess) {
        HILOGE("ParseAdvertisDataParameters faild.");
        return NapiGetNull(env);
    }
    BleAdvertiserData ScanRespData;
    if (argv[PARAM2] != nullptr) {
        napi_get_value_bool(env, ParseAdvertisDataParameters(env, info, argv[PARAM2], ScanRespData), &isSuccess);
        if (!isSuccess) {
            HILOGE("ParseAdvertisDataParameters faild.");
            return NapiGetNull(env);
        }
    }

    bleAdvertiser.StartAdvertising(settings, AdvData, ScanRespData, bleAdvertiseCallback);
    return NapiGetNull(env);
}

napi_value StopAdvertising(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    bleAdvertiser.StopAdvertising(bleAdvertiseCallback);

    return NapiGetNull(env);
}

napi_value GetConnectedBLEDevices(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    napi_value result = nullptr;
    napi_create_array(env, &result);

    ConvertStringVectorToJS(env, result, NapiGattServer::deviceList);
    return result;
}

napi_value PropertyInit(napi_env env, napi_value exports)
{
    HILOGI("enter");

    napi_value matchModeObj = nullptr;
    napi_value scanDutyObj = nullptr;
    napi_create_object(env, &matchModeObj);
    napi_create_object(env, &scanDutyObj);

    SetNamedPropertyByInteger(env, matchModeObj, MatchMode::MATCH_MODE_STICKY, "MATCH_MODE_STICKY");
    SetNamedPropertyByInteger(env, matchModeObj, MatchMode::MATCH_MODE_AGGRESSIVE, "MATCH_MODE_AGGRESSIVE");
    SetNamedPropertyByInteger(
        env, scanDutyObj, static_cast<int32_t>(ScanDuty::SCAN_MODE_BALANCED), "SCAN_MODE_BALANCED");
    SetNamedPropertyByInteger(
        env, scanDutyObj, static_cast<int32_t>(ScanDuty::SCAN_MODE_LOW_LATENCY), "SCAN_MODE_LOW_LATENCY");
    SetNamedPropertyByInteger(
        env, scanDutyObj, static_cast<int32_t>(ScanDuty::SCAN_MODE_LOW_POWER), "SCAN_MODE_LOW_POWER");

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("MatchMode", matchModeObj),
        DECLARE_NAPI_PROPERTY("ScanDuty", scanDutyObj),
    };

    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    return exports;
}
}  // namespace Bluetooth
}  // namespace OHOS
