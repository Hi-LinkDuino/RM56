/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "huks_napi_finish.h"

#include "securec.h"

#include "hks_api.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "hks_type.h"
#include "huks_napi_common.h"

namespace HuksNapi {
namespace {
constexpr int HUKS_NAPI_FINISH_MIN_ARGS = 2;
constexpr int HUKS_NAPI_FINISH_MAX_ARGS = 3;
}  // namespace

struct FinishAsyncContext {
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callback = nullptr;

    int32_t result = 0;
    struct HksBlob *handle = nullptr;
    struct HksParamSet *paramSet = nullptr;
    struct HksBlob *inData = nullptr;
    struct HksBlob *outData = nullptr;
};
using FinishAsyncCtxPtr = FinishAsyncContext *;

static FinishAsyncCtxPtr CreateFinishAsyncContext()
{
    FinishAsyncCtxPtr context = (FinishAsyncCtxPtr)HksMalloc(sizeof(FinishAsyncContext));
    if (context != nullptr) {
        (void)memset_s(context, sizeof(FinishAsyncContext), 0, sizeof(FinishAsyncContext));
    }
    return context;
}

static void DeleteFinishAsyncContext(napi_env env, FinishAsyncCtxPtr &context)
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

    if (context->inData != nullptr) {
        if (context->inData->data != nullptr && context->inData->size != 0) {
            (void)memset_s(context->inData->data, context->inData->size, 0, context->inData->size);
        }
        FreeHksBlob(context->inData);
    }

    if (context->outData != nullptr) {
        if (context->outData->data != nullptr && context->outData->size != 0) {
            (void)memset_s(context->outData->data, context->outData->size, 0, context->outData->size);
        }
        FreeHksBlob(context->outData);
    }

    if (context->paramSet != nullptr) {
        HksFreeParamSet(&context->paramSet);
    }

    HksFree(context);
    context = nullptr;
}

static napi_value GetHandleValue(napi_env env, napi_value object, FinishAsyncCtxPtr context)
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
        HKS_LOG_E("malloc failed");
        return nullptr;
    }
    (void)memcpy_s(context->handle->data, sizeof(uint64_t), &handle, sizeof(uint64_t));

    return GetInt32(env, 0);
}

static bool ParseFinishInParam(napi_env env, napi_value *argv, FinishAsyncCtxPtr context, size_t index)
{
    napi_value inData = nullptr;
    napi_status status = napi_get_named_property(env, argv[index], HKS_OPTIONS_PROPERTY_INDATA.c_str(), &inData);
    if (status != napi_ok || inData == nullptr) {
        GET_AND_THROW_LAST_ERROR((env));
        HKS_LOG_E("could not get property %s", HKS_OPTIONS_PROPERTY_INDATA.c_str());
        return false;
    }

    napi_value result = GetUint8Array(env, inData, *context->inData);
    if (result == nullptr) {
        HKS_LOG_E("could not get indata");
        return false;
    }

    return true;
}

static bool ParseFinishOutParam(napi_env env, napi_value *argv, FinishAsyncCtxPtr context, size_t index)
{
    napi_value outData = nullptr;
    bool hasResult = false;
    napi_has_named_property(env, argv[index], HKS_OPTIONS_PROPERTY_OUTDATA.c_str(), &hasResult);
    napi_status status = napi_get_named_property(env, argv[index], HKS_OPTIONS_PROPERTY_OUTDATA.c_str(), &outData);
    if (status == napi_ok && outData != nullptr && hasResult) {
        if (context->outData == nullptr) {
            HKS_LOG_E("could not alloc memory");
            return false;
        }
        napi_value result = GetUint8Array(env, outData, *context->outData);
        if (result == nullptr) {
            HKS_LOG_E("could not get outdata");
            return false;
        }
    } else {
        context->outData->size = context->inData->size + DATA_SIZE_64KB;
        context->outData->data = (uint8_t *)HksMalloc(context->outData->size + DATA_SIZE_64KB);
        if (context->outData->data == nullptr) {
            HKS_LOG_E("could not alloc memory");
            return false;
        }
    }

    return true;
}

