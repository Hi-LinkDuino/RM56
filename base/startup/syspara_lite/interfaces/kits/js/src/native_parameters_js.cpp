/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "native_parameters_js.h"

static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "StartupParametersJs" };
using namespace OHOS::HiviewDFX;
static constexpr int MAX_LENGTH = 128;
static constexpr int ARGC_NUMBER = 2;
static constexpr int ARGC_THREE_NUMBER = 3;
static constexpr int BUF_LENGTH = 256;

using StorageAsyncContext = struct {
    napi_env env = nullptr;
    napi_async_work work = nullptr;

    char key[BUF_LENGTH] = { 0 };
    size_t keyLen = 0;
    char value[BUF_LENGTH] = { 0 };
    size_t valueLen = 0;
    napi_deferred deferred = nullptr;
    napi_ref callbackRef = nullptr;

    int status = -1;
    std::string getValue;
};

using StorageAsyncContextPtr = StorageAsyncContext *;

static void SetCallbackWork(napi_env env, StorageAsyncContextPtr asyncContext)
{
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "JSStartupSet", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void *data) {
            StorageAsyncContext *asyncContext = (StorageAsyncContext *)data;
            asyncContext->status = SetParameter(asyncContext->key, asyncContext->value);
            HiLog::Debug(LABEL,
                "JSApp set::asyncContext-> status = %{public}d, asyncContext->key = %{public}s, asyncContext->value = "
                "%{public}s.",
                asyncContext->status, asyncContext->key, asyncContext->value);
        },
        [](napi_env env, napi_status status, void *data) {
            StorageAsyncContext *asyncContext = (StorageAsyncContext *)data;
            napi_value result[ARGC_NUMBER] = { 0 };
            if (asyncContext->status == 0) {
                napi_get_undefined(env, &result[0]);
                napi_get_undefined(env, &result[1]);
            } else {
                napi_value value = nullptr;
                napi_create_object(env, &result[0]);
                napi_create_int32(env, asyncContext->status, &value);
                napi_set_named_property(env, result[0], "code", value);
                napi_get_undefined(env, &result[1]);
            }

            if (asyncContext->deferred) {
                if (asyncContext->status == 0) {
                    napi_resolve_deferred(env, asyncContext->deferred, result[1]);
                } else {
                    napi_reject_deferred(env, asyncContext->deferred, result[0]);
                }
            } else {
                napi_value callback = nullptr;
                napi_value callResult = nullptr;
                napi_get_reference_value(env, asyncContext->callbackRef, &callback);
                napi_call_function(env, nullptr, callback, ARGC_NUMBER, result, &callResult);
                napi_delete_reference(env, asyncContext->callbackRef);
            }
            napi_delete_async_work(env, asyncContext->work);
            delete asyncContext;
        },
        (void *)asyncContext, &asyncContext->work);
    napi_queue_async_work(env, asyncContext->work);
}

static napi_value Set(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_THREE_NUMBER;
    napi_value argv[ARGC_THREE_NUMBER] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    NAPI_ASSERT(env, argc >= ARGC_NUMBER, "requires 2 parameter");
    StorageAsyncContextPtr asyncContext = new StorageAsyncContext();
    asyncContext->env = env;
    for (size_t i = 0; i < argc; i++) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, argv[i], &valueType);

        if (i == 0 && valueType == napi_string) {
            napi_get_value_string_utf8(env, argv[i], asyncContext->key,
                BUF_LENGTH - 1, &asyncContext->keyLen);
        } else if (i == 1 && valueType == napi_string) {
            napi_get_value_string_utf8(env, argv[i], asyncContext->value,
                BUF_LENGTH - 1, &asyncContext->valueLen);
        } else if (i == ARGC_NUMBER && valueType == napi_function) {
            napi_create_reference(env, argv[i], 1, &asyncContext->callbackRef);
        } else {
            delete asyncContext;
            NAPI_ASSERT(env, false, "type mismatch");
        }
    }

    napi_value result = nullptr;
    if (asyncContext->callbackRef == nullptr) {
        napi_create_promise(env, &asyncContext->deferred, &result);
    } else {
        napi_get_undefined(env, &result);
    }

    SetCallbackWork(env, asyncContext);
    return result;
}

