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
#include "hiview_service.h"

#include <memory>
#include <vector>
#include <string>
#include <cinttypes>
#include <cstdio>

#include "logger.h"
#include "hiview_service_adapter.h"
#include "hiview_platform.h"

namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("HiView-Service");
namespace {
constexpr int MIN_SUPPORT_CMD_SIZE = 1;
}
void HiviewService::StartService()
{
    std::unique_ptr<HiviewServiceAdapter> adapter = std::make_unique<HiviewServiceAdapter>();
    adapter->StartService(this);
}

void HiviewService::DumpRequestDispatcher(int fd, const std::vector<std::string> &cmds)
{
    if (fd < 0) {
        HIVIEW_LOGW("invalid fd.");
        return;
    }

    if (cmds.size() == 0) {
        DumpLoadedPluginInfo(fd);
        return;
    }

    // hidumper hiviewdfx -d
    if ((cmds.size() == MIN_SUPPORT_CMD_SIZE) && (cmds[0] == "-d")) {
        DumpDetailedInfo(fd);
        return;
    }

    // hidumper hiviewdfx -p
    if ((cmds.size() >= MIN_SUPPORT_CMD_SIZE) && (cmds[0] == "-p")) {
        DumpPluginInfo(fd, cmds);
        return;
    }

    PrintUsage(fd);
    return;
}

void HiviewService::DumpPluginInfo(int fd, const std::vector<std::string> &cmds) const
{
    std::string pluginName = "";
    const int pluginNameSize = 2;
    const int pluginNamePos = 1;
    std::vector<std::string> newCmd;
    if (cmds.size() >= pluginNameSize) {
        pluginName = cmds[pluginNamePos];
        newCmd.insert(newCmd.begin(), cmds.begin() + pluginNamePos, cmds.end());
    }

    auto &platform = HiviewPlatform::GetInstance();
    auto const &curPluginMap = platform.GetPluginMap();
    for (auto const &entry : curPluginMap) {
        auto const &pluginPtr = entry.second;
        if (pluginPtr == nullptr) {
            continue;
        }

        if (pluginName.empty()) {
            pluginPtr->Dump(fd, newCmd);
            continue;
        }

        if (pluginPtr->GetName() == pluginName) {
            pluginPtr->Dump(fd, newCmd);
            break;
        }
    }
}

void HiviewService::DumpDetailedInfo(int fd)
{
    if (parser_ != nullptr) {
        parser_.reset();
    }
    DumpLoadedPluginInfo(fd);
    parser_ = std::make_unique<AuditLogParser>();
    parser_->StartParse();
    std::string timeScope = parser_->GetAuditLogTimeScope();
    dprintf(fd, "%s\n", timeScope.c_str());
    DumpPluginUsageInfo(fd);
    DumpThreadUsageInfo(fd);
    DumpPipelineUsageInfo(fd);
    parser_.reset();
}

void HiviewService::DumpLoadedPluginInfo(int fd) const
{
    auto &platform = HiviewPlatform::GetInstance();
    auto const &curPluginMap = platform.GetPluginMap();
    dprintf(fd, "Current Loaded Plugins:\n");
    for (auto const &entry : curPluginMap) {
        auto const &pluginName = entry.first;
        if (entry.second != nullptr) {
            dprintf(fd, "PluginName:%s ", pluginName.c_str());
            dprintf(fd, "IsDynamic:%s ",
                (entry.second->GetType() == Plugin::PluginType::DYNAMIC) ? "True" : "False");
            dprintf(fd, "Version:%s ", (entry.second->GetVersion().c_str()));
            dprintf(fd,
                "ThreadName:%s\n",
                ((entry.second->GetWorkLoop() == nullptr) ? "Null" : entry.second->GetWorkLoop()->GetName().c_str()));
        }
    }
    dprintf(fd, "Dump Plugin Loaded Info Done.\n\n");
}

void HiviewService::DumpPluginUsageInfo(int fd)
{
    auto &platform = HiviewPlatform::GetInstance();
    auto const &curPluginMap = platform.GetPluginMap();
    for (auto const &entry : curPluginMap) {
        auto pluginName = entry.first;
        if (entry.second != nullptr) {
            DumpPluginUsageInfo(fd, pluginName);
        }
    }
}
void HiviewService::DumpPluginUsageInfo(int fd, const std::string &pluginName) const
{
    if (parser_ == nullptr) {
        return;
    }
    auto logList = parser_->GetPluginSummary(pluginName);
    dprintf(fd, "Following events processed By Plugin %s:\n", pluginName.c_str());
    for (auto &log : logList) {
        dprintf(fd, " %s.\n", log.c_str());
    }
    dprintf(fd, "Dump Plugin Usage Done.\n\n");
}

void HiviewService::DumpThreadUsageInfo(int fd) const
{
    auto &platform = HiviewPlatform::GetInstance();
    auto const &curThreadMap = platform.GetWorkLoopMap();
    dprintf(fd, "Start Dump ThreadInfo:\n");
    for (auto const &entry : curThreadMap) {
        if (entry.second != nullptr) {
            std::string name = entry.second->GetName();
            DumpThreadUsageInfo(fd, name);
        }
    }
    dprintf(fd, "Dump ThreadInfo Done.\n\n");
}

void HiviewService::DumpThreadUsageInfo(int fd, const std::string &threadName) const
{
    if (parser_ == nullptr) {
        return;
    }
    auto logList = parser_->GetThreadSummary(threadName);
    dprintf(fd, "Following events processed on Thread %s:\n", threadName.c_str());
    for (auto &log : logList) {
        dprintf(fd, " %s.\n", log.c_str());
    }
}

void HiviewService::DumpPipelineUsageInfo(int fd) const
{
    auto &platform = HiviewPlatform::GetInstance();
    auto const &curPipelineMap = platform.GetPipelineMap();
    dprintf(fd, "Start Dump Pipeline Info:\n");
    for (auto const &entry : curPipelineMap) {
        auto pipeline = entry.first;
        DumpPipelineUsageInfo(fd, pipeline);
    }
}

void HiviewService::DumpPipelineUsageInfo(int fd, const std::string &pipelineName) const
{
    if (parser_ == nullptr) {
        return;
    }
    auto logList = parser_->GetPipelineSummary(pipelineName);
    dprintf(fd, "Following events processed on Pipeline %s:\n", pipelineName.c_str());
    for (auto &log : logList) {
        dprintf(fd, " %s.\n", log.c_str());
    }
    dprintf(fd, "Dump Pipeline Usage Info Done.\n\n");
}

void HiviewService::PrintUsage(int fd) const
{
    dprintf(fd, "Hiview Plugin Platform dump options:\n");
    dprintf(fd, "hidumper hiviewdfx [-d(etail)]\n");
    dprintf(fd, "    [-p(lugin) pluginName]\n");
}
}  // namespace HiviewDFX
}  // namespace OHOS
