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
#include "napi_bluetooth_utils.h"
#include "napi_bluetooth_spp_server.h"

namespace OHOS {
namespace Bluetooth {
const int num_20 = 20;
using namespace std;
int NapiSppServer::count = 0;
std::map<int, std::shared_ptr<NapiSppServer>> NapiSppServer::serverMap;

void DefineSppFunctions(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("sppListen", NapiSppServer::SppListen),
        DECLARE_NAPI_FUNCTION("sppAccept", NapiSppServer::SppAccept),
        DECLARE_NAPI_FUNCTION("sppConnect", NapiSppClient::SppConnect),
        DECLARE_NAPI_FUNCTION("sppCloseServerSocket", NapiSppServer::SppCloseServerSocket),
        DECLARE_NAPI_FUNCTION("sppCloseClientSocket", NapiSppClient::SppCloseClientSocket),
        DECLARE_NAPI_FUNCTION("sppWrite", NapiSppClient::SppWrite),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
}

napi_value NapiSppServer::SppListen(napi_env env, napi_callback_info info)
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

    string name;
    if (!ParseString(env, name, argv[PARAM0])) {
        HILOGE("Wrong argument type. String expected.");
        return ret;
    }

    SppListenCallbackInfo *callbackInfo = new SppListenCallbackInfo();
    callbackInfo->env_ = env;
    callbackInfo->sppOption_ = GetSppOptionFromJS(env, argv[PARAM1]);
    if (callbackInfo->sppOption_ == nullptr) {
        HILOGE("GetSppOptionFromJS faild.");
        return ret;
    }
    callbackInfo->name_ = name;

    napi_value promise = nullptr;

