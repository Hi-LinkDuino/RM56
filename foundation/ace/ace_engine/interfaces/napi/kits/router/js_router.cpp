/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "frameworks/bridge/common/utils/engine_helper.h"
#include "frameworks/bridge/js_frontend/engine/common/js_engine.h"

namespace OHOS::Ace::Napi {

static void ParseUri(napi_env env, napi_value uriNApi, std::string& uriString)
{
    if (uriNApi != nullptr) {
        auto nativeUri = reinterpret_cast<NativeValue*>(uriNApi);
        auto resultUri = nativeUri->ToString();
        auto nativeStringUri = reinterpret_cast<NativeString*>(resultUri->GetInterface(NativeString::INTERFACE_ID));
        size_t uriLen = nativeStringUri->GetLength() + 1;
        std::unique_ptr<char[]> uri = std::make_unique<char[]>(uriLen);
        size_t retLen = 0;
        napi_get_value_string_utf8(env, uriNApi, uri.get(), uriLen, &retLen);
        uriString = uri.get();
    }
}

static void ParseParams(napi_env env, napi_value params, std::string& paramsString)
{
    if (params != nullptr) {
        napi_value globalValue;
        napi_get_global(env, &globalValue);
        napi_value jsonValue;
        napi_get_named_property(env, globalValue, "JSON", &jsonValue);
        napi_value stringifyValue;
        napi_get_named_property(env, jsonValue, "stringify", &stringifyValue);
        napi_value funcArgv[1] = { params };
        napi_value returnValue;
        napi_call_function(env, jsonValue, stringifyValue, 1, funcArgv, &returnValue);
        auto nativeValue = reinterpret_cast<NativeValue*>(returnValue);
        auto resultValue = nativeValue->ToString();
        auto nativeString = reinterpret_cast<NativeString*>(resultValue->GetInterface(NativeString::INTERFACE_ID));
        size_t len = nativeString->GetLength() + 1;
        std::unique_ptr<char[]> paramsChar = std::make_unique<char[]>(len);
        size_t ret = 0;
        napi_get_value_string_utf8(env, returnValue, paramsChar.get(), len, &ret);
        paramsString = paramsChar.get();
    }
}

static napi_value JSRouterPush(napi_env env, napi_callback_info info)
{
    LOGI("NAPI router push called");
    size_t requireArgc = 1;
    size_t argc = 1;
    napi_value argv = nullptr;
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, &argv, &thisVar, &data);
    if (argc < requireArgc) {
        LOGE("params number err");
        return nullptr;
    }
    napi_value uriNApi = nullptr;
    napi_value params = nullptr;
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv, &valueType);
    if (valueType == napi_object) {
        napi_get_named_property(env, argv, "url", &uriNApi);
        napi_typeof(env, uriNApi, &valueType);
        if (valueType != napi_string) {
            LOGE("url is invalid");
            return nullptr;
        }
        napi_get_named_property(env, argv, "params", &params);
    }
    std::string paramsString;
    ParseParams(env, params, paramsString);
    std::string uriString;
    napi_typeof(env, uriNApi, &valueType);
    if (valueType == napi_string) {
        ParseUri(env, uriNApi, uriString);
    } else {
        LOGW("The parameter type is incorrect.");
    }
    auto delegate = EngineHelper::GetCurrentDelegate();
    if (!delegate) {
        LOGE("can not get delegate.");
        return nullptr;
    }
    delegate->Push(uriString, paramsString);
    return nullptr;
}

static napi_value JSRouterReplace(napi_env env, napi_callback_info info)
{
    size_t requireArgc = 1;
    size_t argc = 1;
    napi_value argv = nullptr;
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, &argv, &thisVar, &data);
    if (argc < requireArgc) {
        LOGE("params number err");
        return nullptr;
    }
    napi_value uriNApi = nullptr;
    napi_value params = nullptr;
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv, &valueType);
    if (valueType == napi_object) {
        napi_get_named_property(env, argv, "url", &uriNApi);
        napi_typeof(env, uriNApi, &valueType);
        if (valueType != napi_string) {
            LOGE("url is invalid");
            return nullptr;
        }
        napi_get_named_property(env, argv, "params", &params);
    }
    std::string paramsString;
    ParseParams(env, params, paramsString);
    std::string uriString;
    napi_typeof(env, uriNApi, &valueType);
    if (valueType == napi_string) {
        ParseUri(env, uriNApi, uriString);
    } else {
        LOGW("The parameter type is incorrect.");
    }

    auto delegate = EngineHelper::GetCurrentDelegate();
    if (!delegate) {
        LOGE("can not get delegate.");
        return nullptr;
    }
    delegate->Replace(uriString, paramsString);
    return nullptr;
}

