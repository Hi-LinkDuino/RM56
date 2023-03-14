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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_COMMON_JS_API_PERF_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_COMMON_JS_API_PERF_H

#include <map>
#include <stack>
#include <string>
#include <vector>

#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"

namespace OHOS::Ace::Framework {

struct TimeStamp {
    std::string name;
    int64_t startTime = 0;
};

class ACE_EXPORT JsApiPerf : public NonCopyable {
public:
    static JsApiPerf& GetInstance();

    void InsertJsBeginLog(const std::string& functionName, int64_t timeStamp = 0);
    void InsertJsEndLog(const std::string& functionName, int64_t timeStamp = 0);

    std::string PrintToLogs() const;

private:
    void InsertPerfLog(const std::string& functionName, int64_t timeConsumed);

    std::map<std::string, std::vector<int64_t>> data_;
    std::stack<TimeStamp> startTimeData_;
    static JsApiPerf instance_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_COMMON_JS_API_PERF_H
