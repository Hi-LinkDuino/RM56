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
#ifndef LOG_STORE_LOG_STORE_EX_H
#define LOG_STORE_LOG_STORE_EX_H
#include "log_file.h"

#include <cinttypes>
#include <functional>
#include <ostream>
#include <string>
#include <vector>

#include <sys/stat.h>
#include <unistd.h>

namespace OHOS {
namespace HiviewDFX {
class LogStoreEx {
public:
    using LogFileFilter = std::function<bool(const LogFile&)>;
    using LogFileComparator = std::function<bool(const LogFile&, const LogFile&)>;
    using FileHandle = int32_t;

    // managing logs in specific path with following functions
    // 1. get logs in specific order
    // 2. remove old logs in specific rule
    LogStoreEx(const std::string& path, bool autoDeleteFiles = false);
    ~LogStoreEx() {};

    // create path with expected permission
    bool Init();
    void SetLogFileComparator(LogFileComparator comparator);

    // Get all files in log store, sorted by last modify time by default
    std::vector<LogFile> GetLogFiles();
    std::vector<LogFile> GetLogFiles(LogFileFilter filter);

    // Get the path managed by store
    const std::string& GetPath() const;

    // Remove all log files in managed folder
    bool Clear();

    // remove file according to the size and file count quota
    // keep minimum number of file in store
    void ClearOldestFilesIfNeeded();

    // remove filtered files if the number of files exceeds maximum count
    void ClearSameLogFilesIfNeeded(LogFileFilter filter, uint32_t maxCount);

    // create a log file inside store path
    FileHandle CreateLogFile(const std::string& name);
    bool RemoveLogFile(const std::string& name);

    void SetMaxSize(uint32_t size);
    void SetMinKeepingFileNumber(uint32_t number);

protected:
    bool autoDeleteFiles_;
    uint32_t maxSize_;
    uint32_t minKeepingNumberOfFiles_;
    LogFileComparator comparator_;
    std::string path_;

private:
    void DoDeleteLogFiles(const std::vector<LogFile> &fileList, int32_t removeFileNums) const;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // LOG_STORE_LOG_STORE_EX_H
