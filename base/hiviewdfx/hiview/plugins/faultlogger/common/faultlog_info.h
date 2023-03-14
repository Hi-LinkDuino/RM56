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
#ifndef HIVIEW_FAULT_LOG_INFO_H
#define HIVIEW_FAULT_LOG_INFO_H
#include <cinttypes>
#include <list>
#include <map>
#include <string>

namespace OHOS {
namespace HiviewDFX {
struct FaultLogInfo {
    int64_t time {0};
    int32_t id {0};
    int32_t pid {0};
    int32_t faultLogType {0};
    int32_t fd = {-1};
    std::string module;
    std::string reason;
    std::string summary;
    std::string logPath;
    std::map<std::string, std::string> sectionMap;
    std::list<std::string> additionalLogs;
};

enum FaultLogType {
    ALL = 0,
    JAVA_CRASH,
    CPP_CRASH,
    JS_CRASH,
    APP_FREEZE,
    SYS_FREEZE,
};
}  // namespace HiviewDFX
}  // namespace OHOS
#endif  // HIVIEW_FAULT_LOG_INFO_H
