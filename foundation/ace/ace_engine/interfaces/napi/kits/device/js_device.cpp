/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <string>

#include "napi/native_api.h"
#include "napi/native_engine/native_value.h"
#include "napi/native_node_api.h"

#include "frameworks/base/log/log.h"
#include "frameworks/bridge/common/plugin_adapter/plugin_bridge.h"

namespace OHOS::Ace::Napi {

struct CallbackContext {
    napi_env env = nullptr;
    napi_async_work work = nullptr;

    napi_ref sucCallbackRef = nullptr;
    napi_ref failCallbackRef = nullptr;
    napi_ref cmpCallbackRef = nullptr;

    napi_value infoList = nullptr;
    bool status = 0;
};

static napi_value JSGetInfo(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_object) {
        LOGE("params is not object");
        return nullptr;
    }
    napi_value successFunc;
    napi_value failFunc;
    napi_value cmpFunc;
    auto asyncContext = new CallbackContext();
    asyncContext->env = env;
    napi_get_named_property(env, argv[0], "success", &successFunc);
    napi_typeof(env, successFunc, &valueType);
    if (valueType == napi_function) {
        napi_create_reference(env, successFunc, 1, &asyncContext->sucCallbackRef);
    }
    napi_get_named_property(env, argv[0], "fail", &failFunc);
    napi_typeof(env, failFunc, &valueType);
    if (valueType == napi_function) {
        napi_create_reference(env, failFunc, 1, &asyncContext->failCallbackRef);
    }
    napi_get_named_property(env, argv[0], "complete", &cmpFunc);
    napi_typeof(env, cmpFunc, &valueType);
    if (valueType == napi_function) {
        napi_create_reference(env, cmpFunc, 1, &asyncContext->cmpCallbackRef);
    }

    if (asyncContext->sucCallbackRef == nullptr && asyncContext->failCallbackRef == nullptr &&
        asyncContext->cmpCallbackRef == nullptr) {
        LOGE("all callback is null");
        delete asyncContext;
        return nullptr;
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "JSGetInfo", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(
        env, nullptr, resource, [](napi_env env, void* data) {},
        [](napi_env env, napi_status status, void* data) {
            CallbackContext* asyncContext = (CallbackContext*)data;
            auto getInfoRet = OHOS::Ace::Framework::PluginBridge::GetDeviceInfo();
            napi_value globalValue;
            napi_get_global(env, &globalValue);
            napi_value jsonValue;
            napi_get_named_property(env, globalValue, "JSON", &jsonValue);
            napi_value parseValue;
            napi_get_named_property(env, jsonValue, "parse", &parseValue);
            napi_value info;
            napi_create_string_utf8(env, getInfoRet.first.c_str(), NAPI_AUTO_LENGTH, &info);
            napi_value funcArgv[1] = { info };
            napi_value returnValue;
            napi_call_function(env, jsonValue, parseValue, 1, funcArgv, &returnValue);
            napi_valuetype valueType = napi_undefined;
            napi_typeof(env, returnValue, &valueType);
            if (valueType != napi_object) {
                LOGE("parse result fail");
                delete asyncContext;
                return;
            }
            asyncContext->infoList = returnValue;
            asyncContext->status = getInfoRet.second;

            napi_value result[2] = { 0 };
            result[0] = asyncContext->infoList;
            napi_value code;
            if (asyncContext->status && asyncContext->sucCallbackRef) {
                napi_create_int32(env, 0, &code);
                result[1] = code;
                napi_value callback = nullptr;
                napi_get_reference_value(env, asyncContext->sucCallbackRef, &callback);
                napi_value ret;
                napi_call_function(env, nullptr, callback, 2, result, &ret);
                napi_delete_reference(env, asyncContext->sucCallbackRef);
            } else if (!asyncContext->status && asyncContext->failCallbackRef) {
                napi_create_int32(env, 200, &code);
                result[1] = code;
                napi_value callback = nullptr;
                napi_get_reference_value(env, asyncContext->failCallbackRef, &callback);
                napi_value ret;
                napi_call_function(env, nullptr, callback, 2, result, &ret);
                napi_delete_reference(env, asyncContext->failCallbackRef);
            }
            if (asyncContext->cmpCallbackRef) {
                napi_value callback = nullptr;
                napi_get_reference_value(env, asyncContext->cmpCallbackRef, &callback);
                napi_value ret;
                napi_call_function(env, nullptr, callback, 0, nullptr, &ret);
                napi_delete_reference(env, asyncContext->cmpCallbackRef);
            }
            napi_delete_async_work(env, asyncContext->work);
            delete asyncContext;
        },
        (void*)asyncContext, &asyncContext->work);
    napi_queue_async_work(env, asyncContext->work);

    return nullptr;
}

static napi_value DeviceExport(napi_env env, napi_value exports)
{
    static napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getInfo", JSGetInfo),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

static napi_module deviceModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = DeviceExport,
    .nm_modname = "device",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void DeviceRegister()
{
    napi_module_register(&deviceModule);
}

} // namespace OHOS::Ace::Napi
