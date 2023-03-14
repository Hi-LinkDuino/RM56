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

#include <functional>
#include <vector>
#include "native_parameters_js.h"
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "StartupParametersJs" };
using namespace OHOS::HiviewDFX;
using namespace OHOS::system;
static constexpr int ARGC_NUMBER = 2;
static constexpr int BUF_LENGTH = 128;

static napi_ref g_paramWatchRef;

using ParamAsyncContext = struct {
    napi_env env = nullptr;
    napi_async_work work = nullptr;

    char key[BUF_LENGTH] = { 0 };
    size_t keyLen = 0;
    char value[BUF_LENGTH] = { 0 };
    size_t valueLen = 0;
    int32_t timeout = 0;
    napi_deferred deferred = nullptr;
    napi_ref callbackRef = nullptr;

    int status = -1;
    std::string getValue;
};

using ParamWatcher = struct {
    napi_env env = nullptr;
    napi_ref thisVarRef = nullptr;
    char keyPrefix[BUF_LENGTH] = { 0 };
    size_t keyLen = 0;
    bool notifySwitch = false;
    bool startWatch = false;
    std::mutex mutex {};
    napi_ref currCallbackRef = nullptr;
    std::map<uint32_t, napi_ref> callbackReferences {};
};

using ParamWatcherWork = struct {
    napi_async_work work = nullptr;
    ParamWatcher *watcher = nullptr;
    bool startWatch = false;
};

using ParamAsyncContextPtr = ParamAsyncContext *;
using ParamWatcherPtr = ParamWatcher *;

static napi_value NapiGetNull(napi_env env)
{
    napi_value result = 0;
    napi_get_null(env, &result);
    return result;
}

static napi_value GetNapiValue(napi_env env, int val)
{
    napi_value result = nullptr;
    napi_create_int32(env, val, &result);
    return result;
}

static int GetParamValue(napi_env env, napi_value arg, napi_valuetype valueType, char *buffer, size_t &buffLen)
{
    napi_valuetype type = napi_null;
    napi_typeof(env, arg, &type);
    PARAM_JS_CHECK(type == valueType, return -1, "Invalid type %d %d", type, valueType);
    napi_status status = napi_ok;
    if (valueType == napi_string) {
        status = napi_get_value_string_utf8(env, arg, buffer, buffLen, &buffLen);
    } else if (valueType == napi_number) {
        status = napi_get_value_int32(env, arg, (int *)buffer);
    }
    return status;
}

static void WaitCallbackWork(napi_env env, ParamAsyncContextPtr asyncContext)
{
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "JSStartupGet", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void *data) {
            ParamAsyncContext *asyncContext = (ParamAsyncContext *)data;
            asyncContext->status = WaitParameter(asyncContext->key, asyncContext->value, asyncContext->timeout);
            HiLog::Debug(LABEL, "JSApp Wait status: %{public}d, key: %{public}s",
                asyncContext->status, asyncContext->key);
        },
        [](napi_env env, napi_status status, void *data) {
            ParamAsyncContext *asyncContext = (ParamAsyncContext *)data;
            napi_value result[ARGC_NUMBER] = { 0 };
            napi_value message = nullptr;
            napi_create_object(env, &result[0]);
            napi_create_int32(env, asyncContext->status, &message);
            napi_set_named_property(env, result[0], "code", message);
            napi_get_undefined(env, &result[1]); // only one param

            HiLog::Debug(LABEL, "JSApp Wait status: %{public}d, key: %{public}s ",
                asyncContext->status, asyncContext->key);
            if (asyncContext->deferred) {
                if (asyncContext->status == 0) {
                    napi_resolve_deferred(env, asyncContext->deferred, result[1]);
                } else {
                    napi_reject_deferred(env, asyncContext->deferred, result[0]);
                }
            } else {
                napi_value callbackRef = nullptr;
                napi_value callResult = nullptr;
                napi_status status = napi_get_reference_value(env, asyncContext->callbackRef, &callbackRef);
                PARAM_JS_CHECK(status == 0 && callbackRef != nullptr, return, "Failed to get reference ");
                napi_value undefined;
                napi_get_undefined(env, &undefined);
                napi_call_function(env, undefined, callbackRef, ARGC_NUMBER, result, &callResult);
                napi_delete_reference(env, asyncContext->callbackRef);
            }
            napi_delete_async_work(env, asyncContext->work);
            delete asyncContext;
        },
        (void *)asyncContext, &asyncContext->work);
    napi_queue_async_work(env, asyncContext->work);
}

