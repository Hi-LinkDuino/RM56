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

#include "huks_napi_common.h"

#include "securec.h"

#include "hks_log.h"
#include "hks_param.h"
#include "hks_type.h"

namespace HuksNapi {
namespace {
constexpr int HKS_MAX_DATA_LEN = 0x6400000; // The maximum length is 100M
constexpr size_t ASYNCCALLBACK_ARGC = 2;
}  // namespace

napi_value ParseKeyAlias(napi_env env, napi_value object, HksBlob *&alias)
{
    char *data = (char *)HksMalloc(HKS_MAX_KEY_ALIAS_LEN);
    if (data == nullptr) {
        napi_throw_error(env, NULL, "could not alloc memory");
        HKS_LOG_E("could not alloc memory");
        return nullptr;
    }
    (void)memset_s(data, HKS_MAX_KEY_ALIAS_LEN, 0x0, HKS_MAX_KEY_ALIAS_LEN);

    size_t length = 0;
    napi_status status = napi_get_value_string_utf8(env, object, data, HKS_MAX_KEY_ALIAS_LEN, &length);
    if (status != napi_ok) {
        HksFree(data);
        GET_AND_THROW_LAST_ERROR((env));
        HKS_LOG_E("could not get string");
        return nullptr;
    }

    alias = (HksBlob *)HksMalloc(sizeof(HksBlob));
    if (alias == nullptr) {
        HksFree(data);
        napi_throw_error(env, NULL, "could not alloc memory");
        HKS_LOG_E("could not alloc memory");
        return nullptr;
    }
    alias->data = (uint8_t *)data;
    alias->size = (uint32_t)(length & UINT32_MAX);

    return GetInt32(env, 0);
}

napi_value GetUint8Array(napi_env env, napi_value object, HksBlob &arrayBlob)
{
    napi_typedarray_type arrayType;
    napi_value arrayBuffer = nullptr;
    size_t length = 0;
    size_t offset = 0;
    void *rawData = nullptr;

    NAPI_CALL(
        env, napi_get_typedarray_info(env, object, &arrayType, &length, (void **)&rawData, &arrayBuffer, &offset));
    NAPI_ASSERT(env, arrayType == napi_uint8_array, "it's not uint8 array");

    if (length > HKS_MAX_DATA_LEN) {
        HKS_LOG_E("data len is too large, len = %x", length);
        return nullptr;
    }
    if (length == 0) {
        HKS_LOG_I("the created memory length just 1 Byte");
        // the created memory length just 1 Byte
        arrayBlob.data = (uint8_t *)HksMalloc(1);
    } else {
        arrayBlob.data = (uint8_t *)HksMalloc(length);
    }
    if (arrayBlob.data == nullptr) {
        return nullptr;
    }
    (void)memcpy_s(arrayBlob.data, length, rawData, length);
    arrayBlob.size = (uint32_t)length;

    return GetInt32(env, 0);
}

static napi_value GetHksParam(napi_env env, napi_value object, HksParam &param)
{
    napi_value tag = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, object, HKS_PARAM_PROPERTY_TAG.c_str(), &tag));
    NAPI_CALL(env, napi_get_value_uint32(env, tag, &param.tag));

    napi_value value = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, object, HKS_PARAM_PROPERTY_VALUE.c_str(), &value));

    napi_value result = nullptr;

    switch (param.tag & HKS_TAG_TYPE_MASK) {
        case HKS_TAG_TYPE_INT:
            NAPI_CALL(env, napi_get_value_int32(env, value, &param.int32Param));
            result = GetInt32(env, 0);
            break;
        case HKS_TAG_TYPE_UINT:
            NAPI_CALL(env, napi_get_value_uint32(env, value, &param.uint32Param));
            result = GetInt32(env, 0);
            break;
        case HKS_TAG_TYPE_ULONG:
            NAPI_CALL(env, napi_get_value_int64(env, value, (int64_t *)&param.uint64Param));
            result = GetInt32(env, 0);
            break;
        case HKS_TAG_TYPE_BOOL:
            NAPI_CALL(env, napi_get_value_bool(env, value, &param.boolParam));
            result = GetInt32(env, 0);
            break;
        case HKS_TAG_TYPE_BYTES:
            result = GetUint8Array(env, value, param.blob);
            if (result == nullptr) {
                HKS_LOG_E("get uint8 array fail.");
            } else {
                HKS_LOG_D("tag 0x%x, len 0x%x", param.tag, param.blob.size);
            }
            break;
        default:
            HKS_LOG_E("invalid tag value 0x%x", param.tag);
            break;
    }

    return result;
}

