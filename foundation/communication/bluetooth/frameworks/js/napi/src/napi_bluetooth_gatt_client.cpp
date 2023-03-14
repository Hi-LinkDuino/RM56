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
#include "napi_bluetooth_gatt_client.h"
#include <unistd.h>
#include "bluetooth_log.h"
#include "napi_bluetooth_utils.h"
#include "napi_bluetooth_host.h"
#include "napi_bluetooth_event.h"


namespace OHOS {
namespace Bluetooth {
using namespace std;

thread_local napi_ref NapiGattClient::consRef_ = nullptr;
napi_value NapiGattClient::CreateGattClientDevice(napi_env env, napi_callback_info info)
{
    HILOGI("enter");

    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = {0};

    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    NAPI_ASSERT(env, argc == expectedArgsCount, "Requires 1 arguments.");

    napi_value result;
    napi_value constructor = nullptr;
    napi_get_reference_value(env, consRef_, &constructor);
    napi_new_instance(env, constructor, argc, argv, &result);

    return result;
}

void NapiGattClient::DefineGattClientJSClass(napi_env env)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("connect", Connect),
        DECLARE_NAPI_FUNCTION("disconnect", Disconnect),
        DECLARE_NAPI_FUNCTION("close", Close),
        DECLARE_NAPI_FUNCTION("getDeviceName", GetDeviceName),
        DECLARE_NAPI_FUNCTION("getServices", GetServices),
        DECLARE_NAPI_FUNCTION("readCharacteristicValue", ReadCharacteristicValue),
        DECLARE_NAPI_FUNCTION("readDescriptorValue", ReadDescriptorValue),
        DECLARE_NAPI_FUNCTION("writeCharacteristicValue", WriteCharacteristicValue),
        DECLARE_NAPI_FUNCTION("writeDescriptorValue", WriteDescriptorValue),
        DECLARE_NAPI_FUNCTION("getRssiValue", GetRssiValue),
        DECLARE_NAPI_FUNCTION("setBLEMtuSize", SetBLEMtuSize),
        DECLARE_NAPI_FUNCTION("setNotifyCharacteristicChanged", SetNotifyCharacteristicChanged),
        DECLARE_NAPI_FUNCTION("on", On),
        DECLARE_NAPI_FUNCTION("off", Off),
    };

    napi_value constructor = nullptr;
    napi_define_class(env, "GattClientDevice", NAPI_AUTO_LENGTH, GattClientConstructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    napi_create_reference(env, constructor, 1, &consRef_);
}

napi_value NapiGattClient::GattClientConstructor(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    napi_value thisVar = nullptr;

    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = {0};

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);

    string deviceId;
    ParseString(env, deviceId, argv[PARAM0]);
    SetGattClientDeviceId(deviceId);

    NapiGattClient *gattClient = new NapiGattClient(deviceId);

    napi_wrap(
        env, thisVar, gattClient,
        [](napi_env env, void* data, void* hint) {
            NapiGattClient* client = (NapiGattClient*)data;
            if (client) {
                delete client;
                client = nullptr;
            }
        },
        nullptr,
        nullptr);

    return thisVar;
}

napi_value NapiGattClient::On(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    std::unique_lock<std::mutex> guard(g_gattClientCallbackInfosMutex);
    NapiGattClient* gattClient = nullptr;
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
    string type;
    ParseString(env, type, argv[PARAM0]);
    std::shared_ptr<BluetoothCallbackInfo> callbackInfo;
    if (type.c_str() == STR_BT_GATT_CLIENT_CALLBACK_BLE_CHARACTERISTIC_CHANGE) {
        callbackInfo = std::make_shared<GattCharacteristicCallbackInfo>();
    } else {
        callbackInfo = std::make_shared<BluetoothCallbackInfo>();
    }
    callbackInfo->env_ = env;

    napi_unwrap(env, thisVar, (void **)&gattClient);
    if (gattClient == nullptr) {
        HILOGE("gattClient is nullptr.");
        return ret;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[PARAM1], &valueType);
    if (valueType != napi_function) {
        HILOGE("Wrong argument type. Function expected.");
        return ret;
    }
    napi_create_reference(env, argv[PARAM1], 1, &callbackInfo->callback_);
    gattClient->GetCallback().SetCallbackInfo(type, callbackInfo);

    HILOGI("%{public}s is registered", type.c_str());

    return ret;
}

