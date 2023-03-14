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
#ifndef LOG_UTIL_H
#define LOG_UTIL_H

#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace OHOS {
namespace HiviewDFX {
struct IpcItem {
    std::string pidTid;
    std::string packageName;
    std::string threadName;
};

struct IpcTrans {
    double waitTime;
    std::string code;
    std::string line;
    IpcItem cIpcItem;
    IpcItem sIpcItem;
};

class LogUtil {
public:
    LogUtil() {};
    ~LogUtil() {};
    LogUtil(const LogUtil&) = delete;
    LogUtil& operator=(const LogUtil&) = delete;
    std::tuple<std::pair<int, int>, std::vector<std::string>> ParseIpcInfo(
        std::stringstream& buffer, const int offset, const std::pair<int, int>& pidTid);
    static void GetTrace(std::stringstream& buffer, int cursor, const std::string& reg, std::string& result,
        std::string startReg = "");
    static bool ReadFileBuff(const std::string& file, std::stringstream& buffer);
    static bool IsTestModel(const std::string& sourceFile, const std::string& name,
        const std::string& pattern, std::string& desPath);
    static bool FileExist(const std::string& file);

public:
    static const std::string SPLIT_PATTERN;
    static const std::string SMART_PARSER_TEST_DIR;
    static const int TOTAL_LINE_NUM;


private:
    void ReadIpcInfo(std::stringstream& buffer, const int offset);
    IpcItem ParseIpcStr(std::string IpcStr) const;
    void GetIpcInfo(const std::string& line);
    IpcTrans ParseZeroIpc(const IpcTrans& beginIpc) const;
    void ParseIpcList(const std::string& clientPidTid, std::vector<IpcTrans>& IpcNode) const;
    static int GetFileFd(const std::string& file);

private:
    std::map<std::string, std::pair<std::string, IpcTrans>> IpcInfo_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif /* LOG_UTIL_H */