napi_value ParamWait(napi_env env, napi_callback_info info)
{
    constexpr int PARAM_TIMEOUT_INDEX = 2;
    constexpr int ARGC_THREE_NUMBER = 3;
    size_t argc = ARGC_THREE_NUMBER + 1;
    napi_value argv[ARGC_THREE_NUMBER + 1];
    napi_value thisVar = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    PARAM_JS_CHECK(status == napi_ok, return GetNapiValue(env, status), "Failed to get cb info");
    PARAM_JS_CHECK(argc >= ARGC_THREE_NUMBER, return GetNapiValue(env, status), "Failed to get argc");

    ParamAsyncContextPtr asyncContext = new ParamAsyncContext();
    PARAM_JS_CHECK(asyncContext != nullptr, return GetNapiValue(env, status), "Failed to create context");
    asyncContext->env = env;

    // get param key
    asyncContext->keyLen = BUF_LENGTH - 1;
    asyncContext->valueLen = BUF_LENGTH - 1;
    size_t len = sizeof(asyncContext->timeout);
    int ret = GetParamValue(env, argv[0], napi_string, asyncContext->key, asyncContext->keyLen);
    PARAM_JS_CHECK(ret == 0, delete asyncContext;
        return GetNapiValue(env, ret), "Invalid param for wait");
    ret = GetParamValue(env, argv[1], napi_string, asyncContext->value, asyncContext->valueLen);
    PARAM_JS_CHECK(ret == 0, delete asyncContext;
        return GetNapiValue(env, ret), "Invalid param for wait");
    ret = GetParamValue(env, argv[PARAM_TIMEOUT_INDEX], napi_number, (char *)&asyncContext->timeout, len);
    PARAM_JS_CHECK(ret == 0, delete asyncContext;
        return GetNapiValue(env, ret), "Invalid param for wait");
    if (argc > ARGC_THREE_NUMBER) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, argv[ARGC_THREE_NUMBER], &valueType);
        PARAM_JS_CHECK(valueType == napi_function, delete asyncContext;
            return GetNapiValue(env, ret), "Invalid param for wait callbackRef");
        napi_create_reference(env, argv[ARGC_THREE_NUMBER], 1, &asyncContext->callbackRef);
    }
    HiLog::Debug(LABEL, "JSApp Wait key: %{public}s, value: %{public}s timeout %{public}d.",
        asyncContext->key, asyncContext->value, asyncContext->timeout);

    napi_value result = nullptr;
    if (asyncContext->callbackRef == nullptr) {
        napi_create_promise(env, &asyncContext->deferred, &result);
    } else {
        result = GetNapiValue(env, 0);
    }
    WaitCallbackWork(env, asyncContext);
    return result;
}

static bool GetFristRefence(ParamWatcherPtr watcher, uint32_t &next)
{
    std::lock_guard<std::mutex> lock(watcher->mutex);
    auto iter = watcher->callbackReferences.begin();
    if (iter != watcher->callbackReferences.end()) {
        next = watcher->callbackReferences.begin()->first;
        return true;
    }
    return false;
}

static napi_ref GetWatcherReference(ParamWatcherPtr watcher, uint32_t next)
{
    std::lock_guard<std::mutex> lock(watcher->mutex);
    auto iter = watcher->callbackReferences.find(next);
    if (iter != watcher->callbackReferences.end()) {
        return iter->second;
    }
    return nullptr;
}

static uint32_t GetNextRefence(ParamWatcherPtr watcher, uint32_t &next)
{
    std::lock_guard<std::mutex> lock(watcher->mutex);
    auto iter = watcher->callbackReferences.upper_bound(next);
    if (iter == watcher->callbackReferences.end()) {
        return false;
    }
    next = iter->first;
    return true;
}

static void AddWatcherCallback(ParamWatcherPtr watcher, napi_ref callbackRef)
{
    static uint32_t watcherId = 0;
    std::lock_guard<std::mutex> lock(watcher->mutex);
    watcherId++;
    watcher->callbackReferences[watcherId] = callbackRef;
    HiLog::Debug(LABEL, "JSApp watcher add watcher callback %{public}s %{public}u.",
        watcher->keyPrefix, watcherId);
}