napi_value NapiGattClient::Off(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    std::unique_lock<std::mutex> guard(g_gattClientCallbackInfosMutex);
    NapiGattClient* gattClient = nullptr;
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
    napi_unwrap(env, thisVar, (void **)&gattClient);
    if (gattClient == nullptr) {
        HILOGE("gattClient is nullptr.");
        return ret;
    }
    if (gattClient->GetCallback().GetCallbackInfo(type) == nullptr) {
        HILOGE("type %{public}s callbackInfos isn't exist.", type.c_str());
        return ret;
    }
    uint32_t refCount = INVALID_REF_COUNT;
    napi_reference_unref(env, gattClient->GetCallback().GetCallbackInfo(type)->callback_, &refCount);
    HILOGI("decrements the refernce count, refCount: %{public}d", refCount);
    if (refCount == 0) {
        napi_delete_reference(env, gattClient->GetCallback().GetCallbackInfo(type)->callback_);
    }
    gattClient->GetCallback().SetCallbackInfo(type, nullptr);
    HILOGI("%{public}s is removed", type.c_str());
    return ret;
}


napi_value NapiGattClient::Connect(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattClient *gattClient = nullptr;
    napi_value thisVar = nullptr;
    bool isOK = false;

    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    napi_unwrap(env, thisVar, (void **)&gattClient);
    if (gattClient == nullptr) {
        HILOGE("gattClient is nullptr.");
        return NapiGetBooleanFalse(env);
    }
    int status = gattClient->GetClient()->Connect(gattClient->GetCallback(), true, GATT_TRANSPORT_TYPE_LE);
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

napi_value NapiGattClient::Disconnect(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattClient* gattClient = nullptr;
    napi_value thisVar = nullptr;
    bool isOK = false;

    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    napi_unwrap(env, thisVar, (void**)&gattClient);
    if (gattClient == nullptr) {
        HILOGE("gattClient is nullptr.");
        return NapiGetBooleanFalse(env);
    }
    int status = gattClient->GetClient()->Disconnect();
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

napi_value NapiGattClient::ReadCharacteristicValue(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattClient *gattClient = nullptr;
    size_t expectedArgsCount = ARGS_SIZE_TWO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_TWO] = {0};

    napi_value thisVar = nullptr;
    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount && argc != expectedArgsCount - CALLBACK_SIZE) {
        HILOGE("Requires 1 or 2 arguments.");
        return ret;
    }

    napi_unwrap(env, thisVar, (void**)&gattClient);
    if (gattClient == nullptr) {
        HILOGE("gattClient is nullptr.");
        return ret;
    }
    gattClient->readCharacteristicValueCallbackInfo_ = new ReadCharacteristicValueCallbackInfo();
    ReadCharacteristicValueCallbackInfo *callbackInfo = gattClient->readCharacteristicValueCallbackInfo_;
    callbackInfo->env_ = env;
    callbackInfo->client_ = gattClient->GetClient();
    callbackInfo->inputCharacteristic_ = GetCharacteristicFromJS(env, argv[PARAM0], nullptr, callbackInfo->client_);

    napi_value promise = nullptr;

    if (argc == expectedArgsCount) {
        // Callback mode
        HILOGI("callback mode");
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[PARAM1], &valueType);
        if (valueType != napi_function) {
            HILOGE("Wrong argument type. Function expected.");
            return ret;
        }
        napi_create_reference(env, argv[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        // Promise mode
        HILOGI("promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "readCharacteristicValue", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env,
        nullptr,
        resource,
        [](napi_env env, void *data) {
            HILOGI("ReadCharacteristicValue(): execute");
            ReadCharacteristicValueCallbackInfo* callbackInfo = (ReadCharacteristicValueCallbackInfo*)data;
            callbackInfo->asyncState_ = ASYNC_START;
            int status = -1;
            if (callbackInfo->inputCharacteristic_ != nullptr) {
                HILOGI("ReadCharacteristicValue(): Client read characteristic");
                status = callbackInfo->client_->ReadCharacteristic(*(callbackInfo->inputCharacteristic_));
            }

            if (status == GattStatus::GATT_SUCCESS) {
                HILOGI("ReadCharacteristicValue(): successful");
                callbackInfo->errorCode_ = CODE_SUCCESS;
                int tryTime = 100;
                while (callbackInfo->asyncState_ == ASYNC_START && tryTime > 0) {
                    usleep(SLEEP_TIME);
                    if (callbackInfo->asyncState_ == ASYNC_DONE) {
                        break;
                    }
                    tryTime--;
                }
                if (callbackInfo->asyncState_ != ASYNC_DONE) {
                    HILOGE("ReadCharacteristicValue(): failed, async not done");
                    callbackInfo->errorCode_ = CODE_FAILED;
                }
            } else {
                HILOGE("ReadCharacteristicValue(): failed, status: %{public}d", status);
                callbackInfo->errorCode_ = CODE_FAILED;
            }
        },
        [](napi_env env, napi_status status, void *data) {
            HILOGI("ReadCharacteristicValue(): execute back");
            ReadCharacteristicValueCallbackInfo* callbackInfo = (ReadCharacteristicValueCallbackInfo*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);

            HILOGI("ReadCharacteristicValue(): errorCode: %{public}d", callbackInfo->errorCode_);
            if (callbackInfo->errorCode_ == CODE_SUCCESS) {
                napi_create_object(env, &result[PARAM1]);
                ConvertBLECharacteristicToJS(env, result[PARAM1],
                    const_cast<GattCharacteristic&>(*(callbackInfo->outputCharacteristic_)));
            } else {
                napi_get_undefined(env, &result[PARAM1]);
            }

            if (callbackInfo->callback_) {
                // Callback mode
                result[PARAM0] = GetCallbackErrorValue(callbackInfo->env_, callbackInfo->errorCode_);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &callResult);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                if (callbackInfo->errorCode_ == CODE_SUCCESS) {
                // Promise mode
                    napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
                } else {
                    napi_reject_deferred(env, callbackInfo->deferred_, result[PARAM1]);
                }
            }
            napi_delete_async_work(env, callbackInfo->asyncWork_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->asyncWork_);
    napi_queue_async_work(env, callbackInfo->asyncWork_);
    return promise;
}

napi_value NapiGattClient::ReadDescriptorValue(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattClient* gattClient = nullptr;
    size_t expectedArgsCount = ARGS_SIZE_TWO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_TWO] = {0};

    napi_value thisVar = nullptr;
    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount && argc != expectedArgsCount - CALLBACK_SIZE) {
        HILOGE("Requires 1 or 2 arguments.");
        return ret;
    }

    napi_unwrap(env, thisVar, (void**)&gattClient);
    if (gattClient == nullptr) {
        HILOGE("gattClient is nullptr.");
        return ret;
    }
    gattClient->readDescriptorValueCallbackInfo_ = new ReadDescriptorValueCallbackInfo();
    ReadDescriptorValueCallbackInfo *callbackInfo = gattClient->readDescriptorValueCallbackInfo_;
    callbackInfo->env_ = env;
    callbackInfo->client_ = gattClient->GetClient();
    callbackInfo->inputDescriptor_ = GetDescriptorFromJS(env, argv[PARAM0], nullptr, gattClient->GetClient());

    napi_value promise = nullptr;

    if (argc == expectedArgsCount) {
        // Callback mode
        HILOGI("callback mode");
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[PARAM1], &valueType);
        if (valueType != napi_function) {
            HILOGE("Wrong argument type. Function expected.");
            return ret;
        }
        napi_create_reference(env, argv[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        // Promise mode
        HILOGI("promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "readDescriptorValue", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            HILOGI("ReadDescriptorValue(): execute");
            ReadDescriptorValueCallbackInfo* callbackInfo = (ReadDescriptorValueCallbackInfo*)data;
            callbackInfo->asyncState_ = ASYNC_START;
            int status = -1;
            if (callbackInfo->inputDescriptor_ != nullptr) {
                HILOGI("ReadDescriptorValue(): Client read descriptor");
                status = callbackInfo->client_->ReadDescriptor(*(callbackInfo->inputDescriptor_));
            }

            if (status == GattStatus::GATT_SUCCESS) {
                HILOGI("ReadDescriptorValue(): successful");
                callbackInfo->errorCode_ = CODE_SUCCESS;
                int tryTime = 100;
                while (callbackInfo->asyncState_ == ASYNC_START && tryTime > 0) {
                    usleep(SLEEP_TIME);
                    if (callbackInfo->asyncState_ == ASYNC_DONE) {
                        break;
                    }
                    tryTime--;
                }
                if (callbackInfo->asyncState_ != ASYNC_DONE) {
                    HILOGE("ReadDescriptorValue(): failed, async not done");
                    callbackInfo->errorCode_ = CODE_FAILED;
                }
            } else {
                HILOGE("ReadDescriptorValue(): failed, status: %{public}d", status);
                callbackInfo->errorCode_ = CODE_FAILED;
            }
        },
        [](napi_env env, napi_status status, void* data) {
            HILOGI("ReadDescriptorValue(): execute back");
            ReadDescriptorValueCallbackInfo* callbackInfo = (ReadDescriptorValueCallbackInfo*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);

            HILOGI("ReadDescriptorValue(): errorCode: %{public}d", callbackInfo->errorCode_);
            if (callbackInfo->errorCode_ == CODE_SUCCESS) {
                napi_create_object(env, &result[PARAM1]);
                ConvertBLEDescriptorToJS(env, result[PARAM1],
                    const_cast<GattDescriptor&>(*(callbackInfo->outputDescriptor_)));
            } else {
                napi_get_undefined(env, &result[PARAM1]);
            }

            if (callbackInfo->callback_) {
                // Callback mode
                result[PARAM0] = GetCallbackErrorValue(callbackInfo->env_, callbackInfo->errorCode_);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &callResult);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                if (callbackInfo->errorCode_ == CODE_SUCCESS) {
                // Promise mode
                    napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
                } else {
                    napi_reject_deferred(env, callbackInfo->deferred_, result[PARAM1]);
                }
            }
            napi_delete_async_work(env, callbackInfo->asyncWork_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void*)callbackInfo,
        &callbackInfo->asyncWork_);

    napi_queue_async_work(env, callbackInfo->asyncWork_);
    return promise;
}

napi_value NapiGattClient::GetServices(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattClient* gattClient = nullptr;
    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = {0};

    napi_value thisVar = nullptr;
    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount && argc != expectedArgsCount - CALLBACK_SIZE) {
        HILOGE("Requires 0 or 1 arguments.");
        return ret;
    }

    napi_unwrap(env, thisVar, (void**)&gattClient);
    if (gattClient == nullptr) {
        HILOGE("gattClient is nullptr.");
        return ret;
    }

    GetServiceCallbackInfo *callbackInfo = new GetServiceCallbackInfo();
    callbackInfo->env_ = env;
    callbackInfo->client_ = gattClient->GetClient();

    napi_value promise = nullptr;

    if (argc == expectedArgsCount) {
        // Callback mode
        HILOGI("callback mode");
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[PARAM0], &valueType);
        if (valueType != napi_function) {
            HILOGE("Wrong argument type. Function expected.");
            delete callbackInfo;
            callbackInfo = nullptr;
            return ret;
        }
        napi_create_reference(env, argv[PARAM0], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        // Promise mode
        HILOGI("promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "getServices", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            HILOGI("GetServices(): execute");
            GetServiceCallbackInfo* callbackInfo = (GetServiceCallbackInfo*)data;
            int status = callbackInfo->client_->DiscoverServices();
            if (status != GattStatus::GATT_SUCCESS) {
                HILOGE("GetServices(): failed, status: %{public}d", status);
                callbackInfo->errorCode_ = CODE_FAILED;
            } else {
                HILOGI("GetServices(): successful");
                callbackInfo->services_ = callbackInfo->client_->GetService();
                callbackInfo->errorCode_ = CODE_SUCCESS;
            }
        },
        [](napi_env env, napi_status status, void* data) {
            HILOGI("GetServices(): execute back");
            GetServiceCallbackInfo* callbackInfo = (GetServiceCallbackInfo*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);

            HILOGI("GetServices(): errorCode: %{public}d", callbackInfo->errorCode_);
            if (callbackInfo->errorCode_ == CODE_SUCCESS) {
                napi_create_array(env, &result[PARAM1]);
                ConvertGattServiceVectorToJS(env, result[PARAM1], callbackInfo->services_);
            } else {
                napi_get_undefined(env, &result[PARAM1]);
            }

            if (callbackInfo->callback_) {
                // Callback mode
                result[PARAM0] = GetCallbackErrorValue(callbackInfo->env_, callbackInfo->errorCode_);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &callResult);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                if (callbackInfo->errorCode_ == CODE_SUCCESS) {
                // Promise mode
                    napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
                } else {
                    napi_reject_deferred(env, callbackInfo->deferred_, result[PARAM1]);
                }
            }
            napi_delete_async_work(env, callbackInfo->asyncWork_);
            delete callbackInfo;
            callbackInfo = nullptr;
        },
        (void *)callbackInfo,
        &callbackInfo->asyncWork_);

    napi_queue_async_work(env, callbackInfo->asyncWork_);
    return promise;
}

