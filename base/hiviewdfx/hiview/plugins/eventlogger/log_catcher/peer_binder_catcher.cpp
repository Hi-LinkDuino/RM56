/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "peer_binder_catcher.h"

#include <memory>
#include <string>
#include <vector>

#include <sys/stat.h>

#include "securec.h"

#include "common_utils.h"
#include "file_util.h"
#include "logger.h"
#include "string_util.h"

#include "open_stacktrace_catcher.h"
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_LABEL(0xD002D01, "EventLogger-PeerBinderCatcher");
PeerBinderCatcher::PeerBinderCatcher() : EventLogCatcher()
{
    name_ = "PeerBinderCatcher";
}

bool PeerBinderCatcher::Initialize(const std::string& strParam1, int pid, int layer)
{
    pid_ = pid;
    layer_ = layer;
    char buf[BUF_SIZE_512] = {0};
    int ret = snprintf_s(buf, BUF_SIZE_512, BUF_SIZE_512 - 1,
        "PeerBinderCatcher -- pid==%d layer_ == %d\n", pid_, layer_);
    if (ret > 0) {
        description_ = buf;
    }
    return true;
}

void PeerBinderCatcher::Init(std::shared_ptr<SysEvent> event, std::string filePath)
{
    event_ = event;
    binderProcPath_ = filePath;
}

int PeerBinderCatcher::Catch(int fd)
{
    if (pid_ <= 0) {
        return -1;
    }
    auto originSize = GetFdSize(fd);
    std::set<int> pids;
    if (layer_ <= LOGGER_BINDER_STACK_ONE) {
        pids = GetBinderPeerChainPids(pid_, layer_, fd);
    } else {
        pids = GetBinderPeerPids(pid_, layer_);
    }
    if (pids.size() == 0) {
        logSize_ = GetFdSize(fd) - originSize;
        return logSize_;
    }

    std::string pidStr = "";
    for (auto pidTemp : pids) {
        if (pidTemp != pid_) {
            auto stackCapture = std::make_unique<OpenStacktraceCatcher>();
            stackCapture->Initialize("", pidTemp, 0);
            stackCapture->Catch(fd);
            pidStr += "," + std::to_string(pidTemp);
        }
    }
    SaveBinderTransactionInfo(fd);
    if (event_ != nullptr) {
        event_->SetValue(LOGGER_EVENT_PEERBINDER, StringUtil::TrimStr(pidStr, ','));
    }

    logSize_ = GetFdSize(fd) - originSize;
    return logSize_;
}

std::set<int> PeerBinderCatcher::GetBinderPeerChainPids(int pid, int layer, int fd) const
{
    std::set<int> pids;
    std::map<int, std::list<BinderInfo>> manager = BinderInfoParser(pid);
    if (manager.size() == 0 || manager.find(pid) == manager.end()) {
        return pids;
    }
    if (layer == LOGGER_BINDER_STACK_ONE) {
        for (auto each : manager[pid]) {
            if (pids.size() == PEER_TRACE_NUM || each.wait < TRACE_THRESHOLD) {
                break;
            }
            pids.insert(each.server);
        }
        pids.insert(pid);
        return pids;
    }
    return GetPeerBinderByLayer(pid, layer, manager, fd);
}

std::map<int, std::list<PeerBinderCatcher::BinderInfo>> PeerBinderCatcher::BinderInfoParser(int pid) const
{
    std::map<int, std::list<BinderInfo>> manager;
    std::string path = GetBinderProcPath() + std::to_string(pid);
    std::ifstream fin;
    fin.open(path.c_str());
    if (!fin.is_open()) {
        HIVIEW_LOGE("open binder file failed, %{public}s.", path.c_str());
        return manager;
    }
    const int WAIT_TIME = 10;
    const int DECIMAL = 10;
    const int SERVCER_PID_NUM = 2;
    const int CLIENT_PID_NUM = 1;
    std::string pattern = "\\s*outgoing transaction.*from\\s*(\\d+):.*to\\s(\\d+):.*";
    std::regex reg(pattern);
    std::string line;
    while (getline(fin, line)) {
        std::smatch match;
        if (!std::regex_match(line, match, reg)) {
            continue;
        }
        BinderInfo info;
        info.wait = WAIT_TIME;
        // 2: binder peer id, 10:dec
        info.server = std::strtol(std::string(match[SERVCER_PID_NUM]).c_str(), nullptr, DECIMAL);
        // 1: binder local id, 10:dec
        info.client = std::strtol(std::string(match[CLIENT_PID_NUM]).c_str(), nullptr, DECIMAL);
        manager[info.client].push_back(info);
    }
    HIVIEW_LOGI("manager size: %{public}zu", manager.size());
    for (auto each : manager) {
        each.second.sort([](const BinderInfo &lVal, const BinderInfo &rVal) {
            return lVal.wait > rVal.wait; // desc list
        });
        manager[each.first] = each.second;
    }
    return manager;
}