static napi_value JSRouterBack(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv = nullptr;
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, &argv, &thisVar, &data);

    auto delegate = EngineHelper::GetCurrentDelegate();
    if (!delegate) {
        LOGE("can not get delegate.");
        return nullptr;
    }
    std::string uriString = "";
    std::string paramsString = "";
    napi_value uriNApi = nullptr;
    napi_value params = nullptr;
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv, &valueType);
    if (valueType == napi_object) {
        napi_get_named_property(env, argv, "url", &uriNApi);
        napi_typeof(env, uriNApi, &valueType);
        if (valueType == napi_undefined) {
            napi_get_named_property(env, argv, "path", &uriNApi);
            napi_typeof(env, uriNApi, &valueType);
        }
        if (valueType == napi_string) {
            ParseUri(env, uriNApi, uriString);
        } else {
            LOGE("the url and path is all invalid");
            return nullptr;
        }

        napi_get_named_property(env, argv, "params", &params);
        napi_typeof(env, params, &valueType);
        if (valueType == napi_object) {
            ParseParams(env, params, paramsString);
        } else {
            LOGW("The parameter type is incorrect.");
        }
    }
    delegate->Back(uriString, paramsString);
    return nullptr;
}

static napi_value JSRouterClear(napi_env env, napi_callback_info info)
{
    auto delegate = EngineHelper::GetCurrentDelegate();
    if (!delegate) {
        LOGE("can not get delegate.");
        return nullptr;
    }
    delegate->Clear();
    return nullptr;
}

static napi_value JSRouterGetLength(napi_env env, napi_callback_info info)
{
    auto delegate = EngineHelper::GetCurrentDelegate();
    if (!delegate) {
        LOGE("can not get delegate.");
        return nullptr;
    }
    int32_t routeNumber = delegate->GetStackSize();
    napi_value routeNApiNum = nullptr;
    napi_create_int32(env, routeNumber, &routeNApiNum);
    napi_value result = nullptr;
    napi_coerce_to_string(env, routeNApiNum, &result);
    return result;
}

static napi_value JSRouterGetState(napi_env env, napi_callback_info info)
{
    int32_t routeIndex = 0;
    std::string routeName;
    std::string routePath;
    auto delegate = EngineHelper::GetCurrentDelegate();
    if (!delegate) {
        LOGE("can not get delegate.");
        return nullptr;
    }
    delegate->GetState(routeIndex, routeName, routePath);
    size_t routeNameLen = routeName.length();
    size_t routePathLen = routePath.length();

    napi_value resultArray[3] = { 0 };
    napi_create_int32(env, routeIndex, &resultArray[0]);
    napi_create_string_utf8(env, routeName.c_str(), routeNameLen, &resultArray[1]);
    napi_create_string_utf8(env, routePath.c_str(), routePathLen, &resultArray[2]);

    napi_value result = nullptr;
    napi_create_object(env, &result);
    napi_set_named_property(env, result, "index", resultArray[0]);
    napi_set_named_property(env, result, "name", resultArray[1]);
    napi_set_named_property(env, result, "path", resultArray[2]);
    return result;
}

struct RouterAsyncContext {
    napi_env env = nullptr;
    napi_async_work work = nullptr;
    napi_value message_napi = nullptr;
    napi_ref callbackSuccess = nullptr;
    napi_ref callbackFail = nullptr;
    napi_ref callbackComplete = nullptr;
    std::string messageString;
    std::string fail;
    std::string success;
    std::string complete;
};

napi_value GetReturnObj(napi_env env, std::string callbackString)
{
    napi_value result = nullptr;
    napi_value returnObj = nullptr;
    napi_create_object(env, &returnObj);
    napi_create_string_utf8(env, callbackString.c_str(), NAPI_AUTO_LENGTH, &result);
    napi_set_named_property(env, returnObj, "errMsg", result);
    return returnObj;
}