static napi_value SetSync(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_NUMBER;
    napi_value args[ARGC_NUMBER] = { nullptr };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    NAPI_ASSERT(env, argc == ARGC_NUMBER, "Wrong number of arguments");
    napi_valuetype valuetype0 = napi_null;
    NAPI_CALL(env, napi_typeof(env, args[0], &valuetype0));
    napi_valuetype valuetype1 = napi_null;
    NAPI_CALL(env, napi_typeof(env, args[1], &valuetype1));
    NAPI_ASSERT(env, valuetype0 == napi_string && valuetype1 == napi_string, "Wrong argument type. string expected.");

    char keyBuf[BUF_LENGTH] = { 0 };
    size_t keySize = 0;
    NAPI_CALL(env, napi_get_value_string_utf8(env, args[0], keyBuf, BUF_LENGTH - 1, &keySize));
    if (keySize >= MAX_LENGTH) {
        return nullptr;
    }

    char valueBuf[BUF_LENGTH] = { 0 };
    size_t valueSize = 0;
    NAPI_CALL(env, napi_get_value_string_utf8(env, args[1], valueBuf, BUF_LENGTH - 1, &valueSize));
    if (valueSize >= MAX_LENGTH) {
        return nullptr;
    }

    std::string keyStr = keyBuf;
    std::string valueStr = valueBuf;
    int setResult = SetParameter(keyStr.c_str(), valueStr.c_str());
    HiLog::Debug(LABEL, "JSApp SetSync::setResult = %{public}d, input keyBuf = %{public}s.", setResult, keyBuf);

    napi_value napiValue = nullptr;
    if (setResult != 0) { // set failed
        std::stringstream ss;
        ss << "set: " << keyStr << " failed, error code: " << setResult;
        napi_throw_error((env), nullptr, ss.str().c_str());
    } else {
        napi_get_undefined(env, &napiValue);
    }
    return napiValue;
}

static napi_value GetSync(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_NUMBER;
    napi_value args[ARGC_NUMBER] = { nullptr };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    NAPI_ASSERT(env, argc == 1 || argc == ARGC_NUMBER, "Wrong number of arguments");
    napi_valuetype valuetype0 = napi_null;
    NAPI_CALL(env, napi_typeof(env, args[0], &valuetype0));
    NAPI_ASSERT(env, valuetype0 == napi_string, "Wrong argument type. Numbers expected.");
    if (argc == ARGC_NUMBER) {
        napi_valuetype valuetype1 = napi_null;
        NAPI_CALL(env, napi_typeof(env, args[1], &valuetype1));
        NAPI_ASSERT(env, valuetype1 == napi_string, "Wrong argument type. string expected.");
    }

    char keyBuf[BUF_LENGTH] = { 0 };
    size_t keySize = 0;
    NAPI_CALL(env, napi_get_value_string_utf8(env, args[0], keyBuf, BUF_LENGTH - 1, &keySize));
    if (keySize >= MAX_LENGTH) {
        return nullptr;
    }

    std::string keyStr = keyBuf;
    std::string valueStr = "";
    std::string getValue = "";
    if (argc == ARGC_NUMBER) {
        char valueBuf[BUF_LENGTH] = { 0 };
        size_t valueSize = 0;
        NAPI_CALL(env, napi_get_value_string_utf8(env, args[1], valueBuf, BUF_LENGTH - 1, &valueSize));
        if (valueSize >= MAX_LENGTH) {
            return nullptr;
        }
        valueStr = valueBuf;
    }
    int ret = OHOS::system::GetStringParameter(keyStr, getValue, valueStr);
    HiLog::Debug(LABEL, "JSApp GetSync::getValue = %{public}s, input keyStr = %{public}s.", getValue.c_str(), keyBuf);

    napi_value napiValue = nullptr;
    if (ret == 0) {
        const char *value = getValue.c_str();
        NAPI_CALL(env, napi_create_string_utf8(env, value, strlen(value), &napiValue));
    }
    return napiValue;
}