std::set<int> PeerBinderCatcher::GetPeerBinderByLayer (int pid, int layer,
    std::map<int, std::list<BinderInfo>> info, int fd) const
{
    std::set<int> pids;
    std::vector<int> path;
    std::vector<std::vector<int>> paths;
    std::map<int, bool> visit;
    FileUtil::SaveStringToFd(fd, "\n\nbinder call chain:\n");
    ParseBinderCallChain(pid, info, path, visit, paths);
    for (const auto each : paths) {
        FileUtil::SaveStringToFd(fd, "\t");
        for (const auto e : each) {
            FileUtil::SaveStringToFd(fd, std::to_string(e) + "->");
        }
        FileUtil::SaveStringToFd(fd, "...\n");
    }
    if (layer == LOGGER_BINDER_STACK_ALL) {
        for (auto keyValue : visit) {
            pids.insert(keyValue.first);
        }
    }
    if (layer == LOGGER_BINDER_STACK_HEAD_TAIL) {
        for (auto onePath : paths) {
            /* first is pid, use second */
            onePath.erase(onePath.begin());
            if (onePath.size() == 0) {
                continue;
            }
            pids.insert(onePath.front());
            pids.insert(onePath.back());
        }
        pids.insert(pid);
    }
    return pids;
}

void PeerBinderCatcher::ParseBinderCallChain(int pid, std::map<int, std::list<BinderInfo>> info,
    std::vector<int> path, std::map<int, bool> &visit, std::vector<std::vector<int>> &paths) const
{
    if (visit[pid]) {
        paths.push_back(path);
        return;
    }
    visit[pid] = true;
    path.push_back(pid);
    for (auto each : info[pid]) {
        ParseBinderCallChain(each.server, info, path, visit, paths);
    }
    if (info[pid].empty()) {
        paths.push_back(path);
    }
}

std::set<int> PeerBinderCatcher::GetBinderPeerPids(int pid, long layer) const
{
    std::set<int> pids;
    int pidTemp = pid;
    int tidTemp = pid;
    int waitTemp = 0;
    std::string matchLine = "";
    for (long i = 0; i < layer; i++) {
        if (GetMatchBinderPeerInfo(pidTemp, tidTemp, waitTemp, matchLine) != 0) {
            break;
        }
        if (waitTemp < BINDER_TIMEOUT) {
            break;
        }
        pids.insert(pidTemp);
    }
    return pids;
}

int PeerBinderCatcher::GetMatchBinderPeerInfo(int &pid, int &tid, int &waitTime, std::string &matchLine) const
{
    std::ifstream contentFile;
    int ret = -1;
    std::string path = GetBinderProcPath() + std::to_string(pid);
    contentFile.open(path.c_str());
    if (!contentFile.is_open()) {
        HIVIEW_LOGE("open binder file failed,get binder failed, %{public}s", path.c_str());
        return ret;
    }

    std::string pattern = "\\s*outgoing transaction.*from\\s*" +
        std::to_string(pid) + ":" + std::to_string(tid) + "\\s*to\\s*(\\d+):(\\d+)\\s*.*";

    std::regex reg(pattern);
    std::string lineContent;
    const int WAIT_TIME = 10;
    const int DECIMAL = 10;
    const int PID_NUM = 1;
    const int TID_NUM = 2;
    while (getline(contentFile, lineContent)) {
        if (StringUtil::TrimStr(lineContent) == "") {
            continue;
        }
        std::smatch result;
        if (!std::regex_search(lineContent, result, reg)) { // 4: match num
            continue;
        }
        pid = static_cast<int>(std::strtol(StringUtil::TrimStr(result[PID_NUM]).c_str(), nullptr, DECIMAL));
        tid = static_cast<int>(std::strtol(StringUtil::TrimStr(result[TID_NUM]).c_str(), nullptr, DECIMAL));
        waitTime = WAIT_TIME;
        matchLine = lineContent;
        ret = 0;
        break;
    }
    return ret;
}

void PeerBinderCatcher::SaveBinderTransactionInfo(int fd) const
{
    std::ifstream fin;
    std::string path = GetBinderProcPath();
    fin.open(path.c_str());
    if (!fin.is_open()) {
        HIVIEW_LOGE("open binder file failed, %{public}s.", path.c_str());
        return;
    }
    std::string line;
    FileUtil::SaveStringToFd(fd, "\nbinder transaction info:\n");
    while (getline(fin, line)) {
        FileUtil::SaveStringToFd(fd, line + "\n");
    }
}

std::string PeerBinderCatcher::GetBinderProcPath() const
{
    if (binderProcPath_ != "") {
        return binderProcPath_;
    }
    if (FileUtil::FileExists(LOGGER_BINDER_DEBUG_PROC_PATH)) {
        return LOGGER_BINDER_DEBUG_PROC_PATH;
    }
    return LOGGER_BINDER_TRACING_PROC_PATH;
}
} // namespace HiviewDFX
} // namespace OHOS