napi_value NapiGattClient::Close(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattClient* gattClient = nullptr;
    bool isOK = false;
    napi_value thisVar = nullptr;

    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    napi_unwrap(env, thisVar, (void**)&gattClient);
    if (gattClient == nullptr) {
        HILOGE("gattClient is nullptr.");
        return NapiGetBooleanFalse(env);
    }

    int status = gattClient->GetClient()->Close();
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

napi_value NapiGattClient::WriteCharacteristicValue(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattClient* gattClient = nullptr;

    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value ret = nullptr;

    napi_value thisVar = nullptr;
    bool isOK = false;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 1 argument.");
        return ret;
    }

    napi_unwrap(env, thisVar, (void**)&gattClient);
    if (gattClient == nullptr) {
        HILOGE("gattClient is nullptr.");
        return NapiGetBooleanFalse(env);
    }

    GattCharacteristic* characteristic = GetCharacteristicFromJS(env, argv[PARAM0], nullptr, gattClient->GetClient());
    if (characteristic != nullptr) {
        HILOGI("Client write characteristic");
        int status = gattClient->GetClient()->WriteCharacteristic(*characteristic);
        if (status == GattStatus::GATT_SUCCESS) {
            HILOGI("successful");
            isOK = true;
        } else {
            HILOGE("failed, status: %{public}d", status);
        }
    }

    napi_get_boolean(env, isOK, &ret);
    return ret;
}