    if (argc == expectedArgsCount) {
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
        HILOGI("promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "SppListen", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            HILOGI("SppListen execute");
            SppListenCallbackInfo* callbackInfo = (SppListenCallbackInfo*)data;
            callbackInfo->server_ = std::make_shared<SppServerSocket>(callbackInfo->name_,
                UUID::FromString(callbackInfo->sppOption_->uuid_), callbackInfo->sppOption_->type_,
                callbackInfo->sppOption_->secure_);
            HILOGI("SppListen SppServerSocket constructor end");
            if (callbackInfo->server_ ->GetStringTag() != "") {
                HILOGI("SppListen execute listen success");
                callbackInfo->errorCode_ = CODE_SUCCESS;
            } else {
                HILOGI("SppListen execute listen failed");
                callbackInfo->errorCode_ = CODE_FAILED;
            }
        },
        [](napi_env env, napi_status status, void* data) {
            HILOGI("SppListen execute back");
            SppListenCallbackInfo* callbackInfo = (SppListenCallbackInfo*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);

            if (callbackInfo->errorCode_ == CODE_SUCCESS) {
                HILOGI("SppListen execute back listen success");
                std::shared_ptr<NapiSppServer> server =  std::make_shared<NapiSppServer>();
                server->id_ = NapiSppServer::count++;
                napi_create_int32(env, server->id_, &result[PARAM1]);
                server->server_ = callbackInfo->server_;
                serverMap.insert(std::make_pair(server->id_, server));
            } else {
                HILOGI("SppListen execute back listen failed");
                napi_get_undefined(env, &result[PARAM1]);
            }

            if (callbackInfo->callback_) {
                HILOGI("SppListen execute back listen Callback mode success");
                result[PARAM0] = GetCallbackErrorValue(callbackInfo->env_, callbackInfo->errorCode_);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &callResult);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                if (callbackInfo->errorCode_ == CODE_SUCCESS) {
                    HILOGI("SppListen execute back listen Promise mode success");
                    napi_resolve_deferred(env, callbackInfo->deferred_, result[PARAM1]);
                } else {
                    HILOGI("SppListen execute back listen Promise mode failed");
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


napi_value NapiSppServer::SppAccept(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    size_t expectedArgsCount = ARGS_SIZE_TWO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_TWO] = {0};

    napi_value ret = nullptr;
    napi_get_undefined(env, &ret);

    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != expectedArgsCount && argc != expectedArgsCount - CALLBACK_SIZE) {
        HILOGE("Requires 2 or 3 arguments.");
        return ret;
    }

    int32_t serverSocketNum = -1;
    if (!ParseInt32(env, serverSocketNum, argv[PARAM0])) {
        HILOGE("Wrong argument type. int expected.");
        return ret;
    }
    std::shared_ptr<NapiSppServer> server = serverMap[serverSocketNum];
    if (!server) {
        HILOGE("server is null");
        return ret;
    }

    SppAcceptCallbackInfo *callbackInfo = new SppAcceptCallbackInfo();
    callbackInfo->env_ = env;
    callbackInfo->server_ = server->server_;

    napi_value promise = nullptr;

    if (argc == expectedArgsCount) {
        HILOGI("callback mode");
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[PARAM1], &valueType);
        if (valueType != napi_function) {
            HILOGE("Wrong argument type. Function expected.");
            delete callbackInfo;
            callbackInfo = nullptr;
            return ret;
        }
        napi_create_reference(env, argv[PARAM1], 1, &callbackInfo->callback_);
        napi_get_undefined(env, &promise);
    } else {
        HILOGI("promise mode");
        napi_create_promise(env, &callbackInfo->deferred_, &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "SppAccept", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            HILOGI("SppAccept execute");
            SppAcceptCallbackInfo* callbackInfo = (SppAcceptCallbackInfo*)data;
            callbackInfo->client_ = callbackInfo->server_->Accept(num_20);
            if (callbackInfo->client_ != nullptr) {
                callbackInfo->errorCode_ = CODE_SUCCESS;
            } else {
                callbackInfo->errorCode_ = CODE_FAILED;
            }
        },
        [](napi_env env, napi_status status, void* data) {
            HILOGI("SppAccept execute back");
            SppAcceptCallbackInfo* callbackInfo = (SppAcceptCallbackInfo*)data;
            napi_value result[ARGS_SIZE_TWO] = {0};
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);

            if (callbackInfo->errorCode_ == CODE_SUCCESS) {
                std::shared_ptr<NapiSppClient> client =  std::make_shared<NapiSppClient>();
                client->id_ = NapiSppClient::count++;
                napi_create_int32(env, client->id_, &result[PARAM1]);
                client->client_ = callbackInfo->client_;
                NapiSppClient::clientMap.insert(std::make_pair(client->id_, client));
            } else {
                napi_get_undefined(env, &result[PARAM1]);
            }

            if (callbackInfo->callback_) {
                result[PARAM0] = GetCallbackErrorValue(callbackInfo->env_, callbackInfo->errorCode_);
                napi_get_reference_value(env, callbackInfo->callback_, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, result, &callResult);
                napi_delete_reference(env, callbackInfo->callback_);
            } else {
                if (callbackInfo->errorCode_ == CODE_SUCCESS) {
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

    return ret;
}

napi_value NapiSppServer::SppCloseServerSocket(napi_env env, napi_callback_info info)
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

    std::shared_ptr<NapiSppServer> server = nullptr;
    std::shared_ptr<NapiSppClient> client = nullptr;
    int id =  -1;
    ParseInt32(env, id, argv[PARAM0]);

    if (NapiSppClient::clientMap[id] != nullptr) {
        client = NapiSppClient::clientMap[id];
        if (client->client_) {
            client->client_->Close();
            NapiSppClient::clientMap.erase(id);
        }
    } else {
        HILOGE("no such key in clientMap.");
    }

    if (serverMap[id] != nullptr) {
        server = serverMap[id];
        if (server->server_) {
            server->server_->Close();
            serverMap.erase(id);
            isOK = true;
        }
    } else {
        HILOGE("no such key in serverMap.");
    }

    napi_get_boolean(env, isOK, &ret);
    return ret;
}
} // namespace Bluetooth
} // namespace OHOS