static void DelWatcherCallback(ParamWatcherPtr watcher, uint32_t next)
{
    HiLog::Debug(LABEL, "JSApp watcher key %{public}s delete callack %{public}u", watcher->keyPrefix, next);
    std::lock_guard<std::mutex> lock(watcher->mutex);
    watcher->callbackReferences.erase(next);
}

static void DelCallbackRef(napi_env env, ParamWatcherPtr watcher, napi_ref callbackRef, uint32_t id)
{
    std::lock_guard<std::mutex> lock(watcher->mutex);
    if (callbackRef == watcher->currCallbackRef) {
        HiLog::Debug(LABEL, "JSApp watcher key %{public}s has been callbacked %{public}u.",
            watcher->keyPrefix, id);
        watcher->currCallbackRef = nullptr;
    } else {
        napi_delete_reference(env, callbackRef);
    }
    watcher->callbackReferences.erase(id);
}

static void DelCallback(napi_env env, napi_value callback, ParamWatcherPtr watcher)
{
    bool isEquals = false;
    uint32_t next = 0;
    bool ret = GetFristRefence(watcher, next);
    while (ret) {
        napi_ref callbackRef = GetWatcherReference(watcher, next);
        if (callbackRef == nullptr) {
            HiLog::Debug(LABEL, "JSApp watcher key %{public}s callbackRef has been deleted %{public}d.",
                watcher->keyPrefix, next);
            DelWatcherCallback(watcher, next);
        } else if (callback != nullptr) {
            napi_value handler = nullptr;
            napi_get_reference_value(env, callbackRef, &handler);
            napi_strict_equals(env, handler, callback, &isEquals);
            if (isEquals) {
                DelCallbackRef(env, watcher, callbackRef, next);
                break;
            }
        } else {
            DelCallbackRef(env, watcher, callbackRef, next);
        }
        ret = GetNextRefence(watcher, next);
    }
}

static bool CheckCallbackEqual(napi_env env, napi_value callback, ParamWatcherPtr watcher)
{
    bool isEquals = false;
    uint32_t next = 0;
    bool ret = GetFristRefence(watcher, next);
    while (ret) {
        napi_ref callbackRef = GetWatcherReference(watcher, next);
        if (callbackRef != nullptr) {
            napi_value handler = nullptr;
            napi_get_reference_value(env, callbackRef, &handler);
            napi_strict_equals(env, handler, callback, &isEquals);
            if (isEquals) {
                return true;
            }
        }
        ret = GetNextRefence(watcher, next);
    }
    return false;
}

static napi_value ParamWatchConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    ParamWatcherPtr watcher = new ParamWatcher();
    PARAM_JS_CHECK(watcher != nullptr, return NapiGetNull(env), "Failed to create param watcher");
    napi_status status = napi_create_reference(env, thisVar, 1, &watcher->thisVarRef);
    PARAM_JS_CHECK(status == 0, delete watcher;
        return NapiGetNull(env), "Failed to create reference %d", status);

    napi_wrap(
        env, thisVar, watcher,
        [](napi_env env, void *data, void *hint) {
            ParamWatcherPtr watcher = static_cast<ParamWatcherPtr>(data);
            if (watcher) {
                DelCallback(env, nullptr, watcher);
                WatchParameter(watcher->keyPrefix, nullptr, nullptr);
                delete watcher;
                watcher = nullptr;
            }
        },
        nullptr, nullptr);
    return thisVar;
}

napi_value GetWatcher(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));

    napi_value obj = thisVar;
    ParamWatcherPtr watcher = nullptr;
    napi_unwrap(env, thisVar, (void **)&watcher);
    if (watcher == nullptr) { // check if watcher exist
        napi_value constructor = nullptr;
        int status = napi_get_reference_value(env, g_paramWatchRef, &constructor);
        PARAM_JS_CHECK(status == 0, return NapiGetNull(env), "Failed to get reference");
        status = napi_new_instance(env, constructor, 0, nullptr, &obj);
        PARAM_JS_CHECK(status == 0, return NapiGetNull(env), "Failed to create instance for watcher");
        napi_unwrap(env, obj, (void **)&watcher);
    }
    if (watcher != nullptr) {
        watcher->keyLen = BUF_LENGTH;
        int ret = GetParamValue(env, argv[0], napi_string, watcher->keyPrefix, watcher->keyLen);
        PARAM_JS_CHECK(ret == 0, return NapiGetNull(env), "Failed to get key prefix");
        HiLog::Debug(LABEL, "JSApp watcher keyPrefix = %{public}s ", watcher->keyPrefix);
        ret = WatchParameter(watcher->keyPrefix, nullptr, watcher);
        PARAM_JS_CHECK(ret == 0, return NapiGetNull(env), "Failed to get watcher ret %{public}d", ret);
    }
    return obj;
}

