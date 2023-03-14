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
#ifndef HIVIEW_FAULT_LOG_FORMATTER_H
#define HIVIEW_FAULT_LOG_FORMATTER_H
#include <cstdint>
#include <map>
#include <string>
#include "faultlog_info.h"
namespace OHOS {
namespace HiviewDFX {
namespace FaultLogger {
void WriteFaultLogToFile(int32_t fd, int32_t logType, std::map<std::string, std::string> sections);
FaultLogInfo ParseFaultLogInfoFromFile(const std::string& path, bool isTempFile = false);
void WriteStackTraceFromLog(int32_t fd, const std::string& pidStr, const std::string& path);
std::string GetSummaryByType(int32_t logType, std::map<std::string, std::string> sections);
bool WriteLogToFile(int32_t fd, const std::string& path);
}  // namespace FaultLogger
}  // namespace HiviewDFX
}  // namespace OHOS
#endif  // HIVIEW_FAULT_LOG_FORMATTER_H