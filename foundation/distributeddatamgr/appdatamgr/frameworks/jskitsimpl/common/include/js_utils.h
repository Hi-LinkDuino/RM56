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

#ifndef DISTRIBUTEDDATAMGR_APPDATAMGR_JSUTILS_H
#define DISTRIBUTEDDATAMGR_APPDATAMGR_JSUTILS_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace AppDataMgrJsKit {
class JSUtils final {
public:
    static constexpr int32_t DEFAULT_BUF_SIZE = 1024;
    static constexpr int32_t ASYNC_RST_SIZE = 2;
    static constexpr int32_t SYNC_RESULT_ELEMNT_NUM = 2;

    static std::string Convert2String(napi_env env, napi_value jsStr, size_t max = DEFAULT_BUF_SIZE);
    static std::vector<std::string> Convert2StrVector(napi_env env, napi_value value, size_t strMax);
    static std::vector<uint8_t> Convert2U8Vector(napi_env env, napi_value jsValue);
    static std::string ConvertAny2String(napi_env env, const napi_value jsValue);

    static napi_value Convert2JSValue(napi_env env, const std::vector<std::string> &value);
    static napi_value Convert2JSValue(napi_env env, const std::string &value);
    static napi_value Convert2JSValue(napi_env env, const std::vector<uint8_t> &value);
    static napi_value Convert2JSValue(napi_env env, int32_t value);
    static napi_value Convert2JSValue(napi_env env, int64_t value);
    static napi_value Convert2JSValue(napi_env env, double value);
    static napi_value Convert2JSValue(napi_env env, bool value);
    static napi_value Convert2JSValue(napi_env env, const std::map<std::string, int>& value);
};
} // namespace AppDataMgrJsKit
} // namespace OHOS

#endif // DISTRIBUTEDDATAMGR_APPDATAMGR_JSUTILS_H