static ParamWatcherPtr GetWatcherInfo(napi_env env, napi_callback_info info, napi_value *callback)
{
    size_t argc = ARGC_NUMBER;
    napi_value argv[ARGC_NUMBER];
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    size_t typeLen = BUF_LENGTH - 1;
    std::vector<char> eventType(typeLen, 0);
    int ret = GetParamValue(env, argv[0], napi_string, eventType.data(), typeLen);
    PARAM_JS_CHECK(ret == 0, return nullptr, "Failed to get event type");
    if (strcmp(eventType.data(), "valueChange") != 0) {
        return nullptr;
    }
    // argv[1]:callbackRef
    if (argc > 1) {
        napi_valuetype valuetype;
        napi_status status = napi_typeof(env, argv[1], &valuetype);
        PARAM_JS_CHECK(status == 0, return nullptr, "Failed to get type");
        PARAM_JS_CHECK(valuetype == napi_function, return nullptr, "Invalid type %d", valuetype);
        *callback = argv[1];
    }
    ParamWatcherPtr watcher = nullptr;
    napi_unwrap(env, thisVar, (void **)&watcher);
    return watcher;
}

static void NotifyValueChange(ParamWatcherPtr watcher, uint32_t id, napi_value thisVar, const napi_value result[])
{
    napi_ref callbackRef = GetWatcherReference(watcher, id);
    PARAM_JS_CHECK(callbackRef != nullptr, return,
        "Failed to get callback for %{public}s %{public}d", watcher->keyPrefix, id);
    napi_value callbackFunc = nullptr;
    napi_status status = napi_get_reference_value(watcher->env, callbackRef, &callbackFunc);
    PARAM_JS_CHECK(status == 0 && callbackFunc != nullptr, return,
        "Failed to get callback for %{public}s %{public}d", watcher->keyPrefix, id);
    {
        std::lock_guard<std::mutex> lock(watcher->mutex);
        watcher->currCallbackRef = callbackRef;
    }

    napi_value callbackResult = nullptr;
    napi_call_function(watcher->env, thisVar, callbackFunc, ARGC_NUMBER, result, &callbackResult);

    {
        std::lock_guard<std::mutex> lock(watcher->mutex);
        if (watcher->currCallbackRef == nullptr) {
            HiLog::Debug(LABEL, "JSApp watcher notify key %{public}s callback deleted watcherId %{public}u",
                watcher->keyPrefix, id);
            napi_delete_reference(watcher->env, callbackRef);
        }
        watcher->currCallbackRef = nullptr;
    }
}

static void ProcessParamChange(const char *key, const char *value, void *context)
{
    ParamWatcherPtr watcher = static_cast<ParamWatcherPtr>(context);
    PARAM_JS_CHECK(watcher != nullptr && watcher->env != nullptr, return, "Invalid param");
    PARAM_JS_CHECK(watcher->callbackReferences.size() > 0, return, "No callback for watcher");

    napi_handle_scope scope = nullptr;
    napi_status status = napi_open_handle_scope(watcher->env, &scope);
    PARAM_JS_CHECK(status == 0, return, "Failed to get reference ");
    napi_value result[ARGC_NUMBER] = { 0 };
    napi_create_string_utf8(watcher->env, key, strlen(key), &result[0]);
    napi_create_string_utf8(watcher->env, value, strlen(value), &result[1]);
    napi_value thisVar = nullptr;
    status = napi_get_reference_value(watcher->env, watcher->thisVarRef, &thisVar);
    PARAM_JS_CHECK(status == 0 && thisVar != nullptr, napi_close_handle_scope(watcher->env, scope);
        return, "Failed to get reference ");
    uint32_t next = 0;
    bool ret = GetFristRefence(watcher, next);
    while (ret) {
        NotifyValueChange(watcher, next, thisVar, result);
        ret = GetNextRefence(watcher, next);
    }
    napi_close_handle_scope(watcher->env, scope);
    HiLog::Debug(LABEL, "JSApp watcher ProcessParamChange %{public}s finish", key);
}

