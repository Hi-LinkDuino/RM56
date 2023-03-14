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
#ifndef HIVIEWDFX_HIVIEW_FAULTLOGGER_UTIL_H
#define HIVIEWDFX_HIVIEW_FAULTLOGGER_UTIL_H
#include <cstdint>
#include <string>

#include "faultlog_info.h"

namespace OHOS {
namespace HiviewDFX {
std::string GetFormatedTime(uint64_t time);
std::string GetFaultNameByType(int32_t faultType, bool asfileName);
std::string GetFaultLogName(const FaultLogInfo& info);
int32_t GetLogTypeByName(const std::string& type);
FaultLogInfo ExtractInfoFromFileName(const std::string& fileName);
FaultLogInfo ExtractInfoFromTempFile(const std::string& fileName);
int32_t GetRawEventIdByType(int32_t logType);
std::string RegulateModuleNameIfNeed(const std::string& name);
std::vector<std::string> GetApplicationNamesById(int32_t uid);
std::string GetApplicationNameById(int32_t uid);
std::string GetApplicationVersion(int32_t uid, const std::string& bundleName);
}  // namespace HiviewDFX
}  // namespace OHOS
#endif  // HIVIEWDFX_HIVIEW_FAULTLOGGER_UTIL_H