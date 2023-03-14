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
#include "napi_zlib_common.h"

#include "securec.h"

namespace OHOS {
namespace AppExecFwk {
namespace LIBZIP {
namespace {
const int E_OK = 0;
}

bool IsTypeForNapiValue(napi_env env, napi_value param, napi_valuetype expectType)
{
    napi_valuetype valueType = napi_undefined;

    if (napi_typeof(env, param, &valueType) != napi_ok) {
        return false;
    }

    return valueType == expectType;
}

std::string UnwrapStringFromJS(napi_env env, napi_value param, const std::string &defaultValue)
{
    size_t size = 0;
    if (napi_get_value_string_utf8(env, param, nullptr, 0, &size) != napi_ok) {
        return defaultValue;
    }

    std::string value("");
    if (size == 0) {
        return defaultValue;
    }

    char *buf = new (std::nothrow) char[size + 1];
    if (buf == nullptr) {
        return value;
    }
    if (memset_s(buf, size + 1, 0, size + 1) != E_OK) {
        delete[] buf;
        buf = nullptr;
        return nullptr;
    }

    bool rev = napi_get_value_string_utf8(env, param, buf, size + 1, &size) == napi_ok;
    if (rev) {
        value = buf;
    } else {
        value = defaultValue;
    }

    delete[] buf;
    buf = nullptr;
    return value;
}

bool UnwrapIntValue(napi_env env, napi_value jsValue, int &result)
{
    napi_valuetype jsValueType = napi_undefined;
    NAPI_CALL_BASE(env, napi_typeof(env, jsValue, &jsValueType), false);
    if (jsValueType != napi_number) {
        return false;
    }
    int32_t natValue32 = 0;
    NAPI_CALL_BASE(env, napi_get_value_int32(env, jsValue, &natValue32), false);
    result = static_cast<int>(natValue32);
    return true;
}
napi_value GetCallbackErrorValue(napi_env env, int errCode)
{
    napi_value jsObject = nullptr;
    napi_value jsValue = nullptr;
    NAPI_CALL(env, napi_create_int32(env, errCode, &jsValue));
    NAPI_CALL(env, napi_create_object(env, &jsObject));
    NAPI_CALL(env, napi_set_named_property(env, jsObject, "code", jsValue));
    return jsObject;
}

void SetNamedProperty(napi_env env, napi_value obj, const char *propName, const int propValue)
{
    napi_value prop = nullptr;
    napi_create_int32(env, propValue, &prop);
    napi_set_named_property(env, obj, propName, prop);
}
}  // namespace LIBZIP
}  // namespace AppExecFwk
}  // namespace OHOS