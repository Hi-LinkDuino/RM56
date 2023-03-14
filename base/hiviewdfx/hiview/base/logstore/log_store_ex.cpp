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
#include "log_store_ex.h"

#include <fstream>
#include <functional>
#include <string>
#include <vector>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "file_util.h"
#include "log_file.h"
#include "logger.h"

namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("HiView-LogStoreEx");
constexpr int32_t DEFAULT_LOGSTORE_SIZE = 1024 * 1024 * 10;
constexpr int32_t DEFAULT_LOGSTORE_MIN_KEEP_FILE_COUNT = 100;
constexpr mode_t DEFAULT_LOG_FILE_MODE = 0664;
constexpr mode_t DEFAULT_LOG_DIR_MODE = 0770;
LogStoreEx::LogStoreEx(const std::string& path, bool autoDeleteFiles)
    : autoDeleteFiles_(autoDeleteFiles),
      maxSize_(DEFAULT_LOGSTORE_SIZE),
      minKeepingNumberOfFiles_(DEFAULT_LOGSTORE_MIN_KEEP_FILE_COUNT),
      comparator_(nullptr),
      path_(path)
{}

bool LogStoreEx::Init()
{
    if (!FileUtil::FileExists(path_)) {
        FileUtil::ForceCreateDirectory(path_);
        FileUtil::ChangeModeDirectory(path_, DEFAULT_LOG_DIR_MODE);
    }
    return true;
}

void LogStoreEx::SetLogFileComparator(LogFileComparator comparator)
{
    comparator_ = comparator;
}

void LogStoreEx::SetMaxSize(uint32_t size)
{
    maxSize_ = size;
}

void LogStoreEx::SetMinKeepingFileNumber(uint32_t number)
{
    minKeepingNumberOfFiles_ = number;
}

const std::string& LogStoreEx::GetPath() const
{
    return path_;
}

std::vector<LogFile> LogStoreEx::GetLogFiles()
{
    std::vector<std::string> fileVec;
    std::vector<LogFile> logFileList;
    FileUtil::GetDirFiles(path_, fileVec);
    for (auto& filePath : fileVec) {
        logFileList.push_back(LogFile(filePath));
    }

    if (comparator_ != nullptr) {
        std::sort(logFileList.begin(), logFileList.end(), comparator_);
    } else {
        std::sort(logFileList.begin(), logFileList.end());
    }
    return logFileList;
}

std::vector<LogFile> LogStoreEx::GetLogFiles(LogFileFilter filter)
{
    auto logFileList = GetLogFiles();
    auto iter = logFileList.begin();
    while (iter != logFileList.end()) {
        if (!filter(*iter)) {
            iter = logFileList.erase(iter);
            continue;
        }
        iter++;
    }
    return logFileList;
}

bool LogStoreEx::Clear()
{
    if (!FileUtil::ForceRemoveDirectory(path_)) {
        return false;
    }
    return Init();
}

void LogStoreEx::DoDeleteLogFiles(const std::vector<LogFile> &fileList, int32_t removeFileNums) const
{
    int32_t deleteCount = 0;
    for (auto it = fileList.rbegin(); it != fileList.rend(); ++it) {
        if (deleteCount >= removeFileNums) {
            break;
        }

        FileUtil::RemoveFile(it->path_);
        deleteCount++;
    }
    HIVIEW_LOGI("Remove %d Files.", deleteCount);
}

void LogStoreEx::ClearOldestFilesIfNeeded()
{
    auto size = FileUtil::GetFolderSize(path_);
    if (size < maxSize_) {
        return;
    }

    auto fileList = GetLogFiles();
    int32_t removeFileNumber = fileList.size() - minKeepingNumberOfFiles_;
    if (removeFileNumber < 0) {
        removeFileNumber = fileList.size() / 2; // 2 : remove half of the total
    }
    DoDeleteLogFiles(fileList, removeFileNumber);
}

void LogStoreEx::ClearSameLogFilesIfNeeded(LogFileFilter filter, uint32_t maxCount)
{
    auto fileList = GetLogFiles(filter);
    uint32_t removeFileNumber = 0;
    if (fileList.size() > maxCount) {
        removeFileNumber = fileList.size() - maxCount;
    }
    DoDeleteLogFiles(fileList, removeFileNumber);
}

LogStoreEx::FileHandle LogStoreEx::CreateLogFile(const std::string& name)
{
    if (autoDeleteFiles_) {
        ClearOldestFilesIfNeeded();
    }

    auto path = path_ + "/" + name;
    auto fd = open(path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, DEFAULT_LOG_FILE_MODE);
    if (fd < 0) {
        HIVIEW_LOGI("Fail to create %s.", name.c_str());
    }
    return fd;
}

bool LogStoreEx::RemoveLogFile(const std::string& name)
{
    auto path = path_ + "/" + name;
    std::string realPath;
    if (!FileUtil::PathToRealPath(path, realPath)) {
        return false;
    }
    return FileUtil::RemoveFile(path);
}
} // namespace HiviewDFX
} // namespace OHOS