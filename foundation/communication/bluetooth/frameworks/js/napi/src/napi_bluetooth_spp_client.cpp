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

#include "napi_bluetooth_spp_client.h"
#include "securec.h"
#include <unistd.h>
#include <uv.h>

namespace OHOS {
namespace Bluetooth {
const int sleepTime = 5;
std::map<int, std::shared_ptr<NapiSppClient>> NapiSppClient::clientMap;
int NapiSppClient::count = 0;

napi_value NapiSppClient::SppConnect(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    size_t expectedArgsCount = ARGS_SIZE_THREE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_THREE] = {0};

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);

    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != expectedArgsCount && argc != expectedArgsCount - CALLBACK_SIZE) {
        HILOGE("Requires 2 or 3 arguments.");
        return ret;
    }

    std::string deviceId;
    if (!ParseString(env, deviceId, argv[PARAM0])) {
        HILOGE("Wrong argument type. String expected.");
        return ret;
    }

    SppConnectCallbackInfo *callbackInfo = new SppConnectCallbackInfo();
    callbackInfo->env_ = env;
    callbackInfo->sppOption_ = GetSppOptionFromJS(env, argv[PARAM1]);
    if (callbackInfo->sppOption_ == nullptr) {
        HILOGE("GetSppOptionFromJS faild.");
        return ret;
    }
    callbackInfo->deviceId_ = deviceId;

    napi_value promise = nullptr;

    if (argc == expectedArgsCount) {
        // Callback mode
        HILOGI("callback mode");
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[PARAM2], &valueType);
        if (valueType != napi_function) {
            HILOGE("Wrong argument type. Function expected.");
            delete callbackInfo;
            callbackInfo = nullptr;
            return ret;
        }
        napi_create_reference(env, argv[PARAM2], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        // Promise mode
        HILOGI("promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "SppConnect", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            HILOGI("SppConnect execute");
            SppConnectCallbackInfo* callbackInfo = (SppConnectCallbackInfo*)data;
            callbackInfo->device_ = std::make_shared<BluetoothRemoteDevice>(callbackInfo->deviceId_, 0);
            callbackInfo->client_ = std::make_shared<SppClientSocket>(*callbackInfo->device_,
                UUID::FromString(callbackInfo->sppOption_->uuid_),
                callbackInfo->sppOption_->type_, callbackInfo->sppOption_->secure_);
            HILOGI("SppConnect client_ constructed");
            if (callbackInfo->client_->Connect() == BtStatus::BT_SUCCESS) {
                HILOGI("SppConnect successfully");
                callbackInfo->errorCode_ = CODE_SUCCESS;
            } else {
                HILOGI("SppConnect failed");
                callbackInfo->errorCode_ = CODE_FAILED;
            }
        },
        [](napi_env env, napi_status status, void* data) {
            HILOGI("SppConnect execute back");
            SppConnectCallbackInfo* callbackInfo = (SppConnectCallbackInfo*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);

            if (callbackInfo->errorCode_ == CODE_SUCCESS) {
                HILOGI("SppConnect execute back success");
                std::shared_ptr<NapiSppClient> client =  std::make_shared<NapiSppClient>();
                client->device_ = callbackInfo->device_;
                client->id_ = NapiSppClient::count++;
                napi_create_int32(env, client->id_, &result[PARAM1]);
                client->client_ = callbackInfo->client_;
                clientMap.insert(std::make_pair(client->id_, client));
                HILOGI("SppConnect execute back successfully");
            } else {
                napi_get_undefined(env, &result[PARAM1]);
                HILOGI("SppConnect execute back failed");
            }

            if (callbackInfo->callback_) {
                // Callback mode
                HILOGI("SppConnect execute Callback mode");
                result[PARAM0] = GetCallbackErrorValue(callbackInfo->env_, callbackInfo->errorCode_);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &callResult);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                if (callbackInfo->errorCode_ == CODE_SUCCESS) {
                // Promise mode
                    HILOGI("SppConnect execute Promise mode successfully");
                    napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
                } else {
                    HILOGI("SppConnect execute Promise mode failed");
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
    return ret;
}

napi_value NapiSppClient::SppCloseClientSocket(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value thisVar = nullptr;
    bool isOK = false;
    napi_value ret = nullptr;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 1 argument.");
        return ret;
    }

    std::shared_ptr<NapiSppClient> client = nullptr;
    int id =  -1;
    ParseInt32(env, id, argv[PARAM0]);

    if (clientMap[id]) {
        client = clientMap[id];
    } else {
        HILOGE("no such key in map.");
        return ret;
    }

    if (client->client_) {
        client->client_->Close();
        isOK = true;
    }
    clientMap.erase(id);
    napi_get_boolean(env, isOK, &ret);
    return ret;
}

napi_value NapiSppClient::SppWrite(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);
    size_t expectedArgsCount = ARGS_SIZE_TWO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_TWO] = {0};
    napi_value thisVar = nullptr;
    char* totalBuf = nullptr;
    size_t totalSize = 0;
    bool isOK = false;
    int id = -1;
    napi_get_boolean(env, isOK, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 3 argument.");
        return ret;
    }

    ParseInt32(env, id, argv[PARAM0]);

    if (!ParseArrayBuffer(env, (uint8_t**)(&totalBuf), totalSize, argv[PARAM1])) {
        HILOGE("ParseArrayBuffer failed");
        return ret;
    }

    if (clientMap[id]) {
        OutputStream outputStream = clientMap[id]->client_->GetOutputStream();
        while (totalSize) {
            int result = outputStream.Write(totalBuf, totalSize);
            if (result > 0) {
                totalSize = totalSize - result;
                totalBuf += result;
                isOK = true;
            } else if (result < 0) {
                HILOGI("Write socket exception!");
                return ret;
            }
        }
    } else {
        HILOGI("Client socket not exit!");
    }
    napi_get_boolean(env, isOK, &ret);
    return ret;
}

