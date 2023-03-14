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
#include "faultlogger.h"

#include <climits>
#include <cstdint>
#include <ctime>
#include <memory>
#include <regex>
#include <string>
#include <vector>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "common_utils.h"
#include "constants.h"
#include "event.h"
#include "file_util.h"
#include "logger.h"
#include "parameter_ex.h"
#include "plugin_factory.h"
#include "string_util.h"
#include "sys_event.h"
#include "sys_event_dao.h"
#include "time_util.h"

#include "faultlog_formatter.h"
#include "faultlog_info.h"
#include "faultlog_query_result_inner.h"
#include "faultlog_util.h"
#include "faultlogger_adapter.h"

namespace OHOS {
namespace HiviewDFX {
REGISTER(Faultlogger);
DEFINE_LOG_TAG("Faultlogger");
using namespace FaultLogger;
namespace {
constexpr char FILE_SEPERATOR[] = "******";
constexpr uint32_t DUMP_MAX_NUM = 100;
constexpr int32_t MAX_QUERY_NUM = 100;
constexpr int ROOT_UID = 0;
constexpr int DUMP_PARSE_CMD = 0;
constexpr int DUMP_PARSE_FILE_NAME = 1;
constexpr int DUMP_PARSE_TIME = 2;
constexpr int DUMP_START_PARSE_MODULE_NAME = 3;
constexpr uint32_t MAX_NAME_LENGTH = 4096;
constexpr char TEMP_LOG_PATH[] = "/data/log/faultlog/temp";
DumpRequest InitDumpRequest()
{
    DumpRequest request;
    request.requestDetail = false;
    request.requestList = false;
    request.fileName = "";
    request.moduleName = "";
    request.time = -1;
    return request;
}

bool IsNameValid(const std::string& name, const std::string& sep, bool canEmpty)
{
    std::vector<std::string> nameVec;
    StringUtil::SplitStr(name, sep, nameVec, canEmpty, false);
    std::regex re("^[a-zA-Z][a-zA-Z0-9_]*$");
    for (auto const& splitName : nameVec) {
        if (!std::regex_match(splitName, re)) {
            HIVIEW_LOGI("Invalid splitName:%{public}s", splitName.c_str());
            return false;
        }
    }
    return true;
}

bool IsModuleNameValid(const std::string& name)
{
    if (name.empty() || name.size() > MAX_NAME_LENGTH) {
        HIVIEW_LOGI("invalid log name.");
        return false;
    }

    if (name.find("/") != std::string::npos || name.find(".") == std::string::npos) {
        std::string path = name.substr(1); // may skip first .
        path.erase(path.find_last_not_of(" \n\r\t") + 1);
        HIVIEW_LOGI("module name:%{public}s", name.c_str());
        return IsNameValid(path, "/", false);
    }

    return IsNameValid(name, ".", true);
}

bool IsLogNameValid(const std::string& name)
{
    if (name.empty() || name.size() > MAX_NAME_LENGTH) {
        HIVIEW_LOGI("invalid log name.");
        return false;
    }

    std::vector<std::string> out;
    StringUtil::SplitStr(name, "-", out, true, false);
    if (out.size() != 4) { // FileName LogType-ModuleName-uid-YYYYMMDDHHMMSS, thus contains 4 sections
        return false;
    }

    std::regex reType("^[a-z]+$");
    if (!std::regex_match(out[0], reType)) { // 0 : type section
        HIVIEW_LOGI("invalid type.");
        return false;
    }

    if (!IsModuleNameValid(out[1])) { // 1 : module section
        HIVIEW_LOGI("invalid module name.");
        return false;
    }

    std::regex reDigits("^[0-9]*$");
    if (!std::regex_match(out[2], reDigits)) { // 2 : uid section
        HIVIEW_LOGI("invalid uid.");
        return false;
    }

    if (!std::regex_match(out[3], reDigits)) { // 3 : time section
        HIVIEW_LOGI("invalid digits.");
        return false;
    }
    return true;
}

bool FillDumpRequest(DumpRequest &request, int status, const std::string &item)
{
    switch (status) {
        case DUMP_PARSE_FILE_NAME:
            if (!IsLogNameValid(item)) {
                return false;
            }
            request.fileName = item;
            break;
        case DUMP_PARSE_TIME:
            if (item.size() == 14) { // 14 : BCD time size
                request.time = TimeUtil::StrToTimeStamp(item, "%Y%m%d%H%M%S");
            } else {
                StringUtil::ConvertStringTo<time_t>(item, request.time);
            }
            break;
        case DUMP_START_PARSE_MODULE_NAME:
            if (!IsModuleNameValid(item)) {
                return false;
            }
            request.moduleName = item;
            break;
        default:
            HIVIEW_LOGI("Unknown status.");
            break;
    }
    return true;
}

std::string GetSummaryFromSectionMap(int32_t type, const std::map<std::string, std::string>& maps)
{
    std::string key = "";
    switch (type) {
        case JAVA_CRASH:
            key = "TRUSTSTACK";
            break;
        case CPP_CRASH:
            key = "KEY_THREAD_INFO";
            break;
        default:
            break;
    }

    if (key.empty()) {
        return "";
    }

    auto value = maps.find(key);
    if (value == maps.end()) {
        return "";
    }
    return value->second;
}
} // namespace

void Faultlogger::AddPublicInfo(FaultLogInfo &info)
{
    info.sectionMap["DEVICE_INFO"] = Parameter::GetString("ro.product.name", "Unknown");
    info.sectionMap["BUILD_INFO"] = Parameter::GetString("ro.build.display.id", "Unknown");
    info.sectionMap["UID"] = std::to_string(info.id);
    info.sectionMap["PID"] = std::to_string(info.pid);
    info.sectionMap["PACKAGE"] = info.module;
    if (info.id != 0) {
        if (info.sectionMap["APPVERSION"].empty()) {
            info.sectionMap["APPVERSION"] = GetApplicationVersion(info.id, info.module);
        }
    } else {
        info.module = RegulateModuleNameIfNeed(info.module);
    }

    if (info.reason.empty()) {
        info.reason = info.sectionMap["REASON"];
    } else {
        info.sectionMap["REASON"] = info.reason;
    }

    if (info.summary.empty()) {
        info.summary = GetSummaryFromSectionMap(info.faultLogType, info.sectionMap);
    } else {
        info.sectionMap["SUMMARY"] = info.summary;
    }
}

void Faultlogger::Dump(int fd, const std::vector<std::string> &cmds)
{
    auto request = InitDumpRequest();
    int32_t status = DUMP_PARSE_CMD;
    for (auto it = cmds.begin(); it != cmds.end(); it++) {
        if ((*it) == "-f") {
            status = DUMP_PARSE_FILE_NAME;
            continue;
        } else if ((*it) == "-l") {
            request.requestList = true;
            continue;
        } else if ((*it) == "-t") {
            status = DUMP_PARSE_TIME;
            continue;
        } else if ((*it) == "-m") {
            status = DUMP_START_PARSE_MODULE_NAME;
            continue;
        } else if ((*it) == "-d") {
            request.requestDetail = true;
            continue;
        } else if ((*it) == "Faultlogger") {
            // skip first params
            continue;
        } else if ((!(*it).empty()) && ((*it).at(0) == '-')) {
            dprintf(fd, "Unknown command.\n");
            return;
        }

        if (!FillDumpRequest(request, status, *it)) {
            dprintf(fd, "invalid parameters.\n");
            return;
        }
        status = DUMP_PARSE_CMD;
    }

    if (status != DUMP_PARSE_CMD) {
        dprintf(fd, "empty parameters.\n");
        return;
    }

    HIVIEW_LOGI("DumpRequest: detail:%d, list:%d, file:%s, name:%s, time:%ld",
                request.requestDetail, request.requestList, request.fileName.c_str(), request.moduleName.c_str(),
                request.time);
    Dump(fd, request);
}

void Faultlogger::Dump(int fd, const DumpRequest &request) const
{
    if (!request.fileName.empty()) {
        std::string content;
        if (mgr_->GetFaultLogContent(request.fileName, content)) {
            dprintf(fd, "%s\n", content.c_str());
        } else {
            dprintf(fd, "Fail to dump the log.\n");
        }
        return;
    }

    auto fileList = mgr_->GetFaultLogFileList(request.moduleName, request.time, -1, 0, DUMP_MAX_NUM);
    if (fileList.empty()) {
        dprintf(fd, "No fault log exist.\n");
        return;
    }

    dprintf(fd, "Fault log list:\n");
    dprintf(fd, "%s\n", FILE_SEPERATOR);
    for (auto &file : fileList) {
        std::string fileName = FileUtil::ExtractFileName(file);
        dprintf(fd, "%s\n", fileName.c_str());
        if (request.requestDetail) {
            std::string content;
            if (FileUtil::LoadStringFromFile(file, content)) {
                dprintf(fd, "%s\n", content.c_str());
            } else {
                dprintf(fd, "Fail to dump detail log.\n");
            }
            dprintf(fd, "%s\n", FILE_SEPERATOR);
        }
    }
    dprintf(fd, "%s\n", FILE_SEPERATOR);
}

bool Faultlogger::OnEvent(std::shared_ptr<Event> &event)
{
    if (!hasInit_) {
        return false;
    }

    if (event == nullptr) {
        return false;
    }

    if (event->eventName_ == "JS_ERROR") {
        if (event->jsonExtraInfo_.empty()) {
            return false;
        }

        FaultLogInfo info;
        auto sysEvent = std::static_pointer_cast<SysEvent>(event);
        info.time = sysEvent->happenTime_ / 1000; // 1000 : millsecond to second
        info.id = sysEvent->GetUid();
        info.pid = sysEvent->GetPid();
        info.faultLogType = FaultLogType::JS_CRASH;
        info.module = sysEvent->GetEventValue("PACKAGE_NAME");
        info.reason = sysEvent->GetEventValue("REASON");
        auto summary = sysEvent->GetEventValue("SUMMARY");
        info.summary = StringUtil::ReplaceStr(summary, "\\n", "\n");
        info.sectionMap = sysEvent->GetKeyValuePairs();
        AddFaultLog(info);

        EventStore::SysEventQuery eventQuery = EventStore::SysEventDao::BuildQuery();
        EventStore::ResultSet set = eventQuery.Select( {EventStore::EventCol::TS} )
            .Where(EventStore::EventCol::TS, EventStore::Op::EQ, static_cast<int64_t>(event->happenTime_))
            .And(EventStore::EventCol::DOMAIN, EventStore::Op::EQ, sysEvent->domain_)
            .And(EventStore::EventCol::NAME, EventStore::Op::EQ, sysEvent->eventName_)
            .Execute();
        if (set.GetErrCode() != 0) {
            HIVIEW_LOGE("failed to get db, error:%{public}d.", set.GetErrCode());
            return false;
        }
        if (set.HasNext()) {
            auto record = set.Next();
            if (record->GetSeq() == sysEvent->GetSeq()) {
                HIVIEW_LOGI("Seq match success, info.logPath %{public}s", info.logPath.c_str());
                sysEvent->SetEventValue("FAULT_TYPE", std::to_string(info.faultLogType));
                sysEvent->SetEventValue("MODULE", info.module);
                sysEvent->SetEventValue("LOG_PATH", info.logPath);
                auto retCode = EventStore::SysEventDao::Update(sysEvent, false);
                if (retCode == 0) {
                    return true;
                }
            }
        }
        
        HIVIEW_LOGE("eventLog LogPath update to DB failed!");
        return false;
    }
    return true;
}

bool Faultlogger::CanProcessEvent(std::shared_ptr<Event> event)
{
    return true;
}

bool Faultlogger::ReadyToLoad()
{
    return true;
}

void Faultlogger::OnLoad()
{
    mgr_ = std::make_unique<FaultLogManager>(GetHiviewContext()->GetSharedWorkLoop());
    mgr_->Init();
    hasInit_ = true;
#ifndef UNITTEST
    FaultloggerAdapter::StartService(this);
#endif

    // some crash happend before hiview start, ensure every crash event is added into eventdb
    auto eventloop = GetHiviewContext()->GetSharedWorkLoop();
    if (eventloop != nullptr) {
        auto task = std::bind(&Faultlogger::StartBootScan, this);
        eventloop->AddTimerEvent(nullptr, nullptr, task, 10, false); // delay 10 seconds
    }
}

void Faultlogger::AddFaultLog(FaultLogInfo& info)
{
    if (!hasInit_) {
        return;
    }

    AddFaultLogIfNeed(info, nullptr);
}

std::unique_ptr<FaultLogInfo> Faultlogger::GetFaultLogInfo(const std::string &logPath)
{
    if (!hasInit_) {
        return nullptr;
    }

    auto info = std::make_unique<FaultLogInfo>(FaultLogger::ParseFaultLogInfoFromFile(logPath));
    info->logPath = logPath;
    return info;
}

std::unique_ptr<FaultLogQueryResultInner> Faultlogger::QuerySelfFaultLog(int32_t id,
    int32_t pid, int32_t faultType, int32_t maxNum)
{
    if (!hasInit_) {
        return nullptr;
    }

    if ((faultType < FaultLogType::ALL) || (faultType > FaultLogType::SYS_FREEZE)) {
        HIVIEW_LOGW("Unsupported fault type");
        return nullptr;
    }

    if (maxNum < 0 || maxNum > MAX_QUERY_NUM) {
        maxNum = MAX_QUERY_NUM;
    }

    std::string name = "";
    if (id != ROOT_UID) {
        auto appNames = GetApplicationNamesById(id);
        if (!appNames.empty()) {
            name = appNames.front();
        } else {
            name = CommonUtils::GetProcNameByPid(pid);
        }
    }
    return std::make_unique<FaultLogQueryResultInner>(mgr_->GetFaultInfoList(name, id, faultType, maxNum));
}

void Faultlogger::AddFaultLogIfNeed(FaultLogInfo& info, std::shared_ptr<Event> event)
{
    if ((info.faultLogType <= FaultLogType::ALL) || (info.faultLogType > FaultLogType::SYS_FREEZE)) {
        HIVIEW_LOGW("Unsupported fault type");
        return;
    }

    std::string appName = GetApplicationNameById(info.id);
    if (!appName.empty()) {
        info.module = appName;
    }

    HIVIEW_LOGD("nameProc %{public}s", info.module.c_str());
    if ((info.module.empty()) || (!IsModuleNameValid(info.module))) {
        HIVIEW_LOGW("Invalid module name %{public}s", info.module.c_str());
        return;
    }

    AddPublicInfo(info);
    mgr_->SaveFaultLogToFile(info);
    if (info.faultLogType != FaultLogType::JS_CRASH) {   
        mgr_->SaveFaultInfoToRawDb(info);
    }
    HIVIEW_LOGI("\nSave Faultlog of Process:%{public}d\n"
                "ModuleName:%{public}s\n"
                "Reason:%{public}s\n"
                "Summary:%{public}s\n",
                info.pid,
                info.module.c_str(),
                info.reason.c_str(),
                info.summary.c_str());
}

void Faultlogger::OnUnorderedEvent(const Event &msg)
{
}

std::string Faultlogger::GetListenerName()
{
    return "FaultLogger";
}

void Faultlogger::StartBootScan()
{
    std::vector<std::string> files;
    FileUtil::GetDirFiles(TEMP_LOG_PATH, files);
    for (const auto& file : files) {
        auto info = ParseFaultLogInfoFromFile(file, true);
        if (mgr_->IsProcessedFault(info.pid, info.id, info.faultLogType)) {
            HIVIEW_LOGI("Skip processed fault.(%{public}d:%{public}d) ", info.pid, info.id);
            continue;
        }
        AddFaultLogIfNeed(info, nullptr);
    }
}
} // namespace HiviewDFX
} // namespace OHOS
