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

#include "huks_napi_attest_key.h"

#include "securec.h"

#include "hks_api.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "hks_type.h"
#include "huks_napi_common.h"

namespace HuksNapi {
namespace {
constexpr int HUKS_NAPI_ATTEST_KEY_MIN_ARGS = 2;
constexpr int HUKS_NAPI_ATTEST_KEY_MAX_ARGS = 3;
}  // namespace

struct AttestKeyAsyncContextT {
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callback = nullptr;

    int32_t result = 0;
    struct HksBlob *keyAlias = nullptr;
    struct HksParamSet *paramSet = nullptr;
    struct HksCertChain *certChain = nullptr;
};
using AttestKeyAsyncContext = AttestKeyAsyncContextT *;

static AttestKeyAsyncContext CreateAttestKeyAsyncContext()
{
    AttestKeyAsyncContext context = (AttestKeyAsyncContext)HksMalloc(sizeof(AttestKeyAsyncContextT));
    if (context != nullptr) {
        (void)memset_s(context, sizeof(AttestKeyAsyncContextT), 0, sizeof(AttestKeyAsyncContextT));
    }
    return context;
}

static void DeleteAttestKeyAsyncContext(napi_env env, AttestKeyAsyncContext &context)
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

    if (context->certChain != nullptr) {
        FreeHksCertChain(context->certChain);
    }

    HksFree(context);
    context = nullptr;
}

static napi_value AttestKeyParseParams(napi_env env, napi_callback_info info, AttestKeyAsyncContext context)
{
    size_t argc = HUKS_NAPI_ATTEST_KEY_MAX_ARGS;
    napi_value argv[HUKS_NAPI_ATTEST_KEY_MAX_ARGS] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    if (argc < HUKS_NAPI_ATTEST_KEY_MIN_ARGS) {
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

    index++;
    if (index < argc) {
        context->callback = GetCallback(env, argv[index]);
    }

    return GetInt32(env, 0);
}

static napi_value AttestKeyWriteResult(napi_env env, AttestKeyAsyncContext context)
{
    napi_value result = GenerateHksResult(env, context->result, nullptr, 0);
    if (result != nullptr) {
        napi_value certChains = GenerateStringArray(env, context->certChain->certs, context->certChain->certsCount);
        if (certChains != nullptr) {
            napi_set_named_property(env, result, HKS_RESULT_PRPPERTY_CERTCHAINS.c_str(), certChains);
        }
    }
    return result;
}

static napi_value AttestKeyAsyncWork(napi_env env, AttestKeyAsyncContext context)
{
    napi_value promise = nullptr;
    if (context->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &context->deferred, &promise));
    }

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "attestKeyAsyncWork", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AttestKeyAsyncContext context = static_cast<AttestKeyAsyncContext>(data);

            context->certChain = (struct HksCertChain *)HksMalloc(sizeof(struct HksCertChain));
            if (context->certChain != nullptr) {
                context->certChain->certs = nullptr;
                context->certChain->certsCount = 0;
            }

            context->result = HksAttestKey(context->keyAlias, context->paramSet, context->certChain);
        },
        [](napi_env env, napi_status status, void *data) {
            AttestKeyAsyncContext context = static_cast<AttestKeyAsyncContext>(data);
            napi_value result = AttestKeyWriteResult(env, context);
            if (result == nullptr) {
                return;
            }
            if (context->callback != nullptr) {
                CallAsyncCallback(env, context->callback, context->result, result);
            } else {
                napi_resolve_deferred(env, context->deferred, result);
            }
            DeleteAttestKeyAsyncContext(env, context);
        },
        (void *)context,
        &context->asyncWork);

    napi_status status = napi_queue_async_work(env, context->asyncWork);
    if (status != napi_ok) {
        GET_AND_THROW_LAST_ERROR((env));
        DeleteAttestKeyAsyncContext(env, context);
        HKS_LOG_E("could not queue async work");
        return nullptr;
    }

    if (context->callback == nullptr) {
        return promise;
    } else {
        return GetNull(env);
    }
}

napi_value HuksNapiAttestKey(napi_env env, napi_callback_info info)
{
    AttestKeyAsyncContext context = CreateAttestKeyAsyncContext();
    if (context == nullptr) {
        HKS_LOG_E("could not create context");
        return nullptr;
    }

    napi_value result = AttestKeyParseParams(env, info, context);
    if (result == nullptr) {
        HKS_LOG_E("could not parse params");
        DeleteAttestKeyAsyncContext(env, context);
        return nullptr;
    }

    result = AttestKeyAsyncWork(env, context);
    if (result == nullptr) {
        HKS_LOG_E("could not start async work");
        DeleteAttestKeyAsyncContext(env, context);
        return nullptr;
    }
    return result;
}
}  // namespace HuksNapi
