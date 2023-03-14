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

#ifndef FREEZE_VENDOR_H
#define FREEZE_VENDOR_H

#include <set>
#include <string>
#include <vector>

#include "faultlog_info.h"
#include "log_store_ex.h"
#include "pipeline.h"
#include "resolver.h"
#include "smart_parser.h"
#include "singleton.h"
#include "watch_point.h"

namespace OHOS {
namespace HiviewDFX {
class Vendor : public Singleton<Vendor> {
    DECLARE_SINGLETON(Vendor);

public:
    bool Init();
    bool IsFreezeEvent(const std::string& domain, const std::string& stringId) const;
    bool IsApplicationEvent(const std::string& domain, const std::string& stringId) const;
    bool IsBetaVersion() const;
    std::set<std::string> GetFreezeStringIds() const;
    std::string GetTimeString(unsigned long long timestamp) const;
    void DumpEventInfo(std::ostringstream& oss, const std::string& header, const WatchPoint& watchPoint) const;
    bool CheckPid(const WatchPoint &watchPoint, std::list<WatchPoint>& list) const;
    std::string MergeEventLog(
        const WatchPoint &watchPoint, std::list<WatchPoint>& list,
        const FreezeResult& result, std::string& digest) const;
    std::shared_ptr<PipelineEvent> MakeEvent(
        const WatchPoint &watchPoint, const WatchPoint& matchedWatchPoint,
        const std::list<WatchPoint>& list, const FreezeResult& result,
        const std::string& logPath, const std::string& digest) const;
    bool ReduceRelevanceEvents(std::list<WatchPoint>& list, const FreezeResult& result) const;

private:
    static const int MAX_LINE_NUM = 100;
    static const int SYSTEM_RESULT_ID = 1;
    static const int APPLICATION_RESULT_ID = 0;
    static const int TIME_STRING_LEN = 16;
    static const int MAX_FILE_NUM = 500;
    static const int MAX_FOLDER_SIZE = 50 * 1024 * 1024;
    static const inline std::string TRIGGER_HEADER = ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
    static const inline std::string HEADER = "*******************************************";
    static const inline std::string HYPHEN = "-";
    static const inline std::string NEW_LINE = "\n";
    static const inline std::string EVENT_SUMMARY = "SUMMARY";
    static const inline std::string POSTFIX = ".tmp";
    static const inline std::string APPFREEZE = "appfreeze";
    static const inline std::string SYSFREEZE = "sysfreeze";
    static const inline std::string SP_SYSTEMHUNGFAULT = "SystemHungFault";
    static const inline std::string SP_APPFREEZE = "AppFreeze";
    static const inline std::string SP_ENDSTACK = "END_STACK";
    static const inline std::string FREEZE_DETECTOR_PATH = "/data/log/faultlog/";
    static const inline std::string FAULT_LOGGER_PATH = "/data/log/faultlog/faultlogger/";
    static const inline std::string SMART_PARSER_PATH = "/system/etc/hiview/";

    bool GetMatchString(const std::string& src, std::string& dst, const std::string& pattern) const;
    bool IsSystemEvent(const std::string& domain, const std::string& stringId) const;
    bool IsSystemResult(const FreezeResult& result) const;
    bool IsApplicationResult(const FreezeResult& result) const;

    static const std::vector<std::pair<std::string, std::string>> applicationPairs_;
    static const std::vector<std::pair<std::string, std::string>> systemPairs_;
    std::unique_ptr<LogStoreEx> logStore_ = nullptr;
};
}  // namespace HiviewDFX
}  // namespace OHOS
#endif // FREEZE_VENDOR_H
