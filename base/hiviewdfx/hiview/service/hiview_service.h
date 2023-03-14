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
#ifndef HIVIEW_PLATFORM_MANAGEMENT_INFO_EXPORT_SERVICE_H
#define HIVIEW_PLATFORM_MANAGEMENT_INFO_EXPORT_SERVICE_H
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "audit_log_parser.h"
namespace OHOS {
namespace HiviewDFX {
class HiviewService {
public:
    HiviewService() : parser_(nullptr){};
    ~HiviewService(){};
    void StartService();
    // Dump interface
    void DumpRequestDispatcher(int fd, const std::vector<std::string>& cmds);
private:
    void DumpDetailedInfo(int fd);
    void DumpPluginInfo(int fd, const std::vector<std::string>& cmds) const;
    void DumpLoadedPluginInfo(int fd) const;
    void DumpPluginUsageInfo(int fd);
    void DumpPluginUsageInfo(int fd, const std::string& pluginName) const;

    void DumpThreadUsageInfo(int fd) const;
    void DumpThreadUsageInfo(int fd, const std::string& threadName) const;

    void DumpPipelineUsageInfo(int fd) const;
    void DumpPipelineUsageInfo(int fd, const std::string& pipelineName) const;

    void PrintUsage(int fd) const;
    std::unique_ptr<AuditLogParser> parser_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_PLATFORM_MANAGEMENT_INFO_EXPORT_SERVICE_H