napi_value NapiGattClient::WriteDescriptorValue(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattClient* gattClient = nullptr;

    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value ret = nullptr;

    napi_value thisVar = nullptr;
    bool isOK = false;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 1 argument.");
        return ret;
    }

    napi_unwrap(env, thisVar, (void**)&gattClient);
    if (gattClient == nullptr) {
        HILOGE("gattClient is nullptr.");
        return NapiGetBooleanFalse(env);
    }

    GattDescriptor* descriptor = GetDescriptorFromJS(env, argv[PARAM0], nullptr, gattClient->GetClient());
    if (descriptor != nullptr) {
        HILOGI("Client write descriptor");
        int status = gattClient->GetClient()->WriteDescriptor(*descriptor);
        if (status == GattStatus::GATT_SUCCESS) {
            HILOGI("successful");
            isOK = true;
        } else {
            HILOGE("failed, status: %{public}d", status);
        }
    }

    napi_get_boolean(env, isOK, &ret);
    return ret;
}

napi_value NapiGattClient::SetBLEMtuSize(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattClient* gattClient = nullptr;

    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value ret = nullptr;

    napi_value thisVar = nullptr;
    bool isOK = false;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 1 argument.");
        return ret;
    }

    napi_unwrap(env, thisVar, (void**)&gattClient);
    if (gattClient == nullptr) {
        HILOGE("gattClient is nullptr.");
        return NapiGetBooleanFalse(env);
    }

    int32_t mtuSize;
    ParseInt32(env, mtuSize, argv[PARAM0]);

    int status = gattClient->GetClient()->RequestBleMtuSize(mtuSize);
    if (status == GattStatus::GATT_SUCCESS) {
        HILOGI("successful");
        isOK = true;
    } else {
        HILOGE("failed, status: %{public}d", status);
    }

    napi_get_boolean(env, isOK, &ret);
    return ret;
}

napi_value NapiGattClient::SetNotifyCharacteristicChanged(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    NapiGattClient* gattClient = nullptr;

    size_t expectedArgsCount = ARGS_SIZE_TWO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    napi_value ret = nullptr;

    napi_value thisVar = nullptr;
    bool isOK = false;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 1 argument.");
        return ret;
    }

    napi_unwrap(env, thisVar, (void**)&gattClient);
    if (gattClient == nullptr) {
        HILOGE("gattClient is nullptr.");
        return NapiGetBooleanFalse(env);
    }

    GattCharacteristic* characteristic = GetCharacteristicFromJS(env, argv[PARAM0], nullptr, gattClient->GetClient());
    if (characteristic == nullptr) {
        HILOGI("characteristic is nullptr");
        napi_get_boolean(env, false, &ret);
        return ret;
    }
    bool enableNotify = false;
    ParseBool(env, enableNotify, argv[PARAM1]);

    int status = gattClient->GetClient()->SetNotifyCharacteristic(*characteristic, enableNotify);
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
