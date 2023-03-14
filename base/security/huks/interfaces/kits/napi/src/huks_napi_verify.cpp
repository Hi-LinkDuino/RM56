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

#include "huks_napi_verify.h"

#include "securec.h"

#include "hks_api.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "hks_type.h"
#include "huks_napi_common.h"

namespace HuksNapi {
namespace {
constexpr int HUKS_NAPI_VERIFY_MIN_ARGS = 3;
constexpr int HUKS_NAPI_VERIFY_MAX_ARGS = 4;
}  // namespace

struct VerifyAsyncContextT {
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callback = nullptr;

    int32_t result = 0;
    struct HksBlob *keyAlias = nullptr;
    struct HksParamSet *paramSet = nullptr;
    struct HksBlob *srcData = nullptr;
    struct HksBlob *signature = nullptr;
};
using VerifyAsyncContext = VerifyAsyncContextT *;

static VerifyAsyncContext CreateVerifyAsyncContext()
{
    VerifyAsyncContext context = (VerifyAsyncContext)HksMalloc(sizeof(VerifyAsyncContextT));
    if (context != nullptr) {
        (void)memset_s(context, sizeof(VerifyAsyncContextT), 0, sizeof(VerifyAsyncContextT));
    }
    return context;
}

static void DeleteVerifyAsyncContext(napi_env env, VerifyAsyncContext &context)
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

    if (context->srcData != nullptr) {
        if (context->srcData->data != nullptr && context->srcData->size != 0) {
            (void)memset_s(context->srcData->data, context->srcData->size, 0, context->srcData->size);
        }
        FreeHksBlob(context->srcData);
    }

    if (context->signature != nullptr) {
        if (context->signature->data != nullptr && context->signature->size != 0) {
            (void)memset_s(context->signature->data, context->signature->size, 0, context->signature->size);
        }
        FreeHksBlob(context->signature);
    }

    HksFree(context);
    context = nullptr;
}

static napi_value VerifyParseOptions(napi_env env, napi_value options, VerifyAsyncContext context)
{
    napi_value result = nullptr;

    napi_value properties = nullptr;
    napi_status status = napi_get_named_property(env, options, HKS_OPTIONS_PROPERTY_PROPERTIES.c_str(), &properties);
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

    napi_value inData = nullptr;
    status = napi_get_named_property(env, options, HKS_OPTIONS_PROPERTY_INDATA.c_str(), &inData);
    if (status != napi_ok || inData == nullptr) {
        GET_AND_THROW_LAST_ERROR((env));
        HKS_LOG_E("could not get property %s", HKS_OPTIONS_PROPERTY_INDATA.c_str());
        return nullptr;
    }
    context->srcData = (HksBlob *)HksMalloc(sizeof(HksBlob));
    if (context->srcData == nullptr) {
        return nullptr;
    }
    (void)memset_s(context->srcData, sizeof(HksBlob), 0, sizeof(HksBlob));

    if (GetUint8Array(env, inData, *context->srcData) == nullptr) {
        HKS_LOG_E("could not get indata");
        return nullptr;
    }

    return GetInt32(env, 0);
}

static napi_value VerifyParseParams(napi_env env, napi_callback_info info, VerifyAsyncContext context)
{
    size_t argc = HUKS_NAPI_VERIFY_MAX_ARGS;
    napi_value argv[HUKS_NAPI_VERIFY_MAX_ARGS] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    if (argc < HUKS_NAPI_VERIFY_MIN_ARGS) {
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
    result = VerifyParseOptions(env, argv[index], context);
    if (result == nullptr) {
        HKS_LOG_E("could not parse options");
        return nullptr;
    }

    index++;
    context->signature = (HksBlob *)HksMalloc(sizeof(HksBlob));
    if (context->signature == nullptr) {
        return nullptr;
    }
    (void)memset_s(context->signature, sizeof(HksBlob), 0, sizeof(HksBlob));

    if (GetUint8Array(env, argv[index], *context->signature) == nullptr) {
        HKS_LOG_E("could not get signature");
        return nullptr;
    }

    index++;
    if (index < argc) {
        context->callback = GetCallback(env, argv[index]);
    }

    return GetInt32(env, 0);
}

static napi_value VerifyWriteResult(napi_env env, VerifyAsyncContext context)
{
    return GenerateHksResult(env, context->result, nullptr, 0);
}

static napi_value VerifyAsyncWork(napi_env env, VerifyAsyncContext context)
{
    napi_value promise = nullptr;
    if (context->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &context->deferred, &promise));
    }

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "verifyAsyncWork", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            VerifyAsyncContext context = static_cast<VerifyAsyncContext>(data);

            context->result = HksVerify(context->keyAlias, context->paramSet, context->srcData, context->signature);
        },
        [](napi_env env, napi_status status, void *data) {
            VerifyAsyncContext context = static_cast<VerifyAsyncContext>(data);
            napi_value result = VerifyWriteResult(env, context);
            if (result == nullptr) {
                return;
            }
            if (context->callback != nullptr) {
                CallAsyncCallback(env, context->callback, context->result, result);
            } else {
                napi_resolve_deferred(env, context->deferred, result);
            }
            DeleteVerifyAsyncContext(env, context);
        },
        (void *)context,
        &context->asyncWork);

    napi_status status = napi_queue_async_work(env, context->asyncWork);
    if (status != napi_ok) {
        GET_AND_THROW_LAST_ERROR((env));
        DeleteVerifyAsyncContext(env, context);
        HKS_LOG_E("could not queue async work");
        return nullptr;
    }

    if (context->callback == nullptr) {
        return promise;
    } else {
        return GetNull(env);
    }
}

napi_value HuksNapiVerify(napi_env env, napi_callback_info info)
{
    VerifyAsyncContext context = CreateVerifyAsyncContext();
    if (context == nullptr) {
        HKS_LOG_E("could not create context");
        return nullptr;
    }

    napi_value result = VerifyParseParams(env, info, context);
    if (result == nullptr) {
        HKS_LOG_E("could not parse params");
        DeleteVerifyAsyncContext(env, context);
        return nullptr;
    }

    result = VerifyAsyncWork(env, context);
    if (result == nullptr) {
        HKS_LOG_E("could not start async work");
        DeleteVerifyAsyncContext(env, context);
        return nullptr;
    }
    return result;
}
}  // namespace HuksNapi
