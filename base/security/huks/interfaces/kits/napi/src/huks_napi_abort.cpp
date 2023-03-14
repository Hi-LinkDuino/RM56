/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "huks_napi_abort.h"

#include "securec.h"

#include "hks_api.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "hks_type.h"
#include "huks_napi_common.h"

namespace HuksNapi {
namespace {
constexpr int HUKS_NAPI_ABORT_MIN_ARGS = 2;
constexpr int HUKS_NAPI_ABORT_MAX_ARGS = 3;
}  // namespace

struct AbortAsyncContextT {
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callback = nullptr;

    int32_t result = 0;
    struct HksBlob *handle = nullptr;
    struct HksParamSet *paramSet = nullptr;
};
using AbortAsyncContext = AbortAsyncContextT *;

static AbortAsyncContext CreateAbortAsyncContext()
{
    AbortAsyncContext context = (AbortAsyncContext)HksMalloc(sizeof(AbortAsyncContextT));
    if (context != nullptr) {
        (void)memset_s(context, sizeof(AbortAsyncContextT), 0, sizeof(AbortAsyncContextT));
    }
    return context;
}

static void DeleteAbortAsyncContext(napi_env env, AbortAsyncContext &context)
{
    if (context == nullptr) {
        return;
    }

    if (context->asyncWork != nullptr) {
        napi_delete_async_work(env, context->asyncWork);
        context->asyncWork = nullptr;
    }

    if (context->callback != nullptr) {
        napi_delete_reference(env, context->callback);
        context->callback = nullptr;
    }

    if (context->handle != nullptr) {
        FreeHksBlob(context->handle);
    }

    if (context->paramSet != nullptr) {
        HksFreeParamSet(&context->paramSet);
    }

    HksFree(context);
    context = nullptr;
}

static napi_value GetHandleValue(napi_env env, napi_value object, AbortAsyncContext context)
{
    uint64_t handle = 0;
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_typeof(env, object, &valueType);
    if (valueType != napi_valuetype::napi_object) {
        napi_throw_type_error(env, nullptr, "Parameter type does not match");
        return nullptr;
    }

    uint32_t handle1 = 0;
    uint32_t handle2 = 0;
    napi_value handlejs1;
    napi_value handlejs2;
    napi_get_named_property(env, object, HKS_HANDLE_PROPERTY_HANDLE_1.c_str(), &handlejs1);
    napi_status status = napi_get_value_uint32(env, handlejs1, &handle1);
    if (status != napi_ok) {
        HKS_LOG_E("Retrieve field failed");
        return nullptr;
    }

    napi_get_named_property(env, object, HKS_HANDLE_PROPERTY_HANDLE_2.c_str(), &handlejs2);
    status = napi_get_value_uint32(env, handlejs2, &handle2);
    if (status != napi_ok) {
        HKS_LOG_E("Retrieve field failed");
        return nullptr;
    }

    handle = handle1;
    handle = (handle << HKS_HANDLE_OFFSET32) | handle2;

    context->handle = (HksBlob *)HksMalloc(sizeof(HksBlob));
    if (context->handle == nullptr) {
        HKS_LOG_E("could not alloc memory");
        return nullptr;
    }
    context->handle->size = sizeof(uint64_t);
    context->handle->data = (uint8_t *)HksMalloc(sizeof(uint64_t));
    if (context->handle->data == nullptr) {
        HKS_LOG_E("could not alloc memory");
        return nullptr;
    }
    (void)memcpy_s(context->handle->data, sizeof(uint64_t), &handle, sizeof(uint64_t));

    return GetInt32(env, 0);
}

static napi_value ParseAbortParams(napi_env env, napi_callback_info info, AbortAsyncContext context)
{
    size_t argc = HUKS_NAPI_ABORT_MAX_ARGS;
    napi_value argv[HUKS_NAPI_ABORT_MAX_ARGS] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    if (argc < HUKS_NAPI_ABORT_MIN_ARGS) {
        napi_throw_error(env, NULL, "invalid arguments");
        HKS_LOG_E("no enough params");
        return nullptr;
    }

    size_t index = 0;

    napi_value result = GetHandleValue(env, argv[index], context);
    if (result == nullptr) {
        HKS_LOG_E("could not get handle value");
        return nullptr;
    }

    index++;
    napi_value properties = nullptr;
    napi_status status = napi_get_named_property(env, argv[index],
        HKS_OPTIONS_PROPERTY_PROPERTIES.c_str(), &properties);
    if (status != napi_ok || properties == nullptr) {
        GET_AND_THROW_LAST_ERROR((env));
        HKS_LOG_E("could not get property %s", HKS_OPTIONS_PROPERTY_PROPERTIES.c_str());
        return nullptr;
    }
    result = ParseHksParamSet(env, properties, context->paramSet);
    if (result == nullptr) {
        HKS_LOG_E("could not get paramset");
        return nullptr;
    }

    index++;
    if (index < argc) {
        context->callback = GetCallback(env, argv[index]);
    }

    return GetInt32(env, 0);
}

static napi_value AbortWriteResult(napi_env env, AbortAsyncContext context)
{
    return GenerateHksResult(env, context->result, nullptr, 0);
}

static napi_value AbortAsyncWork(napi_env env, AbortAsyncContext context)
{
    napi_value promise = nullptr;
    if (context->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &context->deferred, &promise));
    }

    napi_value resourceName;
    napi_create_string_latin1(env, "AbortAsyncWork", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AbortAsyncContext context = static_cast<AbortAsyncContext>(data);
            context->result = HksAbort(context->handle, context->paramSet);
        },
        [](napi_env env, napi_status status, void *data) {
            AbortAsyncContext context = static_cast<AbortAsyncContext>(data);
            napi_value result = AbortWriteResult(env, context);

            if (context->callback != nullptr) {
                CallAsyncCallback(env, context->callback, context->result, result);
            } else {
                napi_resolve_deferred(env, context->deferred, result);
            }
            DeleteAbortAsyncContext(env, context);
        },
        (void *)context,
        &context->asyncWork);

    napi_status status = napi_queue_async_work(env, context->asyncWork);
    if (status != napi_ok) {
        GET_AND_THROW_LAST_ERROR((env));
        DeleteAbortAsyncContext(env, context);
        HKS_LOG_E("could not queue async work");
        return nullptr;
    }

    if (context->callback == nullptr) {
        return promise;
    } else {
        return GetNull(env);
    }
}

napi_value HuksNapiAbort(napi_env env, napi_callback_info info)
{
    AbortAsyncContext context = CreateAbortAsyncContext();
    if (context == nullptr) {
        HKS_LOG_E("could not create context");
        return nullptr;
    }

    napi_value result = ParseAbortParams(env, info, context);
    if (result == nullptr) {
        HKS_LOG_E("could not parse params");
        DeleteAbortAsyncContext(env, context);
        return nullptr;
    }

    result = AbortAsyncWork(env, context);
    if (result == nullptr) {
        HKS_LOG_E("could not start async work");
        DeleteAbortAsyncContext(env, context);
        return nullptr;
    }
    return result;
}
}  // namespace HuksNapi