napi_value ParseHksParamSet(napi_env env, napi_value object, HksParamSet *&paramSet)
{
    if (HksInitParamSet(&paramSet) != HKS_SUCCESS) {
        napi_throw_error(env, NULL, "native error");
        HKS_LOG_E("init paramset failed");
        return nullptr;
    }

    size_t index = 0;
    bool hasNextElement = false;
    napi_value result = nullptr;
    while ((napi_has_element(env, object, index, &hasNextElement) == napi_ok) && hasNextElement) {
        napi_value element = nullptr;
        NAPI_CALL(env, napi_get_element(env, object, index, &element));

        HksParam param = {0};
        result = GetHksParam(env, element, param);
        if (result == nullptr) {
            HKS_LOG_E("GetHksParam failed.");
            HksFreeParamSet(&paramSet);
            return nullptr;
        }

        if (HksAddParams(paramSet, &param, 1) != HKS_SUCCESS) {
            HKS_LOG_E("HksAddParams failed.");
            HksFreeParamSet(&paramSet);
            return nullptr;
        }
        index++;
    }

    if (HksBuildParamSet(&paramSet) != HKS_SUCCESS) {
        HKS_LOG_E("HksBuildParamSet failed.");
        HksFreeParamSet(&paramSet);
        return nullptr;
    }

    return GetInt32(env, 0);
}

napi_ref GetCallback(napi_env env, napi_value object)
{
    napi_valuetype valueType = napi_undefined;
    napi_status status = napi_typeof(env, object, &valueType);
    if (status != napi_ok) {
        GET_AND_THROW_LAST_ERROR((env));
        HKS_LOG_E("could not get object type");
        return nullptr;
    }

    if (valueType != napi_function) {
        HKS_LOG_E("invalid type");
        return nullptr;
    }

    napi_ref ref = nullptr;
    status = napi_create_reference(env, object, 1, &ref);
    if (status != napi_ok) {
        GET_AND_THROW_LAST_ERROR((env));
        HKS_LOG_E("could not create reference");
        return nullptr;
    }
    return ref;
}

static napi_value GenerateAarrayBuffer(napi_env env, uint8_t *data, uint32_t size)
{
    uint8_t *buffer = (uint8_t *)HksMalloc(size);
    if (buffer == nullptr) {
        return nullptr;
    }

    napi_value outBuffer = nullptr;
    (void)memcpy_s(buffer, size, data, size);

    napi_status status = napi_create_external_arraybuffer(
        env, buffer, size, [](napi_env env, void *data, void *hint) { HksFree(data); }, nullptr, &outBuffer);
    if (status == napi_ok) {
        // free by finalize callback
        buffer = nullptr;
    } else {
        HksFree(buffer);
        GET_AND_THROW_LAST_ERROR((env));
    }

    return outBuffer;
}

