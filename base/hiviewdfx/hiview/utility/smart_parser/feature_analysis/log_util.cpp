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
#include "log_util.h"

#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <unistd.h>

#include "file_util.h"
#include "logger.h"
#include "string_util.h"

using namespace std;
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("LogUtil");

namespace {
    const string ARROW = "->";
    const string CODE = "code";
    const string WAIT = "wait";
    const int BUF_LEN_2048 = 2048;
    const int TOTAL_SKIP_NUM = 4;
}

const std::string LogUtil::SPLIT_PATTERN = "\n";
const std::string LogUtil::SMART_PARSER_TEST_DIR = "/data/test/test_data/SmartParser";
const int LogUtil::TOTAL_LINE_NUM = 200;

/* ParseIpcInfo function:
 * INPUT :
 *  buffer : log buffer
 *  offset : buffer seekg
 *  pidTid : pid:tid input in Ipc transactions
 * OUTPUT :
 *  pair<int, int> : it is pid:tid which input pid:tid is waiting for
 *  vector<string> : it is Ipc string line which show Ipc transactions between input and output
 *  string : it is simplify Ipc list of output for vector<string>
 */
tuple<pair<int, int>, vector<string>> LogUtil::ParseIpcInfo(stringstream& buffer,
    const int offset, const pair<int, int>& pidTid)
{
    ReadIpcInfo(buffer, offset);
    if (IpcInfo_.empty()) {
        return tuple<pair<int, int>, vector<string>> {};
    }

    vector<IpcTrans> IpcNode;
    string clientPidPid = to_string(pidTid.first) + ":" + to_string(pidTid.second);
    // situation 1.tid of IpcTrans is zero && situation 2.multiIpc
    ParseIpcList(clientPidPid, IpcNode);

    if (IpcNode.empty()) {
        return tuple<pair<int, int>, vector<string>> {};
    }

    // assigneValue to output
    vector<string> resultIpcLine;
    for (const auto& Ipc : IpcNode) {
        resultIpcLine.push_back(Ipc.line);
    }
    size_t size = resultIpcLine.size();
    if (size >= 2 && resultIpcLine.back() == resultIpcLine[size - 2]) { // 2 : last two
        resultIpcLine[size - 1] = "Can't find which process holdes Ipc mostly";
    }

    string serverPidTid = IpcNode.back().sIpcItem.pidTid;
    int pid = StringUtil::StrToInt(StringUtil::GetLeftSubstr(serverPidTid, ":"));
    int tid = StringUtil::StrToInt(StringUtil::GetRightSubstr(serverPidTid, ":"));
    pair<int, int> pidTidResult = make_pair(pid, tid);

    return tuple<pair<int, int>, vector<string>>(pidTidResult, resultIpcLine);
}

void LogUtil::ReadIpcInfo(stringstream& buffer, const int offset)
{
    buffer.seekg(offset, ios::beg);
    string line;
    while (getline(buffer, line)) {
        if (line.find("async") == string::npos && line.find(ARROW) != string::npos) {
            GetIpcInfo(line);
        }
        if (line.empty() || line.find("end Ipc transactions") != string::npos) {
            break;
        }
    }
}

IpcItem LogUtil::ParseIpcStr(string IpcStr) const
{
    IpcItem item;
    IpcStr = StringUtil::EraseString(IpcStr, " ");
    string csIpc = StringUtil::EraseString(IpcStr, "\t");
    item.pidTid = StringUtil::GetLeftSubstr(csIpc, "(");
    string rest = StringUtil::GetRightSubstr(csIpc, "(");
    item.packageName = StringUtil::GetLeftSubstr(rest, ":");
    item.threadName = StringUtil::GetMidSubstr(rest, ":", ")");
    return item;
}

/*
 * example of Ipc transactions:
 * 26920:26920(m.Ipc.phone:m.Ipc.phone) -> 1477:2146(system_server:Ipc:1477_A) code 2b wait:7.200053127 s
 * [async] 1477:1973(system_server:Ipc:1477_7) -> 2148:0(awei.HwOPServer:unknown) code 1 wait:37.375423970 s
 * ......
 * return fail show Ipc transaction doesn't conform to the format below; parse fail.
 * return true show parse correctly. ingore [async] Ipc transactions
 */
void LogUtil::GetIpcInfo(const string& line)
{
    string serverIpc = StringUtil::GetMidSubstr(line, ARROW, CODE);
    string codeStr = StringUtil::GetMidSubstr(line, CODE, WAIT);
    if (!serverIpc.empty() && !codeStr.empty()) {
        string clientIpc = StringUtil::GetLeftSubstr(line, ARROW);
        string timeStr = StringUtil::GetRightSubstr(line, WAIT);
        IpcTrans bindStr;
        bindStr.cIpcItem = ParseIpcStr(clientIpc);
        bindStr.sIpcItem = ParseIpcStr(serverIpc);
        bindStr.code = StringUtil::EraseString(codeStr, " ");
        timeStr = StringUtil::GetMidSubstr(timeStr, ":", " ");
        bindStr.waitTime = StringUtil::StringToDouble(timeStr);
        bindStr.line = line;
        pair<string, IpcTrans> IpcPair(bindStr.sIpcItem.pidTid, bindStr);
        IpcInfo_.insert(make_pair(bindStr.cIpcItem.pidTid, IpcPair));
    }
}

