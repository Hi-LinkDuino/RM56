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
#ifndef FAULTLOGGER_NAPI_QUERY_LOG_H
#define FAULTLOGGER_NAPI_QUERY_LOG_H
#include <string>
#include <vector>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "faultlogger_client.h"

namespace OHOS {
namespace HiviewDFX {
class BaseContext {
public:
    BaseContext() {};
    virtual ~BaseContext() {};
    napi_async_work work = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callbackRef = nullptr;
};

struct FaultLogNapiInfo {
    int pid = 0;
    int uid = 0;
    int type = 0;
    int64_t ts = 0;
    std::string reason = "";
    std::string module = "";
    std::string summary = "";
    std::string fullLog = "";
};

class FaultLogInfoContext : public BaseContext {
public:
    FaultLogInfoContext() {};
    ~FaultLogInfoContext() {};
    int32_t faultType = 0;
    bool resolved = false;
    std::vector<FaultLogNapiInfo> infoVector;
};
}  // namespace HiviewDFX
}  // namespace OHOS
#endif  // FAULTLOGGER_JS_NAPI_QUERY_LOG_H