static napi_value GenerateHksParam(napi_env env, const HksParam &param)
{
    napi_value hksParam = nullptr;
    NAPI_CALL(env, napi_create_object(env, &hksParam));

    napi_value tag = nullptr;
    NAPI_CALL(env, napi_create_uint32(env, param.tag, &tag));
    NAPI_CALL(env, napi_set_named_property(env, hksParam, HKS_PARAM_PROPERTY_TAG.c_str(), tag));

    napi_value value = nullptr;
    switch (param.tag & HKS_TAG_TYPE_MASK) {
        case HKS_TAG_TYPE_INT:
            NAPI_CALL(env, napi_create_int32(env, param.int32Param, &value));
            break;
        case HKS_TAG_TYPE_UINT:
            NAPI_CALL(env, napi_create_uint32(env, param.uint32Param, &value));
            break;
        case HKS_TAG_TYPE_ULONG:
            NAPI_CALL(env, napi_create_int64(env, param.uint64Param, &value));
            break;
        case HKS_TAG_TYPE_BOOL:
            NAPI_CALL(env, napi_get_boolean(env, param.boolParam, &value));
            break;
        case HKS_TAG_TYPE_BYTES:
            value = GenerateAarrayBuffer(env, param.blob.data, param.blob.size);
            break;
        default:
            value = GetNull(env);
            break;
    }
    NAPI_CALL(env, napi_set_named_property(env, hksParam, HKS_PARAM_PROPERTY_VALUE.c_str(), value));

    return hksParam;
}

static napi_value GenerateHksParamArray(napi_env env, const HksParamSet &paramSet)
{
    napi_value paramArray = nullptr;
    NAPI_CALL(env, napi_create_array(env, &paramArray));

    for (uint32_t i = 0; i < paramSet.paramsCnt; i++) {
        napi_value element = nullptr;
        element = GenerateHksParam(env, paramSet.params[i]);
        napi_set_element(env, paramArray, i, element);
    }

    return paramArray;
}

napi_value GenerateHksResult(napi_env env, int32_t error)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    napi_value errorCode = nullptr;
    NAPI_CALL(env, napi_create_int32(env, error, &errorCode));
    NAPI_CALL(env, napi_set_named_property(env, result, HKS_RESULT_PROPERTY_ERRORCODE.c_str(), errorCode));

    napi_value outData = GetNull(env);
    NAPI_CALL(env, napi_set_named_property(env, result, HKS_RESULT_PROPERTY_OUTDATA.c_str(), outData));

    napi_value properties = GetNull(env);
    NAPI_CALL(env, napi_set_named_property(env, result, HKS_RESULT_PRPPERTY_PROPERTIES.c_str(), properties));

    return result;
}

napi_value GenerateHksResult(napi_env env, int32_t error, uint8_t *data, uint32_t size)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    napi_value errorCode = nullptr;
    NAPI_CALL(env, napi_create_int32(env, error, &errorCode));
    NAPI_CALL(env, napi_set_named_property(env, result, HKS_RESULT_PROPERTY_ERRORCODE.c_str(), errorCode));

    napi_value outData = nullptr;
    if (data != nullptr && size != 0) {
        napi_value outBuffer = GenerateAarrayBuffer(env, data, size);
        if (outBuffer != nullptr) {
            NAPI_CALL(env, napi_create_typedarray(env, napi_uint8_array, size, outBuffer, 0, &outData));
        }
    } else {
        outData = GetNull(env);
    }
    NAPI_CALL(env, napi_set_named_property(env, result, HKS_RESULT_PROPERTY_OUTDATA.c_str(), outData));

    napi_value properties = GetNull(env);
    NAPI_CALL(env, napi_set_named_property(env, result, HKS_RESULT_PRPPERTY_PROPERTIES.c_str(), properties));

    return result;
}