static napi_value ParseFinishParams(napi_env env, napi_callback_info info, FinishAsyncCtxPtr context)
{
    size_t argc = HUKS_NAPI_FINISH_MAX_ARGS;
    napi_value argv[HUKS_NAPI_FINISH_MAX_ARGS] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    if (argc < HUKS_NAPI_FINISH_MIN_ARGS) {
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

    context->outData = (HksBlob *)HksMalloc(sizeof(HksBlob));
    if (context->outData == nullptr) {
        HKS_LOG_E("could not alloc memory");
        return nullptr;
    }

    context->inData = (HksBlob *)HksMalloc(sizeof(HksBlob));
    if (context->inData == nullptr) {
        HKS_LOG_E("could not alloc memory");
        return nullptr;
    }
    (void)memset_s(context->outData, sizeof(HksBlob), 0, sizeof(HksBlob));
    (void)memset_s(context->inData, sizeof(HksBlob), 0, sizeof(HksBlob));

    if (!ParseFinishInParam(env, argv, context, index) ||
        !ParseFinishOutParam(env, argv, context, index)) {
        HKS_LOG_E("ParseFinishInParam failed");
        return nullptr;
    }

    if (++index < argc) {
        context->callback = GetCallback(env, argv[index]);
    }

    return GetInt32(env, 0);
}

static napi_value FinishWriteResult(napi_env env, FinishAsyncCtxPtr context)
{
    return GenerateHksResult(env,
        context->result,
        ((context->result == HKS_SUCCESS && context->outData != nullptr) ? context->outData->data : nullptr),
        (context->result == HKS_SUCCESS && context->outData != nullptr) ? context->outData->size : 0);
}

static napi_value FinishAsyncWork(napi_env env, FinishAsyncCtxPtr context)
{
    napi_value promise = nullptr;
    if (context->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &context->deferred, &promise));
    }

    napi_value resourceName;
    napi_create_string_latin1(env, "FinishAsyncWork", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            FinishAsyncCtxPtr context = static_cast<FinishAsyncCtxPtr>(data);
            context->result = HksFinish(context->handle, context->paramSet, context->inData, context->outData);
        },
        [](napi_env env, napi_status status, void *data) {
            FinishAsyncCtxPtr context = static_cast<FinishAsyncCtxPtr>(data);
            napi_value result = FinishWriteResult(env, context);
            if (context->callback != nullptr) {
                CallAsyncCallback(env, context->callback, context->result, result);
            } else {
                napi_resolve_deferred(env, context->deferred, result);
            }
            DeleteFinishAsyncContext(env, context);
        },
        (void *)context,
        &context->asyncWork);

    napi_status status = napi_queue_async_work(env, context->asyncWork);
    if (status != napi_ok) {
        GET_AND_THROW_LAST_ERROR((env));
        DeleteFinishAsyncContext(env, context);
        HKS_LOG_E("could not queue async work");
        return nullptr;
    }

    if (context->callback == nullptr) {
        return promise;
    } else {
        return GetNull(env);
    }
}

napi_value HuksNapiFinish(napi_env env, napi_callback_info info)
{
    FinishAsyncCtxPtr context = CreateFinishAsyncContext();
    if (context == nullptr) {
        HKS_LOG_E("could not create context");
        return nullptr;
    }

    napi_value result = ParseFinishParams(env, info, context);
    if (result == nullptr) {
        HKS_LOG_E("could not parse params");
        DeleteFinishAsyncContext(env, context);
        return nullptr;
    }

    result = FinishAsyncWork(env, context);
    if (result == nullptr) {
        HKS_LOG_E("could not start async work");
        DeleteFinishAsyncContext(env, context);
        return nullptr;
    }
    return result;
}
}  // namespace HuksNapi