static void WatchCallbackWork(napi_env env, ParamWatcherPtr watcher)
{
    HiLog::Debug(LABEL, "JSApp WatchCallbackWork key: %{public}s", watcher->keyPrefix);
    ParamWatcherWork *worker = new ParamWatcherWork();
    PARAM_JS_CHECK(worker != nullptr, return, "Failed to create worker ");
    worker->watcher = watcher;
    worker->work = nullptr;
    worker->startWatch = watcher->startWatch;

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "JSStartupWatch", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource,
        [](napi_env env, void *data) {
            ParamWatcherWork *worker = (ParamWatcherWork *)data;
            PARAM_JS_CHECK(worker != nullptr && worker->watcher != nullptr, return, "Invalid worker ");
            int status = WatchParameter(worker->watcher->keyPrefix,
                worker->startWatch ? ProcessParamChange : nullptr, worker->watcher);
            HiLog::Debug(LABEL, "JSApp WatchCallbackWork %{public}s status: %{public}d, key: %{public}s",
                worker->startWatch ? "on" : "off", status, worker->watcher->keyPrefix);
        },
        [](napi_env env, napi_status status, void *data) {
            ParamWatcherWork *worker = (ParamWatcherWork *)data;
            HiLog::Debug(LABEL, "JSApp WatchCallbackWork delete %{public}s key: %{public}s",
                worker->startWatch ? "on" : "off", worker->watcher->keyPrefix);
            napi_delete_async_work(env, worker->work);
            delete worker;
        },
        (void *)worker, &worker->work);
    napi_queue_async_work(env, worker->work);
}

static napi_value SwithWatchOn(napi_env env, napi_callback_info info)
{
    napi_value callback = nullptr;
    ParamWatcherPtr watcher = GetWatcherInfo(env, info, &callback);
    PARAM_JS_CHECK(watcher != nullptr, return GetNapiValue(env, -1), "Failed to get watcher swith param");

    if (CheckCallbackEqual(env, callback, watcher)) {
        HiLog::Warn(LABEL, "JSApp watcher repeater switch on %{public}s", watcher->keyPrefix);
        return 0;
    }
    HiLog::Debug(LABEL, "JSApp watcher on %{public}s", watcher->keyPrefix);
    // save callback
    napi_ref callbackRef;
    napi_create_reference(env, callback, 1, &callbackRef);
    AddWatcherCallback(watcher, callbackRef);
    watcher->env = env;
    {
        std::lock_guard<std::mutex> lock(watcher->mutex);
        if (watcher->startWatch) {
            return GetNapiValue(env, 0);
        }
        watcher->startWatch = true;
    }

    HiLog::Debug(LABEL, "JSApp watcher add %{public}s", watcher->keyPrefix);
    WatchCallbackWork(env, watcher);
    HiLog::Debug(LABEL, "JSApp watcher on %{public}s finish", watcher->keyPrefix);
    return GetNapiValue(env, 0);
}

static napi_value SwithWatchOff(napi_env env, napi_callback_info info)
{
    napi_value callback = nullptr;
    ParamWatcherPtr watcher = GetWatcherInfo(env, info, &callback);
    PARAM_JS_CHECK(watcher != nullptr, return GetNapiValue(env, -1), "Failed to get watcher");
    HiLog::Debug(LABEL, "JSApp watcher off %{public}s", watcher->keyPrefix);
    DelCallback(env, callback, watcher);
    {
        std::lock_guard<std::mutex> lock(watcher->mutex);
        if (watcher->callbackReferences.size() == 0) {
            watcher->startWatch = false;
            WatchCallbackWork(env, watcher);
        }
    }
    return GetNapiValue(env, 0);
}

napi_value RegisterWatcher(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("on", SwithWatchOn),
        DECLARE_NAPI_FUNCTION("off", SwithWatchOff),
    };

    napi_value result = nullptr;
    NAPI_CALL(env,
        napi_define_class(env,
            "paramWatcher",
            NAPI_AUTO_LENGTH,
            ParamWatchConstructor,
            nullptr,
            sizeof(properties) / sizeof(*properties),
            properties,
            &result));
    napi_set_named_property(env, exports, "paramWatcher", result);
    napi_create_reference(env, result, 1, &g_paramWatchRef);
    return exports;
}