napi_value GenerateHksResult(napi_env env, int32_t error, uint8_t *data, uint32_t size, const HksParamSet &paramSet)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    napi_value errorCode = nullptr;
    NAPI_CALL(env, napi_create_int32(env, error, &errorCode));
    NAPI_CALL(env, napi_set_named_property(env, result, HKS_RESULT_PROPERTY_ERRORCODE.c_str(), errorCode));

    napi_value outData = nullptr;
    if (data != nullptr && size != 0) {
        napi_value outBuffer = GenerateAarrayBuffer(env, data, size);
        if (outBuffer != nullptr) {
            NAPI_CALL(env, napi_create_typedarray(env, napi_uint8_array, size, outBuffer, 0, &outData));
        }
    } else {
        outData = GetNull(env);
    }
    NAPI_CALL(env, napi_set_named_property(env, result, HKS_RESULT_PROPERTY_OUTDATA.c_str(), outData));

    napi_value properties = GenerateHksParamArray(env, paramSet);
    NAPI_CALL(env, napi_set_named_property(env, result, HKS_RESULT_PRPPERTY_PROPERTIES.c_str(), properties));

    return result;
}

static napi_value GenerateBusinessError(napi_env env, int32_t errorCode)
{
    napi_value businessError = nullptr;
    NAPI_CALL(env, napi_create_object(env, &businessError));

    napi_value code = nullptr;
    NAPI_CALL(env, napi_create_int32(env, errorCode, &code));
    NAPI_CALL(env, napi_set_named_property(env, businessError, BUSINESS_ERROR_PROPERTY_CODE.c_str(), code));

    return businessError;
}

void CallAsyncCallback(napi_env env, napi_ref callback, int32_t error, napi_value data)
{
    napi_value businessError = GenerateBusinessError(env, error);

    napi_value params[ASYNCCALLBACK_ARGC] = { businessError, data };

    napi_value func = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, callback, &func));

    napi_value recv = nullptr;
    napi_value result = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &recv));
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, recv, func, ASYNCCALLBACK_ARGC, params, &result));
}

napi_value GenerateStringArray(napi_env env, const struct HksBlob *blob, const uint32_t blobCount)
{
    if (blobCount == 0 || blob == nullptr) {
        return nullptr;
    }
    napi_value array = nullptr;
    NAPI_CALL(env, napi_create_array(env, &array));
    for (uint32_t i = 0; i < blobCount; i++) {
        napi_value element = nullptr;
        napi_create_string_latin1(env, (const char *)blob[i].data, blob[i].size, &element);
        napi_set_element(env, array, i, element);
    }
    return array;
}

void FreeHksCertChain(HksCertChain *&certChain)
{
    if (certChain == nullptr) {
        return;
    }

    if (certChain->certsCount > 0 && certChain->certs != nullptr) {
        for (uint32_t i = 0; i < certChain->certsCount; i++) {
            if (certChain->certs[i].data != nullptr) {
                HksFree(certChain->certs[i].data);
                certChain->certs[i].data = nullptr;
            }
        }
    }

    HksFree(certChain);
    certChain = nullptr;
}

napi_value GenerateHksHandle(napi_env env, int32_t error, uint8_t *data, uint32_t size)
{
    if (data == nullptr) {
        HKS_LOG_E("data: invalid pointer");
        return nullptr;
    }

    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    napi_value errorCode = nullptr;
    NAPI_CALL(env, napi_create_int32(env, error, &errorCode));
    NAPI_CALL(env, napi_set_named_property(env, result, HKS_HANDLE_PROPERTY_ERRORCODE.c_str(), errorCode));

    uint64_t tempHandle = *(uint64_t *)data;
    uint32_t handle1 = 0;
    uint32_t handle2 = 0;
    napi_value handlejs1;
    napi_value handlejs2;

    handle2 = tempHandle;
    handle1 = tempHandle >> HKS_HANDLE_OFFSET32;

    NAPI_CALL(env, napi_create_uint32(env, handle1, &handlejs1));
    NAPI_CALL(env, napi_create_uint32(env, handle2, &handlejs2));
    NAPI_CALL(env, napi_set_named_property(env, result, "handle1", handlejs1));
    NAPI_CALL(env, napi_set_named_property(env, result, "handle2", handlejs2));

    return result;
}
}  // namespace HuksNapi
