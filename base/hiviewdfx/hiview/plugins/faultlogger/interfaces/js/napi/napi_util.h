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

#ifndef BASE_FAULTLOGGER_NAPI_NAPI_UTIL_H
#define BASE_FAULTLOGGER_NAPI_NAPI_UTIL_H

#include <string>
#include <vector>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace HiviewDFX {
class NapiUtil {
public:
    static napi_value CreateErrorMessage(napi_env env, std::string message);
    static napi_value CreateUndefined(napi_env env);
    static void SetPropertyInt32(napi_env env, napi_value object, std::string name, int32_t value);
    static void SetPropertyInt64(napi_env env, napi_value object, std::string name, int64_t value);
    static void SetPropertyStringUtf8(napi_env env, napi_value object, std::string name, std::string value);
    static bool IsMatchType(napi_env env, napi_value value, napi_valuetype type);
};
}  // namespace HiviewDFX
}  // namespace OHOS
#endif // NAPI_UTIL_H