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
#ifndef HIVIEW_FAULTLOGGER_CLIENT_INTERFACE_H
#define HIVIEW_FAULTLOGGER_CLIENT_INTERFACE_H
#include <cstdint>
#include <map>
#include <string>
#include "faultlog_query_result.h"
#ifdef __cplusplus
extern "C" {
#endif
struct FaultLogInfoInner {
    int64_t time {0};
    int32_t id {-1};
    int32_t pid {-1};
    int32_t faultLogType {0};
    std::string module;
    std::string reason;
    std::string summary;
    std::string logPath;
    std::map<std::string, std::string> sectionMaps;
};
void AddFaultLog(FaultLogInfoInner* info);
#ifdef __cplusplus
}
#endif

namespace OHOS {
namespace HiviewDFX {
enum FaultLogType {
    NO_SPECIFIC = 0,
    JAVA_CRASH,
    CPP_CRASH,
    JS_CRASH,
    APP_FREEZE,
    SYS_FREEZE,
};

// internal interface
void AddFaultLog(const FaultLogInfoInner &info);
void AddFaultLog(int64_t time, int32_t logType, const std::string &module, const std::string &summary);

// external interface
std::unique_ptr<FaultLogQueryResult> QuerySelfFaultLog(FaultLogType faultType, int32_t maxNum);
}  // namespace HiviewDFX
}  // namespace OHOS
#endif  // HIVIEW_FAULTLOGGER_CLIENT_INTERFACE_H