// situation 1:thread pool have exhausted, serverTid is zero
IpcTrans LogUtil::ParseZeroIpc(const IpcTrans& beginIpc) const
{
    string serverPidMatch = StringUtil::GetLeftSubstr(beginIpc.sIpcItem.pidTid, ":");
    map<double, IpcTrans> timeIpc;
    for (auto it = IpcInfo_.begin(); it != IpcInfo_.end(); it++) {
        string serverPidTid = it->second.first;
        string serverTid = StringUtil::GetRightSubstr(serverPidTid, ":");
        string serverPid = StringUtil::GetLeftSubstr(serverPidTid, ":");
        if (serverPid == serverPidMatch && serverTid != "0") {
            IpcTrans exhaustIpc = it->second.second;
            timeIpc.insert(make_pair(exhaustIpc.waitTime, exhaustIpc));
        }
    }
    // all tid is 0
    if (timeIpc.empty()) {
        HIVIEW_LOGD("all tid is 0");
        return beginIpc;
    }
    // get IpcTrans of max wait time
    auto iter = timeIpc.rbegin();
    return iter->second;
}

// situation 2: there are IpcTrans list in IpcTrans transactions
void LogUtil::ParseIpcList(const string& clientPidTid, vector<IpcTrans>& IpcNode) const
{
    int times = 0;
    // loop lookup Ipclist
    auto iterInfo = IpcInfo_.find(clientPidTid);
    while (iterInfo != IpcInfo_.end() && times++ < 20) { // mostly 20 list
        string serverPidTid = iterInfo->second.first;
        string serverTid = StringUtil::GetRightSubstr(serverPidTid, ":");
        if (serverTid == "0") {
            IpcNode.push_back(iterInfo->second.second); // push IpcItem which tid is 0
            IpcNode.push_back(ParseZeroIpc(iterInfo->second.second)); // (Ipc exhaust) show maxwait time
            break;
        }
        IpcNode.push_back(iterInfo->second.second);
        iterInfo = IpcInfo_.find(serverPidTid);
    }
}

/* GetTrace function:
 * buffer : log buffer
 * cursor : buffer seekg
 * reg : regex which is used to get trace line
 * result : all trace line will be spliced by "\n"
 * startReg : start place when regex is match, default empty string
 */
void LogUtil::GetTrace(stringstream& buffer, int cursor, const string& reg, string& result, string startReg)
{
    buffer.seekg(cursor, ios::beg);
    string line;
    bool start = false;
    int num = 0;
    int skipNum = 0;
    startReg = startReg.empty() ? reg : startReg;
    while (getline(buffer, line) && num++ < TOTAL_LINE_NUM) {
        if (line.length() > BUF_LEN_2048) {
            continue;
        }
        if (line.size() == 0 || skipNum >= TOTAL_SKIP_NUM) {
            break; // blank line
        }
        if (!start) {
            start = regex_search(line, regex(startReg));
            if (!start) {
                continue;
            }
        }

        if (regex_search(line, regex(reg))) {
            skipNum = 0;
        } else {
            skipNum++;
            continue;
        }
        result += line + LogUtil::SPLIT_PATTERN;
    }
}

bool LogUtil::ReadFileBuff(const string& file, stringstream& buffer)
{
    int fd = LogUtil::GetFileFd(file);
    if (fd < 0) {
        HIVIEW_LOGE("%{public}s get fd fail, fd is %{public}d.", file.c_str(), fd);
        return false;
    }

    std::string content;
    if (!FileUtil::LoadStringFromFd(fd, content)) {
        HIVIEW_LOGE("read file: %s failed, fd is %d\n", file.c_str(), fd);
        close(fd);
        return false;
    }
    buffer.str(content);
    close(fd);
    return true;
}

int LogUtil::GetFileFd(const string& file)
{
    if (file.empty() || !FileUtil::IsLegalPath(file)) {
        HIVIEW_LOGE("the system file (%{public}s) is illegal.", file.c_str());
        return -1;
    }
    std::string realFileName;
    if (!FileUtil::PathToRealPath(file, realFileName) || realFileName.empty() ||
        !FileUtil::FileExists(realFileName)) {
        HIVIEW_LOGE("the system file (%{public}s) is not found.", realFileName.c_str());
        return -1;
    }
    return open(realFileName.c_str(), O_RDONLY);
}

bool LogUtil::FileExist(const string& file)
{
    return FileUtil::FileExists(file);
}

bool LogUtil::IsTestModel(const string& sourceFile, const string& name,
    const string& pattern, string& desPath)
{
    return false;
}
} // namespace HiviewDFX
} // namespace OHOS