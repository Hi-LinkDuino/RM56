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

#include "huks_napi_init.h"

#include "securec.h"

#include "hks_api.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "hks_type.h"
#include "huks_napi_common.h"


namespace HuksNapi {
namespace {
constexpr int HUKS_NAPI_INIT_MIN_ARGS = 2;
constexpr int HUKS_NAPI_INIT_MAX_ARGS = 3;

constexpr int HKS_MAX_TOKEN_SIZE = 2048;
}  // namespace

struct InitAsyncContext {
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callback = nullptr;

    int64_t result = 0;
    struct HksBlob *keyAlias = nullptr;
    struct HksParamSet *paramSet = nullptr;
    struct HksBlob *handle = nullptr;
};

using InitAsyncCtxPtr = InitAsyncContext *;

static InitAsyncCtxPtr CreateInitAsyncContext()
{
    InitAsyncCtxPtr context = (InitAsyncCtxPtr)HksMalloc(sizeof(InitAsyncContext));
    if (context != nullptr) {
        (void)memset_s(context, sizeof(InitAsyncContext), 0, sizeof(InitAsyncContext));
    }
    return context;
}

static void DeleteInitAsyncContext(napi_env env, InitAsyncCtxPtr &context)
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

    if (context->keyAlias != nullptr) {
        FreeHksBlob(context->keyAlias);
    }

    if (context->paramSet != nullptr) {
        HksFreeParamSet(&context->paramSet);
    }

    if (context->handle != nullptr) {
        if (context->handle->data != nullptr && context->handle->size != 0) {
            (void)memset_s(context->handle->data, context->handle->size, 0, context->handle->size);
        }
        FreeHksBlob(context->handle);
    }

    HksFree(context);
    context = nullptr;
}

static napi_value ParseInitParams(napi_env env, napi_callback_info info, InitAsyncCtxPtr context)
{
    size_t argc = HUKS_NAPI_INIT_MAX_ARGS;
    napi_value argv[HUKS_NAPI_INIT_MAX_ARGS] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    if (argc < HUKS_NAPI_INIT_MIN_ARGS) {
        napi_throw_error(env, NULL, "invalid arguments");
        HKS_LOG_E("no enough params");
        return nullptr;
    }

    size_t index = 0;
    napi_value result = ParseKeyAlias(env, argv[index], context->keyAlias);
    if (result == nullptr) {
        HKS_LOG_E("could not get alias");
        return nullptr;
    }

    index++;
    napi_value properties = nullptr;
    napi_status status =
        napi_get_named_property(env, argv[index], HKS_OPTIONS_PROPERTY_PROPERTIES.c_str(), &properties);
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
    // make sure inData is needed
    napi_value inData = nullptr;
    status = napi_get_named_property(env, argv[index], HKS_OPTIONS_PROPERTY_INDATA.c_str(), &inData);
    if (status != napi_ok || inData == nullptr) {
        GET_AND_THROW_LAST_ERROR((env));
        HKS_LOG_E("could not get property %s", HKS_OPTIONS_PROPERTY_INDATA.c_str());
        return nullptr;
    }
    if (result == nullptr) {
        HKS_LOG_E("could not get indata");
        return nullptr;
    }

    // parse handle

    index++;
    if (index < argc) {
        context->callback = GetCallback(env, argv[index]);
    }

    return GetInt32(env, 0);
}

static napi_value InitWriteResult(napi_env env, InitAsyncCtxPtr context)
{
    return GenerateHksHandle(env,
        context->result,
        ((context->result == HKS_SUCCESS && context->handle != nullptr) ? context->handle->data : nullptr),
        (context->result == HKS_SUCCESS && context->handle != nullptr) ? context->handle->size : 0);
}


static napi_value InitAsyncWork(napi_env env, InitAsyncCtxPtr context)
{
    napi_value promise = nullptr;
    if (context->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &context->deferred, &promise));
    }

    napi_value resourceName;
    napi_create_string_latin1(env, "InitAsyncWork", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            InitAsyncCtxPtr context = static_cast<InitAsyncCtxPtr>(data);
            context->handle = (HksBlob *)HksMalloc(sizeof(HksBlob));
            if (context->handle != nullptr) {
                context->handle->data = (uint8_t *)HksMalloc(HKS_MAX_TOKEN_SIZE);
                context->handle->size = HKS_MAX_TOKEN_SIZE;
            }
            context->result =
                HksInit(context->keyAlias, context->paramSet, context->handle);
        },
        [](napi_env env, napi_status status, void *data) {
            InitAsyncCtxPtr context = static_cast<InitAsyncCtxPtr>(data);
            napi_value result = InitWriteResult(env, context);
            if (result == nullptr) {
                return;
            }
            if (context->callback != nullptr) {
                CallAsyncCallback(env, context->callback, context->result, result);
            } else {
                napi_resolve_deferred(env, context->deferred, result);
            }
            DeleteInitAsyncContext(env, context);
        },
        (void *)context,
        &context->asyncWork);

    napi_status status = napi_queue_async_work(env, context->asyncWork);
    if (status != napi_ok) {
        GET_AND_THROW_LAST_ERROR((env));
        DeleteInitAsyncContext(env, context);
        HKS_LOG_E("could not queue async work");
        return nullptr;
    }

    if (context->callback == nullptr) {
        return promise;
    } else {
        return GetNull(env);
    }
}

napi_value HuksNapiInit(napi_env env, napi_callback_info info)
{
    InitAsyncCtxPtr context = CreateInitAsyncContext();
    if (context == nullptr) {
        HKS_LOG_E("could not create context");
        return nullptr;
    }

    napi_value result = ParseInitParams(env, info, context);
    if (result == nullptr) {
        HKS_LOG_E("could not parse params");
        DeleteInitAsyncContext(env, context);
        return nullptr;
    }

    result = InitAsyncWork(env, context);
    if (result == nullptr) {
        HKS_LOG_E("could not start async work");
        DeleteInitAsyncContext(env, context);
        return nullptr;
    }
    return result;
}
}  // namespace HuksNapi
