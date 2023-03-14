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
#include "faultlog_formatter.h"

#include <cstdint>
#include <fstream>
#include <list>
#include <sstream>
#include <string>

#include "faultlog_info.h"
#include "faultlog_util.h"
#include "file_util.h"
#include "string_util.h"

namespace OHOS {
namespace HiviewDFX {
namespace FaultLogger {
constexpr int LOG_MAP_KEY = 0;
constexpr int LOG_MAP_VALUE = 1;
static const char *DEVICE_INFO[] = {"DEVICE_INFO", "Device info:"};
static const char *BUILD_INFO[] = {"BUILD_INFO", "Build info:"};
static const char *MODULE_NAME[] = {"PACKAGE", "Module name:"};
static const char *PROCESS_NAME[] = {"PNAME", "Process name:"};
static const char *MODULE_PID[] = {"PID", "Pid:"};
static const char *MODULE_UID[] = {"UID", "Uid:"};
static const char *MODULE_VERSION[] = {"APPVERSION", "Version:"};
static const char *FAULT_TYPE[] = {"FAULT_TYPE", "Fault type:"};
static const char *SYSVMTYPE[] = {"SYSVMTYPE", "SYSVMTYPE:"};
static const char *APPVMTYPE[] = {"APPVMTYPE", "APPVMTYPE:"};
static const char *FOREGROUND[] = {"FG", "Foreground:"};
static const char *LIFETIME[] = {"LIFTIME", "Up time:"};
static const char *REASON[] = {"REASON", "Reason:"};
static const char *FAULT_MESSAGE[] = {"FAULT_MESSAGE", "Fault message:"};
static const char *STACKTRACE[] = {"TRUSTSTACK", "Selected stacktrace:\n"};
static const char *ROOT_CAUSE[] = {"BINDERMAX", "Blocked chain:\n"};
static const char *MSG_QUEUE_INFO[] = {"MSG_QUEUE_INFO", "Message queue info:\n"};
static const char *BINDER_TRANSACTION_INFO[] = {"BINDER_TRANSACTION_INFO", "Binder transaction info:\n"};
static const char *PROCESS_STACKTRACE[] = {"PROCESS_STACKTRACE", "Process stacktrace:\n"};
static const char *OTHER_THREAD_INFO[] = {"OTHER_THREAD_INFO", "Other thread info:\n"};
static const char *KEY_THREAD_INFO[] = {"KEY_THREAD_INFO", "Fault thread Info:\n"};
static const char *KEY_THREAD_REGISTERS[] = {"KEY_THREAD_REGISTERS", "Registers:\n"};
static const char *MEMORY_USAGE[] = {"MEM_USAGE", "Memory Usage:\n"};
static const char *CPU_USAGE[] = {"FAULTCPU", "CPU Usage:"};
static const char *TRACE_ID[] = {"TRACEID", "Trace-Id:"};
static const char *SUMMARY[] = {"SUMMARY", "Summary:\n"};

auto JAVA_CRASH_LOG_SEQUENCE = {
    DEVICE_INFO, BUILD_INFO, MODULE_NAME, MODULE_VERSION, MODULE_PID, MODULE_UID,
    FAULT_TYPE,  FOREGROUND, LIFETIME,    REASON,         TRACE_ID,   STACKTRACE
};

auto CPP_CRASH_LOG_SEQUENCE = {
    DEVICE_INFO,      BUILD_INFO, MODULE_NAME, MODULE_VERSION, MODULE_PID, MODULE_UID,      FAULT_TYPE,
    SYSVMTYPE,        APPVMTYPE,  REASON,      FAULT_MESSAGE,  TRACE_ID,   PROCESS_NAME,    KEY_THREAD_INFO,
    KEY_THREAD_REGISTERS, OTHER_THREAD_INFO
};

auto JAVASCRIPT_CRASH_LOG_SEQUENCE = {
    DEVICE_INFO, BUILD_INFO, MODULE_NAME,   MODULE_VERSION, MODULE_PID,
    MODULE_UID,  FAULT_TYPE, FAULT_MESSAGE, SYSVMTYPE,      APPVMTYPE,
    FOREGROUND,  LIFETIME,   REASON,        TRACE_ID
};

auto APP_FREEZE_LOG_SEQUENCE = {
    DEVICE_INFO, BUILD_INFO, MODULE_NAME, MODULE_VERSION, MODULE_PID,
    MODULE_UID, FAULT_TYPE, SYSVMTYPE, APPVMTYPE, REASON, SUMMARY,
    TRACE_ID, CPU_USAGE, MEMORY_USAGE, ROOT_CAUSE, STACKTRACE,
    MSG_QUEUE_INFO, BINDER_TRANSACTION_INFO, PROCESS_STACKTRACE
};

auto SYS_FREEZE_LOG_SEQUENCE = {
    DEVICE_INFO, BUILD_INFO, MODULE_NAME, MODULE_VERSION, MODULE_PID,
    MODULE_UID, FAULT_TYPE, SYSVMTYPE, APPVMTYPE, REASON, SUMMARY,
    TRACE_ID, CPU_USAGE, MEMORY_USAGE, ROOT_CAUSE, STACKTRACE,
    MSG_QUEUE_INFO, BINDER_TRANSACTION_INFO, PROCESS_STACKTRACE
};

std::list<const char **> GetLogParseList(int32_t logType)
{
    switch (logType) {
        case FaultLogType::JAVA_CRASH:
            return JAVA_CRASH_LOG_SEQUENCE;
        case FaultLogType::CPP_CRASH:
            return CPP_CRASH_LOG_SEQUENCE;
        case FaultLogType::JS_CRASH:
            return JAVASCRIPT_CRASH_LOG_SEQUENCE;
        case FaultLogType::APP_FREEZE:
            return APP_FREEZE_LOG_SEQUENCE;
        case FaultLogType::SYS_FREEZE:
            return SYS_FREEZE_LOG_SEQUENCE;
        default:
            return std::list<const char **>();
    }
}

std::string GetSummaryByType(int32_t logType, std::map<std::string, std::string> sections)
{
    std::string summary = "";
    switch (logType) {
        case FaultLogType::JAVA_CRASH:
        case FaultLogType::JS_CRASH:
        case FaultLogType::APP_FREEZE:
        case FaultLogType::SYS_FREEZE:
            summary = sections[STACKTRACE[LOG_MAP_KEY]];
            break;
        case FaultLogType::CPP_CRASH:
            summary = sections[KEY_THREAD_INFO[LOG_MAP_KEY]];
            break;
        default:
            summary = "Could not figure out summary for this fault.";
            break;
    }

    return summary;
}

bool ParseFaultLogLine(const std::list<const char **>& parseList, const std::string& line, const std::string& multline,
    std::string& multlineName, FaultLogInfo& info)
{
    for (auto &item : parseList) {
        std::string sectionHead = std::string(item[LOG_MAP_VALUE], strlen(item[LOG_MAP_VALUE]) - 1);
        if (line.find(sectionHead) != std::string::npos) {
            if (line.at(line.size() - 1) == ':') {
                if ((item[LOG_MAP_KEY] != multlineName) && (!multline.empty())) {
                    info.sectionMap[multlineName] = multline;
                }
                multlineName = item[LOG_MAP_KEY];
            } else {
                info.sectionMap[item[LOG_MAP_KEY]] = line.substr(line.find_first_of(":") + 1);
            }
            return false;
        }
    }
    return true;
}

void WriteStackTraceFromLog(int32_t fd, const std::string& pidStr, const std::string& path)
{
    std::string realPath;
    if (!FileUtil::PathToRealPath(path, realPath)) {
        FileUtil::SaveStringToFd(fd, "Log file not exist.\n");
        return;
    }

    std::ifstream logFile(realPath);
    std::string line;
    bool startWrite = false;
    while (std::getline(logFile, line)) {
        if (!logFile.good()) {
            break;
        }

        if (line.empty()) {
            continue;
        }

        if ((line.find("----- pid") != std::string::npos) &&
            (line.find(pidStr) != std::string::npos)) {
            startWrite = true;
        }

        if ((line.find("----- end") != std::string::npos) &&
            (line.find(pidStr) != std::string::npos)) {
            FileUtil::SaveStringToFd(fd, line + "\n");
            break;
        }

        if (startWrite) {
            FileUtil::SaveStringToFd(fd, line + "\n");
        }
    }
}

void WriteFaultLogToFile(int32_t fd, int32_t logType, std::map<std::string, std::string> sections)
{
    std::list<const char **> seq = GetLogParseList(logType);
    for (auto &item : seq) {
        auto value = sections[item[LOG_MAP_KEY]];
        if (!value.empty()) {
            FileUtil::SaveStringToFd(fd, item[LOG_MAP_VALUE]);
            if (value.back() != '\n') {
                value.append("\n");
            }
            FileUtil::SaveStringToFd(fd, value);
        }
    }

    auto summary = sections[SUMMARY[LOG_MAP_KEY]];
    if (!summary.empty()) {
        FileUtil::SaveStringToFd(fd, summary);
    }

    if (!sections["KEYLOGFILE"].empty()) {
        FileUtil::SaveStringToFd(fd, "Additional Logs:\n");
        WriteStackTraceFromLog(fd, sections["PID"], sections["KEYLOGFILE"]);
    }
}

static void UpdateFaultLogInfoFromTempFile(FaultLogInfo& info)
{
    if (!info.module.empty()) {
        return;
    }

    StringUtil::ConvertStringTo<int32_t>(info.sectionMap[MODULE_UID[LOG_MAP_KEY]], info.id);
    info.module = info.sectionMap[PROCESS_NAME[LOG_MAP_KEY]];
    info.reason = info.sectionMap[REASON[LOG_MAP_KEY]];
    info.summary = info.sectionMap[KEY_THREAD_INFO[LOG_MAP_KEY]];
    info.sectionMap.clear();
}

FaultLogInfo ParseFaultLogInfoFromFile(const std::string &path, bool isTempFile)
{
    auto fileName = FileUtil::ExtractFileName(path);
    FaultLogInfo info;
    if (!isTempFile) {
        info = ExtractInfoFromFileName(fileName);
    } else {
        info = ExtractInfoFromTempFile(fileName);
    }

    auto parseList = GetLogParseList(info.faultLogType);
    std::ifstream logFile(path);
    std::string line;
    std::string multline;
    std::string multlineName;
    while (std::getline(logFile, line)) {
        if (!logFile.good()) {
            break;
        }

        if (line.empty()) {
            continue;
        }

        if (ParseFaultLogLine(parseList, line, multline, multlineName, info)) {
            multline.append(line).append("\n");
        }
    }

    if (!multline.empty() && !multlineName.empty()) {
        info.sectionMap[multlineName] = multline;
    }
    UpdateFaultLogInfoFromTempFile(info);
    return info;
}

bool WriteLogToFile(int32_t fd, const std::string& path)
{
    if ((fd < 0) || path.empty()) {
        return false;
    }

    std::string line;
    std::ifstream logFile(path);
    while (std::getline(logFile, line)) {
        if (!logFile.good()) {
            return false;
        }
        FileUtil::SaveStringToFd(fd, line);
        FileUtil::SaveStringToFd(fd, "\n");
    }
    return true;
}
} // namespace FaultLogger
} // namespace HiviewDFX
} // namespace OHOS
