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

#include "huks_napi_mac.h"

#include "securec.h"

#include "hks_api.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "hks_type.h"
#include "huks_napi_common.h"

namespace HuksNapi {
namespace {
constexpr int HUKS_NAPI_AGREE_KEY_MIN_ARGS = 2;
constexpr int HUKS_NAPI_AGREE_KEY_MAX_ARGS = 3;

constexpr int HKS_MAX_MAC_SIZE = 512;
}  // namespace

struct MacAsyncContextT {
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callback = nullptr;

    int32_t result = 0;
    struct HksParamSet *paramSet = nullptr;
    struct HksBlob *keyAlias = nullptr;
    struct HksBlob *srcData = nullptr;
    struct HksBlob *mac = nullptr;
};
using MacAsyncContext = MacAsyncContextT *;

static MacAsyncContext CreateMacAsyncContext()
{
    MacAsyncContext context = (MacAsyncContext)HksMalloc(sizeof(MacAsyncContextT));
    if (context != nullptr) {
        (void)memset_s(context, sizeof(MacAsyncContextT), 0, sizeof(MacAsyncContextT));
    }
    return context;
}

static void DeleteMacAsyncContext(napi_env env, MacAsyncContext &context)
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

    if (context->mac != nullptr) {
        if (context->mac->data != nullptr && context->mac->size != 0) {
            (void)memset_s(context->mac->data, context->mac->size, 0, context->mac->size);
        }
        FreeHksBlob(context->mac);
    }

    HksFree(context);
    context = nullptr;
}

static napi_value MacParseParams(napi_env env, napi_callback_info info, MacAsyncContext context)
{
    size_t argc = HUKS_NAPI_AGREE_KEY_MAX_ARGS;
    napi_value argv[HUKS_NAPI_AGREE_KEY_MAX_ARGS] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    if (argc < HUKS_NAPI_AGREE_KEY_MIN_ARGS) {
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

    napi_value inData = nullptr;
    status = napi_get_named_property(env, argv[index], HKS_OPTIONS_PROPERTY_INDATA.c_str(), &inData);
    if (status != napi_ok || inData == nullptr) {
        GET_AND_THROW_LAST_ERROR((env));
        HKS_LOG_E("could not get property %s", HKS_OPTIONS_PROPERTY_INDATA.c_str());
        return nullptr;
    }
    context->srcData = (HksBlob *)HksMalloc(sizeof(HksBlob));
    if (context->srcData == nullptr) {
        HKS_LOG_E("could not alloc memory");
        return nullptr;
    }
    (void)memset_s(context->srcData, sizeof(HksBlob), 0, sizeof(HksBlob));

    if (GetUint8Array(env, inData, *context->srcData) == nullptr) {
        HKS_LOG_E("could not get indata");
        return nullptr;
    }

    index++;
    if (index < argc) {
        context->callback = GetCallback(env, argv[index]);
    }

    return GetInt32(env, 0);
}

static napi_value MacWriteResult(napi_env env, MacAsyncContext context)
{
    return GenerateHksResult(env,
        context->result,
        ((context->result == HKS_SUCCESS && context->mac != nullptr) ? context->mac->data : nullptr),
        (context->result == HKS_SUCCESS && context->mac != nullptr) ? context->mac->size : 0);
}

static napi_value MacAsyncWork(napi_env env, MacAsyncContext context)
{
    napi_value promise = nullptr;
    if (context->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &context->deferred, &promise));
    }

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "macAsyncWork", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            MacAsyncContext context = static_cast<MacAsyncContext>(data);

            context->mac = (HksBlob *)HksMalloc(sizeof(HksBlob));
            if (context->mac != nullptr) {
                context->mac->data = (uint8_t *)HksMalloc(HKS_MAX_MAC_SIZE);
                context->mac->size = HKS_MAX_MAC_SIZE;
            }

            context->result = HksMac(context->keyAlias, context->paramSet, context->srcData, context->mac);
        },
        [](napi_env env, napi_status status, void *data) {
            MacAsyncContext context = static_cast<MacAsyncContext>(data);
            napi_value result = MacWriteResult(env, context);
            if (result == nullptr) {
                return;
            }
            if (context->callback != nullptr) {
                CallAsyncCallback(env, context->callback, context->result, result);
            } else {
                napi_resolve_deferred(env, context->deferred, result);
            }
            DeleteMacAsyncContext(env, context);
        },
        (void *)context,
        &context->asyncWork);

    napi_status status = napi_queue_async_work(env, context->asyncWork);
    if (status != napi_ok) {
        GET_AND_THROW_LAST_ERROR((env));
        DeleteMacAsyncContext(env, context);
        HKS_LOG_E("could not queue async work");
        return nullptr;
    }

    if (context->callback == nullptr) {
        return promise;
    } else {
        return GetNull(env);
    }
}

napi_value HuksNapiMac(napi_env env, napi_callback_info info)
{
    MacAsyncContext context = CreateMacAsyncContext();
    if (context == nullptr) {
        HKS_LOG_E("could not create context");
        return nullptr;
    }

    napi_value result = MacParseParams(env, info, context);
    if (result == nullptr) {
        HKS_LOG_E("could not parse params");
        DeleteMacAsyncContext(env, context);
        return nullptr;
    }

    result = MacAsyncWork(env, context);
    if (result == nullptr) {
        HKS_LOG_E("could not start async work");
        DeleteMacAsyncContext(env, context);
        return nullptr;
    }
    return result;
}
}  // namespace HuksNapi
