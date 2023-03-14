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

#include "frameworks/bridge/js_frontend/engine/common/js_api_perf.h"

#include "base/utils/time_util.h"

namespace OHOS::Ace::Framework {

void JsApiPerf::InsertJsBeginLog(const std::string& functionName, int64_t timeStamp)
{
    TimeStamp stamp = {
        .name = functionName,
        .startTime = timeStamp != 0 ? timeStamp : GetMicroTickCount(),
    };
    startTimeData_.push(stamp);
}

void JsApiPerf::InsertJsEndLog(const std::string& functionName, int64_t timeStamp)
{
    if (startTimeData_.empty()) {
        return;
    }
    const std::string& itemName = startTimeData_.top().name;
    if (itemName == functionName) {
        int64_t now = timeStamp != 0 ? timeStamp : GetMicroTickCount();
        int64_t diff = now - startTimeData_.top().startTime;
        InsertPerfLog(functionName, diff);
        startTimeData_.pop();
    }
}

void JsApiPerf::InsertPerfLog(const std::string& functionName, int64_t timeConsumed)
{
    data_[functionName].emplace_back(timeConsumed);
}

std::string JsApiPerf::PrintToLogs() const
{
    std::string result;
    for (const auto& item : data_) {
        int64_t sum = 0;
        result.append(item.first);
        result.append("#");
        for (int64_t value : item.second) {
            sum += value;
            result.append(std::to_string(value));
            result.append("#");
        }
        result.append("average#");
        result.append(std::to_string(static_cast<uint64_t>(sum) / item.second.size()));
        result.append("\n");
    }

    return result;
}

JsApiPerf JsApiPerf::instance_;

JsApiPerf& JsApiPerf::GetInstance()
{
    return instance_;
}

} // namespace OHOS::Ace::Framework
