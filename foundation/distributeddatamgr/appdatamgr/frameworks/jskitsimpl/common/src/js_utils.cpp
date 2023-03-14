/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "js_utils.h"

#include "securec.h"

namespace OHOS {
namespace AppDataMgrJsKit {
std::string JSUtils::Convert2String(napi_env env, napi_value jsStr, const size_t max)
{
    NAPI_ASSERT_BASE(env, max > 0, "failed on max > 0", std::string());
    char *buf = new char[max + 1];
    size_t len = 0;
    napi_get_value_string_utf8(env, jsStr, buf, max, &len);
    buf[len] = 0;
    std::string value(buf);
    delete[] buf;
    return value;
}

std::vector<std::string> JSUtils::Convert2StrVector(napi_env env, napi_value value, const size_t strMax)
{
    NAPI_ASSERT_BASE(env, strMax > 0, "failed on strMax > 0",  std::vector<std::string>());
    uint32_t arrLen = 0;
    napi_get_array_length(env, value, &arrLen);
    if (arrLen == 0) {
        return {};
    }
    std::vector<std::string> result;
    for (size_t i = 0; i < arrLen; ++i) {
        napi_value element;
        napi_get_element(env, value, i, &element);
        result.push_back(ConvertAny2String(env, element));
    }
    return result;
}

std::vector<uint8_t> JSUtils::Convert2U8Vector(napi_env env, napi_value input_array)
{
    bool isTypedArray = false;
    napi_is_typedarray(env, input_array, &isTypedArray);
    if (!isTypedArray) {
        return {};
    }

    napi_typedarray_type type;
    napi_value input_buffer = nullptr;
    size_t byte_offset = 0;
    size_t length = 0;
    void *data = nullptr;
    napi_get_typedarray_info(env, input_array, &type, &length, &data, &input_buffer, &byte_offset);
    if (type != napi_uint8_array || data == nullptr) {
        return {};
    }
    return std::vector<uint8_t>((uint8_t *)data, ((uint8_t *)data) + length);
}

std::string JSUtils::ConvertAny2String(napi_env env, napi_value jsValue)
{
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL_BASE(env, napi_typeof(env, jsValue, &valueType), "napi_typeof failed");
    if (valueType == napi_string) {
        return JSUtils::Convert2String(env, jsValue, JSUtils::DEFAULT_BUF_SIZE);
    } else if (valueType == napi_number) {
        double valueNumber;
        napi_get_value_double(env, jsValue, &valueNumber);
        return std::to_string(valueNumber);
    } else if (valueType == napi_boolean) {
        bool valueBool = false;
        napi_get_value_bool(env, jsValue, &valueBool);
        return std::to_string(valueBool);
    } else if (valueType == napi_null) {
        return "null";
    } else if (valueType == napi_object) {
        std::vector<uint8_t> bytes = JSUtils::Convert2U8Vector(env, jsValue);
        std::string ret(bytes.begin(), bytes.end());
        return ret;
    }

    return "invalid type";
}

napi_value JSUtils::Convert2JSValue(napi_env env, const std::vector<std::string> &value)
{
    napi_value jsValue;
    napi_status status = napi_create_array_with_length(env, value.size(), &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }

    for (size_t i = 0; i < value.size(); ++i) {
        napi_set_element(env, jsValue, i, Convert2JSValue(env, value[i]));
    }
    return jsValue;
}

napi_value JSUtils::Convert2JSValue(napi_env env, const std::string &value)
{
    napi_value jsValue;
    napi_status status = napi_create_string_utf8(env, value.c_str(), value.size(), &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value JSUtils::Convert2JSValue(napi_env env, const std::vector<uint8_t> &value)
{
    napi_value jsValue;
    void *native = nullptr;
    napi_value buffer = nullptr;
    napi_status status = napi_create_arraybuffer(env, value.size(), &native, &buffer);
    if (status != napi_ok) {
        return nullptr;
    }
    int result = memcpy_s(native, value.size(), value.data(), value.size());
    if (result != EOK && value.size() > 0) {
        return nullptr;
    }
    status = napi_create_typedarray(env, napi_uint8_array, value.size(), buffer, 0, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value JSUtils::Convert2JSValue(napi_env env, int32_t value)
{
    napi_value jsValue;
    napi_status status = napi_create_int32(env, value, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value JSUtils::Convert2JSValue(napi_env env, int64_t value)
{
    napi_value jsValue;
    napi_status status = napi_create_int64(env, value, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value JSUtils::Convert2JSValue(napi_env env, double value)
{
    napi_value jsValue;
    napi_status status = napi_create_double(env, value, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value JSUtils::Convert2JSValue(napi_env env, bool value)
{
    napi_value jsValue;
    napi_status status = napi_get_boolean(env, value, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value JSUtils::Convert2JSValue(napi_env env, const std::map<std::string, int> &value)
{
    napi_value jsValue;
    napi_status status = napi_create_array_with_length(env, value.size(), &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }

    int index = 0;
    for (const auto& [device, result] : value) {
        napi_value jsElement;
        status = napi_create_array_with_length(env, SYNC_RESULT_ELEMNT_NUM, &jsElement);
        if (status != napi_ok) {
            return nullptr;
        }
        napi_set_element(env, jsElement, 0, Convert2JSValue(env, device));
        napi_set_element(env, jsElement, 1, Convert2JSValue(env, result));
        napi_set_element(env, jsValue, index++, jsElement);
    }

    return jsValue;
}
} // namespace AppDataMgrJsKit
} // namespace OHOS
