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
#include "napi_bluetooth_gatt_server.h"
#include "bluetooth_gatt_service.h"
#include "bluetooth_host.h"
#include "bluetooth_log.h"
#include "bluetooth_utils.h"
#include "napi_bluetooth_ble.h"
#include "napi_bluetooth_utils.h"

namespace OHOS {
namespace Bluetooth {
using namespace std;

std::vector<std::string> NapiGattServer::deviceList;
thread_local napi_ref NapiGattServer::consRef_ = nullptr;

napi_value NapiGattServer::CreateGattServer(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    napi_value result;
    napi_value constructor = nullptr;
    napi_get_reference_value(env, consRef_, &constructor);
    napi_new_instance(env, constructor, 0, nullptr, &result);

    return result;
}

void NapiGattServer::DefineGattServerJSClass(napi_env env)
{
    napi_property_descriptor gattserverDesc[] = {
        DECLARE_NAPI_FUNCTION("startAdvertising", StartAdvertising),
        DECLARE_NAPI_FUNCTION("stopAdvertising", StopAdvertising),
        DECLARE_NAPI_FUNCTION("addService", AddService),
        DECLARE_NAPI_FUNCTION("removeService", RemoveGattService),
        DECLARE_NAPI_FUNCTION("close", Close),
        DECLARE_NAPI_FUNCTION("notifyCharacteristicChanged", NotifyCharacteristicChanged),
        DECLARE_NAPI_FUNCTION("sendResponse", SendResponse),
        DECLARE_NAPI_FUNCTION("on", On),
        DECLARE_NAPI_FUNCTION("off", Off),
    };

    napi_value constructor = nullptr;
    napi_define_class(env, "GattServer", NAPI_AUTO_LENGTH, GattServerConstructor, nullptr,
        sizeof(gattserverDesc) / sizeof(gattserverDesc[0]), gattserverDesc, &constructor);
    napi_create_reference(env, constructor, 1, &consRef_);
}

napi_value NapiGattServer::GattServerConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiGattServer* gattServer = new NapiGattServer();

    napi_wrap(
        env, thisVar, gattServer,
        [](napi_env env, void* data, void* hint) {
            NapiGattServer* server = (NapiGattServer*)data;
            if (server) {
                delete server;
                server = nullptr;
            }
        },
        nullptr,
        nullptr);

    return thisVar;
}

napi_value NapiGattServer::On(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattServer *gattServer = nullptr;
    size_t expectedArgsCount = ARGS_SIZE_TWO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    napi_value thisVar = nullptr;

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 2 arguments.");
        return ret;
    }
    string type;
    ParseString(env, type, argv[PARAM0]);
    std::shared_ptr<BluetoothCallbackInfo> callbackInfo ;
    
    if (type.c_str() == STR_BT_GATT_SERVER_CALLBACK_CHARACTERISTIC_READ || 
        type.c_str() == STR_BT_GATT_SERVER_CALLBACK_CHARACTERISTIC_WRITE) {
        callbackInfo = std::make_shared<GattCharacteristicCallbackInfo>();
    } else if (type.c_str() == STR_BT_GATT_SERVER_CALLBACK_DESCRIPTOR_WRITE ||
        type.c_str() == STR_BT_GATT_SERVER_CALLBACK_DESCRIPTOR_READ){
        callbackInfo = std::make_shared<GattDescriptorCallbackInfo>();
    } else {
       callbackInfo = std::make_shared<BluetoothCallbackInfo>();
    }
    callbackInfo->env_ = env;

    napi_unwrap(env, thisVar, (void **)&gattServer);
    if (gattServer == nullptr) {
        HILOGE("gattServer is nullptr.");
        return ret;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[PARAM1], &valueType);
    if (valueType != napi_function) {
        HILOGE("Wrong argument type. Function expected.");
        return ret;
    }
    napi_create_reference(env, argv[PARAM1], 1, &callbackInfo->callback_);
    gattServer->GetCallback().SetCallbackInfo(type, callbackInfo);

    HILOGI("%{public}s is registered", type.c_str());
    return ret;
}

napi_value NapiGattServer::Off(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattServer *gattServer = nullptr;
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
    string type;
    ParseString(env, type, argv[PARAM0]);

    napi_unwrap(env, thisVar, (void **)&gattServer);
    if (gattServer == nullptr) {
        HILOGE("gattServer is nullptr.");
        return ret;
    }
    gattServer->GetCallback().SetCallbackInfo(type, nullptr);
    HILOGI("%{public}s is removed", type.c_str());
    return ret;
}

napi_value NapiGattServer::AddService(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattServer *gattServer = nullptr;
    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value thisVar = nullptr;
    bool isOK = false;
    napi_value ret = nullptr;
    napi_get_boolean(env, isOK, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 1 argument.");
        return ret;
    }
    GattService* gattService = GetServiceFromJS(env, argv[PARAM0], nullptr, nullptr);
    if (gattService == nullptr) {
        HILOGE("gattService is nullptr.");
        return ret;
    }

    napi_unwrap(env, thisVar, (void**)&gattServer);
    if (gattServer == nullptr) {
        HILOGE("gattServer is nullptr.");
        return ret;
    }
    int status = gattServer->GetServer()->AddService(*gattService);
    if (status == GattStatus::GATT_SUCCESS) {
        HILOGI("successful");
        isOK = true;
    } else {
        HILOGE("failed, status: %{public}d", status);
    }

    delete gattService;
    napi_get_boolean(env, isOK, &ret);
    return ret;
}