static napi_value JSRouterEnableAlertBeforeBackPage(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv = nullptr;
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, &argv, &thisVar, &data);
    auto routerAsyncContext = new RouterAsyncContext();
    routerAsyncContext->env = env;
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv, &valueType);
    if (valueType == napi_object) {
        napi_get_named_property(env, argv, "message", &routerAsyncContext->message_napi);
        napi_typeof(env, routerAsyncContext->message_napi, &valueType);
        if (valueType == napi_string) {
            auto nativeValue = reinterpret_cast<NativeValue*>(routerAsyncContext->message_napi);
            auto resultValue = nativeValue->ToString();
            auto nativeString = reinterpret_cast<NativeString*>(resultValue->GetInterface(NativeString::INTERFACE_ID));
            size_t len = nativeString->GetLength() + 1;
            std::unique_ptr<char[]> messageChar = std::make_unique<char[]>(len);
            size_t ret = 0;
            napi_get_value_string_utf8(env, routerAsyncContext->message_napi, messageChar.get(), len, &ret);
            routerAsyncContext->messageString = messageChar.get();
        } else {
            LOGE("The message type is incorrect.");
            return nullptr;
        }
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "JSRouterEnableAlertBeforeBackPage", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(
        env, nullptr, resource, [](napi_env env, void* data) {},
        [](napi_env env, napi_status status, void* data) {
            RouterAsyncContext* asyncContext = (RouterAsyncContext*)data;
            auto callBack = [](int32_t callbackType) {};
            auto delegate = EngineHelper::GetCurrentDelegate();
            if (delegate) {
                delegate->EnableAlertBeforeBackPage(asyncContext->messageString, std::move(callBack));
            } else {
                LOGE("can not get delegate.");
            }
            napi_delete_async_work(env, asyncContext->work);
            delete asyncContext;
        },
        (void*)routerAsyncContext, &routerAsyncContext->work);
    napi_queue_async_work(env, routerAsyncContext->work);
    return nullptr;
}

struct DisableRouterAsyncContext {
    napi_env env = nullptr;
    napi_async_work work = nullptr;
    napi_ref callbackSuccess = nullptr;
    napi_ref callbackFail = nullptr;
    napi_ref callbackComplete = nullptr;
    std::string fail;
    std::string success;
    std::string complete;
};

static napi_value JSRouterDisableAlertBeforeBackPage(napi_env env, napi_callback_info info)
{
    auto disableRouterAsyncContext = new DisableRouterAsyncContext();
    disableRouterAsyncContext->env = env;
    size_t argc = 1;
    napi_value argv = nullptr;
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, &argv, &thisVar, &data);
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "JSRouterDisableAlertBeforeBackPage", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(
        env, nullptr, resource, [](napi_env env, void* data) {},
        [](napi_env env, napi_status status, void* data) {
            DisableRouterAsyncContext* asyncContext = (DisableRouterAsyncContext*)data;
            auto delegate = EngineHelper::GetCurrentDelegate();
            if (delegate) {
                delegate->DisableAlertBeforeBackPage();
            } else {
                LOGE("can not get delegate.");
            }
            napi_delete_async_work(env, asyncContext->work);
            delete asyncContext;
        },
        (void*)disableRouterAsyncContext, &disableRouterAsyncContext->work);
    napi_queue_async_work(env, disableRouterAsyncContext->work);
    return nullptr;
}

static napi_value JSRouterGetParams(napi_env env, napi_callback_info info)
{
    auto delegate = EngineHelper::GetCurrentDelegate();
    if (!delegate) {
        LOGE("can not get delegate.");
        return nullptr;
    }
    std::string paramsStr = delegate->GetParams();
    if (paramsStr.empty()) {
        LOGI("PageGetParams params is null");
        return nullptr;
    }
    napi_value globalValue;
    napi_get_global(env, &globalValue);
    napi_value jsonValue;
    napi_get_named_property(env, globalValue, "JSON", &jsonValue);
    napi_value parseValue;
    napi_get_named_property(env, jsonValue, "parse", &parseValue);
    napi_value routerParamsNApi;
    napi_create_string_utf8(env, paramsStr.c_str(), NAPI_AUTO_LENGTH, &routerParamsNApi);
    napi_value funcArgv[1] = { routerParamsNApi };
    napi_value result;
    napi_call_function(env, jsonValue, parseValue, 1, funcArgv, &result);
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, result, &valueType);
    if (valueType != napi_object) {
        LOGE("parse result fail");
        return nullptr;
    }
    return result;
}

static napi_value RouterExport(napi_env env, napi_value exports)
{
    static napi_property_descriptor routerDesc[] = {
        DECLARE_NAPI_FUNCTION("push", JSRouterPush),
        DECLARE_NAPI_FUNCTION("replace", JSRouterReplace),
        DECLARE_NAPI_FUNCTION("back", JSRouterBack),
        DECLARE_NAPI_FUNCTION("clear", JSRouterClear),
        DECLARE_NAPI_FUNCTION("getLength", JSRouterGetLength),
        DECLARE_NAPI_FUNCTION("getState", JSRouterGetState),
        DECLARE_NAPI_FUNCTION("enableAlertBeforeBackPage", JSRouterEnableAlertBeforeBackPage),
        DECLARE_NAPI_FUNCTION("disableAlertBeforeBackPage", JSRouterDisableAlertBeforeBackPage),
        DECLARE_NAPI_FUNCTION("getParams", JSRouterGetParams),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(routerDesc) / sizeof(routerDesc[0]), routerDesc));
    return exports;
}

static napi_module routerModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = RouterExport,
    .nm_modname = "router",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RouterRegister()
{
    napi_module_register(&routerModule);
}

} // namespace OHOS::Ace::Napi