static void GetCallbackWork(napi_env env, StorageAsyncContextPtr asyncContext)
{
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "JSStartupGet", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void *data) {
            StorageAsyncContext *asyncContext = (StorageAsyncContext *)data;
            asyncContext->status =
                OHOS::system::GetStringParameter(asyncContext->key, asyncContext->getValue, asyncContext->value);
            HiLog::Debug(LABEL,
                "JSApp get::asyncContext->status = %{public}d, asyncContext->getValue = %{public}s, asyncContext->key "
                "= %{public}s, value = %{public}s.",
                asyncContext->status, asyncContext->getValue.c_str(), asyncContext->key, asyncContext->value);
        },
        [](napi_env env, napi_status status, void *data) {
            StorageAsyncContext *asyncContext = (StorageAsyncContext *)data;
            napi_value result[ARGC_NUMBER] = { 0 };
            if (asyncContext->status == 0) {
                napi_get_undefined(env, &result[0]);
                napi_create_string_utf8(env, asyncContext->getValue.c_str(), strlen(asyncContext->getValue.c_str()),
                    &result[1]);
            } else {
                napi_value message = nullptr;
                napi_create_object(env, &result[0]);
                napi_create_int32(env, asyncContext->status, &message);
                napi_set_named_property(env, result[0], "code", message);
                napi_get_undefined(env, &result[1]);
            }

            if (asyncContext->deferred) {
                if (asyncContext->status == 0) {
                    napi_resolve_deferred(env, asyncContext->deferred, result[1]);
                } else {
                    napi_reject_deferred(env, asyncContext->deferred, result[0]);
                }
            } else {
                napi_value callback = nullptr;
                napi_value callResult = nullptr;
                napi_get_reference_value(env, asyncContext->callbackRef, &callback);
                napi_call_function(env, nullptr, callback, ARGC_NUMBER, result, &callResult);
                napi_delete_reference(env, asyncContext->callbackRef);
            }
            napi_delete_async_work(env, asyncContext->work);
            delete asyncContext;
        },
        (void *)asyncContext, &asyncContext->work);
    napi_queue_async_work(env, asyncContext->work);
}

static napi_value Get(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_THREE_NUMBER;
    napi_value argv[ARGC_THREE_NUMBER] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    NAPI_ASSERT(env, argc >= 1, "requires 1 parameter");
    StorageAsyncContextPtr asyncContext = new StorageAsyncContext();
    asyncContext->env = env;
    for (size_t i = 0; i < argc; i++) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, argv[i], &valueType);

        if (i == 0 && valueType == napi_string) {
            napi_get_value_string_utf8(env, argv[i], asyncContext->key,
                BUF_LENGTH - 1, &asyncContext->keyLen);
        } else if (i == 1 && valueType == napi_string) {
            napi_get_value_string_utf8(env, argv[i], asyncContext->value,
                BUF_LENGTH - 1, &asyncContext->valueLen);
        } else if (i == 1 && valueType == napi_function) {
            napi_create_reference(env, argv[i], 1, &asyncContext->callbackRef);
            break;
        } else if (i == ARGC_NUMBER && valueType == napi_function) {
            napi_create_reference(env, argv[i], 1, &asyncContext->callbackRef);
        } else {
            delete asyncContext;
            NAPI_ASSERT(env, false, "type mismatch");
        }
    }

    napi_value result = nullptr;
    if (asyncContext->callbackRef == nullptr) {
        napi_create_promise(env, &asyncContext->deferred, &result);
    } else {
        napi_get_undefined(env, &result);
    }

    GetCallbackWork(env, asyncContext);
    return result;
}

EXTERN_C_START
/*
 * Module init
 */
static napi_value Init(napi_env env, napi_value exports)
{
    /*
     * Attribute definition
     */
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("set", Set),
        DECLARE_NAPI_FUNCTION("setSync", SetSync),
        DECLARE_NAPI_FUNCTION("get", Get),
        DECLARE_NAPI_FUNCTION("getSync", GetSync),
        DECLARE_NAPI_FUNCTION("wait", ParamWait),
        DECLARE_NAPI_FUNCTION("getWatcher", GetWatcher)
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(napi_property_descriptor), desc));
    return RegisterWatcher(env, exports);
}
EXTERN_C_END

/*
 * Module definition
 */
static napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = NULL,
    .nm_register_func = Init,
    .nm_modname = "systemParameter",
    .nm_priv = ((void *)0),
    .reserved = { 0 }
};

/*
 * Module registration function
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&_module);
}