void NapiSppClient::On(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    size_t expectedArgsCount = ARGS_SIZE_THREE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_THREE] = {0};
    napi_value thisVar = nullptr;
    int id = -1;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 3 argument.");
        return;
    }
    std::string type;
    ParseString(env, type, argv[PARAM0]);
    std::shared_ptr<BluetoothCallbackInfo> callbackInfo;
    if (type.c_str() == STR_BT_SPP_READ) {
        callbackInfo = std::make_shared<BufferCallbackInfo>();
    } else {
        HILOGE("Invalid type!");
        return;
    }
    callbackInfo->env_ = env;

    napi_valuetype valueType1 = napi_undefined;
    napi_valuetype valueType2 = napi_undefined;
    napi_typeof(env, argv[PARAM1], &valueType1);
    napi_typeof(env, argv[PARAM2], &valueType2);
    if (valueType1 != napi_number && valueType2 != napi_function) {
        HILOGE("Wrong argument type. Function expected.");
        return;
    }

    napi_create_reference(env, argv[PARAM2], 1, &callbackInfo->callback_);

    ParseInt32(env, id, argv[PARAM1]);
    std::shared_ptr<NapiSppClient> client = clientMap[id];
    if (!client) {
        HILOGI("client is nullptr");
        return;
    }
    client->sppReadFlag = true;
    client->callbackInfos_[type] = callbackInfo;
    HILOGI("sppRead begin");
    client->thread_ = std::make_shared<std::thread>(SppRead, id);
    client->thread_->detach();
    return;
}

void NapiSppClient::Off(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    size_t expectedArgsCount = ARGS_SIZE_THREE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_THREE] = {0};
    napi_value thisVar = nullptr;
    int id = -1;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 3 argument.");
        return;
    }

    std::string type;
    ParseString(env, type, argv[PARAM0]);

    ParseInt32(env, id, argv[PARAM1]);
    std::shared_ptr<NapiSppClient> client = clientMap[id];
    if (!client) {
        HILOGE("client is nullptr");
        return;
    }
    if (type.c_str() != STR_BT_SPP_READ) {
        HILOGE("type is wrong");
        return;
    }
    client->callbackInfos_[type] = nullptr;
    client->sppReadFlag = false;
    sleep(sleepTime);
    return;
}

void NapiSppClient::SppRead(int id)
{
    if (clientMap[id] == nullptr || !clientMap[id]->sppReadFlag ||
        clientMap[id]->callbackInfos_[STR_BT_SPP_READ] == nullptr) {
        HILOGE("thread start failed.");
        return;
    }
    InputStream inputStream = clientMap[id]->client_->GetInputStream();
    char buf[1024];
    int ret = 0;

    while (true) {
        HILOGI("thread start.");
        (void)memset_s(buf, sizeof(buf), 0, sizeof(buf));
        HILOGI("inputStream.Read start");
        ret = inputStream.Read(buf, sizeof(buf));
        HILOGI("inputStream.Read end");
        if (ret <= 0) {
            HILOGI("inputStream.Read failed, ret = %{public}d", ret);
            return;
        } else {
            HILOGI("callback read data to jshap begin");
            if (clientMap[id] == nullptr || !clientMap[id]->sppReadFlag ||
                clientMap[id]->callbackInfos_[STR_BT_SPP_READ] == nullptr) {
                HILOGE("failed");
                return;
            }
            std::shared_ptr<BufferCallbackInfo> callbackInfo =
                std::static_pointer_cast<BufferCallbackInfo>(clientMap[id]->callbackInfos_[STR_BT_SPP_READ]);

            callbackInfo->info_ = ret;
            if (memcpy_s(callbackInfo->buffer_, sizeof(callbackInfo->buffer_), buf, ret) != EOK) {
                HILOGE("memcpy_s failed!");
                return;
            }

            uv_loop_s *loop = nullptr;
            napi_get_uv_event_loop(callbackInfo->env_, &loop);
            uv_work_t *work = new uv_work_t;
            work->data = (void*)callbackInfo.get();

            uv_queue_work(
                loop,
                work,
                [](uv_work_t *work) {},
                [](uv_work_t *work, int status) {
                    BufferCallbackInfo *callbackInfo = (BufferCallbackInfo *)work->data;
                    int size = callbackInfo->info_;
                    uint8_t* totalBuf = (uint8_t*) malloc(size);
                    if (memcpy_s(totalBuf, size, callbackInfo->buffer_, size) != EOK) {
                        HILOGE("memcpy_s failed!");
                    }
                    napi_value result = nullptr;
                    uint8_t* bufferData = nullptr;
                    napi_create_arraybuffer(callbackInfo->env_, size, (void**)&bufferData, &result);
                    if (memcpy_s(bufferData, size, totalBuf, size) != EOK) {
                        HILOGE("memcpy_s failed!");
                    }
                    free(totalBuf);

                    napi_value callback = nullptr;
                    napi_value undefined = nullptr;
                    napi_value callResult = nullptr;
                    napi_get_undefined(callbackInfo->env_, &undefined);
                    napi_get_reference_value(callbackInfo->env_, callbackInfo->callback_, &callback);
                    napi_call_function(callbackInfo->env_, undefined, callback, ARGS_SIZE_ONE, &result, &callResult);
                    delete work;
                    work = nullptr;
                }
            );
        }
    }
    return;
}
} // namespace Bluetooth
} // namespace OHOS