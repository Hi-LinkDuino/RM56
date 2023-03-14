/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "napi_util.h"

namespace OHOS {
namespace HiviewDFX {
napi_value NapiUtil::CreateErrorMessage(napi_env env, std::string msg)
{
    napi_value result = nullptr;
    napi_value message = nullptr;
    napi_create_string_utf8(env, msg.c_str(), msg.length(), &message);
    napi_value codeValue = nullptr;
    std::string errCode = std::to_string(-1);
    napi_create_string_utf8(env, errCode.c_str(), errCode.length(), &codeValue);
    napi_create_error(env, codeValue, message, &result);
    return result;
}

napi_value NapiUtil::CreateUndefined(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

void NapiUtil::SetPropertyInt32(napi_env env, napi_value object, std::string name, int32_t value)
{
    napi_value propertyValue = nullptr;
    napi_create_int32(env, value, &propertyValue);
    napi_set_named_property(env, object, name.c_str(), propertyValue);
}

void NapiUtil::SetPropertyInt64(napi_env env, napi_value object, std::string name, int64_t value)
{
    napi_value propertyValue = nullptr;
    napi_create_int64(env, value, &propertyValue);
    napi_set_named_property(env, object, name.c_str(), propertyValue);
}

void NapiUtil::SetPropertyStringUtf8(napi_env env, napi_value object, std::string name, std::string value)
{
    napi_value propertyValue = nullptr;
    napi_create_string_utf8(env, value.c_str(), value.length(), &propertyValue);
    napi_set_named_property(env, object, name.c_str(), propertyValue);
}

bool NapiUtil::IsMatchType(napi_env env, napi_value value, napi_valuetype type)
{
    napi_valuetype paramType;
    napi_typeof(env, value, &paramType);
    if (paramType == type) {
        return true;
    }
    return false;
}
}  // namespace HiviewDFX
}  // namespace OHOS