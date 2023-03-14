/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef WIFI_NAPI_UTILS_H_
#define WIFI_NAPI_UTILS_H_

#include <string>
#include <chrono>
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Wifi {
class TraceFuncCall final {
public:
    TraceFuncCall(std::string funcName);

    TraceFuncCall() = delete;

    ~TraceFuncCall();

private:
    std::string m_funcName;
    std::chrono::steady_clock::time_point m_startTime;
    bool m_isTrace = true;
};

#define TRACE_FUNC_CALL TraceFuncCall func(__func__)
#define TRACE_FUNC_CALL_NAME(name) TraceFuncCall funcName(name)

constexpr int ERR_CODE_SUCCESS = 0;

class AsyncContext {
public:
    napi_env env;
    napi_async_work work;
    napi_deferred deferred;
    napi_ref callback[2] = { 0 };
    std::function<void(void*)> executeFunc;
    std::function<void(void*)> completeFunc;
    napi_value resourceName;
    napi_value result;
    int errorCode;

    AsyncContext(napi_env e, napi_async_work w = nullptr, napi_deferred d = nullptr)
    {
        env = e;
        work = w;
        deferred = d;
        executeFunc = nullptr;
        completeFunc = nullptr;
        result = nullptr;
        errorCode = ERR_CODE_SUCCESS;
    }

    AsyncContext() = delete;

    virtual ~AsyncContext()
    {
    }
};

napi_value UndefinedNapiValue(const napi_env& env);
napi_value JsObjectToString(const napi_env& env, const napi_value& object,
    const char* fieldStr, const int bufLen, std::string& fieldRef);
napi_value JsObjectToInt(const napi_env& env, const napi_value& object, const char* fieldStr, int& fieldRef);
napi_value JsObjectToBool(const napi_env& env, const napi_value& object, const char* fieldStr, bool& fieldRef);
napi_status SetValueUtf8String(const napi_env& env, const char* fieldStr, const char* str, napi_value& result);
napi_status SetValueInt32(const napi_env& env, const char* fieldStr, const int intValue, napi_value& result);
napi_status SetValueUnsignedInt32(const napi_env& env, const char* fieldStr, const int intValue, napi_value& result);
napi_status SetValueInt64(const napi_env& env, const char* fieldStr, const int64_t intValue, napi_value& result);
napi_status SetValueBool(const napi_env& env, const char* fieldStr, const bool boolValue, napi_value& result);
napi_value DoAsyncWork(const napi_env& env, AsyncContext *asyncContext,
    const size_t argc, const napi_value *argv, const size_t nonCallbackArgNum);

enum class SecTypeJs {
    SEC_TYPE_INVALID = 0, /* Invalid security type */
    SEC_TYPE_OPEN = 1, /* Open */
    SEC_TYPE_WEP = 2, /* Wired Equivalent Privacy (WEP) */
    SEC_TYPE_PSK = 3, /* Pre-shared key (PSK) */
    SEC_TYPE_SAE = 4, /* Simultaneous Authentication of Equals (SAE) */
};
}  // namespace Wifi
}  // namespace OHOS

#endif