napi_value NapiGattServer::Close(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattServer* gattServer = nullptr;
    bool isOK = false;
    napi_value thisVar = nullptr;

    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    napi_unwrap(env, thisVar, (void**)&gattServer);
    if (gattServer == nullptr) {
        HILOGE("gattServer is nullptr.");
        return NapiGetBooleanFalse(env);
    }

    int status = gattServer->GetServer()->Close();
    if (status == GattStatus::GATT_SUCCESS) {
        HILOGI("successful");
        isOK = true;
    } else {
        HILOGE("failed, status: %{public}d", status);
    }

    napi_value ret = nullptr;
    napi_get_boolean(env, isOK, &ret);
    return ret;
}

napi_value NapiGattServer::RemoveGattService(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattServer* gattServer = nullptr;
    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value thisVar = nullptr;
    std::string uuid;
    bool isOK = false;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    NAPI_ASSERT(env, argc == expectedArgsCount, "Requires 1 arguments.");
    bool isSuccess = ParseString(env, uuid, argv[PARAM0]);
    if (!isSuccess) {
        HILOGE("ParseString faild.");
        return NapiGetBooleanFalse(env);
    }
    if (!regex_match(uuid, uuidRegex)) {
        HILOGE("match the UUID faild.");
        return NapiGetBooleanFalse(env);
    }

    napi_unwrap(env, thisVar, (void**)&gattServer);
    if (gattServer == nullptr) {
        HILOGE("gattServer is nullptr.");
        return NapiGetBooleanFalse(env);
    }
    UUID serviceUuid = UUID::FromString(uuid);

    std::optional<std::reference_wrapper<GattService>> gattService =
        gattServer->GetServer()->GetService(serviceUuid, true);
    if (gattService != std::nullopt) {
        int status = gattServer->GetServer()->RemoveGattService(*gattService);
        if (status == GattStatus::GATT_SUCCESS) {
            HILOGI("successful");
            isOK = true;
        } else {
            HILOGE("failed, status: %{public}d", status);
        }
    }
    gattService = gattServer->GetServer()->GetService(serviceUuid, false);
    if (gattService != std::nullopt) {
        int status = gattServer->GetServer()->RemoveGattService(*gattService);
        if (status == GattStatus::GATT_SUCCESS) {
            HILOGI("successful");
            isOK = true;
        } else {
            HILOGE("failed, status: %{public}d", status);
        }
    }

    napi_value ret = nullptr;
    napi_get_boolean(env, isOK, &ret);
    return ret;
}

napi_value NapiGattServer::SendResponse(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattServer* gattServer = nullptr;
    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value thisVar = nullptr;
    bool isOK = false;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    NAPI_ASSERT(env, argc == expectedArgsCount, "Requires 1 arguments.");

    ServerResponse serverresponse = GetServerResponseFromJS(env, argv[PARAM0]);
    napi_unwrap(env, thisVar, (void**)&gattServer);
    if (gattServer == nullptr) {
        HILOGE("gattServer is nullptr.");
        return NapiGetBooleanFalse(env);
    }
    BluetoothRemoteDevice remoteDevice =
        BluetoothHost::GetDefaultHost().GetRemoteDevice(serverresponse.deviceId, 1);
    HILOGI("Remote device address: %{public}s", GET_ENCRYPT_ADDR(remoteDevice));
    int status = gattServer->GetServer()->SendResponse(
        remoteDevice, serverresponse.transId,
        serverresponse.status,
        serverresponse.offset,
        serverresponse.value,
        serverresponse.length);
    if (status == GattStatus::GATT_SUCCESS) {
        HILOGI("successful");
        isOK = true;
    } else {
        HILOGE("failed, status: %{public}d", status);
    }

    napi_value ret = nullptr;
    napi_get_boolean(env, isOK, &ret);
    return ret;
}

napi_value NapiGattServer::NotifyCharacteristicChanged(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattServer* gattServer = nullptr;
    size_t expectedArgsCount = ARGS_SIZE_TWO;
    size_t argc = expectedArgsCount;
    std::string deviceID;
    napi_value argv[ARGS_SIZE_TWO] = {0, 0};
    napi_value thisVar = nullptr;
    bool isOK = false;
    napi_value ret = nullptr;
    napi_get_boolean(env, isOK, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 2 argument.");
        return ret;
    }
    napi_unwrap(env, thisVar, (void**)&gattServer);
    if (gattServer == nullptr) {
        HILOGE("gattServer is nullptr.");
        return ret;
    }
    ParseString(env, deviceID, argv[PARAM0]);
    BluetoothRemoteDevice remoteDevice = BluetoothHost::GetDefaultHost().GetRemoteDevice(deviceID, 1);
    GattCharacteristic* characteristic = GetCharacteristicFromJS(env, argv[PARAM1], gattServer->GetServer(), nullptr);

    if (characteristic == nullptr) {
        HILOGI("characteristic is nullptr");
        return ret;
    }
    int status = gattServer->GetServer()->NotifyCharacteristicChanged(remoteDevice, *characteristic, false);
    if (status == GattStatus::GATT_SUCCESS) {
        HILOGI("successful");
        isOK = true;
    } else {
        HILOGE("failed, status: %{public}d", status);
    }

    napi_get_boolean(env, isOK, &ret);
    return ret;
}
} // namespace Bluetooth
} // namespace OHOS
